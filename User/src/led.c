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
#include "led.h"
#include "stm32_eval.h"
#include "stm3210e_eval.h"

static uint8_t LedTimer;

/**
  * @brief  初始化LED端口，点亮LED。
  */
void LedOpen(void) {
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
	
	STM_EVAL_LEDOn(LED1);
	STM_EVAL_LEDOn(LED2);
}

/**
  * @brief  Led主控制程序 
  *
  */
void LedCtrl(Led_SysStatus LedStatus) {
	static uint8_t ledFlashCnt = 0;
	
  switch(LedStatus) {
		case POWER_ONL:
      STM_EVAL_LEDOn(LED1);
	    STM_EVAL_LEDOn(LED2);
		  break;
		
	  case STICK_CAL:
		  if(LedTimer >= 2){
		    LedTimer = 0;
		    STM_EVAL_LEDToggle(LED1); 	
		    STM_EVAL_LEDToggle(LED2); 	
		  }
			break;
			
	  case RF_MAKE_PAIR:
		  if(LedTimer >= 4){
		    LedTimer = 0;
				
		    ledFlashCnt++;
		    if(ledFlashCnt >= 4) {
			    STM_EVAL_LEDOff(LED1);
			    STM_EVAL_LEDOff(LED2);
		    } else {				
		      STM_EVAL_LEDToggle(LED1); 	
		      STM_EVAL_LEDToggle(LED2); 
		    }
		    if(ledFlashCnt >= 10) ledFlashCnt=0;
		  }
		  break;
			
		default:
      STM_EVAL_LEDOn(LED1);
	    STM_EVAL_LEDOn(LED2);
			break;
	}
}

/**
  * @brief  Led时钟。由中断调用。
  */
void LedTick(void) {
  LedTimer++;
}
    
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
