/*******************************************************************************
 Name        : HX711.c
 Author		 : Randolph Esser
 Version     : 1.0

 Description :
     A simple HX711 test tool for Raspberry Pi for displaying ADC Data

	HX JP2 pins | Raspberry Pin | wiringPi
     ------------|---------------|---------
     HX 1 - GND  |  P39 - GND    |
	 HX 2 - DT   |  P16 - GPIO23 | -> 4
	 HX 3 - SCK  |  P18 - GPIO24 | -> 5
	 HX 4 - Vcc  |  P02 - 5V     |
	 
     See also Definition in config.ini !
	
	verbose mode:
	0 = silent mode -> no prints at all
	1 = HX711 custom result print and urgent errors -> expertmode
	2 = WiringPi Debug mode
	3 = HX711 Debug Mode
 *******************************************************************************
 */
/*
 * This file contains code use of:
 * hx711/hx711.c from https://github.com/ggurov/hx711/blob/master/hx711.c
 * 6d0a8a1 on Nov 8 2016 
 * 224 lines (168 sloc) 3.62 KB 
 *
 * Header:
 * gurov was here, use this code, or don't, whatever, I don't care. 
 * If you see a giant bug with a billion legs, please let me know so it can be squashed
 *
 */
/* Use of Global libraries:
 *  WiringPi library is used in GPIO mode (using orig. GPIO port index
 * 	 Makes use of WiringPI-library of Gordon Henderson 
 *	 (https://projects.drogon.net/raspberry-pi/wiringpi/)
 *	 Recommended connection (http://www.raspberrypi.org/archives/384):
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <wiringPi.h>
#include <math.h>

#include "hx711.h"
#include "get1wire.h"
#include "isr.h"
#include "getini.h"
#include "beehive.h"
#include "beelog.h"

long	samples[MAX_N_SAMPLES];	// global field of measured data per loop
 
/*******************************************************************************
* Function: inithx711()
* Creation: 22.01.2017
* 
* Initialize GPIO ports, TaraRes-Button and ADC-HX711 GAIN for Weight 
* Measurement. ADC reset sequence send out.
* -> to be called only once
*
* Input :
*	verbose		=0..n			log/debug level for console trace output
*   hxidx		=1..n			Index of HX711 module to be initialized
*	channel		0,1,2			Select HX711 channel and gain value
*								 r = 0 - 128 gain ch A
*								 r = 1 - 32  gain ch B
*								 r = 2 - 63  gain ch A
* Output:
*	returncode	= EXIT_SUCCESS	all init routines finished successful
*
* Global:
*********************************************************************************
*/
int inithx711(int hxidx, int channel){
 int i; 	
	if(hxidx >= 2) // positive shortcut: HX711 module 2 not connected
		return(EXIT_SUCCESS);

	// init HX711
	BHLOG(LOGHX1) printf("    HX: Init HX711 Modul #%i ...\n", hxidx);
	setHighPri();

	// preset GPIO line status of HX711 module & TestLED
	setup_gpio();		// define input and output lines + pullup state

	BHLOG(LOGHX1) printf("    HX: Init Tara Button -> register ISR routine\n");
	init_tara_button();	// init  tara button ISR

	reset_converter();		// reset HX module to channel A gain: 128
	
	if(channel > 2 || channel <0 ) channel = 0;	// just to save the module: limit channel selector
	// Select HX711 channel and gain value
	//   r = 0 - 128 gain ch A
	//   r = 1 - 32  gain ch B
	//   r = 2 - 63  gain ch A
	set_HX711gain(channel);		// -> select channel for next read by 1 dummy read

	for (i=0; i < MAX_N_SAMPLES; i++){ // preset result data fields
		samples[i]=0;
	}
	return (EXIT_SUCCESS);
} // end of inithx711.c



/*******************************************************************************
* Function: gethx711()
* Creation: 22.01.2017 by Randolph Esser
* 
* Reset ADC register for next measurement.
* For nsample times: Read current weight from ADC port A+/A- via GPIO
* During ADC-Read TESLED is set ON.
* Simple Average (All weight values devided by nsamples) is base for +/- 5%
* spread range to kick out all measures outside this filter window.
* -> calculate next average2
* Finally average2 weight is multiplied by temp. compensation factor (1 +/- x)
* and devided by 1KG-Reference value to get final data in KG. 
* 
* If ISR for TARA Reset was detected -> preset offset to current average2 value.
* (This this now the new offset during remaining runtime, but for the next 
*  measurement)
* 
* Input :
*	customoffset	custom offset from beehive call -> not used 
*	exttemp			temperature of external temp. sensor for calculation of 
*					weight temperature compensation
*	hxidx	0..n	index of HX Module
*	channel	0,1,2	Select HX711 channel and gain value
*							 r = 0 - 128 gain ch A
*							 r = 1 - 32  gain ch B
*							 r = 2 - 63  gain ch A
* Output:
*	weight		0 .. 999,999	weight in kg
*
* Global:
*   offset			weight offset value as reference to 0kg (in hx711.h)
*	struct cfgini	contains runtime preset values	(in beehive.h)
*		-> hxnsamples
*		-> hxspread
*		-> lediopin
*		-> hxrefkg
*		-> hxtempcomp
*		-> hxtaralock
*********************************************************************************
*/
float gethx711(long customoffset, float exttemp, int hxidx, int channel){
  int	i, j, b;
  long	tmp_avg=0;
  int	nsamples;		// number of measurements per 1 loop
  float	weight;			// final weight of hive
  float	tempcomp;		// Temp. compensation referenced calibration temp - ext. temperature
  char	sbuf[1024];

 
	if (hxidx == 2)		// global offset and cfgini->hxtempcomp must be duplicated !!!
		return(0);		// by now shortcut: HX#2 not connected
		
	BHLOG(LOGHX1) 	printf("    HX: Start collecting weight data of HX#%i channel %d \n", hxidx, channel);
	
	reset_converter();	// after reset channel 0 is selected by default
	if(channel > 2 || channel <0 ) channel = 0;	// just to save the module: limit channel selector
	
	if(channel > 0)		// after reset channel 0 is selected by default -> skip channel change
		set_HX711gain(channel);		// -> select other channel =1: B(32) or =2: A(64)

	nsamples = cfgini->hxnsamples;	// get number of reads per burst
 
#ifndef BHSIM
	digitalWrite(cfgini->hc_lediopin, TESTLED_ON);	
#endif
#ifdef BHSIM
	samples[0]=2886850;			// simulate  any value table
	samples[1]=2886860;
	samples[2]=2886750;
	samples[3]=2886793;
	samples[4]=2886833;
	samples[5]=2886921;
	samples[6]=157;				// simulate a wrong measurement
	samples[7]=2886888;
	samples[8]=2886852;
	samples[9]=2886851;
	samples[10]=2886799;
#endif
	// get the raw samples and average them	
	for(i=0;i<nsamples;i++) {

#ifndef BHSIM
		samples[i] = read_cnt(channel);		// read data raw from ADC (no offset)
#endif
		BHLOG(LOGHX1){
			printf("    HX: ");
			for (j=31;j>=0;j--) {
				printf("%d ", (samples[i] & ( 1 << j )) != 0 );	
			}
			printf(" #: %d -> %3.4f kg\n", samples[i], ((float) samples[i] - customoffset) / (float) cfgini->hxrefkg ); 	 
		}
	}
#ifndef BHSIM
	reset_converter();
	digitalWrite(cfgini->hc_lediopin, TESTLED_OFF);	
#endif
	
	// All "nsamples" raw reads done -> calculate average value
	tmp_avg = getaverage(nsamples);
	// recalculate tempcomp here: 1 +/- compensation	
	tempcomp = gettcomp(exttemp);

	// calculate final weight value with all compensations
	weight = ((float) (tmp_avg - customoffset) / (float) cfgini->hxrefkg) * (float) tempcomp;

	BHLOG(LOGHX1) printf("    HX: After %i counts: average: %d - offset: %d  =>  %3.0f : %3.3fkg (* tempcomp: %3.3f), \n", 
					i, tmp_avg, customoffset, (float) tmp_avg- customoffset, weight/tempcomp, tempcomp );


	if(reset_tara == 1 || cfgini->hxtarareset == 1){ // reset button was pressed, and lock is disabled
		if(cfgini->hxtaralock == 1){
			// reset button was pressed or simulated, and lock is disabled
			offset = tmp_avg;				// preset global offset 
			reset_tara = 0;
			BHLOG(LOGBH) printf("\n    HX: Reset TARA to %d\n", offset);
			sprintf(sbuf, "TARA Button pressed, new offset = %d", offset);  
			beelog(sbuf);

			sprintf(csv_notice, "TARA Reset");
			addwebnotice( "Gewicht1", csv_notice, &bhdb);
		}else{
			BHLOG(LOGBH) printf("\n    HX: Reset TARA locked, offset unchanged = %d\n", offset);
			beelog("TARA Button pressed, but locked, offset unchanged");
			reset_tara = 0;
		}		
	}
	
	return (weight);
} // end of hx711.c



/*******************************************************************************
* Set of helper functions:
*
* gettcomp()        calculate temperature compensation factor: 1,xxx
* getaverage()		calculate average weight value from samples[] field
* setHighPrio()		set current process to higher prio to assure real time values
* setup_gpio()		preset GPIO IO pins of HX711 API to IN/OUT
* unpull_pins()		define pullup status of each used GPIO pin -> not used here
* reset_converter() prepares ADC HX711 for next measurement on port A
* set_HX711gain()	set ADC HX711 GAIN factor for port A
* read_cnt()		final read function of weight scal data via ADC HX711 port A
********************************************************************************
*/
/******************************************************************************
* function: gettcomp()
* Author:	Randolph Esser, 7.1.2017
* Decription:
* Calibrate temperature compensation of weight cell by ord. 4 term calculation
* Input: exttemp	raw temp. value
* output:tc			calibrate temp. correction 
******************************************************************************/
float gettcomp(float exttemp){

float tcomp;
float tc;	// resulting calibration factor
float fakx0 = 0;
float fakx1 = -0.0003;
float fakx2 = 0;
float fakx3 = 0;
float fakx4 = 0;

	tc  = fakx0+ fakx1 * powf(exttemp,1)+ fakx2 * powf(exttemp,2) + fakx3 * powf(exttemp,3) + fakx4 * powf(exttemp,4);

  	// Delta between ext. and Cal.-Temp by Weight/temp. comp. factor 
	tcomp = 1 - (cfgini->hxtempcomp * tc);
	BHLOG(LOGHX1) {
		printf("    HX: exttemp: %3.3f, tc factor: %3.3f, tcomp: %3.3f\n", exttemp, tc, tcomp);
	}
	
	return(tcomp);
}

/******************************************************************************
* function: getaverage()
* Author:	Randolph Esser, 7.1.2017
* Decription:
* Check if measured values are in given trust range (% value from config.ini)
* calculate average weight value from samples[] field by bubble sort and devided
* by # of samples
* If no reliable value inside trust range found -> return 0
* Input: nsamples	number of read samples
*		 samples[]	list of measured values (global)
* output: value		evaluated weight value 
******************************************************************************/
long getaverage(int nsamples){
  float	tmp_avg=0;		// simple average devided by nsamples
  float	tmp_avga=0;		// average within +-5% spread window ascending tab
  float	tmp_avgd=0;		// average within +-5% spread window descending tab
  int	i,j;
  int	na, nd;			// counter for ascending or descending table
 
  float	filter_low, filter_high;	// reliability range values of measurements
  float spread_percent; // reliability range in %/2

	// Sorting sample table in ascending order
	bubble_sort(samples, nsamples);
  

	// Get average of table in ascending order
	tmp_avg = 0;
	for(i=0;i<nsamples-2;i++) {				// allow max. 2 wrong measurements
		tmp_avg += (float)samples[nsamples-i-1];		// sum up all measurements from top up
	}
	tmp_avg = tmp_avg / (i);		//now we have a first guess of an average value

	// Second: recalculate average by trust range +/- spread_percent
	spread_percent = cfgini->hxspread / 100.0 /2.0;	// reliability range in % *2
	filter_low  = tmp_avg * (1.0 - spread_percent);
	filter_high = tmp_avg * (1.0 + spread_percent);

	na=0; tmp_avga = 0;
	for(i=0; i<nsamples; i++) {
	BHLOG(LOGHX2) printf("    HX: low: %3.0f < %3.0f < high: %3.0f\n", filter_low, (float)samples[i], filter_high);
		if((filter_low < (float)samples[i]) && 
		  ((float)samples[i] < filter_high)){			// count only samples in range
				tmp_avga += (float)samples[i];
				na++;	// count the good ones only for ascending order
		}
	}
	BHLOG(LOGHX1) {
		printf("    HX: Ascending filter: low %3.0f  high %3.0f -> average (%2.2f %%)(%i/%i samples):", 
								filter_low, filter_high, spread_percent*100, na, i);
		if(na>0)
			printf("%3.0f\n", tmp_avga/na);
		else
			printf("0\n");		
	}

	// Get average of table in descending order
	tmp_avg = 0;
	for(i=0;i<nsamples-2;i++) {		// allow max. 2 wrong measurements
		tmp_avg += (float)samples[i];		// sum up all measurements from top up
	}
	tmp_avg = tmp_avg / i;			//now we have a first guess of an average value

	// Second: recalculate average by trust range +/- spread_percent
	spread_percent = cfgini->hxspread / 100.0 /2.0;	// reliability range in % *2
	filter_low  = tmp_avg * (1.0 - spread_percent);
	filter_high = tmp_avg * (1.0 + spread_percent);

	nd=0; tmp_avgd = 0;
	for(i=0; i<nsamples; i++) {
		BHLOG(LOGHX2) printf("    HX: low: %3.0f < %3.0f < high: %3.0f\n", filter_low, (float)samples[i], filter_high);
		if((filter_low < (float)samples[i]) && 
		  ((float)samples[i] < filter_high)){			// count only samples in range
				tmp_avgd += (float)samples[i];
				nd++;	// count the good ones only for ascending order
		}
	}
	BHLOG(LOGHX1) {
		printf("    HX: Descending filter: low %3.0f  high %3.0f -> average (%2.2f %%)(%i/%i samples):", 
								filter_low, filter_high, spread_percent*100, nd, i);
		if(nd>0)
			printf("%3.0f\n", tmp_avgd/nd);
		else
			printf("0\n");		
	}
		
	if((na+nd) ==0)
		return(0);	// no good value found at all -> make it clear with zero.

	// If values inside trust range detected -> take new/better average
	if(na <= nd || nd == 0){	// has ascending tab better results ?
		tmp_avg = tmp_avga / na; 	
		return((long)tmp_avg);	// at least one good value found
	}
	if(na > nd || na == 0){	// has descending tab better results ?
		tmp_avg = tmp_avgd / nd; 	
		return((long)tmp_avg);	// at least one good value found
	}
	
  return(0);	// at least no good value found
}





void setHighPri (void)
{
  struct sched_param sched ;

  memset (&sched, 0, sizeof(sched)) ;

  sched.sched_priority = 10 ;
  if (sched_setscheduler (0, SCHED_FIFO, &sched))
    printf ("    HX: Warning: Unable to set high priority\n") ;
}

// preset GPIO line status of HX711 module & TestLED
void setup_gpio() {
#ifndef BHSIM
	pinMode(cfgini->hc_hxdatapin, INPUT);
	pinMode(cfgini->hc_hxclkpin, INPUT);
	pinMode(cfgini->hc_hxclkpin, OUTPUT);

    SCK_OFF;
#endif
}

void 	unpull_pins() {
#ifndef BHSIM
// t.b.d. just to be sure about PIN behaviour
//
//   GPIO_PULL = 0;
//   short_wait();
//   GPIO_PULLCLK0 = 1 << DATA_PIN;
//   short_wait();
//   GPIO_PULL = 0;
//   GPIO_PULLCLK0 = 0;
#endif
} // unpull_pins


void reset_converter(void) {
#ifndef BHSIM
    SCK_ON;			
	usleep(60);		// SCK =1 for >60usec will Powerdown HX711
	SCK_OFF;
	usleep(60);		// SCK = 0 again ->> reset HX711 to normal mode
					// -> default channel A gain 128 
#endif
}

void set_HX711gain(int r) {
	// r = 0 - 128 gain ch a	-> 24+0 clocks
	// r = 1 - 32  gain ch b	-> 24+1 clocks
	// r = 2 - 63  gain ch a	-> 24+2 clocks
	int i;

#ifndef BHSIM
	while( DT_R );	// wait till sensor is ready

	for (i=0;i<24+r;i++) { // make a dummy read for channel selection
		SCK_ON;
		SCK_OFF;
	}
#endif
}

// read out 1-wire sensor data according to 1-wire protocol
// Be aware: works only for 1-wire sensor connected => no sensor selection code yet
unsigned long read_cnt(int channel) {
	long count;
	int i=0;
	int b=0;

	count = 0;		// reset bit field for read data

#ifndef BHSIM
  while( DT_R );	// wait for DOUT 1 -> 0
	b++;			// => wait >= 0,1us
	b++;
	b++;
	b++;

  for(i=0;i<24	; i++) { // create 24 pulse <= 50us
	SCK_ON;
    count = count << 1;	 // shift bit file pointer to next bit
	b++;
	b++;
	b++;
	b++;
    SCK_OFF;
	b++;
	b++;
    if (DT_R > 0 ) { count++; }	// store read bit 0/1
//	b++;				
//	b++;
  } // end of read pulse burst: LSB is the last one

  for(i=0; i <= channel; i++){ // create channel selection pulses
	SCK_ON;				// 25. pulse -> set channel A(128)
	b++;				// 26. pulse -> set channel B(32)
	b++;				// 27. pulse -> set channel A(64)
	b++;
	b++;
	SCK_OFF;
	b++;
	b++;
	b++;
	b++;
  }
	
	//  count = ~0x1800000 & count;
	//  count = ~0x800000 & count;

	 if (count & 0x800000) {	// mask data field
		count |= (long) ~0xffffff;
	 }
#endif
  return (count);	// return final bit field as data
}

void bubble_sort(long list[], int n)
{
  float t;
  int  c;
  int  d;
  
  for (c = 0 ; c < n - 1; c++){
    for (d = 0 ; d < n - c - 1; d++){
      if ((float)list[d] > (float)list[d+1]){
        /* Swapping */
        t         = (float)list[d];
        list[d]   = list[d+1];
        list[d+1] = (long)t;
      }
    }
  }
}
// end of hx711.c