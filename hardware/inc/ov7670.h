#ifndef _OV7670_H
#define _OV7670_H
#include "sys.h"
#include "sccb.h"

#define OV7670_RCK_H	PEout(4)=1//设置读数据时钟高电平
#define OV7670_RCK_L	PEout(4)=0	//设置读数据时钟低电平
#define OV7670_WREN		PEout(3)		//写入FIFO使能
#define OV7670_CS		PEout(2)  		//片选信号(OE)
#define OV7670_WRST		PEout(1)		//写指针复位
#define OV7670_RRST		PEout(0)  		//读指针复位
#define OV7670_VSYNC  	PBin(15)			//同步信号检测IO  used in exti.c
															  					 
#define OV7670_DATA   GPIO_ReadInputData(GPIOC,0x00FF) 					//数据输入端口
//GPIOF->IDR&0x00FF 
#define CHANGE_REG_NUM 							171			//需要配置的寄存器总数		  
extern const u8 ov7670_init_reg_tbl[CHANGE_REG_NUM][2];		//寄存器及其配置表
	    				 
u8   OV7670_Init(void);		  	   		 
void OV7670_Light_Mode(u8 mode);
void OV7670_Color_Saturation(u8 sat);
void OV7670_Brightness(u8 bright);
void OV7670_Contrast(u8 contrast);
void OV7670_Special_Effects(u8 eft);
void OV7670_Window_Set(u16 sx,u16 sy,u16 width,u16 height);

#endif
