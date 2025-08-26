#include "pwm.h"
#include "math.h"
#include "delay.h"
#include "timer.h"
#include "usart.h"
// 全局变量
extern U8 time_6; // ms 计时变量
// 局部变量
xPeriod_xPrescaler xperiod_xprescaler;

/*********************************************************************************
 * @Function	: 计算分频率
 * @Input		: F :频率（HZ）
 *                TIMX :TIM选择
 * @Output		: None
 * @Return		: None
 * @Others		: None
 * @Date		: 2022-11-2
 **********************************************************************************/
static void Math_Cycle(TimeX_Module TIMX, U32 F, xPeriod_xPrescaler *Data)
{
    // 计算最合适的分频和重装值-------------------------------------------
    float midFloat, clkFloat;
    long clkInt;
    long midInt;
    clkFloat = 72000000.0f / F;
    if (clkFloat - (long)clkFloat >= 0.5f)
        clkInt = clkFloat + 1;
    else
        clkInt = (long)clkFloat;

    midFloat = sqrt(clkFloat); // 开方
    if (midFloat - (long)midFloat >= 0.5f)
        midInt = (long)midFloat + 1;
    else
        midInt = (long)midFloat;
    // 找一组最接近的
    for (int i = midInt; i >= 1; i--)
    {
        if (clkInt % i == 0)
        {
            switch (TIMX)
            {
            case TIMX_1:
                Data->TIM1_Period = i - 1;
                Data->TIM1_Prescaler = clkInt / i + 1;
                break;
            case TIMX_2:
                Data->TIM2_Period = i - 1;
                Data->TIM2_Prescaler = clkInt / i + 1;
                break;
            case TIMX_3:
                Data->TIM3_Period = i - 1;
                Data->TIM3_Prescaler = clkInt / i + 1;
                break;
            case TIMX_4:
                Data->TIM4_Period = i - 1;
                Data->TIM4_Prescaler = clkInt / i + 1;
                break;
            case TIMX_5:
                Data->TIM5_Period = i - 1;
                Data->TIM5_Prescaler = clkInt / i + 1;
                break;
            case TIMX_8:
                Data->TIM8_Period = i - 1;
                Data->TIM8_Prescaler = clkInt / i + 1;
                break;
            default:
                break;
            }
            break;
        }
    }
}
/*********************************************************************************
 * @Function	: TIM1 PWM输出初始化
 * @Input		: F :频率（HZ）
 * @Output		: None
 * @Return		: None
 * @Others		: None
 * @Date		: 2022-11-2
 **********************************************************************************/
void TIM1_PWM_Init(U32 f)
{
    Math_Cycle(TIMX_1, f, &xperiod_xprescaler); // 计算合适的PWM

    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    RCC_APB2PeriphClockCmd(TIM1_GPIO_CLK_TIME1_2, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(TIM1_GPIO_PORT_TIME1_2, &GPIO_InitStructure);

    TIM_DeInit(TIM1);                               // 定时器1初始化
    TIM_InternalClockConfig(TIM1);                  /*时钟选择*/
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); // 初始化定时器
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = xperiod_xprescaler.TIM1_Period - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = xperiod_xprescaler.TIM1_Prescaler - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    /*定时器通道1的比较值，输出极性，定时器的工作模式*/
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;            // 配置为PWM2模式，TIMx_CNT>TIMx_CCR1，为有效电平
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset; // 空闲时输出电平为低
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    // 当计数器小于TIM_Pulse的值，输出为高
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;

    // 初始化TIM1 Channel 1 PWM
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               // 配置为PWM2模式，TIMx_CNT>TIMx_CCR0，为有效电平
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;  // 空闲时1N也输出高电平
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;     // 定时器触发之后，输出低电平
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; // 使能输出
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);

    // TIM_CtrlPWMOutputs(TIM1, ENABLE);
    TIM_ARRPreloadConfig(TIM1, ENABLE); // 使能自动重装载寄存器，arr的预装载
    // TIM_Cmd(TIM1, ENABLE);
}
/*********************************************************************************
 * @Function	: TIM8 PWM输出初始化
 * @Input		: F :频率（HZ）
 * @Output		: None
 * @Return		: None
 * @Others		: None
 * @Date		: 2022-11-2
 **********************************************************************************/
void TIM8_PWM_Init(U32 f)
{
    Math_Cycle(TIMX_8, f, &xperiod_xprescaler); // 计算合适的PWM

    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
    RCC_APB2PeriphClockCmd(TIM8_GPIO_CLK_TIME2_3, ENABLE);

    // 设置PA0和PA1为复用输出功能，输出TIM5 CH1和TIM5 CH2的PWM脉冲波形
    GPIO_InitStructure.GPIO_Pin = TIM8_GPIO_PIN3 | TIM8_GPIO_PIN2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TIM8_GPIO_PORT_TIME2_3, &GPIO_InitStructure);

    GPIO_WriteBit(TIM8_GPIO_PORT_TIME2_3, TIM8_GPIO_PIN2 | TIM8_GPIO_PIN3, Bit_RESET); // 上电的时候设置为0，防止上电瞬间为高

    TIM_DeInit(TIM8);                               // 定时器1初始化
    TIM_InternalClockConfig(TIM8);                  /*时钟选择*/
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); // 初始化定时器
    /*基础定时器配置，主要是定时器分频和定时器周期*/
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;                      // 时钟分频设置
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;                  // 技术模式为向上技术
    TIM_TimeBaseStructure.TIM_Period = xperiod_xprescaler.TIM8_Period - 1;       // 计数周期范围0000~0XFFFF，计数周期为72,输出PWM的频率为1M
    TIM_TimeBaseStructure.TIM_Prescaler = xperiod_xprescaler.TIM8_Prescaler - 1; // 对clk时钟进行(0+1)分频,结果是1分频，clk=72M/(0+1)=72M
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);                              // 初始化基础定时器

    /*定时器通道1的比较值，输出极性，定时器的工作模式*/
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;            // 配置为PWM2模式，TIMx_CNT>TIMx_CCR1，为有效电平
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset; // 空闲时输出电平为低
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    // 当计数器小于TIM_Pulse的值，输出为高
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    // TIM_OCInitStructure.TIM_Pulse = Period / 2; //计数器比较值，占空比为50%

    // 初始化TIM8 Channel 2 PWM
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;               // 配置为PWM2模式，TIMx_CNT>TIMx_CCR1，为有效电平
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;   // 空闲时1N也输出底电平
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;      // 定时器触发之后，输出低电平
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; // 使能输出
    TIM_OC2Init(TIM8, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);

    // 初始化TIM8 Channel 3 PWM
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;               // 配置为PWM2模式，TIMx_CNT>TIMx_CCR1，为有效电平
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;   // 空闲时1N也输出高电平
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;      // 定时器触发之后，输出低电平
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; // 使能输出
    TIM_OC3Init(TIM8, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);

    TIM_CtrlPWMOutputs(TIM8, ENABLE);
    TIM_ARRPreloadConfig(TIM8, ENABLE); // 使能自动重装载寄存器，arr的预装载
    TIM_Cmd(TIM8, ENABLE);              // 使能TIM8
}
/*********************************************************************************
 * @Function	: TIM2 PWM输出初始化
 * @Input		: F :频率（HZ）
 * @Output		: None
 * @Return		: None
 * @Others		: None
 * @Date		: 2022-11-2
 **********************************************************************************/
void TIM2_PWM_Init(U32 f)
{
    Math_Cycle(TIMX_2, f, &xperiod_xprescaler); // 计算合适的PWM

    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(TIM2_GPIO_CLK_TIME3_4, ENABLE);
    //  GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE); // 这句话是正点原子官方例程“PWM输出实验”中给的，目的是Timer3完全重映射，将TIM3_CH4->PC9。本实验不用重映像
    //  GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //这句话是正点原子官方例程“PWM输出实验”中给的，目的是Timer3部分重映射，将TIM3_CH2->PB5。本实验不用重映像

    // 设置PC6和PC7为复用输出功能，输出TIM3 CH1和TIM3 CH2的PWM脉冲波形
    GPIO_InitStructure.GPIO_Pin = TIM2_GPIO_PIN1 | TIM2_GPIO_PIN2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TIM2_GPIO_PORT_TIME1_2, &GPIO_InitStructure);

    // 设置PC8和PC9为复用输出功能，输出TIM3 CH3和TIM3 CH4的PWM脉冲波形
    GPIO_InitStructure.GPIO_Pin = TIM2_GPIO_PIN3 | TIM2_GPIO_PIN4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TIM2_GPIO_PORT_TIME3_4, &GPIO_InitStructure);

    TIM_DeInit(TIM3); // 定时器1初始化
    /*--------------------时基结构体初始化-------------------------*/
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    // 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
    TIM_TimeBaseStructure.TIM_Period = xperiod_xprescaler.TIM2_Period - 1;
    // 驱动CNT计数器的时钟 = Fck_int/(psc+1)
    TIM_TimeBaseStructure.TIM_Prescaler = xperiod_xprescaler.TIM2_Prescaler - 1;
    // 时钟分频因子 ，配置死区时间时需要用到
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    // 计数器计数模式，设置为向上计数
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    // 重复计数器的值，没用到不用管
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    // 初始化定时器
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    /*--------------------输出比较结构体初始化-------------------*/
    TIM_OCInitTypeDef TIM_OCInitStructure;
    // 配置为PWM模式1
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    // 输出使能
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    // 空闲时输出电平为低
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    // 输出通道电平极性配置
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_Pulse = xperiod_xprescaler.TIM2_Period / 2; // 计数器比较值，占空比为50%

    // 初始化TIM3 Channel 1 PWM
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               // 配置为PWM2模式，TIMx_CNT>TIMx_CCR1，为有效电平
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;  // 空闲时1N也输出底电平
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;      // 定时器触发之后，输出低电平
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; // 使能输出
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);                        // 输出比较通道 1
    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);

    // 初始化TIM3 Channel 2 PWM
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               // 配置为PWM2模式，TIMx_CNT>TIMx_CCR1，为有效电平
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;  // 空闲时1N也输出高电平
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;      // 定时器触发之后，输出低电平
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; // 使能输出
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);                        // 输出比较通道 2
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);

    // 初始化TIM3 Channel 3 PWM
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               // 配置为PWM2模式，TIMx_CNT>TIMx_CCR1，为有效电平
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;  // 空闲时1N也输出底电平
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;      // 定时器触发之后，输出低电平
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; // 使能输出
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);                        // 输出比较通道 3
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);

    // 初始化TIM3 Channel 4 PWM
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               // 配置为PWM2模式，TIMx_CNT>TIMx_CCR1，为有效电平
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;  // 空闲时1N也输出高电平
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;      // 定时器触发之后，输出低电平
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; // 使能输出
    TIM_OC4Init(TIM2, &TIM_OCInitStructure);                        // 输出比较通道 4
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);

    TIM_CtrlPWMOutputs(TIM2, ENABLE);
    TIM_ARRPreloadConfig(TIM2, ENABLE); // 使能自动重装载寄存器，arr的预装载
    TIM_Cmd(TIM2, ENABLE);
}
/*********************************************************************************
 * @Function	: TIM3 PWM输出初始化
 * @Input		: F :频率（HZ）
 * @Output		: None
 * @Return		: None
 * @Others		: None
 * @Date		: 2022-11-2
 **********************************************************************************/
void TIM3_PWM_Init(U32 f)
{
    Math_Cycle(TIMX_3, f, &xperiod_xprescaler); // 计算合适的PWM

    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(TIM3_GPIO_CLK_TIME3_4 | TIM3_GPIO_CLK_TIME1_2, ENABLE);
    //  GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE); // 这句话是正点原子官方例程“PWM输出实验”中给的，目的是Timer3完全重映射，将TIM3_CH4->PC9。本实验不用重映像
    //  GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //这句话是正点原子官方例程“PWM输出实验”中给的，目的是Timer3部分重映射，将TIM3_CH2->PB5。本实验不用重映像

    // 设置PC6和PC7为复用输出功能，输出TIM3 CH1和TIM3 CH2的PWM脉冲波形
    GPIO_InitStructure.GPIO_Pin = TIM3_GPIO_PIN1 | TIM3_GPIO_PIN2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TIM3_GPIO_PORT_TIME1_2, &GPIO_InitStructure);

    // 设置PC8和PC9为复用输出功能，输出TIM3 CH3和TIM3 CH4的PWM脉冲波形
    GPIO_InitStructure.GPIO_Pin = TIM3_GPIO_PIN4 | TIM3_GPIO_PIN3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TIM3_GPIO_PORT_TIME3_4, &GPIO_InitStructure);

    TIM_DeInit(TIM3); // 定时器1初始化
    /*--------------------时基结构体初始化-------------------------*/
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    // 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
    TIM_TimeBaseStructure.TIM_Period = xperiod_xprescaler.TIM3_Period - 1;
    // 驱动CNT计数器的时钟 = Fck_int/(psc+1)
    TIM_TimeBaseStructure.TIM_Prescaler = xperiod_xprescaler.TIM3_Prescaler - 1;
    // 时钟分频因子 ，配置死区时间时需要用到
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    // 计数器计数模式，设置为向上计数
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    // 重复计数器的值，没用到不用管
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    // 初始化定时器
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    /*--------------------输出比较结构体初始化-------------------*/
    TIM_OCInitTypeDef TIM_OCInitStructure;
    // 配置为PWM模式1
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    // 输出使能
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    // 空闲时输出电平为低
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    // 输出通道电平极性配置
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_Pulse = xperiod_xprescaler.TIM3_Period / 2; // 计数器比较值，占空比为50%

    // 初始化TIM3 Channel 1 PWM
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               // 配置为PWM2模式，TIMx_CNT>TIMx_CCR1，为有效电平
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;  // 空闲时1N也输出底电平
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;      // 定时器触发之后，输出低电平
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; // 使能输出
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);                        // 输出比较通道 1
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

    // // 初始化TIM3 Channel 2 PWM
    // TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               // 配置为PWM2模式，TIMx_CNT>TIMx_CCR1，为有效电平
    // TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;  // 空闲时1N也输出高电平
    // TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;      // 定时器触发之后，输出低电平
    // TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; // 使能输出
    // TIM_OC2Init(TIM3, &TIM_OCInitStructure);                        // 输出比较通道 2
    // TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

    // // 初始化TIM3 Channel 3 PWM
    // TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               // 配置为PWM2模式，TIMx_CNT>TIMx_CCR1，为有效电平
    // TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;  // 空闲时1N也输出底电平
    // TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;      // 定时器触发之后，输出低电平
    // TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; // 使能输出
    // TIM_OC3Init(TIM3, &TIM_OCInitStructure);                        // 输出比较通道 3
    // TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

    // // 初始化TIM3 Channel 4 PWM
    // TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               // 配置为PWM2模式，TIMx_CNT>TIMx_CCR1，为有效电平
    // TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;  // 空闲时1N也输出高电平
    // TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;      // 定时器触发之后，输出低电平
    // TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; // 使能输出
    // TIM_OC4Init(TIM3, &TIM_OCInitStructure);                        // 输出比较通道 4
    // TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);

    TIM_CtrlPWMOutputs(TIM3, ENABLE);
    TIM_ARRPreloadConfig(TIM3, ENABLE); // 使能自动重装载寄存器，arr的预装载
    TIM_Cmd(TIM3, ENABLE);
}
/*********************************************************************************
 * @Function	: TIM4 PWM输出初始化
 * @Input		: F :频率（HZ）
 * @Output		: None
 * @Return		: None
 * @Others		: None
 * @Date		: 2022-11-2
 **********************************************************************************/
void TIM4_PWM_Init(U32 f)
{
    Math_Cycle(TIMX_4, f, &xperiod_xprescaler); // 计算合适的PWM

    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    RCC_APB2PeriphClockCmd(TIM2_GPIO_CLK_TIME3_4, ENABLE);
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE); // 这句话是正点原子官方例程“PWM输出实验”中给的，目的是Timer3完全重映射，将TIM3_CH4->PC9。本实验不用重映像

    // 设置PA0和PA1为复用输出功能，输出TIM5 CH1和TIM5 CH2的PWM脉冲波形
    GPIO_InitStructure.GPIO_Pin = TIM4_GPIO_PIN3 | TIM4_GPIO_PIN4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TIM4_GPIO_PORT_TIME3_4, &GPIO_InitStructure);

    GPIO_WriteBit(TIM4_GPIO_PORT_TIME3_4, TIM4_GPIO_PIN3 | TIM4_GPIO_PIN4, Bit_RESET); // 上电的时候设置为0，防止上电瞬间为高

    TIM_DeInit(TIM4); // 定时器1初始化
    /*--------------------时基结构体初始化-------------------------*/
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    // 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
    TIM_TimeBaseStructure.TIM_Period = xperiod_xprescaler.TIM4_Period - 1;
    // 驱动CNT计数器的时钟 = Fck_int/(psc+1)
    TIM_TimeBaseStructure.TIM_Prescaler = xperiod_xprescaler.TIM4_Prescaler - 1;
    // 时钟分频因子 ，配置死区时间时需要用到
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    // 计数器计数模式，设置为向上计数
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    // 重复计数器的值，没用到不用管
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    // 初始化定时器
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    /*--------------------输出比较结构体初始化-------------------*/
    TIM_OCInitTypeDef TIM_OCInitStructure;
    // 配置为PWM模式1
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    // 输出使能
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    // 输出通道电平极性配置
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    // 空闲时输出电平为低
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;

    // 输出比较通道 3
    TIM_OC3Init(TIM4, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
    // 输出比较通道 4
    TIM_OC4Init(TIM4, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM4, ENABLE); // 使能自动重装载寄存器，arr的预装载
}
/*********************************************************************************
 * @Function	: TIM5 PWM输出初始化
 * @Input		: F :频率（HZ）
 * @Output		: None
 * @Return		: None
 * @Others		: None
 * @Date		: 2022-11-2
 **********************************************************************************/
void TIM5_PWM_Init(U32 f)
{
    Math_Cycle(TIMX_5, f, &xperiod_xprescaler); // 计算合适的PWM

    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
    RCC_APB2PeriphClockCmd(TIM5_GPIO_CLK_TIME1_2, ENABLE);
    // GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE); //这句话是正点原子官方例程“PWM输出实验”中给的，目的是Timer3完全重映射，将TIM3_CH4->PC9。本实验不用重映像
    // GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //这句话是正点原子官方例程“PWM输出实验”中给的，目的是Timer3部分重映射，将TIM3_CH2->PB5。本实验不用重映像

    // 设置PA0和PA1为复用输出功能，输出TIM5 CH1和TIM5 CH2的PWM脉冲波形
    GPIO_InitStructure.GPIO_Pin = TIM5_GPIO_PIN1 | TIM5_GPIO_PIN2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TIM5_GPIO_PORT_TIME1_2, &GPIO_InitStructure);

    GPIO_WriteBit(TIM5_GPIO_PORT_TIME1_2, TIM5_GPIO_PIN1 | TIM5_GPIO_PIN2, Bit_RESET); // 上电的时候设置为0，防止上电瞬间为高

    TIM_DeInit(TIM5);                               // 定时器1初始化
    TIM_InternalClockConfig(TIM5);                  /*时钟选择*/
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); // 初始化定时器
    /*基础定时器配置，主要是定时器分频和定时器周期*/
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;                      // 时钟分频设置
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;                  // 技术模式为向上技术
    TIM_TimeBaseStructure.TIM_Period = xperiod_xprescaler.TIM5_Period - 1;       // 计数周期范围0000~0XFFFF，计数周期为72,输出PWM的频率为1M
    TIM_TimeBaseStructure.TIM_Prescaler = xperiod_xprescaler.TIM5_Prescaler - 1; // 对clk时钟进行(0+1)分频,结果是1分频，clk=72M/(0+1)=72M
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);                              // 初始化基础定时器

    /*定时器通道1的比较值，输出极性，定时器的工作模式*/
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;            // 配置为PWM2模式，TIMx_CNT>TIMx_CCR1，为有效电平
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset; // 空闲时输出电平为低
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    // 当计数器小于TIM_Pulse的值，输出为高
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    // TIM_OCInitStructure.TIM_Pulse = Period / 2; //计数器比较值，占空比为50%

    // 初始化TIM5 Channel 1 PWM
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;               // 配置为PWM2模式，TIMx_CNT>TIMx_CCR1，为有效电平
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;    // 空闲时1N也输出底电平
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;      // 定时器触发之后，输出低电平
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; // 使能输出
    TIM_OC1Init(TIM5, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM5, TIM_OCPreload_Enable);

    // 初始化TIM5 Channel 2 PWM
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;               // 配置为PWM2模式，TIMx_CNT>TIMx_CCR1，为有效电平
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;    // 空闲时1N也输出高电平
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;      // 定时器触发之后，输出低电平
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; // 使能输出
    TIM_OC2Init(TIM5, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Enable);

    // TIM_CtrlPWMOutputs(TIM5, ENABLE);
    TIM_ARRPreloadConfig(TIM5, ENABLE); // 使能自动重装载寄存器，arr的预装载
    // TIM_Cmd(TIM5, ENABLE);              //使能TIM5
}
/*********************************************************************************
 * @Function	: PWM指定输出
 * @Input		: PWM_Port 通道参数修改
 *                Value 占空比比值
 *                TIMx TIM选择
 * @Output		: None
 * @Return		: None
 * @Others		: 用户可以直接规定PWM占空比
 * @Date		: 2022-11-2
 **********************************************************************************/
void Specific_Value(TimeX_Module TIMX, U8 PWM_Port, float Value)
{
    U16 num = 0;
    TIM_TypeDef *TIMx;
    switch (TIMX)
    {
    case TIMX_1:
        num = xperiod_xprescaler.TIM1_Period * Value / 100;
        TIMx = TIM1;
        break;
    case TIMX_2:
        num = xperiod_xprescaler.TIM2_Period * Value / 100;
        TIMx = TIM2;
        break;
    case TIMX_3:
        num = xperiod_xprescaler.TIM3_Period * Value / 100;
        TIMx = TIM3;
        break;
    case TIMX_4:
        num = xperiod_xprescaler.TIM4_Period * Value / 100;
        TIMx = TIM4;
        break;
    case TIMX_5:
        num = xperiod_xprescaler.TIM5_Period * Value / 100;
        TIMx = TIM5;
        break;
    case TIMX_8:
        num = xperiod_xprescaler.TIM8_Period * Value / 100;
        TIMx = TIM8;
        break;
    default:
        break;
    }

    switch (PWM_Port)
    {
    case 1:
        TIM_SetCompare1(TIMx, num); // 修改比较值，修改占空比
        break;
    case 2:
        TIM_SetCompare2(TIMx, num); // 修改比较值，修改占空比
        break;
    case 3:
        TIM_SetCompare3(TIMx, num); // 修改比较值，修改占空比
        break;
    case 4:
        TIM_SetCompare4(TIMx, num); // 修改比较值，修改占空比
        break;
    }
}
