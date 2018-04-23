/*
 * Stress Monitor Based on NB-IoT
 * 项目名称：基于NB-IoT的应力监测系统
 * 作者：宋超超
 *
 * 该代码版权归宋超超所有，请勿用于商业用途。
 * https://neyzoter.github.io/
 */

#ifndef _KEY_H
#define _KEY_H
#include "sys.h"
#include "stm32l1xx_hal_conf.h"


#define KEY0_GPIO            GPIOC
#define KEY0_GPIO_PIN        GPIO_PIN_13



#define KEY0_PRESSED	     1
#define KEY0			     HAL_GPIO_ReadPin(KEY0_GPIO, KEY0_GPIO_PIN)



u8 KEY_Scan(u8 mode);
void key_test(void);
void KEY_Init(void);

#endif

