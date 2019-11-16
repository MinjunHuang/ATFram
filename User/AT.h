#ifndef _AT_H_
#define _AT_H_

#include "Hardware.h"
#include "ATCommand.h"
#include "cmsis_os.h"

extern QueueHandle_t 		ATcmdQueue ;
extern SemaphoreHandle_t  	ATRXCplSemaphore;

void ATCommandRegister(eATCommand	ATCommandName);
ATStatus CheckATCmdConsistency(void);
ATStatus ATFormInit(void);

ATStatus CheckEcho(char* SendCommand,char * str,char ** ReStr);
ATStatus CheckEnd(char *str);

void ATCommandSendScheduler(void);


#endif

