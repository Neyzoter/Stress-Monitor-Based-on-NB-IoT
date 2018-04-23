#ifndef __IWDG_H_
#define __IWDG_H_

#include "sys.h"
extern IWDG_HandleTypeDef IWDG_Handler;

void IWDG_Init(u8 prer,u16 rld);
void IWDG_Feed(void);

#endif

