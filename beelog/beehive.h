/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*******************************************************************************
 * File:   beehive.h
 * Author: Randolph Esser - Copyright 2017-2019
 * Created on 17. Januar 2017, 10:13
 *
 * This file is part of the "beelog" program/project.
 *******************************************************************************
 */

#ifndef BEEHIVE_H
#define BEEHIVE_H

#ifdef __cplusplus
extern "C" {
#endif

// start simulation of RPi HW via QEMU VM
// also controlled by pre compiler switch: "/D BHSIM" in netbeans
//#define BHSIM
	
// Definitions of LogLevel masks instead of verbose mode
#define LOGBH		0b00000001	// 1:   Behive program flow control
#define LOGOW		0b00000010	// 2:   1-wire discovery and value read
#define LOGHX1		0b00000100	// 4:   HX711 init & get values
#define LOGHX2		0b00001000	// 8:   HX711 init & get values
#define LOGXFER		0b00010000	// 16:  Import/export in all formats and protocols
#define WEBUPD		0b00100000	// 32:  all about web page creation & data update
#define LOGMON		0b01000000  // 64:  Raspberry monitoring routines & ADS
#define LOGDAT		0b10000000	// 128: statistic data calc. & validation

unsigned char		lflags;			// log flag field
#define	BHLOG(m)	if(lflags & m)	// macro for Log evaluation

// Action on IO failure (e.g. file open failure, No OW sensors, No weightcell)
// config -> bh_actionIOfailure
#define AOF_NOOP		0	// ignore: continue or retry
#define AOF_EXIT		1	// stop program with exit code EXIT_FAILURE
#define AOF_REBOOT		2	// initiate system("reboot") -> cron restart of beehive expected

// Import/Export protocol and Mode definitions
#define EXPTYPE_FTP		1	// do export of beelogger data by FTP
#define EXPTYPE_SMB		2	// do export of beelogger data by SMB
#define EXPTYPE_MAIL	3	// do export of beelogger data by mail (SMTP)
#define EXPFORM_RAW		1	// use format as given: log/TXT + CSV
#define EXPFORM_SYSTEM  2   // do FTP export of beelogger using system() call
#define EXPFORM_DOC		3	// convert data to a doc type format (t.b.d.)
#define IMPTYPE_FTP		1	// do import of beelogger data by FTP
#define IMPTYPE_SMB		2	// do import of beelogger data by SMB
#define IMPTYPE_MAIL	3	// do import of beelogger data by mail (SMTP)
#define IMPFORM_RAW		1	// use format as given: log/TXT + CSV
#define IMPFORM_SYSTEM  2	// do FTP import of beelogger data using system() function 
#define IMPFORM_DOC		3	// convert data to a doc type format (t.b.d.)

#define DROWNOTELEN	1024
typedef struct {				// data elements of one log line entry
	time_t	tvsec;				// time slot of current data snap
	float	HiveWeight1;
	float	HiveWeight2;
	float	TempHive1;
	float	TempHive2;
	float	TempExtern;
	float	TempIntern;  
	float	Batt3V;				// RPi 3.3V voltage level
	float	Batt5V;				// RPi 5V   voltage level
	float	BattCharge1;		// 1. ext. voltage level for charging (6..48V)
	float	BattCharge2;		// 2. ext. voltage level for charging (6..48V)
	float	GewDelta1;
	float	GewDelta2;
	char	comment[DROWNOTELEN];
} datrow;

typedef struct {
	time_t	daystart;			// start of day measurements
	time_t	dayend;				// end of day measurements
	float	HiveWeight1_day;	// average weight of hive 1 of day
	float	HiveWeight2_day;	// average weight of hive 2 of day
	float	GewDelta1_day;		// sum of all weight delta of day of hive1
	float	GewDelta2_day;		// sum of all weight delta of day of hive2
	float	Texofdaymax;		// Max. temp. of day
	float	Texofdaymin;		// Min temp. of day
	int		samplesofday;		// number of discovered snaps of day
} datday;

typedef struct {
	time_t	weekstart;			// start of week measurements
	time_t	weekend;			// end of week measurements
	float	HiveWeight1_week;	// average weight of hive 1 of week
	float	HiveWeight2_week;	// average weight of hive 2 of week
	float	GewDelta1_week;		// sum of all weight delta of week of hive1
	float	GewDelta2_week;		// sum of all weight delta of week of hive2
	float	Texofweekmaxav;		// average of Max. temp. of 1 week
	float	Texofweekminav;		// average of Min temp. of 1 week
	int		samplesofweek;		// number of discovered snaps of week
} datweek;

typedef struct {
	time_t	monthstart;			// start of month measurements
	time_t	monthend;			// end of month measurements
	float	HiveWeight1_month;	// average weight of hive 1 per month
	float	HiveWeight2_month;	// average weight of hive 2 per month
	float	GewDelta1_month;	// sum of all weight delta of month of hive1
	float	GewDelta2_month;	// sum of all weight delta of month of hive2
	float	Texofmonthmaxav;	// average of Max. temp. of 1 month
	float	Texofmonthminav;	// average of Min temp. of 1 month
	int		samplesofmonth;		// number of discovered snaps of week
} datmonth;

#define datasetsize	24*60		// for one day with max one entry per minute
typedef struct {
	// save timestamp of last datarow entry:
	time_t		tlast;		
	int			inday;		// day of month 1..31
	int			indayw;		// day of week 0..6 == Mo...So
	int			indayy;		// get day of the year 0..365
	int			inmonth;	// in month 0..11 == Jan. ... Dec.
	int			inyear;		// year yyyy
	int			drowidx;	// index of last raw datarow entry
	int			drowvidx;	// index of last valid datarow (verified)
	// now the orig. datafields
	datrow		dlog[datasetsize];	// all logs of the day
	datday		dday;		// for daily statistics
	datweek		dweek;		// for weekly statistics
	datmonth	dmonth;		// for monthly statistics
} dataset;

typedef struct {
	int	weight;
	int swarm;
	int batterie;
} alarmfield;

#define	MAXWDAYS	7	// max. 7 days weather forecast store
typedef struct {
	int day_temp_min;
	int day_temp_max;
	int	day_rain;
}wdataset;

typedef struct {
	int gps_satelites;
	int gps_coordinates;
}gpsdata;

w1_values *	initall(int hxnum, int hxchannel);
void initbhdb	(dataset * data);
int  datacal	(dataset * data);
void getdaystatistic (dataset * data);
void getweekstatistic(dataset * data);
void getmonthstatistic(dataset * data);
int  myalarm	(dataset * data, alarmfield * flags);
int  webui		(dataset * data);
int  import		(int imptype, int impformat, char* url, char * path);
int  export		(int exptype, int expformat, char* url, char * path);
int  getweather	(char * webpath, wdataset * wdata);
char * getnotice(char * notice, dataset* data);
int  beecounter	(int action, long * beecnt);
gpsdata* getgps	(int action, gpsdata * gpsdat);
void bubble_sort(long list[], int n);

/******************************************************************************
 * global data fields
 ******************************************************************************/
dataset		bhdb;					// Database of measured values
alarmfield	alarms;
wdataset	webweather[MAXWDAYS];
char		csv_notice[1024];		// free notice field for *.csv file
int			curlcount;				// count curllib ftp xfer calls for debugging reasons

// The one and only global init parameter set buffer parsed from config.ini file
configuration * cfgini;				// ptr. to struct with initial parameters

struct timeval	MyTime;				// contains time snap in seconds-usecs to epoche time

#ifdef __cplusplus
}
#endif

#endif /* BEEHIVE_H */

