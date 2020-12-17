
#ifndef __BSP_SPI_DMA_H
#define __BSP_SPI_DMA_H

#include "stm32f4xx.h"
#include <stdio.h>

static int total = 4;

//#define AD_BUFF_SIZE total //1通道
//#define AD_BUFF_SIZE  13*1024*2 //4通道
//#define AD_BUFF_SIZE 3*1024*2 //3通道
#define WIFI_BUFF_SIZE 1024

void spi_ad_dma_config();    
void spi_wifi_dma_config();
void  DMA_Write_buf();

#endif /* __BSP_SPI_DMA_H */

