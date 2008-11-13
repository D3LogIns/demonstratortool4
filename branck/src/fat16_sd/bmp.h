#ifndef __BMP_H__
#define __BMP_H__

#ifdef __cplusplus
extern "C" {
#endif



 typedef __packed struct __BmpHead
  {
  	U16 BmpFlag;          //offset 0,length 2,���ֽڵ���������ʶ��λͼ������
  	U32 FileSize;         //offset 2,length 4,�����ļ���С,��λ��ǰ
  	U32 Reserved;         //offset 6,length 4,����,����Ϊ0.
  	U32 DataOffset;       //offset 10,length 4,�ļ�ͷ����������ƫ����
  	U32 HeadSize;         //offset 14,length 4,λͼ��Ϣͷ�ĳ���
  	U32 Width;            //offset 18,length 4,λͼ�Ŀ��,������Ϊ��λ;
  	U32 Height;           //offset 22,length 4,λͼ�ĸ߶�,������Ϊ��λ;
  	U16 Planes;           //offset 26,length 2,λͼ��λ����,��Ϊ1;
  	U16 Bpp;              //offset 28,length 2,ÿ�����ص�λ��
  	U32 Compression;      //offset 30,length 4,�Ƿ�ѹ��:0 ��ѹ��;
  	U32 BmpDataSize;      //offset 34,length 4,���ֽڱ�ʾλͼ��������С,������4�ı���
  }BmpHead_Struct;	      //�ܴ�С38�ֽ�,ֻȡ��Ҫ�Ĳ���






#ifdef __cplusplus
}
#endif
#endif /*__BMP_H__*/