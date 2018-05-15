
#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
    #include <stdint.h>
    #include "main.h" 
    extern uint32_t SystemCoreClock;
#endif


//FreeRTOS的低功耗配置
//------------------------------------------------------------------------------------------//
#define configUSE_TICKLESS_IDLE					1   /*<启用低功耗tickless模式*/

extern void PreSleepProcessing(uint32_t ulExpectedTime);
extern void PostSleepProcessing(uint32_t ulExpectedTime);
extern void RealWaitingTimeProcessing(uint32_t ulCompleteTickPeriods);
////进入低功耗模式前要做的处理
#define configPRE_SLEEP_PROCESSING	PreSleepProcessing
///退出低功耗模式后要做的处理
#define configPOST_SLEEP_PROCESSING		PostSleepProcessing
///退出低功耗模式后得到在tickless中实际等待的时间，是否需要使用
//应用场景：用户开了使用滴答时钟中断中放了一个计数变量，用于计数查看是否超时
//那么如果开了Tickless就会出现进入低功耗后时间不能加上的问题
//但是在PostSleepProcessing函数中只知道预期的时间是多少，而不是到真实事件
//所以在vPortSuppressTicksAndSleep的PostSleepProcessing中处理完开启时钟等操作后，后期会得到
//ulCompleteTickPeriods：实际等待的时间
#define configREAL_WAITINGTIME_PROCESSING		RealWaitingTimeProcessing

//------------------------------------------------------------------------------------------//


#define configAPPLICATION_ALLOCATED_HEAP         0/*<堆内存由编译器自动分配>，如果不定义，默认为0*/
#define configENABLE_BACKWARD_COMPATIBILITY      1 /*<后向兼容使能>，默认为1*/


#define configUSE_PREEMPTION                     1		/*<采用抢占式调度器还是协程>，1：抢占式调度器；0：协程*/
#define configSUPPORT_STATIC_ALLOCATION          0
#define configSUPPORT_DYNAMIC_ALLOCATION         1  /*<支持动态分配>，自动从FreeRTOS的堆中分配。如果不定义，默认为1.*/
#define configUSE_IDLE_HOOK                      0		/*<使用空闲任务钩子函数>，1：使用；0：不使用*/
#define configUSE_TICK_HOOK                      0		/*<使能时间片钩子函数>，1：使能；0：失能*/
#define configCPU_CLOCK_HZ                       ( SystemCoreClock )   /*<设置CPU的功率>*/
#define configTICK_RATE_HZ                       ((TickType_t)1000)    /*<时钟节拍频率>，即滴答定时器的中断频率*/


/*configUSE_PORT_OPTIMISED_TASK_SELECTION设置为1，宏configMAX_PRIORITIES最大不超过32*/
#define configMAX_PRIORITIES                     ( 7 )	/*<给任务的最大优先级数目>，优先级从0到（该参数-1）,0是最低优先级。*/

#define configUSE_TIME_SLICING					 1   /*<支持时间切片>，默认为1*/
#define configMINIMAL_STACK_SIZE                 ((uint16_t)128)/*<给空闲任务最小的堆栈大小>，该单位为字（STM32是32位，即4个字节），即数值是100，则共有400个字节*/
#define configTOTAL_HEAP_SIZE                    ((size_t)3072)    /*<总共的堆栈大小>，*/
#define configMAX_TASK_NAME_LEN                  ( 16 )	/*<最大的任务名称长度>*/
#define configUSE_16_BIT_TICKS                   0    /*<设置系统节拍计数器变量数据类型>,0:32bits;1:16bits*/
#define configUSE_MUTEXES                        1		/*<使用互斥信号量>，1:使用；0：关闭*/
#define configQUEUE_REGISTRY_SIZE                8	/*<最大可注册的队列和信号量的数量>*/
#define configUSE_PORT_OPTIMISED_TASK_SELECTION  1	/*<下一个要运行的任务的两种方法（1：硬件方法，STM32包含异零指令CLZ；2：通用方法）>*/

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES                    0	/*<启动协程>，1：启动；0：关闭*/
#define configMAX_CO_ROUTINE_PRIORITIES          ( 2 )  /*<协程的最大优先级数目>，则优先级从0到（该参数-1），最低优先级是0*/

/* INCLUDE开始的宏，用于API的使能和失能 */
#define INCLUDE_vTaskPrioritySet            1
#define INCLUDE_uxTaskPriorityGet           1
#define INCLUDE_vTaskDelete                 1
#define INCLUDE_vTaskCleanUpResources       0
#define INCLUDE_vTaskSuspend                1
#define INCLUDE_vTaskDelayUntil             0
#define INCLUDE_vTaskDelay                  1
#define INCLUDE_xTaskGetSchedulerState      1


/*<MCU使用几位优先级>，STM32采用4bits*/
#ifdef __NVIC_PRIO_BITS
 #define configPRIO_BITS         __NVIC_PRIO_BITS
#else
 #define configPRIO_BITS         4
#endif

/*<设置最低优先级>，STM32中断优先级配置组4，抢占优先级从0到15（无亚优先级），所以设置为15*/
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY   15


/*<设置FreeRTOS系统可管理的最大优先级>,低于该值的优先级归FreeRTOS管理；高于该值的优先级不归FreeRTOS管理*/
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5

/*<配置内核中断优先级>,将configLIBRARY_LOWEST_INTERRUPT_PRIORITY（最低优先级）写到STM32优先级寄存器的高4位*/
/*不同的MCU采用不同的优先级位数，最多采用8bits*/
#define configKERNEL_INTERRUPT_PRIORITY 		( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/*<设置BASEPRI，小于等于该中断优先级数值（数值越高，中断优先级越低）的中断受FreeRTOS管理>，低于该优先级的中断可以安全的调用FreeRTOS的API函数*/
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )


/* USER CODE BEGIN 1 */ 
//方式1：CubeMX生成的代码
#define configASSERT( x ) if ((x) == 0) {taskDISABLE_INTERRUPTS(); for( ;; );} 

/*方式2：把错误的地点打印到打印到串口*/
//#define vAssertCalled(char,int) printf("Error:%s,%d\r\n",char ,int );
//#define configASSERT( x ) if ((x) == 0) vAssertCalled(__FILE__,__LINE__) ;
/* USER CODE END 1 */

/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
standard names. */
#define vPortSVCHandler    SVC_Handler
#define xPortPendSVHandler PendSV_Handler

/* IMPORTANT: This define MUST be commented when used with STM32Cube firmware, 
              to prevent overwriting SysTick_Handler defined within STM32Cube HAL */
/* #define xPortSysTickHandler SysTick_Handler */

/* USER CODE BEGIN Defines */   	      
/* Section where parameter definitions can be added (for instance, to override default ones in FreeRTOS.h) */
/* USER CODE END Defines */ 

#endif /* FREERTOS_CONFIG_H */
