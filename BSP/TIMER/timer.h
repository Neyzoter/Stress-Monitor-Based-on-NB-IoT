/*
 * Stress Monitor Based on NB-IoT
 * 项目名称：基于NB-IoT的应力监测系统
 * 作者：宋超超
 *
 * 该代码版权归宋超超所有，请勿用于商业用途。
 * https://neyzoter.github.io/
 */
#ifndef __TIMER_H
#define __TIMER_H

#include "sys.h"

extern TIM_HandleTypeDef htim2; 

void TIM2_Init(u16 psc,u16 arr);
	 
#endif

