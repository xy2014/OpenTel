/**
  ******************************************************************************
  * @file    stm3210b_eval.c
  * @author  MCD Application Team
  * @version V4.5.0
  * @date    07-March-2011
  * @brief   This file provides
  *            - set of firmware functions to manage Leds, push-button and COM ports
  *            - low level initialization functions for SD card (on SPI), SPI serial
  *              flash (sFLASH) and temperature sensor (LM75)
  *          available on STM3210B-EVAL evaluation board from STMicroelectronics.    
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************  
  */ 
  
/* Includes ------------------------------------------------------------------*/
#include "rfPackage.h"
#include "bk242x.h"
 
#define NULL (void*)0

#define TRUE  1
#define FALSE 0


/**@brief 数据包拆分，赋值发射缓冲区，为写入射频设备做准备
  *@param
	*  1) *Dev，设备指针。待发射数据包及相关都在设备内部
	*  
  *@return 
	*  1)TRUE(数据待发射) 
	*  2)FALSE(无数据)
  */
uint8_t RFPackageSplit(RFDevice *Dev)
{
	RFPackage* package = &Dev->packageT; 
	RFPackage* pacRes = &Dev->packageReserve;
	uint8_t* FIFO = Dev->TxBuffer; 	
	uint8_t *point = NULL;   
	uint16_t index, length;	
	
	//Step1 数据包为空或者发射完成，重新赋值
	if((package->length == 0) || (package->style.sequence*RF_FRAME_DATA_LENGTH >= package->length)){
		if((pacRes->status == newPackage) || pacRes->style.repeat){			
			for(index = 0; index < pacRes->length; index++) package->buf[index] = pacRes->buf[index];
			package->length = pacRes->length;
			package->id = pacRes->id;
			package->style.sequence = 0;		
			
			pacRes->status = copied;
		} 
	}  
	
	//Step2 获取待发射数据位置	
	if((package->length != 0) && (package->style.sequence*RF_FRAME_DATA_LENGTH < package->length)){
		point = &package->buf[package->style.sequence * RF_FRAME_DATA_LENGTH];
	}
	
	//Step3 更新发射缓冲区
	//1) FIFO[0]    包序列号|包拆分后的子序列号，各占四位 
	//2) FIFO[1]    低5位=数据长度(0~30)，最高位=数据包是否结束
	//3) FIFO[2~31] 数据
	if(point != NULL){		
		length = package->length - package->style.sequence*RF_FRAME_DATA_LENGTH;
		if(length > RF_FRAME_DATA_LENGTH) {length = RF_FRAME_DATA_LENGTH; FIFO[1] = 0;}
		else {FIFO[1] = 0x80;}
		
		FIFO[0] = ((uint8_t)package->id << 4) | ((uint8_t)package->style.sequence & 0x0f); 
		FIFO[1] |= length;
		for(index=0; index<length; index++){
			FIFO[2+index] = point[index];
		}
		
		package->style.sequence++;                      
		
		return TRUE;
	}	
	
	return FALSE;
}

/**@brief 数据包拼接
  *  1) 射频设备获取一帧数据后调用此函数，将一帧或多帧数据拼接成一个完整的数据包
  *
  *@param
  *  1) *Dev 射频设备
  *  
  *@return 
  *  1) TRUE 获取一个完成的数据包；FALSE 出错或数据包未完成
  *
  *@note 当遥控设备重复发射同一个数据包时，此函数也会重复返回相同的数据包。
  */
uint8_t RFPackageSplice(RFDevice *Dev) {
  RFPackage* package = &Dev->packageR;
	uint8_t* FIFO = Dev->RxBuffer;
	uint8_t i, length;
	
	//Case1 Sequence==0，开始一个新数据包的接收(可能和之前的数据包相同，即同ID)
	//  1) 只要sequence=0，优先启动新数据包接收，不检查之前状态
	//  2) 单帧射频数据也可能是一个完整的数据包，所以最后要检查FIFO[1].7标志位
	if((FIFO[0] & 0x0f) == 0){                       
		package->id = FIFO[0] & 0xf0;
		package->style.sequence = 0;		
		package->length = FIFO[1] & 0x1f;		
		for(i=0; i<package->length; i++) package->buf[i] = FIFO[i+2];
		
		if(FIFO[1] & 0x80){
			package->status = receive;
			return TRUE;
		} 
		else return FALSE;
	} 
	
	//Case2 多帧组成一个数据包
	//  1) 只有当前射频帧sequence等于之前加1时才更新。
	if(package->id == (FIFO[0] & 0xf0)){                    
		if((package->style.sequence + 1) == (FIFO[0]&0x0f)){  
			package->style.sequence++; 
			length = FIFO[1] & 0x1f;
			for(i=0; i<length; i++) package->buf[package->length+i] = FIFO[2+i];
			package->length += length;			
			
			if(FIFO[1] & 0x80){
			  package->status = receive;
			  return TRUE;
		  }
		}
	} 
	
	return FALSE;
}


/**@brief 将待发射数据写入发射缓冲区
  *  1) 这是对外接口函数。
  *  2) 将待发射数据指针传递给该函数，数据即会被写入发射缓冲区。
  *  3) 射频设备完成当前工作后，将自动完成数据发射。
  *  
  *@param
  *  *Dev    射频设备
  *  *Data   待发射数据指针
  *  Length  待发射数据长度。若超过数据包最大长度，则取最大。
  *  Repeat  =1，重复发射；=0，只发射一次
  */
void RFSendPackage(RFDevice *Dev, uint8_t* Data, uint16_t Length, uint8_t Repeat) {
	RFPackage* pacRes = &Dev->packageReserve;
	uint16_t index; 
	
	if(Length > RF_PACKAGE_LENGTH) Length = RF_PACKAGE_LENGTH;	
	for(index=0; index<Length; index++) pacRes->buf[index] = Data[index];
	pacRes->length = Length;
	pacRes->id++;
	pacRes->style.repeat = Repeat;	
	pacRes->status = newPackage;
}


/**@brief 监测函数
  *  1)实时调用，更新射频设备。
  *  2)实现发射，完成接收
  */






