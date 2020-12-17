/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ADC����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 F429 ������
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
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

// ADCת���ĵ�ѹֵͨ��DMA��ʽ����SRAM
extern  uint16_t ADCValue1[800];
extern  uint16_t ADCValue2[800];

int flag =0;
#define Delaytime 0x4fff
extern uint8_t TX_Buff[BuffSize];
extern uint8_t RX_Buff[BuffSize];

static void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}
void reconfigureADC(int samplingDelay);


/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{				

  /*��ʼ��USART ����ģʽΪ 115200 8-N-1���жϽ���*/
  Debug_USART_Config();	
	TIM2_Configuration();
	GPIO_Config();	
	SDRAM_Init();  
	Rheostat_Init();	
	DAC_Mode_Init();
	spi_dma_config();
	
	TIM_Cmd(TIM2, ENABLE);


	printf("start");
	
	int i=0;


	while (1){	
			
			if(GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_14)==1){  //����ָ��
				SPI_DMA_WRITE_READ_BUFF();	
				for(i=0;i<16;i++)
					printf("%x ",RX_Buff[i]);
				printf("\r\n");
				if(RX_Buff[13]==0x00 && RX_Buff[14]==0x00 && RX_Buff[15]==0x05){  //ADC����
					
						if(RX_Buff[3]==0x04&&RX_Buff[4]==0xe2){   //1.25 MSPS
						
				//		reconfigureADC(6);
						printf("reconfigureADC\r\n");
							

					
						}
						else if(RX_Buff[3]==0x02&&RX_Buff[4]==0xc1){  //0.705MSPS

							//reconfigureADC(20);
							printf("reconfigureADCr\n");
						}
				}			
				else if((RX_Buff[13]==0x00 && RX_Buff[14]==0x00&& RX_Buff[15]==0x01) || (RX_Buff[13]==0x00 && RX_Buff[14]==0x00&&RX_Buff[15]==0x08)){  //��֡�ɼ�  || �ϴ���������


					printf("start adc\r\n");
					TIM_Cmd(TIM2, ENABLE);


				
				}
				else if(RX_Buff[0] == 's'){  //ֹͣadc
					

						printf("stop adc\r\n");
									TIM_Cmd(TIM2, DISABLE);

				
				}

			
		 }
			

	}

}

void reconfigureADC(int samplingDelay){
	
	DMA_Cmd(RHEOSTAT_ADC_DMA_STREAM, DISABLE); 
	ADC_DMARequestAfterLastTransferCmd(RHEOSTAT_ADC, DISABLE);
	ADC_DMACmd(RHEOSTAT_ADC, DISABLE);
  ADC_Cmd(RHEOSTAT_ADC, DISABLE);  
	ADC_DeInit();
	Rheostat_ADC_Mode_Config(samplingDelay);
  

}
void DMA2_Stream0_IRQHandler(void){
	
///	printf("111");

	if(DMA_GetFlagStatus(DMA2_Stream0,DMA_FLAG_TCIF0)==SET){
		
		DMA_ClearITPendingBit(DMA2_Stream0,DMA_FLAG_TCIF0);

		if(GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_9)==1){
			
			if(DMA_GetCurrentMemoryTarget(DMA2_Stream0)){
				memcpy(TX_Buff,ADCValue2,BuffSize);


			}
			else{

				memcpy(TX_Buff,ADCValue1,BuffSize);

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

