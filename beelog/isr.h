/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/******************************************************************************* 
 * File:   isr.h
 * Author: Randolph Esser - Copyright 2017-2019
 * Created on 17. January 2017, 10:21
 *
 * This file is part of the "beelog" program/project.
 *******************************************************************************
 */ 
 
#ifndef ISR_H
#define ISR_H

#ifdef __cplusplus
extern "C" {
#endif

volatile int reset_tara;			// the ISR event counter
int			init_tara_button(void);	// initializes and activates ISR routine
void		myTaraISR(void);		// The ISR routine itself


#ifdef __cplusplus
}
#endif

#endif /* ISR_H */

