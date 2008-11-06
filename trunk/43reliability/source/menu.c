/////////////////////////////////////////////////////////////
//main menu//
/////////////////////////////////////////////////////////////

#include "def.h"
#include "lcdlib.h"
#include "glib.h"
#include "lcd.h"
#include <stdlib.h>
#include <string.h>
#include "option.h"
#include "addr.h"
#include "lib.h"
#include "slib.h"
#include "etc.h"
#include "int.h"
#include "k9s1208.h"
#include "mmu.h"
#include "usbfifo.h"
#include "palette.h"
#include "etc.h"
#include "flash.h"
#include "pll.h"
#include "PowerOff.h"
 
#define MENU_NUM 6
extern char *Part_number;
//extern unsigned char Data_buffer[0x200];
//extern unsigned char LCD_XSIZE_TFT_800480,LCD_YSIZE_TFT_800480;
//extern unsigned int Viovol;
//extern unsigned char LCD_code,LCD_data_mode,LCD_dots,BMP_start_block,BMP_block_number;
extern volatile unsigned char Menu_status,Menu_status_1,Menu_status_2;
//extern void  Delay100ns(long int  dly);
//extern void  Delayms(long int  dly);
//extern void Display_bin(unsigned char hex,unsigned int window_start_x,unsigned  int window_start_y,unsigned long int color,char times_x,char times_y);
//extern unsigned int Adc_oprate(void);
extern void Display_string(char *string,unsigned int window_start_x,unsigned  int window_start_y,unsigned long int color,char times_x,char times_y);
extern void Display_block(unsigned int window_start_x,unsigned  int window_start_y,unsigned int block_size_x,unsigned  int block_size_y,unsigned long int color);
extern void Screen_clear(void);
extern void Flash_page_read(unsigned int flash_page_addr);
extern char gif_name[6][8];
extern char movi_name[6][8];
extern unsigned char gif_num;
extern unsigned char movi_num;

void Disp_main_menu(char menu_num,unsigned long int ground_color,unsigned long int string_color)
{
	unsigned long int buttom_highth,left_buttom_width,buttom_width,top_y,string_times,string_offset_y;
	unsigned long int left_color;
	char *topic_string;
	buttom_highth=(LCD_YSIZE_TFT_800480/MENU_NUM)-1;
	left_buttom_width=buttom_highth/2;
	buttom_width=LCD_XSIZE_TFT_800480-left_buttom_width-1;
	top_y=(LCD_YSIZE_TFT_800480-(buttom_highth*MENU_NUM)-5)/2;
	string_times=buttom_highth/8;  if(string_times>2) string_times=2; 
	string_offset_y=(buttom_highth-(string_times*8))/2;
	
	switch(menu_num)
	{
		case 0: 	{left_color=0x00ff;topic_string="Image";}
				break;
		case 1: 	{left_color=0x0100;topic_string="Movie";}
				break;
		case 2: 	{left_color=0x0200;topic_string="Camera";}
				break;
		case 3:		{left_color=0x0300;topic_string="Pattern";}
				break;
		case 4:		{left_color=0x0500;topic_string="Extra";}
				break;					
		default:    {left_color=0x0700;topic_string="Download";}
	 	}
	
	
	Display_block(0,top_y+(buttom_highth+1)*menu_num,left_buttom_width,buttom_highth,left_color);
	Display_block(left_buttom_width,top_y+(buttom_highth+1)*menu_num,buttom_width,buttom_highth,ground_color);
	Display_string(topic_string,left_buttom_width+1,top_y+(buttom_highth+1)*menu_num+string_offset_y,string_color,string_times,string_times);
	   
}

void Main_menu(char menu_num)
{
	char i;
	unsigned long int ground_color, string_color;
	Screen_clear();
	for (i=0;i<MENU_NUM;i++)
	{
		if(i==menu_num)
		{
	//		ground_color=MENU2_GROUND_COLOR;
	//		string_color=MENU2_STRING_COLOR;
		}
		else
		{
		//	ground_color=MENU1_GROUND_COLOR;
		//	string_color=MENU1_STRING_COLOR;
		}
		Disp_main_menu(i,ground_color,string_color);
	}
	
}

void Disp_download_menu(char download_num,unsigned long int ground_color,unsigned long int string_color)
{
	unsigned int buttom_highth,left_buttom_width,buttom_width,top_y,string_times,string_offset_y;
	unsigned long int left_color;
	char *topic_string;
	buttom_highth=(LCD_YSIZE_TFT_800480/MENU_NUM)-1;
	left_buttom_width=buttom_highth/2;
	buttom_width=LCD_XSIZE_TFT_800480-left_buttom_width-1;
	top_y=(LCD_YSIZE_TFT_800480-(buttom_highth*MENU_NUM)-5)/2;
	string_times=buttom_highth/8;  if(string_times>2) string_times=2; 
	string_offset_y=(buttom_highth-(string_times*8))/2;
	
	switch(download_num)
	{
		case 0: 	{left_color=0x00FF0000;topic_string="96*64";}
				break;
		case 1: 	{left_color=0x00FFFF00;topic_string="128*128";}
				break;
		case 2: 	{left_color=0x0000FF00;topic_string="128*160";}
				break;
		case 3:		{left_color=0x0000FFFF;topic_string="176*220";}
				break;
		case 4:		{left_color=0x00FF00FF;topic_string="240*320";}
				break;					
		default:    {left_color=0x00FF8080;topic_string="Movie";}
	 }
	
	
	Display_block(0,top_y+(buttom_highth+1)*download_num,left_buttom_width,buttom_highth,left_color);
	Display_block(left_buttom_width,top_y+(buttom_highth+1)*download_num,buttom_width,buttom_highth,ground_color);
	Display_string(topic_string,left_buttom_width+1,top_y+(buttom_highth+1)*download_num+string_offset_y,string_color,string_times,string_times);
		
}

void Dowload_menu(char menu_num)
{
	
	char i;
	unsigned long int ground_color, string_color;
	Screen_clear();
	for (i=0;i<MENU_NUM;i++)
	{
		if(i==menu_num)
		{
		//	ground_color=MENU2_GROUND_COLOR;
	//		string_color=MENU2_STRING_COLOR;
		}
		else
		{
	//		ground_color=MENU1_GROUND_COLOR;
	//		string_color=MENU1_STRING_COLOR;
		}
		Disp_download_menu(i,ground_color,string_color);
	}
}

void Disp_pattern_menu(char pattern_num,char current_num,unsigned long int ground_color,unsigned long int string_color)
{
	unsigned int buttom_highth,left_buttom_width,buttom_width,top_y,string_times,string_offset_y;
	unsigned long int left_color;
	char *topic_string;
	buttom_highth=(LCD_YSIZE_TFT_800480/MENU_NUM)-1;
	left_buttom_width=buttom_highth/2;
	buttom_width=LCD_XSIZE_TFT_800480-left_buttom_width-1;
	top_y=(LCD_YSIZE_TFT_800480-(buttom_highth*MENU_NUM)-5)/2;
	string_times=buttom_highth/8;  if(string_times>2) string_times=2; 
	string_offset_y=(buttom_highth-(string_times*8))/2;
	
	switch(pattern_num)
	{
		case 0: 	{left_color=0x0100;topic_string="white";}
				break;
		case 1: 	{left_color=0x0200;topic_string="black";}
				break;
		case 2: 	{left_color=0x0300;topic_string="red";}
				break;
		case 3:		{left_color=0x0400;topic_string="green";}
				break;
		case 4:		{left_color=0x0500;topic_string="blue";}
				break;	
		case 5: 	{left_color=0x0600;topic_string="checker";}
				break;
		case 6: 	{left_color=0x0700;topic_string="gray";}
				break;
		case 7:		{left_color=0x0000;topic_string="cross";}
				break;
		case 8:		{left_color=0x0100;topic_string="column";}
				break;
		case 9:		{left_color=0x0200;topic_string="common";}
				break;
		case 10: 	{left_color=0x0300;topic_string="white";}
				break;
		case 11: 	{left_color=0x0400;topic_string="black";}
				break;
		case 12: 	{left_color=0x0500;topic_string="red";}
				break;
		case 13:	{left_color=0x0600;topic_string="green";}
				break;
		case 14:	{left_color=0x0700;topic_string="blue";}
				break;	
											
											
		default:    ;
	 }
	
	
	Display_block(0,top_y+(buttom_highth+1)*(pattern_num-current_num),left_buttom_width,buttom_highth,left_color);
	Display_block(left_buttom_width,top_y+(buttom_highth+1)*(pattern_num-current_num),buttom_width,buttom_highth,ground_color);
	Display_string(topic_string,left_buttom_width+1,top_y+(buttom_highth+1)*(pattern_num-current_num)+string_offset_y,string_color,string_times,string_times);
		
}

void Disp_pattern_menu_1(char pattern_num,char current_num,unsigned long int ground_color,unsigned long int string_color)
{
	unsigned int buttom_highth,left_buttom_width,buttom_width,top_y,string_times,string_offset_y;
	unsigned short int left_color;
	char *topic_string;
	buttom_highth=(LCD_YSIZE_TFT_800480/MENU_NUM)-1;
	left_buttom_width=buttom_highth/2;
	buttom_width=LCD_XSIZE_TFT_800480-left_buttom_width-1;
	top_y=(LCD_YSIZE_TFT_800480-(buttom_highth*MENU_NUM)-5)/2;
	string_times=buttom_highth/8;  if(string_times>2) string_times=2; 
	string_offset_y=(buttom_highth-(string_times*8))/2;
	
	switch(pattern_num)
	{
		case 0: 	{left_color=0x0100;topic_string="white";}
				break;
		case 1: 	{left_color=0x0200;topic_string="black";}
				break;
		case 2: 	{left_color=0x0300;topic_string="red";}
				break;
		case 3:		{left_color=0x0400;topic_string="green";}
				break;
		case 4:		{left_color=0x0500;topic_string="blue";}
				break;	
		case 5: 	{left_color=0x0600;topic_string="checker";}
				break;
		case 6: 	{left_color=0x0700;topic_string="gray";}
				break;
		case 7:		{left_color=0x0000;topic_string="cross";}
				break;
		case 8:		{left_color=0x0100;topic_string="column";}
				break;
		case 9:		{left_color=0x0200;topic_string="common";}
				break;
		case 10: 	{left_color=0x0300;topic_string="white";}
				break;
		case 11: 	{left_color=0x0400;topic_string="black";}
				break;
		case 12: 	{left_color=0x0500;topic_string="red";}
				break;
		case 13:	{left_color=0x0600;topic_string="green";}
				break;
		case 14:	{left_color=0x0700;topic_string="blue";}
				break;	
											
		default:    ;
	 }
	
	
	Display_block(left_buttom_width,top_y+(buttom_highth+1)*(pattern_num-current_num),buttom_width,buttom_highth,ground_color);
	Display_string(topic_string,left_buttom_width+1,top_y+(buttom_highth+1)*(pattern_num-current_num)+string_offset_y,string_color,string_times,string_times);
		
}


void Pattern_menu(char menu_num)
{
	
	char i;
	unsigned long int ground_color, string_color;
	Screen_clear();
	for (i=menu_num;i<menu_num+MENU_NUM;i++)
	{
		if(i==menu_num)
		{
	//		ground_color=MENU2_GROUND_COLOR;
	//		string_color=MENU2_STRING_COLOR;
		}
		else
		{
	//		ground_color=MENU1_GROUND_COLOR;
		//	string_color=MENU1_STRING_COLOR;
		}
		Disp_pattern_menu(i,menu_num,ground_color,string_color);
	}
}

void Pattern_menu_1(char menu_num)
{
	
	char i;
	unsigned long int ground_color, string_color;
	for (i=menu_num;i<menu_num+MENU_NUM;i++)
	{
		if(i==menu_num)
		{
	//		ground_color=MENU2_GROUND_COLOR;
	//		string_color=MENU2_STRING_COLOR;
		}
		else
		{
	//		ground_color=MENU1_GROUND_COLOR;
	//		string_color=MENU1_STRING_COLOR;
		}
		Disp_pattern_menu_1(i,menu_num,ground_color,string_color);
	}
}
 
void Disp_movie_menu(char download_num,unsigned long int ground_color,unsigned long int string_color)
{
	unsigned int buttom_highth,left_buttom_width,buttom_width,top_y,string_times,string_offset_y;
	unsigned long int left_color;
	char *topic_string;
	char *ts[6];
	unsigned char c[4];
	unsigned char i;
	
	
	c[0]='M';
	c[1]='O';
	c[2]='V';
	c[3]='I';
	movi_num=file_num(c);
	buttom_highth=(LCD_YSIZE_TFT_800480/MENU_NUM)-1;
	left_buttom_width=buttom_highth/2;
	buttom_width=LCD_XSIZE_TFT_800480-left_buttom_width-1;
	top_y=(LCD_YSIZE_TFT_800480-(buttom_highth*MENU_NUM)-5)/2;
	string_times=buttom_highth/8;  if(string_times>2) string_times=2; 
	string_offset_y=(buttom_highth-(string_times*8))/2;
	
	switch(download_num)
	{
		case 0: 	{left_color=0x00FF0000;topic_string=&movi_name[0][0];*(topic_string+8)='\0';}
				break;
		case 1: 	{left_color=0x00FFFF00;topic_string=&movi_name[1][0];*(topic_string+8)='\0';}
				break;
		case 2: 	{left_color=0x0000FF00;topic_string=&movi_name[2][0];*(topic_string+8)='\0';}
				break;
		case 3:		{left_color=0x0000FFFF;topic_string=&movi_name[3][0];*(topic_string+8)='\0';}
				break;
		case 4:		{left_color=0x00FF00FF;topic_string=&movi_name[4][0];*(topic_string+8)='\0';}
				break;					
		default:    {left_color=0x00FF8080;topic_string=&movi_name[5][0];*(topic_string+8)='\0';}
	 }
	
	Display_block(0,top_y+(buttom_highth+1)*download_num,left_buttom_width,buttom_highth,left_color);
	Display_block(left_buttom_width,top_y+(buttom_highth+1)*download_num,buttom_width,buttom_highth,ground_color);
	Display_string(topic_string,left_buttom_width+1,top_y+(buttom_highth+1)*download_num+string_offset_y,string_color,string_times,string_times);
		
}

void Movie_menu(char menu_num,char n)
{
	
	char i;
	unsigned long int ground_color, string_color;
	Screen_clear();

	for (i=0;i<n;i++)
	{
		if(i==menu_num)
		{
		//	ground_color=MENU2_GROUND_COLOR;
		//	string_color=MENU2_STRING_COLOR;
		}
		else
		{
		//	ground_color=MENU1_GROUND_COLOR;
		//	string_color=MENU1_STRING_COLOR;
		}
		Disp_movie_menu(i,ground_color,string_color);
	}
}



void Insonstructing_menu()
{
	
	Screen_clear();
	Display_string("Sorry",(LCD_x_size-30)/2,(LCD_y_size-32)/2,0x00FFFF00,1,2);
	Display_string("In constructing",(LCD_x_size-90)/2,LCD_y_size/2,0x00FFFF00,1,2);
	
}


void extra_menu()
{
	Message_menu();
	
}
