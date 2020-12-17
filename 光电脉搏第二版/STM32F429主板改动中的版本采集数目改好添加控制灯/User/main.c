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
#include "./systick/bsp_SysTick.h"
#include <stdlib.h>
#include "./433/433.h"

#include "ff.h"

#define ADDIR	"0:/ad"	

#define LittletoBig16(A)   (   (( (uint16_t)(A) & 0x00ff) << 8)| ( ((uint16_t)(A) & 0xff00) >> 8))

FATFS fs;													/* FatFs�ļ�ϵͳ���� */
FRESULT result;                /* �ļ�������� */
FIL file;													/* �ļ����� */
UINT fnum;            					  /* �ļ��ɹ���д���� */
DIR dir; 

int selectedNums = 0;
char fileName[100];//�ļ���

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
	
uint16_t US_RX_BUF[16]={0};
//extern uint8_t AD_RX_Buff[AD_BUFF_SIZE];//ad�Ľ���buff,armΪ�ӻ���cpldΪ����
//extern uint8_t AD_TX_Buff[AD_BUFF_SIZE];//ad�ķ���buff
extern uint8_t *AD_RX_Buff;//ad�Ľ���buff,armΪ�ӻ���cpldΪ����
extern uint8_t *AD_TX_Buff;//ad�ķ���buff
int ad_BUFF_SIZE = 0;

extern int eventFlag;
extern uint8_t WIFI_TX_Buff[WIFI_BUFF_SIZE];//wifi�ķ�������
extern uint8_t WIFI_RX_Buff[WIFI_BUFF_SIZE];//wifi�Ľ�������

static void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}
void receiveAD(void);//����ad�ɼ�����
void upload(void);//�ϴ��ļ�����

void reverseEndian(uint8_t *little,int size);
void spiTransmit();//spi����
void sendADCmd(char cmd[],int size);//����


double  samplePoints =4;//������
char sampleRate[2]={0x02,0xc1}; //Ĭ�ϲ���������  0.705MHZ
char fileSize[4]={0x00}; //�ļ���С 
void dectohex(int dec,char result[]);//ת��Ϊ16����




int main(void)
{
//	uint8_t *AD_RX_Buff = (uint8_t*) malloc (4 * 2*1024 * sizeof(int));
//	uint8_t *AD_TX_Buff = (uint8_t*) malloc (4 * 2*1024 * sizeof(int));
	int i=0;
	int j=0;
	__IO uint32_t uwData[16]= {0};
	Delay(0xffffff);
  /*��ʼ��USART������ģʽΪ 115200 8-N-1*/
	//Debug_USART_Config();//����ͨ������,��ʼ����2
	 SysTick_Init();	
	USART_Config();
	Delay_ms(500);
	MD_Init();
	printf("START\r\n");
	Set433Config(airspeed[3]); // ����433����
	printf("line88\r\n");
	GPIO_Config();	//GPIO����
	spi_ad_dma_config(); //ʹ��spi1��armΪ������cpldΪ�ӻ�
	spi_wifi_dma_config();//armΪ�ӻ���wifiΪ������ʹ��cpld2
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
//		while(1);
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
		printf6("nihaoa");	
		char *p = Rev433Cmd();
		long Data[10]={0};
		if(p)
		{
			
			printf("%s\r\n",p);
		 
		}
		
		if(GPIO_ReadInputDataBit(GPIOH,GPIO_Pin_15)==0){ 
				int i=0;
			  printf("enter167\r\n");
				Delay(0xffff);
			  spiTransmit(); //recv cmd;
			  for(int i = 0;i < 20;i++)
						printf("%x\r\n",WIFI_RX_Buff[i]);
			  //printf("%x%x%x",WIFI_RX_Buff[0],WIFI_RX_Buff[1],WIFI_RX_Buff[2],WIFI_RX_Buff[3]);
				if(WIFI_RX_Buff[0]==0x43 && WIFI_RX_Buff[1]==0x4D && WIFI_RX_Buff[2]==0x44 ){
					  //sendADCmd(WIFI_RX_Buff,16); 
						printf("enter174\r\n");
						Delay(0xfff);
						if(WIFI_RX_Buff[13]==0x00 && WIFI_RX_Buff[14]==0x00 && WIFI_RX_Buff[15]==0x01)  //��֡�ɼ�
						{
							sendADCmd(WIFI_RX_Buff,16); 
							printf("��֡�ɼ�\r\n");
							receiveAD();//����ad�����ݣ���adrxbuff��ֵ�����ļ�
						}
						else if(WIFI_RX_Buff[13]==0x00 && WIFI_RX_Buff[14]==0x00 && WIFI_RX_Buff[15]==0x08){  //�Ϸ����²ɼ�����
							sendADCmd(WIFI_RX_Buff,16); 
							printf("�Ϸ����²ɼ�����\r\n");
							receiveAD();
							upload();//wifi�Ϸ��ļ�
						}
						else if(WIFI_RX_Buff[13]==0x00 && WIFI_RX_Buff[14]==0x00 && (WIFI_RX_Buff[15]==0x05||WIFI_RX_Buff[14]==0x00)){  //ADC����
//							printf("%x\r\n",WIFI_RX_Buff[7]);
							selectedNums = 0;
							//WIFI_RX_Buff[4]==0x01||WIFI_RX_Buff[8]==0x10||WIFI_RX_Buff[8]==0x00
							if(WIFI_RX_Buff[4]==0x01){
								samplePoints =4;
							}
							//WIFI_RX_Buff[4]==0x0A||WIFI_RX_Buff[8]==0xA0||WIFI_RX_Buff[8]==0x00
							else if(WIFI_RX_Buff[4]==0x0A){
								
								samplePoints = 6;
							}
							//||WIFI_RX_Buff[8]==0x40||WIFI_RX_Buff[8]==0x00
							else if(WIFI_RX_Buff[4]==0x05){
								samplePoints =10;
							}
							else if(WIFI_RX_Buff[4]==0x02){
								samplePoints = 0.5;
							}
							else if(WIFI_RX_Buff[4]==0x03){
								samplePoints =1;
							}
							else if(WIFI_RX_Buff[4]==0x04){
								samplePoints =2;
							}
//							sampleRate[0]=WIFI_RX_Buff[3];
//							sampleRate[1]=WIFI_RX_Buff[4];
							//���㱻ѡ�е�ͨ������
							char byte = WIFI_RX_Buff[6];
							char byte1 = WIFI_RX_Buff[7];
							printf("byte1 = %x\r\n",byte1);
							//for(int i = 0;i < 8; i++)  byte1 = byte1 >>1;
							//printf("byte1 = %x\r\n",byte1);
							for(int i=0; i<8; i++)
							{
									if(byte&0x01)
								{
										selectedNums++;
							}
										byte = byte>>1;
								}
							for(int i=0; i<8; i++)
							{
									if(byte1&0x01)
								{
									selectedNums++;
							}
									byte1 = byte1>>1;
							}
						  ad_BUFF_SIZE = selectedNums*2*1024;
							if(AD_RX_Buff!=NULL&&AD_TX_Buff!=NULL) {
									free(AD_RX_Buff);
									free(AD_TX_Buff);
							}
							spi_ad_dma_config();
							Delay(0xffff);
//							spiTransmit(); //recv cmd;
							sendADCmd(WIFI_RX_Buff,16); //����ADCָ����ӻ�
							printf("selectedNums= %d\r\n",selectedNums);
							
							printf("ADC����\r\n");
							printf("��������%fk\r\n",samplePoints);	
						}	
						else if(WIFI_RX_Buff[13]==0x00 && WIFI_RX_Buff[14]==0x00 && WIFI_RX_Buff[15]==0x09){
						printf("�ϴ���ʷ�ļ�\r\n");
							sprintf(fileName,"0:/ad/ad%03d.bin",WIFI_RX_Buff[4]);//Ŀǰֻʹ���ļ����
							printf("%s",fileName);
							upload();
						}
						//ת����ָ��
						else if(WIFI_RX_Buff[13]==0x12 && WIFI_RX_Buff[14]==0x34 && WIFI_RX_Buff[15]==0x56){
							//printf("ת��ָ��");
							sendADCmd(WIFI_RX_Buff,16);
						}
						//ת�����ȵ���ָ���д
						else if(WIFI_RX_Buff[13]==0x56 && WIFI_RX_Buff[14]==0x78 && WIFI_RX_Buff[15]==0x12){
							sendADCmd(WIFI_RX_Buff,16);
						}
						
				}
				
		
				
		
		}
		
		
	}

	
}


void receiveAD(void){
		uint16_t i=0,j = 0;
	  printf("selectedNums: %d\r\n",selectedNums);
		//int sum = samplePoints *2*8*1024;//�������õ����ܹ������ĵ���
		//int sum = samplePoints *2*8*1024;//�������õ����ܹ������ĵ���
		int sum = samplePoints*4*selectedNums*1024;//�������õ����ܹ������ĵ���.
    //int sum = samplePoints *2*9*1024;//�������õ����ܹ������ĵ���
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
		//����λheader[6]��������м���ͨͨheader[6]
		//����λheader[8]��������м���ͨͨheader[6]
		//char header[1024]={0x41,0x44,0x43,0x03,0x00,0x02,0xc1,0x01,0xFF,0x00,0x0C,0x80,0x00,0x43,0x44,0x41};
		char header[1024]={0x41,0x44,0x43,0x03,0x00,0x02,0xc1,0x01,0x07,0x00,0x0C,0x80,0x00,0x43,0x44,0x41};

		header[3] = 0x01;
		header[8] = 0x01;
		switch(selectedNums) {
			case 1:		
				header[3] = 0x01;//�洢��ɼ����Ǽ���ͨ����ѡͨ����Ϣ
				header[8] = 0x01;
			break;
			case 2:		
				header[3] = 0x02;
				header[8] = 0x03;
			break;
			case 3:
				header[3] = 0x03;
				header[8] = 0x07;	
			break;
			case 4:
				header[3] = 0x04;
				header[8] = 0x0F;
			break;
			case 5:
				header[3] = 0x05;
				header[8] = 0x1F;
			break;
			case 6:
				header[3] = 0x06;
				header[8] = 0x3F;
			break;
			case 7:
				header[3] = 0x07;
				header[8] = 0x7F;
			break;
			case 8:
				header[3] = 0x08;
				header[8] = 0xFF;
			break;
			case 9:
				header[3] = 0x09;
				header[8] = 0x0F;
			break;
			case 10:
				header[3] = 0x10;
				header[8] = 0x0F;
			break;
			case 11:
				header[3] = 0x11;
				header[8] = 0x0F;
			break;
			case 12:
				header[3] = 0x12;
				header[8] = 0x0F;
			break;
			case 13:
				header[3] = 0x13;
				header[8] = 0x0F;
			break;
			default:
				break;
		}

		
		memset(header+16,0,1008);
		header[5] = sampleRate[0];//�Ѳ����ʸ�ͷ1024�ֽ���
		header[6] = sampleRate[1];
		dectohex(sum,fileSize);//��sumת����16���ƣ��ֱ�浽ͷ1024�ֽ���
		header[9] = fileSize[0];
		header[10] = fileSize[1];
		header[11] = fileSize[2];
		header[12] = fileSize[3];
		
		result=f_write(&file,header,1024,&fnum);//д��ͷ�ļ�Ǯ1024���ֽڵ�����
		

		printf("��ʼ����AD����!\r\n");
		DMA_Cmd(DMA2_Stream2, ENABLE);	//�򿪽���AD���ݵ� DMA

		int dataCount = 1024;
		
		while(1){
			
			GPIO_SetBits(GPIOA,GPIO_Pin_0);  //����Ƭѡ����ʼ��������
//			printf("line375\r\n");
			while(DMA_GetITStatus(DMA2_Stream2,DMA_IT_TCIF2)==RESET); 
//			printf("line377\r\n");
			DMA_ClearITPendingBit(DMA2_Stream2,DMA_IT_TCIF2);	
//			printf("line379\r\n");
			
			GPIO_ResetBits(GPIOA,GPIO_Pin_0);  //����Ƭѡ
//			printf("line3382\r\n");
	//		DMA_Cmd(DMA2_Stream2, DISABLE);	//�򿪽���AD���ݵ� DMA
			//��С��ת��
//			for( i =0;i<AD_BUFF_SIZE;i+=2)
//			{
//					uint8_t temp = AD_RX_Buff[i];
//					AD_RX_Buff[i] = AD_RX_Buff[i+1];
//					AD_RX_Buff[i+1] = temp;
//			}	
//			
			
			result=f_write(&file,AD_RX_Buff,ad_BUFF_SIZE,&fnum);	/* д������ */
				//Delay(0xfffff);  
			dataCount+=ad_BUFF_SIZE;//adcount == 1024
			
//      printf("sum+1024 = %d\r\n",sum+1024);
				printf("dataCount = %d\r\n",dataCount/1024);
//			printf("%d\r\n",dataCount);
      //�����յ����ݵ������õ�����
			if(dataCount == sum+1024) 
			{
				result = f_close (&file);			
				result = f_closedir(&dir);
				printf("AD����д�����!\r\n");
				break;

			}
		
		}	
		DMA_Cmd(DMA2_Stream2, DISABLE);	//�رս���AD���ݵ� DMA

		
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
						memset(AD_RX_Buff,0,ad_BUFF_SIZE);
						memset(AD_TX_Buff,0,ad_BUFF_SIZE);
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

		DMA_Cmd(DMA1_Stream3, ENABLE);	//����dma
		DMA_Cmd(DMA1_Stream4, ENABLE);  //����DMA

		while(DMA_GetITStatus(DMA1_Stream3,DMA_IT_TCIF3)==RESET);
		DMA_ClearFlag(DMA1_Stream3,DMA_IT_TCIF3);	//����DMA��־λ
		DMA_ClearFlag(DMA1_Stream4,DMA_IT_TCIF4);	//����DMA��־λ

		return ;
}



void sendADCmd(char cmd[],int size){
	    printf("sendADCmd\r\n");		
	
			memcpy(AD_TX_Buff,cmd,size);		
			DMA_SetCurrDataCounter(DMA2_Stream5,ad_BUFF_SIZE);	//���÷��͵�������    

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
	//int i=3;
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

