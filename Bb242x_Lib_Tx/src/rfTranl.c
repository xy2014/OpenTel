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
#include "rfTranl.h"
#include "bk242x.h"
 
#define NULL (void*)0

#define TRUE 1
#define FALSE 0


/**@brief 数据包拆分，然后赋值发射缓冲区，为写入射频设备做准备
  *@param
	*  1) *Dev，设备指针。待发射数据包及相关都在设备内部
	*  
  *@return 
	*  1)TRUE(数据待发射) 
	*  2)FALSE(无数据)
  */
uint8_t RFPackageSplit(RFDevice *Dev)
{
	RFPackage *package, *pacRes;
	uint8_t* FIFO; 
	
	uint8_t *point = NULL;   
	uint16_t index, length;
	
	
	//Step1 数据包为空或者发射完成，重新赋值
	if((package->length == 0) || (package->style.sequence*RF_FRAME_DATA_LENGTH >= package->length)){
		if((package->id != pacRes->id) || pacRes->style.repeat){			
			for(index = 0; index < pacRes->length; index++) package->buf[index] = pacRes->buf[index];
			package->length = pacRes->length;
			package->id = pacRes->id;
			package->style.sequence = 0;			
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

/**@brief 将待发射数据写入发射缓冲区
  *@Parm
  *  *Dev    射频设备
  *  Package 待发射数据指针
  *  Length  待发射数据长度。若超过数据包最大长度，则取最大。
  *  Repeat  =1，重复发射；=0，只发射一次
  */
void RFSendPackage(RFDevice *Dev, uint8_t* Package, uint16_t Length, uint8_t Repeat) {
	RFPackage* pacRes;
	uint16_t index; 
	
	if(Length > RF_PACKAGE_LENGTH) Length = RF_PACKAGE_LENGTH;	
	for(index=0; index<Length; index++) pacRes->buf[index] = Package[index];
	pacRes->length = Length;
	pacRes->id++;
	pacRes->style.repeat = Repeat;	
}


/**@brief 数据包拼接
  *
  */
uint8_t RFPackageSplice(uint8_t* FIFO) {
  RFPackage* package;
	uint8_t i, length;
	
	if(package->id == (FIFO[0] & 0xf0)){
		if((package->style.sequence+1) == (FIFO[0]&0x0f)) {
			package->style.sequence++; 
			length = FIFO[1] & 0x1f;
			for(i=0; i<length; i++) package->buf[package->length+i] = FIFO[2+i];
			package->length += length;			
			
			if(FIFO[1] & 0x80) {
				return TRUE;
			}
		}
	} else {
		package->id = FIFO[0] & 0xf0;
		package->style.sequence = FIFO[0] & 0x0f;
		
		length = FIFO[1] & 0x1f;
		package->length = length;
		for(i=0; i<length; i++) package->buf[i] = FIFO[i+2];
	}
	
	return FALSE;
}






