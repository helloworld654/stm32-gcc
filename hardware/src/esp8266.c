/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	esp8266.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-05-08
	*
	*	版本： 		V1.0
	*
	*	说明： 		ESP8266的简单驱动
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//网络设备驱动
#include "esp8266.h"

//硬件驱动
#include "delay.h"
#include "usart.h"

//C库
#include <string.h>
#include <stdio.h>

#define COMPARE_MESS_SUCCESS    60

unsigned char esp8266_buf[128];
unsigned short esp8266_cnt = 0, esp8266_cntPre = 0;
uint8_t receive_flag = 0;
uint8_t mess1_receive_flag,mess2_receive_flag,mess3_receive_flag,mess4_receive_flag,mess5_receive_flag;

//==========================================================
//	函数名称：	ESP8266_Clear
//
//	函数功能：	清空缓存
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ESP8266_Clear(void)
{
	memset(esp8266_buf, 0, sizeof(esp8266_buf));
	esp8266_cnt = 0;
}

void clear_all_mess_rece_flags(void)
{
	mess1_receive_flag = 0;
	mess2_receive_flag = 0;
	mess3_receive_flag = 0;
	mess4_receive_flag = 0;
	mess5_receive_flag = 0;
}

//==========================================================
//	函数名称：	ESP8266_WaitRecive
//
//	函数功能：	等待接收完成
//
//	入口参数：	无
//
//	返回参数：	REV_OK-接收完成		REV_WAIT-接收超时未完成
//
//	说明：		循环调用检测是否接收完成
//==========================================================
_Bool ESP8266_WaitRecive(void)
{

	if(esp8266_cnt == 0) 							//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return REV_WAIT;
		
	if(esp8266_cnt == esp8266_cntPre)				//如果上一次的值和这次相同，则说明接收完毕
	{			
		return REV_OK;								//返回接收完成标志
	}
		
	esp8266_cntPre = esp8266_cnt;					//置为相同
	
	return REV_WAIT;								//返回接收未完成标志

}

//==========================================================
//	函数名称：	ESP8266_SendCmd
//
//	函数功能：	发送命令
//
//	入口参数：	cmd：命令
//				res：需要检查的返回指令
//
//	返回参数：	0-成功	1-失败
//
//	说明：		
//==========================================================
_Bool ESP8266_SendCmd(char *cmd, char *res)
{
	
	unsigned char timeOut = 200;
	ESP8266_Clear();
	Usart_SendString(USART2, (unsigned char *)cmd, strlen((const char *)cmd));
	
	while(timeOut--)
	{
		if(ESP8266_WaitRecive() == REV_OK)							//如果收到数据
		{
			esp8266_buf[esp8266_cnt] = '\0';
			if(strstr((const char *)esp8266_buf, res) != NULL)		//如果检索到关键词
			{
				ESP8266_Clear();									//清空缓存
				
				return 0;
			}
		}
		
		delay_lms(10);
	}
	
	return 1;

}

void compare_rece_mess(char *nlp_mess[])
{
	uint8_t i;
	clear_all_mess_rece_flags();
	printf("[%s]esp8266_buf:%s\r\n",__func__,esp8266_buf);
	if(Find_Lcs(esp8266_buf,nlp_mess[0]) >= COMPARE_MESS_SUCCESS)
		mess1_receive_flag = 1;
	if(Find_Lcs(esp8266_buf,nlp_mess[1]) >= COMPARE_MESS_SUCCESS)
		mess2_receive_flag = 1;
	if(Find_Lcs(esp8266_buf,nlp_mess[2]) >= COMPARE_MESS_SUCCESS)
		mess3_receive_flag = 1;
	if(Find_Lcs(esp8266_buf,nlp_mess[3]) >= COMPARE_MESS_SUCCESS)
		mess4_receive_flag = 1;
	if(Find_Lcs(esp8266_buf,nlp_mess[4]) >= COMPARE_MESS_SUCCESS)
		mess5_receive_flag = 1;
	ESP8266_Clear();
	receive_flag = 0;
}

void print_rece_mess(void)
{
	printf("get a mess:");
	printf("%s\r\n",esp8266_buf);
	ESP8266_Clear();
	receive_flag = 0;
}

//==========================================================
//	函数名称：	USART2_IRQHandler
//
//	函数功能：	串口2收发中断
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void USART2_IRQHandler(void)
{

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //接收中断
	{
		if(esp8266_cnt >= sizeof(esp8266_buf))	esp8266_cnt = 0; //防止串口被刷爆
		esp8266_buf[esp8266_cnt++] = USART2->DR;
		if(esp8266_buf[esp8266_cnt-1]=='\n' && esp8266_buf[esp8266_cnt-2]=='\r' && esp8266_buf[esp8266_cnt-3]=='\r'){
			esp8266_buf[esp8266_cnt-3] = '\0';
		// if(esp8266_buf[esp8266_cnt-1]=='\n' && esp8266_buf[esp8266_cnt-2]=='\r'){
		// 	esp8266_buf[esp8266_cnt-2] = '\0';
			receive_flag = 1;
		}
		
		USART_ClearFlag(USART2, USART_FLAG_RXNE);
	}

}
