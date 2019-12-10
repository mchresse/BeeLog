/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*******************************************************************************
 * File:   beelog
 * Author: Randolph Esser - Copyright 2017-2019
 * Created on 17. Januar 2017, 10:09
 * This file is part of the "beelog" program/project. 
 *
 * Creation of all Logfiles and reports to extern:
 * - Create log files as interface to further rpograms
 *		- BeeLog.txt, 
 *		- Daily CSV Format for measured raw data sets
 *		- BeeDays - statistic CSV file
 * - Add web notices to Webpage index file for service purpose
 *******************************************************************************
*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sched.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <sys/time.h>

#include "get1wire.h"
#include "getini.h"
#include "beehive.h"
#include "beelog.h"

/*******************************************************************************
 * Global data reference section
 *******************************************************************************
*/
extern struct mondat mondata;		// Ref. to data collection of RPi runtime Parameters
									// maitanined in raspimion.c
char		  TimeString[128];		// Timestamp object of current action
									// loop common for all Log files -> beelog()

/*******************************************************************************
* Function: beelog()
* Creation: 22.01.2017
* 
* Write LOG data in a special line format with time stamp into log file.
* Logfile path and name as defined in config.ini
*
* Input :
*	comment		string of logdata (/wo timestamp)
*
* Global:	
*	cfgini struct	struct of runtime parameters
*	beelog.log file
*
* Output:
*	return		 0 log data written to LOG file, 
*				-1 file could not be found/opened
*
*********************************************************************************
*/
int beelog(char * comment){
char	logline[LOGLINELEN];	// text buffer of one log line
FILE *	bhlog;					// file handle of logfile
char	logpath[1024];			// log file path buffer	
struct timeval	MyTime;

	// build logdata string ->  get current timestamp
	gettimeofday(&MyTime, NULL);
	strftime(TimeString, 80, "%d-%m-%y %H:%M:%S", localtime(&MyTime.tv_sec));
	
	// Build the LOG File data entry+--
	logline[0] = 0;
	sprintf(logline, "%s: %s\r\n", TimeString, comment);	// get header line text
	BHLOG(LOGMON) printf("    BeeLog: %s", logline);
	
	// now write logdata to file
	sprintf(logpath, "%s/%s", cfgini->web_root, cfgini->bh_LOGFILE); 
	bhlog = fopen(logpath, "a");
	
	if (!bhlog) {
		//----- FILE NOT FOUND -----	
		if(!(bhlog = fopen(logpath, "rw"))){	// retry open file with rw parameters
			printf("	BeeLog: File not found and could not be created at %s\n", logpath);
			return(-1);
		}
		// else new file created
	}

	fseek(bhlog, 0L, SEEK_END);		// check length of file, and go to end of file 	
	fwrite(&logline[0],sizeof(unsigned char), strlen(logline), bhlog) ;
	fflush(bhlog);			// write and flash file
	fclose(bhlog);

	return(0);	
}

/*******************************************************************************
* Function: beecsv()
* Creation: 22.01.2017
* 
* Write beehive data (all 1-wire sensors + weight scale vale) in a special 
* line format with time stamp into CSV file.
* CSVfile path and name as defined in config.ini
*
* Input :
*	dataset * data	pointer to all log data DB
*
* Global:	
*	cfgini struct	struct of runtime parameters
*	beelog_<date>.csv file
*
* Output:
*	return		 0 log data written to LOG file, 
*				-1 file could not be found/opened
*
*********************************************************************************
*/
int beecsv(dataset* data){
char	logline[LOGLINELEN];	// text buffer of one log line
char	sbuf[LOGLINELEN];		// univ. string buffer
FILE *	bhlog;					// file handle of logfile
char	logpath[1024];			// log file path buffer	
int		i,len;
struct timeval	MyTime;
int		idx;	
	
	idx = data->drowidx;		// get curr. data row index

	// get current timestamp -> date & time
	strftime(TimeString, 80, "%Y/%m/%d %H:%M", localtime(&data->dlog[idx].tvsec));
		
	// Build the CSV File data entry
	logline[0] = 0;
		// get log line text based on CSV_HEADER format !
	sprintf(logline, "%s,%3.3f,%3.3f,%3.3f,%3.3f,%3.3f,%3.3f,%2.2f,%3.3f,%3.3f #%i %s\r\n", 
				TimeString, 
				data->dlog[idx].HiveWeight1,
				data->dlog[idx].HiveWeight2,
				data->dlog[idx].TempHive1,
				data->dlog[idx].TempHive2,
				data->dlog[idx].TempExtern,
				data->dlog[idx].TempIntern,
				data->dlog[idx].Batt5V,
				data->dlog[idx].GewDelta1,
				data->dlog[idx].GewDelta2,
				idx, 
				data->dlog[idx].comment);	// with finalized EOL for Windows & Linux

	BHLOG(LOGHX2) printf("    BeeCSV: %s", logline);

	// now write logdata to log file
	// first, get timestamp of curr. log data -> year only
	strftime(TimeString, 80, "%Y", localtime(&data->dlog[idx].tvsec));
	sprintf(logpath, "%s/%s%s.csv", cfgini->web_root, cfgini->bh_CSVFILE, TimeString); 

	// do we have already a log file existing
	bhlog = fopen(logpath, "a");
	
	if (!bhlog) {
		// original log file not existing -----> FILE NOT FOUND -----	
		if(!(bhlog = fopen(logpath, "rw"))){	// retry open file with rw parameters
			printf("    BeeCSV: File not found and could not be created at %s\n", logpath);
			return(-1);
		}
		// else new file created
		sprintf(sbuf, "    BeeCSV: New %s file created", logpath);
		beelog(sbuf);
	}

	fseek(bhlog, 0L, SEEK_END);		// check length of file 
	if (ftell(bhlog) == 0){			// -> =0 new file created: needs headerline first
		BHLOG(LOGHX2) printf("    BeeCSV: New LogFile created\n");
		fseek(bhlog, 0L, SEEK_SET);				// go back to start of file
		sprintf(sbuf, "%s\n", CSV_HEADER);	// get header line text
		fwrite(&sbuf[0],sizeof(unsigned char), strlen(sbuf), bhlog) ;
		fflush(bhlog);			// write and flash file
	}

	fwrite(&logline[0],sizeof(unsigned char), strlen(logline), bhlog) ;

	fflush(bhlog);			// write and flash file
	fclose(bhlog);
return(0);	
}

/*******************************************************************************
* Function: beedays()
* Creation: 14.09.2017
* 
* Write beehive statstic data of each day end in a special 
* line format with time stamp into a CSV file.
* CSVfile path and name as defined in config.ini
*
* Input :
*	dataset * data	pointer to all log data DB
*	comment *		pointer to comment field 
*
* Global:	
*	cfgini struct	struct of runtime parameters
*	beedays<year>.csv file
*
* Output:
*	return		 0 log data written to LOG file, 
*				-1 file could not be found/opened
*
*********************************************************************************
*/
int beedays(dataset* data, char* comment){
char	logline[LOGLINELEN];	// text buffer of one log line
char	logline2[LOGLINELEN];	// text buffer of one log line
char	sbuf[LOGLINELEN];		// univ. string buffer
FILE *	bhlog;					// file handle of logfile
char	logpath[1024];			// log file path buffer	
int		i,len;
struct timeval	MyTime;
int		idx;	
	
	idx = data->drowidx;		// get curr. data row index

	// get day start timestamp -> date & time
	strftime(TimeString, 80, "%Y/%m/%d %H:%M", localtime(&data->dday.daystart));
		
	// Build the CSV Days File data entry
	logline[0] = 0;
		// get log line text based on CSV_HEADER format !
	sprintf(logline, "%s,%3.3f,%3.3f,%2.2f,%2.2f,%i # %s\r\n", 
			TimeString, 
			data->dday.HiveWeight1_day,
			data->dday.GewDelta1_day, 
			data->dday.Texofdaymin, 
			data->dday.Texofdaymax,
			data->dday.samplesofday,
			comment);	// with finalized EOL for Windows & Linux
	
	// get day end timestamp -> date & time
	strftime(TimeString, 80, "%Y/%m/%d %H:%M", localtime(&data->dday.dayend));
		
	// Build the 2. CSV Days File data entry
	logline2[0] = 0;
		// get log line text based on CSV_HEADER format !
	sprintf(logline2, "%s,%3.3f,%3.3f,%2.2f,%2.2f,%i # %s\r\n", 
			TimeString, 
			data->dday.HiveWeight1_day,
			data->dday.GewDelta1_day, 
			data->dday.Texofdaymin, 
			data->dday.Texofdaymax,
			data->dday.samplesofday,
			comment);	// with finalized EOL for Windows & Linux

	BHLOG(LOGDAT) printf("    BeeDays: %s", logline2);

	// now write logdata to log file
	// first, get timestamp of curr. log data -> year only
	strftime(TimeString, 80, "%Y", localtime(&data->dday.daystart));
	sprintf(logpath, "%s/%s%s.csv", cfgini->web_root, cfgini->bh_CSVDAYS, TimeString); 

	// do we have already a log file existing
	bhlog = fopen(logpath, "a");
	
	if (!bhlog) {
		// original log file not existing -----> FILE NOT FOUND -----	
		if(!(bhlog = fopen(logpath, "rw"))){	// retry open file with rw parameters
			printf("    BeeDays: File not found and could not be created at %s\n", logpath);
			return(-1);
		}
		// else new file created
		sprintf(sbuf, "    BeeDays: New %s file created", logpath);
		beelog(sbuf);
	}

	fseek(bhlog, 0L, SEEK_END);		// check length of file 
	if (ftell(bhlog) == 0){			// -> =0 new file created: needs headerline first
		BHLOG(LOGDAT) printf("    BeeCSV: New LogFile created\n");
		fseek(bhlog, 0L, SEEK_SET);				// go back to start of file
		sprintf(sbuf, "%s\n", CSV_HDDAYS);	// get header line text
		fwrite(&sbuf[0],sizeof(unsigned char), strlen(sbuf), bhlog) ;
		fflush(bhlog);			// write and flash file
	}

	fwrite(&logline[0],sizeof(unsigned char), strlen(logline), bhlog) ;
	fwrite(&logline2[0],sizeof(unsigned char), strlen(logline2), bhlog) ;

	fflush(bhlog);			// write and flash file
	fclose(bhlog);
return(0);	
}


/*******************************************************************************
* Function: addwebnotice()
* Creation: 19.03.2017
* 
* open Web page index file and add one annotation entry:
*	      { series: 'Gewicht1',
*          x: Date.parse('2017/03/05 16:03'),
*          shortText: '11',
*          text: '11. added Single Drop Compensation der Waegezelle'}		
*
* Identity string for add location:	  //NoticeAddHere
*
* Input :
*	char *		pointer to annotation string
*
* Global:	
*	cfgini struct	struct of runtime parameters
*	index<year>.html file
*
* Output:
*	return		 0 log data written to Web index file, 
*				-1 file could not be found/opened
*				-2 file opened but location string could not be discovered
*				-3 notice section line not found
*				-4 webui update blocked
*********************************************************************************
*/
int addwebnotice(char * series, char* notice, dataset* data){
long	Fin;
long	lsize;
long int lstart;		// point to insert line start
long int lend;			// point to insert line end
long int lidx;			// point to last notice index line
long int cur;			// current file pointer position

char	temp[WEBIDXMAXLLEN+1];
char	sbuf[WEBIDXMAXLLEN+1];
char	fname[1024];
char	fname2[1024];
char *	buffer;
size_t	result;
char	ch;
char	mode[] = "0666";
int		i;

FILE*	fd;			// FD = original file; fd2 = file buffer for inserts
FILE*	fd2;
int		rc;
int		lnum = 0;
int		hit = 0;
int		idx;
int		noteidx=0;

	if(cfgini->hc_webui == 0){
		return(-4);
	}

	idx = data->drowidx;

	sprintf(fname,"%s/%s", cfgini->web_root, cfgini->web_deffile); // full source path + name
	if((fd = fopen(fname, "r")) == NULL) {
		return(-1);
	}

	sprintf(fname2,"%s/copy_%s", cfgini->web_root, cfgini->web_deffile);
	if((fd2 = fopen(fname2, "w")) == NULL) {
		fclose(fd);
		return(-2);
	}
    
	fseek (fd, 0L, SEEK_END);
	Fin = ftell(fd);					//  get the orig file size
	fseek(fd, 0L, SEEK_SET);			// go back to start of file

	lstart = ftell(fd);					// get start of first line
	
	while(fgets(temp, WEBIDXMAXLLEN, fd) != NULL) {
		if((strstr(temp, "x: Date.parse(")) != NULL) {
			lnum++;
			fgets(sbuf, WEBIDXMAXLLEN, fd);		// get next notice index line
		}else{	
			if((strstr(temp, WEBNOTICEMARKER)) != NULL) {
				BHLOG(LOGHX2) printf("    AddWebNotice: Hit at [line %d] %s\n", lnum, temp);
				hit++;
				break;
			}
		}
		lstart = ftell(fd);				// get start of next line
		lnum++;
	}
	if(hit == 0){
		fclose(fd);
		fclose(fd2);
		return(-3);						// parameter not found
	}


	// fetch last notice index 
	sscanf(sbuf,"          shortText: '%d'\n", &noteidx);

	// lstart delivers start pos of line; rd pointer is already at end of line
	lend = ftell(fd);					// and save this position
	lsize = Fin - lend;					// calc. remaining file size

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

	// insert new text pattern here
	//	{ series: 'Gewicht1',
	sprintf(sbuf,"          ,{ series: '%s', ", series);
	fputs(sbuf, fd2);
	//          x: Date.parse('2017/03/05 16:03'),
	strftime(temp, 80, "%Y/%m/%d %H:%M", localtime(&data->dlog[idx].tvsec));
	sprintf(sbuf,"x: Date.parse('%s'),\n", temp);
	fputs(sbuf, fd2);
	//          shortText: '11',
	sprintf(sbuf,"            shortText: '%d', ", noteidx+1);
	fputs(sbuf, fd2);				//insert value string
	//          text: '11. added Single Drop Compensation der Waegezelle'}
	sprintf(sbuf,"text: '%s'}\n", notice);
	fputs(sbuf, fd2);
	
	//end pattern with insert of next notice marker string
	sprintf(sbuf,"          %s\n", WEBNOTICEMARKER);
	fputs(sbuf, fd2);
	
	// copy end block to buffer file
	fseek(fd, lend, SEEK_SET);			// go behind insert point
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

	// rename fname2 to fname
    if ( remove (fname) == -1 ) {
		BHLOG(LOGHX2) printf("    AddWebNotice: remove of %s failed\n", fname);
	}	
	if( (rename(fname2, fname)) < 0) {
		BHLOG(LOGHX2) printf("    AddWebNotice: rename %s failed\n", fname2);
      return EXIT_FAILURE;
	}
    i = strtol(mode, 0, 8);
    if (chmod (fname,i) < 0)
    {
		BHLOG(LOGHX2) printf("    AddWebNotice: error in chmod(%s) %d\n", fname, mode);
    }

	return(0);
}