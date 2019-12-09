/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
/* 
 * File:   epdwrapper.c
 * Author: Randolph Esser - Copyright 2017-2019
 * Created on 17. Januar 2017, 10:09
 *
 * This file is part of the "beelog" program/project.
 *
 * Wrapper functions C->Cpp or all E-Paper functions of as provided by the files
 * epd/epd2in7.cpp and epd/epdpaint.cpp
 */

#include <stdio.h>
#include <stdlib.h>

#include "epaper/fonts.h"
#include "epaper/epdif.h"
#include "epaper/epd2in7.h"
#include "epaper/epdpaint.h"

#include "epdwrapper.h"


//	Real methods / functions
extern "C" {

Epd		epd;
Paint	paint;

// Konstruktor
//	Paint(unsigned char* image, int width, int height);
	void paint_Init0(Paint* p, unsigned char* frame_buffer, int width, int height){
		p->Init(frame_buffer, width, height);
	};
	
	Paint* paint_callback(void){
		return &paint;
	};
	
// Destruktor
//	~Paint();
	void paint_destructor(Paint* p){
		p->~Paint();
	};
	
// All Paint Methods:	
	void paint_Clear(Paint* p, int col){
		p->Clear(col);
	};  
	int paint_GetWidth(Paint* p){
		return( p->GetWidth() );
	};
	void paint_SetWidth(Paint* p, int width){
		p->SetWidth(width);
	};
    int  paint_GetHeight(Paint* p){
	    return( p->GetHeight() );
	};
    void paint_SetHeight(Paint* p,int height){
		p->SetHeight(height);
	};
    int paint_GetRotate(Paint* p){
		return( p->GetRotate() );
	};
    void paint_SetRotate(Paint* p,int rotate){
		p->SetRotate(rotate);
	};
    unsigned char* paint_GetImage(Paint* p){
		return( p->GetImage() );
	};
    void paint_DrawAbsolutePixel(Paint* p,int x, int y, int colored){
		p->DrawAbsolutePixel(x,y,colored);
	};
    void paint_DrawPixel(Paint* p,int x, int y, int colored){
		p->DrawPixel(x, y, colored);
	};
    void paint_DrawCharAt(Paint* p,int x, int y, char ascii_char, sFONT* font, int colored){
		p->DrawCharAt(x, y, ascii_char, font, colored);
	};
    void paint_DrawStringAt(Paint* p,int x, int y, const char* text, sFONT* font, int colored){
		p->DrawStringAt(x, y, text, font, colored);
	};
    void paint_DrawLine(Paint* p,int x0, int y0, int x1, int y1, int colored){
		p->DrawLine( x0, y0, x1, y1, colored);
	};
    void paint_DrawHorizontalLine(Paint* p,int x, int y, int width, int colored){
		p->DrawHorizontalLine( x, y, width, colored);
	};
    void paint_DrawVerticalLine(Paint* p,int x, int y, int height, int colored){
    	p->DrawVerticalLine( x, y, height, colored);
	};
    void paint_DrawRectangle(Paint* p,int x0, int y0, int x1, int y1, int colored){
    	p->DrawRectangle( x0, y0, x1, y1, colored);
	};
    void paint_DrawFilledRectangle(Paint* p,int x0, int y0, int x1, int y1, int colored){
    	p->DrawFilledRectangle( x0, y0, x1, y1, colored);
	};
    void paint_DrawCircle(Paint* p,int x, int y, int radius, int colored){
    	p->DrawCircle(x, y, radius, colored);
	};
    void paint_DrawFilledCircle(Paint* p,int x, int y, int radius, int colored){
    	p->DrawFilledCircle(x, y, radius, colored);
	};


// Class:EPD Methods
// Konstruktor subst.
    int	epd_Init(Epd* p){
		return ( p->Init() );
	};
//	   ~Epd();		// EPD Destruktor
    void epd_Destructor(void){
//		epd.~Epd();
	};

	Epd* epd_callback(void){
		return &epd;
	};

    void epd_SendCommand(Epd* p,unsigned char command){
		p->SendCommand(command);
	};
    void epd_SendData(Epd* p,unsigned char data){
		p->SendData(data);
	};
    void epd_WaitUntilIdle(Epd* p){
		p->WaitUntilIdle();
	};
    void epd_Reset(Epd* p){
		p->Reset();
	};
    void epd_SetLut(Epd* p){
		p->SetLut();
	};
    void epd_DisplayFrame(Epd* p, const unsigned char* frame_buffer){
		p->DisplayFrame(frame_buffer);
	};
    void epd_Sleep(Epd* p){	    /* Display the frame_buffer */
		p->Sleep();
	};

}; // end of "extern C"
   
    
