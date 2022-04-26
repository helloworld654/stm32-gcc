#ifndef __SCCB_H
#define __SCCB_H
#include "sys.h"

/*  SCL:PC10    SDA:PC11   */
#define SCCB_SDA_IN()  {GPIOC->CRH&=0XFFFF0FFF;GPIOC->CRH|=0X00008000;}
#define SCCB_SDA_OUT() {GPIOC->CRH&=0XFFFF0FFF;GPIOC->CRH|=0X00003000;}
#define SCCB_SCL    		PCout(10)	 	//SCL
#define SCCB_SDA    		PCout(11) 		//SDA	 
#define SCCB_READ_SDA    	PCin(11)  		// ‰»ÎSDA    
#define SCCB_ID   			0X42  			//OV7670µƒID

void SCCB_Init(void);
void SCCB_Start(void);
void SCCB_Stop(void);
void SCCB_No_Ack(void);
u8 SCCB_WR_Byte(u8 dat);
u8 SCCB_RD_Byte(void);
u8 SCCB_WR_Reg(u8 reg,u8 data);
u8 SCCB_RD_Reg(u8 reg);

#endif
