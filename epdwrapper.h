/* 
 * File:   epdwrapper.h
 * Author: MCHREsse
 *
 * Created on 14. September 2018, 09:46
 */

#ifndef EPDWRAPPER_H
#define EPDWRAPPER_H

#define COLORED      1
#define UNCOLORED    0

#ifdef __cplusplus
extern "C" {  // Dies sorgt dafür, dass der Header sowohl in C als auch in C++ funktioniert
#endif

// list up all EPD C-functions here ...
			Epd* epd_callback(void);
	//  Epd();		// EPD Konstruktor
extern int   epd_Init			( Epd*);
	// ~Epd();		// EPD Destruktor
extern void  epd_Destructor		(void);

extern  void epd_SendCommand	(struct Epd*,unsigned char command);
extern  void epd_SendData		(struct Epd*,unsigned char data);
extern  void epd_WaitUntilIdle	(struct Epd*);
extern  void epd_Reset			(struct Epd*);
extern	void epd_SetLut			(struct Epd*);
extern  void epd_DisplayFrame	(struct Epd*,const unsigned char* frame_buffer);
extern  void epd_Sleep			(struct Epd*);
	
//    Paint(unsigned char* image, int width, int height);	// Konstruktor
			Paint* paint_callback	(void);
extern		void paint_Init0		(Paint* p, unsigned char* framebuffer, int width, int height);
//    ~Paint();
extern		void paint_destructor	(Paint* p);
	
extern		void paint_Clear		(struct Paint* p, int col);
extern		int  paint_GetWidth		(struct Paint* p);
extern     void paint_SetWidth		(struct Paint* p, int width);
extern     int  paint_GetHeight	(struct Paint* p);
extern     void paint_SetHeight	(struct Paint* p, int height);
extern     int  paint_GetRotate	(struct Paint* p);
extern     void paint_SetRotate	(struct Paint* p, int rotate);
extern     unsigned char* paint_GetImage	(struct Paint* p);
extern     void paint_DrawAbsolutePixel	(struct Paint* p, int x, int y, int colored);
extern     void paint_DrawPixel			(struct Paint* p, int x, int y, int colored);
extern     void paint_DrawCharAt			(struct Paint* p, int x, int y, char ascii_char, sFONT* font, int colored);
extern     void paint_DrawStringAt			(struct Paint* p, int x, int y, const char* text, sFONT* font, int colored);
extern     void paint_DrawLine				(struct Paint* p, int x0, int y0, int x1, int y1, int colored);
extern     void paint_DrawHorizontalLine	(struct Paint* p, int x, int y, int width, int colored);
extern     void paint_DrawVerticalLine		(struct Paint* p, int x, int y, int height, int colored);
extern     void paint_DrawRectangle		(struct Paint* p, int x0, int y0, int x1, int y1, int colored);
extern     void paint_DrawFilledRectangle	(struct Paint* p, int x0, int y0, int x1, int y1, int colored);
extern     void paint_DrawCircle			(struct Paint* p, int x, int y, int radius, int colored);
extern     void paint_DrawFilledCircle		(struct Paint* p, int x, int y, int radius, int colored);

#ifdef __cplusplus
}
#endif

#endif /* EPDWRAPPER_H */

