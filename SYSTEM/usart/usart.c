#include "usart.h"


#if SYSTEM_SUPPORT_OS 
#include "FreeRTOS.h"//FreeRTOS
#endif



#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART3->SR&0X40)==0);//循环发送,直到发送完毕   
	USART3->DR = (u8) ch;      
	return ch;
}
#endif 

#if EN_USART3_RX   //如果使能了接收
   	

//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	

u8 aRxBuffer[RXBUFFERSIZE];//HAL库使用的串口接收缓冲

#endif	


UART_HandleTypeDef husart3; //UART句柄

USART usart = {0,0,0,0,0};

//初始化IO 串口1 
//bound:波特率
void uart_init(u32 bound)
{	
	//UART 初始化设置
	husart3.Instance=USART3;					    //USART1
	husart3.Init.BaudRate=bound;				    //波特率
	husart3.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	husart3.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	husart3.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
	husart3.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	husart3.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	husart3.Init.OverSampling = UART_OVERSAMPLING_16;
	if(HAL_UART_Init(&husart3) != HAL_OK)//HAL_UART_Init()会使能UART1
	{
		Error_Handler(__FILE__, __LINE__);
	}	
	
#if EN_USART3_RX   //如果使能了接收
	HAL_UART_Receive_IT(&husart3, (u8 *)aRxBuffer, RXBUFFERSIZE);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
#endif
	
}

//UART底层初始化，时钟使能，引脚配置，中断配置
//此函数会被HAL_UART_Init()调用
//huart:串口句柄

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO端口设置
	GPIO_InitTypeDef GPIO_Initure;
	
	if(huart->Instance==USART3)//如果是串口3，进行串口3 MSP初始化
	{
		__HAL_RCC_GPIOB_CLK_ENABLE();			//使能GPIOB时钟
		__HAL_RCC_USART3_CLK_ENABLE();			//使能USART3时钟
	
		GPIO_Initure.Pin=GPIO_PIN_10|GPIO_PIN_11;			//PB10、A11
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
		GPIO_Initure.Pull=GPIO_PULLDOWN;			//原来是上拉
		GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;		//高速
		GPIO_Initure.Alternate=GPIO_AF7_USART3;	//复用为USART3
		HAL_GPIO_Init(GPIOB,&GPIO_Initure);	   	//初始化PB10、A10

		
#if EN_USART3_RX       //如果使能了接收那么需要开启中断
		HAL_NVIC_EnableIRQ(USART3_IRQn);				//使能USART1中断通道
		HAL_NVIC_SetPriority(USART3_IRQn,0,0);			//抢占优先级0，子优先级1
#endif	
	}

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART3)//如果是串口1
	{
		if(usart.num >= USART_REC_LEN)//超出范围，有问题。重新接收。
		{
			usart.num = 0;
			return;
		}
		usart.USART_RX_BUF[usart.num] = aRxBuffer[0];//(USART3->DR);	//读取接收到的数据	
		usart.num++;
		usart.USART_RX_BUF[usart.num] = 0;//确保后面一个是结尾符，0

	}
}
//串口1中断服务程序
void USART3_IRQHandler(void)                	
{ 
	u32 timeout=0;
	
	HAL_UART_IRQHandler(&husart3);	//调用HAL库中断处理公用函数
	
	timeout=0;
    while (HAL_UART_GetState(&husart3) != HAL_UART_STATE_READY)//等待就绪
	{
	 timeout++;////超时处理
     if(timeout>HAL_MAX_DELAY) break;		
	
	}
     
	timeout=0;
	while(HAL_UART_Receive_IT(&husart3, (u8 *)aRxBuffer, RXBUFFERSIZE) != HAL_OK)//一次处理完成之后，重新开启中断并设置RxXferCount为1
	{
	 timeout++; //超时处理
	 if(timeout>HAL_MAX_DELAY) break;	
	}
} 






