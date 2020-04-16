#ifndef __BSP_TIMEBASE_H
#define __BSP_TIMEBASE_H


#include "stm32f10x.h"


/********************������ʱ��TIM�������壬ֻ��TIM6��7************/
#define BASIC_TIM4 // ���ʹ��TIM7��ע�͵�����꼴��

#ifdef  BASIC_TIM4 // ʹ�û�����ʱ��TIM6
#define            BASIC_TIM                   TIM4
#define            BASIC_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            BASIC_TIM_CLK               RCC_APB1Periph_TIM4
#define            BASIC_TIM_Period            1000-1
#define            BASIC_TIM_Prescaler         71
#define            BASIC_TIM_IRQ               TIM4_IRQn
#define            BASIC_TIM_IRQHandler        TIM4_IRQHandler

/*#else  // ʹ�û�����ʱ��TIM7
#define            BASIC_TIM                   TIM7
#define            BASIC_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            BASIC_TIM_CLK               RCC_APB1Periph_TIM7
#define            BASIC_TIM_Period            1000-1
#define            BASIC_TIM_Prescaler         71
#define            BASIC_TIM_IRQ               TIM7_IRQn
#define            BASIC_TIM_IRQHandler        TIM7_IRQHandler*/

#endif
/**************************��������********************************/

void BASIC_TIM_Init(void);

void TimingDelay_Decrement(void);

void Delay_ms(__IO u32 nTime);



#endif	/* __BSP_TIMEBASE_H */


