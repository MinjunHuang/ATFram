#include "Hardware.h"

char UartRXBuff[_UART_RXBUFFSIZE];

void UartInit(void)
{
	__HAL_UART_ENABLE_IT(&_ATUART_HANDLE, UART_IT_IDLE);  					//使能串口空闲中断
	HAL_UART_Receive_DMA(&_ATUART_HANDLE, (uint8_t*)UartRXBuff, _UART_RXBUFFSIZE);     	//开启DMA传输
	__HAL_DMA_DISABLE_IT(&_UART_DMA_HANDLE,DMA_IT_TC);				//关闭DMA中断
	
	__LOG("size: %d",sizeof(ATCommandConfig));
	
}

void SendString(char * str)
{
	HAL_UART_Transmit(& _ATUART_HANDLE,(uint8_t *)str,strlen(str),_UART_TIMEOUT);
}

void USER_UART_IDLECallback(UART_HandleTypeDef *huart)
{
	 uint8_t RXDataLength;
	if(RESET == __HAL_UART_GET_FLAG(& _ATUART_HANDLE, UART_FLAG_IDLE))   
	{	 // 判断是否是空闲中断
		return;			 
	} 

	// 清除空闲中断标志（否则会一直不断进入中断）
	__HAL_UART_CLEAR_IDLEFLAG(& _ATUART_HANDLE); 
	// 停止本次DMA传输
    HAL_UART_DMAStop(& _ATUART_HANDLE);  
                                                       
    // 计算接收到的数据长度
    RXDataLength  = _UART_RXBUFFSIZE - __HAL_DMA_GET_COUNTER(&_UART_DMA_HANDLE);   
    
	UartRXBuff[RXDataLength]=0;
	
	// 测试函数：将接收到的数据打印出去
	//__LOGARRAY(UartRXBuff,RXDataLength+1,"中断接收的数据");
	 
    __LOGNOLF("Receive Data(length = %d):%s ",RXDataLength,UartRXBuff);                    
	
	//定义信号量，在接收完一帧数据后发布信号量
	BaseType_t xHightPriorityTaskWoken=pdFALSE;
	xSemaphoreGiveFromISR(ATRXCplSemaphore,&xHightPriorityTaskWoken);
    //退出中断后执行最高优先级任务
	portYIELD_FROM_ISR(xHightPriorityTaskWoken);
  
	// 重启开始DMA传输 每次255字节数据
    HAL_UART_Receive_DMA(&_ATUART_HANDLE, (uint8_t*)UartRXBuff, _UART_RXBUFFSIZE);  

}

void MoudleResst(void)
{
	
}

