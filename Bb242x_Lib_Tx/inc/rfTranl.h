/**
  ******************************************************************************
  * @file    stm3210b_eval.h
  * @author  MCD Application Team
  * @version V4.5.0
  * @date    07-March-2011
  * @brief   This file contains definitions for STM3210B_EVAL's Leds, push-buttons
  *          COM ports, SD Card (on SPI), sFLASH (on SPI) and Temperature sensor 
  *          LM75 (on I2C) hardware resources.
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
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RFTRANL_H
#define __RFTRANL_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"    
	
#define RF_FRAME_DATA_LENGTH  30
#define RF_PACKAGE_LENGTH     300

//RF 数据包
//@brief 数据发射采用类似双缓冲机制。一个数据包表示正在发射的数据，另一个为准备要发送的数据。
typedef union {
	uint8_t sequence;                         //数据发射及接收时，数据包拆分后的序列号
	uint8_t repeat;                           //数据准备发射时，是否重复发射标志
} RFPagStyle;	 
	 
typedef struct {
	uint8_t buf[RF_PACKAGE_LENGTH];           //数据包的实际存储位置
	uint16_t length;                          //数据长度(小于等于RF_PACKAGE_LENGTH)
	uint16_t id;                              //数据包编号
	RFPagStyle style;                         //不同数据包表示含义不同
} RFPackage;

	 
#ifdef __cplusplus
}
#endif
  
#endif /* __RFTRANL_H */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/



