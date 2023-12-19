#ifndef __DRAW_H
#define __DRAW_H

void DrawString(unsigned char x, unsigned char y,char *string);
void DrawBitMap(unsigned char x, unsigned char  y, const unsigned char * bitmap, unsigned char  width, unsigned char  heigth);
void DrawInit(void);
void Drawend(void);
void DrawFlushArea(unsigned char x, unsigned char y, unsigned char width, unsigned char heigth);

#endif

