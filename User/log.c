#include "log.h"
#include "string.h"
#include "stdarg.h"

//重定向fputc
int fputc(int ch, FILE *f)
{
	/* 发送一个字节数据到串口DEBUG_USART */
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 1000);	
	return (ch);
}


void StrNoLR(char * str)
{
	int len=strlen(str);
	for(int i=0;i<len;i++)
	{
		if(str[i]=='\r')
		str[i]='-';
		if(str[i]=='\n')
		str[i]='*';
	}
}

char * mystrcat(int n,...)
{
	int len=0,j=0;
    char *str,*first;

    va_list v1;
	va_start(v1,n);
	first=va_arg(v1,char*);
    str = first;

	while(first[len++]);
	len--;
	for (size_t i = 0; i < n-1; i++,j=0)
	{
		str=va_arg(v1,char*);
		printf("%s\n",str);
		while(str[j])
		{
			printf("%c\n",str[j]);
			printf("%c\n",first[len-1]);
			first[len++]=str[j++];
		}
	}
    va_end(v1);
	return first;
}
