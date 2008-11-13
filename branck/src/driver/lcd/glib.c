#include "def.h"
//#include "lcdlib.h"
#include "glib.h"
#include "lcdset.h"


void PutPixel(U32 x,U32 y,U32 c)
{
    if(x<LCD_X && y<LCD_Y)  //640x480
        frameBuffer[(y)][(x)]=( frameBuffer[(y)][(x)]
         & (0x0) | (( c)&0x00ffffff));
       
                 
         
}

//----------------------清屏函数--------------------------
void Glib_ClearScr(U32 c) 
{	
   
 int i,j;
 for(j=0;j<LCD_Y;j++)
     for(i=0;i<LCD_X;i++)
		  PutPixel(i,j,c);
		  
}
//-----------------------------------------------------------------------------
// 用c色填充屏幕
//-----------------------------------------------------------------------------
void Glib_Color_Scr(U32 c) 
{	
    //Very inefficient function.
    int i,j;

	for(j=0;j<LCD_Y;j++)
            for(i=0;i<LCD_X;i++)
		  PutPixel(i,j,c);

}


void Glib_Rectangle(int x1,int y1,int x2,int y2,int color)
{
    Glib_Line(x1,y1,x2,y1,color);
    Glib_Line(x2,y1,x2,y2,color);
    Glib_Line(x1,y2,x2,y2,color);
    Glib_Line(x1,y1,x1,y2,color);
}



void Glib_FilledRectangle(int x1,int y1,int x2,int y2,int color)
{
    int i;

    for(i=y1;i<=y2;i++)
	Glib_Line(x1,i,x2,i,color);
}



// LCD display is flipped vertically
// But, think the algorithm by mathematics point.
//   3I2
//   4 I 1
//  --+--   <-8 octants  mathematical cordinate
//   5 I 8
//   6I7
void Glib_Line(int x1,int y1,int x2,int y2,int color)
{
	int dx,dy,e;
	dx=x2-x1; 
	dy=y2-y1;
    
	if(dx>=0)
	{
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 1/8 octant
			{
				e=dy-dx/2;
				while(x1<=x2)
				{
					PutPixel(x1,y1,color);
					if(e>0){y1+=1;e-=dx;}	
					x1+=1;
					e+=dy;
				}
			}
			else		// 2/8 octant
			{
				e=dx-dy/2;
				while(y1<=y2)
				{
					PutPixel(x1,y1,color);
					if(e>0){x1+=1;e-=dy;}	
					y1+=1;
					e+=dx;
				}
			}
		}
		else		   // dy<0
		{
			dy=-dy;   // dy=abs(dy)

			if(dx>=dy) // 8/8 octant
			{
				e=dy-dx/2;
				while(x1<=x2)
				{
					PutPixel(x1,y1,color);
					if(e>0){y1-=1;e-=dx;}	
					x1+=1;
					e+=dy;
				}
			}
			else		// 7/8 octant
			{
				e=dx-dy/2;
				while(y1>=y2)
				{
					PutPixel(x1,y1,color);
					if(e>0){x1+=1;e-=dy;}	
					y1-=1;
					e+=dx;
				}
			}
		}	
	}
	else //dx<0
	{
		dx=-dx;		//dx=abs(dx)
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 4/8 octant
			{
				e=dy-dx/2;
				while(x1>=x2)
				{
					PutPixel(x1,y1,color);
					if(e>0){y1+=1;e-=dx;}	
					x1-=1;
					e+=dy;
				}
			}
			else		// 3/8 octant
			{
				e=dx-dy/2;
				while(y1<=y2)
				{
					PutPixel(x1,y1,color);
					if(e>0){x1-=1;e-=dy;}	
					y1+=1;
					e+=dx;
				}
			}
		}
		else		   // dy<0
		{
			dy=-dy;   // dy=abs(dy)

			if(dx>=dy) // 5/8 octant
			{
				e=dy-dx/2;
				while(x1>=x2)
				{
					PutPixel(x1,y1,color);
					if(e>0){y1-=1;e-=dx;}	
					x1-=1;
					e+=dy;
				}
			}
			else		// 6/8 octant
			{
				e=dx-dy/2;
				while(y1>=y2)
				{
					PutPixel(x1,y1,color);
					if(e>0){x1-=1;e-=dy;}	
					y1-=1;
					e+=dx;
				} 
			}
		}	
	}
}


//-----------------------------------------------------------------------------
//  64阶条
//-----------------------------------------------------------------------------

void StepGray64() 
{	
    int i,j,k,a=4,c;

   for(k=0;k<32;k++)
	{    	    
	  
	   for(i=0;i<7;i++)    
			for(j=0;j<272;j++)
				{
				  c=a*k*2;
				  PutPixel(i+k*15,j,(c<<16)|(c<<8)|c);
				}
	   for(i=7;i<15;i++)    
			for(j=0;j<272;j++)
				{
				  c=a*(2*k+1);
				  PutPixel(i+k*15,j,(c<<16)|(c<<8)|c);
				}				
	}				  
		  
		  
}

//-----------------------------------------------------------------------------
//千鸟
//-----------------------------------------------------------------------------
void QianNiao() 
{
 int i,j;
 for(j=0;j<272;j=j+2)
   {        
       for(i=0;i<480;i=i+2)
		 { PutPixel(i,j,0xFFFFFFFF);
		   PutPixel(i+1,j,0x00000000);
		 }
	   for(i=0;i<480;i=i+2)
		 { PutPixel(i,j+1,0x00000000);
		   PutPixel(i+1,j+1,0xFFFFFFFF);
		 }
		  
}
     
}



//------new added-------
//draw circle function
//(x,y)为圆心,r为半径,c为填充颜色
void Drawcircle(int x,int y,int r,int c) 
{
	int temp,rr,i,j;
	
	rr=r*r;
  for(i=x-r;i<x+r;i++)
    for(j=y-r;j<y+r;j++)
      {
      	temp=(i-x)*(i-x)+(j-y)*(j-y);
      	if(temp<=rr)PutPixel(i,j,c);
      
      }

}
