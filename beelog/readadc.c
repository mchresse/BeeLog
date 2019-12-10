/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
/******************************************************************************* 
 * File:   readadc.c
 * Author: Randolph Esser - Copyright 2017-2019
 * Created on 17. Januar 2017, 10:09
 * This file is part of the "beelog" program/project.
 *
 * Read out ADC of type MCP300x via SPI Ports
 * using WiringPi Lib in GPIO Mode
 *******************************************************************************
 */
/* Global libraries:
 *  This code makes use of WiringPi library in GPIO mode (using orig. GPIO port#)
 * 	 Makes use of WiringPI-library of Gordon Henderson 
 *	 (https://projects.drogon.net/raspberry-pi/wiringpi/)
 *	 Recommended connection (http://www.raspberrypi.org/archives/384):
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sched.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <errno.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>

#include "getini.h"
#include "get1wire.h"
#include "beehive.h"
#include "beelog.h"
#include "readadc.h"

/*******************************************************************************
* Function: adc_init2()
* Creation: 01.01.2019
* 
* Init MCP3002 SPI ports
* 
* Input :
* void		
*
* Global:	
*	-
*
* Output:
*	volts	final voltage as test measurement
*********************************************************************************
*/
int adc_init2(){
	unsigned int	result=0, adctotal=0;
	float			calibrate = 4.3;			//for 12Volt-Battery only (to be changed for other voltage)
	unsigned char	chnum = ADCCH0;
	unsigned char	rep=10, i=1;	// Result is average of #rep masurements
	float volts=0;

#ifdef BHSIM
	volts = 12.00;	// simulate good battery level
	return (volts);
#endif
	
//  Setup SPI interface of ADC
	pinMode(ADCCLK,	OUTPUT);
	pinMode(ADCDOUT,INPUT);
	pinMode(ADCDIN, OUTPUT);
	pinMode(ADCCS,	OUTPUT);

	// start #rep measurements
	for(i=1; i<=rep; i++){
		result	= readadc2(chnum);
		adctotal += result;	// remmeber sum of all measurements
		delay(70);	// wait for stablizing the conversion lines
	}

	result = adctotal/rep;	// calc average result value

	// calibrate result related to external resistor chain
	volts = result*(3.33/1024)*calibrate;	// 12Bit ADC <-> related to max voltage of 3.33V
	
	BHLOG(LOGMON) printf("    ADC_Init2: ADC%d value: %d ->  Battery: %.2fV\n",chnum, result, volts);

	return(volts);
}

/*******************************************************************************
* Function: readadc2()
* Creation: 01.08.2018
* 
* Read ADC MCP3002 -> 2 channel - ADC 12 Bit 
* Input :
* - chnum	select channel of interest: 
*			0	SingleEndedMode/Channel 0
*			1	SingleEndedMode/Channel 1
*
* Global:	
*	gparam		struct of gparam
*
* Output:
*	rc		=0 all done as expected
*			=-1	wrong channel number
*********************************************************************************
*/unsigned int readadc2(unsigned char chnum){
	unsigned char	commandout;
	unsigned char	i;
	unsigned int	adcout=0;
	
	unsigned char	clockpin	= ADCCLK;
	unsigned char	dinpin		= ADCDIN;
	unsigned char	doutpin		= ADCDOUT;
	unsigned char	cspin		= ADCCS;

// Set channel selection command
// 110        110                110
// ^StartBit   ^SingleEndedMode    ^ODD->Channel 0
	if (chnum == 0)
		commandout = 0x6;	// 110 = SingleEndedMode/Channel 0
	else if (chnum == 1)
		commandout = 0x7;	// 111 = SingleEndedMode/Channel 1
	else{
		BHLOG(LOGMON) printf("    ReadADC2: wrong channel selection:%i -> only dual Channel ADCs!\n", chnum);
		return -1;              
	}
	digitalWrite(cspin,HIGH);	 // End a conversion, leave device in low power standby
	digitalWrite(clockpin, LOW); // trigger clock
	digitalWrite(cspin,LOW);	 // Initiate communication window

// start measurement on DIN
	commandout<<=5;
	for(i=1;i<=3;i++){
		if (commandout & 0x80){
			digitalWrite(dinpin,HIGH);
		}else{
			digitalWrite(dinpin,LOW);
		}
		commandout<<=1;
		digitalWrite(clockpin,HIGH);    // set read trigger
		digitalWrite(clockpin,LOW);     
	}

// read in all 12 ADC Bits
	for(i=1;i<=ADCBITS;i++){
		digitalWrite(clockpin,HIGH);    // trigger per bit
		digitalWrite(clockpin,LOW);     
		adcout<<=1;						// prepare serial read in queue for next bit
		if(digitalRead(doutpin)==HIGH){ // Status of next bit 0/1 ?
			adcout|=0x1;				// set Bit == 1
		}	// done
	}
	digitalWrite(cspin,HIGH);			// release chip by CS

	adcout/=2;							// adjust return value to common value range

return adcout;
} // end of readADC2()



/********************************************************************** 
 * Read ADC MCP3008 -> 8 channel - ADC 10 Bit
 *
 * On some Raspberry Pi software distributions, SPI and I2C kernel modules are explicitly set 
 * not to be loaded (blacklisted). We will need to edit the ‘/etc/modprobe.d/raspi-blacklist.conf’ 
 * file to remove these restrictions. 
 * Place a comment (#)  at the front of the lines containing spi-bcm2708 and i2c-bcm2708. A reboot is needed.
 * 	sudo nano /etc/modprobe.d/raspi-blacklist.conf
 * 		#blacklist spi and i2c by default (many users don’t need them)
 * 		#blacklist spi-bcm2708
 * 		#blacklist i2c-bcm2708
 * 	sudo reboot
 *
 * Requires: wiringPi (http://wiringpi.com)
 * Copyright (c) 2015 http://shaunsbennett.com/piblog
 ***********************************************************************
*/
/*******************************************************************************
* Function: adc_init8()
* Creation: 01.08.2018
* 
* Read ADC MCP3008 -> 8 channel - ADC 10 Bit
*
*  The formula to convert analog to digital is: 
*	1024 (10 bit) * analog voltage in (VIN) / reference voltage (VREF).
*	Example: 2.5VDC analog in @ 5VDC VREF   = (1024*2.5)/5 = 512.
*	Example: 2.5VDC analog in @ 3.3VDC VREF = (1024*2.5)/3.3 = 775.
*
* Input :
* spiload =1	load SPI driver,  default: 0 = do not load
* spichannel	SPI-analogChannel 0 or 1, default:  0
*
* Global:	
* myADCFd	global ADC chip file descriptor
*
* Output:
*	rc		=0 init done as expected
*			=-1	SPI driver load failed
*			=-2 SPI bus not reached
*********************************************************************************
*/
static int myADCFd ;

int adc_init8( int spiload, int spichannel){
	if(spiload == 1){
		if (system("gpio load spi") == -1){
			fprintf (stderr, "    ADCInit8: Can't load the SPI driver: %s\n", strerror (errno)) ;
		//	exit (EXIT_FAILURE) ;
			return(-1);
		}
	}	
	// wiringPiSetup only once in main()
	//   wiringPiSetupGPIO () ; => using GPIO# enumeration

	// check for conflict with ePaper at SPI-0 => t.b.d.
	if((myADCFd = wiringPiSPISetup(spichannel, 1000000)) < 0){
		fprintf (stderr, "    ADCInit8: Can't open the SPI bus: %s\n", strerror (errno)) ;
			return(-2);
	}
 return 0;
}


/*******************************************************************************
* Function: myAnalogRead8()
* Creation: 01.08.2018
* 
* Read ADC MCP3008 -> 8 channel - ADC 10 Bit
*
*  The formula to convert analog to digital is: 
*	1024 (10 bit) * analog voltage in (VIN) / reference voltage (VREF).
*	Example: 2.5VDC analog in @ 5VDC VREF   = (1024*2.5)/5 = 512.
*	Example: 2.5VDC analog in @ 3.3VDC VREF = (1024*2.5)/3.3 = 775.
*
* Input :
* spichannel	SPI-analogChannel 0 or 1, default:  0
* channelconfig	define channel conv mode:
*				CHAN_CONFIG_SINGLE or CHAN_CONFIG_DIFF
* analogchannel	select analog channel 0..7
*
* Global:	
* myADCFd	global ADC chip file descriptor
*
* Output:
*	rc		=0 init done as expected
*			=-1	SPI driver load failed
*			=-2 SPI bus not reached
*********************************************************************************
*/
int myAnalogRead8(int spichannel, int channelconfig, int analogchannel){
 unsigned char buffer[3] = {1}; // start bit

	buffer[1] = (channelconfig + analogchannel) << 4;
	wiringPiSPIDataRW(spichannel, buffer, 3);

 return(((buffer[1] & 3 ) << 8 ) + buffer[2]);		// get last 10 bits
}
	
// ReadADC8(): For MCP3008: 
// channel		=0 all 8 channels, >0 - 8 return dedicated channel value
// spichannel	SPI-analogChannel 0 or 1, default:  0
// chmode		= CHAN_CONFIG_DIFF		-> differential channel input, 
//				= CHAN_CONFIG_SINGLE	-> default: single ended
//
int readadc8( int channel, int spichannel, int chmode){  
	int channelconfig;
	int i;
	int value;

	if(channel < 0 || channel > 8){
		printf("    ReadADC8: wrong channel number %d\n", channel);
		return -1;	// unsupported channel number
	}

	if(channel>0){ // read out dedicated channel number
		printf("    MCP3008(CE%d,%s): analogChannel %d = %d\n",
			spichannel,
			(chmode==CHAN_CONFIG_SINGLE)?"single-ended":"differential",
			channel, 
			value = myAnalogRead8(spichannel, chmode, channel-1));
			BHLOG(LOGMON) printf("    ReadADC8: ADC%d value: %d\n",channel, value);
			close(myADCFd);
			return(value);
	}else{ // read them all => just for test purpose: print out only !
		// t.b.d. requires return array for all values
		for(i=0; i<8; i++){
			printf("    MCP3008(CE%d,%s): analogChannel %d = %d\n",
				spichannel,
				(chmode==CHAN_CONFIG_SINGLE)?"single-ended":"differential",
				i+1,
				value = myAnalogRead8(spichannel,chmode,i));
				BHLOG(LOGMON) printf("    ReadADC8: ADC%d value: %d\n",channel, value);
		}
	}
	close(myADCFd);
	return(value);
}
