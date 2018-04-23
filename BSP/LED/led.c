/*
 * Stress Monitor Based on NB-IoT
 * 项目名称：基于NB-IoT的应力监测系统
 * 作者：宋超超
 *
 * 该代码版权归宋超超所有，请勿用于商业用途。
 * https://neyzoter.github.io/
 */


#include "led.h"
#include "sys.h"


void BSP_LED_Init(void)
{
	GPIO_InitTypeDef  gpioinitstruct;

	LED2_CLK_ENABLE;
	gpioinitstruct.Pin    = LED2_PIN;
	gpioinitstruct.Mode   = GPIO_MODE_OUTPUT_PP;
	gpioinitstruct.Pull   = GPIO_NOPULL;
	gpioinitstruct.Speed  = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(LED2_GPIO_PORT , &gpioinitstruct);
	LED2_OFF();
	
	
	LED1_CLK_ENABLE;
	gpioinitstruct.Pin    = LED1_PIN;
	gpioinitstruct.Mode   = GPIO_MODE_OUTPUT_PP;
	gpioinitstruct.Pull   = GPIO_NOPULL;
	gpioinitstruct.Speed  = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(LED1_GPIO_PORT , &gpioinitstruct);

	LED1_OFF();	
	
}



