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
#include "stdarg.h"	
#include "stm32f4xx_it.h"
#include "string.h"	



 /**
  * @brief  DEBUG_USART GPIO ����,����ģʽ���á�115200 8-N-1
  * @param  ��
  * @retval ��
  */
	
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);						/* Ƕ�������жϿ�������ѡ�� */ 

	//���� ����2
  NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART2_IRQ;		/* ����USARTΪ�ж�Դ */  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	/* �������ȼ�Ϊ1 */  
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;				/* �����ȼ�Ϊ1 */  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;						/* ʹ���ж� */  
  NVIC_Init(&NVIC_InitStructure);														/* ��ʼ������NVIC */

  //433 ����6
  NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART6_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}	

int eventFlag =0;  //0:cmd   1:upload 2: download
//����2
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
	

//	NVIC_InitTypeDef NVIC_InitStructure;  /* ��ʼ������NVIC */	
//		  
//  NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART2_IRQ;		/* ����USARTΪ�ж�Դ */  
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	/* �������ȼ�Ϊ1 */  
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;				/* �����ȼ�Ϊ1 */  
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;						/* ʹ���ж� */  
//  NVIC_Init(&NVIC_InitStructure);		


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


//****************����6****************************
static void USART6_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  RCC_AHB1PeriphClockCmd(DEBUG_USART6_RX_GPIO_CLK|DEBUG_USART6_TX_GPIO_CLK,ENABLE);

  /* ʹ�� USART ʱ�� */
  RCC_APB2PeriphClockCmd(DEBUG_USART6_CLK, ENABLE);
  
  /* GPIO��ʼ�� */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  /* ����Tx����Ϊ���ù���  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = DEBUG_USART6_TX_PIN  ;  
  GPIO_Init(DEBUG_USART6_TX_GPIO_PORT, &GPIO_InitStructure);

  /* ����Rx����Ϊ���ù��� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = DEBUG_USART6_RX_PIN;
  GPIO_Init(DEBUG_USART6_RX_GPIO_PORT, &GPIO_InitStructure);
  
 /* ���� PXx �� USARTx_Tx*/
  GPIO_PinAFConfig(DEBUG_USART6_RX_GPIO_PORT,DEBUG_USART6_RX_SOURCE,DEBUG_USART6_RX_AF);

  /*  ���� PXx �� USARTx__Rx*/
  GPIO_PinAFConfig(DEBUG_USART6_TX_GPIO_PORT,DEBUG_USART6_TX_SOURCE,DEBUG_USART6_TX_AF);
  
  /* ���ô�DEBUG_USART ģʽ */  
  USART_InitStructure.USART_BaudRate = DEBUG_USART6_BAUDRATE;	/* ���������ã�DEBUG_USART_BAUDRATE */  
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;	/* �ֳ�(����λ+У��λ)��8 */  
  USART_InitStructure.USART_StopBits = USART_StopBits_1;			/* ֹͣλ��1��ֹͣλ */  
  USART_InitStructure.USART_Parity = USART_Parity_No;					/* У��λѡ�񣺲�ʹ��У�� */  
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	/* Ӳ�������ƣ���ʹ��Ӳ���� */  
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;									/* USARTģʽ���ƣ�ͬʱʹ�ܽ��պͷ��� */  
  USART_Init(DEBUG_USART6, &USART_InitStructure); 																/* ���USART��ʼ������ */
	  	
	USART_ITConfig(DEBUG_USART6, USART_IT_IDLE, ENABLE);	/* ʹ�ܴ��ڿ����ж� */  
	USART_ITConfig(DEBUG_USART6, USART_IT_RXNE, ENABLE);	/* ʹ�ܴ��ڽ����ж� */ 
  USART_Cmd(DEBUG_USART6, ENABLE);											/* ʹ�ܴ��� */
}


void printf6(char* fmt,...)
{  
	u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	memset(USART6_TX_BUF,0,sizeof(USART6_TX_BUF));
	vsprintf((char*)USART6_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART6_TX_BUF);
	USART_GetFlagStatus(USART6, USART_FLAG_TC);
	for(j=0;j<i;j++)
	{	
		USART_SendData(USART6,USART6_TX_BUF[j]); 		
		while(USART_GetFlagStatus(USART6,USART_FLAG_TC)==RESET);
//		printf("%x\r\n", USART6_TX_BUF[j]);
	}
}



//���ڳ�ʼ��
void USART_Config(void)
{
	NVIC_Configuration();
	Debug_USART_Config();
	//DEBUG_USART_Config();	//���Դ���
	USART6_Config();   //433
}


/*****************  ����һ���ַ� **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* ����һ���ֽ����ݵ�USART */
	USART_SendData(pUSARTx,ch);
		
	/* �ȴ��������ݼĴ���Ϊ�� */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/*****************  �����ַ��� **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* �ȴ�������� */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}
}

/*****************  ����һ��16λ�� **********************/
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	/* ȡ���߰�λ */
	temp_h = (ch&0XFF00)>>8;
	/* ȡ���Ͱ�λ */
	temp_l = ch&0XFF;
	
	/* ���͸߰�λ */
	USART_SendData(pUSARTx,temp_h);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
	/* ���͵Ͱ�λ */
	USART_SendData(pUSARTx,temp_l);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}


/*********************************************END OF FILE**********************/
