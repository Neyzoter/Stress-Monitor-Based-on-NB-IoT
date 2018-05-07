/*
 * Stress Monitor Based on NB-IoT
 * 项目名称：基于NB-IoT的应力监测系统
 * 作者：宋超超
 *
 * 该代码版权归宋超超所有，请勿用于商业用途。
 * https://neyzoter.github.io/
 */

#include "I2C_OLED.h"
#include "myiic.h"
#include "codetab.h"
#include "BC95.h"
#include "algorithm.h"
#include "common.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"

//OLED_TEMP oled_temp = {"FL\0","CMD:\0","MSG:\0","\0","\0","\0","\0","\0","\0","\0",0};


void I2C_OLED_Init(void)
{
	IIC_Init();
}
void WriteCmd(u8 I2C_Command)//写命令
{
	I2C_WriteByte(0x00, I2C_Command);
}

void WriteDat(u8 I2C_Data)//写数据
{
	I2C_WriteByte(0x40, I2C_Data);
}

void I2C_WriteByte(u8 addr,u8 data)
{		   	  	    																 
    IIC_Start();  
	IIC_Send_Byte(OLED_ADDRESS);	 //器件地址
	IIC_Wait_Ack();	   
    IIC_Send_Byte(addr);   //发送寄存器地址（0x00表示写命令、0x04表示写数据）
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(data);     //发送字节							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//产生一个停止条件 
//	HAL_Delay(1);	 //10ms	，这里尽量缩短时间
//	n_nops(1);
}


void OLED_Init(void)
{
	HAL_Delay(1000); //这里的延时很重要
	
	WriteCmd(0xAE); //display off
	WriteCmd(0x20);	//Set Memory Addressing Mode	
	WriteCmd(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	WriteCmd(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
	WriteCmd(0xc8);	//Set COM Output Scan Direction
	WriteCmd(0x00); //---set low column address
	WriteCmd(0x10); //---set high column address
	WriteCmd(0x40); //--set start line address
	WriteCmd(0x81); //--set contrast control register
	WriteCmd(0xff); //亮度调节 0x00~0xff
	WriteCmd(0xa1); //--set segment re-map 0 to 127
	WriteCmd(0xa6); //--set normal display
	WriteCmd(0xa8); //--set multiplex ratio(1 to 64)
	WriteCmd(0x3F); //
	WriteCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	WriteCmd(0xd3); //-set display offset
	WriteCmd(0x00); //-not offset
	WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
	WriteCmd(0xf0); //--set divide ratio
	WriteCmd(0xd9); //--set pre-charge period
	WriteCmd(0x22); //
	WriteCmd(0xda); //--set com pins hardware configuration
	WriteCmd(0x12);
	WriteCmd(0xdb); //--set vcomh
	WriteCmd(0x20); //0x20,0.77xVcc
	WriteCmd(0x8d); //--set DC-DC enable
	WriteCmd(0x14); //
	WriteCmd(0xaf); //--turn on oled panel
	OLED_CLS();//清屏
}


void OLED_SetPos(u8 x, u8 y) //设置起始点坐标
{ 
	WriteCmd(0xb0+y);
	WriteCmd(((x&0xf0)>>4)|0x10);
	WriteCmd((x&0x0f)|0x01);
}

void OLED_Fill(u8 fill_Data)//全屏填充
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		WriteCmd(0xb0+m);		//page0-page1
		WriteCmd(0x00);		//low column start address
		WriteCmd(0x10);		//high column start address
		for(n=0;n<128;n++)
			{
				WriteDat(fill_Data);
			}
	}
}

void OLED_CLS(void)//清屏
{
	OLED_Fill(0x00);
}

//--------------------------------------------------------------
// Prototype      : void OLED_ON(void)
// Calls          : 
// Parameters     : none
// Description    : 将OLED从休眠中唤醒
//--------------------------------------------------------------
void OLED_ON(void)
{
	WriteCmd(0X8D);  //设置电荷泵
	WriteCmd(0X14);  //开启电荷泵
	WriteCmd(0XAF);  //OLED唤醒
}

void OLED_OFF(void)
{
	WriteCmd(0X8D);  //设置电荷泵
	WriteCmd(0X10);  //关闭电荷泵
	WriteCmd(0XAE);  //OLED休眠
}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
// Calls          : 
// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); ch[] -- 要显示的字符串; TextSize -- 字符大小(1:6*8 ; 2:8*16)
// Description    : 显示codetab.h中的ASCII字符,有6*8和8*16可选择
//--------------------------------------------------------------
void OLED_ShowStr(u8 x, u8 y,u8 ch[], u8 TextSize)
{
	u8 c = 0,i = 0,j = 0;
	switch(TextSize)
	{
		case 1:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 126)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<6;i++)
					WriteDat(F6x8[c][i]);
				x += 6;
				j++;
			}
		}break;
		case 2:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 120)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<8;i++)
					WriteDat(F8X16[c*16+i]);
				OLED_SetPos(x,y+1);
				for(i=0;i<8;i++)
					WriteDat(F8X16[c*16+i+8]);
				x += 8;
				j++;
			}
		}break;
	}
}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N)
// Calls          : 
// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); N:汉字在codetab.h中的索引
// Description    : 显示codetab.h中的汉字,16*16点阵
//--------------------------------------------------------------
void OLED_ShowCN(u8 x,u8 y,u8 N)
{
	u8 wm=0;
	u16  adder=32*N;
	OLED_SetPos(x , y);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(F16x16[adder]);
		adder += 1;
	}
	OLED_SetPos(x,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(F16x16[adder]);
		adder += 1;
	}
}

//*******************************************************
//函数名称：void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len,u8 TextSize)
//输入参数：坐标，数字、长度和字体
//函数作用：显示数字，最多显示10位数字，0~4294967295
//返回参数：无
//*******************************************************
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len,u8 TextSize)
{
	u8 t,temp;
	u8 enshow=0;
	u8 str[10] = {0,0,0,0};

	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				str[t] = ' '; 
				continue;
			}else enshow=1; 
		 	 
		}
	 	str[t] = temp+'0';
	}
	str[t] = 0;
	OLED_ShowStr(x,y,str,TextSize);
}


//*******************************************************
//函数名称：MsgDisp
//输入参数：选择从rcv后直接开始还是换行开始显示
//函数作用：显示带\n\r的信息
//返回参数：无
//*******************************************************
void MsgDisp(u8 linefeed)
{
	u16 i=2;//从数据的第2个开始，因为第0和1个必定是\r\n
//	u8 j = 1;//用于清屏
	u8 row = RCV_START_Y;//从第一行开始显示
	u8 col=0;//从第0列开始
	u8 temp;//缓存字符
	u8 x_now;//当前的x位置
	taskENTER_CRITICAL();           //进入临界区
//	//清除msg
//	OLED_ShowStr(RCV_START_X,RCV_START_Y,"                ",1);	//15空
//	while(RCV_START_Y+j<=7)	
//	{
//		OLED_ShowStr(0,RCV_START_Y+j,"                      ",1);//21空
//		j++;
//	}
	while(i < usart.num)
	{
		if(usart.USART_RX_BUF[i]==0x0d&&usart.USART_RX_BUF[i+1]==0x0a)
		{
			i+=2;
			row++;
			col=0;
		}
		else 
		{
			if(usart.USART_RX_BUF[i]>=' '&&usart.USART_RX_BUF[i]<='~')//只要是可显示的字符
				temp = usart.USART_RX_BUF[i];
			else//如果是不可显示的字符，则用?代替
				temp = '?';
			if(linefeed)//如果换行开始显示
				x_now = RCV_START_X+6*col;
			else//rcv后面直接写
			{
				if(row==RCV_START_Y)//第一行和其他的不一样
					x_now = RCV_START_X_NoNewLine+6*col;
				else
					x_now = 6*col;
			}
			if(x_now<=120&&row<=7)//防止他超出去，导致卡死
			{
				OLED_ShowStr(x_now,row,&temp,1);
			}
			col++;
			i++;
		}
	}
	taskEXIT_CRITICAL();            //退出临界区
	
}

//*******************************************************
//函数名称：InitDisp
//输入参数：
//函数作用：清屏并初始化屏幕信息
//返回参数：无
//不允许在定时器中执行
//*******************************************************
void InitDisp(void)
{
	taskENTER_CRITICAL();           //进入临界区
	OLED_CLS();//清屏
	if(flag.BRD_STA==BRD_ATTACKED)
		OLED_ShowStr(0,0,"OL",1);//成功连上，online	
	else
		OLED_ShowStr(0,0,"FL",1);	//offline
	OLED_ShowStr(0,1,"CMD:",1);	
	OLED_ShowStr(0,2,"Rcv:",1);
	taskEXIT_CRITICAL();           //退出临界区
	
}
//*******************************************************
//函数名称：cmdDisp
//输入参数：
//函数作用：显示cmd
//返回参数：无
//*******************************************************
void cmdDisp(u8 * cmd)
{
	taskENTER_CRITICAL();           //进入临界区
	OLED_ShowStr(CMD_START_X,CMD_START_Y,cmd,1);
	taskEXIT_CRITICAL();           //退出临界区
}
//*******************************************************
//函数名称：sgnDisp
//输入参数：
//函数作用：显示cmd
//返回参数：无
//*******************************************************
void sgnDisp(u8 * sgn)
{
	taskENTER_CRITICAL();           //进入临界区
	OLED_ShowStr(SGN_START_X,SGN_START_Y,sgn,1);
	taskEXIT_CRITICAL();           //退出临界区
}
//*******************************************************
//函数名称：MsgSentDisp
//输入参数：
//函数作用：显示发送的数据
//返回参数：无
//*******************************************************
void MsgSentDisp(u8 * msg)
{
	taskENTER_CRITICAL();           //进入临界区
	OLED_ShowStr(0,7,msg,1);
	taskEXIT_CRITICAL();           //退出临界区
}

//*******************************************************
//函数名称：intMsgSentDisp
//输入参数：
//函数作用：显示发送的数据
//返回参数：无
//*******************************************************
void intMsgSentDisp(u16 intmsg)
{
	taskENTER_CRITICAL();           //进入临界区
	OLED_ShowNum(0,7, intmsg,6,1);
	taskEXIT_CRITICAL();           //退出临界区
}
//*******************************************************
//函数名称：CMD_GOT_STA_Disp
//输入参数：
//函数作用：显示接收到的数据的状态
//返回参数：无
//*******************************************************
void CMD_GOT_STA_Disp(void)
{
	taskENTER_CRITICAL();           //进入临界区
	switch(cmdgot.status)
	{
		case CMD_GOT_OK:
			sgnDisp("-  GOT_OK  -");break;
		case CMD_GOT_NUM_0:
			sgnDisp("-   NONE   -");break;
		case CMD_GOT_TOO_LONG:
			sgnDisp("-  2 LONG  -");break;
		case CMD_GOT_FORMAT_ERROR:
			sgnDisp("-  FMTERR  -");break;
		default:
			break;
	}
	taskEXIT_CRITICAL();           //退出临界区
}


//*******************************************************
//函数名称：CMD_GOT_Disp
//输入参数：
//函数作用：显示接收到的数据
//返回参数：无
//*******************************************************
void CMD_GOT_Disp(void)
{
	taskENTER_CRITICAL();           //进入临界区
	OLED_ShowStr(0,7,cmdgot.lastcmd,1);
	OLED_ShowStr(0,6,cmdgot.str,1);
	taskEXIT_CRITICAL();           //退出临界区
}


//*******************************************************
//函数名称：BrdStaDisp
//输入参数：
//函数作用：显示当前板子的状态，用LED
//返回参数：无
//*******************************************************
void BrdStaDisp(void)
{
	switch(flag.BRD_STA)
	{
		case BRD_ATTACKNETWORKING://板子正在尝试连接
		{
			taskENTER_CRITICAL();           //进入临界区
			OLED_ShowStr(0,0,"FL",1);
			taskEXIT_CRITICAL();           //退出临界区
			break;
		}
		case BRD_ATTACKED://板子已经连接成功
		{
			taskENTER_CRITICAL();           //进入临界区
			OLED_ShowStr(0,0,"OL",1);
			taskEXIT_CRITICAL();           //退出临界区
			break;
		}
		default:
		{
			break;
		}
	}
}

//*******************************************************
//函数名称：Error_Handler_Disp
//输入参数：当前文件和行数
//函数作用：显示错误的文件和行数，在Error_Handler中使用
//返回参数：无
//*******************************************************
void Error_Handler_Disp(char * file,int line)
{
	OLED_CLS();//清屏
	OLED_ShowStr(MSG_START_X,0,"- ERROR -",1);
	OLED_ShowStr(0,2,file,1);
	OLED_ShowNum(0,3,line,5,1);
}

