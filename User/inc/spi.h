
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
#ifndef __SPI_H
#define __SPI_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32_eval.h"
#include "stm3210e_eval.h"
    
void SPILowLevelInit(void);
uint8_t SPI_SendByte(uint8_t byte);	 
	 
void SPI_Write_Reg(uint8_t Addr, uint8_t Byte);
void SPI_Write_Buf(uint8_t Addr, const uint8_t *Buf, uint8_t BufLength);
uint8_t SPI_Read_Reg(uint8_t RegAddr);
void SPI_Read_Buf(uint8_t RegAddr, uint8_t *Buf, uint8_t BufLength);	 	 
	 
	 
void SPI_Write_Reg_2(uint8_t Addr, uint8_t Byte, void (*CS_H)(void), void (*CS_L)(void));
void SPI_Write_Buf_2(uint8_t Addr, const uint8_t *Buf, uint8_t BufLength, void (*CS_H)(void), void (*CS_L)(void));
uint8_t SPI_Read_Reg_2(uint8_t RegAddr, void (*CS_H)(void), void (*CS_L)(void));
void SPI_Read_Buf_2(uint8_t RegAddr, uint8_t *Buf, uint8_t BufLength, void (*CS_H)(void), void (*CS_L)(void));
	 
#ifdef __cplusplus
}
#endif
  
#endif /* __SPI_H */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/



