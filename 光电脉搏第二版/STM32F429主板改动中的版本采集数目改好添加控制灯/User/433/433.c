/*
ʹ��˵����
Mode_433 = 0: �ر�433����
Mode_433 = 1: ����433����
ZC_Flag  = 0: 433��ʼ״̬
ZC_Flag  = 1: 433����
ZC_Flag  = 2: 433�ӻ�
Connected433 = 0:
Connected433 = 1:

******************************************************
1�����ų�ʼ����main������ʼ��ʱ���ã���
	MD_Init();

4������433ָ���Ҫʱ���ã���
	send433Cmd();

5����ȡ�յ���433ָ���Ҫʱ���ã���
	Rev433Cmd()��
*/
#include "./433/433.h"
#include "string.h"
#include "./usart/bsp_debug_usart.h"
#include "stm32f4xx_it.h"
#include "./systick/bsp_SysTick.h"
#include "./433/433.h"

//��ģ�������Ϣ
char  RXDATA_433[RX_433BUF_SIZE];	//433���ܻ���
char 	RXDATA_433_STATUS;			    //433���ܱ�־

//433 MD���ų�ʼ��
void MD_Init()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(DEBUG_USART1_RX_GPIO_CLK|DEBUG_USART1_TX_GPIO_CLK,ENABLE);  //MDx��, UART_1_TX, PA9
	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Pin = DEBUG_USART1_TX_PIN  ;  
  GPIO_Init(DEBUG_USART1_TX_GPIO_PORT, &GPIO_InitStructure);
//	GPIO_WriteBit(GPIOA,GPIO_Pin_9,0);
}
static void MD_Low()
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_9, 0);
	printf("PA9 Bit:%d\r\n", GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_9));
}
static void MD_High()
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_9, 1);
	printf("PA9 Bit:%d\r\n", GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_9));
}


uint8_t send433Cmd(char * command)
{
	printf6(command);
	return 1;
}

char* Rev433Cmd()
{
//	if(USART6_RX_STA&0X8000) 
//	{
//		USART6_RX_STA = 0;
//		memcpy(RXDATA_433, USART6_RX_BUF, USART_REC_LEN6);
//		memset(USART6_RX_BUF, 0, sizeof(USART6_RX_BUF));
//		return (char*)RXDATA_433;
//	} 
//	else
//		return NULL;
	
	if(RXDATA_433_STATUS == 1)
	{
		RXDATA_433_STATUS = 0;
		return (char*)RXDATA_433;
	}
	else
		return NULL;
}

#include "stdlib.h"
void DataProc(const char *Source, long *Target)
{
	u8 V_L[6] = {0};
	u8 V_R[6] = {0};
	sscanf((char *)Source, "%*[^:]:%[^;];%[^;]", V_L, V_R); //V:123;456;
	printf(">> V_L:%s; V_R:%s\r\n", V_L, V_R);
	Target[0] = atoi((char*)V_L);
	Target[1] = atoi((char*)V_R);
}

void Delay(__IO uint32_t nCount)	        //�򵥵���ʱ����
{
    for(; nCount != 0; nCount--);
}
/*
	0x1A: 2.4k
	0x1B: 4.8k
	0x1C: 9.6k
	0x1D: 19.6k
*/
u8 airspeed[4] = {0x1A, 0x1B, 0x1C, 0x1D};
void Check433Config()
{
	char config1[4] = {0xC1, 0xC1, 0xC1, 0x00};
	char config2[4] = {0xC3, 0xC3, 0xC3, 0x00};
//	Delay_ms(50);
	MD_High();
	Delay_ms(50);
	send433Cmd(config1);
	Delay_ms(100);
	send433Cmd(config2);
	Delay_ms(50);
	MD_Low();
}
void Set433Config(char conf)
{
	char config[7] = {0xC0, 0x12, 0x34, conf, 0x17, 0x44, 0x00};
//	Delay_ms(50);
	MD_High();
	Delay_ms(100);
	send433Cmd(config);
	Delay_ms(100);
	MD_Low();
	
	Delay_ms(100);
	Check433Config();
}
