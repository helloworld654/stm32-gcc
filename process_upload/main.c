#include "stm32f10x.h"
#include "AllHead.h"
#include "inout.h"
#include "servo.h"
#include "usart.h"
#include "delay.h"
#include "sys.h"
#include "lcd.h"
#include "ov7670.h"
#include "time.h"
#include "exti.h"
#include "grey.h"
#include "a4950.h"
#include "nlp.h"
	
void JTAG_Set(u8 mode);
u8 RUNNING = 0;
extern uint8_t receive_flag,mess1_receive_flag,mess2_receive_flag,mess3_receive_flag,mess4_receive_flag,mess5_receive_flag;

// user pre-def functions

int main(void)
{	 
	JTAG_Set(0x01);
	delay_init();
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
#if defined(REMOTE_TEST) && REMOTE_TEST
// user init functions
#else
	uart1_init(115200);
#endif
#if defined(REMOTE_TEST) && REMOTE_TEST
	while(1){
// user while functions
	}
#else
#if defined(BIKING) && BIKING
#if defined(START_TEST) && START_TEST
	MotorA_start();
	motorA_test();
#endif
#endif
	LCD_Init(); 
	if(lcddev.id==0X6804||lcddev.id==0X5310) 	//强制设置屏幕分辨率为320*240.以支持3.5寸大屏
	{
		lcddev.width=240;
		lcddev.height=320; 
	}
	// usmart_dev.init(72);	//初始化USMART			    
 	POINT_COLOR=RED;//设置字体为红色 
	LCD_ShowString(60,50,200,16,16,"WarShip STM32");	
	LCD_ShowString(60,70,200,16,16,"OV7670 TEST");	
	LCD_ShowString(60,90,200,16,16,"hello world logial");
	LCD_ShowString(60,110,200,16,16,"2021/7/30");  
  	LCD_ShowString(60,230,200,16,16,"OV7670 Init...");	  
	while(OV7670_Init())//初始化OV7670
	{
		LCD_ShowString(60,230,200,16,16,"OV7670 Error!!");
		delay_ms(200);
	    LCD_Fill(60,230,239,246,WHITE);
		delay_ms(200);
	}
 	LCD_ShowString(60,230,200,16,16,"OV7670 Init OK");
	delay_ms(500);	 	   	   
	LCD_Clear(BLACK); 
	
	TIM6_Int_Init(10000,7199);			//10Khz计数频率,1秒钟中断									  
	EXTI11_Init();		// PC12 for ov7670 VSYNC interput pin in exti.c
	OV7670_Window_Set(10,174,240,320);	//设置窗口	  
  	OV7670_CS=0;						 	 
 	while(1)
	{	
		cameraOperation();
	}	   
#endif
}

/*针对调试和使用冲突问题*/
void JTAG_Set(u8 mode)
{
	u32 temp;
	temp = mode;
	temp <<= 25;
	RCC->APB2ENR |= 1<<0;
	AFIO->MAPR &= 0xF8FFFFFF;
	AFIO->MAPR |= temp;
}
