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
#include "./systick/bsp_SysTick.h"
#include <stdlib.h>
#include "./433/433.h"

#include "ff.h"

#define ADDIR	"0:/ad"	

#define LittletoBig16(A)   (   (( (uint16_t)(A) & 0x00ff) << 8)| ( ((uint16_t)(A) & 0xff00) >> 8))

FATFS fs;													/* FatFs文件系统对象 */
FRESULT result;                /* 文件操作结果 */
FIL file;													/* 文件对象 */
UINT fnum;            					  /* 文件成功读写数量 */
DIR dir; 

int selectedNums = 0;
char fileName[100];//文件名

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
	
uint16_t US_RX_BUF[16]={0};
//extern uint8_t AD_RX_Buff[AD_BUFF_SIZE];//ad的接收buff,arm为从机，cpld为主机
//extern uint8_t AD_TX_Buff[AD_BUFF_SIZE];//ad的发送buff
extern uint8_t *AD_RX_Buff;//ad的接收buff,arm为从机，cpld为主机
extern uint8_t *AD_TX_Buff;//ad的发送buff
int ad_BUFF_SIZE = 0;

extern int eventFlag;
extern uint8_t WIFI_TX_Buff[WIFI_BUFF_SIZE];//wifi的发送内容
extern uint8_t WIFI_RX_Buff[WIFI_BUFF_SIZE];//wifi的接收内容

static void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}
void receiveAD(void);//接收ad采集函数
void upload(void);//上传文件函数

void reverseEndian(uint8_t *little,int size);
void spiTransmit();//spi传输
void sendADCmd(char cmd[],int size);//待定


double  samplePoints =4;//采样点
char sampleRate[2]={0x02,0xc1}; //默认采样率设置  0.705MHZ
char fileSize[4]={0x00}; //文件大小 
void dectohex(int dec,char result[]);//转换为16进制




int main(void)
{
//	uint8_t *AD_RX_Buff = (uint8_t*) malloc (4 * 2*1024 * sizeof(int));
//	uint8_t *AD_TX_Buff = (uint8_t*) malloc (4 * 2*1024 * sizeof(int));
	int i=0;
	int j=0;
	__IO uint32_t uwData[16]= {0};
	Delay(0xffffff);
  /*初始化USART，配置模式为 115200 8-N-1*/
	//Debug_USART_Config();//串口通信渲门,初始串口2
	 SysTick_Init();	
	USART_Config();
	Delay_ms(500);
	MD_Init();
	printf("START\r\n");
	Set433Config(airspeed[3]); // 设置433配置
	printf("line88\r\n");
	GPIO_Config();	//GPIO配置
	spi_ad_dma_config(); //使用spi1，arm为主机，cpld为从机
	spi_wifi_dma_config();//arm为从机，wifi为主机，使用cpld2
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
//		while(1);
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
						if(WIFI_RX_Buff[13]==0x00 && WIFI_RX_Buff[14]==0x00 && WIFI_RX_Buff[15]==0x01)  //单帧采集
						{
							sendADCmd(WIFI_RX_Buff,16); 
							printf("单帧采集\r\n");
							receiveAD();//接收ad的数据，把adrxbuff的值传给文件
						}
						else if(WIFI_RX_Buff[13]==0x00 && WIFI_RX_Buff[14]==0x00 && WIFI_RX_Buff[15]==0x08){  //上发最新采集数据
							sendADCmd(WIFI_RX_Buff,16); 
							printf("上发最新采集数据\r\n");
							receiveAD();
							upload();//wifi上发文件
						}
						else if(WIFI_RX_Buff[13]==0x00 && WIFI_RX_Buff[14]==0x00 && (WIFI_RX_Buff[15]==0x05||WIFI_RX_Buff[14]==0x00)){  //ADC配置
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
							//计算被选中的通道总数
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
							sendADCmd(WIFI_RX_Buff,16); //发送ADC指令给从机
							printf("selectedNums= %d\r\n",selectedNums);
							
							printf("ADC配置\r\n");
							printf("采样点数%fk\r\n",samplePoints);	
						}	
						else if(WIFI_RX_Buff[13]==0x00 && WIFI_RX_Buff[14]==0x00 && WIFI_RX_Buff[15]==0x09){
						printf("上传历史文件\r\n");
							sprintf(fileName,"0:/ad/ad%03d.bin",WIFI_RX_Buff[4]);//目前只使用文件序号
							printf("%s",fileName);
							upload();
						}
						//转发灯指令
						else if(WIFI_RX_Buff[13]==0x12 && WIFI_RX_Buff[14]==0x34 && WIFI_RX_Buff[15]==0x56){
							//printf("转发指令");
							sendADCmd(WIFI_RX_Buff,16);
						}
						//转发亮度调节指令，待写
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
		//int sum = samplePoints *2*8*1024;//这里设置的是总共采样的点数
		//int sum = samplePoints *2*8*1024;//这里设置的是总共采样的点数
		int sum = samplePoints*4*selectedNums*1024;//这里设置的是总共采样的点数.
    //int sum = samplePoints *2*9*1024;//这里设置的是总共采样的点数
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
		//第七位header[6]代表的是有几个通通header[6]
		//第七位header[8]代表的是有几个通通header[6]
		//char header[1024]={0x41,0x44,0x43,0x03,0x00,0x02,0xc1,0x01,0xFF,0x00,0x0C,0x80,0x00,0x43,0x44,0x41};
		char header[1024]={0x41,0x44,0x43,0x03,0x00,0x02,0xc1,0x01,0x07,0x00,0x0C,0x80,0x00,0x43,0x44,0x41};

		header[3] = 0x01;
		header[8] = 0x01;
		switch(selectedNums) {
			case 1:		
				header[3] = 0x01;//存储其采集的是几个通道被选通的信息
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
		header[5] = sampleRate[0];//把采样率给头1024字节中
		header[6] = sampleRate[1];
		dectohex(sum,fileSize);//把sum转换成16进制，分别存到头1024字节中
		header[9] = fileSize[0];
		header[10] = fileSize[1];
		header[11] = fileSize[2];
		header[12] = fileSize[3];
		
		result=f_write(&file,header,1024,&fnum);//写入头文件钱1024个字节的数据
		

		printf("开始接收AD数据!\r\n");
		DMA_Cmd(DMA2_Stream2, ENABLE);	//打开接收AD数据的 DMA

		int dataCount = 1024;
		
		while(1){
			
			GPIO_SetBits(GPIOA,GPIO_Pin_0);  //拉高片选，开始接收数据
//			printf("line375\r\n");
			while(DMA_GetITStatus(DMA2_Stream2,DMA_IT_TCIF2)==RESET); 
//			printf("line377\r\n");
			DMA_ClearITPendingBit(DMA2_Stream2,DMA_IT_TCIF2);	
//			printf("line379\r\n");
			
			GPIO_ResetBits(GPIOA,GPIO_Pin_0);  //拉低片选
//			printf("line3382\r\n");
	//		DMA_Cmd(DMA2_Stream2, DISABLE);	//打开接收AD数据的 DMA
			//大小端转换
//			for( i =0;i<AD_BUFF_SIZE;i+=2)
//			{
//					uint8_t temp = AD_RX_Buff[i];
//					AD_RX_Buff[i] = AD_RX_Buff[i+1];
//					AD_RX_Buff[i+1] = temp;
//			}	
//			
			
			result=f_write(&file,AD_RX_Buff,ad_BUFF_SIZE,&fnum);	/* 写入数据 */
				//Delay(0xfffff);  
			dataCount+=ad_BUFF_SIZE;//adcount == 1024
			
//      printf("sum+1024 = %d\r\n",sum+1024);
				printf("dataCount = %d\r\n",dataCount/1024);
//			printf("%d\r\n",dataCount);
      //当接收的数据等于设置的数据
			if(dataCount == sum+1024) 
			{
				result = f_close (&file);			
				result = f_closedir(&dir);
				printf("AD数据写入完成!\r\n");
				break;

			}
		
		}	
		DMA_Cmd(DMA2_Stream2, DISABLE);	//关闭接收AD数据的 DMA

		
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
	
		DMA_Cmd(DMA1_Stream3, DISABLE);	//关闭接受 DMA	
		DMA_Cmd(DMA1_Stream4, DISABLE);		
	
  	DMA_SetCurrDataCounter(DMA1_Stream3,1024);	//设置发送的数据量    
  	DMA_SetCurrDataCounter(DMA1_Stream4,1024);	//设置发送的数据量    
	  SPI2->DR;		//清空数据	  	

		DMA_Cmd(DMA1_Stream3, ENABLE);	//开启dma
		DMA_Cmd(DMA1_Stream4, ENABLE);  //开启DMA

		while(DMA_GetITStatus(DMA1_Stream3,DMA_IT_TCIF3)==RESET);
		DMA_ClearFlag(DMA1_Stream3,DMA_IT_TCIF3);	//擦除DMA标志位
		DMA_ClearFlag(DMA1_Stream4,DMA_IT_TCIF4);	//擦除DMA标志位

		return ;
}



void sendADCmd(char cmd[],int size){
	    printf("sendADCmd\r\n");		
	
			memcpy(AD_TX_Buff,cmd,size);		
			DMA_SetCurrDataCounter(DMA2_Stream5,ad_BUFF_SIZE);	//设置发送的数据量    

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

