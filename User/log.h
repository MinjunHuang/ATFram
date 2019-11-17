#ifndef __LOG_H_
#define __LOG_H_

#include "stm32f1xx_hal.h"
#include "usart.h"
#include "stdlib.h"

#define DEBUG 1


#if DEBUG

#define __LOGNOLF(format, ...) do{		\
								char * UartTXBuff = (char *)calloc(128,sizeof(char)); \
								sprintf(UartTXBuff,"Tick:%d >> "format,HAL_GetTick(), ##__VA_ARGS__); \
								strrpc(UartTXBuff,"\r\n","-*");	\
								UartTXBuff[strlen(UartTXBuff)]='\n';		\
								HAL_UART_Transmit(&huart1, (uint8_t *)UartTXBuff, strlen(UartTXBuff), 1000);	\
								free(UartTXBuff);\
								}while(0u)

#define __LOG(format, ...) printf("Tick:%d >> "format"\n",HAL_GetTick(), ##__VA_ARGS__)
#define __LOGARRAY(array,n,str)	do									\
								{	printf("Log %s array >>:",str);	\
									for(int i=0;i<n;i++)			\
									{								\
									printf("0x%02x ",(array)[i]);	\
									}								\
									printf("\n");	\
								}while(0u)
#else
#define __LOG(format, ...)
#endif

#define __ERRORLOG(format,...)  printf("\nERROR LOG \nTick:%d >>File:" __FILE__ " Line:%d\nERROR LOG INFO >>"format"\n\n",HAL_GetTick(), __LINE__, ##__VA_ARGS__)   

void strrpc(char *str,char *oldstr,char *newstr);
void StrNoLR(char * str);

#endif

