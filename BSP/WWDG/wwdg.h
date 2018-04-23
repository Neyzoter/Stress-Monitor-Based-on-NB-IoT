/*
 * Stress Monitor Based on NB-IoT
 * 项目名称：基于NB-IoT的应力监测系统
 * 作者：宋超超
 *
 * 该代码版权归宋超超所有，请勿用于商业用途。
 * https://neyzoter.github.io/
 */

#ifndef __WWDG_H_
#define __WWDG_H_

#include "sys.h"

extern WWDG_HandleTypeDef hwwdg;

void WWDG_Init(u8 win,u8 cnt , u32 psc);

#endif
