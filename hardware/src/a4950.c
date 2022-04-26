#include "a4950.h"
#include "timer.h"

/*  tim3 for left motor
    forward moving when tim3 ch1 > tim3 ch2
    tim4 for right motor
    forward moving when tim4 ch1 > tim4 ch2    */

#define SPEED_MIN   40  // last 30
#define MAX_ARR_A     900
extern u8 RUNNING;

void set_speed(int speed_L,int speed_R)
{
    // if(RUNNING == 0)
    //     return ;
    if(speed_L>100 || speed_L<-100 || speed_R>100 || speed_R<-100){
        printf("set speed error!\r\n");
        return ;
    }
    if(speed_L >= 0){
        TIM_SetCompare1(TIM3,(int)(MAX_ARR_A*(1-speed_L/100.0)));
        TIM_SetCompare2(TIM3,MAX_ARR_A);
    }
    else if(speed_L < 0){
        TIM_SetCompare1(TIM3,MAX_ARR_A);
        TIM_SetCompare2(TIM3,(int)(MAX_ARR_A*(1+speed_L/100.0)));
    }
    if(speed_R >= 0){
        TIM_SetCompare3(TIM4,MAX_ARR_A);
        TIM_SetCompare4(TIM4,(int)(MAX_ARR_A*(1-speed_R/100.0)));
    }
    else if(speed_R < 0){
        TIM_SetCompare3(TIM4,(int)(MAX_ARR_A*(1+speed_R/100.0)));
        TIM_SetCompare4(TIM4,MAX_ARR_A);
    }
}

void add_speed(uint8_t L_add,uint8_t R_add)
{
    int L_speed,R_speed;
    if(L_add>100-SPEED_MIN){
        L_add = 100-SPEED_MIN;
    }
    if(R_add>100-SPEED_MIN){
        R_add = 100-SPEED_MIN;
    }
    L_speed = SPEED_MIN+L_add;
    R_speed = SPEED_MIN+R_add;
    set_speed(L_speed,R_speed);
}

void start_forward(void)
{
    set_speed(SPEED_MIN,SPEED_MIN);
}

void stop_forward(void)
{
    set_speed(0,0);
}

void turn_left_A(void)
{
    set_speed(0,SPEED_MIN);
}

void turn_right_A(void)
{
    set_speed(SPEED_MIN,0);
}

void MotorA_start(void)
{
	motorA_PWM_Init(MAX_ARR_A-1,0);
    // set_speed(SPEED_MIN+20,SPEED_MIN+20);
    // delay_ms(20);
    // set_speed(SPEED_MIN,SPEED_MIN);
}

void motorA_test(void)
{
    // turn_left_A();
    // delay_ms(1000);
    // turn_right_A();
    // delay_ms(1000);
    // stop_forward();
    // return ;

    // start_forward();
    // delay_ms(1500);
    // delay_ms(1500);
    // stop_forward();
    // // return ;

    set_speed(0,0);
    delay_ms(500);

    set_speed(SPEED_MIN,SPEED_MIN);
    delay_ms(1000);
    set_speed(0,0);
    delay_ms(500);

    set_speed(-SPEED_MIN,0);
    delay_ms(1000);
    set_speed(0,0);
    delay_ms(500);

    set_speed(0,-SPEED_MIN);
    delay_ms(1000);
    set_speed(0,0);
    delay_ms(500);

    stop_forward();
    return ;

    set_speed(40,0);
    delay_ms(1000);
    stop_forward();

    set_speed(0,40);
    delay_ms(1000);
    stop_forward();
	return ;

}

// /* https://blog.csdn.net/qq_36958104/article/details/83661117  */
// void monitor_PWM_Init(u16 arr,u16 psc)
// {
//     GPIO_InitTypeDef     GPIO_InitStrue;
//     TIM_OCInitTypeDef     TIM_OCInitStrue;
//     TIM_TimeBaseInitTypeDef     TIM_TimeBaseInitStrue;

//     RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);        //使能TIM3和相关GPIO时钟
//     RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);// 使能GPIOB时钟(LED在BP5引脚),使能AFIO时钟(定时器3通道2需要重映射到BP5引脚)

// #if defined(TIM3_PartialRemap) && TIM3_PartialRemap
//     GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,ENABLE);
//     RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    
//     GPIO_InitStrue.GPIO_Pin=GPIO_Pin_5;     // TIM3_CH2
//     GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AF_PP;    // 复用推挽
//     GPIO_InitStrue.GPIO_Speed=GPIO_Speed_50MHz;    //设置最大输出速度
//     GPIO_Init(GPIOB,&GPIO_InitStrue);                //GPIO端口初始化设置

//     GPIO_InitStrue.GPIO_Pin=GPIO_Pin_4;     // TIM3_CH1
//     GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AF_PP;    // 复用推挽
//     GPIO_InitStrue.GPIO_Speed=GPIO_Speed_50MHz;    //设置最大输出速度
//     GPIO_Init(GPIOB,&GPIO_InitStrue);                //GPIO端口初始化设置
	
// // 如果PB0被初始化，则lcd不能正常显示，在LCD_Init()中有用到PB0，所以TIM3_CH2无法使用
//     // GPIO_InitStrue.GPIO_Pin=GPIO_Pin_0;     // TIM3_CH2
//     // GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AF_PP;    // 复用推挽
//     // GPIO_InitStrue.GPIO_Speed=GPIO_Speed_50MHz;    //设置最大输出速度
//     // GPIO_Init(GPIOB,&GPIO_InitStrue);                //GPIO端口初始化设置
// #endif

// #if defined(TIM3_FullRemap) && TIM3_FullRemap    
//     GPIO_PinRemapConfig(GPIO_FullRemap_TIM3,ENABLE);
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

//     GPIO_InitStrue.GPIO_Pin=GPIO_Pin_6;     // TIM3_CH1
//     GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AF_PP;    // 复用推挽
//     GPIO_InitStrue.GPIO_Speed=GPIO_Speed_50MHz;    //设置最大输出速度
//     GPIO_Init(GPIOC,&GPIO_InitStrue);                //GPIO端口初始化设置
	
//     GPIO_InitStrue.GPIO_Pin=GPIO_Pin_7;     // TIM3_CH2
//     GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AF_PP;    // 复用推挽
//     GPIO_InitStrue.GPIO_Speed=GPIO_Speed_50MHz;    //设置最大输出速度
//     GPIO_Init(GPIOC,&GPIO_InitStrue);                //GPIO端口初始化设置
// #endif

// #if defined(TIM4_Remap) && TIM4_Remap    
//     GPIO_PinRemapConfig(GPIO_Remap_TIM4,ENABLE);
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);

//     GPIO_InitStrue.GPIO_Pin=GPIO_Pin_12;     // TIM4_CH1
//     GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AF_PP;    // 复用推挽
//     GPIO_InitStrue.GPIO_Speed=GPIO_Speed_50MHz;    //设置最大输出速度
//     GPIO_Init(GPIOD,&GPIO_InitStrue);                //GPIO端口初始化设置
	
//     GPIO_InitStrue.GPIO_Pin=GPIO_Pin_13;     // TIM4_CH2
//     GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AF_PP;    // 复用推挽
//     GPIO_InitStrue.GPIO_Speed=GPIO_Speed_50MHz;    //设置最大输出速度
//     GPIO_Init(GPIOD,&GPIO_InitStrue);                //GPIO端口初始化设置
// #endif
    
//     TIM_TimeBaseInitStrue.TIM_Period=arr;    //设置自动重装载值
//     TIM_TimeBaseInitStrue.TIM_Prescaler=psc;        //预分频系数
//     TIM_TimeBaseInitStrue.TIM_CounterMode=TIM_CounterMode_Up;    //计数器向上溢出
//     TIM_TimeBaseInitStrue.TIM_ClockDivision=TIM_CKD_DIV1;        //时钟的分频因子，起到了一点点的延时作用，一般设为TIM_CKD_DIV1
//     TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStrue);        //TIM3初始化设置(设置PWM的周期)
    
//     TIM_OCInitStrue.TIM_OCMode=TIM_OCMode_PWM2;        // PWM模式2:CNT>CCR时输出有效
//     TIM_OCInitStrue.TIM_OCPolarity=TIM_OCPolarity_High;// 设置极性-有效为高电平
//     TIM_OCInitStrue.TIM_OutputState=TIM_OutputState_Enable;// 输出使能
// 	TIM_OC1Init(TIM3,&TIM_OCInitStrue);			// TIM3_CH1    PB4
//     TIM_OC2Init(TIM3,&TIM_OCInitStrue);        // TIM3_CH2    PB5
// 	//TIM3的通道2PWM 模式设置	PB5	 CH2	the right monitor
// 	// TIM_OC3Init(TIM3,&TIM_OCInitStrue);        //TIM3的通道3PWM 模式设置   PB0  CH3 the left monitor
 
//     TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable);        //使能预装载寄存器
//     TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Enable);        //使能预装载寄存器
// 	// TIM_OC3PreloadConfig(TIM3,TIM_OCPreload_Enable);        //使能预装载寄存器
    
//     TIM_Cmd(TIM3,ENABLE);        //使能TIM3
// }
