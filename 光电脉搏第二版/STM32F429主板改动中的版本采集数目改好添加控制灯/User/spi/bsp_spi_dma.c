/***********************************
 *
 *			bsp_spi_dma.c  
 *���� spi_dma ȫ˫��ͨѶ����Ҫ����
 *
 **********************************/

#include "./spi/bsp_spi_dma.h"
#include <string.h>
#include "usart/bsp_debug_usart.h"
# include <stdlib.h>


extern int selectedNums;

//int  ad_BUFF_SIZE  = 13*1024*2;

extern int  ad_BUFF_SIZE;
uint8_t *AD_RX_Buff = NULL;
uint8_t *AD_TX_Buff = NULL;
//uint8_t AD_RX_Buff[ad_BUFF_SIZE];
//uint8_t AD_TX_Buff[ad_BUFF_SIZE];

uint8_t WIFI_TX_Buff[WIFI_BUFF_SIZE];
uint8_t WIFI_RX_Buff[WIFI_BUFF_SIZE];

//uint8_t writeBuff[BUFF_SIZE*10];
 /*****************************************
  * @brief  ��ʼ��SPI ��Ҫע�� spi��Ƶ��
  * @param  ��
  * @retval ��
  *****************************************/
static void SPI_AD_Config(void)
{
	GPIO_InitTypeDef GPIO_uInitStructure;
	SPI_InitTypeDef SPI_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);//MISO
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);//MOSI
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);//SCK
	//GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);//SCK

	//SCK
	GPIO_uInitStructure.GPIO_Pin = GPIO_Pin_3 ;
	GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_uInitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_uInitStructure.GPIO_OType = GPIO_OType_PP;

	GPIO_Init(GPIOB,&GPIO_uInitStructure);
	
	//MISO
	GPIO_uInitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_uInitStructure.GPIO_OType = GPIO_OType_PP;  

	GPIO_uInitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&GPIO_uInitStructure);
	
	//MOSI  
	GPIO_uInitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_uInitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_uInitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA,&GPIO_uInitStructure);
	
	
	SPI_Cmd(SPI1,DISABLE); 
	
	//SPI��ʼ��
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex ;  
	SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;  //����ARMΪ�ӻ���CPLDΪ����                                                                                                                                                    
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;  
	SPI_InitStructure.SPI_CPOL = 0x00;                                                    
	SPI_InitStructure.SPI_CPHA = 0x00;   
		SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;		

	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  
	SPI_InitStructure.SPI_CRCPolynomial = 7; 
	SPI_Init(SPI1,&SPI_InitStructure);
	


	SPI_Cmd(SPI1,ENABLE);   //����spi

}
/****************************************
  * @brief  SPI ����DMA��ʼ�� 
  * @param              
  * @retval             
  ***************************************/
static void SPI1_AD_TX_DMA_Config(void)
{
			printf("SPI1_AD_TX_DMA_Config:%d\r\n",ad_BUFF_SIZE);
			DMA_InitTypeDef DMA_InitStructure;  		//DMA�ṹ��
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);	/* ʹ��DMAʱ�� */  		
			DMA_DeInit(DMA2_Stream5);								/* ��λ��ʼ��DMA������ */  
			while (DMA_GetCmdStatus(DMA2_Stream5) != DISABLE);	/* ȷ��DMA��������λ��� */  
			
			/* ���� DMA Stream */
			DMA_InitStructure.DMA_Channel = DMA_Channel_3;  					
			DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) (&(SPI1->DR));;					/* �����ַ */  
			DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)AD_TX_Buff;							/* �ڴ��ַ(Ҫ����ı�����ָ��) ,DMA�洢��0��ַ*/  
			DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;									/* ���򣺴洢�������� */		  
			DMA_InitStructure.DMA_BufferSize = (uint32_t)ad_BUFF_SIZE;							/* ���ݴ����� ,������Ϊ0�� ʵ�ʷ���ʱ����������*/	  
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
			
			DMA_Init(DMA2_Stream5, &DMA_InitStructure);									/* ��ʼ��DMA Stream */		   		
			DMA_ITConfig(DMA2_Stream5,DMA_IT_TC,ENABLE);  					/* ������������ж�  */


//			NVIC_InitTypeDef NVIC_InitStructure;		//�жϽṹ��
//				//�жϳ�ʼ�� 
//			NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream5_IRQn;  	/* ���� DMA����Ϊ�ж�Դ */
//			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		/* �������ȼ�Ϊ1 */  
//			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;					/* �����ȼ�Ϊ3 */  
//			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							/* ʹ���ⲿ�ж�ͨ�� */  
//			NVIC_Init(&NVIC_InitStructure);				
			
		
	
	
}

//----------------------------DMA �ĳ���--------------------------------//

/****************************************
  * @brief  SPI ����DMA��ʼ�� 
  * @param              
  * @retval       ֻ��ʹ����2!?      
  ***************************************/
static void SPI1_AD_RX_DMA_Config(void)
{
		printf("SPI1_AD_RX_DMA_Config:%d\r\n",ad_BUFF_SIZE);
		DMA_InitTypeDef DMA_InitStructure;		//DMA�ṹ��  
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);	
		DMA_DeInit(DMA2_Stream2);													/* ��λ��ʼ��DMA������ */  
		while (DMA_GetCmdStatus(DMA2_Stream2) != DISABLE);/* ȷ��DMA��������λ��� */
	
	  DMA_InitStructure.DMA_Channel = DMA_Channel_3;  
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) (&(SPI1->DR)); 	/* ����DMAԴ���������ݼĴ�����ַ*/  
		DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)AD_RX_Buff;				/* �ڴ��ַ(Ҫ����ı�����ָ��)*/  
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;						/*�򣺴洢��������ģʽ */		  
		DMA_InitStructure.DMA_BufferSize =(uint32_t)ad_BUFF_SIZE;				/* ���ݴ����� ,��Ҫ�����ܽ��ܵ�������[����Ϊ0],ʵ�ʷ���ʱ����������*/	  
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 	/* ���������ģʽ */	      
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;						/* �洢������ģʽ */  
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	/* �������ݳ���:8λ */	  
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;		/* �ڴ����ݳ���:8λ */
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	 										/* DMAģʽ������ģʽ */  
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;      					/* ���ȼ����� */	    
		DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;    				/*����FIFO*/    
		DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;      
		DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;    			/* ����ͻ�����δ��� */  
		DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; /* �洢��ͻ�����δ��� */  
		DMA_Init(DMA2_Stream2, &DMA_InitStructure);							/* ��ʼ��DMA Stream */		   

		DMA_ITConfig(DMA2_Stream2,DMA_IT_TC,ENABLE);  					/* ������������жϣ����뿪�������޷�ʹ���жϱ�־δ

//		NVIC_InitTypeDef NVIC_InitStructure;		//�жϽṹ��
//			//�жϳ�ʼ�� 
//		NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream2_IRQn;  	/* ���� DMA����Ϊ�ж�Դ */
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		/* �������ȼ�Ϊ1 */  
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					/* �����ȼ�Ϊ3 */  
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							/* ʹ���ⲿ�ж�ͨ�� */  
//		NVIC_Init(&NVIC_InitStructure);															/* ����NVIC */	
		//printf("111");
	
}



/**
  * @brief  spi_dma ģ��ĳ�ʼ��
  * @param  
  * @retval 
  */
void spi_ad_dma_config(void)
{
	  printf("selectedNums: %d \r\n",ad_BUFF_SIZE);
	  AD_RX_Buff = (uint8_t*) malloc (selectedNums*1024*2* sizeof(uint8_t));
	  AD_TX_Buff = (uint8_t*) malloc (selectedNums*1024*2 * sizeof(uint8_t));
	  if(AD_RX_Buff == NULL)  printf("R null\r\n");
	  if(AD_TX_Buff == NULL)  printf("T null\r\n");
		printf("AD_RX_Buff�Ĵ�С: %d \r\n",sizeof(AD_RX_Buff));
//	  printf("AD_RX_Buff size:%d\r\n",sizeof(AD_RX_Buff));
		SPI_AD_Config();	//SPI����
		SPI1_AD_RX_DMA_Config();	//DMA��������
		SPI1_AD_TX_DMA_Config();//dma��������
		SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);	//SPI1 RX DMA����ʹ��.
		SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);	//SPI1 TX DMA����ʹ��.


}



 /*****************************************
  * @brief  ��ʼ��SPI ��Ҫע�� spi��Ƶ��
  * @param  ��
  * @retval ��
  *****************************************/
static void SPI_WIFI_Config(void)
{
	GPIO_InitTypeDef GPIO_uInitStructure;
	SPI_InitTypeDef SPI_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI,ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);//MISO
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);//MOSI
	GPIO_PinAFConfig(GPIOI, GPIO_PinSource1, GPIO_AF_SPI2);//SCK
	GPIO_PinAFConfig(GPIOI, GPIO_PinSource0, GPIO_AF_SPI2);//CS

	//SCK
	GPIO_uInitStructure.GPIO_Pin = GPIO_Pin_1 ;
	GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_uInitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;

	GPIO_uInitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_uInitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOI,&GPIO_uInitStructure);
	
	//MISO
	GPIO_uInitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_uInitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_uInitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  
	GPIO_uInitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&GPIO_uInitStructure);
	
	//MOSI  
	GPIO_uInitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_uInitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_uInitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOB,&GPIO_uInitStructure);
	
	//CS
	GPIO_uInitStructure.GPIO_Pin = GPIO_Pin_0 ;
	GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_OUT;   		
	GPIO_uInitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_uInitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOI,&GPIO_uInitStructure);

		GPIO_ResetBits(GPIOI,GPIO_Pin_0);

	SPI_Cmd(SPI2,DISABLE); 
	
	//SPI��ʼ��
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex ;  
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;  //����ARMΪ�ӻ���WIFIΪ����                                                                                                                                                    
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;  
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;  
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  
	SPI_InitStructure.SPI_CRCPolynomial = 7; 
	SPI_Init(SPI2,&SPI_InitStructure);
	SPI_Cmd(SPI2,ENABLE);   


}




//----------------------------DMA �ĳ���--------------------------------//
/****************************************
  * @brief  SPI ����DMA��ʼ�� 
  * @param              
  * @retval             
  ***************************************/
static void SPI2_WIFI_TX_DMA_Config(void)
{
			DMA_InitTypeDef DMA_InitStructure;  		//DMA�ṹ��
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);	/* ʹ��DMAʱ�� */  		
			DMA_DeInit(DMA1_Stream4);								/* ��λ��ʼ��DMA������ */  
			while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE);	/* ȷ��DMA��������λ��� */  
			
			/* ���� DMA Stream */
			DMA_InitStructure.DMA_Channel = DMA_Channel_0;  					
			DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) (&(SPI2->DR));;					/* �����ַ */  
			DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)WIFI_TX_Buff;							/* �ڴ��ַ(Ҫ����ı�����ָ��) ,DMA�洢��0��ַ*/  
			DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;									/* ���򣺴洢�������� */		  
			DMA_InitStructure.DMA_BufferSize = (uint32_t)WIFI_BUFF_SIZE;							/* ���ݴ����� ,������Ϊ0�� ʵ�ʷ���ʱ����������*/	  
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
			
			DMA_Init(DMA1_Stream4, &DMA_InitStructure);									/* ��ʼ��DMA Stream */		   		
			DMA_ITConfig(DMA1_Stream4,DMA_IT_TC,ENABLE);  					/* ������������ж�  */


//			NVIC_InitTypeDef NVIC_InitStructure;		//�жϽṹ��
//				//�жϳ�ʼ�� 
//			NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream4_IRQn;  	/* ���� DMA����Ϊ�ж�Դ */
//			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		/* �������ȼ�Ϊ1 */  
//			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;					/* �����ȼ�Ϊ3 */  
//			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							/* ʹ���ⲿ�ж�ͨ�� */  
//			NVIC_Init(&NVIC_InitStructure);				
//			
		
	
	
}

/****************************************
  * @brief  SPI ����DMA��ʼ�� 
  * @param              
  * @retval             
  ***************************************/
static void SPI2_WIFI_RX_DMA_Config(void)
{
		DMA_InitTypeDef DMA_InitStructure;		//DMA�ṹ��  
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);	
		DMA_DeInit(DMA1_Stream3);													/* ��λ��ʼ��DMA������ */  
		while (DMA_GetCmdStatus(DMA1_Stream3) != DISABLE);/* ȷ��DMA��������λ��� */
	
	  DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) (&(SPI2->DR)); 	/* ����DMAԴ���������ݼĴ�����ַ*/  
		DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)WIFI_RX_Buff;				/* �ڴ��ַ(Ҫ����ı�����ָ��)*/  
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;						/* ���򣺴洢��������ģʽ */		  
		DMA_InitStructure.DMA_BufferSize = (uint32_t)WIFI_BUFF_SIZE;				/* ���ݴ����� ,��Ҫ�����ܽ��ܵ�������[����Ϊ0],ʵ�ʷ���ʱ����������*/	  
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
		DMA_Init(DMA1_Stream3, &DMA_InitStructure);							/* ��ʼ��DMA Stream */		   

		DMA_ITConfig(DMA1_Stream3,DMA_IT_TC,ENABLE);  					/* ������������ж�  */

//		NVIC_InitTypeDef NVIC_InitStructure;		//�жϽṹ��
//			//�жϳ�ʼ�� 
//		NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream3_IRQn;  	/* ���� DMA����Ϊ�ж�Դ */
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		/* �������ȼ�Ϊ1 */  
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;					/* �����ȼ�Ϊ3 */  
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							/* ʹ���ⲿ�ж�ͨ�� */  
//		NVIC_Init(&NVIC_InitStructure);															/* ����NVIC */	

//	
}



/**
  * @brief  spi_dma ģ��ĳ�ʼ��
  * @param  
  * @retval 
  */
void spi_wifi_dma_config(void)
{
	  
		SPI_WIFI_Config();	//SPI����
		SPI2_WIFI_TX_DMA_Config();	//DMA��������
		SPI2_WIFI_RX_DMA_Config();	//DMA��������
		SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);	//SPI2 TX DMA����ʹ��.
		SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, ENABLE);	//SPI2 RX DMA����ʹ��.

}
/*********************************************
  * @brief  SPI_DMA ����һ��buf
  * @param              
  * @retval             
  ********************************************/
void DMA_Write_buf()
{   
		
	  DMA_Cmd(DMA1_Stream4, DISABLE);	//�رշ��� DMA		
		DMA_SetCurrDataCounter(DMA1_Stream4,1024);	//���÷��͵�������    

		SPI2->DR;		//�������
		DMA_ClearFlag(DMA1_Stream4,DMA_IT_TCIF4);//����DMA��־λ	
		DMA_Cmd(DMA1_Stream4, ENABLE);	//�������� DMA
	
		return ;
	
}




/*********************************************END OF FILE**********************/

