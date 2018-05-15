/*
 * Stress Monitor Based on NB-IoT
 * 项目名称：基于NB-IoT的应力监测系统
 * 作者：宋超超
 *
 * 该代码版权归宋超超所有，请勿用于商业用途。
 * https://neyzoter.github.io/
 */

#ifndef _LOWPOWER_H
#define _LOWPOWER_H
#include "sys.h"

#define BACKUP_AREA   RTC_BKP_DR1
#define RESET_FROM_PWRDOWN 0  //从掉电启动
#define RESET_FROM_STANDBY 1   //从待机模式启动
#define NEED_REATTACH_NETWORK	0  //需要重新连接网络
extern RTC_HandleTypeDef hrtc;

void Sys_Entry_Standby(void);
void RTC_Init(u16 WKUP_Seconds);
void PreSleepProcessing(uint32_t ulExpectedTime);
void PostSleepProcessing(uint32_t ulExpectedTime);
void RealWaitingTimeProcessing(uint32_t ulCompleteTickPeriods);


#endif


