/**
  ******************************************************************************
  * @file    bsp_debug_usart.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   �ض���c��printf������usart�˿�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 F429 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "./usart/bsp_debug_usart.h"


 /**
  * @brief  DEBUG_USART GPIO ����,����ģʽ���á�115200 8-N-1
  * @param  ��
  * @retval ��
  */
	int eventFlag =0;  //0:cmd   1:upload 2: download
	
void Debug_USART_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  RCC_AHB1PeriphClockCmd(DEBUG_USART2_RX_GPIO_CLK|DEBUG_USART2_TX_GPIO_CLK,ENABLE);

  /* ʹ�� USART ʱ�� */
  RCC_APB1PeriphClockCmd(DEBUG_USART2_CLK, ENABLE);
  
  /* GPIO��ʼ�� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  /* ����Tx����Ϊ���ù���  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = DEBUG_USART2_TX_PIN  ;  
  GPIO_Init(DEBUG_USART2_TX_GPIO_PORT, &GPIO_InitStructure);

  /* ����Rx����Ϊ���ù��� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = DEBUG_USART2_RX_PIN;
  GPIO_Init(DEBUG_USART2_RX_GPIO_PORT, &GPIO_InitStructure);
   
  GPIO_PinAFConfig(DEBUG_USART2_RX_GPIO_PORT,DEBUG_USART2_RX_SOURCE,DEBUG_USART2_RX_AF);	/* ���� PXx �� USARTx_Tx*/ 
  GPIO_PinAFConfig(DEBUG_USART2_TX_GPIO_PORT,DEBUG_USART2_TX_SOURCE,DEBUG_USART2_TX_AF);	/* ���� PXx �� USARTx__Rx*/
  
  /* ���ô�DEBUG_USART ģʽ */  
  USART_InitStructure.USART_BaudRate = DEBUG_USART2_BAUDRATE;		/* ���������ã�DEBUG_USART_BAUDRATE */  
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;		/* �ֳ�(����λ+У��λ)��8 */  
  USART_InitStructure.USART_StopBits = USART_StopBits_1;				/* ֹͣλ��1��ֹͣλ */  
  USART_InitStructure.USART_Parity = USART_Parity_No;						/* У��λѡ�񣺲�ʹ��У�� */  
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	/* Ӳ�������ƣ���ʹ��Ӳ���� */  
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;									/* USARTģʽ���ƣ�ͬʱʹ�ܽ��պͷ��� */  
  USART_Init(DEBUG_USART2, &USART_InitStructure); 																/* ���USART��ʼ������ */
	

	NVIC_InitTypeDef NVIC_InitStructure;  /* ��ʼ������NVIC */	
		  
  NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART2_IRQ;		/* ����USARTΪ�ж�Դ */  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	/* �������ȼ�Ϊ1 */  
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;				/* �����ȼ�Ϊ1 */  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;						/* ʹ���ж� */  
  NVIC_Init(&NVIC_InitStructure);		


	USART_ITConfig(DEBUG_USART2, USART_IT_RXNE, ENABLE);	/* ʹ�ܴ��ڽ����ж� */	
	
	
	
  USART_Cmd(DEBUG_USART2, ENABLE);											/* ʹ�ܴ��� */
}

///�ض���c�⺯��printf������DEBUG_USART���ض�����ʹ��printf����
int fputc(int ch, FILE *f)
{
		/* ����һ���ֽ����ݵ�����DEBUG_USART */
		USART_SendData(DEBUG_USART2, (uint8_t) ch);
		
		/* �ȴ�������� */
		while (USART_GetFlagStatus(DEBUG_USART2, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

///�ض���c�⺯��scanf������DEBUG_USART����д����ʹ��scanf��getchar�Ⱥ���
int fgetc(FILE *f)
{
		/* �ȴ������������� */
		while (USART_GetFlagStatus(DEBUG_USART2, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(DEBUG_USART2);
}



void DEBUG_USART2_IRQHandler(void)
{
  uint8_t ucTemp;
	static uint8_t tempBuff[100];
	static int i =0;
	
	if(USART_GetITStatus(DEBUG_USART2,USART_IT_RXNE)!=RESET)
	{		
		ucTemp = USART_ReceiveData( DEBUG_USART2 );
		if(ucTemp == '1')
		{
			eventFlag =1;
		
		}
		
		if(ucTemp == '2'){
			
			eventFlag =2;


		
		}

	}	 
}	
/*********************************************END OF FILE**********************/
