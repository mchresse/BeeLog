/******************************************************************************* 
 * File:   readads.c
 * Author: Randolph Esser - Copyright 2017-2019
 * Created on 17. Januar 2017, 10:09
 * This file is part of the "beelog" program/project.
 *
 * Read out ADC of type MCP300x via SPI Ports
 * using WiringPi Lib in GPIO Mode
 *******************************************************************************
 */

// Routines makes use of code from following Freeware:
// by Lewis Loflin lewis@bvu.net www.bristolwatch.com
// -> http://www.bristolwatch.com/rpi/geany/ads1115c.c
// Wiki: http://www.bristolwatch.com/rpi/ads1115.html

/* Global libraries:
 *  This code makes use of WiringPi library in GPIO mode (using orig. GPIO port#)
 * 	 Makes use of WiringPI-library of Gordon Henderson 
 *	 (https://projects.drogon.net/raspberry-pi/wiringpi/)
 *	 Recommended connection (http://www.raspberrypi.org/archives/384):
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>    // read/write usleep
#include <stdlib.h>    // exit function
#include <inttypes.h>  // uint8_t, etc
#include <linux/i2c-dev.h> // I2C bus definitions

#include <wiringPi.h>

#include "adslib.h"
#include "getini.h"
#include "get1wire.h"
#include "beehive.h"


/*******************************************************************************
* Function: ads_init()
* Creation: 01.01.2019
* 
* Test I2C ADS device connection
* 
* Input :
* i2cadr	default: 0x48 (parameter here but hard coded on wire)
*
* Global:	-
*
* Output:
*	rc		=0 ADS device available on selected I2C address
*			=-1	could not open ADS device
*			=-2 could not find device on selected addr.
*********************************************************************************
*/
int ads_init(int i2cadr ) {
int		fd;				// I2c Dev handle	

  // open I2C device
  if ((fd = open(ADSI2CDEV, O_RDWR)) < 0) {
    printf("    ADS Init: Couldn't open device %s (%d) !\n", fd, ADSI2CDEV);
    return(-1);
  }

  // test connection to ADS device at i2cadr
  if (ioctl(fd, I2C_SLAVE, i2cadr) < 0) {
    printf("    ADS Init: Couldn't find device on address 0x%X !\n", i2cadr);
    return (-2);
  }
  close(fd);

  pinMode(cfgini->hc_adsalertpin, INPUT);		// Set ADS1115 ALERT/RDY Pin -> Input

  return(0);
}

/*******************************************************************************
* Function: readads()
* Creation: 01.01.2019
* 
* The resolution of the ADC in single ended mode 
* we have 15 bit rather than 16 bit resolution, 
* the 16th bit being the sign of the differential reading.
* 
* Input :
* portmode	see datasheet Cfg.register:
*				Bits 14-12 Analog Input selection port + mode (diff/abs)
*				e.g. ADS_ABSAIN0 in adslib.h
* Global:	-
*
* Output:
*	rc		>=0 ADS conversion value 16Bit
*			=-1	could not open ADS device
*			=-2 could not find device on selected addr.
*			=-3 Write to control register failed
*			=-4 Write register select failed
*			=-5 Read conversion register failed
*			=-6 Write to control register failed after conversion
*********************************************************************************
*/
int16_t readads(uint8_t portmode) {
int		fd;				// I2c Dev handle	
uint8_t writeBuf[3];
uint8_t readBuf[2];
int16_t val;			// conversion data buffer
float	myfloat;		// final calculated conversion value
	
  // open I2C device
  if ((fd = open(ADSI2CDEV, O_RDWR)) < 0) {
    printf("    readADS Error(-1): Couldn't open device! %d\n", fd);
    return(-1);
  }

  // connect to ADS1115 as i2c slave
  if (ioctl(fd, I2C_SLAVE, ADSI2CADR) < 0) {
    printf("    readADS Error(-2): Couldn't find device on address !\n");
    return (-2);
  }

	// Setup to use ADC0 single ended
	// set config register and start conversion
	//	AIN3 to GND, 2.048v, 128 samples/s
	//	Refer to page 19 area of Data spec sheet

  	writeBuf[0] = ADS_REGSELCFG; // Select config register: 0b00000001;
	// WB-high: 0b11110100 0xF4
	writeBuf[1] = portmode | ADS_CFG1SHOT | ADS_PGA4096V | ADS_MODECCV; 
	// WB-low : 0b10000101 0x85
	writeBuf[2] = ADS_DRATE128 | ADS_ALERTSTATIC | ADS_ALERTS2;

	char buffer[sizeof(long)* 8 + 1];
//	BHLOG(LOGMON) printf("    ADS WR-Buffer 0 : HEX 0x%02X    ",writeBuf[0]); 
//	BHLOG(LOGMON) printb(writeBuf[0],8);printf("\n");
//	BHLOG(LOGMON) printf("    ADS WR-Buffer1+2: HEX 0x%02X%02X  ",writeBuf[1], writeBuf[2]); 
//	BHLOG(LOGMON) printb(writeBuf[1],8);printf(" "); 
//	BHLOG(LOGMON) printb(writeBuf[2],8);printf("\n");
		  
	// begin conversion
	if (write(fd, writeBuf, 3) != 3) {
		perror("   readADS: Write to register failed(-3)");
		return(-3);
	}

	sleep(1);

	// printf("ADS1115 Demo will take %i readings.\n", CONFBURST);

	// set pointer to data register
	readBuf[0] = ADS_REGSELDAT;
	if (write(fd, readBuf, 1) != 1) {
		perror("    readADS: Write register select failed (-4)");
		return(-4);
	}
  
    // read conversion register
    if (read(fd, readBuf, 2) != 2) {	// read 2 Data Byte
		perror("   ReadADS: Read conversion error (-5)");
		return(-5);
    }

	// could also multiply by 256 then add readBuf[1]
	val = readBuf[0] << 8 | readBuf[1];	// correct byte order

	// with +- LSB sometimes generates very low neg number.
	if (val < 0)   
		val = 0;


	BHLOG(LOGMON)	printf("    readADS[%i] Conversion: HEX:0x%02x DEC:%d\n", portmode / 16 -4, val, val);
	// TMP37 20mV per deg C
	//    printf("Temp. C = %4.2f \n", myfloat / 0.02);
	//    printf("Temp. F = %4.2f \n", myfloat / 0.02 * 9 / 5 + 32);

	// power down ADS1115
  	writeBuf[0] = ADS_REGSELCFG; // Select config register: 0b00000001;
	// WB-high: 0b11110100 0xF4 -> bit 15-8 single shot on
  	writeBuf[1] = portmode | ADS_CFG1SHOT | ADS_PGA4096V | ADS_MODESSHOT; 
	// WB-low : 0b10000101 0x85 -> bit 7-0
	writeBuf[2] = ADS_DRATE128 | ADS_ALERTSTATIC | ADS_ALERTS2;

	if (write(fd, writeBuf, 3) != 3) {
		perror("    ReadADS: Write to register error (-6)");
		return(-6);
	}
	close(fd);
  return (val);
}

/*******************************************************************************
* Helper function: print int2binary data (like itob())
* Print int value in binary fomat
* Input	value can be up to 32bit
* num	number ogf printed bits from 1..32
* Author: Randolph Esser, 2019
*******************************************************************************
*/
void printb(long a, int num){
    int b[32];
    int i;

	if (num >32) num = 32; // limit output string
	
    for (i = 0; i<num; i++){                                            
        b[i] = a % 2;      // Bit = 0 v 1 ?
        a >>= 1;           // select next bit at pos 0
    }
    for( i = num-1; i >= 0; --i){
		printf ("%d", b[i]);
	}
}
