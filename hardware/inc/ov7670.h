#ifndef _OV7670_H
#define _OV7670_H
#include "sys.h"
#include "sccb.h"

#define OV7670_RCK_H	PEout(4)=1//���ö�����ʱ�Ӹߵ�ƽ
#define OV7670_RCK_L	PEout(4)=0	//���ö�����ʱ�ӵ͵�ƽ
#define OV7670_WREN		PEout(3)		//д��FIFOʹ��
#define OV7670_CS		PEout(2)  		//Ƭѡ�ź�(OE)
#define OV7670_WRST		PEout(1)		//дָ�븴λ
#define OV7670_RRST		PEout(0)  		//��ָ�븴λ
#define OV7670_VSYNC  	PBin(15)			//ͬ���źż��IO  used in exti.c
															  					 
#define OV7670_DATA   GPIO_ReadInputData(GPIOC,0x00FF) 					//��������˿�
//GPIOF->IDR&0x00FF 
#define CHANGE_REG_NUM 							171			//��Ҫ���õļĴ�������		  
extern const u8 ov7670_init_reg_tbl[CHANGE_REG_NUM][2];		//�Ĵ����������ñ�
	    				 
u8   OV7670_Init(void);		  	   		 
void OV7670_Light_Mode(u8 mode);
void OV7670_Color_Saturation(u8 sat);
void OV7670_Brightness(u8 bright);
void OV7670_Contrast(u8 contrast);
void OV7670_Special_Effects(u8 eft);
void OV7670_Window_Set(u16 sx,u16 sy,u16 width,u16 height);

#endif
