#ifndef __WWDG_H_
#define __WWDG_H_

#include "sys.h"

extern WWDG_HandleTypeDef hwwdg;

void WWDG_Init(u8 win,u8 cnt , u32 psc);

#endif
