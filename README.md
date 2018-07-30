# 项目介绍

基于NB-IoT窄带物联网，采集压力数据，并发送到电信物理网平台。代码基于FreeRTOS实时操作系统。

联网后周期性的发送数据，并进入待机模式。于此同时开启了Tickless，会在空闲时进入睡眠模式。

开源硬件地址：[https://github.com/Neyzoter/Stress-Monitor-Based-on-NB-IoT/tree/master/OpensourcePCB/STM32L152](https://github.com/Neyzoter/Stress-Monitor-Based-on-NB-IoT/tree/master/OpensourcePCB/STM32L152 "OpensourcePCB")

版本：2018-5-15

# 版本修改说明

2018-5-15：

1、修改FreeRTOS的Tickless退出函数源码

具体见

https://neyzoter.github.io/2018/05/15/FreeRTOS-Note11-Add-Complete-Time-Function/

2、加入AT等待时间的显示


# 运营伙伴

NB运营商：电信

物联网平台：电信

>如果想要改成其他的运营商和物联网平台，只需要修改，nband和CDP地址即可。

# 资源描述

* 1、USART3

STML1：PB10（TX）和PB11（RX）。

BC95的RX有一个下拉，TX直接通过50R连接。

所以设置PB10和PB11为下拉，进入待机模式时，设置为低电平。

注：如果要改USARTx，需要改的内容：GPIOx、GPIO_PINx、句柄（及其设置）、GPIO时钟、中断名USARTx_IRQHandler等。

* 2、OLED

PB6（SCL）、PB7（SDA），这两个用于软件模拟IIC。

* 3、LED

PD2、PC10。

* 4、KEY1、KEY2、KEY3

PC13、PC12、PC11。

* 5、时钟

系统时钟：MSI  -》   2.097MHz（可选择）

ADC时钟：HSI  -》    16Mhz

LSE：32.768kHz

* 6、RTC

采用RTC经两个分频器分频得到的频率作为源，即一般配置为1Hz。设置自动重装载寄存器的数值可以设定u16以内秒数的时间中断（达到18Hours以上），如果采用17bits可以达到36Hours以上。

* 7、信号调理电路电源控制

LM_CS：PC1

* 8、ADC1

PC0引脚。

必须先调用GetAdcValue（包括开启信号调理电路电源、开始ADC转换、关闭ADC转换、关闭信号调理电路电源等过程）。

* 9、NVIC

移植FreeRTOS的时候，中断优先级分组为4bits：0bits。FreeRTOS不处理亚优先级。

注：在定时器中断中更新OLED时，为了防止出现和非中断更新OLED冲突，需要设置标志位（1：正在更新；0：未更新）。

* 10、Tickless

Tickless进入后，需要关闭某一些低功耗时候不需要的时钟，比如LED的引脚的时钟。

在本程序中，ADC的时钟在读取的时候才打开，所以不需要再Tickless进入前的PreSleepProcessing函数中处理。而且读取数据前不能关闭时钟。

* 11、IWDG

放在SysTick_Handler中，IWDG可以实现唤醒Standby，如果在待机模式中死机，也可以实现唤醒。但是由于时间很短，所以如果想长时间待机，就没有开启。本代码没有开启独立看门狗。

* 12、WWDG

本代码开启了窗口看门狗。



# 其他

如果本代码和PCB有什么问题，联系我。

Email: sonechaochao@gmail.com