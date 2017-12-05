
#include "stm32f4xx_hal.h"
//#include <stdio.h>
#include <stdbool.h>
#include "stm32f4xx_hal_def.h"
#include "stm32f4xx_hal_adc.h"


#define ADC_KANALU_SK 4


typedef struct
{
  ADC_ChannelConfTypeDef adc_kanalas[ADC_KANALU_SK];
  uint32_t adc_sample_sum[ADC_KANALU_SK];
  uint32_t adc_sample_num[ADC_KANALU_SK];
	uint32_t adc_sample_num_ct[ADC_KANALU_SK];
  uint32_t adc_sample[ADC_KANALU_SK];
  uint32_t adc_kanalas_ptr;
  uint32_t status;	
}adc_user_t;


enum {
	ADC_ST_INIT=0,
	ADC_ST_CHANGE_CH,
	ADC_ST_TRIG_CONVERTION,
	ADC_ST_GET_DATA,

};


/* Definition for ADCx clock resources */
/* Definition for ADCx clock resources */
#define ADCx                            ADC1
#define ADCx_CLK_ENABLE()               __HAL_RCC_ADC1_CLK_ENABLE()   
#define ADCx_CHANNEL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()
     
#define ADCx_FORCE_RESET()              __HAL_RCC_ADC_FORCE_RESET()
#define ADCx_RELEASE_RESET()            __HAL_RCC_ADC_RELEASE_RESET()

/* Definition for ADCx Channel Pin */
#define ADCx_CHANNEL0_PIN                GPIO_PIN_0
#define ADCx_CHANNEL1_PIN                GPIO_PIN_1
#define ADCx_CHANNEL3_PIN                GPIO_PIN_3
#define ADCx_CHANNEL4_PIN                GPIO_PIN_4
#define ADCx_CHANNELS_GPIO_PORT          GPIOA

/* Definition for ADCx's Channel PA0 ADC1 */
#define ADCx_CHANNEL_M1_CUR                   ADC_CHANNEL_0
#define ADCx_CHANNEL_M2_CUR                   ADC_CHANNEL_1
#define ADCx_CHANNEL_ADC_7_4BAT               ADC_CHANNEL_3
#define ADCx_CHANNEL_BAT1_V                   ADC_CHANNEL_4

enum{
ADC_SAMPLE_M1_CUR=0,
ADC_SAMPLE_M2_CUR,
ADC_SAMPLE_7_4BAT,
ADC_SAMPLE_BAT1V
};


void Init_ADC(void);
void adc_handler(void);
uint16_t get_adc_current(void);
void adc_print_nokia_batVoltages(void);
float adc_get_7_4bat_voltage(void);