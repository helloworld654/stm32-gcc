
#include "led_c8.h"
#include "stm32f10x.h"
#include "stdio.h"

void leds_init(char led_n)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	switch(led_n){
		case 0:
			LED0_RCC_ENABLE();
			GPIO_InitStructure.GPIO_Pin = LED0_PIN;
			GPIO_Init(LED0_PORT, &GPIO_InitStructure);
			LED0_POWER_OFF();
			break;
		default:
			printf("error input led_n\r\n");
			break;
	}
}

void led_set_status(char led_n,char status)
{
	switch (led_n)
	{
		case 0:
			if(status)
				LED0_POWER_ON();
			else
				LED0_POWER_OFF();
			break;
		default:
			break;
	}
}

void led_set_pwm(char led_n,char duty_cycle)
{
	if(duty_cycle < 0)
		duty_cycle = 0;
	if(duty_cycle > 100)
		duty_cycle = 100;
	if(led_n == 3)
		servo0_set_duty_cycle(duty_cycle);
}

void rest_esp_init(void)
{
    GPIO_InitTypeDef     GPIO_InitStrue;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

    GPIO_InitStrue.GPIO_Pin=GPIO_Pin_15;
    GPIO_InitStrue.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_InitStrue.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOC,&GPIO_InitStrue);

	GPIO_SetBits(GPIOC,GPIO_Pin_15);
}

void set_reset_pin(uint8_t state)
{
	if(state){
		GPIO_SetBits(GPIOC,GPIO_Pin_15);
	}
	else{
		GPIO_ResetBits(GPIOC,GPIO_Pin_15);
	}
}

void reset_esp_process(void)
{
	rest_esp_init();
	set_reset_pin(0);
	delay_lms(1000);
	set_reset_pin(1);
}
