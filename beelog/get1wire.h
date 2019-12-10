/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*******************************************************************************
 * File:	get1wire.h
 * Author:	Randolph Esser - Copyright 2017-2019
 * Created on 15. January 2017, 09:40
 * This file is part of the "beelog" program/project.
 *
 * Defines BeeHive 1-wire connection modules
 *
 * Recommended connection (default by Wheezy kernel driver)
 *
 *	 1-wire sensor  | Raspberry Pin | wiringPi
 *   ---------------|---------------|---------
 *       DATA       |  P07 - GPIO4  | -> 7
 *	     3.3V       |  P01 - 3.3V   |
 *	     GND        |  P09 - GND    | 
 *
 +-----+-----+---------+------+---+---Pi 2---+---+------+---------+-----+-----+
 | BCM | wPi |   Name  | Mode | V | Physical | V | Mode | Name    | wPi | BCM |
 +-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+
 |     |     |    3.3v |      |   |  1 || 2  |   |      | 5v      |     |     |
 |   2 |   8 |   SDA.1 |   IN | 1 |  3 || 4  |   |      | 5V      |     |     |
 |   3 |   9 |   SCL.1 |   IN | 1 |  5 || 6  |   |      | GND     |     |     |
 |   4 |   7 | GPIO.04 |   IN | 1 |  7 || 8  | 1 | ALT0 | TxD     | 15  | 14  |
 |     |     |     GND |      |   |  9 || 10 | 1 | ALT0 | RxD     | 16  | 15  |
 |  17 |   0 | GPIO.17 |   IN | 0 | 11 || 12 | 0 | IN   | GPIO.18 | 1   | 18  |
 |  27 |   2 | GPIO.27 |   IN | 0 | 13 || 14 |   |      | GND     |     |     |
 |  22 |   3 | GPIO.22 |   IN | 0 | 15 || 16 | 0 | IN   | GPIO.23 | 4   | 23  |
 |     |     |    3.3v |      |   | 17 || 18 | 0 | IN   | GPIO.24 | 5   | 24  |
 |  10 |  12 |    MOSI |   IN | 0 | 19 || 20 |   |      | GND     |     |     |
 |   9 |  13 |    MISO |   IN | 0 | 21 || 22 | 0 | IN   | GPIO.25 | 6   | 25  |
 |  11 |  14 |    SCLK |   IN | 0 | 23 || 24 | 1 | IN   | CE0     | 10  | 8   |
 |     |     |     GND |      |   | 25 || 26 | 1 | IN   | CE1     | 11  | 7   |
 |   0 |  30 |   SDA.0 |   IN | 1 | 27 || 28 | 1 | IN   | SCL.0   | 31  | 1   |
 |   5 |  21 | GPIO.05 |   IN | 1 | 29 || 30 |   |      | GND     |     |     |
 |   6 |  22 | GPIO.06 |   IN | 1 | 31 || 32 | 0 | IN   | GPIO.12 | 26  | 12  |
 |  13 |  23 | GPIO.13 |  OUT | 0 | 33 || 34 |   |      | GND     |     |     |
 |  19 |  24 | GPIO.19 |   IN | 1 | 35 || 36 | 0 | IN   | GPIO.16 | 27  | 16  |
 |  26 |  25 | GPIO.26 |   IN | 0 | 37 || 38 | 0 | IN   | GPIO.20 | 28  | 20  |
 |     |     |     GND |      |   | 39 || 40 | 0 | IN   | GPIO.21 | 29  | 21  |
 +-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+
 | BCM | wPi |   Name  | Mode | V | Physical | V | Mode | Name    | wPi | BCM |
 +-----+-----+---------+------+---+---Pi 2/3-+---+------+---------+-----+-----+
*******************************************************************************
*/

#ifndef GET1WIRE_H
#define GET1WIRE_H

// W1-GPIOPIN  see config.ini =>	GPIO4 J8.P7 default 1-wire connector for Pi2+3
// OW Pin is predefined by kernel driver init files /etc/modeprobe

// "/sys/bus/w1/devices"					//path to 1-wire device nodes
#define W1_IDCOUNT_FILE "w1_bus_master1/w1_master_slave_count"	// file containing W1 slave #
#define W1_IDLIST_FILE  "w1_bus_master1/w1_master_slaves"		// file containing W1 slave IDs
#define W1_FNAME_VAL    "w1_slave"          // W1 device node value file
#define W1_MAX_SLAVES   64                  // Max. number of served W1 slaves at 1 path
#define W1_SLAVE_IDLEN  15                  // Max. W1 slave name lenth
#define W1_SLAVE_PATHLEN 1024				// length of w1 value file path

#define W1_TYPE_DS18B20	28					// W1 Slave sensor type: ID-Byte1=28 -> DS18B20
#define W1_TYPE28_NAME	"DS18B20"
#define W1_TYPE_DS1822	22					// W1 Slave sensor type: ID-Byte1=10 -> DS18S20
#define W1_TYPE22_NAME	"DS1822"
#define W1_TYPE_DS18S20	10					// W1 Slave sensor type: ID-Byte1=10 -> DS18S20
#define W1_TYPE10_NAME	"DS18S20"

typedef struct {
        char    ID[W1_SLAVE_IDLEN+1];			// W1 Slave ID name
		char    type[W1_SLAVE_IDLEN];		// W1 Slave sensor type: ID-Byte1=28 -> "DS18B20"
		char	valpath[W1_SLAVE_PATHLEN];	// W1 Slave Value file path to 'w1_slaves'
        int     crc;						// CRC check code
        float   tval;						// Slave temp value %3.3f
} w1_values;

// functions
w1_values * getw1list(char* );	// discovers W1 devices by ID type 
int			getw1val(int );		// read out slave[i] and fill slave value struct

#endif /* GET1WIRE_H */
