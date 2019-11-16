#ifndef __HARDWARE_H_
#define __HARDWARE_H_

#include "FreeRTOS.h"
#include "string.h"
#include "AT.h"

#include "usart.h"


#define _UART_DMA_HANDLE hdma_usart2_rx
#define _ATUART_HANDLE huart2

#define _UART_TIMEOUT 1000
#define _UART_RXBUFFSIZE 128

extern DMA_HandleTypeDef _UART_DMA_HANDLE;
extern char UartRXBuff[_UART_RXBUFFSIZE];


void UartInit(void);
void SendString(char * str);
void MoudleResst(void);
void USER_UART_IDLECallback(UART_HandleTypeDef *huart);

#endif
