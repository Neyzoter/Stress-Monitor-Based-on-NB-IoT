/*
 * Stress Monitor Based on NB-IoT
 * 项目名称：基于NB-IoT的应力监测系统
 * 作者：宋超超
 *
 * 该代码版权归宋超超所有，请勿用于商业用途。
 * https://neyzoter.github.io/
 */


#include "adc.h"
#include "io.h"
#include "timer.h"
#include "FreeRTOS.h"
#include "task.h"

ADC_HandleTypeDef hadc;

/* ADC init function */
void ADC_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
    */
  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  hadc.Init.LowPowerAutoWait = ADC_AUTOWAIT_DISABLE;
  hadc.Init.LowPowerAutoPowerOff = ADC_AUTOPOWEROFF_DISABLE;
  hadc.Init.ChannelsBank = ADC_CHANNELS_BANK_A;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.NbrOfConversion = 1;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = DISABLE;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler(__FILE__, __LINE__);
  }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_384CYCLES;//加大采样时间
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler(__FILE__, __LINE__);
  }

}


//*******************************************************
//函数名称：GetAdcValue
//输入参数：无
//函数作用：得到ADC的数值
//返回参数：无
//*******************************************************
u16 GetAdcValue(void)
{
	u16 adc_temp = 0;
	LM_ON();//打开应力监测电路的电源
	vTaskDelay(100);//等待ms稳定下来
	__HAL_RCC_ADC1_CLK_ENABLE();
	HAL_ADC_Start_IT(&hadc);
	vTaskDelay(20);
	HAL_ADC_Stop_IT(&hadc);
	LM_OFF();//关闭应力信号调理电路的电源
	
	//这里必须先采集数据再关闭时钟，不然会读到0
	adc_temp = HAL_ADC_GetValue(&hadc);
	__HAL_RCC_ADC1_CLK_DISABLE();
	return adc_temp;

}






