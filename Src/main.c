#include "stm32f10x.h"
#include "AllHead.h"
#include "usart.h"
#include "delay.h"
#include "sys.h"
#include "exti.h"
	
void JTAG_Set(u8 mode);

int main(void)
{	 
	JTAG_Set(0x01);
	delay_init();
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart1_init(115200);
	while(1){
		printf("hello world\r\n");
		delay_lms(2000);
	}
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
