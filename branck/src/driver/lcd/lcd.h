#ifndef __LCD_H__
#define __LCD_H__

void Lcd_Init(void);
void Lcd_EnvidOnOff(int onoff);
void Lcd_Lpc3600Enable(void);
void Lcd_PowerEnable(int invpwren,int pwren);

#endif /*__LCD_H__*/
