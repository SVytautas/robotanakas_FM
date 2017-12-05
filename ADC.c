#include "main.h"
#include "Lidar.h"
#include "stm32f4xx_it.h"
#include "debuguart.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
//#include "Servo.h"
#include "Griebtuvas.h"
//#include "zingsniniai_varikliai.h"
#include "ADC.h"
#include "PD8544.h"

ADC_HandleTypeDef AdcHandle;
ADC_ChannelConfTypeDef sConfig;

//static adc_user_t adc_user=

static adc_user_t adc_user={
.status=ADC_ST_INIT,
	///*
	.adc_kanalas={
{.Channel=ADCx_CHANNEL_M1_CUR, 			.Rank=1, .SamplingTime=ADC_SAMPLETIME_3CYCLES, .Offset=0},
{.Channel=ADCx_CHANNEL_M2_CUR,  		.Rank=1, .SamplingTime=ADC_SAMPLETIME_3CYCLES, .Offset=0},
{.Channel=ADCx_CHANNEL_ADC_7_4BAT, 	.Rank=1, .SamplingTime=ADC_SAMPLETIME_3CYCLES, .Offset=0},
{.Channel=ADCx_CHANNEL_BAT1_V,			.Rank=1, .SamplingTime=ADC_SAMPLETIME_3CYCLES, .Offset=0}},
  
  .adc_sample_sum={0,0,0,0},
	                //CH0 , .... CH4 
	.adc_sample_num={16,16,16,16},
	.adc_sample_num_ct={0,0,0,0},
	.adc_kanalas_ptr=0,
	//*/

};

/* Variable used to get converted value */
__IO uint16_t uhADCxConvertedValue = 0;
bool flag_can_poll=false;

void Init_ADC(void)
{
	
	GPIO_InitTypeDef          GPIO_InitStruct;
  
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* ADC3 Periph clock enable */
  ADCx_CLK_ENABLE();
	
	//-------------------------------enabliname GPIO--------------------------------------
  /* Enable GPIO clock ****************************************/
  ADCx_CHANNEL_GPIO_CLK_ENABLE();
  /*##-2- Configure peripheral GPIO ##########################################*/ 
  /* ADC3 Channel8 GPIO pin configuration */
  GPIO_InitStruct.Pin = ADCx_CHANNEL0_PIN | ADCx_CHANNEL1_PIN
											| ADCx_CHANNEL3_PIN | ADCx_CHANNEL4_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ADCx_CHANNELS_GPIO_PORT, &GPIO_InitStruct);
	//-------------------------------------------------------------------------------------
	
	
	
	
/*##-1- Configure the ADC peripheral #######################################*/
  AdcHandle.Instance          = ADCx;
  
  AdcHandle.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV2;
  AdcHandle.Init.Resolution            = ADC_RESOLUTION_12B;
  AdcHandle.Init.ScanConvMode          = DISABLE;
  AdcHandle.Init.ContinuousConvMode    = DISABLE;
  AdcHandle.Init.DiscontinuousConvMode = DISABLE;
  AdcHandle.Init.NbrOfDiscConversion   = 0;
  AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
  AdcHandle.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;
  AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  AdcHandle.Init.NbrOfConversion       = 1;
  AdcHandle.Init.DMAContinuousRequests = DISABLE;
  AdcHandle.Init.EOCSelection          = DISABLE;
      
  if(HAL_ADC_Init(&AdcHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  
  
  if(HAL_ADC_ConfigChannel(&AdcHandle, &adc_user.adc_kanalas[adc_user.adc_kanalas_ptr]) != HAL_OK)
  {
    /* Channel Configuration Error */
    Error_Handler();
  }
  

  /*##-3- Start the conversion process #######################################*/  
  if(HAL_ADC_Start(&AdcHandle) != HAL_OK)
  {
    /* Start Conversation Error */
    Error_Handler();
  }
  
  /*##-4- Wait for the end of conversion #####################################*/  
   /*  Before starting a new conversion, you need to check the current state of 
        the peripheral; if it’s busy you need to wait for the end of current
        conversion before starting a new one.
        For simplicity reasons, this example is just waiting till the end of the 
        conversion, but application may perform other tasks while conversion 
        operation is ongoing. */
  HAL_ADC_PollForConversion(&AdcHandle, 10);
  
  /* Check if the continuous conversion of regular channel is finished */
  if((HAL_ADC_GetState(&AdcHandle) & HAL_ADC_STATE_EOC_REG) == HAL_ADC_STATE_EOC_REG)
  {
    /*##-5- Get the converted value of regular channel #######################*/
    uhADCxConvertedValue = HAL_ADC_GetValue(&AdcHandle);
  }
	//flag_can_poll=true;
	adc_user.status=ADC_ST_CHANGE_CH;
	
}


void adc_handler(void)
{
	
	switch (adc_user.status)
	{
		
		case ADC_ST_INIT:
			
		break;
		
		case ADC_ST_CHANGE_CH:
		if(HAL_ADC_ConfigChannel(&AdcHandle, &adc_user.adc_kanalas[adc_user.adc_kanalas_ptr]) != HAL_OK)
    {
    /* Channel Configuration Error */
    Error_Handler();
	  //pasiliekam tam paciam ST setins per naujo
    }
    else
    {
			
		  adc_user.status=ADC_ST_GET_DATA;
		}			
		break;
		
		case ADC_ST_GET_DATA:
			//pradedame konvercija
			if(HAL_ADC_Start(&AdcHandle) != HAL_OK)
			{
				/* Start Conversation Error */
				Error_Handler();
			}	
			//gaunam duomenis
			HAL_ADC_PollForConversion(&AdcHandle, 10);
			if((HAL_ADC_GetState(&AdcHandle) & HAL_ADC_STATE_EOC_REG) == HAL_ADC_STATE_EOC_REG)
			{
				/*##-5- Get the converted value of regular channel #######################*/
				uhADCxConvertedValue = HAL_ADC_GetValue(&AdcHandle);
				adc_user.adc_sample_sum[adc_user.adc_kanalas_ptr]+=(uint32_t)uhADCxConvertedValue;
				//PRINTF("adc %u\r\n",uhADCxConvertedValue);
				adc_user.adc_sample_num_ct[adc_user.adc_kanalas_ptr]++;
				if (adc_user.adc_sample_num_ct[adc_user.adc_kanalas_ptr]>=adc_user.adc_sample_num[adc_user.adc_kanalas_ptr])
				{
					adc_user.adc_sample_num_ct[adc_user.adc_kanalas_ptr]=0;
					//turim apibresta skaiciavimo skaiciu 
				  adc_user.adc_sample[adc_user.adc_kanalas_ptr]=adc_user.adc_sample_sum[adc_user.adc_kanalas_ptr]/adc_user.adc_sample_num[adc_user.adc_kanalas_ptr];
					adc_user.adc_sample_sum[adc_user.adc_kanalas_ptr]=0;
					//keiciam kanala
					//PRINTF("adc %u val %u\r\n",adc_user.adc_kanalas_ptr,adc_user.adc_sample[adc_user.adc_kanalas_ptr]);
					
					adc_user.adc_kanalas_ptr++;
					if (adc_user.adc_kanalas_ptr>=ADC_KANALU_SK)
					{
					   adc_user.adc_kanalas_ptr=0;
					}
					adc_user.status=ADC_ST_CHANGE_CH;
				}
				
				
			}
		break;
	
		default:
		adc_user.status=ADC_ST_INIT;
		break;
	}
	
 /*
	if (flag_can_poll)
	{
		if(HAL_ADC_Start(&AdcHandle) != HAL_OK)
			{
				// Start Conversation Error //
				Error_Handler();
			}	
		
		HAL_ADC_PollForConversion(&AdcHandle, 10);
	  if((HAL_ADC_GetState(&AdcHandle) & HAL_ADC_STATE_EOC_REG) == HAL_ADC_STATE_EOC_REG)
			{
					// Get the converted value of regular channel //
					uhADCxConvertedValue = HAL_ADC_GetValue(&AdcHandle);
			}
  }
	*/
	

}


uint16_t get_adc_current(void)
{

return uhADCxConvertedValue;


}

float adc_get_7_4bat_voltage(void)
{
   return (float)adc_user.adc_sample[ADC_SAMPLE_7_4BAT]/3.308;///3.276;
}



void adc_print_nokia_batVoltages(void)
{
	 //samplas
	//adc_user.adc_sample[adc_user.adc_kanalas_ptr]//
    PCD8544_Clear();
		//read_RGB();
		
		PCD8544_GotoXY(0, 0);
	  NOKIA_PRINTF("BATERIJOS [V]:");
		PCD8544_GotoXY(0, 9);
	  //NOKIA_PRINTF("EL: %u",adc_user.adc_sample[ADC_SAMPLE_7_4BAT]);
		NOKIA_PRINTF("EL: %u.%u V[%u.%u]",(uint16_t)adc_get_7_4bat_voltage()/100,(uint16_t)adc_get_7_4bat_voltage()%100,((uint16_t)adc_get_7_4bat_voltage()/2)/100,((uint16_t)adc_get_7_4bat_voltage()/2)%100);
		PCD8544_GotoXY(0, 18);
	  NOKIA_PRINTF("ZV: %u",adc_user.adc_sample[ADC_SAMPLE_BAT1V]);
		PCD8544_Refresh();

}


