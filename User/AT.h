#ifndef _AT_H_
#define _AT_H_

#include "Hardware.h"
#include "ATCommand.h"
#include "cmsis_os.h"

extern QueueHandle_t 		ATcmdQueue ;
extern SemaphoreHandle_t  	ATRXCplSemaphore;

typedef struct {
	ATCommand 	RegCommandName;
	CmdType		RegCommandType;
	char *		RegCommandParam;
}ATCommandRegInfo;

void ATCommandRegister(ATCommand ATCommandName,CmdType ATCommandType,char* ATCommandParam);
ATStatus CheckATCmdConsistency(void);
ATStatus ATFormInit(void);

ATStatus CheckEcho(ATCommandRegInfo SendATInfo,char * str,char ** ReStr);
ATStatus CheckEnd(char *str);

void ATCommandSendScheduler(void);


#endif

