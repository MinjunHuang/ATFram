#ifndef _ATCOMMAND_H_
#define _ATCOMMAND_H_

#include "log.h"
#include "ATtype.h"

typedef enum 
{
	ATERROR = 0, 
	ATSUCCESS ,
	ATERRORCODE1,
}ATStatus;

typedef enum 
{
	EXEXCMD,
	TESTCMD,
	READCMD,
	WRITECMD,
}CmdType;

typedef ATStatus (*pFuncCallback)(char* str);

typedef enum 
{
	AT=0,
	 CGSN,CSQ,CGATT,
	/**将指令添加到上面**/
	MAXCMDNUM
}ATCommand;

typedef struct
{		
	ATCommand	ATCommandName;
	char * 		ATStr;				//发送的AT指令
	uint16_t  	MaxResponseTime;	//发送后查询返回信息的延时，ms为单位。可设为指令最大响应时间。
	uint8_t   	MaxTryCount; 		//最大重试次数
	uint8_t   	MaxResetCount; 		//最大重启次数
	pFuncCallback		ATRxCpltCallback;	//AT指令接收完成，指令处理回调函数
} ATCommandConfig;


ATStatus AT_Callback(char * str);
ATStatus CGSN_Callback(char * str);
ATStatus CSQ_Callback(char *str);
ATStatus CGATT_Callback(char *str);

static const ATCommandConfig ATCommandList[]=
{
	{AT,	"AT"		,500,5,3,	NULL		},
	{CGSN,	"AT+CGSN=1"	,500,5,3,CGSN_Callback	},
	{CSQ,	"AT+CSQ"	,500,5,3,CSQ_Callback	},
	{CGATT,	"AT+CGATT"	,500,5,3,CGATT_Callback	},
};

#endif
