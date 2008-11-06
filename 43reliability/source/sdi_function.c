#include <stdio.h>
#include <string.h>
#include "def.h"
#include "option.h"
#include "addr.h"
#include "lib.h"
#include "sdi.h"
unsigned long int RootDirAddr;
unsigned long int Data_start_Addr;
unsigned int RootDirNum;
unsigned int RsvdSec;
unsigned char SecPerClus;
unsigned char Root_Directory[32];
extern  int *Image_data;
extern  volatile unsigned int block;
//extern  int *Tx_buffer;	//128[word]*16[blk]=8192[byte]
extern  int *Rx_buffer;	//128[word]*16[blk]=8192[byte]
unsigned char Data_buffer[0x200];
extern  char gif_name[6][8];
extern  char movi_name[6][8];
void Read_BPB(void)
{	
	unsigned int start_sec_addr;
	unsigned int Total_Sec; 	
	unsigned short int Fatsz16;
	
	block=1;
	
	Rd_Block(0x1f200);    //Flash_page_read(0);	//512M(0x1ce00)/1G(0x1f200)YYS 
	//View_Rx_buf();//
	//Rd_Block(0,0x20); 
	SecPerClus=Data_buffer[13];
	RsvdSec=Data_buffer[15]*256+Data_buffer[14];
	RootDirNum=Data_buffer[18]*256+Data_buffer[17];//0x2e*256+0x2e;
	Total_Sec=(unsigned int)((Data_buffer[20]*256+Data_buffer[19])&0x0000ffff);
	Fatsz16=(unsigned short int)(Data_buffer[23]*256+Data_buffer[22]);
	if(Total_Sec==0x00000000)
	{
		Total_Sec=(unsigned int)(Data_buffer[35]*(0x1000000)+Data_buffer[34]*(0x10000)+Data_buffer[33]*(0x100)+Data_buffer[32]);
	}
	RootDirAddr=Fatsz16*2+RsvdSec+231;
	Data_start_Addr=RootDirAddr+RootDirNum/16;	      //      RootDirNum/16=(RootDirNum*2)/32;	
	
}


/*
unsigned long int  searchfile(unsigned char c[4],unsigned char m[4],unsigned char n)
{
	unsigned int page_addr;
	unsigned int file_addr;
	unsigned short int i;
	unsigned char j;
	unsigned char k;
	
	unsigned int fat_page_addr;
	unsigned int current_sector_addr;
	unsigned int next_sector_addr;
	unsigned int sector_num;	
	unsigned short int FAT[256];

	page_addr=RootDirAddr;
	for(k=0;k<RootDirNum/16;k++)
	{
		block=1;
		Rx_buffer=(int *)0x31800000;
		Rd_Block(0,page_addr*512);     //
		View_Rx_buf();
		for(i=0;i<512/32;i++)
		{
			for(j=0;j<32;j++)
			{	
				Root_Directory[j]=Data_buffer[i*32+j];
			}
			if(Root_Directory[0]==c[0])
				if(Root_Directory[1]==c[1])
				   if(Root_Directory[2]==c[2])
				   	  if(Root_Directory[3]==c[3])
						  goto rr;
		}
		page_addr=page_addr+512;       //page_addr++;
	}
	return 0;
rr:	page_addr=Data_start_Addr+Root_Directory[27]*256*SecPerClus+Root_Directory[26]*SecPerClus-4;
	while(1)
	{
	    block=1;
		Rd_Block(0,page_addr*512);      //Flash_page_read(page_addr);
		for(i=0;i<512/32;i++)
		{
			for(j=0;j<32;j++)
			{	
				Root_Directory[j]=Data_buffer[i*32+j];
			}
			if(m!=0)
			{
			if(Root_Directory[0]==m[0])
				if(Root_Directory[1]==m[1])
					if(Root_Directory[2]==m[2])
						if(Root_Directory[3]==m[3])
							goto rr1;	
			}				 
			else
			{	
			if(Root_Directory[0]==gif_name[n][0])
				if(Root_Directory[1]==gif_name[n][1])			
					if(Root_Directory[2]==gif_name[n][2])
						if(Root_Directory[3]==gif_name[n][3])
							if(Root_Directory[4]==gif_name[n][4])
								if(Root_Directory[5]==gif_name[n][5])			
									if(Root_Directory[6]==gif_name[n][6])
										if(Root_Directory[7]==gif_name[n][7])
											goto rr2;
			}
		}
		for(k=0;k<SecPerClus-1;k++)
		{	
			page_addr=page_addr+512;       //page_addr++;
			block=1;
			Rd_Block(0,page_addr*512);           //Flash_page_read(page_addr);
			for(i=0;i<512/32;i++)
			{
				for(j=0;j<32;j++)
				{	
					Root_Directory[j]=Data_buffer[i*32+j];
				}
				if(m!=0)
				{
				if(Root_Directory[0]==m[0])
					if(Root_Directory[1]==m[1])
						if(Root_Directory[2]==m[2])
							if(Root_Directory[3]==m[3])
								goto rr1;	
				}		
				else
				{		 
				if(Root_Directory[0]==gif_name[n][0])
					if(Root_Directory[1]==gif_name[n][1])			
						if(Root_Directory[2]==gif_name[n][2])
							if(Root_Directory[3]==gif_name[n][3])
								if(Root_Directory[4]==gif_name[n][4])
									if(Root_Directory[5]==gif_name[n][5])			
										if(Root_Directory[6]==gif_name[n][6])
											if(Root_Directory[7]==gif_name[n][7])
												goto rr2;
				}
			}
		}
		current_sector_addr=(page_addr-Data_start_Addr+16)/SecPerClus;	
		fat_page_addr=RsvdSec+current_sector_addr/256;
		sector_num=(current_sector_addr%256);
		//Flash_page_read(fat_page_addr);
		block=1;
		Rd_Block(0,fat_page_addr*512);       //Flash_page_read(fat_page_addr);
		for(i=0;i<256;i++)
		{
			FAT[i]=Data_buffer[2*i]+Data_buffer[2*i+1]*256;
		}
		next_sector_addr=(FAT[sector_num])&(0x0000ffff);
		if(next_sector_addr==0xffff)  goto end;
		page_addr=SecPerClus*next_sector_addr+Data_start_Addr-16;		
		
	}	
rr1:page_addr=Data_start_Addr+Root_Directory[27]*256*SecPerClus+Root_Directory[26]*SecPerClus-16;
	while(1)
	{
	    block=1;
		Rd_Block(0,page_addr*512);    //Flash_page_read(page_addr);      
		for(i=0;i<512/32;i++)
		{
			for(j=0;j<32;j++)
			{	
				Root_Directory[j]=Data_buffer[i*32+j];
			}
			if(Root_Directory[0]==(0x30+n/100))
				if(Root_Directory[1]==(0x30+(n%100)/10))
					if(Root_Directory[2]==(0x30+n%10))
						goto rr2;
		}
		for(k=0;k<SecPerClus-1;k++)
		{	
			page_addr=page_addr+512;         //page_addr++;
			block=1;
			Rd_Block(0,page_addr*512);           //Flash_page_read(page_addr);
			for(i=0;i<512/32;i++)
			{
				for(j=0;j<32;j++)
				{	
					Root_Directory[j]=Data_buffer[i*32+j];
				}
				if(Root_Directory[0]==(0x30+n/100))
					if(Root_Directory[1]==(0x30+(n%100)/10))
						if(Root_Directory[2]==(0x30+n%10))
							goto rr2;
			}
		}
		current_sector_addr=(page_addr-Data_start_Addr+16)/SecPerClus;	
		fat_page_addr=RsvdSec+current_sector_addr/256;
		sector_num=(current_sector_addr%256);
		block=1;	
		Rd_Block(0,fat_page_addr*512);                 //Flash_page_read(fat_page_addr);
		for(i=0;i<256;i++)
		{
			FAT[i]=Data_buffer[2*i]+Data_buffer[2*i+1]*256;
		}
		next_sector_addr=(FAT[sector_num])&(0x0000ffff);
		if(next_sector_addr==0xffff)  goto end;
		page_addr=SecPerClus*next_sector_addr+Data_start_Addr-16;			
	}	
rr2:page_addr=Data_start_Addr+Root_Directory[27]*256*SecPerClus+Root_Directory[26]*SecPerClus-16;	
	return page_addr;
end:return 0;
}

*/

void BMP_data_from_flash(unsigned long int data_addr,unsigned long int flash_page_addr)
{
	unsigned int i;
	unsigned int j;
	unsigned char remainder_number;
	unsigned char remainder[3];

	unsigned int page_number;
	unsigned int page_addr;
	unsigned int file_size; 
	unsigned int header_size;
	
	unsigned short int n;
	unsigned int fat_page_addr;
	unsigned int current_sector_addr;
	unsigned int next_sector_addr;
	unsigned int sector_num;	
	unsigned short int FAT[256];
	
	
	
	page_addr=flash_page_addr;
	block=1;
	Rd_Block(page_addr*512);           //Flash_page_read(page_addr);
	View_Rx_buf();

	file_size=Data_buffer[5];
	file_size=(file_size<<8)+Data_buffer[4];	
   	file_size=(file_size<<8)+Data_buffer[3];
   	file_size=(file_size<<8)+Data_buffer[2];
   
	header_size=Data_buffer[13];
   	header_size=(header_size<<8)+Data_buffer[12];
   	header_size=(header_size<<8)+Data_buffer[11];
   	header_size=(header_size<<8)+Data_buffer[10];
	page_number=(file_size/512)+1;
		

	for (i=header_size;i<512-2;i=i+3)
	{
		//Image_data[data_addr]=Data_buffer[i+2];
   		//Image_data[data_addr]=(Image_data[data_addr]<<8)+Data_buffer[i+1];
   		//Image_data[data_addr]=(Image_data[data_addr]<<8)+Data_buffer[i];
   		Image_data[data_addr]=Data_buffer[i+2];
   		Image_data[data_addr]=(Image_data[data_addr]<<8)+Data_buffer[i+1];
   		Image_data[data_addr]=(Image_data[data_addr]<<8)+Data_buffer[i];
		data_addr++;
	}

	remainder_number=(512-header_size)%3; 
	for (i=0;i<remainder_number;i++)
	{
		remainder[i]=Data_buffer[512-remainder_number+i];                       //Data_buffer[510]//
		                                                                        //Data_buffer[511]//把被隔断的象素处理到下一簇
	}
	while(1)
	{	
		for(n=0;n<SecPerClus-1;n++)
		{
			
			page_addr=page_addr+1;       //page_addr++;    
			block=1;
			Rd_Block(page_addr*512);         //Flash_page_read(page_addr);		
			
			switch(remainder_number)
			{
				case 0:
				{
					for (j=0;j<512-2;j=j+3)
					{
						//Image_data[data_addr]=(Data_buffer[j+2]<<16)+(Data_buffer[j+1]<<8)+Data_buffer[j];
						*(Image_data+data_addr)=(Data_buffer[j+2]<<16)+(Data_buffer[j+1]<<8)+Data_buffer[j];
	   					data_addr++;
					}
					remainder_number=2;
					remainder[0]=Data_buffer[510];
					remainder[1]=Data_buffer[511];
				}
				break;
				case 1:
				{
					//Image_data[data_addr]=(Data_buffer[1]<<16)+(Data_buffer[0]<<8)+remainder[0];
	   				*(Image_data+data_addr)=(Data_buffer[1]<<16)+(Data_buffer[0]<<8)+remainder[0];
	   				data_addr++;
					for (j=2;j<512-2;j=j+3)
					{
						*(Image_data+data_addr)=(Data_buffer[j+2]<<16)+(Data_buffer[j+1]<<8)+Data_buffer[j];
						data_addr++;
					}
					remainder_number=0;
				}
				break;
				case 2:
				{				
					*(Image_data+data_addr)=(Data_buffer[0]<<16)+(remainder[1]<<8)+remainder[0];
	 				data_addr++;
					for (j=1;j<512-2;j=j+3)
					{
						*(Image_data+data_addr)=(Data_buffer[j+2]<<16)+(Data_buffer[j+1]<<8)+Data_buffer[j];
						data_addr++;
					}
					remainder_number=1;
					remainder[0]=Data_buffer[511];
				}
				break;
				default:;
			}		
		}
		current_sector_addr=(page_addr-Data_start_Addr+46)/SecPerClus;	//512M(32)/1G(46)YYS
		fat_page_addr=RsvdSec+current_sector_addr/256+249;//512M(231)/1G(249)YYS
		sector_num=(current_sector_addr%256);
		block=1; 
		Rd_Block(fat_page_addr*512);     //Flash_page_read(fat_page_addr);
		/*for(i=0;i<256;i++)
		{
			FAT[i]=Data_buffer[2*i]+Data_buffer[2*i+1]*256;
		}
		next_sector_addr=(FAT[sector_num])&(0x0000ffff);*/
		next_sector_addr=(Data_buffer[2*sector_num]+256*Data_buffer[2*sector_num+1])&(0x0000ffff);
		if(next_sector_addr==0xffff)  goto end;
		page_addr=SecPerClus*next_sector_addr+Data_start_Addr-46;//512M(32)/1G(46)YYS	
		block=1; 
		Rd_Block(page_addr*512);     //Flash_page_read(page_addr);				
		switch(remainder_number)
		{
			case 0:
			{
				for (j=0;j<512-2;j=j+3)
				{
					*(Image_data+data_addr)=(Data_buffer[j+2]<<16)+(Data_buffer[j+1]<<8)+Data_buffer[j];
   					data_addr++;
				}
				remainder_number=2;
				remainder[0]=Data_buffer[510];
				remainder[1]=Data_buffer[511];
			}
			break;
			case 1:
			{
				*(Image_data+data_addr)=(Data_buffer[1]<<16)+(Data_buffer[0]<<8)+remainder[0];
   				data_addr++;
				for (j=2;j<512-2;j=j+3)
				{
					*(Image_data+data_addr)=(Data_buffer[j+2]<<16)+(Data_buffer[j+1]<<8)+Data_buffer[j];
					data_addr++;
				}
				remainder_number=0;
			}
			break;
			case 2:
			{
				
				*(Image_data+data_addr)=(Data_buffer[0]<<16)+(remainder[1]<<8)+remainder[0];
 				data_addr++;
				for (j=1;j<512-2;j=j+3)
				{
					*(Image_data+data_addr)=(Data_buffer[j+2]<<16)+(Data_buffer[j+1]<<8)+Data_buffer[j];
					data_addr++;
				}
				remainder_number=1;
				remainder[0]=Data_buffer[511];
			}
			break;
			default:;
		}
		
	}
	end:;      //Image_data[0X33176FFF]//800*480
}





