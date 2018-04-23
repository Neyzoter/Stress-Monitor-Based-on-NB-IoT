/*
 * Stress Monitor Based on NB-IoT
 * 项目名称：基于NB-IoT的应力监测系统
 * 作者：宋超超
 *
 * 该代码版权归宋超超所有，请勿用于商业用途。
 * https://neyzoter.github.io/
 */

#include "wwdg.h"
#include "led.h"

WWDG_HandleTypeDef hwwdg;


//WWDG的初始化
void WWDG_Init(u8 win,u8 cnt , u32 psc)
{

	hwwdg.Instance = WWDG;
	hwwdg.Init.Prescaler = psc;
	hwwdg.Init.Window = win;
	hwwdg.Init.Counter = cnt;
	hwwdg.Init.EWIMode = WWDG_EWI_ENABLE;
	if (HAL_WWDG_Init(&hwwdg) != HAL_OK)
	{
	Error_Handler(__FILE__, __LINE__);
	}

}
	
//wwdg的递减计数器重新赋值
void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef* hwwdg)
{
	HAL_WWDG_Refresh(hwwdg);	
}
