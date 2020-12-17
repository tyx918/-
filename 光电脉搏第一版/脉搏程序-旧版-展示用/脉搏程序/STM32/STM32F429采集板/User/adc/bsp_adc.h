#ifndef __BSP_ADC_H
#define	__BSP_ADC_H

#include "stm32f4xx.h"

#define RHEOSTAT_BUFFSIZE    50*1024

/*=====================通道1 IO======================*/
// PF6 ok
#define RHEOSTAT_ADC_GPIO_PORT1    GPIOA
#define RHEOSTAT_ADC_GPIO_PIN1     GPIO_Pin_3
#define RHEOSTAT_ADC_GPIO_CLK1     RCC_AHB1Periph_GPIOA
#define RHEOSTAT_ADC_CHANNEL1      ADC_Channel_3
/*=====================通道2 IO ======================*/
// PF7 ok
#define RHEOSTAT_ADC_GPIO_PORT2    GPIOF
#define RHEOSTAT_ADC_GPIO_PIN2     GPIO_Pin_6
#define RHEOSTAT_ADC_GPIO_CLK2     RCC_AHB1Periph_GPIOF
#define RHEOSTAT_ADC_CHANNEL2      ADC_Channel_4
/*=====================通道3 IO ======================*/
// PF8 ok	
#define RHEOSTAT_ADC_GPIO_PORT3    GPIOF
#define RHEOSTAT_ADC_GPIO_PIN3     GPIO_Pin_8
#define RHEOSTAT_ADC_GPIO_CLK3     RCC_AHB1Periph_GPIOF
#define RHEOSTAT_ADC_CHANNEL3      ADC_Channel_6
/*=====================通道4 IO======================*/
// PF9 ok
#define RHEOSTAT_ADC_GPIO_PORT4    GPIOF 
#define RHEOSTAT_ADC_GPIO_PIN4     GPIO_Pin_9
#define RHEOSTAT_ADC_GPIO_CLK4     RCC_AHB1Periph_GPIOF
#define RHEOSTAT_ADC_CHANNEL4      ADC_Channel_7
/*=====================通道5 IO ======================*/
// PC0
#define RHEOSTAT_ADC_GPIO_PORT5    GPIOC
#define RHEOSTAT_ADC_GPIO_PIN5     GPIO_Pin_0
#define RHEOSTAT_ADC_GPIO_CLK5     RCC_AHB1Periph_GPIOC
#define RHEOSTAT_ADC_CHANNEL5      ADC_Channel_10
/*=====================通道6 IO ======================*/
// PC1
#define RHEOSTAT_ADC_GPIO_PORT6    GPIOC
#define RHEOSTAT_ADC_GPIO_PIN6     GPIO_Pin_1
#define RHEOSTAT_ADC_GPIO_CLK6     RCC_AHB1Periph_GPIOC
#define RHEOSTAT_ADC_CHANNEL6      ADC_Channel_11
/*=====================通道7 IO ======================*/
// PC2
#define RHEOSTAT_ADC_GPIO_PORT7    GPIOC
#define RHEOSTAT_ADC_GPIO_PIN7     GPIO_Pin_2
#define RHEOSTAT_ADC_GPIO_CLK7     RCC_AHB1Periph_GPIOC
#define RHEOSTAT_ADC_CHANNEL7      ADC_Channel_12
/*=====================通道8 IO ======================*/
// PC3
#define RHEOSTAT_ADC_GPIO_PORT8    GPIOC
#define RHEOSTAT_ADC_GPIO_PIN8     GPIO_Pin_3
#define RHEOSTAT_ADC_GPIO_CLK8     RCC_AHB1Periph_GPIOC
#define RHEOSTAT_ADC_CHANNEL8      ADC_Channel_13



// ADC 序号宏定义
#define RHEOSTAT_ADC              ADC3
#define RHEOSTAT_ADC_CLK          RCC_APB2Periph_ADC3
// ADC DR寄存器宏定义，ADC转换后的数字值则存放在这里
#define RHEOSTAT_ADC_DR_ADDR    	((u32)ADC3+0x4c)


// ADC DMA 通道宏定义，这里我们使用DMA传输
#define RHEOSTAT_ADC_DMA_CLK      RCC_AHB1Periph_DMA2
#define RHEOSTAT_ADC_DMA_CHANNEL  DMA_Channel_2
#define RHEOSTAT_ADC_DMA_STREAM   DMA2_Stream0



void Rheostat_Init(void);
void Rheostat_ADC_Mode_Config(int samplingDelay);

#endif /* __BSP_ADC_H */



