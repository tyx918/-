
#ifndef __BSP_SPI_DMA_H
#define __BSP_SPI_DMA_H

#include "stm32f4xx.h"
#include <stdio.h>

#define AD_BUFF_SIZE 8*1024*2
#define WIFI_BUFF_SIZE 1024

void spi_ad_dma_config();    
void spi_wifi_dma_config();
void  DMA_Write_buf();

#endif /* __BSP_SPI_DMA_H */

