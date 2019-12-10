/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*******************************************************************************
 * File:	get1wire.c
 * Author:	Randolph Esser - Copyright 2017-2019
 * Created on 17. Januar 2017, 10:09
 * This file is part of the "beelog" program/project.
 *
 * Description :
 *   A simple access library for 1-wire devices connected to the default RPi w1 GPIO pin
 *	which is  used by the PI kernel module driver
 * Inside Raspian:
 *	After detection of a W1 device a node was created at W1_PATH\<w1-ID>
 *	and a value file was created named W1_PATH\<w1-ID>\W1_FNAME_VAL
 *	W1_PATH is defined in config.ini file
 *
 *******************************************************************************
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <wiringPi.h>

#include "get1wire.h"
#include "getini.h"
#include "beehive.h"
#include "beelog.h"


// Global Data field of 1-wire sensors
w1_values	w1_dev[W1_MAX_SLAVES];	
int			w1_devidx;			// counts # of detected W1 slave devices 1...W1_MAX_SLAVES



/*******************************************************************************
* Function: getw1list()
* Creation: 22.01.2017, R.Esser
* 
* Discovers all detected w1 slaves sensors and fill sensor list w1_dev[]
* This function leverages from RaspberryPi 1-wire sesnsor kernel driver.
* All descovered devices are presented in a filesystem tree starting from
* root point "w1devpath" (typically /sys/bus/w1/devices)
*  1. get discovered sensor counter
*  2. get apropriate sensor IDs
*  3. get sensor data for each ID by function getw1val()
*
* Input :
*	w1devpath		=0..n			log/debug level for console trace output
*
* Global:	
*	w1_dev[]		field of 1-wire sensor attributes and sensor data
*
* Output:
*	w1_dev[] *		pointer to updated 1-wire data struct
*
*********************************************************************************
*/
w1_values * getw1list(char* w1devpath){
	FILE *	fd;
	long	flen;
	int		i;
	int		w1type;
	int		w1_devnew;
	char	w1line[1024] = "";		// buffer for current w1 data line
	char	s1[256];				// generic string buffer
	
	w1_devidx	=0;		// up to now no w1 slaves known at all
	w1_devnew	=0;		// and no known one for beehive

	// get ID count	
	sprintf(w1line, "%s/%s", w1devpath, W1_IDCOUNT_FILE);  
	BHLOG(LOGOW) printf("    OW: ID CountPath: %s\n", w1line);

#ifdef BHSIM
	i=3;
#else
	// open W1 Device ID list file
	fd = fopen(w1line, "r");
    if (!fd) 
		return NULL;
	fgets(w1line, 1024, fd);	// read out first line of slave counter file
	fclose(fd);
	sscanf(w1line,"%i", &i);	// assumed only one value in this file/line
#endif
	w1_devnew=i;				// now we know the amount of slaves

	if(i=0){					// if no slave active exit here
	  BHLOG(LOGOW)
		printf("    OW: IDCount=0: No 1-wire sensors found\n");
		beelog(" - No 1-wire device detected - count=0\n");
	  return NULL;			// return: no slaves detected
	};

	BHLOG(LOGOW)	printf("    OW: -> %i sensors connected\n", w1_devnew);

	// get ID list
	sprintf(w1line, "%s/%s", w1devpath, W1_IDLIST_FILE);  
	BHLOG(LOGOW)	printf("    OW: ID ListPath: %s\n", w1line);

#ifdef BHSIM
	fd=NULL;
	for(i=0; i < w1_devnew; i++) {	// get ID string line
		switch(i){
		case 0:	sprintf(w1line, "28-051670d062ff");	// find Extern. temp. sensor
				break;
		case 1:	sprintf(w1line, "28-041670f10bff");	// find WScale. temp. sensor
				break;
		case 2: sprintf(w1line, "28-0416718bfbff");	// find Hive1 Temp. sensor
				break;
		case 3: sprintf(w1line, "28-012345678900");	// find Hive1 Temp. sensor
				break;
		}
#else
	fd = fopen(w1line, "r");		// open W1 Device ID list file
    if (!fd) return NULL;
	fseek (fd, 0L, SEEK_END);		// check length of file 
	if (ftell(fd) == 0){			// -> =0 no slaves detected
		BHLOG(LOGOW) printf("    OW: No 1-wire sensors found\n");
		beelog("No 1-wire device detected - list empty but count > 0\n");
		return NULL;				// no slaves detected
	};
	fseek(fd, 0L, SEEK_SET);		// go back to start of file

	for(i=0; i < w1_devnew && fgets(w1line, 1024, fd) != NULL; i++) {	// get ID string line
		//fgets reads one line at a time, up to the max size - not the next block of max size.
#endif
		sscanf(w1line,"%i-%s", &w1type, s1);							// get type ID: Byte1
		switch(w1type) {
			case W1_TYPE_DS18B20: 
				strncpy(w1_dev[w1_devidx].ID, w1line, W1_SLAVE_IDLEN);	// fetch ID string
				w1_dev[w1_devidx].ID[W1_SLAVE_IDLEN]=0;
				strncpy(w1_dev[w1_devidx].type, W1_TYPE28_NAME, W1_SLAVE_IDLEN);
				sprintf(s1, " - OW device %i of %i: %s - ID: %s detected", w1_devidx, i, W1_TYPE28_NAME, w1_dev[w1_devidx].ID);
				beelog(s1);
				// remember path to value data file of each ID
				sprintf(w1_dev[i].valpath, "%s/%s/%s", w1devpath, w1_dev[w1_devidx].ID, W1_FNAME_VAL);  
				BHLOG(LOGOW)	printf("    OW: ID%i DataPath: %s\n", w1_devidx, w1_dev[w1_devidx].valpath);
				w1_devidx++;
				break;
			case W1_TYPE_DS18S20: 
				strncpy(w1_dev[w1_devidx].ID, w1line, W1_SLAVE_IDLEN);	// fetch ID string
				strncpy(w1_dev[w1_devidx].type, W1_TYPE10_NAME, W1_SLAVE_IDLEN); 
				sprintf(s1, " - OW device %i of %i: %s - ID: %s detected", w1_devidx, i, W1_TYPE10_NAME, w1_dev[w1_devidx].ID);
				beelog(s1);
				// remember path to value data file of each ID
				sprintf(w1_dev[i].valpath, "%s/%s/%s", w1devpath, w1_dev[w1_devidx].ID, W1_FNAME_VAL);  
				BHLOG(LOGOW)	printf("    OW: ID%i DataPath: %s\n", w1_devidx, w1_dev[w1_devidx].valpath);
				w1_devidx++;
				break;
			case W1_TYPE_DS1822:  
				strncpy(w1_dev[w1_devidx].ID, w1line, W1_SLAVE_IDLEN);	// fetch ID string
				strncpy(w1_dev[w1_devidx].type, W1_TYPE22_NAME, W1_SLAVE_IDLEN); 
				sprintf(s1, " - OW device %i of %i: %s - ID: %s detected", w1_devidx, i, W1_TYPE22_NAME, w1_dev[w1_devidx].ID);
				beelog(s1);
				// remember path to value data file of each ID
				sprintf(w1_dev[i].valpath, "%s/%s/%s", w1devpath, w1_dev[w1_devidx].ID, W1_FNAME_VAL);  
				BHLOG(LOGOW)	printf("    OW: ID%i DataPath: %s\n", w1_devidx, w1_dev[w1_devidx].valpath);
				w1_devidx++;
				break;
			default: 
				sprintf(s1, " - OW device #%i: unknown ID: %s", i, w1line); 
				beelog(s1);
				break;	// unknown type
		}
	}
#ifndef BHSIM
	fclose(fd);
#endif
  return w1_dev;
} // end of getw1list()



/*******************************************************************************
* Function: getw1val()
* Creation: 22.01.2017, R.Esser
* 
* Reads out all detected w1 slaves from w1_dev[] and fill in sensor data
* This function leverages from RaspberryPi 1-wire sensor kernel driver.
* All discovered devices are presented in a filesystem tree starting from
* root point "w1devpath" (typically /sys/bus/w1/devices)
*
* Input :
*	i		= 0..n	index of 1-wire sensor in w1_dev[] table
*
* Global:	
*	w1_dev[]		field of 1-wire sensor attributes and sensor data
*
* Output:
*	w1_dev[]		updated by 1-wire sensor data
*   return			=0 got all data, =-1...-3 something went wrong -> no data written
*
*********************************************************************************
*/
int getw1val(int i){

	FILE *	fd;
	long	flen;
	int		w1type;
	uint	i1, i2, i3, i4, i5, i6, i7, i8, i9;
	char	w1line[1024] = "";		// buffer for current w1 data line
	char	s1[256];

	


#ifdef BHSIM
	sprintf(w1line,"1 2 3 4 5 6 7 8 9 : crc=0x1234 YES");
#else
	// parse 1-wire sensor data to value struct	
	fd = fopen(w1_dev[i].valpath, "r");
	if (!fd){	
		BHLOG(LOGOW) printf("    OW: could not open file %s (0x%x)\n", w1_dev[i].valpath, fd);
		return -1;
	}
	fgets(w1line, 1024, fd);		// get 1. data  line
#endif
	sscanf(w1line,"%x %x %x %x %x %x %x %x %x : crc=%x %s", &i1 ,&i2 ,&i3 ,&i4 ,&i5 ,&i6 ,&i7 ,&i8 ,&i9, &w1_dev[i].crc, s1);		// e.g.: 2d 00 4b 46 ff ff 02 10 19 : crc=19 YES
#ifndef BHSIM
	if(s1 == "NO") { // wrong CRC check
		// try again one time
		fclose(fd);
		fd = fopen(w1_dev[i].valpath, "r");
		if (!fd){
			BHLOG(LOGOW) printf("    OW: could not open file %s (0x%x) (2. try)\n", w1_dev[i].valpath, fd);
			return -2;
		}
		fgets(w1line, 1024, fd);		// get 1. data  line
		sscanf(w1line,"%x %x %x %x %x %x %x %x %x : crc=%x %s", &i1 ,&i2 ,&i3 ,&i4 ,&i5 ,&i6 ,&i7 ,&i8 ,&i9, &w1_dev[i].crc, s1);		// e.g.: 2d 00 4b 46 ff ff 02 10 19 : crc=19 YES
		if(s1 == "NO") { // again wrong CRC check -> give up
			w1_dev[i].tval=0;
			w1_dev[i].crc=0;
			fclose(fd);
			BHLOG(LOGBH) printf("    OW: ID%i CRC Error 2. time -> give up\n", i);
			return -3;	// leave this entry empty
		}
	}
#endif	
	BHLOG(LOGOW) printf("    OW: ID%i DataLine1: %s", i, w1line);

#ifdef BHSIM
	sprintf(w1line,"1 2 3 4 5 6 7 8 9 t=21321\n");
#else
	fgets(w1line, 1024, fd);		// get 2. data  line
	fclose(fd);
#endif
	sscanf(w1line,"%x %x %x %x %x %x %x %x %x t=%f", &i1 ,&i2 ,&i3 ,&i4 ,&i5 ,&i6 ,&i7 ,&i8 ,&i9, &w1_dev[i].tval);				// e.g.: 2d 00 4b 46 ff ff 02 10 19 t=22625
	BHLOG(LOGOW) printf("    OW: ID%i DataLine2: %s", i, w1line);

	w1_dev[i].tval = w1_dev[i].tval / 1000; 

	BHLOG(LOGOW) printf("    OW: ID%i: %s - CRC:%X - Temp:%3.3f°C\n", i, w1_dev[i].ID,  w1_dev[i].crc,  w1_dev[i].tval);
	
  return 0;
} // end of getw1val()

// end of get1wire.c