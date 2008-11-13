#ifndef __FAT16_1_H__
#define __FAT16_1_H__

#ifdef __cplusplus
extern "C" {
#endif

//////////这结构纯粹为了工作需要//////////////////
typedef  struct __CommandTable{
    U8   r;
    U16  value;
    U8   n;
    struct __CommandTable *next;   //指向下一个
    struct __CommandTable *up;	   //指向上一个
    }CmdTable_Struct;


   ////-------------------------------------
typedef  struct __Fat16Dir_Struct{
	U8   Dir_Name[11];
	U8   Dir_Attr;
	U8   Dir_NtRes;
	U8   Dir_CrtTimeTeenth;
	U16  Dir_CrtTime;
	U16  Dir_CrtDate;
	U16  Dir_LastAccDate;
	U16  Dir_FstClusHi;
	U16  Dir_WrtTime;
	U16  Dir_WrtDate;
	U16  Dir_FstClusLo;
	U32  Dir_FileSize;
}Fat16Dir_Struct;							//共32字节

   
   //////////////////////////////////////
 typedef struct __Fat16BPB_Struct
  {
  	U16 BytesPerSec;      //offset 11,length 2,每扇区字节数
  	U8  SecPerClus;       //offset 13,length 1,每簇扇区数
  	U16 RsvdSecCnt;       //offset 14,length 2,保留区中保留扇区数.
  	U8  NumFats;          //offset 16,length 1,fat表的份数(2)
  	U16 RootEntCnt;       //offset 17,length 2,根目录目录项数(512)
  	U16 TotSec16;         //offset 19,length 2,四个区中的总扇区数(<0x10000)
  	U8  Media;            //offset 21,length 1,移动或固定存储介质
  	U16 FatSz16;          //offset 22,length 2,fat16一个fat表所占的扇区数
  	U16 SecPerTrk;        //offset 24,length 2,每个磁道的扇区数
  	U16 NumHeads;         //offset 26,length 2,磁头数
  	U32 HiddSec;          //offset 28,length 4,fat分区前隐藏的扇区数
  	U32 TotSec32;         //offset 32,length 4,总扇区数,同TotSec16. 	
  }Fat16BPB_Struct;							//总长25bytes
   
 //////////////////////////////////////////////////////////
 typedef  struct __Fat16RtDirInfo_Struct
 {
    U32 RtDirStartSec;   
    U32 RtDirTotSec;
    U32 DataStartSec;								
 }Fat16RtDirInfo_Struct;							


 /////////////////////////////////////////////////////////  
   
BOOL Fat16_ReadBPB(void);
BOOL Fat16_RenewRtInfo(void);
//BOOL Fat16_FindDir(U8 *s,U32 *addr);  //s为文件夹名称,n为所在扇区地址
BOOL Fat16_FindDirAtRoot(U8 *s,Fat16Dir_Struct  *Point_RtDir);  //s为文件夹名称,addr为所在扇区地址

BOOL Fat16_FindSonDir(U8 *s,Fat16Dir_Struct  *Point_UpDir,Fat16Dir_Struct  *Point_DownDir) ;

BOOL Fat16_DispBmp(Fat16Dir_Struct  *Point_UpDir) ;
BOOL Fat16_ReadTxt(Fat16Dir_Struct  *Point_UpDir) ; 
#ifdef __cplusplus
}
#endif
#endif /*__FAT16_1_H__*/
