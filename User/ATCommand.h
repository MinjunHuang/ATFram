#ifndef _ATCOMMAND_H_
#define _ATCOMMAND_H_

#include "ATCommand.h"
#include "stdint.h"
#include "log.h"

typedef enum 
{
	ATERROR = 0, 
	ATSUCCESS ,
	ATERRORCODE1,
}ATStatus;

typedef ATStatus (*pFuncCallback)(char* SendCommand,char* str);

ATStatus AT_Callback(char* SendCommand,char * str);
ATStatus CGSN_Callback(char* SendCommand,char * str);

ATStatus CheckEcho(char* SendCommand,char * str);
ATStatus CheckEnd(void);

typedef enum 
{
	EXEXCMD,
	TESTCMD,
	READCMD,
	WRITECMD,
}CmdType;

typedef enum 
{
	AT=0,
	CGSN,
	
	/**将指令添加到上面**/
	MAXCMDNUM
}eATCommand;

typedef struct
{		
	eATCommand	ATCommandName;
	char * 		ATStr;				//发送的AT指令
	CmdType		ATCommandType;		//命令类型
	uint16_t  	MaxResponseTime;	//发送后查询返回信息的延时，ms为单位。可设为指令最大响应时间。
	uint8_t   	MaxTryCount; 		//最大重试次数
	uint8_t   	MaxResetCount; 		//最大重启次数
	pFuncCallback		ATRxCpltCallback;	//AT指令接收完成，指令处理回调函数
} ATCommandConfig;


static const ATCommandConfig ATCommandList[]=
{
	{AT,	"AT\r\n",	EXEXCMD,500,5,3,AT_Callback		},
	{CGSN,	"AT+CGSN=1\r\n",EXEXCMD,500,5,3,CGSN_Callback	},
};

#endif
