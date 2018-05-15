/*
 * Stress Monitor Based on NB-IoT
 * 项目名称：基于NB-IoT的应力监测系统
 * 作者：宋超超
 *
 * 该代码版权归宋超超所有，请勿用于商业用途。
 * https://neyzoter.github.io/
 */


#include "lowpower.h"
#include "led.h"
#include "i2c_oled.h"
#include "io.h"
#include "usart.h"

RTC_HandleTypeDef hrtc;


//*******************************************************
//函数名称：RTC_Init
//输入参数：唤醒中断的时长
//函数作用：RTC初始化
//返回参数：无
//备注：RTC唤醒，不可用wkup唤醒
//*******************************************************
void RTC_Init(u16 WKUP_Seconds)
{	
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;

	/**Initialize RTC Only 
	*/
	hrtc.Instance = RTC;
	hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
	hrtc.Init.AsynchPrediv = 127;
	hrtc.Init.SynchPrediv = 255;
	hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
	hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	if (HAL_RTC_Init(&hrtc) != HAL_OK)
	{
		Error_Handler(__FILE__, __LINE__);
	}

	/**Initialize RTC and set the Time and Date 
	*/
	if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0) != 0x32F2){
	sTime.Hours = 0x0;
	sTime.Minutes = 0x0;
	sTime.Seconds = 0x0;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
	{
		Error_Handler(__FILE__, __LINE__);
	}

	sDate.WeekDay = RTC_WEEKDAY_MONDAY;
	sDate.Month = RTC_MONTH_JANUARY;
	sDate.Date = 0x1;
	sDate.Year = 0x0;

	if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
	{
		Error_Handler(__FILE__, __LINE__);
	}

	HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR0,0x32F2);
	}
	/**Enable the WakeUp 
	*/
	//注意！！！这里我已经修改了HAL_RTCEx_SetWakeUpTimer_IT内部语句，把RTC的WKUP定时器关掉了
	if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, WKUP_Seconds, RTC_WAKEUPCLOCK_CK_SPRE_16BITS) != HAL_OK)
	{
		Error_Handler(__FILE__, __LINE__);
	}
	
}
//*******************************************************
//函数名称：Sys_Entry_Standby
//输入参数：
//函数作用：软件实现进入待机模式
//返回参数：无
//备注：RTC唤醒，不可用wkup唤醒
//*******************************************************
void Sys_Entry_Standby(void)
{
	OLED_OFF();//OLED休眠
	//IO拉高或者拉低
	LED2_OFF();LED1_OFF();
	BEEP_OFF();USART3_TX_RESET();USART3_RX_RESET();
	
	HAL_MspDeInit();
	
	HAL_PWR_EnableBkUpAccess(); //后备区域访问使能,CDP：
	
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);//清除SBF标志位，用于待机模式的标志位

	__HAL_RTC_WRITEPROTECTION_DISABLE(&hrtc);//RTC写保护关闭
	__HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);//清除wkup定时器中断标志位
	__HAL_RTC_WAKEUPTIMER_ENABLE(&hrtc);//RTC的WKUP定时器打开	
	__HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);//清除WTUF，WKUP定时器到0时的中断标志位	

	__HAL_RTC_WRITEPROTECTION_ENABLE(&hrtc);//RTC写保护使能

	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);//清除PWR_CSR的WUF，WUKEUP标志
	
	SET_BIT(PWR->CR, PWR_CR_CWUF);//唤醒标志位清零	
	
	
	HAL_PWR_EnterSTANDBYMode();//进入待机模式	
}

void PreSleepProcessing(uint32_t ulExpectedTime)
{
	//PB有UART中断需要响应、
	LED2_CLK_DISABLE;//PD时钟
	LED1_CLK_DISABLE;//PC时钟
	LMCS_CLK_DISABLE;//PC时钟
	//ADC1在使用的时候才打开所以这里不需要关闭
}

void PostSleepProcessing(uint32_t ulExpectedTime)
{
	LED2_CLK_ENABLE;//PD
	LED1_CLK_ENABLE;//PC
	LMCS_CLK_ENABLE;//PC时钟


}

void RealWaitingTimeProcessing(uint32_t ulCompleteTickPeriods)
{
	if(usart.WAIT_START)	
		usart.waittime += ulCompleteTickPeriods;	
}
//RTC的WKUP唤醒中断
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
	if(hrtc->Instance == RTC)
	{
	}
}

