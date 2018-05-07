
/*
 * Stress Monitor Based on NB-IoT
 * 项目名称：基于NB-IoT的应力监测系统
 * 作者：宋超超
 *
 * 该代码版权归宋超超所有，请勿用于商业用途。
 * https://neyzoter.github.io/
 */
#ifndef __COMMON_H_
#define __COMMON_H_

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "sys.h"

//系统的主要flag值
typedef struct{	
	u8 BRD_STA;
	u8 * IMEI;
	u8 OLED_REFRESHING;//用于判断OLED屏是否现在正在写
	
}MAIN_FLAG;
extern MAIN_FLAG flag;


//数据状态
#define CMD_GOT_MAXNUM 				50//接收数据长度
#define DATA_MAXNUM 				25//发送数据最长
#define CMD_MAXNUM_STR 				3	//CMD_GOT_MAXNUM用字符串表示最多几位
#define USART_REC_LEN  			    100  	//定义最大接收字节数
#define USART_SEND_LEN  			50  	//定义最大发送字节数
#define DATA_NUM_NEEDED 			5//采集的数据个数

//接收到北向数据的信息
typedef struct{
	u16 len;//长度，不超过512byte
	//数据string格式，最长200bytes。实际有可能传过来512byte的数据
	//该数据有可能既有大写又有小写，如Aa
	u8 lastcmd[CMD_GOT_MAXNUM];
	u8 str[CMD_GOT_MAXNUM/2+1];
	u8 status;//数据的状态
}CMD_GOT;
extern CMD_GOT cmdgot;


//下行命令的下标、个数问题
#define CMD_MAXNUM_STR 3
#define CMD_GOT_START_INDEX 2//数据从下标2开始

//下行命令的状态
#define SEND_MSG_ERROR 0//发送数据错误
#define CMD_GOT_NUM_0 1//没有数据
#define CMD_GOT_TOO_LONG 2//数据太长了
#define CMD_GOT_FORMAT_ERROR 3 //数据格式错误
//数据格式错误，包括数据位数和实际转化位数不对应
//或者数据中包含无法确认的数据。比如转化前是“Z1”，包含了除0-9，a-f或者A-F之外的数据
//该hex数据无法转化为str
#define CMD_GOT_OK 4 //数据接收成功


//板子的状态
#define BRD_SYS_INITING             0  //板子正在初始化
#define BRD_ATTACKNETWORKING        1  //板子正在尝试连接
#define BRD_ATTACKED                2  //板子连接成功


//NBIOT数据发送方式
#define  TELECHINA_NBAND     "5"//电信
#define  UDP_LOCAL_PORT "4567"
#define  UDP_SERVER_IP_PORT  	"114.115.144.122,4567"			//UDP:远程服务器IP地址和端口号
#define  CDP_SERVER_IP_PORT  	"180.101.147.115,5683"			//COAP:远程服务器IP地址和端口号，电信iot

//NBIOT相关操作
#define  API_OPEN_REPEAT_NUMBER  	3		//操作多次，失败之后 停留N长时间之后在做尝试，不能总是在尝试-费电，也不能仅尝试一次
#define  API_CLOSE_REPEAT_NUMBER  	3		//操作多次，失败之后 停留N长时间之后在做尝试，不能总是在尝试-费电，也不能仅尝试一次
#define  API_SEND_REPEAT_NUMBER  	3		//操作多次，失败之后 停留N长时间之后在做尝试，不能总是在尝试-费电，也不能仅尝试一次
#define  API_RECV_REPEAT_NUMBER  	3		//操作多次，失败之后 停留N长时间之后在做尝试，不能总是在尝试-费电，也不能仅尝试一次

//时间参数
#define  TIMER_REPEAT_NBIOT_REGISTER	3600 	//单位秒：1小时=60分钟x60秒
#define  TIMER_REPEAT_NBIOT_RECV		3		//单位秒：3
#define  COUNT_REPEAT_NBIOT_RECV		10		//尝试读取10次，间隔时间为TIMER_REPEAT_RECV
#define  TIMER_KEY_PUSH_DELAY			10		//单位秒：10,按键按下10秒
#define  WAKEUP_TIME					100		//待机后唤醒设备的时间/秒

//AT指令尝试重复操作的次数(固定)
#define  NBIOT_RESET_DELAYTIME  	5	//在操作AT指令出现错误，重复操作的次数，默认时间 = (0.3S * N)S
#define  AT_DEFAULT_REPEAT_NUMBER  	3		//默认次数
#define  AT_NTSETID_REPEAT_NUMBER   3         //设置IMEI号次数
#define  AT_NRB_REPEAT_NUMBER  		3		//重启
#define  AT_NCDP_REPEAT_NUMBER  	3		//设置CDP次数
#define  AT_NBAND_REPEAT_NUMBER  	3		//设置CDP次数
#define  AT_AT_REPEAT_NUMBER  		3		//AT指令重复次数

#define  AT_CCLK_REPEAT_NUMBER  	3		//在操作AT指令出现错误，重复操作的次数
#define  AT_NCONFIG_REPEAT_NUMBER  	3		//
#define  AT_CFUN_REPEAT_NUMBER  	3		//在操作AT指令出现错误，重复操作的次数
#define  AT_CSQ_REPEAT_NUMBER  		3		//在操作AT指令出现错误，重复操作的次数
#define  AT_CGATT_REPEAT_NUMBER  	8		//CGATT经常是0，所以这里把它的重复次数设置成8次
#define  AT_CEREG_REPEAT_NUMBER  	3		//在操作AT指令出现错误，重复操作的次数
#define  AT_NSOCR_REPEAT_NUMBER  	3		//在操作AT指令出现错误，重复操作的次数
#define  AT_NSOST_REPEAT_NUMBER  	3		//在操作AT指令出现错误，重复操作的次数
#define  AT_NSORF_REPEAT_NUMBER  	3		//在操作AT指令出现错误，重复操作的次数
#define  AT_NMGS_REPEAT_NUMBER  	5		//在操作AT指令出现错误，重复操作的次数
#define  AT_NMGR_REPEAT_NUMBER  	3		//在操作AT指令出现错误，重复操作的次数


//AT指令等待时间，滴答定时器1ms单位
#define  AT_DEFAULT_WAIT_TIME  	3000		//默认3s
#define  AT_NMGS_WAIT_TIME  	3000	//发送数据等待时间
#define  AT_NMGR_WAIT_TIME  	5000	//接收数据等待时间
#define  AT_CGATT_WAIT_TIME  	5000		//CGATT等待时间
#define  AT_NRB_WAIT_TIME  	    6000		//重启等待时间6s


//AT指令发送时的等待时间
#define  AT_BEFORE_NMGR_WAIT_TIME  	4000		//发送完数据后，需要等待一段时间才能接收下行数据
#define  NEXT_AT_WAIT_TIME      1000     //在发送下一个AT指令前等待1秒



#endif /* COMMON_H_ */
