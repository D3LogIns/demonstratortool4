#ifndef __SD_H__
#define __SD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "..\common\def.h"
// Function prototypes
void Test_SDI(void);
void set_system_clock(void);


void Rd_cid_csd(char reg);

int Check_status(char type);
int Chk_DATend(void);
int Chk_BUSYend(void);
int Chk_CMDend(int cmd, int be_resp);
int Chk_MMC_OCR(void);
int Chk_SD_OCR(void);

void Tx_Buf_Init(void);
void Card_sel_desel(char sel_desel);

void CMD0(void);
int CMD55(void);

void Rd_CSD(void);
void Rd_CID(void);


BOOL SD_card_init(void);
void SD_ReadBlock(U32 block_start,U32 block_num,U32 *read_buf);
void SD_WriteBlock(int block_start,int block_num,int *write_buf);


void Rd_Stream(void);
void Wt_Stream(void);

void Set_Prt(void);
void Clr_Prt(void);

void Flush_Rx_buf(void);
void SetBus(void);
void Set_1bit_bus(void);
void Set_4bit_bus(void);
void View_Rx_buf(int block_num);
void View_Tx_buf(void);


#ifdef __cplusplus
}
#endif
#endif /*__SD_H___*/
