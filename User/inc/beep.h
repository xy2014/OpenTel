
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
#ifndef __BEEP_H
#define __BEEP_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32_eval.h"
#include "stm3210e_eval.h"
    	
typedef struct{	
	uint16_t tune;                 //ÆµÂÊ(Hz)
	uint8_t  duty;                 //0~100
	uint16_t duration;             //Ê±³¤(ms)
 	
	uint8_t  bRunning:1;
	uint8_t  bReStart:1;
  uint16_t durationNew;  	
	
	void (*Start)(uint16_t Tune, uint8_t Duty);
	
	uint16_t timer;
} Beep;
 	 
extern Beep *Beep1, *Vibration;

void BeepNew(Beep *Dev, uint16_t Tune, uint8_t Duty, uint16_t Dur);	 
void BeepCtrl(Beep *Dev);
	 
#ifdef __cplusplus
}
#endif
  
#endif /* __BEEP_H */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/



