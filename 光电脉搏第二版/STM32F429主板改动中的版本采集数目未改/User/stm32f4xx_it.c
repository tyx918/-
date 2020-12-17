/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    06-March-2015
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "./usart/bsp_debug_usart.h"
#include "./systick/bsp_SysTick.h"
#include "./433/433.h"
#include <string.h>

/** @addtogroup STM32F429I_DISCOVERY_Examples
  * @{
  */

/** @addtogroup FMC_SDRAM
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
//void SysTick_Handler(void)
//{}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f429_439xx.s).                         */
/******************************************************************************/
void DEBUG_USART_IRQHandler(void)
{
		uint16_t RES;
	
	
		extern uint16_t US_RX_BUF[16];
		static int CNT=0;	


		if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET) 
		{ 

			
			RES = USART_ReceiveData(UART4); 
		
		
			US_RX_BUF[CNT] = RES;
			
			printf("%d ",US_RX_BUF[CNT]);
			
			CNT++;  
			if( CNT>16 )
			CNT=0;  
				
		}
//			USART_ClearITPendingBit(USART1, USART_IT_RXNE);	
//		if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 
//		{ 
//			 USART_ClearITPendingBit(USART1, USART_IT_RXNE);
//		}	
}


/***************************
		stm32
***************************/
u8  USART2_RX_BUF[USART_REC_LEN2];     //���ջ���,���USART_REC_LEN���ֽ�.
u8  USART2_TX_BUF[USART_REC_LEN2]; 		//���ͻ���,���USART4_MAX_SEND_LEN�ֽ�
u16 USART2_RX_STA=0;        //����״̬���	
u8  USART2_Rev = 0;
//void DEBUG_USART2_IRQHandler(void)
//{
//	u8 res;
//	if(USART_GetFlagStatus(USART2,USART_FLAG_ORE) == SET)
//	{
//		printf("USART2����ж�\r\n");
//		USART2->SR;
//		USART2->DR;
//	}
//	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�
//	{
//		res = USART_ReceiveData(USART2);
//		if((USART2_RX_STA & 0x8000) == 0)
//		{
//			USART2_RX_BUF[USART2_RX_STA&0X3FFF]=res;
//			USART2_RX_STA++; 
//		}
//		USART_ClearFlag(USART2,USART_IT_RXNE); //��������ж�
//  }	
//	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)  //�����ж�
//	{
//		USART2->SR;
//		USART2->DR; //��SR�ٶ�DR�����IDLE�жϱ�־
//		
//		
////		printf("%s\r\n", USART2_RX_BUF);
//		printf("%c", USART2_RX_BUF[USART2_RX_STA-1]);
//		if(USART2_RX_BUF[0]=='$' && USART2_RX_BUF[1]=='4')
//		{
//			printf("���ͣ�%s\r\n", &USART2_RX_BUF[2]);
//			printf6((char *)&USART2_RX_BUF[2]);
//		}
//		memset(USART2_RX_BUF, 0, sizeof(USART2_RX_BUF));
//		USART2_RX_STA = 0;
//  }	
//}	



/***************************
		433�����жϽ���
***************************/
u8  USART6_RX_BUF[USART_REC_LEN6];     //���ջ���,���USART_REC_LEN���ֽ�.
u8  USART6_TX_BUF[USART_REC_LEN6]; 		//���ͻ���,���USART4_MAX_SEND_LEN�ֽ�
u16 USART6_RX_STA=0;        //����״̬���	
u8  USART6_Rev = 0;
void DEBUG_USART6_IRQHandler(void)                	//����1�жϷ�����򣬽�������Э��0x0d��0x0a���ĳɡ�\����0��������,
{
	u8 res;
	if(USART_GetFlagStatus(USART6,USART_FLAG_ORE) == SET)
	{
		//printf("USART6����ж�\r\n");
		USART6->SR;
		USART6->DR;
	}
	if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		res = USART_ReceiveData(USART6);
		if((USART6_RX_STA & 0x8000) == 0)
		{
			printf("res: %X\r\n", res);
			USART6_RX_BUF[USART6_RX_STA&0X3FFF]=res;
			USART6_RX_STA++; 
		}
  }	
	if(USART_GetITStatus(USART6, USART_IT_IDLE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		USART6->SR;
		USART6->DR; //��SR�ٶ�DR�����IDLE�жϱ�־
		USART6_RX_STA = 0;
		
		memcpy(RXDATA_433, USART6_RX_BUF, USART_REC_LEN6);
		memset(USART6_RX_BUF, 0, sizeof(USART6_RX_BUF));
		RXDATA_433_STATUS = 1; //433���ܱ�־
  }
	
//	if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
//	{
//		res = USART_ReceiveData(USART6);	//��ȡ���յ�������
////		printf("res: %X\r\n", res);
//		if((USART6_RX_STA&0x8000)==0)//����δ���
//		{
//			if(USART6_RX_STA&0x4000)//���յ���0x0d
//			{
//				if(res!=0x0a) USART6_RX_STA=0;//���մ���,���¿�ʼ
//				else USART6_RX_STA|=0x8000;	//��������� 
//			}
//			else //��û�յ�0X0D
//			{	
//				if(res==0x0d)USART6_RX_STA|=0x4000;
//				else
//				{
//					USART6_RX_BUF[USART6_RX_STA&0X3FFF]=res ;
//					USART6_RX_STA++;
//					if(USART6_RX_STA>(USART_REC_LEN6-1))USART6_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
//				}		 
//			}
//		}
//	}
//	
//	if(USART6_RX_STA&0X8000) 
//	{
//		USART6_RX_STA = 0;
//    printf("USART6_RX_STA: %s\r\n",USART6_RX_BUF);
//		memcpy(RXDATA_433, USART6_RX_BUF, USART_REC_LEN6);
//		memset(USART6_RX_BUF, 0, sizeof(USART6_RX_BUF));
//		RXDATA_433_STATUS = 1; //433���ܱ�־
//	} 
}




void SysTick_Handler(void)
{
	TimingDelay_Decrement();
}
	
/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
