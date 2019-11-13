#ifndef _AT_H_
#define _AT_H_

#include "Hardware.h"
#include "ATCommand.h"
#include "FreeRTOS.h"

extern QueueHandle_t ATcmdQueue ;

void ATCommandRegister(eATCommand	ATCommandName);
ATStatus CheckATCmdConsistency(void);
ATStatus ATFormInit(void);

void ATCommandSendScheduler(void);


#endif

