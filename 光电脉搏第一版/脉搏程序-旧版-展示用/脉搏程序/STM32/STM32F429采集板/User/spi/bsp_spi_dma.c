/***********************************
 *
 *			bsp_spi_dma.c  
 *功能 spi_dma 全双工通讯的主要函数
 *
 **********************************/

#include "./spi/bsp_spi_dma.h"
#include <string.h>
#include "usart/bsp_debug_usart.h"
#include "./adc/bsp_adc.h"

uint8_t TX_Buff[BuffSize];
uint8_t RX_Buff[BuffSize];

 /*****************************************
  * @brief  初始化SPI 需要注意 spi的频率
  * @param  无
  * @retval 无
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
		GPIO_uInitStructure.GPIO_Pin = GPIO_Pin_15;		/* 选择要控制的GPIO引脚   */													
		GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_OUT;   	
		  GPIO_uInitStructure.GPIO_OType = GPIO_OType_PP;	
  GPIO_uInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_uInitStructure);			
	
	
	SPI_Cmd(SPI3,DISABLE); 
	
	//SPI初始化
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex ;  
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                                                                                                                                                  
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;  
	SPI_InitStructure.SPI_CPOL = 0x00;                                                    
	SPI_InitStructure.SPI_CPHA = 0x00;   
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;  //adc软件触发的情况下，需要spi较高的时钟，不然会导致adc缓存区数据出错，
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  
	SPI_InitStructure.SPI_CRCPolynomial = 7; 
	SPI_Init(SPI3,&SPI_InitStructure);
	SPI_Cmd(SPI3,ENABLE);   
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_15);

}



//----------------------------DMA 的程序--------------------------------//
/****************************************
  * @brief  SPI 发送DMA初始化 
  * @param              
  * @retval             
  ***************************************/
static void SPI3_TX_DMA_Config(void)
{
			DMA_InitTypeDef DMA_InitStructure;  		//DMA结构体
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);	/* 使能DMA时钟 */  		
			DMA_DeInit(DMA1_Stream5);								/* 复位初始化DMA数据流 */  
			while (DMA_GetCmdStatus(DMA1_Stream5) != DISABLE);	/* 确保DMA数据流复位完成 */  
			/* 配置 DMA Stream */
			DMA_InitStructure.DMA_Channel = DMA_Channel_0;  					
			DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) (&(SPI3->DR));;					/* 外设地址 */  
			DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)TX_Buff;							/* 内存地址(要传输的变量的指针) ,DMA存储器0地址*/  
			DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;									/* 方向：存储器到外设模式 */		 		  
			DMA_InitStructure.DMA_BufferSize = BuffSize;							/* 数据传输量 ,可设置为0， 实际发送时会重新设置*/	  
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
			
			DMA_Init(DMA1_Stream5, &DMA_InitStructure);									/* 初始化DMA Stream */		   		
			DMA_ITConfig(DMA1_Stream5,DMA_IT_TC,ENABLE);  					/* 开启传输完成中断  */


//			NVIC_InitTypeDef NVIC_InitStructure;		//中断结构体
//				//中断初始化 
//			NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn;  	/* 配置 DMA接收为中断源 */
//			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		/* 抢断优先级为1 */  
//			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;					/* 子优先级为3 */  
//			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							/* 使能外部中断通道 */  
//			NVIC_Init(&NVIC_InitStructure);				
			printf("init");
		
	
	
}

/****************************************
  * @brief  SPI 接收DMA初始化 
  * @param              
  * @retval             
  ***************************************/
static void SPI3_RX_DMA_Config(void)
{
		DMA_InitTypeDef DMA_InitStructure;		//DMA结构体  
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);	
		DMA_DeInit(DMA1_Stream2);													/* 复位初始化DMA数据流 */  
		while (DMA_GetCmdStatus(DMA1_Stream2) != DISABLE);/* 确保DMA数据流复位完成 */
	
	  DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) (&(SPI3->DR)); 	/* 设置DMA源：串口数据寄存器地址*/  
		DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)RX_Buff;				/* 内存地址(要传输的变量的指针)*/  
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;						/* 方向 外设到存储器 */		 
		DMA_InitStructure.DMA_BufferSize = BuffSize;				/* 数据传输量 ,需要最大可能接受的数据量[不能为0],实际发送时会重新设置*/	  
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
		DMA_Init(DMA1_Stream2, &DMA_InitStructure);							/* 初始化DMA Stream */		   

		DMA_ITConfig(DMA1_Stream2,DMA_IT_TC,ENABLE);  					/* 开启传输完成中断  */

//		NVIC_InitTypeDef NVIC_InitStructure;		//中断结构体
//			//中断初始化 
//		NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream2_IRQn;  	/* 配置 DMA接收为中断源 */
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		/* 抢断优先级为1 */  
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;					/* 子优先级为3 */  
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							/* 使能外部中断通道 */  
//		NVIC_Init(&NVIC_InitStructure);															/* 配置NVIC */	

	
}

/*********************************************
  * @brief  SPI_DMA 发送一个buf
  * @param              
  * @retval             
  ********************************************/
void DMA_Write_buf()
{   
		
	  DMA_Cmd(DMA1_Stream5, DISABLE);	//关闭发送 DMA		
		DMA_SetCurrDataCounter(DMA1_Stream5,BuffSize);	//设置发送的数据量    

		SPI3->DR	;		//清空数据
		DMA_ClearFlag(DMA1_Stream5,DMA_IT_TCIF5);//擦除DMA标志位	
		DMA_Cmd(DMA1_Stream5, ENABLE);	//开启发送 DMA
	
		return ;
	
}


void SPI_DMA_WRITE_READ_BUFF(void) 
{   
		
		DMA_Cmd(DMA1_Stream5, DISABLE);	
		DMA_Cmd(DMA1_Stream2, DISABLE);
	
		DMA_SetCurrDataCounter(DMA1_Stream5,BuffSize);	//设置接受的数据量
		DMA_SetCurrDataCounter(DMA1_Stream2,BuffSize);
	
		DMA_ClearFlag(DMA1_Stream5,DMA_IT_TCIF5);	//擦除DMA标志位
		DMA_ClearFlag(DMA1_Stream2,DMA_IT_TCIF2);	//擦除DMA标志位
	
		SPI3->DR;		//清空数据	  	

		DMA_Cmd(DMA1_Stream5, ENABLE);	
		DMA_Cmd(DMA1_Stream2, ENABLE);
		while(DMA_GetITStatus(DMA1_Stream2,DMA_IT_TCIF2)==RESET);
		DMA_ClearFlag(DMA1_Stream5,DMA_IT_TCIF5);	//擦除DMA标志位
		DMA_ClearFlag(DMA1_Stream2,DMA_IT_TCIF2);	//擦除DMA标志位
		

		return ;
}



/**
  * @brief  spi_dma 模块的初始化
  * @param  
  * @retval 
  */
void spi_dma_config(void)
{
		SPI_Config();	//SPI配置
		SPI3_TX_DMA_Config();	//DMA发送配置
		SPI3_RX_DMA_Config();	//DMA接受配置
	  SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE);	//SPI2 TX DMA请求使能.
		SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Rx, ENABLE);	//SPI2 RX DMA请求使能.

}

/*********************************
 *	
 *	DMA_发送中断服务函数
 *
 ********************************/
void DMA1_Stream5_IRQHandler(void)
{
		if(DMA_GetITStatus(DMA1_Stream5,DMA_IT_TCIF5))	//DMA发送完成
		{		
				DMA_ClearITPendingBit(DMA1_Stream5,DMA_IT_TCIF5);	//清除DMA发送完成标志位
				
		}
	//	GPIO_SetBits(GPIOA,GPIO_Pin_15);

}








/*********************************************END OF FILE**********************/

