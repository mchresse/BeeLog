/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/******************************************************************************* 
 * File:   raspimon.h
 * Author: Randolph Esser - Copyright 2017-2019
 * Created on 3. März 2017
 * This file is part of the "beelog" program/project.
 *******************************************************************************
 */ 
 
#ifndef RASPIMON_H
#define RASPIMON_H

#ifdef __cplusplus
extern "C" {
#endif

#define LANPORTLEN 40
#define IPV4LEN	32			// must be >= NI_MAXHOST
#define MACLEN	20

typedef struct {
	char lanportname[LANPORTLEN];	// LAN Portname buffer
	char ipv4[IPV4LEN];				// ip v4 address string buffer
	char mymac[MACLEN];				// MAC address string buffer
	unsigned long uptime;
	unsigned long avgCpuLoad;
	unsigned long totalRam;
	double	tcpu;	
} mondat;

void	raspimon(void);
int		getipv4(char * portname, char * ipv4);
void	getmac (char * portname, char * mymac);
int		getipv4_2 (char * portname, char * ipv4);
void	PutStartScreen(Epd* pepd, Paint* ppaint, unsigned char * fbuf, int didx);


#ifdef __cplusplus
}
#endif

#endif /* RASPIMON_H */

