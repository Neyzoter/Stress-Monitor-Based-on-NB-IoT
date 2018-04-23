#ifndef __MYTASK_H
#define __MYTASK_H

#include "FreeRTOS.h"
#include "task.h"

#define START_TASK_PRIO		1	//�������ȼ�
#define START_STK_SIZE 		100  //�����ջ��С	
extern TaskHandle_t StartTask_Handler;		//������
void start_task(void *pvParameters);		//������

#define ATTACH_NETWORK_TASK_PRIO		2
#define ATTACH_NETWORK_STK_SIZE 		50  
extern TaskHandle_t AttachNetworkTask_Handler;
void attach_network_task(void *pvParameters);

#define INDICATOR_DATA_TASK_PRIO		3	
#define INDICATOR_STK_SIZE 		30  
extern TaskHandle_t IndicatorTask_Handler;
void indicator_task(void *pvParameters);

#define GET_DATA_TASK_PRIO		4	
#define GET_DATA_STK_SIZE 		50  
extern TaskHandle_t GetDataTask_Handler;
void getdata_task(void *pvParameters);

void SysInit(void);//ϵͳ��ʼ��


#endif
