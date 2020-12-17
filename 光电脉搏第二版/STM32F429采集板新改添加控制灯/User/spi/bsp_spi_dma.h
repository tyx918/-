
#ifndef __BSP_SPI_DMA_H
#define __BSP_SPI_DMA_H

#include "stm32f4xx.h"
#include <stdio.h>


//#define BuffSize 8*1024*2
//#define BuffSize 13*1024*2
void spi_dma_config(int);
void spi_dma_config1(int);
void SPI3_TX_DMA_Config();	//DMA∑¢ÀÕ≈‰÷√
void  DMA_Write_buf();
void SPI_DMA_WRITE_READ_BUFF();

#define WBuffSize = BuffSize * 2;

#endif /* __BSP_SPI_DMA_H */

