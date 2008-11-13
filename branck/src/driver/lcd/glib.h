#ifndef __GLIB_H__
#define __GLIB_H__

void Glib_Line(int x1,int y1,int x2,int y2,int color);
void Glib_Rectangle(int x1,int y1,int x2,int y2,int color);
void Glib_FilledRectangle(int x1,int y1,int x2,int y2,int color);
void Glib_ClearScr(U32 c);
void Glib_Color_Scr(U32 c);

void PutPixel(U32 x,U32 y,U32 c);
//void Ramchange(void);
//extern void (*PutPixel)(U32,U32,U32);

void StepGray64(void);
void QianNiao(void);
void Drawcircle(int x,int y,int r,int c);

#endif //__GLIB_H__
