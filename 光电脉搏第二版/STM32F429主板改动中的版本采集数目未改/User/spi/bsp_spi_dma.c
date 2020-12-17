/***********************************
 *
 *			bsp_spi_dma.c  
 *功能 spi_dma 全双工通讯的主要函数
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
  * @brief  初始化SPI 需要注意 spi的频率
  * @param  无
  * @retval 无
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
	
	//SPI初始化
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex ;  
	SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;  //设置ARM为从机，CPLD为主机                                                                                                                                                    
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;  
	SPI_InitStructure.SPI_CPOL = 0x00;                                                    
	SPI_InitStructure.SPI_CPHA = 0x00;   
		SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;		

	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  
	SPI_InitStructure.SPI_CRCPolynomial = 7; 
	SPI_Init(SPI1,&SPI_InitStructure);
	


	SPI_Cmd(SPI1,ENABLE);   //开启spi

}
/****************************************
  * @brief  SPI 发送DMA初始化 
  * @param              
  * @retval             
  ***************************************/
static void SPI1_AD_TX_DMA_Config(void)
{
			printf("SPI1_AD_TX_DMA_Config:%d\r\n",ad_BUFF_SIZE);
			DMA_InitTypeDef DMA_InitStructure;  		//DMA结构体
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);	/* 使能DMA时钟 */  		
			DMA_DeInit(DMA2_Stream5);								/* 复位初始化DMA数据流 */  
			while (DMA_GetCmdStatus(DMA2_Stream5) != DISABLE);	/* 确保DMA数据流复位完成 */  
			
			/* 配置 DMA Stream */
			DMA_InitStructure.DMA_Channel = DMA_Channel_3;  					
			DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) (&(SPI1->DR));;					/* 外设地址 */  
			DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)AD_TX_Buff;							/* 内存地址(要传输的变量的指针) ,DMA存储器0地址*/  
			DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;									/* 方向：存储器到外设 */		  
			DMA_InitStructure.DMA_BufferSize = (uint32_t)ad_BUFF_SIZE;							/* 数据传输量 ,可设置为0， 实际发送时会重新设置*/	  
			DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				/* 外设非增量模式 */	      
			DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;									/* 存储器增量模式 */  
			DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	/* 外设数据长度:8位 */	  
			DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;					/* 内存数据长度:8位 */
			DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	 													/* DMA模式：正常模式 */  
			DMA_InitStructure.DMA_Priority = DMA_Priority_High;      								/* 优先级：高 */	  
			DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;        					/* 禁用FIFO */
			DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;      
			DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;    					/* 外设突发单次传输 */  
			DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; 		/* 存储器突发单次传输 */
			
			DMA_Init(DMA2_Stream5, &DMA_InitStructure);									/* 初始化DMA Stream */		   		
			DMA_ITConfig(DMA2_Stream5,DMA_IT_TC,ENABLE);  					/* 开启传输完成中断  */


//			NVIC_InitTypeDef NVIC_InitStructure;		//中断结构体
//				//中断初始化 
//			NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream5_IRQn;  	/* 配置 DMA接收为中断源 */
//			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		/* 抢断优先级为1 */  
//			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;					/* 子优先级为3 */  
//			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							/* 使能外部中断通道 */  
//			NVIC_Init(&NVIC_InitStructure);				
			
		
	
	
}

//----------------------------DMA 的程序--------------------------------//

/****************************************
  * @brief  SPI 接收DMA初始化 
  * @param              
  * @retval       只能使用流2!?      
  ***************************************/
static void SPI1_AD_RX_DMA_Config(void)
{
		printf("SPI1_AD_RX_DMA_Config:%d\r\n",ad_BUFF_SIZE);
		DMA_InitTypeDef DMA_InitStructure;		//DMA结构体  
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);	
		DMA_DeInit(DMA2_Stream2);													/* 复位初始化DMA数据流 */  
		while (DMA_GetCmdStatus(DMA2_Stream2) != DISABLE);/* 确保DMA数据流复位完成 */
	
	  DMA_InitStructure.DMA_Channel = DMA_Channel_3;  
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) (&(SPI1->DR)); 	/* 设置DMA源：串口数据寄存器地址*/  
		DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)AD_RX_Buff;				/* 内存地址(要传输的变量的指针)*/  
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;						/*向：存储器到外设模式 */		  
		DMA_InitStructure.DMA_BufferSize =(uint32_t)ad_BUFF_SIZE;				/* 数据传输量 ,需要最大可能接受的数据量[不能为0],实际发送时会重新设置*/	  
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 	/* 外设非增量模式 */	      
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;						/* 存储器增量模式 */  
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	/* 外设数据长度:8位 */	  
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;		/* 内存数据长度:8位 */
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	 										/* DMA模式：正常模式 */  
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;      					/* 优先级：高 */	    
		DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;    				/*禁用FIFO*/    
		DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;      
		DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;    			/* 外设突发单次传输 */  
		DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; /* 存储器突发单次传输 */  
		DMA_Init(DMA2_Stream2, &DMA_InitStructure);							/* 初始化DMA Stream */		   

		DMA_ITConfig(DMA2_Stream2,DMA_IT_TC,ENABLE);  					/* 开启传输完成中断，必须开启否则无法使用中断标志未

//		NVIC_InitTypeDef NVIC_InitStructure;		//中断结构体
//			//中断初始化 
//		NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream2_IRQn;  	/* 配置 DMA接收为中断源 */
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		/* 抢断优先级为1 */  
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					/* 子优先级为3 */  
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							/* 使能外部中断通道 */  
//		NVIC_Init(&NVIC_InitStructure);															/* 配置NVIC */	
		//printf("111");
	
}



/**
  * @brief  spi_dma 模块的初始化
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
		printf("AD_RX_Buff的大小: %d \r\n",sizeof(AD_RX_Buff));
//	  printf("AD_RX_Buff size:%d\r\n",sizeof(AD_RX_Buff));
		SPI_AD_Config();	//SPI配置
		SPI1_AD_RX_DMA_Config();	//DMA接受配置
		SPI1_AD_TX_DMA_Config();//dma发送配置
		SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);	//SPI1 RX DMA请求使能.
		SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);	//SPI1 TX DMA请求使能.


}



 /*****************************************
  * @brief  初始化SPI 需要注意 spi的频率
  * @param  无
  * @retval 无
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
	
	//SPI初始化
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex ;  
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;  //设置ARM为从机，WIFI为主机                                                                                                                                                    
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




//----------------------------DMA 的程序--------------------------------//
/****************************************
  * @brief  SPI 发送DMA初始化 
  * @param              
  * @retval             
  ***************************************/
static void SPI2_WIFI_TX_DMA_Config(void)
{
			DMA_InitTypeDef DMA_InitStructure;  		//DMA结构体
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);	/* 使能DMA时钟 */  		
			DMA_DeInit(DMA1_Stream4);								/* 复位初始化DMA数据流 */  
			while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE);	/* 确保DMA数据流复位完成 */  
			
			/* 配置 DMA Stream */
			DMA_InitStructure.DMA_Channel = DMA_Channel_0;  					
			DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) (&(SPI2->DR));;					/* 外设地址 */  
			DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)WIFI_TX_Buff;							/* 内存地址(要传输的变量的指针) ,DMA存储器0地址*/  
			DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;									/* 方向：存储器到外设 */		  
			DMA_InitStructure.DMA_BufferSize = (uint32_t)WIFI_BUFF_SIZE;							/* 数据传输量 ,可设置为0， 实际发送时会重新设置*/	  
			DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				/* 外设非增量模式 */	      
			DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;									/* 存储器增量模式 */  
			DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	/* 外设数据长度:8位 */	  
			DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;					/* 内存数据长度:8位 */
			DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	 													/* DMA模式：正常模式 */  
			DMA_InitStructure.DMA_Priority = DMA_Priority_High;      								/* 优先级：高 */	  
			DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;        					/* 禁用FIFO */
			DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;      
			DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;    					/* 外设突发单次传输 */  
			DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; 		/* 存储器突发单次传输 */
			
			DMA_Init(DMA1_Stream4, &DMA_InitStructure);									/* 初始化DMA Stream */		   		
			DMA_ITConfig(DMA1_Stream4,DMA_IT_TC,ENABLE);  					/* 开启传输完成中断  */


//			NVIC_InitTypeDef NVIC_InitStructure;		//中断结构体
//				//中断初始化 
//			NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream4_IRQn;  	/* 配置 DMA接收为中断源 */
//			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		/* 抢断优先级为1 */  
//			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;					/* 子优先级为3 */  
//			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							/* 使能外部中断通道 */  
//			NVIC_Init(&NVIC_InitStructure);				
//			
		
	
	
}

/****************************************
  * @brief  SPI 接收DMA初始化 
  * @param              
  * @retval             
  ***************************************/
static void SPI2_WIFI_RX_DMA_Config(void)
{
		DMA_InitTypeDef DMA_InitStructure;		//DMA结构体  
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);	
		DMA_DeInit(DMA1_Stream3);													/* 复位初始化DMA数据流 */  
		while (DMA_GetCmdStatus(DMA1_Stream3) != DISABLE);/* 确保DMA数据流复位完成 */
	
	  DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) (&(SPI2->DR)); 	/* 设置DMA源：串口数据寄存器地址*/  
		DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)WIFI_RX_Buff;				/* 内存地址(要传输的变量的指针)*/  
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;						/* 方向：存储器到外设模式 */		  
		DMA_InitStructure.DMA_BufferSize = (uint32_t)WIFI_BUFF_SIZE;				/* 数据传输量 ,需要最大可能接受的数据量[不能为0],实际发送时会重新设置*/	  
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 	/* 外设非增量模式 */	      
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;						/* 存储器增量模式 */  
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	/* 外设数据长度:8位 */	  
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;		/* 内存数据长度:8位 */
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	 										/* DMA模式：正常模式 */  
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;      					/* 优先级：高 */	    
		DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;    				/*禁用FIFO*/    
		DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;      
		DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;    			/* 外设突发单次传输 */  
		DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; /* 存储器突发单次传输 */  
		DMA_Init(DMA1_Stream3, &DMA_InitStructure);							/* 初始化DMA Stream */		   

		DMA_ITConfig(DMA1_Stream3,DMA_IT_TC,ENABLE);  					/* 开启传输完成中断  */

//		NVIC_InitTypeDef NVIC_InitStructure;		//中断结构体
//			//中断初始化 
//		NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream3_IRQn;  	/* 配置 DMA接收为中断源 */
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		/* 抢断优先级为1 */  
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;					/* 子优先级为3 */  
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							/* 使能外部中断通道 */  
//		NVIC_Init(&NVIC_InitStructure);															/* 配置NVIC */	

//	
}



/**
  * @brief  spi_dma 模块的初始化
  * @param  
  * @retval 
  */
void spi_wifi_dma_config(void)
{
	  
		SPI_WIFI_Config();	//SPI配置
		SPI2_WIFI_TX_DMA_Config();	//DMA发送配置
		SPI2_WIFI_RX_DMA_Config();	//DMA接受配置
		SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);	//SPI2 TX DMA请求使能.
		SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, ENABLE);	//SPI2 RX DMA请求使能.

}
/*********************************************
  * @brief  SPI_DMA 发送一个buf
  * @param              
  * @retval             
  ********************************************/
void DMA_Write_buf()
{   
		
	  DMA_Cmd(DMA1_Stream4, DISABLE);	//关闭发送 DMA		
		DMA_SetCurrDataCounter(DMA1_Stream4,1024);	//设置发送的数据量    

		SPI2->DR;		//清空数据
		DMA_ClearFlag(DMA1_Stream4,DMA_IT_TCIF4);//擦除DMA标志位	
		DMA_Cmd(DMA1_Stream4, ENABLE);	//开启发送 DMA
	
		return ;
	
}




/*********************************************END OF FILE**********************/

