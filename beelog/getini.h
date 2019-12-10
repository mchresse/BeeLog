/*
* The "inih" library is distributed under the New BSD license:
*
* Copyright (c) 2009, Ben Hoyt
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*    * Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*    * Redistributions in binary form must reproduce the above copyright
*      notice, this list of conditions and the following disclaimer in the
*      documentation and/or other materials provided with the distribution.
*    * Neither the name of Ben Hoyt nor the names of its contributors
*      may be used to endorse or promote products derived from this software
*      without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY BEN HOYT ''AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL BEN HOYT BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* --- end of LICENSE ---
*
* inih is released under the New BSD license. Go to the project
* home page for more info: https://github.com/benhoyt/inih
*/
/* 
 * File:   getini.h
 * Created on 21. Januar 2017
 */

#ifndef GETINI_H
#define GETINI_H

#ifdef __cplusplus
extern "C" {
#endif

	
// Config file name
#define CONFIGINI	"./config.ini"
#define CONFIGINI2	"./config.ini~"
#define LOGFILELEN	255		// length of LOG file NAME
#define VERSIONLEN	10		// length of Config.ini version string
#define PATHLEN		1024	// length of Home path string
#define OWIDLEN		20		// length of OneWire sensor IDs

// structure for config ini file parameters parsed by getini()
typedef struct{
    unsigned char version[VERSIONLEN];			// version of ini file
	// section HWCONFIG
	int		hc_hxdatapin;	//BCM GPIO pin (for piWire-lib) of data line connection
	int		hc_hxclkpin;
	int		hc_adsalertpin;
	int		hc_lediopin;
	int		hc_taraiopin;
	int		hc_owiopin;		//BCM GPIO pin (for piWire-lib) of data line connection
	int		ep_dinpin;		// SPI0 MOSI
	int		ep_clkpin;		// SPI0 SCLK
	int		ep_cspin;		// SPI0 CE0
	int		ep_dcpin;
	int		ep_rstpin;		// RST
	int		ep_busypin;		// BUSY
	int		ep_key1;
	int		ep_key2;
	int		ep_key3;
	int		ep_key4;	
	// component enabler flags
	int		hc_gpsmouse;
	int		hc_thermocam;
	int		hc_thermoloop;
	long	hc_beecounter;
	int		hc_display;		// like local epaper hat
	int     hc_webui;		// webpage at BEELOGWEB
	// section BeeLog
	int		bh_loopwait;
	char	bh_home[PATHLEN];
	char	bh_LOGFILE[LOGFILELEN];
	char	bh_CSVFILE[LOGFILELEN];
	char	bh_CSVDAYS[LOGFILELEN];
	int		bh_actionIOfailure;
	int		bh_verbose;
	// section HX711
	long	hxtara;
	int		hxtaralock;
	int		hxtarareset;
	long	hxrefkg;
    float	hxspread;
    float	datspread;
	float	hxtempcomp;
	int		hxnsamples;
	// section OneWire
	char	owfspath[PATHLEN];	// path to one wire data files
	float	owtcint;			// temp. correction internal sensor
	float	owtcext;			// temp. compensation external sensor
	float	owtchive1;			// temp compensation hive sensor 1
	float	owtchive2;			// temp compensation hive sensor 2
	char	owtempintid[OWIDLEN];	// Internal Temp. Sensor ID
	char	owtempextid[OWIDLEN];	// External Temp. Sensor ID
	char	owtemphive1id[OWIDLEN];	// Ext. Hive1 Temp. Sensor ID
	char	owtemphive2id[OWIDLEN];	// Ext. Hive2 Temp. Sensor ID
	// section WEBUI
	char	web_root[PATHLEN];
	char	web_beekeeper[LOGFILELEN];
	char	web_locdat1[LOGFILELEN];
	char	web_locdat2[LOGFILELEN];
	char	web_locdat3[LOGFILELEN];
	int		web_locplz;				// PLZ for weather data from web
	char	web_picsmall[LOGFILELEN];
	char	web_piclarge[LOGFILELEN];
	char	web_noticefile[LOGFILELEN];
	int		web_autoupdate;
	char	web_deffile[LOGFILELEN];	// default web page file for update
	int		web_alarm_on;
	int		web_alarm_weight;
	int		web_alarm_swarm;
	int		web_alarm_batt1;	// =0 disabled; 1..99% enabled, typical 98%
	int		web_alarm_batt2;	// =0 disabled; 1..99% enabled, typical 95%
	// section EXPORT
	char	exp_ftpurl[PATHLEN];
	char	exp_ftpport[10];
	char	exp_ftppath[PATHLEN];
	char	exp_ftpproxy[PATHLEN];
	char	exp_ftpproxyport[10];
	char	exp_ftpuser[LOGFILELEN];
	char	exp_bkuppath[PATHLEN];
	char	exp_bkupfile[LOGFILELEN];
	
} configuration;


configuration * getini(char* inifile);
configuration * newini(char* inifile);
int setini(char *inifile, char * inifilebuf, char * section, char * name, char * insertline);

#ifdef __cplusplus
}
#endif

#endif /* GETINI_H */

