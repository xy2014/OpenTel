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
#include "adc.h"
#include "stm32_eval.h"
#include "stm3210e_eval.h"

void ADCLowLevelInit(void)
{
  ADC_InitTypeDef ADC_InitStructure;

  /* Enable ADC1 clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  RCC_ADCCLKConfig(RCC_PCLK2_Div6);//12MHZ 
	
  /* Configure PA.1 (ADC Channel) as analog input -------------------------*/
	STM_EVAL_ADC_Port_Init(ADCChannel1);
	STM_EVAL_ADC_Port_Init(ADCChannel2);
	STM_EVAL_ADC_Port_Init(ADCChannel3);
	STM_EVAL_ADC_Port_Init(ADCChannel4);
	STM_EVAL_ADC_Port_Init(ADCChannel5);
	STM_EVAL_ADC_Port_Init(ADCChannel6);
	STM_EVAL_ADC_Port_Init(ADCChannel7);
	STM_EVAL_ADC_Port_Init(ADCChannel8);
        
  /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
    
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);

  /* Enable ADC1 reset calibration register */   
  ADC_ResetCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));

  /* Start ADC1 calibration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));
}

/**
  * @brief  ???????ADC??
  *
  */
uint16_t UserADCGetValue(uint8_t ch)
{
  u16 dataValue;  
  
	/* ADC1 regular channel14 configuration */ 
  ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5);
    
  /* Start ADC1 Software Conversion */ 
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);    
  /* Test if the ADC1 EOC flag is set or not */ 
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));	
  /*Returns the ADC1 Master data value of the last converted channel*/
  dataValue = ADC_GetConversionValue(ADC1); 
	
  return dataValue; 
}


