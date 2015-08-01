
/***************************************************************************************************
						Bk2423 各子函数
***************************************************************************************************/
#include "bk242x_sub.h"	 

/*================================================================================================================							
================================================================================================================*/
/**  外部函数
  *  @ RF对码期间，函数堵塞。直到对码完成才退出。
  *  @ 若有外部函数需要调用，如指示灯。可将外部函数放入此处。
  *  @ 另，RF计时变量默认在中断中递增，若中断不满足要求，需要自定义计时函数，那么这个函数也应放入此处。
  */
void RFBindCallFunction(void){
	//LedCtrl(make_pair);	
}
	  
/**
  *获取产品唯一序列号，赋值滚码
  */
void RFGetUinqueAddr(uint8_t *Addr)
{	   	
  Addr[0] = 1;
  Addr[1] = 2;
  Addr[2] = 3;
  Addr[3] = 4;
}
 
//================================================================================================================	
//						子函数
//================================================================================================================ 
//--------------------------------------------------------------
//				IRC 22M,不确定长度延时										  
//--------------------------------------------------------------
void RFDelay(uint16_t Count)
{
  /* Decrement nCount value */
  while (Count != 0)
  {
    Count--;
  }	
}
		
/* RF读写函数 ----------------------------------------------------------------*/
/** @Brief RF读写函数
  *
  */
//写寄存器
void RFWriteReg(RFDevice *Dev, uint8_t Addr, uint8_t Data) {
	SPI_Write_Reg_2(BK_WRITE_REG|Addr, Data, Dev->CS_HIGH, Dev->CS_LOW); 
}

void RFWriteBuf(RFDevice *Dev, uint8_t Addr, uint8_t* Buf, uint8_t Length) {
  SPI_Write_Buf_2(BK_WRITE_REG|Addr, Buf, Length, Dev->CS_HIGH, Dev->CS_LOW);
}

//写命令
void RFWriteCmd(RFDevice *Dev, uint8_t CMD, uint8_t ARG) {
	SPI_Write_Reg_2(CMD, ARG, Dev->CS_HIGH, Dev->CS_LOW);
}

//读寄存器
uint8_t RFReadReg(RFDevice *Dev, uint8_t Addr) {
  return SPI_Read_Reg_2(BK_READ_REG|Addr, Dev->CS_HIGH, Dev->CS_LOW);
}

//收发射缓冲区
void RFWritePayload(RFDevice *Dev, uint8_t Style, uint8_t* Buf, uint8_t Length){
	SPI_Write_Buf_2(Style, Buf, Length, Dev->CS_HIGH, Dev->CS_LOW);
}
void RFReadPayload(RFDevice *Dev, uint8_t Style, uint8_t* Buf, uint8_t Length){
	SPI_Read_Buf_2(Style, Buf, Length, Dev->CS_HIGH, Dev->CS_LOW);
}


/* RF功能函数 ----------------------------------------------------------------*/
/** @brief RF功能函数
  *
  */  
//休眠
void RFPowerDown(RFDevice *Dev)
{	
	RFDelay(200);
	RFWriteReg(Dev, CONFIG, 0x7c); 
}	

//待机
void RFStandby(RFDevice *Dev) {	  
	RFWriteReg(Dev, CONFIG, 0x0d);
	RFDelay(200);
}
											
//清标志位	   
void RFClearFlags(RFDevice *Dev)
{							  
	RFWriteReg(Dev, RF_STATUS, 0x70);
}
 
//清发射缓冲区
void RFFlushTx(RFDevice *Dev)
{	
	RFWriteCmd(Dev, FLUSH_TX, BK_NOP);
}

//清接收缓冲区
void RFFlushRx(RFDevice *Dev)
{	
	RFWriteCmd(Dev, FLUSH_RX, BK_NOP);
}

//启动发射
void RFTxStart(RFDevice *Dev)
{
	RFWriteReg(Dev, CONFIG, CONFIG_INIT&0xfe);	
}

//启动接收
void RFRxStart(RFDevice *Dev)
{
	RFWriteReg(Dev, CONFIG, CONFIG_INIT|0x01);	
}

//--------------------------------------------------------------
//				bank切换
//		读0x07.7,判断当前bank与目标值是否相同
//		若相同,退出;不同,切换bank
//--------------------------------------------------------------
void RFBankSelect(RFDevice *Dev, uint8_t bank)
{	
	uint8_t bank_status;

	bank_status = RFReadReg(Dev, 0x07); 
	bank_status &= (uint8_t)0x80;
	
	if(bank_status != bank)
	{
		//当前bank与目标bank不同
		//写入ACTIVATE_CMD+0x53切换 				
		RFWriteCmd(Dev, ACTIVATE, 0x53);
	}
}


