#ifndef __DEBUG_USART_H
#define	__DEBUG_USART_H

#include "stm32f4xx.h"
#include <stdio.h>

//串口1引脚定义
/*******************************************************/
#define DEBUG_USART1                             USART1
#define DEBUG_USART1_CLK                         RCC_APB2Periph_USART1
#define DEBUG_USART1_BAUDRATE                    115200  //串口波特率

#define DEBUG_USART1_RX_GPIO_PORT                GPIOA
#define DEBUG_USART1_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define DEBUG_USART1_RX_PIN                      GPIO_Pin_10
#define DEBUG_USART1_RX_AF                       GPIO_AF_USART1
#define DEBUG_USART1_RX_SOURCE                   GPIO_PinSource10

#define DEBUG_USART1_TX_GPIO_PORT                GPIOA
#define DEBUG_USART1_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define DEBUG_USART1_TX_PIN                      GPIO_Pin_9
#define DEBUG_USART1_TX_AF                       GPIO_AF_USART1
#define DEBUG_USART1_TX_SOURCE                   GPIO_PinSource9

#define DEBUG_USART1_IRQHandler                  USART1_IRQHandler
#define DEBUG_USART1_IRQ                 				USART1_IRQn
/************************************************************/

//串口2引脚定义
/*******************************************************/
#define DEBUG_USART2                             USART2
#define DEBUG_USART2_CLK                         RCC_APB1Periph_USART2
#define DEBUG_USART2_BAUDRATE                    115200  //串口波特率

#define DEBUG_USART2_RX_GPIO_PORT                GPIOD
#define DEBUG_USART2_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOD
#define DEBUG_USART2_RX_PIN                      GPIO_Pin_6
#define DEBUG_USART2_RX_AF                       GPIO_AF_USART2
#define DEBUG_USART2_RX_SOURCE                   GPIO_PinSource6

#define DEBUG_USART2_TX_GPIO_PORT                GPIOD
#define DEBUG_USART2_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOD
#define DEBUG_USART2_TX_PIN                      GPIO_Pin_5
#define DEBUG_USART2_TX_AF                       GPIO_AF_USART2
#define DEBUG_USART2_TX_SOURCE                   GPIO_PinSource5

#define DEBUG_USART2_IRQHandler                  USART2_IRQHandler
#define DEBUG_USART2_IRQ                 				USART2_IRQn
/************************************************************/


////串口波特率
//#define DEBUG_USART2_BAUDRATE                    115200

//串口6引脚定义
/*******************************************************/
#define DEBUG_USART6                             USART6
#define DEBUG_USART6_CLK                         RCC_APB2Periph_USART6
#define DEBUG_USART6_ClockCmd                    RCC_APB2PeriphClockCmd
#define DEBUG_USART6_BAUDRATE                    9600 //串口波特率

#define DEBUG_USART6_RX_GPIO_PORT                GPIOG
#define DEBUG_USART6_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOG
#define DEBUG_USART6_RX_PIN                      GPIO_Pin_9
#define DEBUG_USART6_RX_AF                       GPIO_AF_USART6
#define DEBUG_USART6_RX_SOURCE                   GPIO_PinSource9

#define DEBUG_USART6_TX_GPIO_PORT                GPIOC
#define DEBUG_USART6_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOC
#define DEBUG_USART6_TX_PIN                      GPIO_Pin_6
#define DEBUG_USART6_TX_AF                       GPIO_AF_USART6
#define DEBUG_USART6_TX_SOURCE                   GPIO_PinSource6

#define DEBUG_USART6_IRQHandler                  USART6_IRQHandler
#define DEBUG_USART6_IRQ                 				 USART6_IRQn
/************************************************************/

//串口初始化
void USART_Config(void);
void printf4(char* fmt,...);
void printf6(char* fmt,...);



void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
void Usart_SendString( USART_TypeDef * pUSARTx, char *str);

void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch);


void Debug_USART_Config(void);
//int fputc(int ch, FILE *f);

#endif /* __USART1_H */
