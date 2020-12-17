
#ifndef __BSP_SPI_DMA_H
#define __BSP_SPI_DMA_H

#include "stm32f4xx.h"
#include <stdio.h>

#define BuffSize 8*1024*2
void spi_dma_config();                          
void  DMA_Write_buf();
void SPI_DMA_WRITE_READ_BUFF();

#endif /* __BSP_SPI_DMA_H */

