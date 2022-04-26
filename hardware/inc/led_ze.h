
#ifndef LED_H
#define LED_H

#define LED0_PORT   GPIOB
#define LED0_PIN    GPIO_Pin_5
#define LED0_RCC_ENABLE()		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE)
#define LED0_POWER_ON()   GPIO_ResetBits(LED0_PORT,LED0_PIN)
#define LED0_POWER_OFF()   GPIO_SetBits(LED0_PORT,LED0_PIN)

#define LED1_PORT   GPIOE
#define LED1_PIN    GPIO_Pin_5
#define LED1_RCC_ENABLE()		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE)
#define LED1_POWER_ON()   GPIO_ResetBits(LED1_PORT,LED1_PIN)
#define LED1_POWER_OFF()   GPIO_SetBits(LED1_PORT,LED1_PIN)

void LED_Init(void);
void leds_init(char led_n);
void led_set_status(char led_n,char status);

#endif
