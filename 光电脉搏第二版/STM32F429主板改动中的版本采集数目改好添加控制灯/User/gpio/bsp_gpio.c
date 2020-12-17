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
		RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOA, ENABLE); 	/*����GPIO����ʱ��*/
	  RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOH, ENABLE); 	/*����GPIO����ʱ��*/

	//AD_ARM
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;		/* ѡ��Ҫ���Ƶ�GPIO����   */													
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   		/* ��������ģʽΪ����ģʽ */
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	   /*�������ŵ��������Ϊ�������*/
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
		GPIO_Init(GPIOA, &GPIO_InitStructure);						/* ��ʼ��GPIO */
        
	//AD_ARM
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;		/* ѡ��Ҫ���Ƶ�GPIO����   */													
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   		/* ��������ģʽΪ����ģʽ */
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
		GPIO_Init(GPIOA, &GPIO_InitStructure);						/* ��ʼ��GPIO */

		//WIFI FB 1
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;		/* ѡ��Ҫ���Ƶ�GPIO����   */													
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;   		/* ��������ģʽΪ����ģʽ */
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* ��������Ϊ������Ҳ������ */
		GPIO_Init(GPIOH, &GPIO_InitStructure);						/* ��ʼ��GPIO */	
		
		//WIFI FB 2
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;		/* ѡ��Ҫ���Ƶ�GPIO����   */													
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   		/* ��������ģʽΪ����ģʽ */
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;	/* ��������Ϊ������Ҳ������ */
		GPIO_Init(GPIOH, &GPIO_InitStructure);						/* ��ʼ��GPIO */

		GPIO_ResetBits(GPIOA,GPIO_Pin_0);
		GPIO_ResetBits(GPIOA,GPIO_Pin_1);
		GPIO_SetBits(GPIOH,GPIO_Pin_14);



		return ;
}
/*********************************************END OF FILE**********************/
