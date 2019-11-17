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
	/**��ָ����ӵ�����**/
	MAXCMDNUM
}ATCommand;

typedef struct
{		
	ATCommand	ATCommandName;
	char * 		ATStr;				//���͵�ATָ��
	uint16_t  	MaxResponseTime;	//���ͺ��ѯ������Ϣ����ʱ��msΪ��λ������Ϊָ�������Ӧʱ�䡣
	uint8_t   	MaxTryCount; 		//������Դ���
	uint8_t   	MaxResetCount; 		//�����������
	pFuncCallback		ATRxCpltCallback;	//ATָ�������ɣ�ָ���ص�����
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
