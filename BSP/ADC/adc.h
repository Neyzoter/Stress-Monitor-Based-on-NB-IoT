/*
 * Stress Monitor Based on NB-IoT
 * 项目名称：基于NB-IoT的应力监测系统
 * 作者：宋超超
 *
 * 该代码版权归宋超超所有，请勿用于商业用途。
 * https://neyzoter.github.io/
 */

#ifndef _ADC_H
#define _ADC_H
#include "sys.h"

#define ZJU_ADCx                                 ADC1
#define ZJU_ADCx_CLK_ENABLE()                    __HAL_RCC_ADC1_CLK_ENABLE()
#define ZJU_ADCx_CLK_DISABLE()                   __HAL_RCC_ADC1_CLK_DISABLE()
    
#define ZJU_ADCx_GPIO_PORT                       GPIOC
#define ZJU_ADCx_GPIO_PIN                        GPIO_PIN_0
#define ZJU_ADCx_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOC_CLK_ENABLE()
#define ZJU_ADCx_GPIO_CLK_DISABLE()              __HAL_RCC_GPIOC_CLK_DISABLE()

extern ADC_HandleTypeDef hadc;

void ADC_Init(void);
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc);
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc);
u16 GetAdcValue(void);

#endif


