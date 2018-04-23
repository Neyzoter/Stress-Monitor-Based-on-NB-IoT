#ifndef __TIMER_H
#define __TIMER_H

#include "sys.h"

extern TIM_HandleTypeDef htim2; 

void TIM2_Init(u16 psc,u16 arr);
	 
#endif

