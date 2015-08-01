#ifndef _BK242X_H_
#define _BK242X_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "bk242x_map.h"
#include "bk242x_sub.h"

/* ----------------------------------------------------------------------------*/
//如果使用的不是Bk2423芯片(而是Bk2425)，注释掉下一行，如 /* #define RF_STYLE_BK2423	1*/
#define	RF_STYLE_BK2423			  1		
	 
//如果CE脚恒接高电平，注释掉下一行。 	
#define	RF_CE_USED				    1			   		    

#define	RF_CODE_NULL					0
#define	RF_SHANKHAND_FIRST		1
#define	RF_SHANKHAND_SECOND		2
#define	RF_SHANKHAND_THRID		3
#define	RF_CODE_NORMAL				4
#define	RF_CODE_DATA_RETURN		5

#define	RF_PUBLIC_ADDR0       1
#define	RF_PUBLIC_ADDR1       1
#define	RF_PUBLIC_ADDR2       1
#define	RF_PUBLIC_ADDR3       1
#define	RF_PUBLIC_ADDR4       0x03

#define	RF_PUBLIC_ADDR_TX	 	  0x81  	
#define RF_PUBLIC_ADDR_RX     0x01
 
/* ----------------------------------------------------------------------------*/ 
extern RFDevice *Bk242xTx, *Bk242xRx;
 
/* ----------------------------------------------------------------------------*/
void RFDeviceOpen(void);
void RFTxBind(RFDevice *Dev);
void RFRxBind(RFDevice *Dev);
void RFTick(void);

void RFTxCtrl(RFDevice *Dev);
void RFRxCtrl(RFDevice *Dev);
 
#endif /*#define _BK242X_H_*/




