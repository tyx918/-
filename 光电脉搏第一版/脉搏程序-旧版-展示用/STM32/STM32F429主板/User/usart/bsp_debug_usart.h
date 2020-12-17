#ifndef __DEBUG_USART_H
#define	__DEBUG_USART_H

#include "stm32f4xx.h"
#include <stdio.h>



//引脚定义
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


//串口波特率
#define DEBUG_USART2_BAUDRATE                    115200



void Debug_USART_Config(void);
//int fputc(int ch, FILE *f);

#endif /* __USART1_H */
