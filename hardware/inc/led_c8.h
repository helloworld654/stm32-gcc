
#ifndef LED_H
#define LED_H

#define LED0_PORT   GPIOC
#define LED0_PIN    GPIO_Pin_13
#define LED0_RCC_ENABLE()		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE)
#define LED0_POWER_ON()   GPIO_ResetBits(LED0_PORT,LED0_PIN)
#define LED0_POWER_OFF()   GPIO_SetBits(LED0_PORT,LED0_PIN)

void leds_init(char led_n);
void led_set_status(char led_n,char status);

#endif
