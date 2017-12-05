#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "debuguart.h"
#include "main.h"


UART_HandleTypeDef Uartdebug;
void vprint(const  char *fmt, va_list argp)
{
    char string[256];
    if(0 < vsprintf(string,fmt,argp)) // 
    {
        HAL_UART_Transmit(&Uartdebug, (uint8_t*)string, strlen(string), 0xffffff); // send message via UART
    }
}

void PRINTF(const char *fmt, ...) // 
{
    va_list argp;
    va_start(argp, fmt);
    vprint( fmt, argp);
    va_end(argp);
}


void debuguartinit()
{
	
	  GPIO_InitTypeDef  GPIO_InitStruct;
  
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  USARTy_TX_GPIO_CLK_ENABLE();
  USARTy_RX_GPIO_CLK_ENABLE();
  
  /* Enable USARTx clock */
  USARTy_CLK_ENABLE(); 
  
  /*##-2- Configure peripheral GPIO ##########################################*/  
  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = USARTy_TX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
  GPIO_InitStruct.Alternate = USARTy_TX_AF;
  
  HAL_GPIO_Init(USARTy_TX_GPIO_PORT, &GPIO_InitStruct);
    
  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = USARTy_RX_PIN;
  GPIO_InitStruct.Alternate = USARTy_RX_AF;
    
  HAL_GPIO_Init(USARTy_RX_GPIO_PORT, &GPIO_InitStruct);
	
	
	Uartdebug.Instance          = USARTy;
  
  Uartdebug.Init.BaudRate     = 115200;
  Uartdebug.Init.WordLength   = UART_WORDLENGTH_8B;
  Uartdebug.Init.StopBits     = UART_STOPBITS_1;
  Uartdebug.Init.Parity       = UART_PARITY_NONE;
  Uartdebug.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  Uartdebug.Init.Mode         = UART_MODE_TX_RX;
  Uartdebug.Init.OverSampling = UART_OVERSAMPLING_16;
    
  if(HAL_UART_Init(&Uartdebug) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler(); 
  }
	//PRINTF("\r\n start \r\n");
  //HAL_UART_Transmit(&Uartdebug, (uint8_t*) "ab", 2,0xffffff); // send message via UART
}