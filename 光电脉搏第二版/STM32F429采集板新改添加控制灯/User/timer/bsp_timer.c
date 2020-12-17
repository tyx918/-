#include "./timer/bsp_timer.h"
#include "./usart/bsp_debug_usart.h"
 

void TIM2_Configuration(void)
{
	
	TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure; 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	
  TIM_TimeBaseStructure.TIM_Period = 20-1;       		// 10KHZ/10 = 1KHZ	  
  TIM_TimeBaseStructure.TIM_Prescaler = 9000-1;       	//90M/9000 = 10KHZ

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	TIM_ClearFlag(TIM2, TIM_IT_Update);
 TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
//	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
  NVIC_InitTypeDef NVIC_InitStructure; 
    // 设置中断组为0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
		// 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; 	
		// 设置抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	  // 设置子优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

//	TIM_Cmd(TIM2, ENABLE);

}


void TIM2_IRQHandler(){
	
if ( TIM_GetITStatus( TIM2, TIM_IT_Update) != RESET ) 
	{	
		//printf("2");
		TIM_ClearITPendingBit(TIM2 , TIM_IT_Update); 
 		 
	}		 	

}
