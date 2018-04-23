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

