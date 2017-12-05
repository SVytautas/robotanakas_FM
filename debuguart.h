#include "stm32f4xx_hal.h"
//#include "stm324xg_eval.h"
#include "stdio.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* User can use this section to tailor USARTx/UARTx instance used and associated 
   resources */
/* Definition for USARTx clock resources */
#define USARTy                          USART3
#define USARTy_CLK_ENABLE()              __HAL_RCC_USART3_CLK_ENABLE();
#define USARTy_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOD_CLK_ENABLE()
#define USARTy_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOD_CLK_ENABLE() 

#define USARTy_FORCE_RESET()             __HAL_RCC_USART3_FORCE_RESET()
#define USARTy_RELEASE_RESET()           __HAL_RCC_USART3_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTy_TX_PIN                    GPIO_PIN_8
#define USARTy_TX_GPIO_PORT              GPIOD  
#define USARTy_TX_AF                     GPIO_AF7_USART3
#define USARTy_RX_PIN                    GPIO_PIN_9
#define USARTy_RX_GPIO_PORT              GPIOD 
#define USARTy_RX_AF                     GPIO_AF7_USART3

//void vprint(const char *fmt, va_list argp);
void PRINTF(const char *fmt, ...);
void debuguartinit();

