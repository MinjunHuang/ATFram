#include "ATCommand.h"
#include "string.h"

char DateHandleBuff[128];

ATStatus AT_Callback(char* SendCommand,char * str)
{
	
	return ATSUCCESS;
}

void CheckEcho(char* SendCommand,char * str)
{
	char *ret;
	int len=strlen(str);
	memcpy(DateHandleBuff,str,len);
	DateHandleBuff[len]=0;				//将字符串末尾置0
	//查找第一个换行符，进行回显检查
	ret = (char*)memchr(DateHandleBuff, '\n', strlen(str));
	*ret=0;								//将字符串末尾置0		
	__LOG("Echo is:%s",DateHandleBuff);
}

ATStatus CGSN_Callback(char* SendCommand,char * str)
{
	return ATSUCCESS;
}




