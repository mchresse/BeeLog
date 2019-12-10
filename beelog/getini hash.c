/* parse BeeLog CONFIG FILE FOR INITILISATION PARAMETERS */

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "ini.h"
#include "getini.h"

configuration config;	// global default runtime data configuration buffer

// die Hash-Funktion zur Berechnung des Hash-Werts eines Strings
const unsigned long fhash(const char *str) {
    unsigned long hash = 5381;  
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

#define H1(s,i,x)   (x*65599u+(uint8_t)s[(i)<sizeof(s)?sizeof(s)-1-(i):sizeof(s)])
#define H4(s,i,x)   H1(s,i,H1(s,i+1,H1(s,i+2,H1(s,i+3,x))))
#define H16(s,i,x)  H4(s,i,H4(s,i+4,H4(s,i+8,H4(s,i+12,x))))
#define H64(s,i,x)  H16(s,i,H16(s,i+16,H16(s,i+32,H16(s,i+48,x))))
#define H256(s,i,x) H64(s,i,H64(s,i+64,H64(s,i+128,H64(s,i+192,x))))

#define HASH(s)    ((uint32_t)(H256(s,0,0)^(H256(s,0,0)>>16)))

#define HASH2(s)	(s[0]+s[1]+s[2])

static int handler(void* inibuf, const char* section, const char* name, const char* value) {
configuration* pconfig = (configuration*)inibuf;

  switch(*section){
  case HASH2("HWCONFIG"):
	  switch(*name){
	  case 'HX711DT':		pconfig->hc_hxdatapin = atoi(value); break;
	  case 'HX711CLK':	pconfig->hc_hxclkpin = atoi(value); break;
	  case 'OWIOPIN':		pconfig->hc_owiopin = atoi(value); break;
	  case 'TESTLEDPIN':	pconfig->hc_lediopin = atoi(value); break;
	  case 'TARABUTTON':	pconfig->hc_taraiopin = atoi(value); break;
	  case 'GPSMOUSE':		pconfig->hc_gpsmouse = atoi(value); break;
	  case 'THERMOCAM':		pconfig->hc_thermocam = atoi(value); break;
	  case 'THERMOLOOP':	pconfig->hc_thermoloop = atoi(value); break;
	  case 'BEECOUNTER':	pconfig->hc_beecounter = atol(value); break;				  
	  } break;
  case fhash("BEELOG"):
	  switch(name){
	  case fhash("BHLOOPWAIT"):	pconfig->bh_loopwait = atoi(value); break;
	  case fhash("BEELOGHOME"):	strcpy(pconfig->bh_home,value); break;
	  case fhash("LOGFILE"):		strcpy(pconfig->bh_LOGFILE,value); break;
	  case fhash("CSVFILE"):		strcpy(pconfig->bh_CSVFILE,value); break;
	  case fhash("VERBOSE"):		pconfig->bh_verbose = atoi(value); break;
	  } break;
  case fhash("HX711"):
	  switch(name){
	  case fhash("TARA"):			pconfig->hxtara = atol(value); break;
	  case fhash("TARARESLOCK"):	pconfig->hxtaralock = atoi(value); break;
	  case fhash("TARARESET"):		pconfig->hxtarareset = atoi(value); break;
	  case fhash("REFKG"):			pconfig->hxrefkg = atol(value); break;
	  case fhash("SPREAD"):		pconfig->hxspread = atof(value); break;
	  case fhash("TEMPCOMP"):		pconfig->hxtempcomp = atof(value); break;
	  case fhash("NSAMPLES"):		pconfig->hxnsamples = atoi(value); break;
	  } break;
  case fhash("ONEWIRE"):
	  switch(name){
	  case fhash("OWFILEPATH"):	strcpy(pconfig->owfspath,value); break;
	  case fhash("TEMPCINT"):		pconfig->owtcint = atof(value); break;
	  case fhash("TEMPCEXT"):		pconfig->owtcext = atof(value); break;
	  case fhash("TEMPCHIVE1"):	pconfig->owtchive1 = atof(value); break;
	  case fhash("TEMPCHIVE2"):	pconfig->owtchive2 = atof(value); break;
	  case fhash("OWTEMPINTID"):	strcpy(pconfig->owtempintid,value); break;
	  case fhash("OWTEMPEXTID"):	strcpy(pconfig->owtempextid,value); break;
	  case fhash("OWTEMPHIVE1ID"):	strcpy(pconfig->owtemphive1id,value); break;
	  case fhash("OWTEMPHIVE2ID"):	strcpy(pconfig->owtemphive2id,value); break;
	  } break;
  case fhash("WEBUI"):
	  switch(name){
	  case fhash("BEELOGWEB"):		strcpy(pconfig->web_root,value); break;
	  case fhash("BEEKEEPER"):		strcpy(pconfig->web_beekeeper,value); break;
	  case fhash("LOCDAT1"):		strcpy(pconfig->web_locdat1,value); break;
	  case fhash("LOCDAT2"):		strcpy(pconfig->web_locdat2,value); break;
	  case fhash("LOCPLZ"):		pconfig->web_locplz = atoi(value); break;
	  case fhash("LOCDAT3"):		strcpy(pconfig->web_locdat3,value); break;
	  case fhash("PICSMALL"):		strcpy(pconfig->web_picsmall,value); break;
	  case fhash("PICLARGE"):		strcpy(pconfig->web_piclarge,value); break;
	  case fhash("AUTOUPDATE"):	pconfig->web_autoupdate = atoi(value); break;
	  case fhash("NOTICEFILE"):	strcpy(pconfig->web_noticefile,value); break;
	  case fhash("ALARMON"):		pconfig->web_alarm_on = atoi(value); break;
	  case fhash("ALARMWEIGHT"):	pconfig->web_alarm_weight = atoi(value); break;
	  case fhash("ALARMSWARM"):	pconfig->web_alarm_swarm = atoi(value); break;
	  case fhash("ALARMBATT1"):	pconfig->web_alarm_batt1 = atoi(value); break;
	  case fhash("ALARMBATT2"):	pconfig->web_alarm_batt2 = atoi(value); break;
	  } break;
  case fhash("EXPORT"):
	  switch(name){
	  case fhash("EXFTPURL"):		strcpy(pconfig->exp_ftpurl,value); break;
	  case fhash("EXFTPPORT"):		pconfig->exp_ftpport = atoi(value); break;
	  case fhash("EXFTPPATH"):		strcpy(pconfig->exp_ftppath,value); break;
	  case fhash("EXFTPPROXY"):	strcpy(pconfig->exp_ftpproxy,value); break;
	  case fhash("EXFTPPROXYPORT"):pconfig->exp_ftpproxyport = atoi(value); break;
	  case fhash("EXFTPUSER"):		strcpy(pconfig->exp_ftpuser,value); break;
	  case fhash("EXBKUPPATH"):	strcpy(pconfig->exp_bkuppath,value); break;
	  case fhash("BKUPFILE"):		strcpy(pconfig->exp_bkupfile,value); break;
	  } break;
  default:
	  if (strcmp(name, "VERSION") == 0) strcpy(pconfig->version,value);	break;		  
  }
 return 1;
}


configuration* getini(char* inifile){
	
    if (ini_parse(inifile, handler, &config) < 0) {
        printf("    GetINI: Can't load %s\n", inifile);
        return NULL;
    }
	
#ifdef GETINIDBG
    printf("    GetINI: Config loaded from %s: version=%s, TARA: %i, TempCalibr:%f, BH-Home: %s\n", inifile,
        config.version, config.hxtara, config.hxtempcomp, config.bh_home);
#endif
	
    return (&config);
}

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

	lstart = ini_search_file(fd, section, name);	// search parameter in file
	if(lstart <0)
		return lstart;						// parameter not found

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