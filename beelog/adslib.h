/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/******************************************************************************* 
 * File:   adslib.h
 * Author: Randolph Esser - Copyright 2017-2019
 * Created on 12. Januar 2019, 23:18
 *
 * This file is part of the "beelog" program/project.
 *******************************************************************************
 */

#ifndef ADSLIB_H
#define ADSLIB_H

// Connect ADDR to GRD.
// Note PCF8591 defaults to 0x48!
#define ADSI2CDEV	"/dev/i2c-1"	//  on /dev/i2c-1 the default on Raspberry Pi B
#define ADSI2CADR	0x48

// Refer to page 19 area of ADS1115 data spec sheet
// Register Selection:
#define ADS_REGSELDAT	0b00000000	// read Data Conversion Register
#define ADS_REGSELCFG	0b00000001	// write Configuration Register
#define ADS_REGSELTHLO	0b00000010	// Threshold Low Register
#define ADS_REGSELTHHI	0b00000011	// Threshold High Register

// Confguration Register: Byte - high
  // bit 15 flag bit for single shot conversion
  // Bits 14-12 Analog Input selection port + mode (diff/abs)
  // Bits 11-9 Power Amplifier Gaining - default to 010 -> +-4.096V
  // Bit 8 Operational conversion mode of the ADS1115:
  //		0 : Continuous conversion mode
  //		1 : Power-down single-shot mode (default)
#define ADS_CFG1SHOT	0b10000000	// Single Shot Conversion
#define ADS_DIFF0TO1	0b00000000	// AIN0 <> AIN1
#define ADS_DIFF0TO3	0b00010000	// AIN0 <> AIN3
#define ADS_DIFF1TO3	0b00100000	// AIN1 <> AIN3
#define ADS_DIFF2TO3	0b00110000	// AIN2 <> AIN3
#define ADS_ABSAIN0		0b01000000	// AIN0 to Gnd
#define ADS_ABSAIN1		0b01010000	// AIN1 to Gnd
#define ADS_ABSAIN2		0b01100000	// AIN2 to Gnd
#define ADS_ABSAIN3		0b01110000	// AIN3 to Gnd
#define ADS_PGA6144V	0b00000000	// FS = +-6.144V (not useful)
#define ADS_PGA4096V	0b00000010	// FS = +-4.096V (Default for VDD:5V)
									// VPS = 4.096 / 32768(15Bit) = 125 uV/S
#define ADS_PGA2048V	0b00000100	// FS = +-2.048V (Default for VDD:3.3V)
#define ADS_PGA1024V	0b00000110	// FS = +-1.024V
#define ADS_MODECCV		0b00000000	// Mode Sel = Continuos Conversion
#define ADS_MODESSHOT	0b00000001	// Mode Sel = SingleShot (default)

// Confguration Register: Byte - low
#define ADS_DRATE8		0b00000000	// DataRate: 8SPS
#define ADS_DRATE16		0b00100000	// DataRate: 8SPS
#define ADS_DRATE32		0b01000000	// DataRate: 8SPS
#define ADS_DRATE64		0b01100000	// DataRate: 8SPS
#define ADS_DRATE128	0b10000000	// DataRate: 8SPS
#define ADS_DRATE250	0b10100000	// DataRate: 8SPS
#define ADS_DRATE475	0b11000000	// DataRate: 8SPS
#define ADS_DRATE860	0b11100000	// DataRate: 8SPS

#define ADS_COMPHYST	0b00000000	// CompMode: Hysterese
#define ADS_COMPWINDOW	0b00010000	// CompMode: WIndow
									// using LoThresh & HiThresh Register
#define ADS_ALERTLOW	0b00000000	// ALERT/RDY Active: Low level
#define ADS_ALERTHIGH	0b00001000	// ALERT/RDY Active: High level
#define ADS_ALERTALT	0b00000000	// ALERT/RDY Toggles at each conversions
#define ADS_ALERTSTATIC	0b00000100	// ALERT/RDY Static for all conversions
#define ADS_ALERTS1		0b00000000	// ALERT/RDY Active after 1 Edge pass
#define ADS_ALERTS2		0b00000001	// ALERT/RDY Active after 2 Edge passes
#define ADS_ALERTS4		0b00000010	// ALERT/RDY Active after 4 Edge passes
#define ADS_NOALERT		0b00000011	// ALERT/RDY >inactive on high level

int16_t readads	(uint8_t portmode);
void	printb	(long a,int i);		// print long/int in binary digits.
int		ads_init(int i2cadr );


#endif /* ADSLIB_H */

