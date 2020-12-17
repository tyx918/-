/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ���ڽӷ����ԣ����ڽ��յ����ݺ����ϻش���
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
#include "./usart/bsp_debug_usart.h"
#include "./internalFlash/bsp_internalFlash.h"  
#include "./gpio/bsp_gpio.h"
#include "./spi/bsp_spi_dma.h"
#include <stdlib.h>

#include "ff.h"

#define ADDIR	"0:/ad"	

#define LittletoBig16(A)   (   (( (uint16_t)(A) & 0x00ff) << 8)| ( ((uint16_t)(A) & 0xff00) >> 8))

FATFS fs;													/* FatFs�ļ�ϵͳ���� */
FRESULT result;                /* �ļ�������� */
FIL file;													/* �ļ����� */
UINT fnum;            					  /* �ļ��ɹ���д���� */
DIR dir; 

char fileName[100];//�ļ���

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
	
uint16_t US_RX_BUF[16]={0};
extern uint8_t AD_RX_Buff[AD_BUFF_SIZE];
extern uint8_t AD_TX_Buff[AD_BUFF_SIZE];

extern int eventFlag;
extern uint8_t WIFI_TX_Buff[WIFI_BUFF_SIZE];
extern uint8_t WIFI_RX_Buff[WIFI_BUFF_SIZE];

static void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}
void receiveAD(void);
void upload(void);

void reverseEndian(uint8_t *little,int size);
void spiTransmit();
void sendADCmd(char cmd[],int size);


int  samplePoints =100;
char sampleRate[2]={0x02,0xc1}; //Ĭ�ϲ���������  0.705MHZ
char fileSize[4]={0x00}; //�ļ���С 
void dectohex(int dec,char result[]);



int main(void)
{
	int i=0;
	int j=0;
	__IO uint32_t uwData[16]= {0};
	Delay(0xffffff);
  /*��ʼ��USART������ģʽΪ 115200 8-N-1*/
	Debug_USART_Config();
	GPIO_Config();	
	spi_ad_dma_config();
	spi_wifi_dma_config();
	result = f_mount(&fs,"0:",1);
	
	/*----------------------- ��ʽ������ ---------------------------*/  
	/* ���û���ļ�ϵͳ�͸�ʽ�����������ļ�ϵͳ */
	if(result == FR_NO_FILESYSTEM)
	{
		printf("��SD����û���ļ�ϵͳ���������и�ʽ��...\r\n");
    /* ��ʽ�� */
		result=f_mkfs("0:",0,0);							
		
		if(result == FR_OK)
		{
			printf("��SD���ѳɹ���ʽ���ļ�ϵͳ��\r\n");
      /* ��ʽ������ȡ������ */
			result = f_mount(NULL,"0:",1);			
      /* ���¹���	*/			
			result = f_mount(&fs,"0:",1);
		}
		else
		{
			//LED_RED;
			printf("������ʽ��ʧ�ܡ�����\r\n");
			while(1);
		}
	}
  else if(result!=FR_OK)
  {
    printf("����SD�������ļ�ϵͳʧ�ܡ�(%d)\r\n",result);
    printf("��������ԭ��SD����ʼ�����ɹ���\r\n");
		while(1);
  }
  else
  {
    printf("���ļ�ϵͳ���سɹ�\r\n");
  }


//while(1){
//	if(eventFlag ==1){
//			eventFlag =0;
//			GPIO_SetBits(GPIOA,GPIO_Pin_0);  //����Ƭѡ����ʼ��������
//			DMA_Cmd(DMA2_Stream2, ENABLE);	//�򿪽���AD���ݵ� DMA
//			while(DMA_GetITStatus(DMA2_Stream2,DMA_IT_TCIF2)==RESET); 
//			DMA_ClearITPendingBit(DMA2_Stream2,DMA_IT_TCIF2);	
//			DMA_Cmd(DMA2_Stream2, DISABLE);	//�򿪽���AD���ݵ� DMA

//			GPIO_ResetBits(GPIOA,GPIO_Pin_0);  //����Ƭѡ
//		//	reverseEndian(AD_RX_Buff,AD_BUFF_SIZE);
//				int j =0;
//			for(j =0;j<17;j++)
//				printf("0x%x ",AD_RX_Buff[j]);
//			printf("\r\n");
//	}

//}
	while(1){
		
		if(GPIO_ReadInputDataBit(GPIOH,GPIO_Pin_15)==0){
				Delay(0xffff);
				spiTransmit(); //recv cmd;
					int i=0;
				printf("rcv");
				printf("%x%x%x",WIFI_RX_Buff[0],WIFI_RX_Buff[1],WIFI_RX_Buff[2]);
				if(WIFI_RX_Buff[0]==0x43 && WIFI_RX_Buff[1]==0x4d && WIFI_RX_Buff[2]==0x44 ){
						sendADCmd(WIFI_RX_Buff,16); //����ADCָ��
						Delay(0xfff);
						if(WIFI_RX_Buff[13]==0x00 && WIFI_RX_Buff[14]==0x00 && WIFI_RX_Buff[15]==0x01)  //��֡�ɼ�
						{
							printf("��֡�ɼ�\r\n");
							receiveAD();
						
						}
						else if(WIFI_RX_Buff[13]==0x00 && WIFI_RX_Buff[14]==0x00 && WIFI_RX_Buff[15]==0x08){  //�Ϸ����²ɼ�����
							
							printf("�Ϸ����²ɼ�����\r\n");
							receiveAD();
							upload();
						
						}
					
						else if(WIFI_RX_Buff[13]==0x00 && WIFI_RX_Buff[14]==0x00 && WIFI_RX_Buff[15]==0x05){  //ADC����
						
							if(WIFI_RX_Buff[7]==0x01){
								samplePoints =1;
							}
							else if(WIFI_RX_Buff[7]==0x0A){
								
								samplePoints =10;
							}
							else if(WIFI_RX_Buff[7]==0x64){
								samplePoints =100;
							}
							sampleRate[0]=WIFI_RX_Buff[3];
							sampleRate[1]=WIFI_RX_Buff[4];

							
							printf("ADC����\r\n");
							printf("��������%dk\r\n",samplePoints);

							
						}
						
						else if(WIFI_RX_Buff[13]==0x00 && WIFI_RX_Buff[14]==0x00 && WIFI_RX_Buff[15]==0x09){
						printf("�ϴ���ʷ�ļ�\r\n");
							sprintf(fileName,"0:/ad/ad%03d.bin",WIFI_RX_Buff[4]);//Ŀǰֻʹ���ļ����
							printf("%s",fileName);
							upload();
						 
						
						}
						
				}
				
		
				
		
		}
		
		
	}

	
}


void receiveAD(void){

		uint16_t i=0,j = 0;	
		int sum = samplePoints *2*8*1024;

		result = f_opendir(&dir,ADDIR);/* ��ָ���ļ��� */
		while(result != FR_OK)
		{  
				f_mkdir(ADDIR);
				result = f_opendir(&dir,ADDIR);						
		}	
		for(i=1;i<999;i++)
		{
				sprintf(fileName,"0:/ad/ad%03d.bin",i);
				result=f_open(&file,(const TCHAR *)fileName,FA_READ);/* ��׷�ӷ�ʽ��ָ���ļ� */
				if(result==FR_OK)
					f_close(&file);	// �ر��ļ�
				else if(result==FR_NO_FILE)	
				break;					
		}
		f_close(&file);	// �ر��ļ�
		
		if(i==999)	/* �����ļ�ʧ�� */
		{   
			printf("����wav�ļ�ʧ�ܡ�����\r\n");
			result = f_closedir(&dir);	/* �ر��ļ��� */
		//	eventFlag = 0;	/* ��ѯ:���� */
			return;
		}

		result=f_open(&file,fileName,FA_CREATE_ALWAYS|FA_WRITE);	/* ����ָ�������ļ���ֻд */
		if(result==FR_OK) 	
		{ 
				printf("����wav�ļ��ɹ����ļ�����%s\r\n",fileName);
			
		}	
		else
		{
				printf("Open wavfile fail!!!->%d\r\n",result);	
				//�˳���������Ĳ�ѯ

				result = f_close (&file);	/* �ر��ļ� */
				result = f_closedir(&dir);	/* �ر��ļ��� */
			//	eventFlag=0;							/* ��ѯ:���� */
				return;
		}
		
		char header[1024]={0x41,0x44,0x43,0x02,0x00,0x02,0xc1,0x01,0xFF,0x00,0x0C,0x80,0x00,0x43,0x44,0x41};
		memset(header+16,0,1008);
		
		header[5] = sampleRate[0];
		header[6] = sampleRate[1];
		dectohex(sum,fileSize);
		header[9] = fileSize[0];
		header[10] = fileSize[1];
		header[11] = fileSize[2];
		header[12] = fileSize[3];
		
		result=f_write(&file,header,1024,&fnum);	
		

		printf("��ʼ����AD����!\r\n");
		DMA_Cmd(DMA2_Stream2, ENABLE);	//�򿪽���AD���ݵ� DMA

		int dataCount = 1024;
		
		while(1){
			
			GPIO_SetBits(GPIOA,GPIO_Pin_0);  //����Ƭѡ����ʼ��������
			while(DMA_GetITStatus(DMA2_Stream2,DMA_IT_TCIF2)==RESET); 
			DMA_ClearITPendingBit(DMA2_Stream2,DMA_IT_TCIF2);	
			
			GPIO_ResetBits(GPIOA,GPIO_Pin_0);  //����Ƭѡ
	//		DMA_Cmd(DMA2_Stream2, DISABLE);	//�򿪽���AD���ݵ� DMA
			//��С��ת��
//			for( i =0;i<AD_BUFF_SIZE;i+=2)
//			{
//					uint8_t temp = AD_RX_Buff[i];
//					AD_RX_Buff[i] = AD_RX_Buff[i+1];
//					AD_RX_Buff[i+1] = temp;
//			}	
//			
			
			result=f_write(&file,AD_RX_Buff,AD_BUFF_SIZE,&fnum);	/* д������ */
				//Delay(0xfffff);  
			dataCount+=AD_BUFF_SIZE;
			//printf("%d ",dataCount);
			if(dataCount == sum+1024) 
			{
				result = f_close (&file);			
				result = f_closedir(&dir);
				printf("AD����д�����!\r\n");

				break;

			}
		
		}	
		DMA_Cmd(DMA2_Stream2, DISABLE);	//�򿪽���AD���ݵ� DMA

		
		sendADCmd("stopstopstopstop",16); //����adcָ��



}




void upload(void){
			int count=0,endflag =0;
	result=f_open(&file,fileName,FA_READ); 
			if(result!=FR_OK)
			{
					printf("���ļ�ʧ��!!!->%d\r\n",result);
					result = f_close (&file);
					return;			
			}
			else
			{
					printf("���ļ��ɹ�!!!->\r\n");
			}

			
			while(1){
				result = f_read(&file,WIFI_TX_Buff,WIFI_BUFF_SIZE,&fnum);		
				if(result!=FR_OK){
						printf("��ȡ��%d֡ʧ�ܣ���\r\n",count);
						printf("!!�ϴ�����\r\n");						
						count=0;								// �����ϴ�֡���� 
						printf("�ر��ļ�\r\n");
						result = f_close(&file);		// �ر��ļ�
						break;
				
				}
				if(file.fptr==file.fsize)  // �ɹ���ȡ��β(��ȡ�������) 
				{		
						endflag =1;
				}
				
					printf("�ϴ���%0005d֡\r\n",++count);
					GPIO_ResetBits(GPIOH,GPIO_Pin_14);//WIFI FB 2 LOW
				  Delay(0x12222);	
					while(GPIO_ReadInputDataBit(GPIOH,GPIO_Pin_15)==1);
					spiTransmit();
					GPIO_SetBits(GPIOH,GPIO_Pin_14);				
					while(GPIO_ReadInputDataBit(GPIOH,GPIO_Pin_15)==0);  //�ȴ�wifi�ϴ����

					if(endflag ==1){
						printf("!!�ϴ�����\r\n");						
						count=0;								// �����ϴ�֡���� 
						printf("�ر��ļ�\r\n");
						result = f_close (&file);
						eventFlag = 0;
						break;
					}
			
			}	
			


}



void DMA1_Stream3_IRQHandler(void)
{	
		int i =0;
	  printf("rcv\r\n");
		if(DMA_GetITStatus(DMA1_Stream3,DMA_IT_TCIF3))	
		{		
				DMA_ClearITPendingBit(DMA1_Stream3,DMA_IT_TCIF3);	
				
		}
	//	GPIO_SetBits(GPIOA,GPIO_Pin_15);

}
void DMA1_Stream4_IRQHandler(void)
{	
		int i =0;
	 // printf("send\r\n");
		if(DMA_GetITStatus(DMA1_Stream4,DMA_IT_TCIF4))	
		{		
				DMA_ClearITPendingBit(DMA1_Stream4,DMA_IT_TCIF4);	
				
		}
	//	GPIO_SetBits(GPIOA,GPIO_Pin_15);

}

void DMA2_Stream5_IRQHandler(void)
{	
		int i =0;
	 // printf("send\r\n");
		if(DMA_GetITStatus(DMA2_Stream5,DMA_IT_TCIF5))	
		{		
				DMA_ClearITPendingBit(DMA2_Stream5,DMA_IT_TCIF5);	
				
		}
	//	GPIO_SetBits(GPIOA,GPIO_Pin_15);

}
void spiTransmit(){
	
		DMA_Cmd(DMA1_Stream3, DISABLE);	//�رս��� DMA	
		DMA_Cmd(DMA1_Stream4, DISABLE);		
	
  	DMA_SetCurrDataCounter(DMA1_Stream3,1024);	//���÷��͵�������    
  	DMA_SetCurrDataCounter(DMA1_Stream4,1024);	//���÷��͵�������    
	  SPI2->DR;		//�������	  	

		DMA_Cmd(DMA1_Stream3, ENABLE);	
		DMA_Cmd(DMA1_Stream4, ENABLE);

		while(DMA_GetITStatus(DMA1_Stream3,DMA_IT_TCIF3)==RESET);
		DMA_ClearFlag(DMA1_Stream3,DMA_IT_TCIF3);	//����DMA��־λ
		DMA_ClearFlag(DMA1_Stream4,DMA_IT_TCIF4);	//����DMA��־λ

		return ;
}



void sendADCmd(char cmd[],int size){
			
			memcpy(AD_TX_Buff,cmd,size);		
			DMA_SetCurrDataCounter(DMA2_Stream5,AD_BUFF_SIZE);	//���÷��͵�������    

			DMA_Cmd(DMA2_Stream5, ENABLE);	

			GPIO_SetBits(GPIOA,GPIO_Pin_1);  
			while(DMA_GetITStatus(DMA2_Stream5,DMA_IT_TCIF5)==RESET); 
			DMA_ClearITPendingBit(DMA2_Stream5,DMA_IT_TCIF5);	
			DMA_ClearITPendingBit(DMA2_Stream2,DMA_IT_TCIF2);	

			DMA_Cmd(DMA2_Stream5, DISABLE);
			GPIO_ResetBits(GPIOA,GPIO_Pin_1);  //����Ƭѡ
			SPI1->DR;  //��Ҫ


}

void dectohex(int dec,char result[]){
	if(sizeof(result)<4){
		printf("dectohex error\r\n");
		return;
	}
	int i=3;
	 while (1){
        result[i] = dec % 256;
        dec /= 256;
        if (dec == 0) {
            break;
        }
        i--;
    }
	

}
/*********************************************END OF FILE**********************/

