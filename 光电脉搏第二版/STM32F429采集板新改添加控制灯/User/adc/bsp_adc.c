/**
  ******************************************************************************
  * @file    bsp_bsp_adc.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   adc����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 F429 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "./adc/bsp_adc.h"
#include "stdlib.h"
#include "string.h"

//extern int selectedNums;
//extern int selectedChannel[16];
uint16_t *ADCValue1=NULL;
uint16_t *ADCValue2=NULL;
int test[12] = {1,2,3,4,5,6,7,8,9,10,11,12};
//uint16_t ADCValue1[13*1024]={0};
//uint16_t ADCValue2[13*1024]={0};

void Rheostat_ADC_GPIO_Config(int selectedChannel[])
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	for(int i = 0;i < 16;i++) {
		if(selectedChannel[i]) {
			switch(i) {
				case 0:
				/*=====================ͨ��1======================*/	
				// ʹ�� GPIO ʱ��
					RCC_AHB1PeriphClockCmd(RHEOSTAT_ADC_GPIO_CLK1,ENABLE);		
					// ���� IO
					GPIO_InitStructure.GPIO_Pin = RHEOSTAT_ADC_GPIO_PIN1;
					GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
					//������������	
					GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
					GPIO_Init(RHEOSTAT_ADC_GPIO_PORT1, &GPIO_InitStructure);
					printf("%d\r\n",i+1);
					break;
				case 1:
					/*=====================ͨ��2======================*/
					// ʹ�� GPIO ʱ��
					RCC_AHB1PeriphClockCmd(RHEOSTAT_ADC_GPIO_CLK2,ENABLE);		
					// ���� IO
					GPIO_InitStructure.GPIO_Pin = RHEOSTAT_ADC_GPIO_PIN2;
					GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
					//������������	
					GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
					GPIO_Init(RHEOSTAT_ADC_GPIO_PORT2, &GPIO_InitStructure);						
					printf("%d\r\n",i+1);
					break;
				case 2:
				/*=====================ͨ��3=======================*/
				// ʹ�� GPIO ʱ��
					RCC_AHB1PeriphClockCmd(RHEOSTAT_ADC_GPIO_CLK3,ENABLE);		
					// ���� IO
					GPIO_InitStructure.GPIO_Pin = RHEOSTAT_ADC_GPIO_PIN3;
					GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
					//������������	
					GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
					GPIO_Init(RHEOSTAT_ADC_GPIO_PORT3, &GPIO_InitStructure);
					printf("%d\r\n",i+1);
					break;
				case 3:
						/*=====================ͨ��4======================*/	
					// ʹ�� GPIO ʱ��
					RCC_AHB1PeriphClockCmd(RHEOSTAT_ADC_GPIO_CLK4,ENABLE);		
					// ���� IO
					GPIO_InitStructure.GPIO_Pin = RHEOSTAT_ADC_GPIO_PIN4;
					GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
					//������������	
					GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
					GPIO_Init(RHEOSTAT_ADC_GPIO_PORT4, &GPIO_InitStructure);
					printf("%d\r\n",i+1);
					break;
				case 4:
					/*=====================ͨ��5======================*/
					// ʹ�� GPIO ʱ��
					RCC_AHB1PeriphClockCmd(RHEOSTAT_ADC_GPIO_CLK5,ENABLE);		
					// ���� IO
					GPIO_InitStructure.GPIO_Pin = RHEOSTAT_ADC_GPIO_PIN5;
					GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
					//������������	
					GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
					GPIO_Init(RHEOSTAT_ADC_GPIO_PORT5, &GPIO_InitStructure);	
					printf("%d\r\n",i+1);
					break;
				case 5:
						/*=====================ͨ��6=======================*/
					// ʹ�� GPIO ʱ��
					RCC_AHB1PeriphClockCmd(RHEOSTAT_ADC_GPIO_CLK6,ENABLE);		
					// ���� IO
					GPIO_InitStructure.GPIO_Pin = RHEOSTAT_ADC_GPIO_PIN6;
					GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
					//������������	
					GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
					GPIO_Init(RHEOSTAT_ADC_GPIO_PORT6, &GPIO_InitStructure);
					printf("%d\r\n",i+1);
					break;
				case 6:
						/*=====================ͨ��7======================*/
					// ʹ�� GPIO ʱ��
					RCC_AHB1PeriphClockCmd(RHEOSTAT_ADC_GPIO_CLK7,ENABLE);		
					// ���� IO
					GPIO_InitStructure.GPIO_Pin = RHEOSTAT_ADC_GPIO_PIN7;
					GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
					//������������	
					GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
					GPIO_Init(RHEOSTAT_ADC_GPIO_PORT7, &GPIO_InitStructure);	
					printf("%d\r\n",i+1);
					break;
				case 7:
					/*=====================ͨ��8=======================*/
					// ʹ�� GPIO ʱ��
					RCC_AHB1PeriphClockCmd(RHEOSTAT_ADC_GPIO_CLK8,ENABLE);		
					// ���� IO
					GPIO_InitStructure.GPIO_Pin = RHEOSTAT_ADC_GPIO_PIN8;
					GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
					//������������	
					GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
					GPIO_Init(RHEOSTAT_ADC_GPIO_PORT8, &GPIO_InitStructure);
					printf("%d\r\n",i+1);
					break;
				case 8:
							/*=====================ͨ��9=======================*/
					// ʹ�� GPIO ʱ��
					RCC_AHB1PeriphClockCmd(RHEOSTAT_ADC_GPIO_CLK9,ENABLE);		
					// ���� IO
					GPIO_InitStructure.GPIO_Pin = RHEOSTAT_ADC_GPIO_PIN9;
					GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
					//������������	
					GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
					GPIO_Init(RHEOSTAT_ADC_GPIO_PORT9, &GPIO_InitStructure);
					printf("%d\r\n",i+1);
					break;
				case 9:
					/*=====================ͨ��10=======================*/
					// ʹ�� GPIO ʱ��
					RCC_AHB1PeriphClockCmd(RHEOSTAT_ADC_GPIO_CLK10,ENABLE);		
					// ���� IO
					GPIO_InitStructure.GPIO_Pin = RHEOSTAT_ADC_GPIO_PIN10;
					GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
					//������������	
					GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
					GPIO_Init(RHEOSTAT_ADC_GPIO_PORT10, &GPIO_InitStructure);
					printf("%d\r\n",i+1);
					break;
				case 10:
					/*=====================ͨ��11=======================*/
					// ʹ�� GPIO ʱ��
					RCC_AHB1PeriphClockCmd(RHEOSTAT_ADC_GPIO_CLK11,ENABLE);		
					// ���� IO
					GPIO_InitStructure.GPIO_Pin = RHEOSTAT_ADC_GPIO_PIN11;
					GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
					//������������	
					GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
					GPIO_Init(RHEOSTAT_ADC_GPIO_PORT11, &GPIO_InitStructure);
					printf("%d\r\n",i+1);
					break;
				case 11:
			/*=====================ͨ��12=======================*/
					// ʹ�� GPIO ʱ��
					RCC_AHB1PeriphClockCmd(RHEOSTAT_ADC_GPIO_CLK12,ENABLE);		
					// ���� IO
					GPIO_InitStructure.GPIO_Pin = RHEOSTAT_ADC_GPIO_PIN12;
					GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
					//������������	
					GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
					GPIO_Init(RHEOSTAT_ADC_GPIO_PORT12, &GPIO_InitStructure);				
					printf("%d\r\n",i+1);
					break;
				case 12:
					/*=====================ͨ��13=======================*/
					// ʹ�� GPIO ʱ��
					RCC_AHB1PeriphClockCmd(RHEOSTAT_ADC_GPIO_CLK13,ENABLE);		
					// ���� IO
					GPIO_InitStructure.GPIO_Pin = RHEOSTAT_ADC_GPIO_PIN13;
					GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
					//������������	
					GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
					GPIO_Init(RHEOSTAT_ADC_GPIO_PORT13, &GPIO_InitStructure);
					printf("%d\r\n",i+1);
					break;
					default:
					break;
		}
	}
}	
}

void Rheostat_ADC_Mode_Config(int samplingDelay,int nums,int selectedChannel[])
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
	
  // ------------------DMA Init �ṹ����� ��ʼ��--------------------------
  // ADC1ʹ��DMA2��������0��ͨ��0��������ֲ�̶�����
  // ����DMAʱ��
  RCC_AHB1PeriphClockCmd(RHEOSTAT_ADC_DMA_CLK, ENABLE); 
	// �����ַΪ��ADC ���ݼĴ�����ַ
	DMA_InitStructure.DMA_PeripheralBaseAddr = RHEOSTAT_ADC_DR_ADDR;	
  // �洢����ַ��ʵ���Ͼ���һ���ڲ�SRAM�ı���	
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)ADCValue1;  
  // ���ݴ��䷽��Ϊ���赽�洢��	
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;	
	// ��������СΪ��ָһ�δ����������
	DMA_InitStructure.DMA_BufferSize = nums*1024; //ÿ��8��ͨ��1024����
	// ����Ĵ���ֻ��һ������ַ���õ���
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  // �洢����ַ�̶�
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
  // // �������ݴ�СΪ���֣��������ֽ� 
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; 
  //	�洢�����ݴ�СҲΪ���֣����������ݴ�С��ͬ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	
	
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  // DMA ����ͨ�����ȼ�Ϊ�ߣ���ʹ��һ��DMAͨ��ʱ�����ȼ����ò�Ӱ��
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  // ��ֹDMA FIFO	��ʹ��ֱ��ģʽ
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;  
  // FIFO ��С��FIFOģʽ��ֹʱ�������������	
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;  
	// ѡ�� DMA ͨ����ͨ������������
  DMA_InitStructure.DMA_Channel = RHEOSTAT_ADC_DMA_CHANNEL; 
	
	DMA_DoubleBufferModeConfig(DMA2_Stream0,(uint32_t)ADCValue1,DMA_Memory_0);
	DMA_DoubleBufferModeConfig(DMA2_Stream0,(uint32_t)ADCValue2,DMA_Memory_1);
	DMA_DoubleBufferModeCmd(DMA2_Stream0,ENABLE);
//DMA_Cmd(DMA2_Stream0, ENABLE);

	
  //��ʼ��DMA�������൱��һ����Ĺܵ����ܵ������кܶ�ͨ��
	DMA_Init(RHEOSTAT_ADC_DMA_STREAM, &DMA_InitStructure);
	
	DMA_ITConfig(RHEOSTAT_ADC_DMA_STREAM,DMA_IT_TC,ENABLE);
//	
	NVIC_InitTypeDef   NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//��ռ���ȼ�0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;//�����ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);//����
	
	// ʹ��DMA��
  DMA_Cmd(RHEOSTAT_ADC_DMA_STREAM, ENABLE);
	
	// ����ADCʱ��
	RCC_APB2PeriphClockCmd(RHEOSTAT_ADC_CLK , ENABLE);
  // -------------------ADC Common �ṹ�� ���� ��ʼ��------------------------
	// ����ADCģʽ
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  // ʱ��Ϊfpclk x��Ƶ	
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8;  //ADC_CLOCK = PCLK2(90MHZ) / ADC_DIV    90/4=22.5M
  // ��ֹDMAֱ�ӷ���ģʽ	
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  // ����ʱ����	

	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;     

	
  // -------------------ADC Init �ṹ�� ���� ��ʼ��--------------------------
	ADC_StructInit(&ADC_InitStructure);
  // ADC �ֱ���
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  // ɨ��ģʽ����ͨ���ɼ���Ҫ	
  ADC_InitStructure.ADC_ScanConvMode = ENABLE; 
  // ����ת��	
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; 
  //��ֹ�ⲿ���ش���
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
  //�ⲿ����ͨ����������ʹ�������������ֵ��㸳ֵ����
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_TRGO;
  //�����Ҷ���	
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  //ת��ͨ�� 8��
  ADC_InitStructure.ADC_NbrOfConversion = nums;                                    
  ADC_Init(RHEOSTAT_ADC, &ADC_InitStructure);
  //---------------------------------------------------------------------------
	
  // ���� ADC ͨ��ת��˳��Ͳ���ʱ������
for(int i = 0;i < 16;i++) {
		if(selectedChannel[i]) {
			switch(i) {
				case 0:
				  ADC_RegularChannelConfig(RHEOSTAT_ADC, RHEOSTAT_ADC_CHANNEL1, 1, 
	                         ADC_SampleTime_15Cycles);
					printf("%d\r\n",i+1);
					break;
				case 1:
          ADC_RegularChannelConfig(RHEOSTAT_ADC, RHEOSTAT_ADC_CHANNEL2, 2, 
	                         ADC_SampleTime_15Cycles);
					printf("%d\r\n",i+1);				
					break;
				case 2:
					ADC_RegularChannelConfig(RHEOSTAT_ADC, RHEOSTAT_ADC_CHANNEL3, 3, 
	                         ADC_SampleTime_15Cycles); 
					printf("%d\r\n",i+1);
					break;
				case 3:
					ADC_RegularChannelConfig(RHEOSTAT_ADC, RHEOSTAT_ADC_CHANNEL4, 4, 
	                         ADC_SampleTime_15Cycles);
					printf("%d\r\n",i+1);
					break;
				case 4:
					ADC_RegularChannelConfig(RHEOSTAT_ADC, RHEOSTAT_ADC_CHANNEL5, 5, 
	                         ADC_SampleTime_15Cycles);
					printf("%d\r\n",i+1);
					break;
				case 5:
					ADC_RegularChannelConfig(RHEOSTAT_ADC, RHEOSTAT_ADC_CHANNEL6, 6, 
	                         ADC_SampleTime_15Cycles);
					printf("%d\r\n",i+1);
					break;
				case 6:
					ADC_RegularChannelConfig(RHEOSTAT_ADC, RHEOSTAT_ADC_CHANNEL7, 7, 
	                         ADC_SampleTime_15Cycles);
					printf("%d\r\n",i+1);
					break;
				case 7:
					ADC_RegularChannelConfig(RHEOSTAT_ADC, RHEOSTAT_ADC_CHANNEL8, 8, 
	                         ADC_SampleTime_15Cycles);
					printf("%d\r\n",i+1);
					break;
				case 8:
					ADC_RegularChannelConfig(RHEOSTAT_ADC, RHEOSTAT_ADC_CHANNEL9, 9, 
	                         ADC_SampleTime_15Cycles);
					printf("%d\r\n",i+1);
					break;
				case 9:
					ADC_RegularChannelConfig(RHEOSTAT_ADC, RHEOSTAT_ADC_CHANNEL10, 10, 
	                         ADC_SampleTime_15Cycles);
					printf("%d\r\n",i+1);
					break;
				case 10:
					ADC_RegularChannelConfig(RHEOSTAT_ADC, RHEOSTAT_ADC_CHANNEL11, 11, 
	                         ADC_SampleTime_15Cycles);
					printf("%d\r\n",i+1);
					break;
				case 11:
					ADC_RegularChannelConfig(RHEOSTAT_ADC, RHEOSTAT_ADC_CHANNEL12, 12, 
	                         ADC_SampleTime_15Cycles);			
					printf("%d\r\n",i+1);
					break;
				case 12:
					ADC_RegularChannelConfig(RHEOSTAT_ADC, RHEOSTAT_ADC_CHANNEL13, 13, 
	                         ADC_SampleTime_15Cycles);
					printf("%d\r\n",i+1);
					break;
					default:
					break;
			}
		}
	}
  // ʹ��DMA���� after last transfer (Single-ADC mode)
  ADC_DMARequestAfterLastTransferCmd(RHEOSTAT_ADC, ENABLE);
  // ʹ��ADC DMA
 ADC_DMACmd(RHEOSTAT_ADC, ENABLE);
	
	// ʹ��ADC
  ADC_Cmd(RHEOSTAT_ADC, ENABLE);  


}



void Rheostat_Init(int selectedChannel[],int nums)
{
		if(ADCValue1!=NULL||ADCValue2!=NULL) {
				free(ADCValue1);
				free(ADCValue2);
		}
		ADCValue1 = (uint16_t *) malloc (nums*1024 *2);
	  ADCValue2 = (uint16_t *) malloc (nums*1024 *2);
		memset(ADCValue1,0,nums*1024);
		memset(ADCValue2,0,nums*1024);
		Rheostat_ADC_GPIO_Config(selectedChannel);
//	Rheostat_ADC_Mode_Config(20,13);
}



