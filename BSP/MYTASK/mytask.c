/*
 * Stress Monitor Based on NB-IoT
 * 项目名称：基于NB-IoT的应力监测系统
 * 作者：宋超超
 *
 * 该代码版权归宋超超所有，请勿用于商业用途。
 * https://neyzoter.github.io/
 */

#include "mytask.h"
#include "common.h"
#include "stdlib.h"

TaskHandle_t GetDataTask_Handler;
TaskHandle_t IndicatorTask_Handler;
TaskHandle_t AttachNetworkTask_Handler;
TaskHandle_t StartTask_Handler;


//用于存储数据
u16 adc_datas[DATA_NUM_NEEDED] = {0};


//数值和真实值的关系
//倍数和零点
struct ADC_PARA
{
	int getZero ;
	float getMul ;
	float beta;
}adc_para = {-1866,3.28,0.8};


////////////////////////////////////////////////////////////
//板子的状态

//第一个参数，BRD_STA
//BRD_SYS_INITING:正在初始化
//BRD_ATTACKNETWORKING：尝试连接
//BRD_LOOPING:板子正在循环运行

//第二个参数IMEI号

//第三个参数：OLED_REFRESHING，当前OLED是不是在写
//不能在两个地方同时写。比如定时器和循环不能同时更新。

MAIN_FLAG flag = {0,"863703035480214",0};


/////////////////////////////////////////////////////////////

char Adc_Str[9] = "+000000g\0";

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
	
	
    //创建getdata任务
    xTaskCreate((TaskFunction_t )getdata_task,     	
                (const char*    )"getdata_task",   	
                (uint16_t       )GET_DATA_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )GET_DATA_TASK_PRIO,	
                (TaskHandle_t*  )&GetDataTask_Handler);  				
    //创建指示灯任务
    xTaskCreate((TaskFunction_t )indicator_task,     	
                (const char*    )"indicator_task",   	
                (uint16_t       )INDICATOR_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )INDICATOR_DATA_TASK_PRIO,	
                (TaskHandle_t*  )&IndicatorTask_Handler); 				
    //创建LED1任务
    xTaskCreate((TaskFunction_t )attach_network_task,     
                (const char*    )"attach_network_task",   
                (uint16_t       )ATTACH_NETWORK_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )ATTACH_NETWORK_TASK_PRIO,
                (TaskHandle_t*  )&AttachNetworkTask_Handler);        

    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//采集数据任务函数
void getdata_task(void *pvParameters)
{
	static u8 Data_Num = 0;//数据的数量
	static float Adc_Val = 0.0;
    while(1)
    {
		if(Data_Num<DATA_NUM_NEEDED)//如果还没到数据要求个数
		{
			adc_datas[Data_Num] = GetAdcValue();	//采集数据并相加后面取平均			
//			Adc_Val = (adc_para.beta * Adc_Val + (1.0 - adc_para.beta) * (float)GetAdcValue());
			Data_Num++;
			vTaskDelay(100);//阻塞态，每5秒采集一次
		}
		else if(flag.BRD_STA == BRD_ATTACKED||HAL_RTCEx_BKUPRead(&hrtc,BACKUP_AREA)==RESET_FROM_STANDBY)//如果数据采集够了，而且已经联网了或者从待机唤醒
		{
			if(DATA_NUM_NEEDED>=3)//如果数据个数大于等于3个，简单的去掉异常值（取中值）
			{
				u16sortup(adc_datas,DATA_NUM_NEEDED);//升序排序
				Adc_Val = adc_datas[DATA_NUM_NEEDED/2];//去中间的数据，比如5个，取下标为2的数值
			}
			else //如果是只有1个或者2个数据，取第1个数据
			{
				Adc_Val = adc_datas[0];
			}
//			Adc_Val/=(1.0 - mypow_float(adc_para.beta,Data_Num+1));
			Adc_Val = adc_para.getMul * (Adc_Val + adc_para.getZero);//得到真实的数据			
			Adc_Val = (Adc_Val>=0)?Adc_Val:0;//防止出现小于零
			Num2Str(Adc_Str,Adc_Val);	
			flag.BRD_STA = BRD_ATTACKED;//从待机唤醒的话，直接给他赋值
//			if(!intMSG_Send((u16)Adc_Val,2))//发送int型，这里平台也要配置成int型
			if(!MSG_Send(Adc_Str,"8"))//发送5次失败，那么显示error
			{
				MsgSentDisp("ERROR");//这里需要球盖
				HAL_PWR_EnableBkUpAccess(); //后备区域访问使能,CDP：
				HAL_RTCEx_BKUPWrite(&hrtc, BACKUP_AREA,NEED_REATTACH_NETWORK);//下次需要重新联网
				HAL_PWR_DisableBkUpAccess(); //后备区域访问除能,CDP：
			}
			else
			{
				HAL_PWR_EnableBkUpAccess(); //后备区域访问使能,CDP：
				HAL_RTCEx_BKUPWrite(&hrtc, BACKUP_AREA,RESET_FROM_STANDBY);//下次任务待机模式启动，不需要重新联网
				HAL_PWR_DisableBkUpAccess(); //后备区域访问除能,CDP：
			}
			MsgSentDisp(" Entering Standby...");
			vTaskDelay(2000);
			Sys_Entry_Standby();//进入待机模式			
		}
		else//如果已经到了次数，但是还没有联网那么等待两秒
		{
			vTaskDelay(2000);//阻塞态，每5秒采集一次
		}
		
    }
}

 void indicator_task(void *pvParameters)
 {
	while(1)
	{
//		if(flag.BRD_STA == BRD_SYS_INITING)//板子在初始化
//		{	LED1_ON();	LED2_OFF();	}
//		else if(flag.BRD_STA == BRD_ATTACKNETWORKING)//板子正在连接网络
//		{	LED2_ON();	LED1_OFF();	}
//		else if(flag.BRD_STA == BRD_ATTACKED)//网络已经连接
//		{	LED2_ON();	LED1_ON();	}
//		vTaskDelay(100);
//		if(flag.BRD_STA == BRD_SYS_INITING)//板子在初始化
//		{	LED1_OFF();	LED2_OFF();	}
//		else if(flag.BRD_STA == BRD_ATTACKNETWORKING)
//		{	LED2_OFF();	LED1_OFF();	}		
//		vTaskDelay(500);
		if(flag.BRD_STA == BRD_SYS_INITING)//板子在初始化
		{	LED1_ON();	LED2_OFF();	}
		else if(flag.BRD_STA == BRD_ATTACKNETWORKING)//板子正在连接网络
		{	LED2_ON();	LED1_OFF();	}
		else if(flag.BRD_STA == BRD_ATTACKED)//网络已经连接
		{	LED2_ON();	LED1_ON();	}	
		vTaskDelay(1000);
	}
 }

//联网任务函数
void attach_network_task(void *pvParameters)
{
	while(1)
	{
		if(!HAL_RTCEx_BKUPRead(&hrtc,BACKUP_AREA)&&flag.BRD_STA != BRD_ATTACKED)//如果是第一次开机，需要联网
		{
			while(AttachNetwork()==0)//如果连接不成功，系统重新初始化
			{
				vTaskDelay(2000);
				SysInit();
			}
		}
		vTaskDelete(NULL);//删除任务
		vTaskDelay(4000);
	}
}

//**************************************************************************************
//函数名称：void SysInit(void)
//输入参数：无
//函数作用：初始化程序，包括延时初始化、中断初始化、IO、LED及报警、OLED、定时器、启动BC95
//返回参数：无
//****************************************************************************************
void SysInit(void)
{
	flag.BRD_STA = BRD_SYS_INITING;//板子正在初始化
	HAL_Init();
	SystemClock_Config();
	I2C_OLED_Init();//初始化IIC
	OLED_Init();//初始化OLED
	
	OLED_CLS();//清屏	
	OLED_ShowStr(0,0,"FL",1);	//offline
	OLED_ShowStr(0,1,"CMD:",1);	
	OLED_ShowStr(0,2,"Rcv:",1);
	
	OLED_ShowStr(CMD_START_X,CMD_START_Y,"Sys Init...",1);
	HAL_Delay(1000);
	
//	//在需要启动中断的时候，再启动中断
//	TIM2_Init(99,2096);//100*2097/2097000  =  100ms
	uart_init(9600);              //初始化USART
	BSP_LED_Init();//PD2C10
	ADC_Init();
	
	//iwdg在开启后无法在关闭，所以如果进入待机时间长的话，那么就不要开启iwdg了
	//除非在进入standby之前，复位一下	
	//如果要开IWDG，那么要开一下LSI，不然会让程序卡死
//	IWDG_Init(IWDG_PRESCALER_32,4095);//大约4s钟，没有喂狗则要重启

	WWDG_Init(65,127, WWDG_PRESCALER_8);//大约1s中进入一次窗口看门狗中断
	
//	HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);
//	KEY_Init();	//按键初始化
//	BSP_JOY_Init();
	
	OLED_ShowStr(CMD_START_X,CMD_START_Y,"Sys Inited   ",1);
	
	HAL_Delay(1000);
		
	IO_Init();
	RTC_Init(60);//60秒开启一次
}

