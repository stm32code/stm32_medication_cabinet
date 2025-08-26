#ifndef __PWM_H
#define __PWM_H

#include "stm32f10x.h"
#include "git.h"

typedef struct
{
    U32 TIM1_Period;
    U32 TIM2_Period;
    U32 TIM3_Period;
    U32 TIM4_Period;
    U32 TIM5_Period;
    U32 TIM8_Period;

    U32 TIM1_Prescaler;
    U32 TIM2_Prescaler;
    U32 TIM3_Prescaler;
    U32 TIM4_Prescaler;
    U32 TIM5_Prescaler;
    U32 TIM8_Prescaler;
} xPeriod_xPrescaler;

typedef enum
{
    TIMX_1,
    TIMX_2,
    TIMX_3,
    TIMX_4,
    TIMX_5,
    TIMX_8,
} TimeX_Module;

/* TIM1--PWM连接的GPIO端口, 用户只需要修改下面的代码即可改变控制的PWM引脚 */
#define TIM1_GPIO_PORT_TIME1_2 GPIOA               /* GPIO端口 */
#define TIM1_GPIO_CLK_TIME1_2 RCC_APB2Periph_GPIOA /* GPIO端口时钟 */
#define TIM1_GPIO_PIN1 GPIO_Pin_8
/* TIM8--PWM连接的GPIO端口, 用户只需要修改下面的代码即可改变控制的PWM引脚 */
#define TIM8_GPIO_PORT_TIME2_3 GPIOB               /* GPIO端口 */
#define TIM8_GPIO_CLK_TIME2_3 RCC_APB2Periph_GPIOB /* GPIO端口时钟 */
#define TIM8_GPIO_PIN2 GPIO_Pin_0
#define TIM8_GPIO_PIN3 GPIO_Pin_1
/* TIM2--PWM连接的GPIO端口, 用户只需要修改下面的代码即可改变控制的PWM引脚 */
#define TIM2_GPIO_PORT_TIME3_4 GPIOA
#define TIM2_GPIO_PORT_TIME1_2 GPIOA                                     /* GPIO端口 */
#define TIM2_GPIO_CLK_TIME3_4 RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO /* GPIO端口时钟 */
#define TIM2_GPIO_PIN1 GPIO_Pin_0
#define TIM2_GPIO_PIN2 GPIO_Pin_1
#define TIM2_GPIO_PIN3 GPIO_Pin_2
#define TIM2_GPIO_PIN4 GPIO_Pin_3
/* TIM3--PWM连接的GPIO端口, 用户只需要修改下面的代码即可改变控制的PWM引脚 */
#define TIM3_GPIO_PORT_TIME1_2 GPIOA                                     /* GPIO端口 */
#define TIM3_GPIO_PORT_TIME3_4 GPIOB                                     /* GPIO端口 */
#define TIM3_GPIO_CLK_TIME1_2 RCC_APB2Periph_GPIOA                       /* GPIO端口时钟 */
#define TIM3_GPIO_CLK_TIME3_4 RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO /* GPIO端口时钟 */
#define TIM3_GPIO_PIN1 GPIO_Pin_6
#define TIM3_GPIO_PIN2 GPIO_Pin_7
#define TIM3_GPIO_PIN3 GPIO_Pin_0
#define TIM3_GPIO_PIN4 GPIO_Pin_1
/* TIM4--PWM连接的GPIO端口, 用户只需要修改下面的代码即可改变控制的PWM引脚 */
#define TIM4_GPIO_PORT_TIME1_2 GPIOB                                     /* GPIO端口 */
#define TIM4_GPIO_PORT_TIME3_4 GPIOB                                     /* GPIO端口 */
#define TIM4_GPIO_CLK_TIME1_2 RCC_APB2Periph_GPIOB                       /* GPIO端口时钟 */
#define TIM4_GPIO_CLK_TIME3_4 RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO /* GPIO端口时钟 */
#define TIM4_GPIO_PIN1 GPIO_Pin_6
#define TIM4_GPIO_PIN2 GPIO_Pin_7
#define TIM4_GPIO_PIN3 GPIO_Pin_8
#define TIM4_GPIO_PIN4 GPIO_Pin_9
/* TIM5--PWM连接的GPIO端口, 用户只需要修改下面的代码即可改变控制的PWM引脚 */
#define TIM5_GPIO_PORT_TIME1_2 GPIOA                                     /* GPIO端口 */
#define TIM5_GPIO_CLK_TIME1_2 RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO /* GPIO端口时钟 */
#define TIM5_GPIO_PIN1 GPIO_Pin_0
#define TIM5_GPIO_PIN2 GPIO_Pin_1

/*********************************************************************************
 * @Function	: TIM1 PWM输出初始化
 * @Input		: F :频率（HZ）
 * @Output		: None
 * @Return		: None
 * @Others		: None
 * @Date		: 2022-11-2
 **********************************************************************************/
void TIM1_PWM_Init(U32 f);
/*********************************************************************************
 * @Function	: TIM8 PWM输出初始化
 * @Input		: F :频率（HZ）
 * @Output		: None
 * @Return		: None
 * @Others		: None
 * @Date		: 2022-11-2
 **********************************************************************************/
void TIM8_PWM_Init(U32 f);
/*********************************************************************************
 * @Function	: TIM2 PWM输出初始化
 * @Input		: F :频率（HZ）
 * @Output		: None
 * @Return		: None
 * @Others		: None
 * @Date		: 2022-11-2
 **********************************************************************************/
void TIM2_PWM_Init(U32 f);
/*********************************************************************************
 * @Function	: TIM3 PWM输出初始化
 * @Input		: F :频率（HZ）
 * @Output		: None
 * @Return		: None
 * @Others		: None
 * @Date		: 2022-11-2
 **********************************************************************************/
void TIM3_PWM_Init(U32 f);
/*********************************************************************************
 * @Function	: TIM4 PWM输出初始化
 * @Input		: F :频率（HZ）
 * @Output		: None
 * @Return		: None
 * @Others		: None
 * @Date		: 2022-11-2
 **********************************************************************************/
void TIM4_PWM_Init(U32 f);
/*********************************************************************************
 * @Function	: TIM5 PWM输出初始化
 * @Input		: F :频率（HZ）
 * @Output		: None
 * @Return		: None
 * @Others		: None
 * @Date		: 2022-11-2
 **********************************************************************************/
void TIM5_PWM_Init(U32 f);
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
void Specific_Value(TimeX_Module TIMX, U8 PWM_Port, float Value);

#endif /* __PWM_H */
