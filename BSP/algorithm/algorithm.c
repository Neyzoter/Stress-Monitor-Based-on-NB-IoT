/*
 * Stress Monitor Based on NB-IoT
 * 项目名称：基于NB-IoT的应力监测系统
 * 作者：宋超超
 *
 * 该代码版权归宋超超所有，请勿用于商业用途。
 * https://neyzoter.github.io/
 */

#include "algorithm.h"
#include "math.h"
#include "common.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "I2C_OLED.h"
#include "timer.h"

/*******************************************************************************
* 函数名 : Num2Str
* 描述   : 数字转化为字符串,改变数组的第1~6位，共6位
		默认通信位数为符号数字和单位如+001232N
* 输入   :
* 输出   :
* 返回   :
* 注意   :
*******************************************************************************/
void Num2Str(char *str, unsigned long num)
{
	unsigned char index[]="0123456789";
	char i = 6;
    do
    {
        str[i--] = index[num%10];
        num /= 10;
    }while(i);
	
}

/*******************************************************************************
* 函数名 : Num2Str
* 描述   : 数字转化为字符串,如123——>"123"
* 输入   :
* 输出   :
* 返回   :
* 注意   :
*******************************************************************************/
void Dnum2Str(unsigned char * str,unsigned int num,unsigned char digit)
{
	unsigned char index[]="0123456789";
	char i = digit;
    do
    {
        str[--i] = index[num%10];
        num /= 10;
    }while(i);
}
/*******************************************************************************
* 函数名 : Num2Str
* 描述   : 数字转化为hex字符串,如十进制24 = 十六进制18 再转成字符串"18"
* 输入   :
* 输出   :
* 返回   :
* 注意   :
*******************************************************************************/
void Num2HexStr(char *str, unsigned long num, unsigned char digit)
{
	unsigned char index[]="0123456789ABCDEF";
	char i = digit;
    do
    {
        str[--i] = index[num%16];
        num /= 16;
    }while(i);

}
/*******************************************************************************
* 函数名 : Hex2Str
* 描述   : 16进制数转化成字符串，用于AT+NSOST命令
* 输入   : 输入: pSrc - 源数据指针
* 输出   : 输出: pDst - 目标字符串指针
* 返回   :
* 注意   : 如：{0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01} --> "C8329BFD0E01"
*******************************************************************************/
void Hex2Str(char* pSrc, char* pDst, unsigned int nSrcLength)
{
	int i = 0;
	const char tab[]="0123456789ABCDEF";	// 0x0-0xf的字符查找表

	for (i = 0; i < nSrcLength; i++)
	{
		*pDst++ = tab[*pSrc >> 4];		// 输出高4位
		*pDst++ = tab[*pSrc & 0x0f];	// 输出低4位
		pSrc++;
	}

	// 输出字符串加个结束符
	*pDst = '\0';
}

/*******************************************************************************
* 函数名 : AsciiToHex
* 描述   : ASCALL码转换成字符
* 输入   : unsigned char cNum  ASC-II字符码
* 输出   : unsigned char HEX码
* 返回   :
* 注意   : 如：{'A'} --> 0xA
*******************************************************************************/
unsigned char AsciiToHex(unsigned char cNum)
{
	if(cNum>='0'&&cNum<='9')
	{
		cNum -= '0';
	}
	else if(cNum>='A'&&cNum<='F')
	{
		cNum -= 'A';
		cNum += 10;
	}
	else if(cNum>='a'&&cNum<='f')
	{
		cNum -= 'a';
		cNum += 10;
	}
	return cNum;
}

/*******************************************************************************
* 函数名 : StrToHex
* 描述   : 字符串转换hex
* 输入   : char *ptr字符存储区，unsigned short len数据长度
* 输出   :
* 返回   :
* 注意   : 如："C8329BFD0E01" -->  {0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01}
*******************************************************************************/
void StrToHex(char *ptr, unsigned short len)
{
	unsigned char n1  = 0;
	unsigned char n2  = 0;
	unsigned char num = 0;
	unsigned short i = 0;
	unsigned short index = 0;

	for(i=0; i<len; i++)
	{
		index = i << 1;				//index=i*2
		n1 = AsciiToHex(ptr[index]);
		n2 = AsciiToHex(ptr[index+1]);
		num = (n1<<4) + n2;
		ptr[i] = num;
	}
}

/*******************************************************************************
* 函数名 : Str2Hex
* 描述   : 字符串转换hex
* 输入   : 字符串str和转化后存储的区域hexstr
* 输出   :
* 返回   :
* 注意   : 如："A" -->  {43}
str = +002321N

hex = 2b3030323332314e
*******************************************************************************/
void Str2HexStr(char * Str,char * HexStr)
{
    int i =0;
    u8 dec;
    char * p = HexStr;
    u8 hex = 0;
	const char tab[]="0123456789ABCDEF";	// 0x0-0xf的字符查找表
	
    for(i = 0;Str[i];i++)
    {
        dec = *(Str+i);//十进制表示一个ACCII
        hex = dec/16;
		*p = tab[hex];
		p++;
        hex = dec%16;
		*p = tab[hex];
		p++;
    }
    *p = 0;//最后一位赋值0
}
/*******************************************************************************
* 函数名 : HexStr2Str
* 描述   : hex转换为字符串
* 输入   : hexstr和转化后存储字符串的空间
* 输出   :数据是否符合格式0：不符合或者位数是0；>0：位数
* 返回   :
* 注意   : 如："A" -->  {43}

*******************************************************************************/
u16 HexStr2Str(char * HexStr,char * Str)
{
    u16 i =0;
	char * p = Str;
	
    for(i = 0;HexStr[i]&&HexStr[i+1];i+=2,p++)
    {
		//高四位
		if((HexStr[i]<='9')&&(HexStr[i]>='0'))
		{
			*p = (HexStr[i]-'0')*16;
		}
		else if((HexStr[i]<='F')&&(HexStr[i]>='A'))
		{
			*p = (HexStr[i]-'A'+10)*16;
		}
		else if((HexStr[i]<='f')&&(HexStr[i]>='a'))
		{
			*p = (HexStr[i]-'a'+10)*16;
		}
		else//不符合格式，因为可能出现了除了0-9,a-f或者A-F之外的数据
			return 0;
		
		//低四位
		if((HexStr[i+1]<='9')&&(HexStr[i+1]>='0'))
		{
			*p += (HexStr[i+1]-'0');
		}
		else if((HexStr[i+1]<='F')&&(HexStr[i+1]>='A'))
		{
			*p += (HexStr[i+1]-'A'+10);
		}
		else if((HexStr[i+1]<='f')&&(HexStr[i+1]>='a'))
		{
			*p += (HexStr[i+1]-'a'+10);
		}
		else//不符合格式，因为可能出现了除了0-9,a-f或者A-F之外的数据
			return 0;
    }
    *p = 0;//最后一位赋值0
	return i/2;//返回数据的位数
}
//*******************************************************
//函数名称：u8 DataPrc(u8 *buff,u8 *ch)
//输入参数：缓存数组
//函数作用：处理串口转换完成后的数据，和指令对比，得出相应的措施
//返回参数：指令完成情况
//*******************************************************
u8 StrCmp(u8 *buff,u8 *ch)
{
	u16 i,j;
	for(i = 0,j = 0;;i++,j++)
	{
		if(*(buff+i) != *(ch+j))//如果不相等
		{
			return 0;//不同
		}
		if(*(ch+j) == '\0'||*(buff+i)=='\0')//如果遍历到了参考数据的末尾
		{
			return 1;//相同
		}
	}
}
//*******************************************************
//函数名称：u16 Lenstr2Lennum(u8 lenstr[])
//输入参数：以字符形式存储的数字
//函数作用：将字符数字转化为u16数字，最大5万多
//返回参数：转化后的数字
//*******************************************************
u16 Lenstr2Lennum(u8 lenstr[])
{
    u8 i = 0;
    u16 num = 0;
    while((lenstr[i]<='9')&&(lenstr[i]>='0'))
    {
        num += (lenstr[i] - '0')*(mypow(10,i));
        i++;
    }
    return num;
}
//*******************************************************
//函数名称：u8 CMD_Anls(u8 cmd[])
//输入参数：缓存数组
//函数作用：处理串口转换完成后的数据，和指令对比，得出相应的措施
//返回参数：该数据的状态

//调用的时候需要将u8返回的数据给CDM_GOT.status赋值
//包括
/*
CMD_GOT_NUM_0 1//没有数据
CMD_GOT_TOO_LONG 2//没有数据
CMD_GOT_FORMAT_ERROR 3 //数据格式错误
CMD_GOT_OK 4 //数据接收成功
*/
//*******************************************************
u8 CMD_Anls(u8 cmd[])
{
    u16 i = CMD_GOT_START_INDEX;//从第二个开始遍历，因为前两个是\r\n
    u16 j = 0;//结构体中的下标遍历
    u16 data_end_i = 0;//数据结束的下标
    u8 len_str[CMD_MAXNUM_STR+1] = {0};//默认是长度0,分别表示个位十位百位
    if((cmd[i]>'9')||(cmd[i]<='0'))//如果开头不是数字说明有问题
        return CMD_GOT_NUM_0;//1：表示返回信息中没有数据
    while(cmd[i]!=',')//逗号作为分隔长度和数据
    {
        if(i>CMD_GOT_START_INDEX+CMD_MAXNUM_STR-1)//下标大于4就说明数据要溢出
            return CMD_GOT_TOO_LONG;//2：表示数据过长
        if(cmd[i]>'9'||cmd[i]<'0')//如果在逗号之前出现了除了数字之外的数据
            return CMD_GOT_FORMAT_ERROR;//数据格式错误
        len_str[i-CMD_GOT_START_INDEX] = cmd[i];
        i++;
    }
    cmdgot.len = Lenstr2Lennum(len_str);//把字符串转化为数字
    i++;//跳过','
    data_end_i = i + cmdgot.len*2-1;//指向数据最后一个byte
    while(cmd[i]!='\r')//还没有遇到\r时
    {
        if(((cmd[i]>='0'&&cmd[i]<='9')||
           (cmd[i]>='A'&&cmd[i]<='F')||
           (cmd[i]>='a'&&cmd[i]<='f'))&&
           (i <= data_end_i))
            cmdgot.lastcmd[j++] = cmd[i++];//保存
        else
            return CMD_GOT_FORMAT_ERROR;//数据格式错误
    }
    cmdgot.lastcmd[j] = 0;//最后一位赋值0
    return CMD_GOT_OK;
}

//m^n函数
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

//m^n函数
float mypow_float(float m,u8 n)
{
	float result=1.0;	 
	while(n--)result*=m;    
	return result;
}

//u16升序排序
void u16sortup(u16 * ain,u16 len)
{
    u16 i,j;
    u16 t;
    for(i=0;i<len-1;i++)//n个数的数列总共扫描len-1次
    {
        for(j=0;j<len-i-1;j++)//每一趟扫描到a[n-i-2]与a[n-i-1]比较为止结束
        {
            if(ain[j]>ain[j+1])//后一位数比前一位数小的话，就交换两个数的位置（升序）
            {
               t=ain[j+1];
               ain[j+1]=ain[j];
               ain[j]=t;
            }
        }
    }
}

