#ifndef __433_H
#define __433_H
#include "stm32f4xx.h"


extern uint8_t Receive_433;  //�յ�433��־
#define RX_433BUF_SIZE 1024
extern char RXDATA_433[];	//433���ܻ���
extern char 	RXDATA_433_STATUS;			    //433���ܱ�־

void MD_Init(void);
char* Rev433Cmd(void);
uint8_t send433Cmd(char * command);
void DataProc(const char *Source, long *Target);


void Check433Config();
void Set433Config(char conf);
extern u8 airspeed[4];


#endif

















