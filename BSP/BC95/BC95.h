/*
 * Stress Monitor Based on NB-IoT
 * 项目名称：基于NB-IoT的应力监测系统
 * 作者：宋超超
 *
 * 该代码版权归宋超超所有，请勿用于商业用途。
 * https://neyzoter.github.io/
 */

#ifndef __BC95_H
#define __BC95_H
#include "common.h"

#define AT_TICKLESS_ON 1	//是否开始在BC95联网等待usart信号时，进入tickless
#define  AT_CHECK_EVERYTIME		300 //用于在发送AT指令后，每次200ms进入一次来查看是否时间超过了设定的等待时间	
#define WAITINGTIME_Y    3//显示等待时间的纵坐标
#define WAITINGTIME_X    12//显示等待时间的横坐标
#define WAITINGTIME_DISP_DIGITS    4//显示等待时间的横坐标
//总体步骤
u8 AttachNetwork(void);
//基本函数
u8 AT_NRB(void);
u8 AT_Test(void);
//发送AT指令，网络连接
u8 CDP_Get_Cfg(char * ip_port);
u8 CDP_Cfg(char * ip_port);
u8 CFUN_Get_Set(void);
u8 CFUN_Set(void);
u8 NBAND_Get_Set(char * nband);
u8 NBAND_Set(char * nband);
u8 CGATT_Get_Set(void);
u8 CGATT_Set(void);
//数据发送
u8 MSG_Send(char * msg,char * len);
u8 intMSG_Send(u16 num,u8 len);//发送int  message
//数据接收
u8 CMD_Get(void);
//简短的函数
void StartWaitMsg(void);
void StopWaitMsg(void);

#endif

