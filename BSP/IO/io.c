/*
 * Stress Monitor Based on NB-IoT
 * 项目名称：基于NB-IoT的应力监测系统
 * 作者：宋超超
 *
 * 该代码版权归宋超超所有，请勿用于商业用途。
 * https://neyzoter.github.io/
 */


#include "IO.h"
#include "sys.h"

//控制信号调理电路的IO设置
void IO_Init(void)
{
	GPIO_InitTypeDef  gpioinitstruct;


	/* Reset PIN to switch off the LED */
	HAL_GPIO_WritePin(LMCS_GPIO_PORT,LMCS_PIN , GPIO_PIN_SET);
	/* Enable the GPIO_LED Clock */
	LMCS_CLK_ENABLE;

	/* Configure the GPIO_LED pin */
	gpioinitstruct.Pin    = LMCS_PIN;
	gpioinitstruct.Mode   = GPIO_MODE_OUTPUT_PP;
	gpioinitstruct.Pull   = GPIO_NOPULL;
	gpioinitstruct.Speed  = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(LMCS_GPIO_PORT , &gpioinitstruct);

	HAL_GPIO_WritePin(LMCS_GPIO_PORT,LMCS_PIN , GPIO_PIN_SET);	
	
}

//控制信号调理电路的IO设置
void Beep_Init(void)
{
	GPIO_InitTypeDef  gpioinitstruct;


	/* Reset PIN to switch off the LED */
	HAL_GPIO_WritePin(BEEP_GPIO_PORT,BEEP_PIN , GPIO_PIN_SET);
	/* Enable the GPIO_LED Clock */
	BEEP_CLK_ENABLE;

	/* Configure the GPIO_LED pin */
	gpioinitstruct.Pin    = BEEP_PIN;
	gpioinitstruct.Mode   = GPIO_MODE_OUTPUT_PP;
	gpioinitstruct.Pull   = GPIO_NOPULL;
	gpioinitstruct.Speed  = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(BEEP_GPIO_PORT , &gpioinitstruct);

	BEEP_OFF();//关闭BEEP	
	
}

