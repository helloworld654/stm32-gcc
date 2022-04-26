/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	esp8266.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-05-08
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		ESP8266�ļ�����
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//�����豸����
#include "esp8266.h"

//Ӳ������
#include "delay.h"
#include "usart.h"

//C��
#include <string.h>
#include <stdio.h>

#define COMPARE_MESS_SUCCESS    60

unsigned char esp8266_buf[128];
unsigned short esp8266_cnt = 0, esp8266_cntPre = 0;
uint8_t receive_flag = 0;
uint8_t mess1_receive_flag,mess2_receive_flag,mess3_receive_flag,mess4_receive_flag,mess5_receive_flag;

//==========================================================
//	�������ƣ�	ESP8266_Clear
//
//	�������ܣ�	��ջ���
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
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
//	�������ƣ�	ESP8266_WaitRecive
//
//	�������ܣ�	�ȴ��������
//
//	��ڲ�����	��
//
//	���ز�����	REV_OK-�������		REV_WAIT-���ճ�ʱδ���
//
//	˵����		ѭ�����ü���Ƿ�������
//==========================================================
_Bool ESP8266_WaitRecive(void)
{

	if(esp8266_cnt == 0) 							//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return REV_WAIT;
		
	if(esp8266_cnt == esp8266_cntPre)				//�����һ�ε�ֵ�������ͬ����˵���������
	{			
		return REV_OK;								//���ؽ�����ɱ�־
	}
		
	esp8266_cntPre = esp8266_cnt;					//��Ϊ��ͬ
	
	return REV_WAIT;								//���ؽ���δ��ɱ�־

}

//==========================================================
//	�������ƣ�	ESP8266_SendCmd
//
//	�������ܣ�	��������
//
//	��ڲ�����	cmd������
//				res����Ҫ���ķ���ָ��
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		
//==========================================================
_Bool ESP8266_SendCmd(char *cmd, char *res)
{
	
	unsigned char timeOut = 200;
	ESP8266_Clear();
	Usart_SendString(USART2, (unsigned char *)cmd, strlen((const char *)cmd));
	
	while(timeOut--)
	{
		if(ESP8266_WaitRecive() == REV_OK)							//����յ�����
		{
			esp8266_buf[esp8266_cnt] = '\0';
			if(strstr((const char *)esp8266_buf, res) != NULL)		//����������ؼ���
			{
				ESP8266_Clear();									//��ջ���
				
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
//	�������ƣ�	USART2_IRQHandler
//
//	�������ܣ�	����2�շ��ж�
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void USART2_IRQHandler(void)
{

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //�����ж�
	{
		if(esp8266_cnt >= sizeof(esp8266_buf))	esp8266_cnt = 0; //��ֹ���ڱ�ˢ��
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
