#include "log.h"
#include "string.h"


//�ض���fputc
int fputc(int ch, FILE *f)
{
	/* ����һ���ֽ����ݵ�����DEBUG_USART */
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 1000);	
	return (ch);
}

void strrpc(char *str,char *oldstr,char *newstr){
   int len=strlen(str);
	for(int i=0;i<len;i++)
	{
		if(str[i]=='\r')
		str[i]='-';
		if(str[i]=='\n')
		str[i]='*';
	}
}

void StrNoLR(char * str)
{
	strrpc(str,"\r\n","**");
}
