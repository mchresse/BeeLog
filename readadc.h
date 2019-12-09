/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/******************************************************************************* 
 * File:   readadc.h
 * Author: Randolph Esser - Copyright 2017-2019
 * Created on 18. März 2018, 13:14
 * This file is part of the "beelog" program/project.
 *******************************************************************************
 */ 
 
#ifndef READADC_H
#define READADC_H

#ifdef __cplusplus
extern "C" {
#endif

// MCP3002 definitions
//
// GPIO SPI port connection => GPIO# ID setup
//pin 24 - SPICLK/Serial Clock
#define ADCCLK		8	// => conflict with EPaper ! t.b.d.
	// better use CE1 at GPIO7
//pin 16 - SPIMISO/Serial Data Output
#define ADCDOUT		23
//pin 18 - SPIMOSI/Serial Data Input
#define ADCDIN		24
//pin 22 - SPICS - Chip Select/Shutdown Input
#define ADCCS		25

	
#define ADCPORTS	8
#define ADCBITS		12
	
//zu messende Spannung liegt auf CH0 - CH1/CH7
#define ADCCH0		0
#define ADCCH1		1

// MCP3008 definitions
//
#define ADCCH2		2
#define ADCCH3		3
#define ADCCH4		4
#define ADCCH5		5
#define ADCCH6		6
#define ADCCH7		7

#define CHAN_CONFIG_SINGLE  8
#define CHAN_CONFIG_DIFF    0


unsigned int readadc2(unsigned char chnum);
int adc_init2();
int adc_init8( int spiload, int spichannel);
int myAnalogRead8(int spichannel, int channelconfig, int analogchannel);
int readadc8( int channel, int spichannel, int chmode);

#ifdef __cplusplus
}
#endif

#endif /* READADC_H */

