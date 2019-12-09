/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*******************************************************************************
 * File:   hx711.h
 * Author: Randolph Esser - Copyright 2017-2019
 * Created on 07. Januar 2017, 10:09
 * This file is part of the "beelog" program/project.
 *
 * Defines of HX711 module connections and initialisation values
 *******************************************************************************
 */

/*
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

*/

#ifndef HX711_H
#define HX711_H

#include "getini.h"


#define TESTLED_ON		0		// set TESTLEDPIN =0 -> LED ON
#define TESTLED_OFF		1		// set TESTLEDPIN =1 -> LED OFF

// Weight scale metrics
#define MAX_N_SAMPLES   100		// number of max. weight scale measures
#define	CALTEMP			21.00	// calibration temperature

// HX711 Pin control macros
#define SCK_ON  digitalWrite(cfgini->hc_hxclkpin,1)	// set HX711 module SCK pin = 1
#define SCK_OFF digitalWrite(cfgini->hc_hxclkpin,0)	// set HX711 module SCK pin = 0
#define DT_R    digitalRead(cfgini->hc_hxdatapin)	// read HX711 module DT pin

 
  long	offset;					// store adapted offset

// functions in hx711.c
int				inithx711	(int hxidx, int channel);
float			gethx711	(long offset, float TempExt, int hxidx, int channel);
float			gettcomp	(float exttemp);
long			getaverage	(int nsamples);
void			setHighPri	(void);
void			setup_gpio	();
void			unpull_pins	();
void			reset_converter(void);
void			set_HX711gain(int r);
unsigned long	read_cnt	(int channel);

#endif /* HX711_H */
