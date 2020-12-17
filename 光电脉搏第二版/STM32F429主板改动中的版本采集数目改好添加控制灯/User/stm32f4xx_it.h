/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.h 
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    06-March-2015
  * @brief   This file contains the headers of the interrupt handlers.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4xx_IT_H
#define __STM32F4xx_IT_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

#ifdef __cplusplus
}
#endif

#define USART_REC_LEN2  300  //定义最大接收字节数
extern u8  USART2_RX_BUF[USART_REC_LEN2];	//stm32接受串口缓存
extern u16 USART2_RX_lenth;									//接受长度
extern u8  USART2_RX_Status;									//接受标志


#define UART_REC_LEN4  300  //定义最大接收字节数
extern u8  UART4_RX_BUF[UART_REC_LEN4];     //接收缓冲,最大USART_REC_LEN个字节.
extern u8  UART4_TX_BUF[UART_REC_LEN4]; 		//发送缓冲,最大USART4_MAX_SEND_LEN字节
extern u16 UART4_RX_STA;        //接收状态标记	
extern u8  UART4_Rev;

#define USART_REC_LEN6  300  //定义最大接收字节数
extern u8  USART6_RX_BUF[USART_REC_LEN6];     //接收缓冲,最大USART_REC_LEN个字节.
extern u8  USART6_TX_BUF[USART_REC_LEN6]; 		//发送缓冲,最大USART4_MAX_SEND_LEN字节
extern u16 USART6_RX_STA;        //接收状态标记	
extern u8  USART6_Rev;

#endif /* __STM32F4xx_IT_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
