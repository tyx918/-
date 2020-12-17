/**
  ******************************************************************************
  * @file    bsp_gpio.c
  * @author  
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   GPIOӦ�ú����ӿ�
  ******************************************************************************
  */
  
#include "./gpio/bsp_gpio.h"   

 /**
  * @brief  ��ʼ�����Ƶ�IO
  * @param  ��
  * @retval ��
  */
void GPIO_Config(void)
{				
		GPIO_InitTypeDef GPIO_InitStructure;											/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
		RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOG, ENABLE); 	/*����GPIO����ʱ��*/

		//WIFI FB 1
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		/* ѡ��Ҫ���Ƶ�GPIO����   */													
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;   		/* ��������ģʽΪ����ģʽ */
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;	/* ��������Ϊ������Ҳ������ */
		GPIO_Init(GPIOG, &GPIO_InitStructure);						/* ��ʼ��GPIO */
        
	
		//WIFI FB 2
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;		/* ѡ��Ҫ���Ƶ�GPIO����   */													
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;   		/* ��������ģʽΪ����ģʽ */
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;	/* ��������Ϊ������Ҳ������ */
		GPIO_Init(GPIOG, &GPIO_InitStructure);						/* ��ʼ��GPIO */



		return ;
}
/*********************************************END OF FILE**********************/
