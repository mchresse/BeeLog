/******************************************************************************* 
 * File:   getip.c
 * Author: Randolph Esser
 * Created on 17. Januar 2017, 10:09
 *
 * Read out IP address of given portname
 *******************************************************************************
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h> //errno
#include <curl/curl.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ether.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>

#include "getini.h"
#include "get1wire.h"
#include "hx711.h"
#include "curlftp.h"
#include "beehive.h"
#include "beelog.h"
#include "epaper/fonts.h"
#include "epaper/EPD_2in7.h"
#include "epaper/GUI_Paint.h"
#include "raspimon.h"

int getipv4(char * portname, char * ipv4){ 
	struct ifreq ifr; 
    int sock_fd = socket( AF_INET , SOCK_DGRAM , 0 ); 

	if( sock_fd == -1 ){
		BHLOG(LOGMON){
			printf("    RASPIMON: %s: Error - creating socket.\n", portname);
		}
		ipv4[0]=0;
		return(-1);
	} 
     
    strcpy( ifr.ifr_name , portname ); 
    if( ioctl( sock_fd , SIOCGIFADDR , &ifr ) == -1 ) {
		BHLOG(LOGMON){
			printf("    RASPIMON:%s: problems with ioctl.\n", portname);
		}
		ipv4[0]=0;
		return(-1);
	}      
    strncpy( ipv4 , inet_ntoa( ((struct sockaddr_in *) (&ifr.ifr_addr))->sin_addr ), IPV4LEN);
	
	BHLOG(LOGMON){
		printf("    RASPIMON:%s IPv4: %s\n", portname, ipv4); 
	}
    
	close( sock_fd ); 
	return(0);
}

void getmac(char * portname, char * mymac){
	
    int sock_fd = socket( AF_INET , SOCK_DGRAM , 0 ); 
    struct ifreq ifr; 
    
	if( sock_fd == -1 ) {
		BHLOG(LOGMON){
			printf("    RASPIMON:%s: Error - creating socket.\n", portname);
		}
		mymac[0]=0;
		return;
	} 
    strcpy( ifr.ifr_name , portname ); 
    if( ioctl( sock_fd , SIOCGIFHWADDR , &ifr ) == -1 ) {
		BHLOG(LOGMON){
			printf("    RASPIMON:%s: problems with ioctl.\n", portname);
		}
		mymac[0]=0;
		return;
	} 
    strncpy( mymac , (char *) ether_ntoa( (struct ether_addr*) ifr.ifr_hwaddr.sa_data ), MACLEN);
	BHLOG(LOGMON){
		printf("    RASPIMON:%s  MAC: %s\n", portname, mymac); 
	}
    close( sock_fd );	
return;
}

/*
  * Find local ip used as source ip in ip packets.
  * Read the /proc/net/route file
  */
  

  
int getipv4_2 (char * portname, char * ipv4){
FILE *f;
char line[100] , *p , *c;
int  fm;
struct ifaddrs *ifaddr, *ifa;
int  family , s;
char host[NI_MAXHOST];

	f = fopen("/proc/net/route" , "r");
	while(fgets(line , 100 , f)) {
		p = strtok(line , " \t");
		c = strtok(NULL , " \t");
		if(p!=NULL && c!=NULL) {
			if(strcmp(c , "00000000") == 0) {
				strncpy(portname, p, 16);
				break;
			}
		}
	}
	fclose(f);

// Get IP Address
//  which family do we require , AF_INET or AF_INET6
	fm = AF_INET;  // IPv4
	if (getifaddrs(&ifaddr) == -1) {
		BHLOG(LOGMON)printf("    RASPIMON: getipv4_2 problem with getifaddrs\n"); 
		perror("getifaddrs");
//		exit(EXIT_FAILURE);
		return(-1);
	}
	
//Walk through linked list, maintaining head pointer so we can free list later
	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL) {
			continue;
		}
		family = ifa->ifa_addr->sa_family;
  
		if(strcmp( ifa->ifa_name , p) == 0){
			if (family == fm) {
				s = getnameinfo( ifa->ifa_addr, (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6),
								 host , NI_MAXHOST , NULL , 0 , NI_NUMERICHOST);
				if (s != 0) {
					BHLOG(LOGMON) printf("    RASPIMON: getnameinfo() failed: %s\n", gai_strerror(s));
					freeifaddrs(ifaddr);
					return(-1);
					// exit(EXIT_FAILURE);
				}
				strncpy(ipv4, host, IPV4LEN);
				freeifaddrs(ifaddr);
				return(0);
			}
		}
	}
	freeifaddrs(ifaddr);
		
	return 0;
}