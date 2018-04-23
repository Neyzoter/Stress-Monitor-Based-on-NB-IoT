/*
 * Stress Monitor Based on NB-IoT
 * 项目名称：基于NB-IoT的应力监测系统
 * 作者：宋超超
 *
 * 该代码版权归宋超超所有，请勿用于商业用途。
 * https://neyzoter.github.io/
 */

#ifndef __IWDG_H_
#define __IWDG_H_

#include "sys.h"
extern IWDG_HandleTypeDef IWDG_Handler;

void IWDG_Init(u8 prer,u16 rld);
void IWDG_Feed(void);

#endif

