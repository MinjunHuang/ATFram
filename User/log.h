#ifndef __LOG_H_
#define __LOG_H_

#include "stm32f1xx_hal.h"

#define DEBUG 1


#if DEBUG
#define __LOG(format, ...) printf("Tick:%d >> "format"\n",HAL_GetTick(), ##__VA_ARGS__)
#else
#define LOG(format, ...)
#endif

#define __ERRORLOG(format,...)  printf("\nERROR LOG \nTick:%d >>File:" __FILE__ " Line:%d\nERROR LOG INFO >>"format"\n\n",HAL_GetTick(), __LINE__, ##__VA_ARGS__)   


#endif

