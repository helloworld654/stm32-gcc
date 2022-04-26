#include "motor.h"
#include "stm32f10x.h"
#include "stdio.h"

#define MAX_ARR     899
#define MIN_ARR    800
#define CHANGE_MOTOR_ONE    0    //the right motor
#define CHANGE_MOTOR_TWO    1    //the left motor

void motor_init(char motor_n)
{
    switch(motor_n){
        case 1:
            TIM3_CH12_init(MAX_ARR,0);
            break;
        case 2:
            TIM3_CH34_init(MAX_ARR,0);
            break;
        default:
            break;
    }
}

void motor_setSpeed(char motor_n,signed char speed)
{
    if(speed>100 || speed<-100){
        printf("error speed input!\r\n");
        return ;
    }
#if defined(CHANGE_MOTOR_ONE) && CHANGE_MOTOR_ONE
    if(motor_n == 1)
        speed = -speed;
#endif
#if defined(CHANGE_MOTOR_TWO) && CHANGE_MOTOR_TWO
    if(motor_n == 2)
        speed = -speed;
#endif
    switch(motor_n){
        case 1:
            if(speed == 0){
                TIM_SetCompare1(TIM3,0);
                TIM_SetCompare2(TIM3,0);
            }
	    else if(speed >  0){
                TIM_SetCompare1(TIM3,MIN_ARR+(int)((MAX_ARR-MIN_ARR)*speed/100.0));
                TIM_SetCompare2(TIM3,0);
            }
            else{
                TIM_SetCompare1(TIM3,0);
                TIM_SetCompare2(TIM3,MIN_ARR+(int)((MAX_ARR-MIN_ARR)*(-speed)/100.0));
            }
            break;
        case 2:
            if(speed == 0){
                TIM_SetCompare3(TIM3,0);
                TIM_SetCompare4(TIM3,0);
            }
	    else if(speed > 0){
                TIM_SetCompare3(TIM3,MIN_ARR+(int)((MAX_ARR-MIN_ARR)*speed/100.0));
                TIM_SetCompare4(TIM3,0);
            }
            else{
                TIM_SetCompare3(TIM3,0);
                TIM_SetCompare4(TIM3,MIN_ARR+(int)((MAX_ARR-MIN_ARR)*(-speed)/100.0));
            }
            break;
        default:
            printf("error motor_n input\r\n");
            break;
    }
}
