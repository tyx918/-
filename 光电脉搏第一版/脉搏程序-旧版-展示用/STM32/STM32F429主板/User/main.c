/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   串口接发测试，串口接收到数据后马上回传。
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
#include "./usart/bsp_debug_usart.h"
#include "./internalFlash/bsp_internalFlash.h"  
#include "./gpio/bsp_gpio.h"
#include "./spi/bsp_spi_dma.h"
#include <stdlib.h>

#include "ff.h"

#define ADDIR	"0:/ad"	

#define LittletoBig16(A)   (   (( (uint16_t)(A) & 0x00ff) << 8)| ( ((uint16_t)(A) & 0xff00) >> 8))

FATFS fs;													/* FatFs文件系统对象 */
FRESULT result;                /* 文件操作结果 */
FIL file;													/* 文件对象 */
UINT fnum;            					  /* 文件成功读写数量 */
DIR dir; 

char fileName[100];//文件名

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
	
uint16_t US_RX_BUF[16]={0};
extern uint8_t AD_RX_Buff[AD_BUFF_SIZE];
extern uint8_t AD_TX_Buff[AD_BUFF_SIZE];

extern int eventFlag;
extern uint8_t WIFI_TX_Buff[WIFI_BUFF_SIZE];
extern uint8_t WIFI_RX_Buff[WIFI_BUFF_SIZE];

static void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}
void receiveAD(void);
void upload(void);

void reverseEndian(uint8_t *little,int size);
void spiTransmit();
void sendADCmd(char cmd[],int size);


int  samplePoints =100;
char sampleRate[2]={0x02,0xc1}; //默认采样率设置  0.705MHZ
char fileSize[4]={0x00}; //文件大小 
void dectohex(int dec,char result[]);



int main(void)
{
	int i=0;
	int j=0;
	__IO uint32_t uwData[16]= {0};
	Delay(0xffffff);
  /*初始化USART，配置模式为 115200 8-N-1*/
	Debug_USART_Config();
	GPIO_Config();	
	spi_ad_dma_config();
	spi_wifi_dma_config();
	result = f_mount(&fs,"0:",1);
	
	/*----------------------- 格式化测试 ---------------------------*/  
	/* 如果没有文件系统就格式化创建创建文件系统 */
	if(result == FR_NO_FILESYSTEM)
	{
		printf("》SD卡还没有文件系统，即将进行格式化...\r\n");
    /* 格式化 */
		result=f_mkfs("0:",0,0);							
		
		if(result == FR_OK)
		{
			printf("》SD卡已成功格式化文件系统。\r\n");
      /* 格式化后，先取消挂载 */
			result = f_mount(NULL,"0:",1);			
      /* 重新挂载	*/			
			result = f_mount(&fs,"0:",1);
		}
		else
		{
			//LED_RED;
			printf("《《格式化失败。》》\r\n");
			while(1);
		}
	}
  else if(result!=FR_OK)
  {
    printf("！！SD卡挂载文件系统失败。(%d)\r\n",result);
    printf("！！可能原因：SD卡初始化不成功。\r\n");
		while(1);
  }
  else
  {
    printf("》文件系统挂载成功\r\n");
  }


//while(1){
//	if(eventFlag ==1){
//			eventFlag =0;
//			GPIO_SetBits(GPIOA,GPIO_Pin_0);  //拉高片选，开始接收数据
//			DMA_Cmd(DMA2_Stream2, ENABLE);	//打开接收AD数据的 DMA
//			while(DMA_GetITStatus(DMA2_Stream2,DMA_IT_TCIF2)==RESET); 
//			DMA_ClearITPendingBit(DMA2_Stream2,DMA_IT_TCIF2);	
//			DMA_Cmd(DMA2_Stream2, DISABLE);	//打开接收AD数据的 DMA

//			GPIO_ResetBits(GPIOA,GPIO_Pin_0);  //拉低片选
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
						sendADCmd(WIFI_RX_Buff,16); //发送ADC指令
						Delay(0xfff);
						if(WIFI_RX_Buff[13]==0x00 && WIFI_RX_Buff[14]==0x00 && WIFI_RX_Buff[15]==0x01)  //单帧采集
						{
							printf("单帧采集\r\n");
							receiveAD();
						
						}
						else if(WIFI_RX_Buff[13]==0x00 && WIFI_RX_Buff[14]==0x00 && WIFI_RX_Buff[15]==0x08){  //上发最新采集数据
							
							printf("上发最新采集数据\r\n");
							receiveAD();
							upload();
						
						}
					
						else if(WIFI_RX_Buff[13]==0x00 && WIFI_RX_Buff[14]==0x00 && WIFI_RX_Buff[15]==0x05){  //ADC配置
						
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

							
							printf("ADC配置\r\n");
							printf("采样点数%dk\r\n",samplePoints);

							
						}
						
						else if(WIFI_RX_Buff[13]==0x00 && WIFI_RX_Buff[14]==0x00 && WIFI_RX_Buff[15]==0x09){
						printf("上传历史文件\r\n");
							sprintf(fileName,"0:/ad/ad%03d.bin",WIFI_RX_Buff[4]);//目前只使用文件序号
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

		result = f_opendir(&dir,ADDIR);/* 打开指定文件夹 */
		while(result != FR_OK)
		{  
				f_mkdir(ADDIR);
				result = f_opendir(&dir,ADDIR);						
		}	
		for(i=1;i<999;i++)
		{
				sprintf(fileName,"0:/ad/ad%03d.bin",i);
				result=f_open(&file,(const TCHAR *)fileName,FA_READ);/* 以追加方式打开指定文件 */
				if(result==FR_OK)
					f_close(&file);	// 关闭文件
				else if(result==FR_NO_FILE)	
				break;					
		}
		f_close(&file);	// 关闭文件
		
		if(i==999)	/* 创建文件失败 */
		{   
			printf("创建wav文件失败。。。\r\n");
			result = f_closedir(&dir);	/* 关闭文件夹 */
		//	eventFlag = 0;	/* 查询:空闲 */
			return;
		}

		result=f_open(&file,fileName,FA_CREATE_ALWAYS|FA_WRITE);	/* 创建指定名字文件，只写 */
		if(result==FR_OK) 	
		{ 
				printf("创建wav文件成功，文件名：%s\r\n",fileName);
			
		}	
		else
		{
				printf("Open wavfile fail!!!->%d\r\n",result);	
				//退出这个函数的查询

				result = f_close (&file);	/* 关闭文件 */
				result = f_closedir(&dir);	/* 关闭文件夹 */
			//	eventFlag=0;							/* 查询:空闲 */
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
		

		printf("开始接收AD数据!\r\n");
		DMA_Cmd(DMA2_Stream2, ENABLE);	//打开接收AD数据的 DMA

		int dataCount = 1024;
		
		while(1){
			
			GPIO_SetBits(GPIOA,GPIO_Pin_0);  //拉高片选，开始接收数据
			while(DMA_GetITStatus(DMA2_Stream2,DMA_IT_TCIF2)==RESET); 
			DMA_ClearITPendingBit(DMA2_Stream2,DMA_IT_TCIF2);	
			
			GPIO_ResetBits(GPIOA,GPIO_Pin_0);  //拉低片选
	//		DMA_Cmd(DMA2_Stream2, DISABLE);	//打开接收AD数据的 DMA
			//大小端转换
//			for( i =0;i<AD_BUFF_SIZE;i+=2)
//			{
//					uint8_t temp = AD_RX_Buff[i];
//					AD_RX_Buff[i] = AD_RX_Buff[i+1];
//					AD_RX_Buff[i+1] = temp;
//			}	
//			
			
			result=f_write(&file,AD_RX_Buff,AD_BUFF_SIZE,&fnum);	/* 写入数据 */
				//Delay(0xfffff);  
			dataCount+=AD_BUFF_SIZE;
			//printf("%d ",dataCount);
			if(dataCount == sum+1024) 
			{
				result = f_close (&file);			
				result = f_closedir(&dir);
				printf("AD数据写入完成!\r\n");

				break;

			}
		
		}	
		DMA_Cmd(DMA2_Stream2, DISABLE);	//打开接收AD数据的 DMA

		
		sendADCmd("stopstopstopstop",16); //发送adc指令



}




void upload(void){
			int count=0,endflag =0;
	result=f_open(&file,fileName,FA_READ); 
			if(result!=FR_OK)
			{
					printf("打开文件失败!!!->%d\r\n",result);
					result = f_close (&file);
					return;			
			}
			else
			{
					printf("打开文件成功!!!->\r\n");
			}

			
			while(1){
				result = f_read(&file,WIFI_TX_Buff,WIFI_BUFF_SIZE,&fnum);		
				if(result!=FR_OK){
						printf("读取第%d帧失败！！\r\n",count);
						printf("!!上传结束\r\n");						
						count=0;								// 重置上传帧数量 
						printf("关闭文件\r\n");
						result = f_close(&file);		// 关闭文件
						break;
				
				}
				if(file.fptr==file.fsize)  // 成功读取到尾(读取即将完成) 
				{		
						endflag =1;
				}
				
					printf("上传第%0005d帧\r\n",++count);
					GPIO_ResetBits(GPIOH,GPIO_Pin_14);//WIFI FB 2 LOW
				  Delay(0x12222);	
					while(GPIO_ReadInputDataBit(GPIOH,GPIO_Pin_15)==1);
					spiTransmit();
					GPIO_SetBits(GPIOH,GPIO_Pin_14);				
					while(GPIO_ReadInputDataBit(GPIOH,GPIO_Pin_15)==0);  //等待wifi上传完成

					if(endflag ==1){
						printf("!!上传结束\r\n");						
						count=0;								// 重置上传帧数量 
						printf("关闭文件\r\n");
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
	
		DMA_Cmd(DMA1_Stream3, DISABLE);	//关闭接受 DMA	
		DMA_Cmd(DMA1_Stream4, DISABLE);		
	
  	DMA_SetCurrDataCounter(DMA1_Stream3,1024);	//设置发送的数据量    
  	DMA_SetCurrDataCounter(DMA1_Stream4,1024);	//设置发送的数据量    
	  SPI2->DR;		//清空数据	  	

		DMA_Cmd(DMA1_Stream3, ENABLE);	
		DMA_Cmd(DMA1_Stream4, ENABLE);

		while(DMA_GetITStatus(DMA1_Stream3,DMA_IT_TCIF3)==RESET);
		DMA_ClearFlag(DMA1_Stream3,DMA_IT_TCIF3);	//擦除DMA标志位
		DMA_ClearFlag(DMA1_Stream4,DMA_IT_TCIF4);	//擦除DMA标志位

		return ;
}



void sendADCmd(char cmd[],int size){
			
			memcpy(AD_TX_Buff,cmd,size);		
			DMA_SetCurrDataCounter(DMA2_Stream5,AD_BUFF_SIZE);	//设置发送的数据量    

			DMA_Cmd(DMA2_Stream5, ENABLE);	

			GPIO_SetBits(GPIOA,GPIO_Pin_1);  
			while(DMA_GetITStatus(DMA2_Stream5,DMA_IT_TCIF5)==RESET); 
			DMA_ClearITPendingBit(DMA2_Stream5,DMA_IT_TCIF5);	
			DMA_ClearITPendingBit(DMA2_Stream2,DMA_IT_TCIF2);	

			DMA_Cmd(DMA2_Stream5, DISABLE);
			GPIO_ResetBits(GPIOA,GPIO_Pin_1);  //拉低片选
			SPI1->DR;  //重要


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

