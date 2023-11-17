/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/******************************************************************************* 
 * File:   raspimon.c
 * Author: Randolph Esser - Copyright 2017-2019
 * Created on 17. Januar 2017, 10:09
 *
 * This file is part of the "beelog" program/project.
 *
 * Description :
 *    Raspberry Pi monitoring routine of some system informations.
 *******************************************************************************
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <wiringPi.h>

#include "getini.h"
#include "get1wire.h"
#include "hx711.h"
#include "curlftp.h"
#include "beehive.h"
#include "beelog.h"

#include "epaper/EPD_Test.h"
#include "epaper/fonts.h"
#include "epaper/EPD_2in7.h"
#include "epaper/GUI_Paint.h"
#include "epaper/GUI_BMPfile.h"

#include "raspimon.h"

#define COLORED BLACK
#define UNCOLORED WHITE


mondat mondata;
extern const unsigned char gImage_Lea_Portrait[];	// Test Image for E-Paper
// Display scratch area
#define Imagesize   EPD_2IN7_WIDTH * EPD_2IN7_HEIGHT
UBYTE BlackImage[Imagesize];


/*******************************************************************************
 * RASPIMON()
 * Function to collect System runtime values and store them into a Monitor Structure
 * "mondata" is declared global for read usage by every funtion in the program.
 ******************************************************************************/
/*******************************************************************************
* Function: raspimon()
* Creation: 01.01.2019
* 
* Function to collect System runtime values and store them into a Monitor Structure
* "mondata" is declared global for read usage by every funtion in the program.
* 
* Input :	void
*
* Global:	
* mondata	global monitoring data struct gets updated
*
* Output:	void
*********************************************************************************
*/
void raspimon(){

struct sysinfo sys_info;
char	uptimeInfo[15];
char	cpuInfo[10]; 
char	ramInfo[10]; 
FILE *	tcpufile;
  
  //Get IPv4 Address string
	mondata.lanportname[0]	=0;
	mondata.ipv4[0]			=0;
	mondata.mymac[0]		=0;


	if(getipv4_2(mondata.lanportname, mondata.ipv4)){
		BHLOG(LOGMON) printf("    RASPIMON: no active LAN Port found\n");
	}else{
		BHLOG(LOGMON) printf("    RASPIMON: Default LAN port: %s %s\n", mondata.lanportname, mondata.ipv4);
	}
	
	// get system usage / info
	if(sysinfo(&sys_info) != 0){
		printf("sysinfo-Error\n");
		BHLOG(LOGMON) printf("    RASPIMON: sysinfo-Error\n");
		mondata.totalRam	=0;
		mondata.uptime		=0;
		mondata.avgCpuLoad	=0;
	}else{
		// uptime
		mondata.uptime = sys_info.uptime / 60;
		sprintf(uptimeInfo, "Uptime%5i:%2i", mondata.uptime/60, mondata.uptime % 60);

		// cpu info
		mondata.avgCpuLoad = sys_info.loads[0] / 1000;
		sprintf(cpuInfo, "CPU %ld%%", mondata.avgCpuLoad);

		// ram info
		mondata.totalRam = sys_info.freeram / 1024 / 1024;
		sprintf(ramInfo, "RAM %ld MB", mondata.totalRam);	
		BHLOG(LOGMON) printf("    RASPIMON: %s, %s, %s\n", uptimeInfo, cpuInfo, ramInfo);
	}

	tcpufile = fopen ("/sys/class/thermal/thermal_zone0/temp", "r");
	if (tcpufile == NULL){
		BHLOG(LOGMON){
			printf("    RASPIMON: TempCPU stream failed\n");
		}
	}else{
		fscanf (tcpufile, "%lf", &mondata.tcpu);
		mondata.tcpu /= 1000;
		BHLOG(LOGMON) printf("    RASPIMON: CPU temperature: %6.3f C.\n", mondata.tcpu);
		fclose (tcpufile);
	}	
	return;	  
}


/*******************************************************************************
 * PutStartScreen()
 * Display startup Image frame to EPaper
 *
 * Input: 
 *
 * Global: 
 * Output:
 * - void
 ******************************************************************************/
void PutStartScreen(){
//	EPD_2in7_test();
	
	printf("Paint_NewImage\r\n");
	Paint_NewImage(BlackImage, EPD_2IN7_WIDTH, EPD_2IN7_HEIGHT, 270, WHITE);
    
    printf("show window BMP: 100 x 100\r\n");
    Paint_SelectImage(BlackImage);
    GUI_ReadBmp("./100x100.bmp", 50, 50);
    EPD_2IN7_Display(BlackImage);
    DEV_Delay_ms(2000);
}


/*******************************************************************************
 * PutStatusScreen()
 * Function to display Welcome Info via E-paper "WaveShare 2.7" B/W " (SPI)
 * 2.7" Epape: EPD_HEIGHT 264, EPD_WIDTH = 176, (-> Rotate = 90) 
 * 			//  BxH  c/line  lines
 * Font24;	// 17x24  10,3    11
 * Font20;	// 14x20  12,5    13,2
 * Font16;	// 11x16  16      16,5
 * Font12;	//  7x12  25,1    22
 * Font8;	//  5x8   35,2    33
 *
 * Input: 
 *	pepd		Pointer on class Epd instance
 *  ppaint		Pointer on class Paint instance
 *  fbuf		Pointer on FrameBuffer Array
 *  didx		curr. data row index into bhdb
 *
 * Global: 
 * - mondata	-> delivers IP address
 * - bhdb		global beehive DB struct
 * Output:
 * - void
 ******************************************************************************/
void PutStatusScreen(int didx ){

unsigned char linebuf[EPD_2IN7_HEIGHT/8];
char		  TimeString[128];
struct tm *   tinfo;

	BHLOG(LOGMON) printf("    BeeHive: Preparing E-paper Frame Update ...\n");	
    EPD_2IN7_Clear();		// clear Frame Buffer

	// For testing: Display default image buffers
    printf("Paint_NewImage\r\n");
    Paint_NewImage(BlackImage, EPD_2IN7_WIDTH, EPD_2IN7_HEIGHT, ROTATE_90, UNCOLORED);
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
	
    /* For simplicity, the arguments are explicit numerical coordinates */
    Paint_SetRotate(ROTATE_90);		// horizontal view

	sprintf(linebuf, "BeeHive v%s", cfgini->version);
    Paint_DrawString_EN(0, 0, linebuf, &Font24, BLACK, WHITE);
	Paint_DrawString_EN(10, 20, "hello world", &Font12, BLACK, WHITE);

	tinfo = localtime(&MyTime.tv_sec);
	strftime(TimeString, 80, "%d.%m.%y %H:%M", tinfo);	
    Paint_DrawString_EN(0, 20, TimeString, &Font16, BLACK, WHITE);

	Paint_DrawLine(0, 33, EPD_2IN7_WIDTH, 33, COLORED, DOT_PIXEL_1X1, LINE_STYLE_SOLID);

	sprintf(linebuf, "%s: %s", mondata.lanportname, mondata.ipv4);
    Paint_DrawString_EN(0, 36, linebuf, &Font16, BLACK, WHITE);

	// print 2 bracketed rectangles for stronger line
	Paint_DrawRectangle(0, 52, EPD_2IN7_HEIGHT-1, EPD_2IN7_WIDTH-1, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
	Paint_DrawRectangle(1, 53, EPD_2IN7_HEIGHT-2, EPD_2IN7_WIDTH-2, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
	
	sprintf(linebuf, "Weight : %3.2f kg", bhdb.dlog[didx].HiveWeight1);
    Paint_DrawString_EN(2, 58, linebuf, &Font20, BLACK, WHITE);

	sprintf(linebuf, "TempExt: %3.1f C", bhdb.dlog[didx].TempExtern);
    Paint_DrawString_EN(2, 80, linebuf, &Font20, BLACK, WHITE);

	sprintf(linebuf, "TempHv1: %3.1f C", bhdb.dlog[didx].TempHive1);
    Paint_DrawString_EN(2, 100, linebuf, &Font20, BLACK, WHITE);
	
	sprintf(linebuf, " ");		// free/empty line
    Paint_DrawString_EN(2, 120, linebuf, &Font20, BLACK, WHITE);

	sprintf(linebuf, "Power: %1.1fV - %1.1fV", bhdb.dlog[didx].Batt3V, bhdb.dlog[didx].Batt5V);
    Paint_DrawString_EN(2, 154, linebuf, &Font20, BLACK, WHITE);
	
    /* Finally display the frame_buffer */
    EPD_2IN7_Display(BlackImage);
    DEV_Delay_ms(2000);

    printf("Goto Sleep...\r\n");
    EPD_2IN7_Sleep();

	return;
}