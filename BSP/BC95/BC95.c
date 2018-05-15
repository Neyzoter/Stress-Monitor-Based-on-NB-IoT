/*
 * Stress Monitor Based on NB-IoT
 * 项目名称：基于NB-IoT的应力监测系统
 * 作者：宋超超
 *
 * 该代码版权归宋超超所有，请勿用于商业用途。
 * https://neyzoter.github.io/
 */

#include "BC95.h"
#include "I2C_OLED.h"
#include "usart.h"
#include "string.h"
#include "algorithm.h"
#include "timer.h"
#include "common.h"
#include "FreeRTOS.h"
#include "task.h"

////////////////////////////////////////////////////////////
//下行数据和其状态
//如果数据的status是错误的，那么存储的信息有可能是上次的，认为没有作用
//下行命令的状态
//CMD_GOT_NUM_0 1//没有数据
//CMD_GOT_TOO_LONG 2//没有数据
//CMD_GOT_FORMAT_ERROR 3 //数据格式错误
//CMD_GOT_OK 4 //数据接收成功
CMD_GOT cmdgot = {0,0,0,0};



///////////////////////////////////////////////////////////
//**************************************************************************************
//函数名称：void AttachNetwork(void)
//输入参数：无
//函数作用：网络连接设置，包括多个步骤
//返回参数：无
//****************************************************************************************
u8 AttachNetwork(void)
{
	u8 try_num = API_OPEN_REPEAT_NUMBER;
	flag.BRD_STA = BRD_ATTACKNETWORKING;//板子正在尝试连接
	while(try_num) 
	{
		if(AT_NRB()==0)//如果重启不成功
			goto ATTACKERROR;
		if(AT_Test()==0)//如果剩余次数已经到达0,
			goto ATTACKERROR;
		if(CDP_Get_Cfg(CDP_SERVER_IP_PORT)==0)
			goto ATTACKERROR;
		if(CFUN_Get_Set()==0)
			goto ATTACKERROR;
		if(NBAND_Get_Set(TELECHINA_NBAND)==0)
			goto ATTACKERROR;
		if(CGATT_Get_Set()==0)
			goto ATTACKERROR;
		flag.BRD_STA = BRD_ATTACKED;//成功连接上了
		BrdStaDisp();//更新板子的状态
		break;
		ATTACKERROR:
		try_num--;
	}
	return try_num;
}
//*********************************************************************************
//函数名称：StartWaitMsg
//输入参数：
//函数作用：开始等待通信信号.包括等待开始标志位打开，等待时间变量清零，数据位数清零
//返回参数：
//*********************************************************************************
void StartWaitMsg(void)
{
	taskENTER_CRITICAL();
	usart.WAIT_START = 1;//开始等待
	usart.waittime = 0;
	usart.num = 0;
	taskEXIT_CRITICAL();
}
//*******************************************************
//函数名称：StopWaitMsg
//输入参数：
//函数作用：停止等待通信信号.包括等待开始标志位打开，等待时间变量清零，数据位数清零
//返回参数：
//*******************************************************
void StopWaitMsg(void)
{
	taskENTER_CRITICAL();
	usart.WAIT_START = 0;//停止等待
	usart.waittime = 0;
	
	taskEXIT_CRITICAL();
}
//*******************************************************
//函数名称：AT_NRB
//输入参数：
//函数作用：BC95重启reboot
//返回参数：1
//*******************************************************
u8 AT_NRB(void)
{
	u8 try_num = AT_NRB_REPEAT_NUMBER;//重启次数
	char cmd[10] = "AT+NRB\r\n";
	while(try_num)
	{
		//初始化界面
		InitDisp();
		cmdDisp("REBOOT...");	
		//开始时间等待
		StartWaitMsg();
		//发送数据		
		printf(cmd);
		//等待超过3s则说明发生错误
		while(usart.waittime <= AT_NRB_WAIT_TIME)//超过5秒
		{
			if(usart.num>50)
			{
				//清除等待时间
				OLED_ShowStr(WAITINGTIME_X,WAITINGTIME_Y,"    ",1);
				//关闭等待
				StopWaitMsg();//停止等待信息
				MsgDisp(LINE_FEED);
				//复位信号比较多，等待时间长一点
				//这个要在前面，因为不知道NRB收到什么东西
				//需要等待全部传输完毕，才能关闭等待
				//其实好像也不需要，因为每次接收前都要初始化num为0
				vTaskDelay(2*NEXT_AT_WAIT_TIME);

				return try_num;//返回
			}
			OLED_ShowNum(WAITINGTIME_X, WAITINGTIME_Y, usart.waittime, WAITINGTIME_DISP_DIGITS,1);
			#ifdef AT_TICKLESS_ON 
				vTaskDelay(AT_CHECK_EVERYTIME);//每200ms进入一次查看是否有数据
			#endif
			
		}
		sgnDisp("- OVERTIME -");
		vTaskDelay(NEXT_AT_WAIT_TIME);
		try_num--;
	}
	//关闭等待
	StopWaitMsg();
	return try_num;//剩余尝试次数，如果为0.则表示AT指令测试失败
}

//*******************************************************
//函数名称：AT_TEST
//输入参数：
//函数作用：测试AT通信是否正常
//返回参数：成功（>=1,TRUE）或者失败（0,FALSE）
//*******************************************************
u8 AT_Test(void)
{
	u8 try_num = AT_AT_REPEAT_NUMBER;//AT发送后，返回ERROR，尝试的次数
	char cmd[6] = "AT\r\n";
	while(try_num)
	{
		//初始化界面
		InitDisp();
		cmdDisp("AT TEST...");
		
		//开始时间等待
		StartWaitMsg();
		//发送数据		
//		printf("AT\r\n");
		printf(cmd);
		//等待超过3s则说明发生错误
		while(usart.waittime <= AT_DEFAULT_WAIT_TIME)//超过三秒
		{
			if(usart.num!=0&&StrCmp("\r\nOK\r\n",usart.USART_RX_BUF))
			{
				//清除等待时间
				OLED_ShowStr(WAITINGTIME_X,WAITINGTIME_Y,"    ",1);
				//关闭等待
				StopWaitMsg();//停止等待信息
				MsgDisp(LINE_FEED);
				vTaskDelay(NEXT_AT_WAIT_TIME);

				return try_num;//返回
			}
			OLED_ShowNum(WAITINGTIME_X, WAITINGTIME_Y, usart.waittime, WAITINGTIME_DISP_DIGITS,1);
			#ifdef AT_TICKLESS_ON 
				vTaskDelay(AT_CHECK_EVERYTIME);//每200ms进入一次查看是否有数据
			#endif
			
		}
		sgnDisp("- OVERTIME -");
		vTaskDelay(NEXT_AT_WAIT_TIME);
		try_num--;
	}
	//关闭等待
	StopWaitMsg();
	return try_num;//剩余尝试次数，如果为0.则表示AT指令测试失败
}

//*******************************************************
//函数名称：CFUN_Get_Cfg
//输入参数：
//函数作用：配置CDP
//返回参数：成功（>=1,TRUE）或者失败（0,FALSE）
//*******************************************************
u8 CDP_Get_Cfg(char * ip_port)
{
	u8 try_num = AT_NCDP_REPEAT_NUMBER;//AT发送后，返回ERROR，尝试的次数
	//下面给字符串赋值时，一定要给数组的大小
	char cmd[15] = "AT+NCDP?\r\n";
	
	char rcv[40] = "\r\n+NCDP:";
	strcat(rcv,ip_port);
	strcat(rcv,"\r\n\r\nOK\r\n");

	while(try_num)
	{	
		//初始化界面
		InitDisp();
		cmdDisp("CFUN GetSet...");
		//开始时间等待
		StartWaitMsg();
		//发送数据		
//		printf("AT+NCDP=180.101.147.115,5683\r\n");
		printf(cmd);
		//等待超过3s则说明发生错误
		while(usart.waittime <= AT_DEFAULT_WAIT_TIME)//超过三秒
		{
			if(usart.num!=0&&StrCmp(rcv,usart.USART_RX_BUF))
//			if(usart.num!=0&&StrCmp("\r\n+NCDP:180.101.147.115,5683\r\n\r\nOK\r\n",usart.USART_RX_BUF))
			{
				//清除等待时间
				OLED_ShowStr(WAITINGTIME_X,WAITINGTIME_Y,"    ",1);
				//关闭等待
				StopWaitMsg();//停止等待信息
				MsgDisp(LINE_FEED);
				//下个AT指令前等待一段时间
				vTaskDelay(NEXT_AT_WAIT_TIME);

				return try_num;//返回
			}
			//显示等待时间
			OLED_ShowNum(WAITINGTIME_X, WAITINGTIME_Y, usart.waittime, WAITINGTIME_DISP_DIGITS,1);
			#ifdef AT_TICKLESS_ON 
				vTaskDelay(AT_CHECK_EVERYTIME);//每200ms进入一次查看是否有数据
			#endif
			
		}
		sgnDisp("- OVERTIME -");
		vTaskDelay(NEXT_AT_WAIT_TIME);
		CDP_Cfg(ip_port);
		try_num--;
	}
	//关闭等待
	StopWaitMsg();
	return try_num;//剩余尝试次数，如果为0.则表示AT指令测试失败
}
//*******************************************************
//函数名称：AT_TEST
//输入参数：CDP地址
//函数作用：CDP地址设置
//返回参数：成功（>=1,TRUE）或者失败（0,FALSE）
//*******************************************************
u8 CDP_Cfg(char * ip_port)
{
	u8 try_num = AT_NCDP_REPEAT_NUMBER;//AT发送后，返回ERROR，尝试的次数
	//下面给字符串赋值时，一定要给数组的大小
	char cmd[64] = "AT+NCDP=";
	strcat(cmd,ip_port);
	strcat(cmd,"\r\n");
	
	while(try_num)
	{	
		//初始化界面
		InitDisp();
		cmdDisp("CDP Cfg...");
		//开始时间等待
		StartWaitMsg();
		//发送数据		
//		printf("AT+NCDP=180.101.147.115,5683\r\n");
		printf(cmd);
		//等待超过3s则说明发生错误
		while(usart.waittime <= AT_DEFAULT_WAIT_TIME)//超过三秒
		{
			if(usart.num!=0&&StrCmp("\r\nOK\r\n",usart.USART_RX_BUF))
			{
				//清除等待时间
				OLED_ShowStr(WAITINGTIME_X,WAITINGTIME_Y,"    ",1);
				//关闭等待
				StopWaitMsg();//停止等待信息
				MsgDisp(LINE_FEED);
				//下个AT指令前等待一段时间
				vTaskDelay(NEXT_AT_WAIT_TIME);

				return try_num;//返回
//				return 1;
			}
			OLED_ShowNum(WAITINGTIME_X, WAITINGTIME_Y, usart.waittime, WAITINGTIME_DISP_DIGITS,1);
			#ifdef AT_TICKLESS_ON 
				vTaskDelay(AT_CHECK_EVERYTIME);//每200ms进入一次查看是否有数据
			#endif
			
		}
		sgnDisp("- OVERTIME -");
		vTaskDelay(NEXT_AT_WAIT_TIME);
		try_num--;
	}
	//关闭等待
	StopWaitMsg();
	return try_num;//剩余尝试次数，如果为0.则表示AT指令测试失败
}
//*******************************************************
//函数名称：CFUN_Get_Set
//输入参数：
//函数作用：设置cfun功能
//返回参数：成功（>=1,TRUE）或者失败（0,FALSE）
//*******************************************************
u8 CFUN_Get_Set(void)
{
	u8 try_num = AT_CFUN_REPEAT_NUMBER;//AT发送后，返回ERROR，尝试的次数
//	u16 time_temp=0;
	//下面给字符串赋值时，一定要给数组的大小
	char cmd[15] = "AT+CFUN?\r\n";
	
	while(try_num)
	{	
		//初始化界面
		InitDisp();
		cmdDisp("CFUN GetSet...");
		//开始时间等待
		StartWaitMsg();
		//发送数据		
		printf(cmd);
		//等待超过3s则说明发生错误
		while(usart.waittime <= AT_DEFAULT_WAIT_TIME)//超过三秒
		{
			if(usart.num!=0&&StrCmp("\r\n+CFUN:1\r\n\r\nOK\r\n",usart.USART_RX_BUF))
			{
				//清除等待时间
				OLED_ShowStr(WAITINGTIME_X,WAITINGTIME_Y,"    ",1);
				//关闭等待
				StopWaitMsg();//停止等待信息
				MsgDisp(LINE_FEED);
				//下个AT指令前等待一段时间
				vTaskDelay(NEXT_AT_WAIT_TIME);

				return try_num;//返回
			}
			OLED_ShowNum(WAITINGTIME_X, WAITINGTIME_Y, usart.waittime, WAITINGTIME_DISP_DIGITS,1);
			#ifdef AT_TICKLESS_ON 
				vTaskDelay(AT_CHECK_EVERYTIME);//每200ms进入一次查看是否有数据
			#endif
			
		}
		sgnDisp("- OVERTIME -");
		vTaskDelay(NEXT_AT_WAIT_TIME);
		CFUN_Set();
		try_num--;
	}
	//关闭等待
	StopWaitMsg();
	return try_num;//剩余尝试次数，如果为0.则表示AT指令测试失败
}

//*******************************************************
//函数名称：CFUN_Set
//输入参数：
//函数作用：CFUN的设置
//返回参数：成功（>=1,TRUE）或者失败（0,FALSE）
//*******************************************************
u8 CFUN_Set(void)
{
	u8 try_num = AT_CFUN_REPEAT_NUMBER;//AT发送后，返回ERROR，尝试的次数
	char cmd[15] = "AT+CFUN=1\r\n";
	
	while(try_num)
	{
		//初始化界面
		InitDisp();
		cmdDisp("CFUN SET...");		
		//开始时间等待
		StartWaitMsg();
		//发送数据		
		printf(cmd);
		//等待超过3s则说明发生错误
		while(usart.waittime <= AT_DEFAULT_WAIT_TIME)//超过三秒
		{
			if(usart.num!=0&&StrCmp("\r\nOK\r\n",usart.USART_RX_BUF))
			{
				//清除等待时间
				OLED_ShowStr(WAITINGTIME_X,WAITINGTIME_Y,"    ",1);
				//关闭等待
				StopWaitMsg();//停止等待信息
				MsgDisp(LINE_FEED);
				//CGATT目前不是很稳定，等待时间长一点，两个默认的AT等待长度
				vTaskDelay(2*NEXT_AT_WAIT_TIME);

				return try_num;//返回
			}
			OLED_ShowNum(WAITINGTIME_X, WAITINGTIME_Y, usart.waittime, WAITINGTIME_DISP_DIGITS,1);
			#ifdef AT_TICKLESS_ON 
				vTaskDelay(AT_CHECK_EVERYTIME);//每200ms进入一次查看是否有数据
			#endif
			
		}
		sgnDisp("- OVERTIME -");
		vTaskDelay(NEXT_AT_WAIT_TIME);
		try_num--;
	}
	//关闭等待
	StopWaitMsg();
	return try_num;//剩余尝试次数，如果为0.则表示AT指令测试失败
}
//*******************************************************
//函数名称：NBAND_Get_Set
//输入参数：电信等5,8,20
//函数作用：设置NBAND功能
//返回参数：成功（>=1,TRUE）或者失败（0,FALSE）
//*******************************************************
u8 NBAND_Get_Set(char * nband)
{
	u8 try_num = AT_NBAND_REPEAT_NUMBER;//AT发送后，返回ERROR，尝试的次数
//	u16 time_temp=0;
	//下面给字符串赋值时，一定要给数组的大小
	char cmd[20] = "AT+NBAND?\r\n";
	
	char rcv[25] = "\r\n+NBAND:";
	strcat(rcv,nband);
	strcat(rcv,"\r\n\r\nOK\r\n");
	
	while(try_num)
	{	
		//初始化界面
		InitDisp();
		cmdDisp("NBAND GetSet...");
		//开始时间等待
		StartWaitMsg();
		//发送数据		
		printf(cmd);
		vTaskDelay(NEXT_AT_WAIT_TIME);
		//等待超过3s则说明发生错误
		while(usart.waittime <= AT_DEFAULT_WAIT_TIME)//超过三秒
		{
			//固定为电信时，一般采用下面的。
//			if(usart.num!=0&&StrCmp("\r\n+NBAND:5\r\n\r\nOK\r\n",usart.USART_RX_BUF))
			//不固定平台。可通过该函数设置5、8、20
			if(usart.num!=0&&StrCmp(rcv,usart.USART_RX_BUF))
			{
				//清除等待时间
				OLED_ShowStr(WAITINGTIME_X,WAITINGTIME_Y,"    ",1);
				//关闭等待
				StopWaitMsg();//停止等待信息
				MsgDisp(LINE_FEED);
				//下个AT指令前等待一段时间
				vTaskDelay(NEXT_AT_WAIT_TIME);

				return try_num;//返回
			}
			OLED_ShowNum(WAITINGTIME_X, WAITINGTIME_Y, usart.waittime, WAITINGTIME_DISP_DIGITS,1);
			#ifdef AT_TICKLESS_ON 
				vTaskDelay(AT_CHECK_EVERYTIME);//每200ms进入一次查看是否有数据
			#endif
			
		}
		sgnDisp("- OVERTIME -");
		vTaskDelay(NEXT_AT_WAIT_TIME);
		NBAND_Set(nband);//设置NBAND=5
		try_num--;
	}
	//关闭等待
	StopWaitMsg();
	return try_num;//剩余尝试次数，如果为0.则表示AT指令测试失败
}

//*******************************************************
//函数名称：NBAND_Set
//输入参数：nband设置，电信为5，联通为8
//函数作用：nband的设置
//返回参数：成功（>=1,TRUE）或者失败（0,FALSE）
//*******************************************************
u8 NBAND_Set(char * nband)
{
	u8 try_num = AT_NBAND_REPEAT_NUMBER;//AT发送后，返回ERROR，尝试的次数
	char cmd[15] = "AT+NBAND=";
	strcat(cmd,nband);
	strcat(cmd,"\r\n");
	while(try_num)
	{
		//初始化界面
		InitDisp();
		cmdDisp("NBAND Set...   ");
		
		//开始时间等待
		StartWaitMsg();
		//发送数据		
		printf(cmd);
		//等待超过3s则说明发生错误
		while(usart.waittime <= AT_CGATT_WAIT_TIME)//超过3秒
		{
			if(usart.num!=0&&StrCmp("\r\nOK\r\n",usart.USART_RX_BUF))
			{
				//清除等待时间
				OLED_ShowStr(WAITINGTIME_X,WAITINGTIME_Y,"    ",1);
				//关闭等待
				StopWaitMsg();//停止等待信息
				MsgDisp(LINE_FEED);
				vTaskDelay(NEXT_AT_WAIT_TIME);

				return try_num;//返回
			}
			OLED_ShowNum(WAITINGTIME_X, WAITINGTIME_Y, usart.waittime, WAITINGTIME_DISP_DIGITS,1);
			#ifdef AT_TICKLESS_ON 
				vTaskDelay(AT_CHECK_EVERYTIME);//每200ms进入一次查看是否有数据
			#endif
			
		}
		sgnDisp("- OVERTIME -");
		vTaskDelay(NEXT_AT_WAIT_TIME);
		try_num--;
	}
	//关闭等待
	StopWaitMsg();
	return try_num;//剩余尝试次数，如果为0.则表示AT指令测试失败
}

//*******************************************************
//函数名称：CGATT_Get_Set
//输入参数：
//函数作用：连接网络
//返回参数：成功（>=1,TRUE）或者失败（0,FALSE）
//*******************************************************
u8 CGATT_Get_Set(void)
{
	u8 try_num = AT_CGATT_REPEAT_NUMBER;//AT发送后，返回ERROR，尝试的次数
	//下面给字符串赋值时，一定要给数组的大小
	char cmd[15] = "AT+CGATT?\r\n";
	CGATT_Set();//打开网络，CGATT = 1

	
	while(try_num)
	{	
		//初始化界面
		InitDisp();
		cmdDisp("CGATT GetSet...");
		//开始时间等待
		StartWaitMsg();
		//发送数据		
		printf(cmd);
		//等待超过3s则说明发生错误
		while(usart.waittime <= AT_CGATT_WAIT_TIME)//超过3秒
		{
			if(usart.num!=0&&StrCmp("\r\n+CGATT:1\r\n\r\nOK\r\n",usart.USART_RX_BUF))
			{
				//清除等待时间
				OLED_ShowStr(WAITINGTIME_X,WAITINGTIME_Y,"    ",1);
				//关闭等待
				StopWaitMsg();//停止等待信息
				MsgDisp(LINE_FEED);
				//下个AT指令前等待一段时间
				vTaskDelay(NEXT_AT_WAIT_TIME);

				return try_num;//返回
			}
			OLED_ShowNum(WAITINGTIME_X, WAITINGTIME_Y, usart.waittime, WAITINGTIME_DISP_DIGITS,1);
			#ifdef AT_TICKLESS_ON 
				vTaskDelay(AT_CHECK_EVERYTIME);//每200ms进入一次查看是否有数据
			#endif
			

		}
		sgnDisp("- OVERTIME -");		
		vTaskDelay(NEXT_AT_WAIT_TIME);
		CGATT_Set();
		try_num--;
	}
	//关闭等待
	StopWaitMsg();
	return try_num;//剩余尝试次数，如果为0.则表示AT指令测试失败
}

//*******************************************************
//函数名称：CGATT_Set
//输入参数：
//函数作用：CFUN的设置
//返回参数：成功（>=1,TRUE）或者失败（0,FALSE）
//*******************************************************
u8 CGATT_Set(void)
{
	u8 try_num = AT_CGATT_REPEAT_NUMBER;//AT发送后，返回ERROR，尝试的次数
	char cmd[15] = "AT+CGATT=1\r\n";
	
	while(try_num)
	{	
		//初始化界面
		InitDisp();
		cmdDisp("CGATT SET...");	
		//开始时间等待
		StartWaitMsg();
		//发送数据		
		printf(cmd);
		//等待超过3s则说明发生错误
		while(usart.waittime <= AT_DEFAULT_WAIT_TIME)//超过三秒
		{
			if(usart.num!=0&&StrCmp("\r\nOK\r\n",usart.USART_RX_BUF))
			{
				//清除等待时间
				OLED_ShowStr(WAITINGTIME_X,WAITINGTIME_Y,"    ",1);
				//关闭等待
				StopWaitMsg();//停止等待信息
				MsgDisp(LINE_FEED);
				vTaskDelay(NEXT_AT_WAIT_TIME);
				return try_num;//返回
			}
			OLED_ShowNum(WAITINGTIME_X, WAITINGTIME_Y, usart.waittime, WAITINGTIME_DISP_DIGITS,1);
			#ifdef AT_TICKLESS_ON 
				vTaskDelay(AT_CHECK_EVERYTIME);//每200ms进入一次查看是否有数据
			#endif
			
		}
		sgnDisp("- OVERTIME -");
		vTaskDelay(NEXT_AT_WAIT_TIME);
		try_num--;
	}
	//关闭等待
	StopWaitMsg();
	return try_num;//剩余尝试次数，如果为0.则表示AT指令测试失败
}
//*******************************************************
//函数名称：MSG_Send
//输入参数：数据的hexstr格式，和数据长度两个hex为一个数据
//如A为1个数据，hex格式为43
//函数作用：发送数据
//返回参数：成功（>=1,TRUE）或者失败（0,FALSE）
//*******************************************************
u8 intMSG_Send(u16 num,u8 len)
{
	u8 try_num = AT_NMGS_REPEAT_NUMBER;//重启次数
	char hexstr[DATA_MAXNUM] = {0,0,0};
	char numlenstr[3] = {0,0,0};//数据的长度，用十进制字符串表示如8位十进制数，则转化为"8"
	Dnum2Str(numlenstr,len,1);//这里设置成了1位十进制数，需要优化
	//数据AT指令
	char cmd[USART_SEND_LEN] = "AT+NMGS=";
	Num2HexStr(hexstr,num,2*len);
	strcat(cmd,numlenstr);
	strcat(cmd,",");
	strcat(cmd,hexstr);
	strcat(cmd,"\r\n");
	while(try_num)
	{
		//初始化界面
		InitDisp();
		cmdDisp("MSG Send...");	
		//开始时间等待
		StartWaitMsg();
		//发送数据		
		printf(cmd);
		//等待超过3s则说明发生错误
		while(usart.waittime <= AT_NMGS_WAIT_TIME)//超过5秒
		{
			if(usart.num!=0&&StrCmp("\r\nOK\r\n",usart.USART_RX_BUF))
			{
				//清除等待时间
				OLED_ShowStr(WAITINGTIME_X,WAITINGTIME_Y,"    ",1);
				//关闭等待
				StopWaitMsg();//停止等待信息
				MsgDisp(LINE_FEED);
				intMsgSentDisp(num);
				vTaskDelay(NEXT_AT_WAIT_TIME);				
				return try_num;//返回
			}
			OLED_ShowNum(WAITINGTIME_X, WAITINGTIME_Y, usart.waittime, WAITINGTIME_DISP_DIGITS,1);
			#ifdef AT_TICKLESS_ON 
				vTaskDelay(AT_CHECK_EVERYTIME);//每200ms进入一次查看是否有数据
			#endif
		}
		sgnDisp("- OVERTIME -");
		vTaskDelay(NEXT_AT_WAIT_TIME);
		try_num--;
	}
	//关闭等待
	StopWaitMsg();
	return try_num;//剩余尝试次数，如果为0.则表示AT指令测试失败
}

//*******************************************************
//函数名称：MSG_Send
//输入参数：数据的hexstr格式，和数据长度两个hex为一个数据
//如A为1个数据，hex格式为43
//函数作用：发送数据
//返回参数：成功（>=1,TRUE）或者失败（0,FALSE）
//*******************************************************
u8 MSG_Send(char * msg,char * len)
{
	u8 try_num = AT_NMGS_REPEAT_NUMBER;//重启次数
	char hexstr[DATA_MAXNUM] = {0,0,0};
	//数据AT指令
	char cmd[USART_SEND_LEN] = "AT+NMGS=";
	Str2HexStr(msg,hexstr);
	strcat(cmd,len);
	strcat(cmd,",");
	strcat(cmd,hexstr);
	strcat(cmd,"\r\n");
	while(try_num)
	{
		//初始化界面
		InitDisp();
		cmdDisp("MSG Send...");	
		//开始时间等待
		StartWaitMsg();
		//发送数据		
		printf(cmd);
		//等待超过3s则说明发生错误
		while(usart.waittime <= AT_NMGS_WAIT_TIME)//超过5秒
		{
			if(usart.num!=0&&StrCmp("\r\nOK\r\n",usart.USART_RX_BUF))
			{
				//清除等待时间
				OLED_ShowStr(WAITINGTIME_X,WAITINGTIME_Y,"    ",1);
				//关闭等待
				StopWaitMsg();//停止等待信息
				MsgDisp(LINE_FEED);
				MsgSentDisp(msg);
				vTaskDelay(NEXT_AT_WAIT_TIME);				
				return try_num;//返回
			}
			OLED_ShowNum(WAITINGTIME_X, WAITINGTIME_Y, usart.waittime, WAITINGTIME_DISP_DIGITS,1);
			#ifdef AT_TICKLESS_ON 
				vTaskDelay(AT_CHECK_EVERYTIME);//每200ms进入一次查看是否有数据
			#endif
		}
		sgnDisp("- OVERTIME -");
		vTaskDelay(NEXT_AT_WAIT_TIME);
		try_num--;
	}
	//关闭等待
	StopWaitMsg();
	return try_num;//剩余尝试次数，如果为0.则表示AT指令测试失败
}
//*******************************************************
//函数名称：CMD_Get
//输入参数：无
//函数作用：接收命令
//返回参数：成功（>=1,TRUE）或者失败（0,FALSE）
//*******************************************************
u8 CMD_Get(void)
{
	u8 try_num = AT_NMGR_REPEAT_NUMBER;//如果数据错误，重启次数
	s16 i = 0;//用于遍历buff
	//接收到命令
	char cmd[15] = "AT+NMGR\r\n";
	
	while(try_num)
	{
		//发送数据，并等待下行数据到来
		if(MSG_Send("get  msg","8") == 0)//如果数据发送失败，那么从新开始
			return SEND_MSG_ERROR;
		vTaskDelay(AT_BEFORE_NMGR_WAIT_TIME);
		//开始向CDP服务器请求下行数据
		i = 0;
		//初始化界面
		InitDisp();
		cmdDisp("MSG Rcv... ");	
		//开始时间等待
		StartWaitMsg();
		//发送数据		
		printf(cmd);
		//等待超过3s则说明发生错误
		while(usart.waittime <= AT_NMGR_WAIT_TIME)//超过5秒
		{
			while(usart.USART_RX_BUF[i]==0)//到达最后一位
			{
				if(usart.num>=5&&
					i-4>=0&&
					usart.USART_RX_BUF[i-4]=='O'&&
					usart.USART_RX_BUF[i-3]=='K')
				{
					//清除等待时间
					OLED_ShowStr(WAITINGTIME_X,WAITINGTIME_Y,"    ",1);
					cmdgot.status = CMD_Anls(usart.USART_RX_BUF);
					if(cmdgot.len != HexStr2Str(cmdgot.lastcmd,cmdgot.str))//如果位数不相等
						cmdgot.status = CMD_GOT_FORMAT_ERROR;//数据格式错误
					//关闭等待
					StopWaitMsg();//停止等待信息
					MsgDisp(LINE_FEED);
					vTaskDelay(NEXT_AT_WAIT_TIME);

					return try_num;//返回
				}
				if(usart.waittime > AT_NMGR_WAIT_TIME)
					break;
			}
			i++;
			OLED_ShowNum(WAITINGTIME_X, WAITINGTIME_Y, usart.waittime, WAITINGTIME_DISP_DIGITS,1);
			#ifdef AT_TICKLESS_ON 
				vTaskDelay(AT_CHECK_EVERYTIME);//每200ms进入一次查看是否有数据
			#endif
			
		}
		sgnDisp("- OVERTIME -");
		vTaskDelay(NEXT_AT_WAIT_TIME);
		try_num--;
	}
	//关闭等待
	StopWaitMsg();
	return try_num;//剩余尝试次数，如果为0.则表示AT指令测试失败
}


