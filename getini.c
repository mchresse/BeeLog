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
********************************************************************************
*/

/*******************************************************************************
 * Sub Module GetINI
 * parse BeeLog CONFIG FILE (config.ini) for initial runtime parameters
 * of beehive program
 * 
 * The functions handler(), getini() and setini() are derived from "inih" project
 * and relies on Lincese conditions as listed above by Ben Hoyt (2009)
 *
 * The function newini() is added for BeeLog config.ini initialization in case
 * of absence of any ini file -> relies onder central BeeLog project license
 * conditions as defined in the main() function module by R.Esser(2019)
 *
 *******************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ini.h"
#include "getini.h"

configuration config;	// global default runtime data configuration buffer
						// used by all modules

static int handler(void* inibuf, const char* section, const char* name, const char* value){

	configuration* pconfig = (configuration*) inibuf;
#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

    if (strcmp(name, "VERSION") == 0) {
        strcpy(pconfig->version,value);
    } else 
		if (       MATCH("HWCONFIG", "PINHX711DT")) {
			pconfig->hc_hxdatapin = atoi(value);
		} else if (MATCH("HWCONFIG", "PINHX711CLK")) {
			pconfig->hc_hxclkpin = atoi(value);
		} else if (MATCH("HWCONFIG", "PINADSALERT")) {
			pconfig->hc_adsalertpin = atoi(value);
		} else if (MATCH("HWCONFIG", "PINOWIO")) {
			pconfig->hc_owiopin = atoi(value);
		} else if (MATCH("HWCONFIG", "PINTESTLED")) {
			pconfig->hc_lediopin = atoi(value);
		} else if (MATCH("HWCONFIG", "PINTARABT")) {
			pconfig->hc_taraiopin = atoi(value);
		} else if (MATCH("HWCONFIG", "EPAPERDIN")) {
			pconfig->ep_dinpin = atoi(value);
		} else if (MATCH("HWCONFIG", "EPAPERCLK")) {
			pconfig->ep_clkpin = atoi(value);
		} else if (MATCH("HWCONFIG", "EPAPERCS")) {
			pconfig->ep_cspin = atoi(value);
		} else if (MATCH("HWCONFIG", "EPAPERDC")) {
			pconfig->ep_dcpin = atoi(value);
		} else if (MATCH("HWCONFIG", "EPAPERRST")) {
			pconfig->ep_rstpin = atoi(value);
		} else if (MATCH("HWCONFIG", "EPAPERBUSY")) {
			pconfig->ep_busypin = atoi(value);
		} else if (MATCH("HWCONFIG", "EPAPERKEY1")) {
			pconfig->ep_key1 = atoi(value);
		} else if (MATCH("HWCONFIG", "EPAPERKEY2")) {
			pconfig->ep_key2 = atoi(value);
		} else if (MATCH("HWCONFIG", "EPAPERKEY3")) {
			pconfig->ep_key3 = atoi(value);
		} else if (MATCH("HWCONFIG", "EPAPERKEY4")) {
			pconfig->ep_key4 = atoi(value);
		} else if (MATCH("HWCONFIG", "GPSMOUSE")) {
			pconfig->hc_gpsmouse = atoi(value);
		} else if (MATCH("HWCONFIG", "THERMOCAM")) {
			pconfig->hc_thermocam = atoi(value);
		} else if (MATCH("HWCONFIG", "THERMOLOOP")) {
			pconfig->hc_thermoloop = atoi(value);
		} else if (MATCH("HWCONFIG", "BEECOUNTER")) {
			pconfig->hc_beecounter = atol(value);
		} else if (MATCH("HWCONFIG", "DISPLAY")) {
			pconfig->hc_display = atoi(value);
		} else if (MATCH("HWCONFIG", "WEBUI")) {
			pconfig->hc_webui = atoi(value);
	} else 
		if (       MATCH("BEELOG", "BHLOOPWAIT")) {
			pconfig->bh_loopwait = atoi(value);
		} else if (MATCH("BEELOG", "BEELOGHOME")) {
			strcpy(pconfig->bh_home,value);
		} else if (MATCH("BEELOG", "LOGFILE")) {
			strcpy(pconfig->bh_LOGFILE,value);
		} else if (MATCH("BEELOG", "CSVFILE")) {
			strcpy(pconfig->bh_CSVFILE,value);
		} else if (MATCH("BEELOG", "CSVDAYS")) {
			strcpy(pconfig->bh_CSVDAYS,value);	
		} else if (MATCH("BEELOG", "ACTIOFAIL")) {
			pconfig->bh_actionIOfailure = atoi(value);
		} else if (MATCH("BEELOG", "VERBOSE")) {
			pconfig->bh_verbose = atoi(value);
    } else 
		if (       MATCH("HX711", "TARA")) {
			pconfig->hxtara = atol(value);
		} else if (MATCH("HX711", "TARARESLOCK")) {
			pconfig->hxtaralock = atoi(value);
		} else if (MATCH("HX711", "TARARESET")) {
			pconfig->hxtarareset = atoi(value);
		} else if (MATCH("HX711", "REFKG")) {
			pconfig->hxrefkg = atol(value);
		} else if (MATCH("HX711", "HXSPREAD")) {
			pconfig->hxspread = atof(value);
		} else if (MATCH("HX711", "DATSPREAD")) {
			pconfig->datspread = atof(value);
		} else if (MATCH("HX711", "TEMPCOMP")) {
			pconfig->hxtempcomp = atof(value);
		} else if (MATCH("HX711", "NSAMPLES")) {
			pconfig->hxnsamples = atoi(value);
    } else 
		if (       MATCH("ONEWIRE", "OWFILEPATH")) {
			strcpy(pconfig->owfspath,value);
		} else if (MATCH("ONEWIRE", "TEMPCINT")) {
			pconfig->owtcint = atof(value);
		} else if (MATCH("ONEWIRE", "TEMPCEXT")) {
			pconfig->owtcext = atof(value);
		} else if (MATCH("ONEWIRE", "TEMPCHIVE1")) {
			pconfig->owtchive1 = atof(value);
		} else if (MATCH("ONEWIRE", "TEMPCHIVE2")) {
			pconfig->owtchive2 = atof(value);
		} else if (MATCH("ONEWIRE", "OWTEMPINTID")) {
			strcpy(pconfig->owtempintid,value);
		} else if (MATCH("ONEWIRE", "OWTEMPEXTID")) {
			strcpy(pconfig->owtempextid,value);
		} else if (MATCH("ONEWIRE", "OWTEMPHIVEID1")) {
			strcpy(pconfig->owtemphive1id,value);
		} else if (MATCH("ONEWIRE", "OWTEMPHIVEID2")) {
			strcpy(pconfig->owtemphive2id,value);
    } else 
		if (       MATCH("WEBUI", "BEELOGWEB")) {
			strcpy(pconfig->web_root,value);
		} else if (MATCH("WEBUI", "BEEKEEPER")) {
			strcpy(pconfig->web_beekeeper,value);
		} else if (MATCH("WEBUI", "LOCDAT1")) {
			strcpy(pconfig->web_locdat1,value);
		} else if (MATCH("WEBUI", "LOCDAT2")) {
			strcpy(pconfig->web_locdat2,value);
		} else if (MATCH("WEBUI", "LOCPLZ")) {
			pconfig->web_locplz = atoi(value);
		} else if (MATCH("WEBUI", "LOCDAT3")) {
			strcpy(pconfig->web_locdat3,value);
		} else if (MATCH("WEBUI", "PICSMALL")) {
			strcpy(pconfig->web_picsmall,value);
		} else if (MATCH("WEBUI", "PICLARGE")) {
			strcpy(pconfig->web_piclarge,value);
		} else if (MATCH("WEBUI", "AUTOUPDATE")) {
			pconfig->web_autoupdate = atoi(value);
		} else if (MATCH("WEBUI", "WEBDEFFILE")) {
			strcpy(pconfig->web_deffile	,value);
		} else if (MATCH("WEBUI", "NOTICEFILE")) {
			strcpy(pconfig->web_noticefile,value);
		} else if (MATCH("WEBUI", "ALARMON")) {
			pconfig->web_alarm_on = atoi(value);
		} else if (MATCH("WEBUI", "ALARMWEIGHT")) {
			pconfig->web_alarm_weight = atoi(value);
		} else if (MATCH("WEBUI", "ALARMSWARM")) {
			pconfig->web_alarm_swarm = atoi(value);
		} else if (MATCH("WEBUI", "ALARMBATT1")) {
			pconfig->web_alarm_batt1 = atoi(value);
		} else if (MATCH("WEBUI", "ALARMBATT2")) {
			pconfig->web_alarm_batt2 = atoi(value);
    } else 	
		if (       MATCH("EXPORT", "EXFTPURL")) {
			strcpy(pconfig->exp_ftpurl,value);
		} else if (MATCH("EXPORT", "EXFTPPORT")) {
			strcpy(pconfig->exp_ftpport,value);
		} else if (MATCH("EXPORT", "EXFTPPATH")) {
			strcpy(pconfig->exp_ftppath,value);
		} else if (MATCH("EXPORT", "EXFTPPROXY")) {
			strcpy(pconfig->exp_ftpproxy,value);
		} else if (MATCH("EXPORT", "EXFTPPROXYPORT")) {
			strcpy(pconfig->exp_ftpproxyport,value);
		} else if (MATCH("EXPORT", "EXFTPUSER")) {
			strcpy(pconfig->exp_ftpuser,value);
		} else if (MATCH("EXPORT", "EXBKUPPATH")) {
			strcpy(pconfig->exp_bkuppath,value);
		} else if (MATCH("EXPORT", "BKUPFILE")) {
			strcpy(pconfig->exp_bkupfile,value);
    } else{
        return 0;  // unknown section/name, skip
    }
    return 1;
}

/*******************************************************************************
* Function: getini()
* Creation: 01.11.2017
* 
* open Beehive runtime config file (as defined by inifile string) 
* and parse INIT key/value pairs into resulting configuraion struct
*
* Input :
* inifile	string of beehive config file in ini format
*
* Global:	
*	config	struct of runtime config data 'configuration'
*
* Output:
*	*config	pointer on globale config data struct: 'config'
*********************************************************************************
*/
configuration* getini(char* inifile){
	
    if (ini_parse(inifile, handler, &config) < 0) {
        printf("    GetINI: Can't load user cfgfile: %s\n", inifile);
		if(!newini(inifile)){	// creation of new config file works ?
			printf("    GetINI: Can't create new cfgfile at %s - Giving Up !\n", inifile);
			return (NULL);				// No -> we give up
		}
		printf("    GetINI: new INI File created at %s\n", inifile);
		printf("\n    Bitte %s File individuell konfigurieren vor BEEHIVE Neustart !\n", inifile);
		exit(EXIT_FAILURE);
	}
    return (&config);
}

/*******************************************************************************
* Function: SetINI()
* Creation: 01.11.2017
* 
* Scan config.ini file of Beehive and insert given insertline at given 
* section and key-name 
* 
* Input :
* inifile		name of original config file
* inifilebuf	name of temporary copy file for expansion (just to assure its unique name)
* section		section name for insertion
* name			name of new Key, whose line is to be replaced
* insertline	new line string
*
* Global:	-
*
* Output:
*	rc		0 all done as expected
*			-1	inifile could npt be opened in read mode
*			-2	Iniflebuf could not be opened in write mode
*			-3	section name not finished by ']' -> inconsistent ini file
*			-4	key 'name' not found in inifile
*********************************************************************************
*/
int setini(char *inifile, char * inifilebuf, char * section, char * name, char * insertline){
long Fin;
long lsize;
long int lstart;		// point of insert line start
long int lend;			// point of insert line end
long int cur;			// current file pointer position

char dumbuf[INI_MAX_LINE+1];
char *	buffer;
size_t	result;
char	ch;

FILE*	fd;			// FD = original file; fd2 = file buffer for inserts
FILE*	fd2;
int		error;

    fd = fopen(inifile, "r");
    if (!fd)
        return -1;
    fd2 = fopen(inifilebuf, "w");
    if (!fd2)
        return -2;
    
	fseek (fd, 0L, SEEK_END);
	Fin = ftell(fd);					//  get the orig file size
	fseek(fd, 0L, SEEK_SET);			// go back to start of file
	
	// Search stream by parameter and return position of first character in file
	// lastart == 0(key found), -3 or -4 (key not found)
	lstart = ini_search_file(fd, section, name);	// search parameter in file
	if(lstart <0)
		return lstart;	// parameter not found: rc= -3 (no ']'found)
						// or -4 (key not found)

	// lstart delivers start pos of line; rd pointer is already at end of line
	lend = ftell(fd);					// and save this position
	lsize = Fin - lend;

	// copy start block to buffer file
	fseek(fd, 0L, SEEK_SET);
	while (1) {
      ch = fgetc(fd);
	  cur = ftell(fd);
	  
      if (ch == EOF || cur == lstart)
         break;
      else
         putc(ch, fd2);
	}
	fputs ("\n", fd2);
	fputs(insertline, fd2);				//insert value string

	fseek(fd, lend, SEEK_SET);			// go behind insert point
	
	// copy end block to buffer file
	while (lsize) {
      ch = fgetc(fd);
      if (ch == EOF)
         break;
      else{
         putc(ch, fd2);
		 lsize--;
	  }
	}
	fflush(fd2);
    fclose(fd);
	fclose(fd2);
	
	return 0;							// Done: string inserted
}

/*******************************************************************************
* Function: NewINI()
* Creation: 01.11.2017
* Author: R.Esser, 2019
* Creat new config.ini file of Beehive by default parameters
* !!! any format change of config ini requires change of this file as well !!!
*
* Input :
* inifile		name of default beehive config file
*
* Global:	-
*
* Output:
*	rc		NULL  no file coulf be created
*			ptr on global initialized configuration struct
*********************************************************************************
*/
configuration* newini(char* inifile){
#define	NEWCFGLINEMAX	100
#define CFGLINELEN		256
// allocate space for char pointers to strings
char * cfgline[NEWCFGLINEMAX];
int i = 0;
FILE *	bhcfg;

// allocate space for each cfgline string
    for(i = 0; i < NEWCFGLINEMAX; i++){
        cfgline[i] = (char*)malloc(CFGLINELEN * sizeof(char));
		*cfgline[i] = 0;	//reset all cfgline buffers
    }

	if(!(bhcfg = fopen(inifile, "w"))){
		//----- cfg file could not be created -----	
		return(NULL);
	}	// else new file created

	strcpy(cfgline[0],  "; BeeHive Logger config file for initialization\n");
	strcpy(cfgline[1],  "; 03.01.2019\n");
	strcpy(cfgline[2],  "VERSION = '2.2'\n");
	strcpy(cfgline[3],  "; Beehive tool configuration runtime parameters\n");
	strcpy(cfgline[4],  "; This file can be modified during runtime of beehive !\n");
	strcpy(cfgline[5],  "; All Values are parsed cyclic during runtime right after the wait loop.\n");
	strcpy(cfgline[6],  "\n");

	strcpy(cfgline[7],  "[HWCONFIG]                 ; wPi-GPIO configuration settings (BCM GPIO#)\n");
	strcpy(cfgline[8],  "PINHX711DT = 23            ; J8.P16: HX711 DT  dataline\n");
	strcpy(cfgline[9],  "PINHX711CLK= 20            ; J8.P38: HX711 SCK clockline\n");
	strcpy(cfgline[10], "PINADSALERT= 22            ; J8.P15: ADS ALERT/RDY GPIO22\n");
	strcpy(cfgline[11], "PINTESTLED = 12            ; J8.P32: TEST LED (red) /w ext. pullup\n");
	strcpy(cfgline[12], "PINTARABT  = 16            ; J8.P36; TARA Reset ISR trigger Button\n");
	strcpy(cfgline[13], "PINOWIOPIN = 4             ; J8.P07: OneWire dataline -> claimed by kernel cfg files => do not change !\n");
	strcpy(cfgline[14], "	; Display: WaveShare 2.7'' ePaper HAT (DISPLAY =1) SPI0 => wPi setup with GPIO#\n");
	strcpy(cfgline[15], "EPAPERDIN  = 10            ; J8.P19 SPI0-MOSI GPIO10\n");
	strcpy(cfgline[16], "EPAPERCLK  = 11            ; J8.P23 SPI0-SCLK GPIO11\n");
	strcpy(cfgline[17], "EPAPERCS   = 8             ; J8.P24 SPI0-CE0  GPIO08\n");
	strcpy(cfgline[18], "EPAPERDC   = 25            ; J8.P22 GPIO25\n");
	strcpy(cfgline[19], "EPAPERRST  = 17            ; J8.P11 GPIO17\n");
	strcpy(cfgline[20], "EPAPERBUSY = 24            ; J8.P18 GPIO24\n");
	strcpy(cfgline[21], "EPAPERKEY1 = 5             ; J8.P29 GPIO05\n");
	strcpy(cfgline[22], "EPAPERKEY2 = 6             ; J8.P31 GPIO06\n");
	strcpy(cfgline[23], "EPAPERKEY3 = 13            ; J8.P33 GPIO13\n");
	strcpy(cfgline[24], "EPAPERKEY4 = 26            ; J8.P37 GPIO26\n");
	strcpy(cfgline[25], "	; Component enabler\n");
	strcpy(cfgline[26], "GPSMOUSE   = 0             ; =1 GPS mouse connected -> part of logdata\n");
	strcpy(cfgline[27], "THERMOCAM  = 0             ; =1 Thermo camera connected >0: save pics each x min.\n");
	strcpy(cfgline[28], "THERMOLOOP = 10            ; Wait time for thermo pic creation in minutes\n");
	strcpy(cfgline[29], "BEECOUNTER = 0             ; =1 BeeCounter connected -> part of logdata\n");
	strcpy(cfgline[30], "DISPLAY    = 1             ; =1 Activate local display update => =1 for WaveShare ePaper 2.7\n");
	strcpy(cfgline[31], "WEBUI      = 1             ; =1 Activate Webpage date preparation at BEELOGWEB\n");

	strcpy(cfgline[32], "[BEELOG]   ; Init of Main Loop\n");
	strcpy(cfgline[33], "BHLOOPWAIT = 600           ; loop wait time in Sec. (600 = 10Min.)\n");
	strcpy(cfgline[34], "BEELOGHOME = /home/pi/share/beelog  ; Home path for beelog housekeeping data\n");
	strcpy(cfgline[35], "LOGFILE    = beelog.txt    ; log file name (/w extension)\n");
	strcpy(cfgline[36], "CSVFILE    = beelog        ; *.csv data log file name (/wo extension)\n");
	strcpy(cfgline[37], "CSVDAYS    = beedays       ; *.csv file of daily statistic summary (/wo extension)\n");
	strcpy(cfgline[38], "ACTIOFAIL  = 1             ; allowed action on IO Error: 0= no Action, 1= exit, 2=reboot\n");
	strcpy(cfgline[39], "VERBOSE    = 1             ; verbose levels +1:main flow + 2=1-wire + 4+8=hx711(lev 1+2)\n");
	strcpy(cfgline[40], "                           ; 16=Import/export + 32=Web Update + 64=raspimon\n");
	strcpy(cfgline[41], "                           ; +128 = statistic data calculation\n");
	strcpy(cfgline[42], "[HX711]    ; Init of Weight scale ADC\n");
	strcpy(cfgline[43], "TARA       = 823500        ; Calibration for 0 kg\n");
	strcpy(cfgline[44], "TARARESLOCK= 0             ; =0 TARA reset button is disabled, =1 enabled\n");  
	strcpy(cfgline[45], "TARARESET  = 0             ; =0 TARA Button active; =1 TARA Button simulated\n");
	strcpy(cfgline[46], "REFKG      = 46350         ; weight scale reference value of 1kg\n");
	strcpy(cfgline[47], "HXSPREAD   = 10            ; trust range of weight scale +/- 5% = 10 per measurement\n");
	strcpy(cfgline[48], "DATSPREAD  = 10            ; trust range of weight scale +/- 5% = 10 between 2 measurements\n");
	strcpy(cfgline[49], "TEMPCOMP   = 0.9           ; Temp. compensation factor per Grad\n");
	strcpy(cfgline[50], "NSAMPLES   = 10            ; Number of read loops for average calculation\n"); 
	strcpy(cfgline[51], "                           ; Max Range: 2..100\n");
	strcpy(cfgline[52], "\n");

	strcpy(cfgline[53], "[ONEWIRE]  ; Init of One-Wire devices\n");
	strcpy(cfgline[54], "OWFILEPATH  = /sys/bus/w1/devices ; path to one wire filesystem of kernel driver\n");
	strcpy(cfgline[55], "OWTEMPEXTID  = 28-051670d062ff  ; OW ID of external DS18B20 sensor 1\n");
	strcpy(cfgline[56], "TEMPCEXT     = 1.00             ; temperature compensation External sensor\n");
	strcpy(cfgline[57], "OWTEMPINTID  = 28-041670f10bff  ; OW ID of internal DS18B20 sensor 2 -> used as beelog UID\n");
	strcpy(cfgline[58], "TEMPCINT     = 0.00             ; temperature compensation Internal sensor\n");
	strcpy(cfgline[59], "OWTEMPHIVEID1= 28-0213180089aa  ; OW ID of  WScale1 DS18B20 sensor 3\n");
	strcpy(cfgline[60], "TEMPCHIVE1   = 0.00             ; temperature compensation Hive1 sensor\n");
	strcpy(cfgline[61], "OWTEMPHIVEID2= 28-012345678900  ; OW ID of  WScale2 DS18B20 sensor 4\n");
	strcpy(cfgline[62], "TEMPCHIVE2   = 0.00             ; temperature compensation Hive2 sensor\n");
	strcpy(cfgline[63], "\n");

	strcpy(cfgline[64], "[WEBUI]\n");
	strcpy(cfgline[65], "BEELOGWEB   = /var/www/beelog   ; root path to webserver home of beelog for log & data files\n");
	strcpy(cfgline[66], "BEEKEEPER   = 'UserName'        ; Full name of Owner/User/BeeKeeper\n");
	strcpy(cfgline[67], "LOCDAT1     = '-Garten-'        ; Location of BeeHive1\n");
	strcpy(cfgline[68], "LOCDAT2     = 'Strasse'         ; Street\n");
	strcpy(cfgline[69], "LOCPLZ      = 'PLZ'             ; ZIP code of location (also fro weather data from web)\n");
	strcpy(cfgline[70], "LOCDAT3     = 'Ort'             ; location name\n");
	strcpy(cfgline[71], "PICSMALL    = BeeLog_Picture_compressed.jpg ; Pic of BeeHive (compressed) used as WebLogo\n");
	strcpy(cfgline[72], "PICLARGE    = BeeLog_Picture.jpg ; Pix of Beehive full size\n");
	strcpy(cfgline[73], "AUTOUPDATE  = 0                 ; init automatic update of website\n");
	strcpy(cfgline[74], "WEBDEFFILE  = index.html        ; default Web index file to be updated\n");
	strcpy(cfgline[75], "NOTICEFILE  = beenote.txt       ; text file of service notices for logging\n");
	strcpy(cfgline[76], "ALARMON     = 0                 ; =1 Global 'ALARM enabled' for security/events\n");
	strcpy(cfgline[77], "ALARMWEIGHT = 0                 ; Alarm on Weight change > 50% in 5 seconds: thieve\n");
	strcpy(cfgline[78], "                                ; =0 disabled, 1..99% enabled, typical 50(%)\n");
	strcpy(cfgline[79], "ALARMSWARM  = 0                 ; Alarm on weight change > 10% in 10 minutes: swarm\n");
	strcpy(cfgline[80], "                                ; =0 disabled, 1..99% enabled, typical 10(%)\n");
	strcpy(cfgline[81], "ALARMBATT1  = 0                 ; =0 disabled; 1..99% enabled, typical 98(%)= 4.9V\n");
	strcpy(cfgline[82], "ALARMBATT2  = 0                 ; =0 disabled; 1..99% enabled, typical 95(%)= 4.75V\n");
	strcpy(cfgline[83], "\n");

	strcpy(cfgline[84], "[EXPORT]\n");
	strcpy(cfgline[85], "EXFTPURL    = <ftp URL>         ; FTP site URL for upload of raw logger data from BEELOGWEB\n");
	strcpy(cfgline[86], "EXFTPPORT   = 21                ; Portnumber of URL (used as string)\n");
	strcpy(cfgline[87], "EXFTPPATH   = imkerei/beelog    ; relative FTP path to URL\n");
	strcpy(cfgline[88], "EXFTPPROXY  =                   ; If needed: FTP proxy server; set '' if no proxy needed\n");
	strcpy(cfgline[89], "EXFTPPROXYPORT =                ; used proxy port (used as string)\n");
	strcpy(cfgline[90], "EXFTPUSER   =                   ;no user name for FTP access (get pwd by dialogue or local .netrc file)\n");
	strcpy(cfgline[91], "BKUPPATH    = /home/pi/share/beelog ; Backup file path (local or remote)\n");
	strcpy(cfgline[92], "BKUPFILE    = beelog.bak            ; name of config/log backup file at BKUPPATH\n");
	
	for (i=0; i<NEWCFGLINEMAX; i++){	// write all cfgline buffer to new cfg file
		fwrite(cfgline[i],sizeof(unsigned char), strlen(cfgline[i]), bhcfg) ;
		free(cfgline[i]);				// release string buffer of cfgline
	}
	
	fflush(bhcfg);
	fclose(bhcfg);
	return(&config);
} // endof newini()