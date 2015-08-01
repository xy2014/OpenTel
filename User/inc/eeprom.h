
#ifndef __EEPROM_H
#define __EEPROM_H
 
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stickKnob.h"
	 
typedef struct {
	uint16_t good;
	StickKnob StickKnobBuf[STICK_KNOB_SUM];
	uint8_t rfAddr[5];	
} ConfigGlobalTS;	 
	 

extern ConfigGlobalTS ConfigGlobal;

void EEPROM_Write(void);
void EEPROM_Read(void);

#ifdef __cplusplus
}
#endif

#endif  /*__EEPROM_H*/


