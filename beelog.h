/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*******************************************************************************
 * File:   beelog.h
 * Author: Randolph Esser - Copyright 2017-2019
 * Created on 19. January 2017, 13:31
 *
 * This file is part of the "beelog" program/project.
 *******************************************************************************
 */
 
#ifndef BEELOG_H
#define BEELOG_H

#ifdef __cplusplus
extern "C" {
#endif

#define	CSV_HEADER	"Datum,GewichtBeute1,GewichtBeute2,Temp.Beute1,Temp.Beute2,Temp.Extern,TempIntern,BatterieLevel,Gew1Delta,Gew2Delta"
#define	CSV_HDDAYS	"Datum,GewichtBeut1Av,Gew1DeltaSum,TempExtMin,TempExtMax,Samples"
#define LOGLINELEN	1024	// max length of one log textline incl. comments
#define LOGDATELEN	32		// length of log dat&time string
#define LOGNOTELEN	512		// length of logline comment string
#define WEBIDXMAXLLEN 512
#define WEBNOTICEMARKER "//NoticeAddHere"
	
int beecsv(dataset * data);
int beedays(dataset* data, char* comment);
int	beelog(char * comment);
int addwebnotice(char * serial, char* notice, dataset* data);




#ifdef __cplusplus
}
#endif

#endif /* BEELOG_H */

