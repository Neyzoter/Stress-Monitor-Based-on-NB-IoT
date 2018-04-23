/*
 * Stress Monitor Based on NB-IoT
 * 项目名称：基于NB-IoT的应力监测系统
 * 作者：宋超超
 *
 * 该代码版权归宋超超所有，请勿用于商业用途。
 * https://neyzoter.github.io/
 */


#include "key.h"
#include "led.h"

//IIC初始化
void KEY_Init(void)
{
    GPIO_InitTypeDef gpioinitstruct;
    
    __HAL_RCC_GPIOC_CLK_ENABLE();   //使能GPIOC时钟
    
    //PC13初始化设置
    gpioinitstruct.Pin=GPIO_PIN_13;
    gpioinitstruct.Mode=GPIO_MODE_INPUT;  //推挽输出
    gpioinitstruct.Pull=GPIO_PULLUP;          //上拉
    gpioinitstruct.Speed=GPIO_SPEED_FREQ_VERY_HIGH;     //快速
    HAL_GPIO_Init(GPIOC,&gpioinitstruct);
 
}

//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY0按下

//注意此函数有响应优先级,KEY0>KEY1>KEY2>KEY3!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(KEY0==0))
	{
		HAL_Delay(10);//去抖动 
		key_up=0;
		if(KEY0==0)return KEY0_PRESSED;

	}	  
	else if(!key_up&&(KEY0==1))
		key_up = 1;
 	return 0;// 无按键按下
}

//按键测试
//如果按键按下，LED灯取反
void key_test(void)
{
	if(KEY_Scan(0) == KEY0_PRESSED)
	{
		HAL_GPIO_TogglePin(LED2_GPIO_PORT,LED2_PIN);
	}
}


