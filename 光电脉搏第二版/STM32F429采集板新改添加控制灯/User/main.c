/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ADC例程
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F429 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "./dac/bsp_dac.h"
#include "./usart/bsp_debug_usart.h"
#include "./adc/bsp_adc.h"
#include "./spi/bsp_spi_dma.h"
#include "./gpio/bsp_gpio.h"   
#include "./sdram/bsp_sdram.h"   
#include "./timer/bsp_timer.h"

// ADC转换的电压值通过DMA方式传到SRAM
//extern  uint16_t ADCValue1[13*1024];
//extern  uint16_t ADCValue2[13*1024];
//extern uint8_t TX_Buff[BuffSize];
//extern uint8_t RX_Buff[BuffSize];
extern  uint16_t *ADCValue1;
extern  uint16_t *ADCValue2;
extern uint8_t *TX_Buff;
extern uint8_t *RX_Buff;

int test1[32] = {0};

extern int test[6];

int flag =0;
#define Delaytime 0x4fff

int BuffSize = 1024;
int selectedNums = 0;
int selectedChannel[16]={0};
//int myBuffSize = 1024*2;

static void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}
void reconfigureADC(int samplingDelay,int nums);
int getSelectedNums(char  ,char );
void getSelectedChannel(char ,char);
void reconfigureSpi(int);


/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{				

  /*初始化USART 配置模式为 115200 8-N-1，中断接收*/
  Debug_USART_Config();	
	TIM2_Configuration();
	GPIO_Config();	
	SDRAM_Init();  
//	Rheostat_Init();	
	DAC_Mode_Init();
	spi_dma_config(1);
	//默认两个灯全亮
	DAC_Cmd(DAC_Channel_1, ENABLE);
  DAC_Cmd(DAC_Channel_2, ENABLE);
	
	//TIM_Cmd(TIM2, ENABLE);


	printf("start");
	
	int i=0;


	while (1){	
			
			if(GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_14)==1){  //接收指令
				SPI_DMA_WRITE_READ_BUFF();	
				for(i=0;i<16;i++)
					printf("%x ",RX_Buff[i]);
				printf("\r\n");
				if(RX_Buff[13]==0x00 && RX_Buff[14]==0x00 && RX_Buff[15]==0x05){  //ADC配置
						//BuffSize = 1024;
						selectedNums = getSelectedNums(RX_Buff[6],RX_Buff[7]);
						BuffSize = BuffSize  * selectedNums * 2; 
						getSelectedChannel(RX_Buff[6],RX_Buff[7]);
					  Rheostat_Init(selectedChannel,selectedNums);//配置引脚，动态分配内存
					  reconfigureADC(20,selectedNums);//重新配置ADC模式
						reconfigureSpi(selectedNums);
						TIM_Cmd(TIM2, ENABLE);
					
						if(RX_Buff[3]==0x04&&RX_Buff[4]==0xe2){   //1.25 MSPS
				//		reconfigureADC(6);
						printf("reconfigureADC\r\n");
						}
						else if(RX_Buff[3]==0x02&&RX_Buff[4]==0xc1){  //0.705MSPS

							//reconfigureADC(20);
							printf("reconfigureADCr\n");
						}
				}			
				else if((RX_Buff[13]==0x00 && RX_Buff[14]==0x00&& RX_Buff[15]==0x01) || (RX_Buff[13]==0x00 && RX_Buff[14]==0x00&&RX_Buff[15]==0x08)){  //单帧采集  || 上传最新数据
					printf("start adc\r\n");
					TIM_Cmd(TIM2, ENABLE);				
				}
				else if((RX_Buff[13]==0x12 && RX_Buff[14]==0x34&& RX_Buff[15]==0x56)) {
					 printf("enter control light\r\n");
					 if(RX_Buff[3] == 0x01) {
						 	DAC_Cmd(DAC_Channel_1, ENABLE);
							DAC_Cmd(DAC_Channel_2, DISABLE);
					 }
					 if(RX_Buff[3] == 0x02) {
						 	DAC_Cmd(DAC_Channel_1, DISABLE);
							DAC_Cmd(DAC_Channel_2, ENABLE);
					 }
					 if(RX_Buff[3] == 0x03) {
						 	DAC_Cmd(DAC_Channel_1, ENABLE);
							DAC_Cmd(DAC_Channel_2, ENABLE);
					 }
				}
				else if(RX_Buff[0] == 's'){  //停止adc
					
						printf("stop adc\r\n");
									TIM_Cmd(TIM2, DISABLE);

				
				}

			
		 }
			

	}

}

int getSelectedNums(char byte1 ,char byte2){
	printf("byte1:%x\r\n",byte1 );
	printf("byte2:%x\r\n",byte2 );
	int nums = 0;
	for(int i=0; i<8; i++)
		{
			 if(byte1&0x01)
				{
						nums++;
				}
				byte1 = byte1>>1;
		}
		for(int j=0; j<8; j++)
		{
			if(byte2&0x01)
			{
					nums++;
			}
			byte2 = byte2>>1;
		}
		return nums;
}

void getSelectedChannel(char byte1 ,char byte2) {
//	  int* temp = (int *) malloc(16*4);
   	  for(int i = 0;i<16;i++) selectedChannel[i] = 0;
	    for(int i = 0;i<16;i++)  
	       printf("%d ",selectedChannel[i]);
		if(byte2 == 0) {
			printf("line157");
		 	for(int i=0; i<8; i++)
			{
						if(byte1&0x01)
			{						
					selectedChannel[i] = 1;
			}
				byte1 = byte1>>1;
			}
	 }
	 if(byte2!=0) {
	 //byte1:9-13通道
			for(int i=0; i<8; i++)
			{
			 if(byte1&0x01)
				{						
					selectedChannel[i+8] = 1;
				}
				byte1 = byte1>>1;
			}
		//byte2:1-8通道
			for(int j=0; j<8; j++)
			{
			if(byte2&0x01)
			{
					selectedChannel[j] = 1;
			}
					byte2 = byte2>>1;
			}
	 }
 }

void reconfigureADC(int samplingDelay,int nums){
	
	DMA_Cmd(RHEOSTAT_ADC_DMA_STREAM, DISABLE); 
	ADC_DMARequestAfterLastTransferCmd(RHEOSTAT_ADC, DISABLE);
	ADC_DMACmd(RHEOSTAT_ADC, DISABLE);
  ADC_Cmd(RHEOSTAT_ADC, DISABLE);  
	ADC_DeInit();
	Rheostat_ADC_Mode_Config(samplingDelay,nums,selectedChannel);
}

void reconfigureSpi(int selectedNums){
	DMA_Cmd(DMA1_Stream5, DISABLE);	
	DMA_Cmd(DMA1_Stream2, DISABLE);
	SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, DISABLE);	//SPI2 TX DMA请求使能关闭.
	SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Rx,  DISABLE);	//SPI2 RX DMA请求使能关闭.
	//SPI_Cmd(SPI3,DISABLE); 
	//SPI_DeInit(SPI3);
	spi_dma_config1(selectedNums);
}

void DMA2_Stream0_IRQHandler(void){

	if(DMA_GetFlagStatus(DMA2_Stream0,DMA_FLAG_TCIF0)==SET){
		
		DMA_ClearITPendingBit(DMA2_Stream0,DMA_FLAG_TCIF0);
		if(GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_9)==1){
			
			if(DMA_GetCurrentMemoryTarget(DMA2_Stream0)==1) {          
        memcpy(TX_Buff, ADCValue1, BuffSize);   
      }
      else{                                   
        memcpy(TX_Buff, ADCValue2, BuffSize); 
      }

			DMA_Write_buf();
			int i;
		for( i =0;i<16;i++){
			printf("%x ",TX_Buff[i]);
		}
		printf("\r\n");
		
	}
	


	}
}

void DMA1_Stream2_IRQHandler(void){

	if(DMA_GetFlagStatus(DMA1_Stream2,DMA_FLAG_TCIF2)==SET){
			
		DMA_ClearITPendingBit(DMA1_Stream2,DMA_FLAG_TCIF2);

		//printf("%x,%x,%x",RX_Buff[0],RX_Buff[1],RX_Buff[2]);

	}
}





/*********************************************END OF FILE**********************/

