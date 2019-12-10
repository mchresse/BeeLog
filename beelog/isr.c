/* =============================================================================
 Name        : ISR.c
 Version     : 1.0
 Copyright	 : Randolph Esser, 2017

 Description :
     Raspberry Pi ISR for tara button action

============================================================================= */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <wiringPi.h>

#include "isr.h"
#include "getini.h"
#include "get1wire.h"
#include "beehive.h"
#include "beelog.h"


// the ISR event counter 
// volatile int reset_tara = 0; // defined in hx711.c

// -------------------------------------------------------------------------
// myInterrupt:  called every time an event occurs
void myTaraISR(void) {
   reset_tara=1;
}

// -------------------------------------------------------------------------
int init_tara_button(void) {
int errno;
// sets up the wiringPi library
//	if (wiringPiSetup () < 0) {
//		fprintf (stderr, "Unable to setup wiringPi: %s\n", strerror (errno));
//		return 1;
//	}
	reset_tara = 0;
#ifndef BHSIM  	
  // set Tara Button_Pin 17/0 generates an interrupt on high-to-low transitions
  // and attach ISR() to the interrupt
	if ( errno = wiringPiISR (cfgini->hc_taraiopin, INT_EDGE_FALLING, &myTaraISR) < 0 ) {
		fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
		return 1;
	}
#endif
// check tara button event every second.
//	printf("Waiting for TaraButton:");
//	while ( reset_tara == 0 ) {  
//		delay( 1000 ); // wait 1 second
//		printf(".");
//	}
	
	return 0;
}