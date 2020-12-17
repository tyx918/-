/**
  ******************************************************************************
  * @file    bsp_gpio.c
  * @author  
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   GPIO应用函数接口
  ******************************************************************************
  */
  
#include "./gpio/bsp_gpio.h"   

 /**
  * @brief  初始化控制的IO
  * @param  无
  * @retval 无
  */
void GPIO_Config(void)
{				
		GPIO_InitTypeDef GPIO_InitStructure;											/*定义一个GPIO_InitTypeDef类型的结构体*/
		RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOG, ENABLE); 	/*开启GPIO外设时钟*/

		//WIFI FB 1
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		/* 选择要控制的GPIO引脚   */													
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;   		/* 设置引脚模式为输入模式 */
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;	/* 设置引脚为不上拉也不下拉 */
		GPIO_Init(GPIOG, &GPIO_InitStructure);						/* 初始化GPIO */
        
	
		//WIFI FB 2
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;		/* 选择要控制的GPIO引脚   */													
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;   		/* 设置引脚模式为输入模式 */
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;	/* 设置引脚为不上拉也不下拉 */
		GPIO_Init(GPIOG, &GPIO_InitStructure);						/* 初始化GPIO */



		return ;
}
/*********************************************END OF FILE**********************/
