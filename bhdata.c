/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*******************************************************************************
 * File:	BHData.c
 * Author:	Randolph Esser - Copyright 2017-2019
 * Created on 17. Januar 2017, 10:09
 * This file is part of the "beelog" program/project.
 *
 * Manage beehive internal DB struct
 * - evaluate measured values
 * - calculate statistic data by day/week/month/year
 * - in case set alarm flags if threshold are reached
 *******************************************************************************
 */

/* Global libraries:
 *  WiringPi library is used in GPIO mode (using orig. GPIO port index
 * 	 Makes use of WiringPI-library of Gordon Henderson 
 *	 (https://projects.drogon.net/raspberry-pi/wiringpi/)
 *	 Recommended connection (http://www.raspberrypi.org/archives/384):
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <sys/time.h>

#include <wiringPi.h>

#include "getini.h"
#include "get1wire.h"
#include "beehive.h"
#include "beelog.h"



/*******************************************************************************
* Function: initbhdb()
* Creation: 04.03.2017
* 
* Preset/preload all data fields in BHdb based on new program start time stamp.
* If existing prev. exported bhdb is imported and init data was taken from there.
* if not everything is preset to real start values.
*
* Input :
*	data	 *		pointer to dynamic bhdb of program to be initialized
*
* Global:	
*	cfgini		struct of runtime parameters
*	curTime		global date & time stamp
*
* Output:	void
******************************************************************************
*/
void initbhdb(dataset * data){
struct tm *	tinfo;		// get current time&date in formated values
int	idx = 0;			// datarow index

	gettimeofday(&MyTime, NULL);		// save current timestamp
	tinfo = localtime(&MyTime.tv_sec);
	
	// ToDo: check if exported bhdb is existing based on cfgini data
	// if( external bhdb found){
			// ToDo: import all data from ext. stor and update bhdb here
	// }else{
		// Start from brand new and reset all bhdb fields
		data->tlast	 = MyTime.tv_sec;	// get first data log timestamp
		data->inday  = tinfo->tm_mday;	// get day of the month:	1..31
		data->indayw = tinfo->tm_wday;	// get day of the week:		0..6
		data->indayy = tinfo->tm_yday;	// get day of the month:	0..365
		data->inmonth= tinfo->tm_mon;	// get month of the year:	0..11
		data->inyear = tinfo->tm_year+1900; // save current year absolute: 20xx
		data->drowidx =0;				// point to raw work data row index
		data->drowvidx = 0;				// point to last validated data row index

		data->dlog[idx].tvsec = MyTime.tv_sec;	// init empty marker of first data row.
		data->dlog[idx].GewDelta1 = 0;
		data->dlog[idx].GewDelta2 = 0;
		data->dlog[idx].HiveWeight1 = 0;
		data->dlog[idx].HiveWeight2 = 0;
		data->dlog[idx].TempExtern = 0;
		data->dlog[idx].TempHive1 = 0;
		data->dlog[idx].TempHive2 = 0;
		data->dlog[idx].TempIntern = 0;
		data->dlog[idx].Batt3V = 0;
		data->dlog[idx].Batt5V = 0;
		data->dlog[idx].BattCharge1 = 0;
		data->dlog[idx].BattCharge2 = 0;
		data->dlog[idx].comment[0] = 0;

		data->dday.daystart = MyTime.tv_sec;
		data->dday.dayend = MyTime.tv_sec;	// by now start = end
		data->dday.samplesofday = 0;
		data->dday.GewDelta1_day = 0;
		data->dday.GewDelta2_day = 0;
		data->dday.HiveWeight1_day = 0;
		data->dday.HiveWeight2_day = 0;
		data->dday.Texofdaymax = -99;
		data->dday.Texofdaymin = 99;

		data->dweek.weekstart = MyTime.tv_sec;
		data->dweek.weekend =  MyTime.tv_sec;	// by now start = end
		data->dweek.samplesofweek = 0;
		data->dweek.GewDelta1_week = 0;
		data->dweek.GewDelta2_week = 0;
		data->dweek.HiveWeight1_week = 0;
		data->dweek.HiveWeight2_week = 0;
		data->dweek.Texofweekmaxav = -99;
		data->dweek.Texofweekminav = 99;

		data->dmonth.monthstart	= MyTime.tv_sec;
		data->dmonth.monthend   = MyTime.tv_sec;	// by now start = end
		data->dmonth.samplesofmonth = 0;
		data->dmonth.GewDelta1_month = 0;
		data->dmonth.GewDelta2_month = 0;
		data->dmonth.HiveWeight1_month = 0;
		data->dmonth.HiveWeight2_month = 0;
		data->dmonth.Texofmonthmaxav = -99;
		data->dmonth.Texofmonthminav = 99;
	// }
	return;
}

/*******************************************************************************
* Function: datacal()
* Creation: 03.02.2017
* 
* Check for each new discovered dataset for statistical relevance:
* - evaluation of datalines: is each value in a valid change range ?
*   otherwise trigger an alarm or drop/restart this line
* - min/max values (weight/temp) per day/month/year
* - cumulated weight per day/month/year
* - threshold calculation to initiate alarms in case (weight, swarm, battery)
*   
* This check is done for each data collection loop in main()
*
* Input :
*	dataset		currently collected parameter set for evaluation
*
* Global:	
*	cfgini		struct of runtime parameters
*	bhdb		virt. database of statistic data
*	alarmflag	field of alarmflags dep. on type.
*	tinfo		global date & time stamp
*
* Output:
*	return		 1 all data is valid but alarms was triggered
*				 0 all data is valid and calculation done 
*				-1 Dataset invalid; to be dropped
*				-2 Dataset invalid; to be repeated (advise)
*********************************************************************************
*/
int datacal(dataset * data){
struct tm *	tinfo;		// get current time&date in formated values
			// struct tm {				as defined in time.h:
			//   int tm_sec;         /* seconds,  range 0 to 59          */
			//   int tm_min;         /* minutes, range 0 to 59           */
			//   int tm_hour;        /* hours, range 0 to 23             */
			//   int tm_mday;        /* day of the month, range 1 to 31  */
			//   int tm_mon;         /* month, range 0 to 11             */
			//   int tm_year;        /* The number of years since 1900   */
			//   int tm_wday;        /* day of the week, range 0 to 6    */
			//   int tm_yday;        /* day in the year, range 0 to 365  */
			//   int tm_isdst;       /* daylight saving time             */		
			//	};
int	didx;
int vidx;
int	rc = 0;
float spread_percent;
char sbuf[1024];

	didx  = data->drowidx;			// get index of new data row
	vidx  = data->drowvidx;			// get index of last validated data row

	// we have a new datarow -> update its related log time stamp data
	// ( this can be done always : with pos. or neg. data check below
	tinfo = localtime(&data->dlog[didx].tvsec); 
	data->tlast	 = data->dlog[didx].tvsec;	// store latest data log timestamp
	data->inday  = tinfo->tm_mday;		// get day of the month:	1..31
	data->indayw = tinfo->tm_wday;		// get days since Sunday	0..6
	data->indayy = tinfo->tm_yday;		// get day of the year: 	0..365
	data->inmonth= tinfo->tm_mon;		// get month of the year:	0..11
	data->inyear = tinfo->tm_year+1900; // calc. current year absolute: 20xx
	
	// migrate new dataline to bhdb statistic values
	BHLOG(LOGDAT) strftime(sbuf, 80, "%H:%M:%S", tinfo);
	BHLOG(LOGDAT) printf("    DATCAL: %d.%d.%d %s [%i-%i-%i-%i] -> Index[%i v:%i] weight1: %3.3f\n", 
			data->inday, data->inmonth+1, data->inyear, 
			sbuf,
			data->indayw,
			data->inday, 
			data->indayy, 
			data->inmonth,
			data->drowvidx, data->drowidx, 
			data->dlog[didx].HiveWeight1);
	
	// detect new start of day measurement independent on time of day
	if(didx == 0){						// first measurement is always valid
		data->drowvidx = data->drowidx;	// we got new validated datrow for release
		alarms.swarm = 0;
		alarms.weight = 0;
		rc = 0;
		data->dday.daystart	= data->dlog[didx].tvsec;	// remember start of day measurements
		data->dday.dayend	= data->dlog[didx].tvsec;	// remember last entry of day measurements
		data->dday.samplesofday++;						// remember # of validated entries
		return(rc);
	}
	
	// check if new value is in trust range against last good value (indexed by vidx)
	spread_percent = cfgini->datspread / 100.0;	// reliability range in %
	if( abs(data->dlog[didx].HiveWeight1 - data->dlog[vidx].HiveWeight1) > 
							data->dlog[vidx].HiveWeight1 * spread_percent){
		// No, its out of trust-range
		sprintf(data->dlog[didx].comment,"Weight1 data skipped: %3.3f", data->dlog[didx].HiveWeight1);
		BHLOG(LOGDAT) printf("    bhdata: weight1 data skipped; range exceeded %3.3f > %3.3f\n", 
						data->dlog[vidx].HiveWeight1 * spread_percent, 
						abs(data->dlog[didx].HiveWeight1 - data->dlog[vidx].HiveWeight1));
		// leave data->drowvidx as it is -> remains as last good value

		if((didx - vidx) == 1){	// if its first bad entry in line
			// we keep last validated entry index as it is: data->drowvidx remains unchanged
			// create a new entry but containing last validated value
			data->dlog[didx].HiveWeight1 = data->dlog[vidx].HiveWeight1;
			data->dlog[didx].GewDelta1 = 0;
			data->dday.dayend	= data->dlog[didx].tvsec;	// remember last entry of day measurements
			data->dday.samplesofday++;						// remember # of validated entries
			BHLOG(LOGDAT) printf("    DATCAL: reset weight1 to previous value: %3.3f\n", data->dlog[vidx].HiveWeight1); 
			return(1);		// and we are done for now
		} 
		// for >1 do nothing...
		// for further exceeded entries, we assume weight really dropped so fast and cont. with good case status
	}
	
	// ToDo:same to be done for weight2 here

	// Assumption from here on: 
	// weight data good: at least in range of expectation
	data->drowvidx = data->drowidx;	// yippieh. we got a new validated datarow for release
	data->dday.samplesofday++;		// remember # of validated entries


	// in case initiate actions for certain date transitions
	// get time stamp of last (!) validated entry -> may be from the the day before !!!
	tinfo = localtime(&data->dlog[vidx].tvsec);		

	// New Day ? (12:59 > 00:00) or database of day full ?
	if(data->inday != tinfo->tm_mday || data->drowidx >= datasetsize){		
		if(data->drowidx >= datasetsize){
			sprintf(sbuf, "Max dataset size reached %iof%i", data->drowidx, datasetsize);
			BHLOG(LOGDAT) printf("    DATCAL: %s\n", sbuf);
			beelog(sbuf);
		}

		// New day reached: calculate and store min/max day statistics
		getdaystatistic(data);		
		getweekstatistic(data);		// update also weekly min/max statistics
		getmonthstatistic(data);	// update also monthly min/max statistics			
		// getyearstatistic(data);	// update also yearly min/max statistics

		sprintf(sbuf, "Last day [%i-%i-%i-%i]>[%i-%i-%i-%i](%3i of %3i): Weight1: %3.3f, DofDay1: %3.3f, MinTemp: %3.3f, MaxTemp: %3.3f",
				tinfo->tm_wday,	// get days since Sunday	0..6
				tinfo->tm_mday,	// get day of the month:	1..31
				tinfo->tm_yday,	// get day of the year: 	0..365
				tinfo->tm_mon,	// get month of the year:	0..11
				data->indayw,
				data->inday, 
				data->indayy, 
				data->inmonth,
				didx, data->dday.samplesofday,
				data->dday.HiveWeight1_day, data->dday.GewDelta1_day, 
				data->dday.Texofdaymin, data->dday.Texofdaymax);
		BHLOG(LOGDAT) printf("    DATCAL: Avg. values of %s\n", sbuf);
		beelog(sbuf);
		//	BHLOG(LOGDAT) printf("    DATCAL: Daily: MinDay %3.3f MinWeek %3.3f\n", data->dday.Texofdaymin, data->dweek.Texofweekminav);

		data->dday.dayend	= data->dlog[vidx].tvsec;	// remember last entry of day measurements
		sprintf(sbuf, "EOD");
		beedays(data, sbuf);		// store all daily statistic data to a CSV file
		
		// reset database to start of day and keep last datarow as first one of new day
		data->drowvidx = data->drowidx = 0;						// start next day measurement from start idx
		data->dday.daystart			= data->dlog[didx].tvsec;	// remember start of day measurements
		data->dday.dayend			= data->dlog[didx].tvsec;	// remember last entry of day measurements
		data->dlog[0].tvsec			= data->dlog[didx].tvsec;	// and datarow time stamp itself
		data->dday.samplesofday		= 1;
		data->dlog[0].HiveWeight1	= data->dlog[didx].HiveWeight1;
		data->dlog[0].HiveWeight2	= data->dlog[didx].HiveWeight2;
		data->dlog[0].TempHive1		= data->dlog[didx].TempHive1;
		data->dlog[0].TempHive2		= data->dlog[didx].HiveWeight2;
		data->dlog[0].TempExtern	= data->dlog[didx].TempExtern;
		data->dlog[0].TempIntern	= data->dlog[didx].TempIntern;
		data->dlog[0].Batt3V		= data->dlog[didx].Batt3V;
		data->dlog[0].Batt5V		= data->dlog[didx].Batt5V;
		data->dlog[0].BattCharge1	= data->dlog[didx].BattCharge1;
		data->dlog[0].BattCharge2	= data->dlog[didx].BattCharge2;
		data->dlog[0].GewDelta1		= data->dlog[didx].GewDelta1;
		data->dlog[0].GewDelta2		= data->dlog[didx].GewDelta2;
		strncpy(data->dlog[0].comment, data->dlog[didx].comment, strlen(data->dlog[didx].comment));

		if(data->indayw == 0){		// Start of New Week ? == Sonntag (6->0) ?			
			sprintf(sbuf, "Last week(%d > %d; #%i): WeekAvWeight1: %3.3f, WeekGewDelta1: %3.3f, MinTemp: %3.3f, MaxTemp: %3.3f",
				tinfo->tm_wday, data->indayw, data->dweek.samplesofweek,
				data->dweek.HiveWeight1_week, data->dweek.GewDelta1_week, 
				data->dweek.Texofweekminav, data->dweek.Texofweekmaxav);
			BHLOG(LOGDAT) printf("    DATCAL: Avg. values of %s\n", sbuf);
			beelog(sbuf);

			// t.b.d. beeweeks(data, sbuf);		// store all weekly statistic data to a CSV file

			data->dweek.weekstart = data->dday.daystart;
			data->dweek.samplesofweek = 0;
			data->dweek.GewDelta1_week = 0;
			data->dweek.GewDelta2_week = 0;
			data->dweek.HiveWeight1_week = 0;
			data->dweek.HiveWeight2_week = 0;
			data->dweek.Texofweekmaxav = -100;	// simulate lowest  max value to get a valid start at next day
			data->dweek.Texofweekminav =  100;	// simulate highest min value to get a valid start at next day
		} // endif "new week"

		if(data->inday == 1){			// Monatsgrenze überschritten ? (30/31->1)
			// Yes, calculate and store min/max week statistics	here	
			sprintf(sbuf, "Last month(%d > %d; #%i): AvWeight1: %3.3f, AvGewDelta1: %3.3f, MinTemp: %3.3f, MaxTemp: %3.3f",
				tinfo->tm_mon, data->inmonth, data->dmonth.samplesofmonth,
				data->dmonth.HiveWeight1_month, data->dmonth.GewDelta1_month, 
				data->dmonth.Texofmonthminav, data->dmonth.Texofmonthmaxav);
			BHLOG(LOGDAT) printf("    DATCAL: Average %s\n", sbuf);
			beelog(sbuf);

			// t.b.d. beemonths(data, sbuf);		// store all monthly statistic data to a CSV file

			data->dmonth.monthstart = data->dday.daystart;
			data->dmonth.samplesofmonth = 0;
			data->dmonth.GewDelta1_month = 0;
			data->dmonth.GewDelta2_month = 0;
			data->dmonth.HiveWeight1_month = 0;
			data->dmonth.HiveWeight2_month = 0;
			data->dmonth.Texofmonthmaxav = -100;	// simulate lowest  max value to get a valid start at next day
			data->dmonth.Texofmonthminav =  100;	// simulate highest min value to get a valid start at next day
		} // endif "new month"

		if(data->indayy == 0){					// Jahresgrenze überschritten ? [1-31-'364'-11]>[2-1-'0'-0]
			// Yes, do what ever could be calculated per year here...
			// by now nothing to do... just logging

			sprintf(sbuf, "Last year(%d(%i) > %d(%i)): Happy new year !",
				tinfo->tm_year+1900, tinfo->tm_yday, 
				data->inyear, data->indayy);
			BHLOG(LOGDAT) printf("    DATCAL: %s\n", sbuf);
			beelog(sbuf);
			// beeyears(data, sbuf);		// store all yearly statistic data to a CSV file
		} // endif "new year"
				
	} // endif "new day"

	// but by now no weight alarms to be flagged
	alarms.swarm = 0;
	alarms.weight = 0;
	rc = 0;
  
  return(rc);	// all data o.k.
} // end of datacal()

/*******************************************************************************
* Function: getdaystatistic()
* Creation: 10.09.2017
* 
* Calculate max/Min values of the day
* 
*
* Input :
*	dataset		currently collected parameter set for evaluation
*
* Global:	
*
* Output:
*	return		 -
*********************************************************************************
*/
void getdaystatistic(dataset * data){
	int i;		// work index of datarow
	int didx;	// index of last value row
	float	sum_weight1;	// avg. weight of day
	float	sum_dweight1;	// cummulated weight deltas of day
	float	sum_mintemp1;	// max temp of day
	float	sum_maxtemp1;	// min temp of day
	
	
	sum_weight1 =0;	// avg. weight of day
	sum_dweight1=0;	// cummulated weight deltas of day
	sum_mintemp1=data->dlog[0].TempExtern;	// init min temp of day
	sum_maxtemp1=data->dlog[0].TempExtern;	// init max temp of day

	for(i=0; i < data->dday.samplesofday-1; i++){	
		// sum up for all datalines of the past day
		// except the current/last one which is already first one of next day
		sum_weight1 += data->dlog[i].HiveWeight1;
		sum_dweight1+= data->dlog[i].GewDelta1;
		if(sum_mintemp1 > data->dlog[i].TempExtern) {
			sum_mintemp1 = data->dlog[i].TempExtern;
		}
		if(sum_maxtemp1 < data->dlog[i].TempExtern) {
			sum_maxtemp1 = data->dlog[i].TempExtern;
		}
	}
	data->dday.HiveWeight1_day	= sum_weight1 / i;
	data->dday.GewDelta1_day	= sum_dweight1;
	data->dday.HiveWeight2_day	= 0;
	data->dday.GewDelta2_day	= 0;
	data->dday.Texofdaymin		= sum_mintemp1;
	data->dday.Texofdaymax		= sum_maxtemp1;
	
	return;
} // end of getdaystatistic()

/*******************************************************************************
* Function: getweekstatistic()
* Creation: 01.01.2018
* 
* Calculate max/Min values of the week
* 
*
* Input :
*	dataset		currently collected parameter set for evaluation
*
* Global:	
*
* Output:
*	return		 -
*********************************************************************************
*/
void getweekstatistic(dataset * data){

	if(data->dday.HiveWeight1_day > data->dweek.HiveWeight1_week){
		data->dweek.HiveWeight1_week = data->dday.HiveWeight1_day;
	}
	data->dweek.GewDelta1_week += data->dday.GewDelta1_day;
	
//	BHLOG(LOGDAT) printf("    DATCAL: Before: MinDay %3.3f MinWeek %3.3f\n", data->dday.Texofdaymin, data->dweek.Texofweekminav);
	if(data->dday.Texofdaymin < data->dweek.Texofweekminav){
		data->dweek.Texofweekminav = data->dday.Texofdaymin;
//	BHLOG(LOGDAT) printf("    DATCAL: After: MinDay %3.3f MinWeek %3.3f\n", data->dday.Texofdaymin, data->dweek.Texofweekminav);
	}

	if(data->dday.Texofdaymax > data->dweek.Texofweekmaxav){
		data->dweek.Texofweekmaxav = data->dday.Texofdaymax;
	}
	data->dweek.samplesofweek += data->dday.samplesofday;
	data->dweek.weekend = data->dday.dayend;
	
	return;
} // end of getweekstatistic()

/*******************************************************************************
* Function: getmonthstatistic()
* Creation: 01.01.2018
* 
* Calculate max/Min values of the month
* 
*
* Input :
*	dataset		currently collected parameter set for evaluation
*
* Global:	
*
* Output:
*	return		 -
*********************************************************************************
*/
void getmonthstatistic(dataset * data){

	if(data->dday.HiveWeight1_day > data->dmonth.HiveWeight1_month){
		data->dmonth.HiveWeight1_month = data->dday.HiveWeight1_day;
	}
	data->dmonth.GewDelta1_month += data->dday.GewDelta1_day;
	
//	BHLOG(LOGDAT) printf("    DATCAL: Before: MinDay %3.3f MinMonth %3.3f\n", data->dday.Texofdaymin, data->dmonth.Texofmonthminav);
	if(data->dday.Texofdaymin < data->dmonth.Texofmonthminav){
		data->dmonth.Texofmonthminav = data->dday.Texofdaymin;
	}
	if(data->dday.Texofdaymax > data->dmonth.Texofmonthmaxav){
		data->dmonth.Texofmonthmaxav = data->dday.Texofdaymax;
	}
//	BHLOG(LOGDAT) printf("    DATCAL: After: MinDay %3.3f MinMonth %3.3f\n", data->dday.Texofdaymin, data->dmonth.Texofmonthminav);

	data->dmonth.samplesofmonth += data->dday.samplesofday;
	data->dmonth.monthend = data->dday.dayend;
	
	return;
} // end of getmonthstatistic()


/*******************************************************************************
* Function: myalarm()
* Creation: 03.02.2017
* 
* Depending on datacal() advise an alarms has to be triggered.
* - if thresholds reached to initiate alarms in case (weight, swarm, battery)
*   
* This check is done by datacal() for each data collection loop in main() 
 and in case a alarmflag was rised -> act accordingly
*
* Input :
*	dataset		currently collected parameter set for evaluation
*	alarmflag *	field of alarmflags dep. on type.
*
* Global:	
*	cfgini		struct of runtime parameters
*	bhdb		virt. database of statistic data
*
* Output:
*	return		 0 alarms was confirmed and send out/notified
*				 <0 couldn't identify alarm
*********************************************************************************
*/
int myalarm(dataset * data, alarmfield * flags){

	// check done, reset alarm flags
	alarms.batterie = 0;
	alarms.swarm = 0;
	alarms.weight = 0;
	
  return(0); // no alarm action
} // end of alarm()

