#ifndef _BK242X_SUB_H_
#define _BK242X_SUB_H_

#include "spi.h"
#include "bk242x_map.h"
#include "rfPackage.h"					  		 
				
/* 参数配置 ------------------------------------------------------------------*/
#define RF_FRAME_LENGTH        32     //射频帧长度
#define RF_FRAME_DATA_LENGTH   30     //射频帧中有效数据长度FIFO[id][length][Data]...[Data]

#define RF_CHANNEL_SUM         16     //频点数16个(可重复)


/* 数据定义(在缓冲区位置) ----------------------------------------------------*/
#define RF_DATA_STYLE          0
				 
								 
/*----------------------------------------------------------------------------*/
typedef struct {   	
	uint8_t  bPackageSent:1;
	uint8_t  bPackageReceive:1;
  uint8_t  bBinding:1; 
  uint8_t  bDataReturn:1;	 
	
	uint8_t  bFrequencyAlternative:1;	
} RFFlags;	
 
typedef struct {
  uint8_t  txTimer;
  uint8_t  rxTimer;
  uint8_t  bindingTimer;
  uint8_t  dataReturnTimer;
  uint8_t  frequencyHopTimer;
  uint16_t frequencyOccupyTimer;
 
  uint8_t  txPeriod;
  uint8_t  rxPeriod;
  uint8_t  frequencyHopPeriod;
 
  uint8_t  channelIndex;
  uint8_t  channel;
  uint8_t  channelNext;
  uint8_t  channelNextNext;
	uint8_t  channelEffect;
} RFVariable;
 
typedef struct {
	RFFlags flag;
	RFVariable variable;

  uint8_t  Addr[5];
  uint8_t  TxBuffer[RF_FRAME_LENGTH];
	uint8_t  RxBuffer[RF_FRAME_LENGTH];
	
	RFPackage packageT, packageReserve; 
	RFPackage packageR; 
	
	void (*CS_HIGH)(void);
	void (*CS_LOW)(void);
	void (*CE_HIGH)(void);
	void (*CE_LOW)(void);
} RFDevice;	 									
						

/*----------------------------------------------------------------------------*/		
void RFBindCallFunction(void);
void RFGetUinqueAddr(uint8_t *Addr); 
void RFDelay(uint16_t Count);								
		
void RFWriteReg(RFDevice *Dev, uint8_t Addr, uint8_t Data);
void RFWriteBuf(RFDevice *Dev, uint8_t Addr, uint8_t* Buf, uint8_t Length);
void RFWriteCmd(RFDevice *Dev, uint8_t CMD, uint8_t ARG);
uint8_t RFReadReg(RFDevice *Dev, uint8_t Addr);
void RFWritePayload(RFDevice *Dev, uint8_t Style, uint8_t* Buf, uint8_t Length);
void RFReadPayload(RFDevice *Dev, uint8_t Style, uint8_t* Buf, uint8_t Length);

void RFPowerDown(RFDevice *Dev);
void RFStandby(RFDevice *Dev); 
void RFClearFlags(RFDevice *Dev);
void RFFlushTx(RFDevice *Dev);
void RFFlushRx(RFDevice *Dev);
void RFTxStart(RFDevice *Dev);
void RFRxStart(RFDevice *Dev);
void RFBankSelect(RFDevice *Dev, uint8_t bank);

//----------------------------------------------------------------------
void RFSendPackage(RFDevice *Dev, uint8_t* Package, uint16_t Length, uint8_t Repeat);
uint8_t RFPackageSplit(RFDevice *Dev);
uint8_t RFPackageSplice(RFDevice *Dev);


#endif	/*_BK242X_SUB_H_*/


