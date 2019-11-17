#include "ATCommand.h"
#include "string.h"
#include "log.h"

typedef struct 
{
	char* RxBuff;
	int   len;
}RxStrInfo;

char DateHandleBuff[128];
RxStrInfo RxHandleInfo;


ATStatus CGSN_Callback(char * str)
{	
	return ATSUCCESS;
}

ATStatus CSQ_Callback(char *str)
{	
	return ATSUCCESS;
}

ATStatus CGATT_Callback(char *str)
{
	return ATSUCCESS;
}
