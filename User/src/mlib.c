
#include "mlib.h"

uint16_t ConstrainU16(uint16_t Value, uint16_t Min, uint16_t Max) {
	if(Value < Min) Value = Min;
	if(Value > Max) Value = Max;
	
	return Value;
}


