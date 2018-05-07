/*
 * Stress Monitor Based on NB-IoT
 * 项目名称：基于NB-IoT的应力监测系统
 * 作者：宋超超
 *
 * 该代码版权归宋超超所有，请勿用于商业用途。
 * https://neyzoter.github.io/
 */
#ifndef __I2C_OLED_H
#define __I2C_OLED_H

#include "sys.h"
////OLED信息缓存
//typedef struct{	
//	u8 olfl_line1[3];//存放是否在线OL或者FL
//	u8 cmd_title[5];//"CMD:"
//	u8 msg_title[5];//"MSG:"
//	u8 msg_line1[17];//第1行的信息，是否overtime
//	u8 cmd_line2[22];//发送的AT命令
//	u8 msg_line4[22];//接收到的屏幕第4行信息,实际第1行信息
//	u8 msg_line5[22];
//	u8 msg_line6[22];
//	u8 msg_line7[22];
//	u8 msg_line8[22];//存放压力数据和AT收到的msg
//	u16 OLED_REFRESHING_STAT;//每一位表示一个模块是否要更新，1：需要更新；0：不需要更新
//}OLED_TEMP;
//extern OLED_TEMP oled_temp;

//#define OLFL_LINE1_BIT_CS	0x0001
//#define CMD_TITLE_BIT_CS	0x0001<<1
//#define MSG_TITILE_BIT_CS	0x0001<<2
//#define MSG_LINE1_BIT_CS	0x0001<<3
//#define CMD_LINE2_BIT_CS	0x0001<<4
//#define MSG_LINE4_BIT_CS	0x0001<<5
//#define MSG_LINE5_BIT_CS	0x0001<<6
//#define MSG_LINE6_BIT_CS	0x0001<<7
//#define MSG_LINE7_BIT_CS	0x0001<<8
//#define MSG_LINE8_BIT_CS	0x0001<<9
//#define CLEAR_SCREEN_BIT_CS	0x0001<<10

//#define ClearScreenFlagSet()		{oled_temp.OLED_REFRESHING_STAT |=  CLEAR_SCREEN_BIT_CS}  //设置清屏flag，可以自动清屏



#define OLED_ADDRESS	0x78 //通过调整0R电阻,屏可以0x78和0x7A两个地址 -- 默认0x78
#define CMD_START_X       30    //OLED上命令显示的位置
#define CMD_START_Y       1
#define MSG_START_X		  30	//OLED上AT消息开始的x
#define RCV_START_X       12     //信息再OLED上开始的位置
#define RCV_START_X_NoNewLine       30     //信息再OLED上开始的位置
#define LINE_FEED         1//信息换行开始显示
#define NO_LINE_FEED      0//信息不换行显示
#define RCV_START_Y       3 
#define OLED_X_MAX        127
#define OLED_Y_MAX        7
#define SGN_START_X       18
#define SGN_START_Y       0

#define OLED_ADDRESS	0x78 //通过调整0R电阻,屏可以0x78和0x7A两个地址 -- 默认0x78
//#define OLED_MAX_TRIALS 300   //最多进行这么多次HAL_I2C_IsDeviceReady，设备准备完毕
//#define I2C_XFER_TIMEOUT_MAX    300//IIC最多等待的时间

void I2C_OLED_Init(void);//初始化IIC

void I2C_WriteByte(u8 addr,u8 data);
void WriteCmd(u8 I2C_Command);//写命令
void WriteDat(u8 I2C_Data);//写数据
void OLED_Init(void);


void OLED_SetPos(u8 x, u8 y); //设置起始点坐标
void OLED_Fill(u8 fill_Data);//全屏填充
void OLED_CLS(void);//清屏
void OLED_ON(void);//打开OLED
void OLED_OFF(void);//关闭OLED
void OLED_ShowStr(u8 x,u8 y,u8 ch[],u8 TextSize);
void OLED_ShowCN(u8 x,u8 y,u8 N);//显示汉字
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len,u8 TextSize);//显示数字


//具体的显示函数
void MsgDisp(u8 linefeed);//显示信息
void InitDisp(void);//显示初始化
void cmdDisp(u8 * cmd);//显示cmd
void sgnDisp(u8 * sgn);//显示信息，比如overtime，信号强度等
void MsgSentDisp(u8 * msg);//显示已经发送的数据
void intMsgSentDisp(u16 intmsg);//发送int型数据的显示
void CMD_GOT_STA_Disp(void);//显示接收到的信息状态
void CMD_GOT_Disp(void);//显示接收到的数据
void BrdStaDisp(void);//更新板子的状态OL还是FL
void Error_Handler_Disp(char * file,int line);//显示错误的文件和行数，在Error_Handler中使用
#endif

