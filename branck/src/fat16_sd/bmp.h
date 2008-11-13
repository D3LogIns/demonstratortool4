#ifndef __BMP_H__
#define __BMP_H__

#ifdef __cplusplus
extern "C" {
#endif



 typedef __packed struct __BmpHead
  {
  	U16 BmpFlag;          //offset 0,length 2,两字节的内容用来识别位图的类型
  	U32 FileSize;         //offset 2,length 4,整个文件大小,低位再前
  	U32 Reserved;         //offset 6,length 4,保留,必须为0.
  	U32 DataOffset;       //offset 10,length 4,文件头到数据区的偏移量
  	U32 HeadSize;         //offset 14,length 4,位图信息头的长度
  	U32 Width;            //offset 18,length 4,位图的宽度,以像素为单位;
  	U32 Height;           //offset 22,length 4,位图的高度,以像素为单位;
  	U16 Planes;           //offset 26,length 2,位图的位面数,总为1;
  	U16 Bpp;              //offset 28,length 2,每个像素的位数
  	U32 Compression;      //offset 30,length 4,是否压缩:0 不压缩;
  	U32 BmpDataSize;      //offset 34,length 4,用字节表示位图数据区大小,必须是4的倍数
  }BmpHead_Struct;	      //总大小38字节,只取需要的部分






#ifdef __cplusplus
}
#endif
#endif /*__BMP_H__*/