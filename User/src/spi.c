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
#include "spi.h"

void SPILowLevelInit(void) {
	SPI_InitTypeDef  SPI_InitStructure;

	Bk242x_LowLevel_Init();
 
	/*!< Deselect the FLASH: Chip Select high */
  Bk242x_CS_HIGH();

	/*!< SPI configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	
  #if defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || defined (STM32F10X_HD_VL)
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
  #else
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  #endif

	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(Bk242x_SPI, &SPI_InitStructure);

  /*!< Enable the sFLASH_SPI  */
  SPI_Cmd(Bk242x_SPI, ENABLE);
}

/**
  * @brief  Sends a byte through the SPI interface and return the byte received
  *         from the SPI bus.
  * @param  byte: byte to send.
  * @retval The value of the received byte.
  */
uint8_t SPI_SendByte(uint8_t byte) 
{
  /*!< Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(Bk242x_SPI, SPI_I2S_FLAG_TXE) == RESET);

  /*!< Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(Bk242x_SPI, byte);
 
  /*!< Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(Bk242x_SPI, SPI_I2S_FLAG_RXNE) == RESET);
 
  /*!< Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(Bk242x_SPI);
}

/**
  * @brief  
  */
void SPI_Write_Reg(uint8_t Addr, uint8_t Byte) {
  Bk242x_CS_LOW();	
  SPI_SendByte(Addr);
  SPI_SendByte(Byte);	 
  Bk242x_CS_HIGH();
}

/**
  * @brief  
  */
void SPI_Write_Buf(uint8_t Addr, const uint8_t *Buf, uint8_t BufLength) {	
	uint8_t i;

  Bk242x_CS_LOW();	
  SPI_SendByte(Addr);
	for(i = 0; i < BufLength; i++) {
    SPI_SendByte(*Buf++);		
	}	 
  Bk242x_CS_HIGH();
} 

uint8_t SPI_Read_Reg(uint8_t RegAddr)
{
	uint8_t Temp;

  Bk242x_CS_LOW();	
	Temp = SPI_SendByte(RegAddr);
	Temp = SPI_SendByte(0x00);
  Bk242x_CS_HIGH();  
	
	return Temp;
}

void SPI_Read_Buf(uint8_t RegAddr, uint8_t *Buf, uint8_t BufLength)
{
	uint8_t i;

  Bk242x_CS_LOW();	 
	SPI_SendByte(RegAddr);
	for(i=0; i<BufLength; i++)
		Buf[i] = SPI_SendByte(0x00);
  Bk242x_CS_HIGH();	
}
 

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------

/**
  * @brief  
  */
void SPI_Write_Reg_2(uint8_t Addr, uint8_t Byte, void (*CS_H)(void), void (*CS_L)(void)){
  CS_L();	 
  SPI_SendByte(Addr);
  SPI_SendByte(Byte);	 
  CS_H(); 
}

/**
  * @brief  
  */
void SPI_Write_Buf_2(uint8_t Addr, const uint8_t *Buf, uint8_t BufLength, void (*CS_H)(void), void (*CS_L)(void)){	
	uint8_t i;

  CS_L();	 
  SPI_SendByte(Addr);
	for(i = 0; i < BufLength; i++) {
    SPI_SendByte(*Buf++);		
	}	 
  CS_H(); 
} 

uint8_t SPI_Read_Reg_2(uint8_t RegAddr, void (*CS_H)(void), void (*CS_L)(void))
{
	uint8_t Temp;

  CS_L();	 
	Temp = SPI_SendByte(RegAddr);
	Temp = SPI_SendByte(0x00);
  CS_H(); 
	
	return Temp;
}

void SPI_Read_Buf_2(uint8_t RegAddr, uint8_t *Buf, uint8_t BufLength, void (*CS_H)(void), void (*CS_L)(void))
{
	uint8_t i;

  CS_L();	  
	SPI_SendByte(RegAddr);
	for(i=0; i<BufLength; i++)
		Buf[i] = SPI_SendByte(0x00);
  CS_H(); 
}
 

 
