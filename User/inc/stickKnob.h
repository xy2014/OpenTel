
#ifndef __STICKKNOB_H
#define __STICKKNOB_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32_eval.h"
#include "stm3210e_eval.h"

#define STICK_KNOB_SUM        8

typedef struct {
	uint16_t adcLimitHigh;
	uint16_t adcLimitLow;
	uint16_t adcLimitMiddle;
	
	uint16_t position;
	uint16_t output;
	
	uint8_t  adcChannel;
	uint8_t  bDerictionReverse;
} StickKnob;	  
	 
void StickKnobOpen(void);
void StickKnobUpdate(void);
void StickKnobCalibrationEdge(void);
void StickKnobCalibrationMiddle(void);

#ifdef __cplusplus
}
#endif


#endif /* #ifndef __STICKKNOB_H */


