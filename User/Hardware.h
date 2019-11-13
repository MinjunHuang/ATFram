#ifndef __HARDWARE_H_
#define __HARDWARE_H_

#include "main.h"
#include "string.h"

#define _UART_DMA_HANDLE hdma_usart2_rx
#define _ATUART_HANDLE huart2

#define _UARTTIMEOUT 1000
#define _UART_RXBUFFSIZE 128


extern DMA_HandleTypeDef _UART_DMA_HANDLE;

void UartInit(void);
void SendString(char * str);
void USER_UART_IDLECallback(UART_HandleTypeDef *huart);

#endif
