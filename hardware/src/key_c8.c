#include "key_c8.h"
#include "stm32f10x.h"
#include "exti.h"

void KEY0_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
}

void keys_init(char key_n)
{
	switch (key_n)
	{
	case 0:
		KEY0_init();
		EXTI0_Init();
		break;

	default:
		break;
	}
}
