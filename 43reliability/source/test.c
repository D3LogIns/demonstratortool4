#include "soucehead.h"
#include "LPC2294.h"
extern void Display_hex(unsigned char hex,unsigned int window_start_x,unsigned  int window_start_y,unsigned long int color,char times_x,char times_y);
extern void Display_string(char *string,unsigned int window_start_x,unsigned  int window_start_y,unsigned long int color,char times_x,char times_y);
extern void Screen_clear(void);

extern unsigned char Data_buffer[0x200];
extern void Flash_page_read(unsigned long int flash_page_addr);
char Root_Directory[32];
extern char gif_name[6][8];
extern char movi_name[6][8];
extern unsigned char  file_num(unsigned char c[4]);
extern unsigned char movi_num;
extern unsigned char gif_num;
unsigned long int RootDirAddr;
unsigned long int Data_start_Addr;
unsigned RootDirNum;
unsigned int RsvdSec;
unsigned char SecPerClus;
void Read_BPB(void)
{	
	unsigned start_sec_addr;
	unsigned long int Total_Sec; 	
	unsigned int Fatsz16;
	Uart_Printf("\nsd ok\n");
	Flash_page_read(0);
	SecPerClus=Data_buffer[13];
	
	RsvdSec=Data_buffer[15]*256+Data_buffer[14];
	RootDirNum=Data_buffer[18]*256+Data_buffer[17];
	Total_Sec=(Data_buffer[20]*256+Data_buffer[19])&0x0000ffff;
	Fatsz16=Data_buffer[23]*256+Data_buffer[22];
	if(Total_Sec==0)
	{
		Total_Sec=Data_buffer[35]*(0x1000000)+Data_buffer[34]*(0x10000)+Data_buffer[33]*(0x100)+Data_buffer[32];
	}
	RootDirAddr=Fatsz16*2+RsvdSec;
	Data_start_Addr=RootDirAddr+RootDirNum/16;	
}
unsigned long int  searchfile(unsigned char c[4],unsigned char m[4],unsigned char n)
{
	unsigned long int page_addr;
	unsigned long int file_addr;
	unsigned int i;
	unsigned char j;
	unsigned char k;
	
	unsigned long int fat_page_addr;
	unsigned long int current_sector_addr;
	unsigned long int next_sector_addr;
	unsigned long int sector_num;	
	unsigned int FAT[256];

	page_addr=RootDirAddr;
	for(k=0;k<RootDirNum/16;k++)
	{
		Flash_page_read(page_addr);
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
		page_addr++;
	}
	return 0;
rr:	page_addr=Data_start_Addr+Root_Directory[27]*256*SecPerClus+Root_Directory[26]*SecPerClus-4;
	while(1)
	{
		Flash_page_read(page_addr);
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
			page_addr++;
			Flash_page_read(page_addr);
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
		current_sector_addr=(page_addr-Data_start_Addr+4)/SecPerClus;	
		fat_page_addr=RsvdSec+current_sector_addr/256;
		sector_num=(current_sector_addr%256);
		Flash_page_read(fat_page_addr);
		for(i=0;i<256;i++)
		{
			FAT[i]=Data_buffer[2*i]+Data_buffer[2*i+1]*256;
		}
		next_sector_addr=(FAT[sector_num])&(0x0000ffff);
		if(next_sector_addr==0xffff)  goto end;
		page_addr=SecPerClus*next_sector_addr+Data_start_Addr-4;		
		
	}	
rr1:page_addr=Data_start_Addr+Root_Directory[27]*256*SecPerClus+Root_Directory[26]*SecPerClus-4;
	while(1)
	{
		Flash_page_read(page_addr);
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
			page_addr++;
			Flash_page_read(page_addr);
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
		current_sector_addr=(page_addr-Data_start_Addr+4)/SecPerClus;	
		fat_page_addr=RsvdSec+current_sector_addr/256;
		sector_num=(current_sector_addr%256);
		Flash_page_read(fat_page_addr);
		for(i=0;i<256;i++)
		{
			FAT[i]=Data_buffer[2*i]+Data_buffer[2*i+1]*256;
		}
		next_sector_addr=(FAT[sector_num])&(0x0000ffff);
		if(next_sector_addr==0xffff)  goto end;
		page_addr=SecPerClus*next_sector_addr+Data_start_Addr-4;			
	}	
rr2:page_addr=Data_start_Addr+Root_Directory[27]*256*SecPerClus+Root_Directory[26]*SecPerClus-4;	
	return page_addr;
end:return 0;
}



unsigned long int  searchfile_movi(unsigned char n,unsigned char m)
{
	unsigned long int page_addr;
	unsigned long int file_addr;
	unsigned int i;
	unsigned char j;
	unsigned char k;
		
	unsigned long int fat_page_addr;
	unsigned long int current_sector_addr;
	unsigned long int next_sector_addr;
	unsigned long int sector_num;	
	unsigned int FAT[256];
	
	page_addr=RootDirAddr;
	for(k=0;k<RootDirNum/16;k++)
	{
		Flash_page_read(page_addr);
		for(i=0;i<16;i++)
		{
			for(j=0;j<32;j++)
			{	
				Root_Directory[j]=Data_buffer[i*32+j];
			}
			if(Root_Directory[0]=='M')
				if(Root_Directory[1]=='O')
				   if(Root_Directory[2]=='V')
				   	  if(Root_Directory[3]=='I')
						  goto rr;
		}
		page_addr++;
	}
	return 0;
rr:	page_addr=Data_start_Addr+Root_Directory[27]*256*SecPerClus+Root_Directory[26]*SecPerClus-4;	
	while(1)
	{
		Flash_page_read(page_addr);
		for(i=0;i<16;i++)
		{
			for(j=0;j<32;j++)
			{	
				Root_Directory[j]=Data_buffer[i*32+j];
			}
			if(Root_Directory[0]==movi_name[n][0])
				if(Root_Directory[1]==movi_name[n][1])			
					if(Root_Directory[2]==movi_name[n][2])
						if(Root_Directory[3]==movi_name[n][3])
							if(Root_Directory[4]==movi_name[n][4])
								if(Root_Directory[5]==movi_name[n][5])			
									if(Root_Directory[6]==movi_name[n][6])
										if(Root_Directory[7]==movi_name[n][7])
											goto rr1;		
		}	
			
		for(k=0;k<SecPerClus-1;k++)
		{	
			page_addr++;
			Flash_page_read(page_addr);
			for(i=0;i<512/32;i++)
			{
				for(j=0;j<32;j++)
				{	
					Root_Directory[j]=Data_buffer[i*32+j];
				}
				if(Root_Directory[0]==movi_name[n][0])
					if(Root_Directory[1]==movi_name[n][1])			
						if(Root_Directory[2]==movi_name[n][2])
							if(Root_Directory[3]==movi_name[n][3])
								if(Root_Directory[4]==movi_name[n][4])
									if(Root_Directory[5]==movi_name[n][5])			
										if(Root_Directory[6]==movi_name[n][6])
											if(Root_Directory[7]==movi_name[n][7])
												goto rr1;		
			}
		}
		current_sector_addr=(page_addr-Data_start_Addr+4)/SecPerClus;	
		fat_page_addr=RsvdSec+current_sector_addr/256;
		sector_num=(current_sector_addr%256);
		Flash_page_read(fat_page_addr);
		for(i=0;i<256;i++)
		{
			FAT[i]=Data_buffer[2*i]+Data_buffer[2*i+1]*256;
		}
		next_sector_addr=(FAT[sector_num])&(0x0000ffff);
		if(next_sector_addr==0xffff)  goto end;
		page_addr=SecPerClus*next_sector_addr+Data_start_Addr-4;		
			
	}	
rr1:page_addr=Data_start_Addr+Root_Directory[27]*256*SecPerClus+Root_Directory[26]*SecPerClus-4;//0X9FA;
	if(m==0)
	{
		while(1)
		{	
			Flash_page_read(page_addr);
			for(i=0;i<512/32;i++)
			{
				for(j=0;j<32;j++)
				{	
					Root_Directory[j]=Data_buffer[i*32+j];
				}
				if(Root_Directory[0]=='S')//0x53)
					if(Root_Directory[1]=='H')//0x41)
						if(Root_Directory[2]=='E')//0x49)
							goto rr2;
			}	
			for(n=0;n<SecPerClus-1;n++)
			{	
				page_addr++;
				Flash_page_read(page_addr);
				for(i=0;i<512/32;i++)
				{
					for(j=0;j<32;j++)
					{	
						Root_Directory[j]=Data_buffer[i*32+j];
					}
					if(Root_Directory[0]=='S')//0x53)
						if(Root_Directory[1]=='H')//0x41)
							if(Root_Directory[2]=='E')//0x49)
								goto rr2;
				}
			}
			current_sector_addr=(page_addr-Data_start_Addr+4)/SecPerClus;	
			fat_page_addr=RsvdSec+current_sector_addr/256;
			sector_num=(current_sector_addr%256);
			Flash_page_read(fat_page_addr);
			for(i=0;i<256;i++)
			{
				FAT[i]=Data_buffer[2*i]+Data_buffer[2*i+1]*256;
			}
				next_sector_addr=(FAT[sector_num])&(0x0000ffff);
			if(next_sector_addr==0xffff)  goto end;
			page_addr=SecPerClus*next_sector_addr+Data_start_Addr-4;		
			
		}	
	}
	else
	{
		while(1)
		{
			
			Flash_page_read(page_addr);
			for(i=0;i<512/32;i++)
			{
				for(j=0;j<32;j++)
				{	
					Root_Directory[j]=Data_buffer[i*32+j];
				}
				if(Root_Directory[0]==(0x30+m/100))//0x53)
					if(Root_Directory[1]==(0x30+(m%100)/10))//0x41)
						if(Root_Directory[2]==(0x30+m%10))//0x49)
							goto rr2;
			}	
			for(n=0;n<SecPerClus-1;n++)
			{	
				page_addr++;
				Flash_page_read(page_addr);
				for(i=0;i<512/32;i++)
				{
					for(j=0;j<32;j++)
					{	
						Root_Directory[j]=Data_buffer[i*32+j];
					}
					if(Root_Directory[0]==(0x30+m/100))//0x53)
						if(Root_Directory[1]==(0x30+(m%100)/10))//0x41)
							if(Root_Directory[2]==(0x30+m%10))//0x49)
								goto rr2;
				}
			}
			current_sector_addr=(page_addr-Data_start_Addr+4)/SecPerClus;	
			fat_page_addr=RsvdSec+current_sector_addr/256;
			sector_num=(current_sector_addr%256);
			Flash_page_read(fat_page_addr);
			for(i=0;i<256;i++)
			{
				FAT[i]=Data_buffer[2*i]+Data_buffer[2*i+1]*256;
			}
			next_sector_addr=(FAT[sector_num])&(0x0000ffff);
			if(next_sector_addr==0xffff)  goto end;
			page_addr=SecPerClus*next_sector_addr+Data_start_Addr-4;		
			
		}	
	}
rr2:page_addr=Data_start_Addr+Root_Directory[27]*256*SecPerClus+Root_Directory[26]*SecPerClus-4;	
	return page_addr;
end:return 0;
}


unsigned char  file_num(unsigned char c[4])
{
	unsigned long int page_addr;
	unsigned long int file_addr;
	unsigned int i;
	unsigned char j;	
	unsigned char num;
	unsigned char n;
	
	unsigned long int fat_page_addr;
	unsigned long int current_sector_addr;
	unsigned long int next_sector_addr;
	unsigned long int sector_num;	
	//unsigned int FAT[256];
	num=0;	
	page_addr=RootDirAddr;
	for(n=0;n<RootDirNum/16;n++)
	{
		Flash_page_read(page_addr);
		for(i=0;i<16;i++)
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
		page_addr++;
	}
	return 0;
rr:	page_addr=Data_start_Addr+Root_Directory[27]*256*2+Root_Directory[26]*2-4;
	while(1)
	{		
		Flash_page_read(page_addr);
		for(i=0;i<16;i++)
		{
			for(j=0;j<32;j++)
			{	
				Root_Directory[j]=Data_buffer[i*32+j];
			}
			if((Root_Directory[0]!=0x2e)&(Root_Directory[0]!=0xe5)&(Root_Directory[0]!=0x00))
			{	
				if(c[0]=='G')
				{
					
					gif_name[num][0]=Root_Directory[0];
					gif_name[num][1]=Root_Directory[1];
					gif_name[num][2]=Root_Directory[2];
					gif_name[num][3]=Root_Directory[3];
					gif_name[num][4]=Root_Directory[4];
					gif_name[num][5]=Root_Directory[5];
					gif_name[num][6]=Root_Directory[6];
					gif_name[num][7]=Root_Directory[7];
					num++;n=num;
				}
				if(c[0]=='M')
				{
					movi_name[num][0]=Root_Directory[0];
					movi_name[num][1]=Root_Directory[1];
					movi_name[num][2]=Root_Directory[2];
					movi_name[num][3]=Root_Directory[3];
					movi_name[num][4]=Root_Directory[4];
					movi_name[num][5]=Root_Directory[5];
					movi_name[num][6]=Root_Directory[6];
					movi_name[num][7]=Root_Directory[7];
					num++;
				}
			 }
		}
		for(n=0;n<SecPerClus-1;n++)
		{
			page_addr++;
			Flash_page_read(page_addr);
			for(i=0;i<512/32;i++)
			{
				for(j=0;j<32;j++)
				{	
					Root_Directory[j]=Data_buffer[i*32+j];
				}
				if((Root_Directory[0]!=0x2e)&(Root_Directory[0]!=0xe5)&(Root_Directory[0]!=0x00))
				{	
					
					if(c[0]=='G')
					{
						
						gif_name[num][0]=Root_Directory[0];
						gif_name[num][1]=Root_Directory[1];
						gif_name[num][2]=Root_Directory[2];
						gif_name[num][3]=Root_Directory[3];
						gif_name[num][4]=Root_Directory[4];
						gif_name[num][5]=Root_Directory[5];
						gif_name[num][6]=Root_Directory[6];
						gif_name[num][7]=Root_Directory[7];
						num++;
					}
					
					if(c[0]=='M')
					{
						movi_name[num][0]=Root_Directory[0];
						movi_name[num][1]=Root_Directory[1];
						movi_name[num][2]=Root_Directory[2];
						movi_name[num][3]=Root_Directory[3];
						movi_name[num][4]=Root_Directory[4];
						movi_name[num][5]=Root_Directory[5];
						movi_name[num][6]=Root_Directory[6];
						movi_name[num][7]=Root_Directory[7];
						num++;
					}
				}
			}
		}	
		current_sector_addr=(page_addr-Data_start_Addr+4)/SecPerClus;	
		fat_page_addr=RsvdSec+current_sector_addr/256;
		sector_num=(current_sector_addr%256);
		Flash_page_read(fat_page_addr);
	
		next_sector_addr=(Data_buffer[2*sector_num]+256*Data_buffer[2*sector_num+1])&(0x0000ffff);
		
		
		
		if(next_sector_addr==0xffff)
		 { goto	end;}
		page_addr=SecPerClus*next_sector_addr+Data_start_Addr-4;
	}
end:
	
	return num;		
}
