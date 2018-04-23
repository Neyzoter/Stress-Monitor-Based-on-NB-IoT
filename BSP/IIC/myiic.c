/*
 * Stress Monitor Based on NB-IoT
 * 项目名称：基于NB-IoT的应力监测系统
 * 作者：宋超超
 *
 * 该代码版权归宋超超所有，请勿用于商业用途。
 * https://neyzoter.github.io/
 */

#include "myiic.h"
#include "main.h"
	

//IIC初始化
void IIC_Init(void)
{
    GPIO_InitTypeDef gpioinitstruct;
    
    __HAL_RCC_GPIOB_CLK_ENABLE();   //使能GPIOB时钟
    
    //PB6、B7初始化设置
    gpioinitstruct.Pin=GPIO_PIN_6|GPIO_PIN_7;
    gpioinitstruct.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    gpioinitstruct.Pull=GPIO_PULLUP;          //上拉
    gpioinitstruct.Speed=GPIO_SPEED_FREQ_VERY_HIGH;     //快速
    HAL_GPIO_Init(GPIOB,&gpioinitstruct);
    
    IIC_SDA=1;
    IIC_SCL=1;  
}

//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	IIC_SDA=1;	  	  
	IIC_SCL=1;
//	n_nops(1);
	__NOP;
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
//	n_nops(1);//需要延时4us
	__NOP;
	IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
// 	n_nops(1);//需要延时4us
	__NOP;
	IIC_SCL=1; 
	IIC_SDA=1;//发送I2C总线结束信号
//	n_nops(1);//需要延时4us		
	__NOP;
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	IIC_SDA=1;__NOP;//n_nops(1);//需要延时1us	   
	IIC_SCL=1;__NOP;//n_nops(1);//需要延时1us	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
//	n_nops(1);//需要延时2us
	__NOP;
	IIC_SCL=1;
//	n_nops(1);//需要延时2us
	__NOP;
	IIC_SCL=0;
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
//	n_nops(1);//需要延时2us
	__NOP;
	IIC_SCL=1;
//	n_nops(1);//需要延时2us
	__NOP;
	IIC_SCL=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
//		n_nops(1);//需要延时2us  
		__NOP;
		IIC_SCL=1;
//		n_nops(1);//需要延时2us
		__NOP;
		IIC_SCL=0;	
//		n_nops(1);//需要延时2us
		__NOP;
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
//        n_nops(1);//需要延时2us
		__NOP;
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
//		n_nops(1);//需要延时1us
		__NOP;
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}

void n_nops(u8 n)
{
	u8 num = n;
	while(num--)
	{};
}



