#ifndef __MAIN_H
#define __MAIN_H

#include "stm32l1xx_hal.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "I2C_OLED.h"
#include "common.h"
#include "BC95.h"
#include "key.h"
#include "led.h"
#include "adc.h"
#include "algorithm.h"
#include "io.h"
#include "lowpower.h"
#include "iwdg.h"
#include "wwdg.h"


//强制把__NVIC_PRIO_BITS定义为4，而不是4U
#if 1
#ifdef __NVIC_PRIO_BITS
#undef __NVIC_PRIO_BITS
#define __NVIC_PRIO_BITS      4
#endif
#endif


#endif /* __MAIN_H */
