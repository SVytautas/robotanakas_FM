#include "delay_simple.h"
#include "stm32f4xx_hal.h"


uint32_t Delay_Init(void) {
#if !defined(STM32F0xx)
	uint32_t c;
	
    /* Enable TRC */
    CoreDebug->DEMCR &= ~0x01000000;
    CoreDebug->DEMCR |=  0x01000000;
	
    /* Enable counter */
    DWT->CTRL &= ~0x00000001;
    DWT->CTRL |=  0x00000001;
	
    /* Reset counter */
    DWT->CYCCNT = 0;
	
	/* Check if DWT has started */
	c = DWT->CYCCNT;
	
	/* 2 dummys */
	__ASM volatile ("NOP");
	__ASM volatile ("NOP");
	
	/* Return difference, if result is zero, DWT has not started */
	return (DWT->CYCCNT - c);
#else
	/* Return OK */
	return 1;
#endif
}

__STATIC_INLINE void Delay(__IO uint32_t micros) {

    uint32_t start = DWT->CYCCNT;
    
    /* Go to number of cycles for system */
    micros *= (HAL_RCC_GetHCLKFreq() / 1000000);
    
    /* Delay till end */
    while ((DWT->CYCCNT - start) < micros);

    /* Go to clock cycles */
    micros *= (SystemCoreClock / 1000000) / 5;
    
    /* Wait till done */
    while (micros--);

}





// micro second ??? delay
void delay_us(uint32_t us)
{
    Delay(us);
}





// mili second ??? delay
void delay_ms(uint32_t ms)
{
    Delay(ms * 1000);
}



void _delay_us(uint32_t us)
{
   uint32_t delay_us_cycle=us*168;
	while(delay_us_cycle--) {};

}