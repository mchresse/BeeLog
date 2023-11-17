/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/******************************************************************************* 
 * File:   main.c
 * Author: Randolph Esser - Copyright 2017-2019
 * Created on 17. Januar 2017, 10:09
 * This file is part of the "beelog" program/project.
 *
 * BeeHive weight & temperature logging tool
 * call: beehive <custom-offset>
 *		<custom-offset> replace default offset from config.ini
 *
 * Description:
 *	Monitoring of 1 or more beehives by weight and temperature. Further sensore
 *  are planned. result of sensor data is stored in different csv formatted files.
 *  Log files are maintained and cyclic ftp transfer of resulting files to remote 
 *  destination is supported. All runtime settings can customised by a *.ini file
 *  which is interpreted once at start and after each measurement loop for online
 *  control of runtime behaviour.
 *
 *  Supported/used sensor protocols: 1-wire, I2c and SPI and GPIO-direct
 *  Supported sensors: DS18B20		for temperature by 1-1ire protocol
 *                     Bosche H40A	weight cell via HX711 A/D converter by SPI
 *                     ADS1115      A/D conversin of all power source lines
 *                                  as base for a battery management control srv.
 *                     epaper 2in7  as local display via I2C
 * Actions of main loop:
 *  Step0: get runtime parameters from local config.ini file (local) at bin-path.
 *  Step1: initialize all Wiring-Pi, GPIO and ADC devices
 *  Step2: Identify and read out all 1-wire sensor devices
 *  Step3: Get current weight data (inkl. temp compensation and offset calibration)
 * 
 * ISR: If Tara Reset button was pressed: reset offset to current weight value -> 0kg
 *      Blocked when TARARESLOCK = 0 (config.ini)
 * 
 * All results are stored in beelog.log and beelog.csv data file per default
 * (as defined in config.ini)
 *******************************************************************************
 */

/* Global libraries:
 *  This code makes use of WiringPi library in GPIO mode (using orig. GPIO port#)
 * 	 Makes use of WiringPI-library of Gordon Henderson 
 *	 (https://projects.drogon.net/raspberry-pi/wiringpi/)
 *	 Recommended connection (http://www.raspberrypi.org/archives/384):
 */
/*
 *  Curl-lib	for cyclic ftp transfer
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <curl/curl.h>
#include <sched.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>

#include <wiringPi.h>

#include "getini.h"
#include "get1wire.h"
#include "hx711.h"
#include "curlftp.h"
#include "beehive.h"
#include "beelog.h"
#include "readadc.h"

#include "epaper/EPD_Test.h"
#include "epaper/EPD_2in7.h"
#include "epaper/fonts.h"
#include "epaper/EPD_2in7.h"
#include "epaper/GUI_Paint.h"
#include "epaper/GUI_BMPfile.h"

#include "raspimon.h"
#include "adslib.h"

/******************************************************************************
 * global data fields
 ******************************************************************************/

struct timeval	MyTime;		// contains time snap in seconds-usecs to epoche time

mondat * dmon;				// data collection of RPi runtime Parameters -> raspimion.c
extern int	w1_devidx;		// counts # of detected W1 slave devices 1...W1_MAX_SLAVES


/*******************************************************************************
 * MAIN ()
 ******************************************************************************/
int main(int argc, char** argv) {
  int	rc, i;			// return code & loop counter
  int	newstart=0;		// semaphor for restart actions
  float	tempext=0;		// buffer for temp. & weight measurements
  float tempintern =0;
  float temphive1 = 0;
  float temphive2 = 0;
  float weight1 = 0;
  float weight2 = 0;
  float battlevel = 0;  // battery volt level
  int	verbose;		// providex verbose mode for logging on console
  char	sbuf[1024];		// universals string buffer for log line creation
  int	didx=0;			// curr. data row index into bhdb (runtime DB)
  char	TimeString[128]; // timestamp of start of current measurement
  struct tm * tinfo;
  char * notice;		 // notice buffer for logfile and csv comment fields
  unsigned char* frame_buffer;	// ptr. to Image buffer of E-Paper devcie
  w1_values *	 w1_devices;	// ptr. to 1-wire sensor data field
								// w1_devidx is defined global
  
// Start of main()
	setbuf(stdout, NULL);		// disable buffering on stdout

	printf("\n");
	printf(" BeeLogger tool - by R.Esser 15.01.2019 (using wiringPi_GPIO#)\n");

	// initialize all IO modules + read of config.ini runtime datparameters
	// -> result: all found 1-wire devices
	w1_devices	= initall(1, 0);	// expecting one HX711 Module; only Port A is connected

	// any cmdline parameter ? -> customer offset value
	if(argc == 2){
		offset=atol(argv[2]);	// use of custom offset
	}else{
		offset=cfgini->hxtara;	// use default TARA
	}
	sprintf(sbuf, " - HX711: used offset: %d", offset);
	beelog(sbuf);	// remember used offset 

	//	Show E-Paper Startup Screen with discovered Runtime data
	if(cfgini->hc_display == 1){
		PutStartScreen();
	}
	
	// Start of IO data logging endless-loop
	// ---------------------------------------------------------
	//	1. Get OneWire sensor data -> w1_devices[i]
	//	2. get HX711 ADC Data -> weight1, weight2
	//	3. get battery level -> battlevel
	// Check logged data for statistical conformance -> datcal()
	// log data in BHDB & CSV file and any notice into LOG file
	// Update Web representation & export data if enabled
	//		Wait a while
	// Rescan config.ini file again
	// Rescan notice field
	while(1){
		// get global time stamp of current dataloop
		gettimeofday(&MyTime, NULL);	
		tinfo = localtime(&MyTime.tv_sec);	// get current timestamp
		
		// in certain log data cases we need a newline
		if((lflags >0) && (lflags != LOGBH)) printf("\n");
		BHLOG(LOGBH){
			strftime(TimeString, 80, "%d-%m-%y %H:%M:%S ", tinfo);
			printf("%s", TimeString);
		}
		if((lflags >0) && (lflags != LOGBH)) printf("\n");
		didx = bhdb.drowidx;		// get index to next free data-row in BH-DB
		bhdb.dlog[didx].tvsec = MyTime.tv_sec; // remember current timestamp

		// Get battery/charging voltage levels
		float VPS = 4.096 / 32768;	// volts per step; corresponding to register ADS_PGA4096V

		// check 3.3V Battery-level on ADS-Port0
		// battlevel = readadc2(ADCCH0);	// get current battery level at CH0
		battlevel = (float) readads(ADS_ABSAIN0);	// read 5V battery level on ADS port 0 to GND
		if (battlevel <0){
			BHLOG(LOGMON) printf("    ReadADS: channel#%d: RC=%f\n",ADS_ABSAIN0/16-4, battlevel);
			bhdb.dlog[didx].Batt3V = 3.3;
		}else{
			bhdb.dlog[didx].Batt3V = battlevel * VPS;	// convert result to voltage range
			BHLOG(LOGMON) printf("    ReadADS: channel#%d: BattLevel= %1.3fV\n", ADS_ABSAIN0/16-4, bhdb.dlog[didx].Batt3V);
		}
		
		// check 5V Battery-level on ADS-Port3
		// battlevel = readadc2(ADCCH3);	// get current battery level at CH0
		battlevel = (float) readads(ADS_ABSAIN3);	// read 5V battery level on ADS port 3 to GND
		if (battlevel <0){
			BHLOG(LOGMON) printf("    ReadADS: channel#%d: RC=%f\n",ADS_ABSAIN3/16-4, battlevel);
			bhdb.dlog[didx].Batt5V = 5.0;
		}else{
			bhdb.dlog[didx].Batt5V = battlevel * VPS * 2;	// convert result to voltage range
			BHLOG(LOGMON) printf("    ReadADS: channel#%d: BattLevel= %1.3fV\n", ADS_ABSAIN3/16-4, bhdb.dlog[didx].Batt5V);

			// Voltage range of dmon->Batt5V should be >4,7V -> 5.00V
			if(bhdb.dlog[didx].Batt5V < cfgini->web_alarm_batt1){	// battery level has reached 1. threshold
				if(bhdb.dlog[didx].Batt5V < cfgini->web_alarm_batt2){// battery level has reached 2. threshold	
					alarms.batterie = 2;
				}else{
					alarms.batterie = 1;
				}
				rc = myalarm(&bhdb, &alarms);	// check if alarm action to be handled
			}
		}
		bhdb.dlog[didx].BattCharge1	= 0;
		bhdb.dlog[didx].BattCharge2	= 0;
			
		// get all 1-wire sensor data of beehive
		for(i=0; i < w1_devidx; i++){	// for all discovered w1 slaves...
			w1_devices[i].tval=0;
			if( rc=getw1val(i)){			// read out slave[i] data of each sensor
				tempext=0;
				tempintern=0;
				temphive1=0;
				temphive2=0;
				BHLOG(LOGOW) printf("    BeeHive: Error getting OneWire data of Index %i (%i)\n", i, rc);
				sprintf(sbuf, " - Exit: Error getting OneWire data of Index %i (%i)", i, rc);
				beelog(sbuf);			// init of program log file and remember used offset
				
				i=w1_devidx;			// step out of lop
//				exit(EXIT_FAILURE);
			}else{						// we got sensor data
				if(strncmp(w1_devices[i].ID, cfgini->owtempextid, strlen(cfgini->owtempextid))==0 ){ // external temperature sensor
					w1_devices[i].tval += cfgini->owtcext;
					tempext = w1_devices[i].tval;			// get external temperature for weight compensation
				}
				if(strncmp(w1_devices[i].ID, cfgini->owtempintid, strlen(cfgini->owtempintid))==0){  // hive weight cell temperature sensor
					w1_devices[i].tval += cfgini->owtcint;
					tempintern = w1_devices[i].tval;		// get hive1 internal temperature for weight log
				}
				if(strncmp(w1_devices[i].ID, cfgini->owtemphive1id, strlen(cfgini->owtemphive1id))==0){  // hive internal temperature sensor1
					w1_devices[i].tval += cfgini->owtchive1;
					temphive1 = w1_devices[i].tval;			// get hive2 internal temperature for weight log
				}
				if(strncmp(w1_devices[i].ID, cfgini->owtemphive2id, strlen(cfgini->owtemphive2id))==0){  // hive internal temperature sensor2
					w1_devices[i].tval += cfgini->owtchive2;
					temphive2 = w1_devices[i].tval;			// get hive2 internal temperature for weight log
				}
			}
		}
		// save discovered Temp. sensor data
		bhdb.dlog[didx].TempExtern	= tempext;
		bhdb.dlog[didx].TempIntern	= tempintern;
		bhdb.dlog[didx].TempHive1	= temphive1;
		bhdb.dlog[didx].TempHive2	= temphive2;

		// get Bee Hive weight of Port A
		weight1 = gethx711(offset, tempext, i, 0);	// get all sensor data: weight1 & temp.1 on channel A(128)
		weight2=0;
		//	weight2 = gethx711(offset2, tempext, i, 0);		// get all sensor data: weight2 & temp.2  on channel A(128)
		//	BHLOG(LOGBH) printf("Gewicht: %3.3f kg", weight2); 
		//		cfgini section: hx must be duplicated for hive2
		//		and we need a hxidx data field

		// Save discovered Weight values
		bhdb.dlog[didx].HiveWeight1	= weight1;
		bhdb.dlog[didx].HiveWeight2	= weight2;

		// well, we got raw weight value, make statistical validation 
		if(didx>0){	// from 2. value on ...
			bhdb.dlog[didx].GewDelta1 = weight1-bhdb.dlog[didx-1].HiveWeight1;
			bhdb.dlog[didx].GewDelta2 = weight2-bhdb.dlog[didx-1].HiveWeight2;
		}
		strncpy(bhdb.dlog[didx].comment, csv_notice, strlen(csv_notice));

		// check logdata for statistical conformance
		rc = datacal(&bhdb);
		if(rc >= 0){
			// data value range o.k.
			// write all results to Log File
			rc = beecsv(&bhdb);
			if(rc < 0){
				//Problem writing data back to csv log file
				BHLOG(LOGBH){
					sprintf(sbuf, " BeeCSV1 error RC=%i\n", rc); 
					beelog(sbuf);
				}
				// probably no more free handles
				system("netstat -tulip > netstat1.txt");
				system("lsof > lsof1_exit.txt");

				if(cfgini->bh_actionIOfailure == AOF_EXIT){
					BHLOG(LOGBH) printf("    Beehive: RC=%i -> BeeCsv1 error Exit initiated\n", rc);
					exit(EXIT_FAILURE);
				}
				if(cfgini->bh_actionIOfailure == AOF_REBOOT){
					BHLOG(LOGBH) printf("    Beehive: RC=%i -> BeeCsv1 error Reboot initiated\n", rc);
					system("reboot");
				}
			}
			rc = myalarm(&bhdb, &alarms);	// check if alarm action to be handled
			// todo: RC check here...
			
			sprintf(csv_notice, "ok");	// reset notice field once it was written			
		}else{			
			// drop current weight value, something was wrong with it
			BHLOG(LOGBH) printf("skipped\n"); // current data entry was skipped -> invalid/out of range

			// write results to Log File except weight data
			rc = beecsv(&bhdb);
			if(rc < 0){
				//Problem writing data back to csv log file
				BHLOG(LOGBH){
					sprintf(sbuf, " BeeCSV2 error RC=%i\n", rc); 
					beelog(sbuf);
				}

				system("netstat -tulip > netstat1.txt");
				system("lsof > lsof1_exit.txt");

				if(cfgini->bh_actionIOfailure == AOF_EXIT){
					BHLOG(LOGBH) printf("    Beehive: RC=%i -> BeeCsv2 error Exit initiated\n", rc);
					exit(EXIT_FAILURE);
				}
				if(cfgini->bh_actionIOfailure == AOF_REBOOT){
					BHLOG(LOGBH) printf("    Beehive: RC=%i -> BeeCsv2 error Reboot initiated\n", rc);
					system("reboot");
				}
			}

			BHLOG(LOGHX1){
				sprintf(sbuf, " CSV line written - but weight1+2: got dropped");  
				beelog(sbuf);
			}
			rc = myalarm(&bhdb, &alarms);	// check if alarm action to be handled
			// todo: RC check here...			
		}
		
		BHLOG(LOGBH) printf("    BeeHive[%d]: Weight1: %3.3fkg  Temp1(Ext/Hive/WScale): %2.1f°C / %2.1f°C / %2.1f°C\n",
			didx, bhdb.dlog[didx].HiveWeight1, bhdb.dlog[didx].TempExtern, bhdb.dlog[didx].TempHive1, bhdb.dlog[didx].TempIntern); 

		
		// update monitoring data of ePaper display
		if(cfgini->hc_display == 1){
			PutStatusScreen(didx );
		}
		
		// update Webpage data destination
		if(cfgini->hc_webui == 1){
			BHLOG(LOGXFER) printf("    BeeHive: Start Web UI Update...\n");
			// update web for data update/action
			rc = webui(&bhdb);
			BHLOG(LOGXFER) printf("    BeeHive: webui rc=%i\n", rc);
		}
		
		//*********************************************************************
		// custom wait loop till next measurement cycle
		delay(cfgini->bh_loopwait*1000); // get wait time in sec. -> calc. msec.
		//*********************************************************************

		if((lflags >0) && (lflags != LOGBH)) printf("    BeeHive: (Re-)Read config data ...\n");
		//re-read config.ini file again (could have been changed in between) 
		cfgini = getini(CONFIGINI);
		if( cfgini == NULL){
			BHLOG(LOGBH) printf("    BeeHive: CFG File not found (2) at: %s\n", CONFIGINI);

			// exit(EXIT_FAILURE);
			// continue with already buffered config struct data till we have access again

			BHLOG(LOGBH){
				sprintf(sbuf, " CFG file not found at: %s - but beehive continued\n", CONFIGINI); 
				beelog(sbuf);
			}
			sprintf(csv_notice, "No cfg file!");
		}
		lflags	= (unsigned char) cfgini->bh_verbose;	// get the custom verbose mode again
		
		if(newstart == 0){
			// Put a notice to the data diagramm: "restarted"
			addwebnotice("Gewicht1", "Logger restarted", &bhdb);
			newstart=1;		// o.k. restart message is placed
		}else{
			// probably someone has provided a new notice for us for last dataline
			if(getnotice(csv_notice, &bhdb) != NULL){
				strncpy(bhdb.dlog[didx].comment, csv_notice, strlen(csv_notice));
			}
		}
		bhdb.drowidx++;				// enable raw datarow idx to next free entry

	} // end of while(1) log loop
	
  return (rc);
} // end of main.c


/*******************************************************************************
* Function: InitAll()
* Creation: 13.02.2017
* 
* Initialize all program related infrastructure and runtime settings:
* - setup WiringPi Lib
* - get config.ini start runtime parameters
* - init all IO modules: HX711 and OnwWire modules
*   
* Input :
*	hxnum		1..n	number of HX711 modules to be initialized
*	hxchannel	0,1,2	Select HX711 channel and gain value
*								 r = 0 - 128 gain ch A
*								 r = 1 - 32  gain ch B
*								 r = 2 - 63  gain ch A
* Global:	
*	cfgini		struct of runtime parameters	-> get runtime params
*	bhdb		virt. database of statistic data -> set to "no data"
*	alarmflag	field of alarmflags dep. on type.-> set to 0
*   csv_notice	field for service notices for csv-file logging
*
* Output:
* w1_devices	pointer to OneWire device init data struct	
* and direct exit by errors
*********************************************************************************
*/
w1_values * initall(int hxnum, int hxchannel){
int	i,rc=0;
char sbuf[1024];
w1_values * w1_dev;	// 1-wire sensor data field pointer

	if (setuid(getuid()) < 0){	//make this App mono user
		perror("    BeeLog-Init: Dropping privileges failed\n");
		exit(EXIT_FAILURE);
	}
	
	// retrieve initial config runtime settings
	cfgini = getini(CONFIGINI);
	if( cfgini == NULL){
		printf("    BeeLog-Init: INI FIle not found at: %s\n", CONFIGINI);
		exit(EXIT_FAILURE);
	}
	lflags		= (unsigned char) cfgini->bh_verbose;	// now we have the custom verbose mode
	sprintf(sbuf, "##### BeeLogger - logging started #####");  
	beelog(sbuf);	// log init of program log file

	printf("============== Version %s =========================\n\n", cfgini->version);

#ifdef BHSIM
	printf("    BeeLog: HW Simulation only\n");
#else
	//  setup Wiring-PI Lib
	if(wiringPiSetupGpio() < 0) {    // using Broadcom GPIO pin mapping
		BHLOG(LOGBH) printf("    BeeLog-Init: wiringPi-Init for GPIO# Error\n");
		beelog(" - Exit: Wiring-Pi Lib GPIO init failed\n");
		if(cfgini->bh_actionIOfailure != AOF_NOOP){
			exit(EXIT_FAILURE);
		}
	}	

	// activate test LED IO Port
	pinMode(cfgini->hc_lediopin, OUTPUT);
	digitalWrite(cfgini->hc_lediopin, TESTLED_ON);	// show up Init Phase

	BHLOG(LOGHX1) printf("    BeeLog-Init: WiringPi-Lib detected -> Init HX711 now ...\n");
#endif
		
// Get 1-Wire slave IDS and values
	BHLOG(LOGHX1) 	printf("    BeeLog: Get 1-wire IDs and values\n");
	w1_dev = getw1list(cfgini->owfspath);  // discover W1 devices by ID type 
	
// Init HX711 A/D Ports A+B
	for(i=1; i<=hxnum; i++){
		if(rc=inithx711(i, hxchannel)){	// init HX module on channel A(128)
			BHLOG(LOGBH) printf("    Beelog: Error: HX711#1 Init rc = %i\n", rc);
			sprintf(sbuf, " - Exit: Init HX711#%i: failed rc: %i", i, rc);  
			beelog(sbuf);
			if(cfgini->bh_actionIOfailure != AOF_NOOP){
				digitalWrite(cfgini->hc_lediopin, TESTLED_OFF);	// show End of Init Phase
				exit(EXIT_FAILURE);
			}
		}
		sprintf(sbuf, " - HX711#%i: Init done.", i);
		BHLOG(LOGBH) beelog(sbuf);
	}

//	reset datastor, alarmflag field and weather forecast field
		initbhdb(&bhdb);

// Get system runtime data	-> result is in globale "struct mondata"
		BHLOG(LOGMON) printf("    BeeLog-Init: Start raspimon...\n");
		raspimon();	
	
// preset ADC / ADS device for battery management
	// preset ADS1115 
		BHLOG(LOGMON) printf("    BeeLog-Init: Start ADS-Init...\n");
		i = ads_init(ADSI2CADR);

	// preset ADC MCP300x 
	//	i = adc_init2();
	
	if (i < 0){
		BHLOG(LOGMON) printf("    BeeLog-Init: ADS/ADC Init fails (%i)\n",i);
		alarms.batterie	= 1;
	}else{
		alarms.batterie	= 0;		
	}

	alarms.swarm	= 0;
	alarms.weight	= 0;

// preset weather data
	for (i=0; i < MAXWDAYS; i++){
		webweather[i].day_rain = 0;
		webweather[i].day_temp_max=0;
		webweather[i].day_temp_min=0;
	}

//
// WaveShare E-paper Configuration
//
	if( cfgini->hc_display == 1){
		BHLOG(LOGMON) printf("    BeeLog-Init: Start init E-Paper\n");
		if( DEV_Module_Init() != 0){
			printf("    BeeLog-Init: e-Paper init failed !\n");
				exit(EXIT_FAILURE);
		}
		printf("    BeeLog-Init: e-Paper Init and Clear...\r\n");
		EPD_2IN7_Init();
		EPD_2IN7_Clear();

		// preset GPIO line status of Key 1-4
		pinMode(cfgini->ep_key1, INPUT);
		pinMode(cfgini->ep_key2, INPUT);
		pinMode(cfgini->ep_key3, INPUT);
		pinMode(cfgini->ep_key4, INPUT);
	}

// Inhouse stuff init
	sprintf(csv_notice, "started");	// prepare "Start" notice of new value series	
	curlcount = 0;				// reset counter of curllib perform actions for debugging
	
	// show "InitPhase done"
	digitalWrite(cfgini->hc_lediopin, TESTLED_OFF);	// show End of Init Phase
	
  return(w1_dev);
} // end of InitAll()


/*******************************************************************************
* Function: webui()
* Creation: 03.02.2017
* 
* do everything to create and update the web page pages
*
* Input :
*	dataset		currently collected parameter set for evaluation
*
* Global:	
*	cfgini		struct of runtime parameters
*	bhdb		virt. database of statistic data
*	alarmflag *	field of alarmflags dep. on type.
*
* Output:
*	return		 0 all web pages updated
*				 <0 web page not found / corrupt
*********************************************************************************
*/
int webui(dataset * data){
	int rc=0;
	
	// A CURL commandline that works also:
	// curl -T beelog2017.csv -u <user>:<passwd> -x socks5://socks.fsc.net:1080 ftp://ftp.randolphesser.de/beelog/beelog2017.csv
	BHLOG(WEBUPD) printf("    WebUpd: started via ftp...\n");

	if(cfgini->web_autoupdate == 0){ // FTP access not permitted
		BHLOG(WEBUPD) printf("    WebUpd: no permission for FTP access by config.ini autoupdate flag\n");
		rc = 0;
	}else{
		// export result data to ftp site

		// ... using curllib function
		// rc = export(EXPTYPE_FTP, EXPFORM_RAW, cfgini->exp_ftpurl, cfgini->exp_ftppath);

		// ... using system() function
		rc = export(EXPTYPE_FTP, EXPFORM_SYSTEM, cfgini->exp_ftpurl, cfgini->exp_ftppath);

		// for test: import result data from ftp site again
		// rc = import(IMPTYPE_FTP, IMPFORM_SYSTEM, cfgini->exp_ftpurl, cfgini->exp_ftppath);
	}
		
  return(rc);
} // end of webui()


/*******************************************************************************
* Function: import()
* Creation: 03.02.2017
* 
* check import and update internal/DB datsets accordingly
* 
* Input :
*	imptype		do import by FTP/SMB/MAIL/...
*	impformat	import it raw or in other format (conversion needed, t.b.d.)
*	url			string of target SITE URL
*	path		relative path to url on target side
*
* Global:	
*	cfgini		struct of runtime parameters
*
* Output:
*	return		 0 all data could be imported
*				 <0 could not import data -> format not readable/unknown
*********************************************************************************
*/
int import(int imptype, int impformat, char* url, char * path){
struct timeval	curTime;
char			TimeString[128];
char	sbuf[1024];
char	cmdline[1024];	// commandline buffer for system() call
int		rc=0;

	// get current timestamp
	gettimeofday(&curTime, NULL);
	strftime(TimeString, 80, "%Y", localtime(&curTime.tv_sec));	// get current year only
#ifdef BHSIM
	char sbufsim[256];
	sprintf(sbufsim, "%s%ssim.csv", cfgini->bh_CSVFILE, TimeString);	// build full CSV file name
#endif
	sprintf(sbuf, "%s%s.csv", cfgini->bh_CSVFILE, TimeString);	// build full CSV file name

	switch(imptype){
	case IMPTYPE_FTP: 
		if(impformat == IMPFORM_RAW){ 	// copy with same name to dest. using .netrc file
#ifdef BHSIM
			BHLOG(LOGXFER) printf("    Import: started by FTP(raw) from %s/%s/%s\n", url, path, sbufsim);
			rc = ftpget(url, path, sbufsim, cfgini->web_root, sbufsim, NULL, NULL, cfgini->exp_ftpport, cfgini->exp_ftpproxy, cfgini->exp_ftpproxyport);
#else
			BHLOG(LOGXFER) printf("    Import: started by FTP(raw) from %s/%s/%s\n", url, path, sbuf);
			rc = ftpget(url, path, sbuf,  cfgini->web_root, sbuf, NULL, NULL, cfgini->exp_ftpport, cfgini->exp_ftpproxy, cfgini->exp_ftpproxyport);
#endif
		}else if(impformat == IMPFORM_SYSTEM){	// start FTP transfer uing system() function
		//	sprintf(cmdline,"curl -n ftp://%s:%s/%s/%s -o %s/ext%s", url, cfgini->exp_ftpport, path, cfgini->web_noticefile, cfgini->web_root, cfgini->web_noticefile );
		//	BHLOG(LOGXFER) printf("     Import: %s\n", cmdline);
		//	system(cmdline);
			BHLOG(LOGXFER) printf("    Import: started by FTP(system) but no job definition yet\n");
			rc=-4;
		}else{
			// IMPFORM_DOC: other format conversion not supp. now
			rc=-3;
		}
		break;
	case IMPTYPE_SMB: 
		BHLOG(LOGXFER) printf("    Import: started by SMB: but not supported\n");
		rc=-1;
		break;
	case IMPTYPE_MAIL: 
		BHLOG(LOGXFER) printf("    Import: started by MAIL: but not supported\n");
		rc=-2;
		break;
	}

	return(rc);
} // end of import()


/*******************************************************************************
* Function: export()
* Creation: 03.02.2017
* 
* collect all data for export and create export data format accordingly
* 
* Input :
*	exptype		do export by FTP/SMB/MAIL/...
*	expformat	export it raw or in other format (conversion needed, t.b.d.)
*	url			string of target SITE URL
*	path		relative path to url on target side
*
* Global:	
*	cfgini		struct of runtime parameters
*
* Output:
*	return		 0 all data could be exported
*				 <0 could not export data -> format unknown / sources not reachable
*********************************************************************************
*/
int export(int exptype, int expformat, char* url, char * path){
struct timeval	curTime;
char			TimeString[128];
char	cmdline[1024];	// commandline buffer for system() call
char	sbuf[1024];
char	sbuf2[1024];
int		rc=0;
char	logfile[1024];			// log file path buffer	
char	logfile2[1024];			// log file2 path buffer	
CURL *curlhandle = NULL;

	// get current timestamp
	gettimeofday(&curTime, NULL);
	strftime(TimeString, 80, "%Y", localtime(&curTime.tv_sec));	// get current year only
#ifdef BHSIM
	char sbufsim[256];
	char sbufsim2[256];
	sprintf(sbufsim, "%s%ssim.csv", cfgini->bh_CSVFILE, TimeString);	// build full LOG.CSV file name
	sprintf(sbufsim2, "%s%ssim.csv", cfgini->bh_CSVDAYS, TimeString);	// build full BEEDAYS.CSV file name
#endif
	sprintf(sbuf, "%s%s.csv", cfgini->bh_CSVFILE, TimeString);	// build full LOG.CSV file name
	sprintf(sbuf2, "%s%s.csv", cfgini->bh_CSVDAYS, TimeString);	// build full BEEDAYS.CSV file name
	sprintf(logfile, "%s", cfgini->bh_LOGFILE); 
	sprintf(logfile2, "xxl_%s", cfgini->bh_LOGFILE); 

	switch(exptype){
	case EXPTYPE_FTP: 
		if(expformat == EXPFORM_RAW){ 	// copy with same name to dest. using .netrc file
#ifdef BHSIM
			BHLOG(LOGXFER) printf("    Export(BHSIM): started by FTP(raw) to %s/%s/%s\n", url, path, sbufsim);
			curl_global_init(CURL_GLOBAL_ALL);
			curlhandle = curl_easy_init();

			if(rc = ftpput(curlhandle, url, cfgini->web_root, sbuf, path, sbufsim, NULL, NULL, cfgini->exp_ftpport, cfgini->exp_ftpproxy, cfgini->exp_ftpproxyport) !=0 ){
				BHLOG(LOGXFER) printf("    Export: ftpput failed with rc=%i, curlcount=%i\n", rc, curlcount);
				rc=-10;
			}else
			if(rc = ftpput(curlhandle, url, cfgini->web_root, sbuf2, path, sbufsim2, NULL, NULL, cfgini->exp_ftpport, cfgini->exp_ftpproxy, cfgini->exp_ftpproxyport) !=0 ){
				BHLOG(LOGXFER) printf("    Export: ftpput failed with rc=%i, curlcount=%i\n", rc, curlcount);
				rc=-11;
			}

			curl_easy_cleanup(curlhandle);
			curl_global_cleanup();
#else
			BHLOG(LOGXFER) printf("    Export: started by FTP(raw) to %s/%s\n", url, path);
			curl_global_init(CURL_GLOBAL_ALL);
			curlhandle = curl_easy_init();
			
			if(rc = ftpput(curlhandle, url, cfgini->web_root, sbuf,	path, sbuf,		NULL, NULL, cfgini->exp_ftpport, cfgini->exp_ftpproxy, cfgini->exp_ftpproxyport) !=0 ){
				BHLOG(LOGXFER) printf("    Export: ftpput failed with rc=%i, curlcount=%i\n", rc, curlcount);
				rc=-10;
			} else			
			if(rc = ftpput(curlhandle, url, cfgini->web_root, sbuf2,	path, sbuf2,	NULL, NULL, cfgini->exp_ftpport, cfgini->exp_ftpproxy, cfgini->exp_ftpproxyport) !=0){
				BHLOG(LOGXFER) printf("    Export: ftpput failed with rc=%i, curlcount=%i\n", rc, curlcount);
				rc=-11;
			} else
			if(rc = ftpput(curlhandle, url, cfgini->web_root, logfile,	path, logfile,	NULL, NULL, cfgini->exp_ftpport, cfgini->exp_ftpproxy, cfgini->exp_ftpproxyport) !=0){
				BHLOG(LOGXFER) printf("    Export: ftpput failed with rc=%i, curlcount=%i\n", rc, curlcount);
				rc=-12;
			} else
			if(rc = ftpput(curlhandle, url, cfgini->web_root, cfgini->web_deffile, path, cfgini->web_deffile, NULL, NULL, cfgini->exp_ftpport, cfgini->exp_ftpproxy, cfgini->exp_ftpproxyport) !=0){
				BHLOG(LOGXFER) printf("    Export: ftpput failed with rc=%i, curlcount=%i\n", rc, curlcount);
				rc=-13;
			}else{
				break;	// all done correctly
			}
			
			curl_easy_cleanup(curlhandle);
			curl_global_cleanup();
#endif
		}else if(expformat == EXPFORM_SYSTEM){	
		// start FTP transfer using cmdline call of curllib by system() function
#ifndef BHSIM
			sprintf(cmdline,"/usr/bin/curl -T %s/%s -n ftp://%s:%s/%s/%s", cfgini->web_root, sbuf, url, cfgini->exp_ftpport, path, sbuf );
			BHLOG(LOGXFER) printf("     Export: %s\n", cmdline);
			system(cmdline);
			sprintf(cmdline,"/usr/bin/curl -T %s/%s -n ftp://%s:%s/%s/%s", cfgini->web_root, sbuf2, url, cfgini->exp_ftpport, path, sbuf2 );
			BHLOG(LOGXFER) printf("     Export: %s\n", cmdline);
			system(cmdline);
			sprintf(cmdline,"/usr/bin/curl -T %s/%s -n ftp://%s:%s/%s/%s", cfgini->web_root, logfile, url, cfgini->exp_ftpport, path, logfile );
			BHLOG(LOGXFER) printf("     Export: %s\n", cmdline);
			system(cmdline);
			sprintf(cmdline,"/usr/bin/curl -T %s/%s -n ftp://%s:%s/%s/%s", cfgini->web_root, cfgini->web_deffile, url, cfgini->exp_ftpport, path, cfgini->web_deffile );
			BHLOG(LOGXFER) printf("     Export: %s\n", cmdline);
			system(cmdline);			

			BHLOG(LOGBH){
				sprintf(cmdline,"/usr/bin/curl -T ./%s -n ftp://%s:%s/%s/%s", logfile2, url, cfgini->exp_ftpport, path, logfile2 );
				BHLOG(LOGXFER) printf("     Export: %s\n", cmdline);
				system(cmdline);
			}
#endif
		}else{
			// EXPFORM_DOC: other format conversion not supp. now
			BHLOG(LOGXFER) printf("    Export: started by FTP(doc), but conversion not supported yet\n");
			rc=-15;
		}
		break;
	case EXPTYPE_SMB: 
			BHLOG(LOGXFER) printf("    Export: started by SMB, but not supported yet\n");
		rc=-20;
		break;
	case EXPTYPE_MAIL: 
			BHLOG(LOGXFER) printf("    Export: started by MAIL, but not supported yet\n");
		rc=-30;
		break;
	}

 return(rc);
} // end of export()

/*******************************************************************************
* Function: getweather()
* Creation: 03.02.2017
* 
* get weather forecast data from the web and prepare it for webui
* 
* Input :
*	webpath 	path to webdata source/provider
*	wdata	*	pointer to weather data forecast structure
*
* Global:	
*	cfgini		struct of runtime parameters
*
* Output:
*	return		 0 new weather data was created
*				 <0 could not find web source data 
*			        or weather data not readable / provider path unknown
*********************************************************************************
*/
int getweather(char * webpath, wdataset * wdata){
	BHLOG(WEBUPD) printf("    WebUPD: started for weather forecast, but not supported yet\n");
	return(-1);
} // end of getweather()

/*******************************************************************************
* Function: getnotice()
* Creation: 03.02.2017
* 
* read and store a service notice to the BHDB and WebUI presentation
* Drive the dialogue with the user to enter a notice (via webui?)
* Create a dataline by date and time + a full prepared string
* 
* Input : notice pointer to notice text buffer field
*         data	 pointer to bhdb database
*	
* Global:	
*	cfgini		struct of runtime parameters
*
* Output:
*	return		 char * new notice string was created/retrieved
*				 NULL   dialogue was canceled or failed
*********************************************************************************
*/
char * getnotice(char * notice, dataset* data){
int idx;
char fname[256];
char sbuf[WEBIDXMAXLLEN];
char notebuf[WEBIDXMAXLLEN];
char cmdline[1024];	// commandline buffer for system() call
char series[256];
FILE * fd;
int		i, slen;			// char counter
long int nlen;				// length of notice message
char	mode[] = "0666";	// access mode of empty notice text file
int		modeoct;

	if(cfgini->web_autoupdate == 0){	// FTP access not permitted
		BHLOG(LOGMON) printf("    GetNotice: no permission for FTP access by config.ini autoupdate flag\n");
		return(NULL);
	}

	// check presence of annotation file
	idx = data->drowidx;

	// get Web Noticefile and store as external one using system(curl) function
	sprintf(cmdline,"/usr/bin/curl -n ftp://%s:%s/%s/%s -o %s/ext%s",
			cfgini->exp_ftpurl, cfgini->exp_ftpport, cfgini->exp_ftppath, cfgini->web_noticefile, 
			cfgini->web_root, cfgini->web_noticefile );
#ifdef BHSIM
	BHLOG(LOGXFER) printf("    GetNotice(BHSIM): get Web-Noticefile: %s simulated\n", cmdline);
	return(NULL);
#endif

	BHLOG(LOGXFER) printf("    GetNotice: get Web-Noticefile: %s\n", cmdline);
	system(cmdline);

	// build full source path + name of web-notice file
	sprintf(fname,"%s/ext%s", cfgini->web_root, cfgini->web_noticefile);
	if((fd = fopen(fname, "r")) == NULL) {
		BHLOG(LOGMON) printf("    GetNotice: no notice file found at %s\n", fname);
		return(NULL);
	}
		
	fseek (fd, 0L, SEEK_END);
	nlen = ftell(fd);					// get the orig file size
	fseek(fd, 0L, SEEK_SET);			// go back to start of file

	if(nlen == 0){
		BHLOG(LOGMON) printf("    GetNotice: no text found in %s\n", fname);
		fclose(fd);				// release handle
		return(NULL);
	}
	
	// fetch first message line (only)
	fgets(sbuf, WEBIDXMAXLLEN, fd);
	fclose(fd);				// thats it
	
	slen = strlen(sbuf);
	for(i=0; i<slen; i++){	// search for end sign of 'series' string
		if(sbuf[i]==':') 
			break;
	}
	strncpy(series, sbuf, i);	// get 'series' string
	series[i]='\000';
	strncpy(notice, &sbuf[i+1], slen-i);	// get remaining notice text
	notice[slen-i-1]='\000';

	// Re-create the file (but keep it empty).
    if ( remove (fname) == -1 ) {
		BHLOG(LOGBH) printf("    GetNotice: recreated %s with 0 byte length\n", fname);
	}
	if((fd = fopen(fname, "wb")) == NULL) {
		BHLOG(LOGBH) printf("    GetNotice: recreation of notice file %s failed\n", fname);
	}else{
		fclose(fd);	// we want just an empty file
	}
	modeoct = strtol(mode, 0, 8);
    if (chmod (fname,modeoct) < 0)
    {
		BHLOG(LOGHX2) printf("    AddNotice: error in chmod(%s) [%s]\n", fname, mode);
    }

	// Got annotation text -> put it to webpage of current date
	addwebnotice(series, notice, data);
	
	// overwrite web notice file remote by local reference file using system(curl) function
	sprintf(cmdline,"/usr/bin/curl -T %s/%s -n ftp://%s:%s/%s/%s", 
			cfgini->web_root, cfgini->web_noticefile, 
			cfgini->exp_ftpurl, cfgini->exp_ftpport, cfgini->exp_ftppath, cfgini->web_noticefile );
	BHLOG(LOGXFER) printf("     GetNotice: reset web-notice file: %s\n", cmdline);
	system(cmdline);
		
	return(notice);
} // end of getnotice()

/*******************************************************************************
* Function: beecounter()
* Creation: 03.02.2017
* 
* In case a bee counter was installed:
* 1. call: initiate the beecounter ISR and 
* 2. call: provide statistic data of counting window: init <-> up to now
* 
* Input :
* action	=1 init beecounter
*			=2 provide count data
*			=3 reset/stop beecounter
*	beecnt * global beecounter served by ISR
*	
* Global:	
*	cfgini		struct of runtime parameters
*	bhdb		virt. database of statistic data
*
* Output:
*	return		 0 all o.k.
*				 <0 error by rc code
*********************************************************************************
*/
int beecounter(int action, long * beecnt){
	if(cfgini->hc_beecounter == 0){
		BHLOG(LOGBH) printf("    BeeCounter: started, but no count module configured\n");
		return(-1);
	}

	BHLOG(LOGBH) printf("    BeeCounter: started, but not Count module not supported yet\n");
	return(0);
} // end of beecounter()

/*******************************************************************************
* Function: getgps()
* Creation: 03.02.2017
* 
* In case a GPS mouse was installed (via USB):
* 1. call: initialize the GPS module  and 
* 2. call: read GPS data
* 
* Input :
* action	=1 init GPS mouse
*			=2 provide GPS data
* Global:	
*	cfgini		struct of runtime parameters
*	bhdb		virt. database of statistic data
*
* Output:
*	GPSdat *	pointer to GPS data struct
*				NULL	if an error occured
*********************************************************************************
*/
gpsdata* getgps(int action, gpsdata * gpsdat){
	if(cfgini->hc_gpsmouse == 0){
		BHLOG(LOGBH) printf("    getGPS: started, but no GPS mouse configured\n");
		return(NULL);
	}

	BHLOG(LOGBH) printf("    getGPS: started, but no GPS mouse supported yet\n");
	return(gpsdat);
} // end of getgps()



/*******************************************************************************
* Function: function() template
* Creation: 01.01.2019
* 
* <Description Text>
* 
* Input :
* Param		=0 action 0
*			=1 action 1
* Global:	
*	gparam		struct of gparam
*
* Output:
*	rc		=0 all done as expected
*			=-1	an error occured
*********************************************************************************
*/