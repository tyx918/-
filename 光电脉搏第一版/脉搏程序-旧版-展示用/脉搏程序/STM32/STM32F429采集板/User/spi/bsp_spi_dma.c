/***********************************
 *
 *			bsp_spi_dma.c  
 *���� spi_dma ȫ˫��ͨѶ����Ҫ����
 *
 **********************************/

#include "./spi/bsp_spi_dma.h"
#include <string.h>
#include "usart/bsp_debug_usart.h"
#include "./adc/bsp_adc.h"

uint8_t TX_Buff[BuffSize];
uint8_t RX_Buff[BuffSize];

 /*****************************************
  * @brief  ��ʼ��SPI ��Ҫע�� spi��Ƶ��
  * @param  ��
  * @retval ��
  *****************************************/
static void SPI_Config(void)
{
	GPIO_InitTypeDef GPIO_uInitStructure;
	SPI_InitTypeDef SPI_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3,ENABLE);
	
	
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_SPI3);//MISO
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SPI3);//MOSI
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SPI3);//SCK
	
	//SCK
	GPIO_uInitStructure.GPIO_Pin = GPIO_Pin_10 ;
	GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_uInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_uInitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOC,&GPIO_uInitStructure);
	
	//MISO
	GPIO_uInitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_uInitStructure.GPIO_OType = GPIO_OType_PP;  
	GPIO_uInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_uInitStructure);
	
	//MOSI  
	GPIO_uInitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_uInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_uInitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOC,&GPIO_uInitStructure);
	
	
	//CS 
		GPIO_uInitStructure.GPIO_Pin = GPIO_Pin_15;		/* ѡ��Ҫ���Ƶ�GPIO����   */													
		GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_OUT;   	
		  GPIO_uInitStructure.GPIO_OType = GPIO_OType_PP;	
  GPIO_uInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_uInitStructure);			
	
	
	SPI_Cmd(SPI3,DISABLE); 
	
	//SPI��ʼ��
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex ;  
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                                                                                                                                                  
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;  
	SPI_InitStructure.SPI_CPOL = 0x00;                                                    
	SPI_InitStructure.SPI_CPHA = 0x00;   
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;  //adc�������������£���Ҫspi�ϸߵ�ʱ�ӣ���Ȼ�ᵼ��adc���������ݳ���
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  
	SPI_InitStructure.SPI_CRCPolynomial = 7; 
	SPI_Init(SPI3,&SPI_InitStructure);
	SPI_Cmd(SPI3,ENABLE);   
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_15);

}



//----------------------------DMA �ĳ���--------------------------------//
/****************************************
  * @brief  SPI ����DMA��ʼ�� 
  * @param              
  * @retval             
  ***************************************/
static void SPI3_TX_DMA_Config(void)
{
			DMA_InitTypeDef DMA_InitStructure;  		//DMA�ṹ��
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);	/* ʹ��DMAʱ�� */  		
			DMA_DeInit(DMA1_Stream5);								/* ��λ��ʼ��DMA������ */  
			while (DMA_GetCmdStatus(DMA1_Stream5) != DISABLE);	/* ȷ��DMA��������λ��� */  
			/* ���� DMA Stream */
			DMA_InitStructure.DMA_Channel = DMA_Channel_0;  					
			DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) (&(SPI3->DR));;					/* �����ַ */  
			DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)TX_Buff;							/* �ڴ��ַ(Ҫ����ı�����ָ��) ,DMA�洢��0��ַ*/  
			DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;									/* ���򣺴洢��������ģʽ */		 		  
			DMA_InitStructure.DMA_BufferSize = BuffSize;							/* ���ݴ����� ,������Ϊ0�� ʵ�ʷ���ʱ����������*/	  
			DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				/* ���������ģʽ */	      
			DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;									/* �洢������ģʽ */  
			DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	/* �������ݳ���:8λ */	  
			DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;					/* �ڴ����ݳ���:8λ */
			DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	 													/* DMAģʽ������ģʽ */  
			DMA_InitStructure.DMA_Priority = DMA_Priority_High;      								/* ���ȼ����� */	  
			DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;        					/* ����FIFO */
			DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;      
			DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;    					/* ����ͻ�����δ��� */  
			DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; 		/* �洢��ͻ�����δ��� */
			
			DMA_Init(DMA1_Stream5, &DMA_InitStructure);									/* ��ʼ��DMA Stream */		   		
			DMA_ITConfig(DMA1_Stream5,DMA_IT_TC,ENABLE);  					/* ������������ж�  */


//			NVIC_InitTypeDef NVIC_InitStructure;		//�жϽṹ��
//				//�жϳ�ʼ�� 
//			NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn;  	/* ���� DMA����Ϊ�ж�Դ */
//			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		/* �������ȼ�Ϊ1 */  
//			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;					/* �����ȼ�Ϊ3 */  
//			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							/* ʹ���ⲿ�ж�ͨ�� */  
//			NVIC_Init(&NVIC_InitStructure);				
			printf("init");
		
	
	
}

/****************************************
  * @brief  SPI ����DMA��ʼ�� 
  * @param              
  * @retval             
  ***************************************/
static void SPI3_RX_DMA_Config(void)
{
		DMA_InitTypeDef DMA_InitStructure;		//DMA�ṹ��  
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);	
		DMA_DeInit(DMA1_Stream2);													/* ��λ��ʼ��DMA������ */  
		while (DMA_GetCmdStatus(DMA1_Stream2) != DISABLE);/* ȷ��DMA��������λ��� */
	
	  DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) (&(SPI3->DR)); 	/* ����DMAԴ���������ݼĴ�����ַ*/  
		DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)RX_Buff;				/* �ڴ��ַ(Ҫ����ı�����ָ��)*/  
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;						/* ���� ���赽�洢�� */		 
		DMA_InitStructure.DMA_BufferSize = BuffSize;				/* ���ݴ����� ,��Ҫ�����ܽ��ܵ�������[����Ϊ0],ʵ�ʷ���ʱ����������*/	  
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 	/* ���������ģʽ */	      
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;						/* �洢������ģʽ */  
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	/* �������ݳ���:8λ */	  
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;		/* �ڴ����ݳ���:8λ */
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	 										/* DMAģʽ������ģʽ */  
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;      					/* ���ȼ����� */	    
		DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;    				/*����FIFO*/    
		DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;      
		DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;    			/* ����ͻ�����δ��� */  
		DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; /* �洢��ͻ�����δ��� */  
		DMA_Init(DMA1_Stream2, &DMA_InitStructure);							/* ��ʼ��DMA Stream */		   

		DMA_ITConfig(DMA1_Stream2,DMA_IT_TC,ENABLE);  					/* ������������ж�  */

//		NVIC_InitTypeDef NVIC_InitStructure;		//�жϽṹ��
//			//�жϳ�ʼ�� 
//		NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream2_IRQn;  	/* ���� DMA����Ϊ�ж�Դ */
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		/* �������ȼ�Ϊ1 */  
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;					/* �����ȼ�Ϊ3 */  
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							/* ʹ���ⲿ�ж�ͨ�� */  
//		NVIC_Init(&NVIC_InitStructure);															/* ����NVIC */	

	
}

/*********************************************
  * @brief  SPI_DMA ����һ��buf
  * @param              
  * @retval             
  ********************************************/
void DMA_Write_buf()
{   
		
	  DMA_Cmd(DMA1_Stream5, DISABLE);	//�رշ��� DMA		
		DMA_SetCurrDataCounter(DMA1_Stream5,BuffSize);	//���÷��͵�������    

		SPI3->DR	;		//�������
		DMA_ClearFlag(DMA1_Stream5,DMA_IT_TCIF5);//����DMA��־λ	
		DMA_Cmd(DMA1_Stream5, ENABLE);	//�������� DMA
	
		return ;
	
}


void SPI_DMA_WRITE_READ_BUFF(void) 
{   
		
		DMA_Cmd(DMA1_Stream5, DISABLE);	
		DMA_Cmd(DMA1_Stream2, DISABLE);
	
		DMA_SetCurrDataCounter(DMA1_Stream5,BuffSize);	//���ý��ܵ�������
		DMA_SetCurrDataCounter(DMA1_Stream2,BuffSize);
	
		DMA_ClearFlag(DMA1_Stream5,DMA_IT_TCIF5);	//����DMA��־λ
		DMA_ClearFlag(DMA1_Stream2,DMA_IT_TCIF2);	//����DMA��־λ
	
		SPI3->DR;		//�������	  	

		DMA_Cmd(DMA1_Stream5, ENABLE);	
		DMA_Cmd(DMA1_Stream2, ENABLE);
		while(DMA_GetITStatus(DMA1_Stream2,DMA_IT_TCIF2)==RESET);
		DMA_ClearFlag(DMA1_Stream5,DMA_IT_TCIF5);	//����DMA��־λ
		DMA_ClearFlag(DMA1_Stream2,DMA_IT_TCIF2);	//����DMA��־λ
		

		return ;
}



/**
  * @brief  spi_dma ģ��ĳ�ʼ��
  * @param  
  * @retval 
  */
void spi_dma_config(void)
{
		SPI_Config();	//SPI����
		SPI3_TX_DMA_Config();	//DMA��������
		SPI3_RX_DMA_Config();	//DMA��������
	  SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE);	//SPI2 TX DMA����ʹ��.
		SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Rx, ENABLE);	//SPI2 RX DMA����ʹ��.

}

/*********************************
 *	
 *	DMA_�����жϷ�����
 *
 ********************************/
void DMA1_Stream5_IRQHandler(void)
{
		if(DMA_GetITStatus(DMA1_Stream5,DMA_IT_TCIF5))	//DMA�������
		{		
				DMA_ClearITPendingBit(DMA1_Stream5,DMA_IT_TCIF5);	//���DMA������ɱ�־λ
				
		}
	//	GPIO_SetBits(GPIOA,GPIO_Pin_15);

}








/*********************************************END OF FILE**********************/

