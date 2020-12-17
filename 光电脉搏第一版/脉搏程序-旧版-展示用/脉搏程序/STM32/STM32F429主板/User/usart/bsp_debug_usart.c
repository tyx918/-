/**
  ******************************************************************************
  * @file    bsp_debug_usart.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   重定向c库printf函数到usart端口
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F429 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "./usart/bsp_debug_usart.h"


 /**
  * @brief  DEBUG_USART GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
  */
	int eventFlag =0;  //0:cmd   1:upload 2: download
	
void Debug_USART_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  RCC_AHB1PeriphClockCmd(DEBUG_USART2_RX_GPIO_CLK|DEBUG_USART2_TX_GPIO_CLK,ENABLE);

  /* 使能 USART 时钟 */
  RCC_APB1PeriphClockCmd(DEBUG_USART2_CLK, ENABLE);
  
  /* GPIO初始化 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  /* 配置Tx引脚为复用功能  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = DEBUG_USART2_TX_PIN  ;  
  GPIO_Init(DEBUG_USART2_TX_GPIO_PORT, &GPIO_InitStructure);

  /* 配置Rx引脚为复用功能 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = DEBUG_USART2_RX_PIN;
  GPIO_Init(DEBUG_USART2_RX_GPIO_PORT, &GPIO_InitStructure);
   
  GPIO_PinAFConfig(DEBUG_USART2_RX_GPIO_PORT,DEBUG_USART2_RX_SOURCE,DEBUG_USART2_RX_AF);	/* 连接 PXx 到 USARTx_Tx*/ 
  GPIO_PinAFConfig(DEBUG_USART2_TX_GPIO_PORT,DEBUG_USART2_TX_SOURCE,DEBUG_USART2_TX_AF);	/* 连接 PXx 到 USARTx__Rx*/
  
  /* 配置串DEBUG_USART 模式 */  
  USART_InitStructure.USART_BaudRate = DEBUG_USART2_BAUDRATE;		/* 波特率设置：DEBUG_USART_BAUDRATE */  
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;		/* 字长(数据位+校验位)：8 */  
  USART_InitStructure.USART_StopBits = USART_StopBits_1;				/* 停止位：1个停止位 */  
  USART_InitStructure.USART_Parity = USART_Parity_No;						/* 校验位选择：不使用校验 */  
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	/* 硬件流控制：不使用硬件流 */  
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;									/* USART模式控制：同时使能接收和发送 */  
  USART_Init(DEBUG_USART2, &USART_InitStructure); 																/* 完成USART初始化配置 */
	

	NVIC_InitTypeDef NVIC_InitStructure;  /* 初始化配置NVIC */	
		  
  NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART2_IRQ;		/* 配置USART为中断源 */  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	/* 抢断优先级为1 */  
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;				/* 子优先级为1 */  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;						/* 使能中断 */  
  NVIC_Init(&NVIC_InitStructure);		


	USART_ITConfig(DEBUG_USART2, USART_IT_RXNE, ENABLE);	/* 使能串口接收中断 */	
	
	
	
  USART_Cmd(DEBUG_USART2, ENABLE);											/* 使能串口 */
}

///重定向c库函数printf到串口DEBUG_USART，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到串口DEBUG_USART */
		USART_SendData(DEBUG_USART2, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(DEBUG_USART2, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

///重定向c库函数scanf到串口DEBUG_USART，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
		/* 等待串口输入数据 */
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
