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
#include "stickKnob.h"
#include "eeprom.h"
#include "adc.h"

StickKnob *pStickKnobBuf = ConfigGlobal.StickKnobBuf;
StickKnob *StickLeftPitch, *StickLeftRoll, *StickRightPitch, *StickRightRoll, \
          *Knob1, *Knob2, *Knob3, *Knob4;

uint16_t StickKnobCalTimer;


/** @Brief 摇杆\旋钮初始化
  *   1) 定义电位器方向
  *   2) 定义电位器对应ADC通道
	*   3) 另，各电位器端点及中值已在读取EEPROM时更新
  */
void StickKnobOpen(void) {
	//1) 映射
	StickLeftPitch  = &pStickKnobBuf[0];
	StickLeftRoll   = &pStickKnobBuf[1];
	StickRightPitch = &pStickKnobBuf[2];
	StickRightRoll  = &pStickKnobBuf[3];
	Knob1 = &pStickKnobBuf[4];
	Knob2 = &pStickKnobBuf[5];
	Knob3 = &pStickKnobBuf[6];
	Knob4 = &pStickKnobBuf[7];	
	
	//2) 初始化摇杆\旋钮方向
	StickLeftPitch->bDerictionReverse = 0;
	StickLeftRoll->bDerictionReverse = 0;
	StickRightPitch->bDerictionReverse = 0;
	StickRightRoll->bDerictionReverse = 0;
	Knob1->bDerictionReverse = 0;
	Knob2->bDerictionReverse = 0;
	Knob3->bDerictionReverse = 0;
	Knob4->bDerictionReverse = 0;
	
	//3) 初始化摇杆\旋钮ADC通道
	StickLeftPitch->adcChannel  = ADC_Channel_0;
	StickLeftRoll->adcChannel   = ADC_Channel_1;
	StickRightPitch->adcChannel = ADC_Channel_2;
	StickRightRoll->adcChannel  = ADC_Channel_3;
	Knob1->adcChannel = ADC_Channel_4;
	Knob2->adcChannel = ADC_Channel_5;
	Knob3->adcChannel = ADC_Channel_6;
	Knob4->adcChannel = ADC_Channel_7;		
}


/** @Brief 获取电位器\旋钮数值
  * @Return StickKnob.output = 0~1000
  */
void StickKnobUpdate(void) {
	uint16_t adc, result;
	float def, range;
	uint8_t i;
	
	//1) 依次更新所有电位器
	for(i=0; i<STICK_KNOB_SUM; i++) {
		adc = UserADCGetValue(pStickKnobBuf[i].adcChannel);
		
		//2) 以中值为界，上下两段分别解算
		if(adc >= pStickKnobBuf[i].adcLimitMiddle){
			def = adc - pStickKnobBuf[i].adcLimitMiddle;
			range = pStickKnobBuf[i].adcLimitHigh - pStickKnobBuf[i].adcLimitMiddle;
			if(range <= 0) range = 1;
			def /= range;
			result = def * 500.0f;
			if(result > 500) result = 500;
			pStickKnobBuf[i].position = 500 + result;
		} else {
			def = pStickKnobBuf[i].adcLimitMiddle - adc;
			range = pStickKnobBuf[i].adcLimitMiddle - pStickKnobBuf[i].adcLimitLow;
			if(range <= 0) range = 1;
			def /= range;
			result = def * 500.0f;
			if(result > 500) result = 500;
			pStickKnobBuf[i].position = 500 - result;
		}			
		
		//3) 若电位器反向，调整输出数据为(左小右大、后小前大)
		if(pStickKnobBuf[i].bDerictionReverse) pStickKnobBuf[i].output = 1000 - pStickKnobBuf[i].position;
		else pStickKnobBuf[i].output = pStickKnobBuf[i].position;
	}	
}


/** @Brief 电位器\旋钮矫正上下边缘
  *
  */
void StickKnobCalibrationEdge(void) {
	uint16_t adc;
	uint8_t i;
	int16_t def;	
	
	//1) 读取电位器ADC数值，更新上下边缘
	//2) 若3s内没有获得新的最大值或最小值，认为校验完成
	StickKnobCalTimer = 0;
	while(StickKnobCalTimer < 3000) {
		if((StickKnobCalTimer%20) == 0) {
			for(i=0; i<STICK_KNOB_SUM; i++) {
		    adc = UserADCGetValue(pStickKnobBuf[i].adcChannel);
		    if(adc > pStickKnobBuf[i].adcLimitHigh){
					pStickKnobBuf[i].adcLimitHigh = adc;
					StickKnobCalTimer = 0;
				}
				if(adc < pStickKnobBuf[i].adcLimitLow){
					pStickKnobBuf[i].adcLimitLow = adc;
					StickKnobCalTimer = 0;
				}
	    }
			StickKnobCalTimer++;
		}
	}
	
	//1) 上下边缘各留出5%的虚位
	for(i=0; i<STICK_KNOB_SUM; i++) {
		def = pStickKnobBuf[i].adcLimitHigh - pStickKnobBuf[i].adcLimitLow;
		def /= 20;
		if(def < 0) def = 0;
		
		pStickKnobBuf[i].adcLimitHigh -= def;
		pStickKnobBuf[i].adcLimitLow += def;
	}	
	
	//1) 保存至EEPROM
	EEPROM_Write();	
}

/** @Brief 电位器\旋钮获取中值
  * @ 多次采用取平均
  *
  * @Note
  *   1) 调用此程序前，各电位器拨到中间位置
  */
void StickKnobCalibrationMiddle(void) {
	uint16_t adc;
	uint32_t adcSum[STICK_KNOB_SUM];
	uint8_t i, count;	
	
	//1) 每20ms采样一次，1s后计算均值
	for(i=0; i<STICK_KNOB_SUM; i++) adcSum[i] = 0;	
	count = 0;
  StickKnobCalTimer = 0;
	while(StickKnobCalTimer < 1000) {
		if((StickKnobCalTimer%20) == 0) {		
	    for(i=0; i<STICK_KNOB_SUM; i++) {	
	      adc = UserADCGetValue(pStickKnobBuf[i].adcChannel);
		    adcSum[i] += adc;
			}
			StickKnobCalTimer++;
			count++;
		}
	}
	if(count == 0) count = 1;
	
	for(i=0; i<STICK_KNOB_SUM; i++) {	
		pStickKnobBuf[i].adcLimitMiddle = adcSum[i]/count;
	}	
	
	//1) 保存至EEPROM
	EEPROM_Write();	
}






