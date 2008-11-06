#include "def.h"
#include "option.h"
#include "addr.h"
#include "lib.h"
#include "sdi.h"
#include "lcdlib.h"
 
extern unsigned long int Data_start_Addr;
extern unsigned int RootDirAddr;
extern unsigned int RootDirNum;
extern unsigned int RsvdSec;
extern unsigned char SecPerClus;
extern unsigned char LCD_code;
extern unsigned int LCD_x_size,LCD_y_size;
extern int *Image_data;
extern unsigned char Data_buffer[0x200];
//extern int MODE_TFT_16BIT_800480;
extern void Glib_ClearScr(U32 c,int type);
extern void BMP_data_from_flash(unsigned long int data_addr,unsigned long int flash_page_addr);
//void LCD_ramwrite_command(unsigned int window_start_x,unsigned  int window_start_y,unsigned int window_size_x,unsigned  int window_size_y)
//{
//	switch (LCD_code)
//	{
//
//		case 0: 	//LCD_ramwrite_command_LQ020B3UD04(window_start_x, window_start_y, window_size_x, window_size_y);
//		  break;
//		default:	//LCD_ramwrite_command_CGS(window_start_x,window_start_y,window_size_x,window_size_y);
//		  break;
//	}	
//}


/*void Display_image(unsigned long int page_addr)
{

	Image_data=(int *)0x33000000;
    BMP_data_from_flash(Image_data,page_addr);    //BMP_data_from_flash(VRAM0,page_addr);

   Uart_Init(0,115200);      
   Test_Uart1_Int();


   
   View_Rx_buf();
    Card_sel_desel(0);	// Card deselect

    if(!CMD9())
    Led_Display(8);
	//Uart_Printf("Get CSD fail!!!\n");
    rSDIDCON=0;//tark???
    rSDICSTA=0xffff;
    Glib_ClearScr(0, MODE_TFT_16BIT_800480);// wangxiaohui  add
    Glib_ClearScr(0xf800, MODE_TFT_16BIT_800480);
     //  Glib_ClearScr(0x07e0, MODE_TFT_16BIT_800480);
     //  Glib_ClearScr(0x001f, MODE_TFT_16BIT_800480);
     //  Glib_ClearScr(0xffff, MODE_TFT_16BIT_800480);         
         
}
*/
///////////////////////////////////////////////////////////
/*void Trans_window_data_to_LCD(unsigned int window_start_x,unsigned int window_start_y,unsigned int image_width,unsigned int image_highth)
{
	unsigned short  int i,j,tem;
	unsigned  volatile long int addr;
	//A20_LOW;

	addr=VRAM0+window_start_y*LCD_x_size+window_start_x;
	for (i=window_start_y;i<image_highth+window_start_y;i=i+1)
	{
		for (j=window_start_x;j<image_width+window_start_x;j=j+1)
		{	
			CTL_MEM[addr]=Image_data[addr];
			addr++;	
		}
		addr=addr+LCD_x_size-image_width;
					
	} 
}
	*/
//////////////////////////////////////////////////////////
void Display_logo(int ii)
{  
	unsigned char Root_Directory[32];
	unsigned char c[4];
	unsigned char m[4];
	unsigned long int page_addr;
	unsigned int block;
	unsigned int i;
	unsigned int j;
	unsigned int k;
	unsigned char h;
	unsigned long int data,addr_offset;
	unsigned long int current_sector_addr,next_sector_addr,fat_page_addr;
	unsigned int sector_num;
	c[0]=0x42;
	c[1]=0x4d;
	c[2]=0x50;
	c[3]=0x20; 
	
	m[0]=0x32; 
	m[1]=0x34;
	m[2]=0x33;
	m[3]=0x32;
   for(h=ii;h<ii+1;h++)   //1~4 ¸£Í¼¼´ÊäÈëi=1~4
	{      	
       // h=6;	
     
		page_addr=RootDirAddr;
		
		for(k=0;k<RootDirNum/16;k++)//¸ùÄ¿Â¼32¸öÉÈÇø
		{  //ÕÒ¸ùÄ¿Â¼
		    block=1;
		    Uart_Printf("\nsd ok\n");
			 Rd_Block(page_addr*512);//µØÖ·²»±äå£¿
			 
			for(i=0;i<512/32;i++)//¶Á1¸öÉÈÇø
			{
				for(j=0;j<32;j++)
				{	//Ä¿Â¼Æ¥Åä
					Root_Directory[j]=Data_buffer[i*32+j];
				}
				
				if(Root_Directory[0]==c[0])
					if(Root_Directory[1]==c[1])
				   		if(Root_Directory[2]==c[2])
						   if(Root_Directory[3]==c[3])
						    {  
						       Uart_Printf("%c ,  ",Root_Directory[0]);//////////////yys//////////////////
						       Uart_Printf("%c ,  ",Root_Directory[1]);//////////////yys/////////////////
						       Uart_Printf("%c ,  ",Root_Directory[2]);//////////////yys/////////////////
						       Uart_Printf("%c ,  ",Root_Directory[3]);//////////////yys////////////////
							  goto rr;
							 }
			}	
			page_addr++;	
		}
	
rr:	page_addr=Data_start_Addr+Root_Directory[27]*256*SecPerClus+Root_Directory[26]*SecPerClus-64;	//µÚ2´Ø£¨Ê×´Ø£©512M(32)/1G(64)YYS
	while(1)
	{//ÕÒ×ÓÄ¿Â¼
	  for(k=0;k<SecPerClus;k++)
	   { block=1;
	     Rd_Block(page_addr*512);
		 for(i=0;i<512/32;i++)
		 {
			for(j=0;j<32;j++)
			{	
				Root_Directory[j]=Data_buffer[i*32+j];
			}
			
			if(Root_Directory[0]==m[0])
				if(Root_Directory[1]==m[1])
					if(Root_Directory[2]==m[2])
						if(Root_Directory[3]==m[3])
							{
							 Uart_Printf("%c ,  ",Root_Directory[0]);//////////////yys//////////////////
						     Uart_Printf("%c ,  ",Root_Directory[1]);//////////////yys/////////////////
						     Uart_Printf("%c ,  ",Root_Directory[2]);//////////////yys/////////////////
						     Uart_Printf("%c ,  ",Root_Directory[3]);//////////////yys/////////////////
							goto rr1;
							}	

		 }
		 page_addr++;
	   }
	}
	
rr1:page_addr=Data_start_Addr+Root_Directory[27]*256*SecPerClus+Root_Directory[26]*SecPerClus-64;	//µÚ3´Ø512M(32)/1G(64)YYS	
	 while(1)
	{//ÕÒÎÄ¼þ
	  for(k=0;k<SecPerClus;k++)  
	   {  block=1;
		  Rd_Block(page_addr*512);
		for(i=0;i<512/32;i++)
		 {
			for(j=0;j<32;j++)
			{	
				Root_Directory[j]=Data_buffer[i*32+j];
			}
			if(Root_Directory[0]==0x30)
				if(Root_Directory[1]==(0x30+h/10))
					if(Root_Directory[2]==(0x30+h%10))
							{
							 Uart_Printf("%c ,  ",Root_Directory[0]);//////////////yys//////////////////
						     Uart_Printf("%c ,  ",Root_Directory[1]);//////////////yys/////////////////
						     Uart_Printf("%c ,  ",Root_Directory[2]);//////////////yys/////////////////
						    goto rr2;	
						    }					
		 }	//001	
		page_addr++;
	   }
	 }
		 

			
rr2:  page_addr=Data_start_Addr+Root_Directory[27]*256*SecPerClus+Root_Directory[26]*SecPerClus-46;//512M(32)/1G(46)YYS     
	  BMP_data_from_flash(VRAM0,page_addr);	//Image_data[0X33000000-¡·]
	  
      Ramchange();
    //  Delay(50000);
	}			
//end: ;
	//	file_addr=searchfile(c,m,j&0x07);
		
		
}

