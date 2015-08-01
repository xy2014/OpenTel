
/***************************************************************************************************
						Bk2423 2G4无线收发芯片
***************************************************************************************************/   
#include "bk242x.h"	 
#include "eeprom.h"
	
/* RF模块寄存器配置 ----------------------------------------------------------*/		
//In the array Bank1_Reg0_13,all[] the register value is the byte reversed!!!!!!!!!!!!!!!!!!!!!
const uint32_t Bank1_Reg0_13[]={  
#ifndef RF_STYLE_BK2423   //Bk2425  
	0xE2014B40,						//REG0
	0x00004BC0,						//REG1
	0x028CFCD0,						//REG2
	0x21390099,						//REG3,120517	
	0x1B8296F9,						/*REG4,120517	 		      1Mbps/2Mbps	       250Kbps
										      Normal Mode		        0x1B8296D9	       0x1B8AB6D9
										      High Power Mode		    0x1B8296F9	       0x1B8AB6F9
										      CW Normal Mode		    0x218296D9	       0x218AB6D9
										      CW High Power Mode	  0x218296F9	       0x218AB6F9	*/	 	
	0xA60F0624,						//REG5,120517,to enable RSSI measurement,use 0xA67F023C(maxium vthcd)
									      //0xA60F0624(20140813)
	0x00000000,						//6
	0x00000000,						//7
	0x00000000,						//8
	0x00000000,						//9
	0x00000000,						//10
	0x00000000,						//11
	0x00127300, 					/*REG12,120517
										      0x00127300:PLL locking time 120us compatible with BK2421;
										      0x00127305(chip default):PLL locking time 130us compatible with nRF24L01;	*/
	0x36B48000,						//REG13,120517	 	 0x46B48000
									      //	0x36B48000(20140813)	
#else  					        //Bk2423				
	0xE2014B40,						//REG0
	0x00004BC0,						//REG1
	0x028CFCD0,						//REG2
	0x21390099,						//REG3,120517	
	0x1B8296F9,						/*REG4,120517	 		1Mbps/2Mbps	        250Kbps
										      Normal Mode		    0x1B8296D9	       0x1B8AB6D9
										      High Power Mode		0x1B8296F9	       0x1B8AB6F9
										      CW Normal Mode		0x218296D9	       0x218AB6D9
										      CW High Power Mode	0x218296F9	       0x218AB6F9	*/	 	
	0xA67F0624,						//REG5,120517,to enable RSSI measurement,use 0xA67F023C(maxium vthcd)
	0x00000000,						//6
	0x00000000,						//7
	0x00000000,						//8
	0x00000000,						//9
	0x00000000,						//10
	0x00000000,						//11
	0x00127300, 					/*REG12,120517
										      0x00127300:PLL locking time 120us compatible with BK2421;
										      0x00127305(chip default):PLL locking time 130us compatible with nRF24L01;	*/		
	0x46B48000,						//REG13,120517
#endif	
};

const uint8_t Bank1_Reg14[] =
{
#ifndef RF_STYLE_BK2423	//Bk2425
	0x41,0x20,0x08,0x04,0x81,0x20,0xcf,0xf7,0xfe,0xff,0xff		//0x41 20 08 04 81 20 CF F7 FE FF FF(20140815)
#else						        //Bk2423  		  
	0x41,0x10,0x04,0x82,0x20,0x08,0x08,0xF2,0x7D,0xEF,0xFF		//0x41,0x10,0x04,0x82,0x20,0x08,0x08,0xF2,0x7D,0xEF,0xFF
#endif
};

//Bank0 register initialization value
const uint8_t Bank0_Reg[10][2]={
	{RX_PW_P0,		RF_FRAME_LENGTH}, 			//0		数据长度6~32字节	
	{RF_STATUS,		0x70}, 			              //1		清所有状态标志
	{RF_SETUP,		0x07}, 			              //2		1Mbps,5dBm，High gain
	{SETUP_RETR,	0x35}, 			              //3		间隔1000us 重发5次
	{SETUP_AW,		0x03}, 			              //4		地址宽度5字节
	{EN_RXADDR,		0x01}, 			              //5		只使能pip0 接收
	{EN_AA,			  0x01},  		              //6		Enb Auto ACK
	{CONFIG,		  0x0d}, 			              //7		Enable中断,2 Byte CRC、Rx、Power Off

	{FEATURE,		  0x04}, 			              //8		no dpl,no ack_pay,dis W_TX_PAYLOAD_NOACK command
	{DYNPD,			  0x01}, 			              //9
};		    


/* 用户自定义 ----------------------------------------------------------------*/
//RF channel frequence
const uint8_t RFFrequencyTable[RF_CHANNEL_SUM] = {	
				48,72,64,56,
        48,72,64,56,	   
	      48,72,64,56,
	      48,72,64,56,	};				  						  
							

#define BK242X_DEV_SUM 2

RFDevice Bk242xDev[BK242X_DEV_SUM];
RFDevice *Bk242xTx, *Bk242xRx;
  

/*=======================================================================================================
=======================================================================================================*/
void RFInitialization(RFDevice *Dev);
void RFTxCtrl(RFDevice *Dev);
void RFRxCtrl(RFDevice *Dev);


/** @Brief 射频设备初始化
  *   1) 上电后，先调用SPI初始化函数，然后再调用此函数。
  *   2) 此处完成以下几项内容:
	*			a、给收发设备指针赋值
	*			b、定义设备的CE及CS管脚(已在SPI中初始化)
	*			c、获取本机地址，作为设备Bk242xTx地址；获取远端地址，作为Bk242xRx地址。
	*			d、初始化射频芯片
	*
	* @Note
	*   1) 有两个Bk242x 2.4G芯片。这两个芯片共用SPI接口，但CS及CE脚各自独立。
	*   2) 一个2.4G芯片主发射，命名Bk242xTx；另一个接收，为Bk242xRx。
	*   3) Bk242xTx的地址是芯片Unique ID，Bk242xRx地址存储在EEPROM中。
	*
  */
void RFDeviceOpen(void) {
	uint8_t i;
	
	Bk242xTx = &Bk242xDev[0]; Bk242xRx = &Bk242xDev[1];	
	
	Bk242xTx->CE_HIGH = Bk242xTx_CE_HIGH;  
	Bk242xTx->CE_LOW  = Bk242xTx_CE_LOW;
  Bk242xTx->CS_HIGH = Bk242xTx_CS_HIGH;
  Bk242xTx->CS_LOW  = Bk242xTx_CS_LOW;	
	Bk242xRx->CE_HIGH = Bk242xRx_CE_HIGH;  
	Bk242xRx->CE_LOW  = Bk242xRx_CE_LOW;
  Bk242xRx->CS_HIGH = Bk242xRx_CS_HIGH;
  Bk242xRx->CS_LOW  = Bk242xRx_CS_LOW;			
	
	Bk242xTx->flag.bPackageSent	= 0;
	Bk242xTx->flag.bPackageReceive = 0;
	Bk242xTx->packageT.length = 0;
	Bk242xTx->packageT.status = ineffect;	
	Bk242xRx->flag.bPackageSent	= 0;
	Bk242xRx->flag.bPackageReceive = 0;
	Bk242xRx->packageT.length = 0;
	Bk242xRx->packageT.status = ineffect;
	
	RFGetUinqueAddr(Bk242xTx->Addr); Bk242xTx->Addr[4] = RF_PUBLIC_ADDR_TX;
	for(i=0; i<5; i++){
		Bk242xRx->Addr[i] = ConfigGlobal.rfAddr[i];
	}
	
	RFInitialization(Bk242xTx);
	RFInitialization(Bk242xRx);
}

/** @Brief 对码
  *   1) 用在发射端(通常指遥控器)
  *   2) 与之对应的接收端对码程序为 RFRxBind()
  *
  * @Note 
  * @ RF_PUBLIC_ADDR_TX, RF_PUBLIC_ADDR_RX，有两个作用:
  *   1)、区分遥控端和接收端。
  *   2)、区分不同遥控器，不同接收端。
  *   取值范围: RF_PUBLIC_ADDR_TX = 10000001 ~ 11111111; RF_PUBLIC_ADDR_RX = 00000001 ~ 01111111。
  *
  */ 
void RFTxBind(RFDevice *Dev)
{	
	RFFlags* rfFlag = &Dev->flag;
  RFVariable* rfCtrl = &Dev->variable;	
	uint8_t* rxBuf = Dev->packageR.buf;
	uint8_t buffer[8];
		
	rfFlag->bBinding = 1;
	rfFlag->bDataReturn  = 0;

	//-------------------------------------------------------------------------------
	//Step1	第一次握手
	//	@ 持续发射本机地址
	//  @ 发射成功后进入第二次握手
	//-------------------------------------------------------------------------------	
RFTxBindingRoll:		
	while(1) {
		RFFlushTx(Dev);
		RFStandby(Dev);
		RFFlushRx(Dev);
		RFClearFlags(Dev);
		 				
 		//-------------------------------------------------------------------
	  //Step1	第一次握手
	  //	@ 持续发射本机地址
	  //  @ 发射成功后进入第二次握手
 		//-------------------------------------------------------------------		
	  rfFlag->bPackageSent	= 0;
	  rfFlag->bPackageReceive = 0;
	  rfCtrl->txPeriod  = 0;
		rfCtrl->frequencyHopPeriod = 0;
		rfCtrl->bindingTimer = 0;
		 	
	  buffer[0] = RF_PUBLIC_ADDR0; 
		buffer[1] = RF_PUBLIC_ADDR1;
		buffer[2] = RF_PUBLIC_ADDR2;
		buffer[3] = RF_PUBLIC_ADDR3;
		buffer[4] = RF_PUBLIC_ADDR4;  								   
		RFWriteBuf(Dev, TX_ADDR,    buffer, 5);	
		RFWriteBuf(Dev, RX_ADDR_P0, buffer, 5);
		
		Dev->packageT.length = 0;
		Dev->packageT.status = ineffect;
		buffer[0] = RF_SHANKHAND_FIRST;
		buffer[1] = Dev->Addr[0];
	  buffer[2] = Dev->Addr[1];
		buffer[3] = Dev->Addr[2];
	  buffer[4] = Dev->Addr[3];
		buffer[5] = RF_PUBLIC_ADDR_TX;			
		RFSendPackage(Dev, buffer, 6, 0);
 		 	
		while(!rfFlag->bPackageSent){
			RFBindCallFunction();				
			RFTxCtrl(Dev);
		}
		rfFlag->bPackageSent = 0;
		RFFlushTx(Dev);
		RFStandby(Dev);
		RFClearFlags(Dev);
				
 		//-------------------------------------------------------------------
 		//Step2  第二次握手
		//  @Brief 获取接收端地址，保存在buffer[]中 
		//  @ 将本机地址写入RF模块，然后切换为接收模式
		//  @ 持续接收100ms
 		//------------------------------------------------------------------- 		
	  buffer[0] = Dev->Addr[0]; 
		buffer[1] = Dev->Addr[1];
		buffer[2] = Dev->Addr[2];
		buffer[3] = Dev->Addr[3];
		buffer[4] = RF_PUBLIC_ADDR_TX;  								   
		RFWriteBuf(Dev, TX_ADDR,    buffer, 5);	
		RFWriteBuf(Dev, RX_ADDR_P0, buffer, 5);
 								
	  rfFlag->bPackageReceive = 0;
	  rfCtrl->rxPeriod = 0;
		rfCtrl->bindingTimer = 0; 		
	  while(rfCtrl->bindingTimer <= 100) {			
		  rfCtrl->channelEffect = rfCtrl->channel;
		  rfCtrl->frequencyHopTimer = 0;
		  rfFlag->bFrequencyAlternative = 0;					
			RFRxCtrl(Dev);
			
			if(rfFlag->bPackageReceive){
				rfFlag->bPackageReceive = 0;
				
				if(rxBuf[2] == RF_SHANKHAND_SECOND){
					buffer[0] = rxBuf[3];
					buffer[1] = rxBuf[4];
					buffer[2] = rxBuf[5];
					buffer[3] = rxBuf[6];
					buffer[4] = rxBuf[7];					
				}
			}
		}		
		RFStandby(Dev);
		RFFlushRx(Dev);
		RFClearFlags(Dev);
		if(!rfFlag->bPackageReceive) goto RFTxBindingRoll;
		rfFlag->bPackageReceive = 0;
 		 		
 		//-------------------------------------------------------------------
 		//Step3  第三次握手
		//  @Brief 对码确认
		//  @ 将接收端地址写入RF模块，然后切换为发射模式
		//  @ 持续发射100ms
 		//-------------------------------------------------------------------
		RFWriteBuf(Dev, TX_ADDR,    buffer, 5);	
		RFWriteBuf(Dev, RX_ADDR_P0, buffer, 5);	

		buffer[0] = RF_SHANKHAND_THRID;
		RFSendPackage(Dev, buffer, 6, 1);
	  rfFlag->bPackageSent = 0;
	  rfCtrl->txPeriod  = 0;
		rfCtrl->bindingTimer = 0;		
		
		while(rfCtrl->bindingTimer <= 100) {
		  rfCtrl->frequencyHopTimer = 0;
			rfCtrl->frequencyOccupyTimer = 0;
			RFTxCtrl(Dev);
		}
		if(!rfFlag->bPackageSent) goto RFTxBindingRoll;		
		rfFlag->bPackageSent = 0;
		RFFlushTx(Dev);
		RFStandby(Dev);
		RFClearFlags(Dev);
 		
 		//-------------------------------------------------------------------
 		//对码完成
		//  @Brief 本机地址写入RF模块，退出对码程序
 		//-------------------------------------------------------------------		 		 		
	  buffer[0] = Dev->Addr[0]; 
		buffer[1] = Dev->Addr[1];
		buffer[2] = Dev->Addr[2];
		buffer[3] = Dev->Addr[3];
		buffer[4] = RF_PUBLIC_ADDR_TX;								   
		RFWriteBuf(Dev, TX_ADDR,    buffer, 5);	
		RFWriteBuf(Dev, RX_ADDR_P0, buffer, 5);		

		Dev->packageT.length = 0;
		Dev->packageT.status = ineffect;
    rfFlag->bBinding = 0; 	
	}		
} 

/** @Brief 对码
  *   1) 用在接收端(通常指接收机)
  *   2) 与之对应的发射端对码程序为 RFTxBind()
  *
  * @Note 
  * @ RF_PUBLIC_ADDR_TX, RF_PUBLIC_ADDR_RX，有两个作用:
  *   1)、区分遥控端和接收端。
  *   2)、区分不同遥控器，不同接收端。
  *   取值范围: RF_PUBLIC_ADDR_TX = 10000001 ~ 11111111; RF_PUBLIC_ADDR_RX = 00000001 ~ 01111111。
  *
  */ 
void RFRxBind(RFDevice *Dev){
	RFFlags* rfFlag = &Dev->flag;
  RFVariable* rfCtrl = &Dev->variable;	
	uint8_t *rxBuf = Dev->packageR.buf;
	uint8_t buffer[8], i;
		
	rfFlag->bBinding = 1;
	rfFlag->bDataReturn  = 0;
	
	//-------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------	
RFRxBindingRoll:		
	while(1) {
		RFFlushTx(Dev);
		RFStandby(Dev);
		RFFlushRx(Dev);
		RFClearFlags(Dev);
		 				
 		//-------------------------------------------------------------------
	  //Step1	第一次握手
	  //	@ 持续接收
	  //  @ 接收成功后进入第二次握手
 		//-------------------------------------------------------------------		 	
	  buffer[0] = RF_PUBLIC_ADDR0; 
		buffer[1] = RF_PUBLIC_ADDR1;
		buffer[2] = RF_PUBLIC_ADDR2;
		buffer[3] = RF_PUBLIC_ADDR3;
		buffer[4] = RF_PUBLIC_ADDR4;  								   
		RFWriteBuf(Dev, TX_ADDR,    buffer, 5);	
		RFWriteBuf(Dev, RX_ADDR_P0, buffer, 5);
 		 	
	  rfFlag->bPackageSent = 0;
	  rfFlag->bPackageReceive	= 0;
	  rfCtrl->rxPeriod  = 0;
		rfCtrl->frequencyHopTimer = 100;         
		while(!rfFlag->bPackageReceive){
			RFBindCallFunction();						
			RFRxCtrl(Dev);
		}
		rfFlag->bPackageReceive = 0;
		RFStandby(Dev);
		RFFlushRx(Dev);
		RFClearFlags(Dev);
	
		if(rxBuf[2] != RF_SHANKHAND_FIRST) goto RFRxBindingRoll;
		
 		//-------------------------------------------------------------------
	  //Step2	第二次握手
 		//-------------------------------------------------------------------	
	  buffer[0] = rxBuf[3]; buffer[1] = rxBuf[4]; buffer[2] = rxBuf[5]; buffer[3] = rxBuf[6]; buffer[4] = rxBuf[7];		 		   
		RFWriteBuf(Dev, TX_ADDR,    buffer, 5);	
		RFWriteBuf(Dev, RX_ADDR_P0, buffer, 5);
		for(i=0; i<5; i++) Dev->Addr[i] = buffer[i];
		
		Dev->packageT.length = 0;
		Dev->packageT.status = ineffect;
	  buffer[0] = RF_SHANKHAND_SECOND;		
		RFGetUinqueAddr(&buffer[1]);
		buffer[5] = RF_PUBLIC_ADDR_RX;		
		RFSendPackage(Dev, buffer, 6, 1);
				
	  rfFlag->bPackageSent	= 0;
	  rfCtrl->txPeriod  = 0;
		rfCtrl->bindingTimer = 0;
		while(rfCtrl->bindingTimer <= 100){
		  rfCtrl->frequencyHopTimer = 0;
		  rfCtrl->frequencyOccupyTimer = 0;
			RFTxCtrl(Dev);
		}
		RFFlushTx(Dev);
		RFStandby(Dev);
		RFClearFlags(Dev);		
		if(!rfFlag->bPackageSent) goto RFRxBindingRoll;
		rfFlag->bPackageSent = 0;
		Dev->packageT.length = 0;
		Dev->packageT.status = ineffect;
		
 		//-------------------------------------------------------------------
		//Step3 第三次握手
 		//-------------------------------------------------------------------
		RFGetUinqueAddr(buffer); buffer[4] = RF_PUBLIC_ADDR_RX;		   
		RFWriteBuf(Dev, TX_ADDR,    buffer, 5);	
		RFWriteBuf(Dev, RX_ADDR_P0, buffer, 5);
		
	  rfFlag->bPackageReceive = 0;
	  rfCtrl->rxPeriod  = 0;
		rfCtrl->bindingTimer = 0;
		while(rfCtrl->bindingTimer <= 100){
		  rfCtrl->channelEffect = rfCtrl->channel;
		  rfCtrl->frequencyHopTimer = 0;
		  rfFlag->bFrequencyAlternative = 0;
			RFRxCtrl(Dev);
		}
		RFStandby(Dev);
		RFFlushRx(Dev);
		RFClearFlags(Dev);		
		if(!rfFlag->bPackageReceive) goto RFRxBindingRoll;		
		rfFlag->bPackageReceive = 0;
		
 		//-------------------------------------------------------------------
 		//对码完成
 		//-------------------------------------------------------------------	  
		for(i=0; i<5; i++) buffer[i] = Dev->Addr[i];	
		RFWriteBuf(Dev, TX_ADDR,    buffer, 5);	
		RFWriteBuf(Dev, RX_ADDR_P0, buffer, 5);
				
	  for(i=0; i<5; i++){
		  ConfigGlobal.rfAddr[i] = Dev->Addr[i];
	  }
		EEPROM_Write();		
	}
}

   
/*=================================================================================================
=================================================================================================*/	   
/** @Brief 初始化Bk242x 2.4G芯片
  *
  */ 
void RFInitialization(RFDevice *Dev)
{				
 	uint8_t WriteArr[4];
	uint8_t i,j; 

	//1) 上电延时
	//2) 如果使用CE脚，CE恒为高电平
	RFDelay(50000); 
	#ifdef RF_CE_USED
	Dev->CE_HIGH();
	#endif			

	//1) Bank0
	//2) 定义数据长度工作模式等，详见数据定义 
	RFBankSelect(Dev, BANK0);	 
	for(i=0; i<8; i++)
		RFWriteReg(Dev, Bank0_Reg[i][0], Bank0_Reg[i][1]);

	i = RFReadReg(Dev, FEATURE);	
	if(i==0) 								            // i!=0 showed that chip has been actived.so do not active again.
		RFWriteCmd(Dev, ACTIVATE, 0x73);	// Active
	
	for(i=8; i<10; i++)
		RFWriteReg(Dev, Bank0_Reg[i][0], Bank0_Reg[i][1]);

	//1) Bank1
	//2) 按照芯片要求，固定值	
	RFBankSelect(Dev, BANK1);
	for(i=0; i<=8; i++){					      //reverse
		for(j=0; j<4; j++) WriteArr[j] = (Bank1_Reg0_13[i]>>(8*(j))) & 0xff;	 		
		RFWriteBuf(Dev, i, &(WriteArr[0]), 4);
	}
	for(i=9; i<=13; i++){
		for(j=0;j<4;j++) WriteArr[j] = (Bank1_Reg0_13[i]>>(8*(3-j))) & 0xff;
		RFWriteBuf(Dev, i, &(WriteArr[0]), 4);
	}
	RFWriteBuf(Dev, 14, (uint8_t *)&(Bank1_Reg14[0]), 11);

	//toggle REG4<25,26>
	for(j=0; j<4; j++) WriteArr[j]=( Bank1_Reg0_13[4]>>(8*(j)) ) & 0xff;

	WriteArr[0] = WriteArr[0]|0x06;
	RFWriteBuf(Dev, 4, &(WriteArr[0]), 4);	
	WriteArr[0] = WriteArr[0] & 0xf9;
	RFWriteBuf(Dev, 4, &(WriteArr[0]), 4);

	//1) 切换回Bank0
	//2) 写入地址，待机，返回	
	RFBankSelect(Dev, BANK0);
	RFDelay(10000);   	
								   
	RFWriteBuf(Dev, TX_ADDR,    Dev->Addr, 5);	
	RFWriteBuf(Dev, RX_ADDR_P0, Dev->Addr, 5);		
} 

/** @brief 查表返回频率
  */
uint8_t RFGetRandomFreq(RFDevice *Dev) {	 	    //更新指针
  if(++Dev->variable.channelIndex >= RF_CHANNEL_SUM) Dev->variable.channelIndex = 0; 
  return RFFrequencyTable[Dev->variable.channelIndex];
} 
uint8_t RFGetNextRandomFreq(RFDevice *Dev) {	 	//不更新指针
  uint8_t i;	
	i = Dev->variable.channelIndex;
	if(++i > RF_CHANNEL_SUM) i = 0;
  return RFFrequencyTable[i];
} 
uint8_t RFGetNNextRandomFreq(RFDevice *Dev) {	 	//不更新指针 
  uint8_t i;	
	i = Dev->variable.channelIndex;
	if(++i > RF_CHANNEL_SUM) i = 0;
	if(++i > RF_CHANNEL_SUM) i = 0;
  return RFFrequencyTable[i];
} 

/** @brief 数据发送
  * @   
  */
void RFTxCtrl(RFDevice *Dev) {	 
  RFFlags* rfFlag = &Dev->flag;
  RFVariable* rfCtrl = &Dev->variable;	
	uint8_t* FIFO = Dev->TxBuffer;
	uint8_t txReady = 0;
	 
  if(rfCtrl->txTimer >= rfCtrl->txPeriod) {
    rfCtrl->txTimer = 0;
    rfCtrl->txPeriod = 5;
		rfCtrl->frequencyHopPeriod = 50;
					 
		//Part1 检测发射状态
		//  1) 发射成功，更新标志位
    if(RFReadReg(Dev, RF_STATUS) & (uint8_t)bTXDS){
      txReady = 1;
      rfCtrl->frequencyHopTimer = 0;			

      if(FIFO[1] & 0x80){ 
				Dev->packageT.status = sent;				
				rfFlag->bPackageSent = 1;
			}					
    }
    RFFlushTx(Dev);		 	 
    RFStandby(Dev);
    RFClearFlags(Dev);		
			 
		//Part2 跳频
    //  1) 跳频
	  //  2) 若要禁止跳频(对码时)，可使:
		//    arfCtrl->frequencyHopTimer = 0;
		//    bfCtrl->frequencyOccupyTimer = 0;
    if((rfCtrl->frequencyHopTimer >= rfCtrl->frequencyHopPeriod) || \
       (rfCtrl->frequencyOccupyTimer >= 360)) {		
      rfCtrl->frequencyHopTimer = 0;						 
      rfCtrl->frequencyOccupyTimer = 0;	 

      rfCtrl->channel = RFGetRandomFreq(Dev);
      RFWriteReg(Dev, RF_CH, rfCtrl->channel);
    }	

		//Part3 重启发射或进入待机状态
		//  1) 
    //  2) 
    if(txReady || ((Dev->packageT.status != tranling) && (Dev->packageReserve.status == newPackage))){  
		  if(RFPackageSplit(Dev)){						
        RFFlushTx(Dev);
        RFWritePayload(Dev, WR_TX_PLOAD, Dev->TxBuffer, RF_FRAME_LENGTH);
        RFTxStart(Dev);
			  Dev->packageT.status = tranling;
			} else {						
        RFFlushTx(Dev);		 	 
        RFStandby(Dev);
        RFClearFlags(Dev);	
						
			  Dev->packageT.status = ineffect;
			}
		} else {
			if(Dev->packageT.status == tranling){					
        RFFlushTx(Dev);
        RFWritePayload(Dev, WR_TX_PLOAD, Dev->TxBuffer, RF_FRAME_LENGTH);
        RFTxStart(Dev);						
			} else {
        RFFlushTx(Dev);		 	 
        RFStandby(Dev);
        RFClearFlags(Dev);	
						
		   	Dev->packageT.status = ineffect;
			}
		}
  }    
}
  		
/** @brief 数据接收
  *
  */
void RFRxCtrl(RFDevice *Dev) {
  RFFlags* rfFlag = &Dev->flag;
  RFVariable* rfCtrl = &Dev->variable;	
	uint8_t rxReady = 0;
  
	if(rfCtrl->rxTimer >= rfCtrl->rxPeriod){
		rfCtrl->rxTimer = 0;
    rfCtrl->rxPeriod = 5;
		
		//Part1 读接收状态
    //  1) 读状态寄存器，判断是否接收完成
		//  2) 若接收完成，置完成标志位、清跳频计时
		//  3) 更新有效频点为当前工作频点
		if(RFReadReg(Dev, RF_STATUS) & (uint8_t)bRXDR){	 	 
      RFStandby(Dev);
      RFReadPayload(Dev, RD_RX_PLOAD, Dev->RxBuffer, RF_FRAME_LENGTH);	 
			if(RFPackageSplice(Dev)) rfFlag->bPackageReceive = 1;
      RFFlushRx(Dev);	
      RFClearFlags(Dev);				
			
      rxReady = 1;
			rfCtrl->frequencyHopTimer = 0;
			rfFlag->bFrequencyAlternative = 0;
			
			if(rfCtrl->channel != RFFrequencyTable[rfCtrl->channelIndex]){
				if(rfCtrl->channel == rfCtrl->channelNext) RFGetRandomFreq(Dev); 
				else if(rfCtrl->channel == rfCtrl->channelNextNext){
					RFGetRandomFreq(Dev);
					RFGetRandomFreq(Dev);
				} 
			} 
			rfCtrl->channelEffect = rfCtrl->channel;	
		}		
		
 		//1) 若没有成功接收，开始跳频
		//2) 前50ms，循环工作于当前频点和下一个频点
		//3) 50~100ms，循环工作于下一个频点和下下个频点
		//4) 大于100ms，逐个扫描频率表
		//5) 若要禁止跳频(对码时)，可使:
		//*    a、rfCtrl->frequencyHopTimer = 0;
		//*    b、rfFlag->bFrequencyAlternative = 0;
		//*    c、rfCtrl->channelEffect = rfCtrl->channel;
		//*
		if(!rxReady){	
      RFStandby(Dev); 
      RFFlushRx(Dev);	
      RFClearFlags(Dev);				
		
			rfCtrl->channelNext = RFGetNextRandomFreq(Dev);		 	 	
			rfCtrl->channelNextNext = RFGetNNextRandomFreq(Dev);  

			if(rfCtrl->frequencyHopTimer < 50){				  				
				rfCtrl->channel = rfCtrl->channelEffect;
				if(rfFlag->bFrequencyAlternative) rfCtrl->channel = rfCtrl->channelNext;	
			} else if(rfCtrl->frequencyHopTimer < 100){ 
				rfCtrl->channel = rfCtrl->channelNext;
				if(rfFlag->bFrequencyAlternative) rfCtrl->channel = rfCtrl->channelNextNext;			
			} else {
				rfCtrl->frequencyHopTimer = 100;
				rfCtrl->channel = RFGetRandomFreq(Dev);
			}	
		
			rfFlag->bFrequencyAlternative = ~rfFlag->bFrequencyAlternative;
			RFWriteReg(Dev, RF_CH, rfCtrl->channel);
		}
		
		//1) 重启接收
 		RFRxStart(Dev);			 
	}
} 		

/** @Brief 中断中自动调用
  * @ 每1ms变量自加1
  */
void RFTick(void){
	RFVariable *timer;
	uint8_t i;

  for(i=0; i<BK242X_DEV_SUM; i++){
		timer = &Bk242xDev[i].variable;
	  timer->txTimer++;
	  timer->rxTimer++;
	  timer->bindingTimer++;
	  timer->dataReturnTimer++;
	  timer->frequencyHopTimer++;
	  timer->frequencyOccupyTimer++;
	}			
}



