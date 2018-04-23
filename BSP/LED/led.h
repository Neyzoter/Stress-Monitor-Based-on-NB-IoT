/*
 * Stress Monitor Based on NB-IoT
 * 项目名称：基于NB-IoT的应力监测系统
 * 作者：宋超超
 *
 * 该代码版权归宋超超所有，请勿用于商业用途。
 * https://neyzoter.github.io/
 */

#ifndef _LED_H
#define _LED_H
#include "sys.h"

#define LED2_GPIO_PORT    GPIOD
#define LED2_PIN          GPIO_PIN_2
#define LED2_CLK_ENABLE   __HAL_RCC_GPIOD_CLK_ENABLE() 
#define LED2_CLK_DISABLE   __HAL_RCC_GPIOD_CLK_DISABLE() 
#define LED2_ON()		  HAL_GPIO_WritePin(LED2_GPIO_PORT,LED2_PIN , GPIO_PIN_RESET);
#define LED2_OFF()		  HAL_GPIO_WritePin(LED2_GPIO_PORT,LED2_PIN , GPIO_PIN_SET);	
#define LED2_Tog()		  HAL_GPIO_TogglePin(LED2_GPIO_PORT, LED2_PIN);

#define LED1_GPIO_PORT    GPIOC
#define LED1_PIN          GPIO_PIN_10
#define LED1_CLK_ENABLE   __HAL_RCC_GPIOC_CLK_ENABLE() 
#define LED1_CLK_DISABLE   __HAL_RCC_GPIOC_CLK_DISABLE() 
#define LED1_ON()		  HAL_GPIO_WritePin(LED1_GPIO_PORT,LED1_PIN , GPIO_PIN_RESET);
#define LED1_OFF()		  HAL_GPIO_WritePin(LED1_GPIO_PORT,LED1_PIN , GPIO_PIN_SET);
#define LED1_Tog()		  HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);

void BSP_LED_Init(void);
	
#endif



