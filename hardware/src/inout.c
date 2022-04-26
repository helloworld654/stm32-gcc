#include "inout.h"
#include "stm32f10x.h"
#include "key_c8.h"

char KEY0_PUSH = 0;
char KEY1_PUSH = 0;

// model:1(output);0(input);2(pwm)
void pinMode(char pin_n,char module)
{
    switch(module){
        case 0:
            keys_init(pin_n);
            break;
        case 1:
            leds_init(pin_n);
            break;
        case 2:
        // PA1 PWM
            servo_attach(0);
            break;
        default:
            break;
    }
}

void digitalWrite(char led_n,char status)
{
    led_set_status(led_n,status);
}

void analogWrite(char led_n,char duty_cycle)
{
    led_set_pwm(led_n,duty_cycle);
}

char digitalRead(char key_n)
{
    switch(key_n){
        case 0:
            if(KEY0_PUSH){
                KEY0_PUSH = 0;
                return 1;
            }
            else{
                return 0;
            }
        case 1:
            if(KEY1_PUSH){
                KEY1_PUSH = 0;
                return 1;
            }
            else{
                return 0;
            }
        default:
            return 0;
    }
}
