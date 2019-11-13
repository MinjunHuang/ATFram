#ifndef __HARDWARE_H_
#define __HARDWARE_H_

#include "main.h"
#include "string.h"
#include "AT.h"

#define DEBUG 1

#if DEBUG
#define __LOG(format, ...) printf("Tick:%d >> "format"\n",HAL_GetTick(), ##__VA_ARGS__)
#else
#define LOG(format, ...)
#endif

#define __ERRORLOG(format,...)  printf("\nERROR LOG \nTick:%d >>File:" __FILE__ " Line:%d\nERROR LOG INFO >>"format"\n\n",HAL_GetTick(), __LINE__, ##__VA_ARGS__)   

#define _UART_DMA_HANDLE hdma_usart2_rx
#define _ATUART_HANDLE huart2

#define _UART_TIMEOUT 1000
#define _UART_RXBUFFSIZE 128

extern DMA_HandleTypeDef _UART_DMA_HANDLE;

void UartInit(void);
void SendString(char * str);
void USER_UART_IDLECallback(UART_HandleTypeDef *huart);

#endif
