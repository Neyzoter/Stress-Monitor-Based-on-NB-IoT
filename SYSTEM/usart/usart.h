#ifndef _USART_H
#define _USART_H
#include "sys.h"
#include "stdio.h"	
#include "common.h"


#define EN_USART3_RX 			1		//使能（1）/禁止（0）串口1接收
#define USART3_TX_SET()			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET) 
#define USART3_TX_RESET()			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET) 
#define USART3_RX_SET()			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET) 
#define USART3_RX_RESET()			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET) 

typedef struct{
	volatile u8 status;
	volatile u16 num;
	volatile u8  USART_RX_BUF[USART_REC_LEN];//用于给接收数据的时间判定
	volatile u8  WAIT_START;
	volatile u16 waittime;
}USART;
extern USART usart;

extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	
extern UART_HandleTypeDef husart3; //UART句柄

#define RXBUFFERSIZE   1 //缓存大小
extern u8 aRxBuffer[RXBUFFERSIZE];//HAL库USART接收Buffer

//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);


#endif	/*_USART_H_*/
