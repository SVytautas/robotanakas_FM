#include "Ultragarsas.h"
#include "stm32f4xx_hal.h"
#include "debuguart.h"
#include "PD8544.h"

ultragarsas_t ultragaras={
  .status=ULTRAGARSAS_ST_INIT,
	.input_capture_pos={0,0,0},
	.input_capture_neg={0,0,0},
	.atstumas={0,0,0},
	.delay=2000, //2s init
	.trig_pin={ULTRAGARSAS_TRIG1_PIN,ULTRAGARSAS_TRIG2_PIN,ULTRAGARSAS_TRIG3_PIN},
	.jutiklis_ptr=0,
	.input_ch_direct={TIM_CHANNEL_1,TIM_CHANNEL_2,TIM_CHANNEL_3},
	.input_ch_indirect={TIM_CHANNEL_2,TIM_CHANNEL_1,TIM_CHANNEL_4}
};



TIM_HandleTypeDef htim1;
TIM_IC_InitTypeDef sConfigIC;

void Ultragarsas_init(void)
{
	
	
	
	
  Ultragarsas_GPIO_Init();
  TIM_MasterConfigTypeDef sMasterConfig;
 //TIM_IC_InitTypeDef sConfigIC;
	TIM_SlaveConfigTypeDef   sSlaveConfig;

 
	//TIM1 clokas RCC_APB2_Clock 84mhz
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 990;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 0xFFFF;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  if (HAL_TIM_IC_Init(&htim1) !=HAL_OK)
	{
	  PRINTF("error init");
	};

  //HAL_TIM_OnePulse_Init(&htim1, TIM_OPMODE_SINGLE);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET; //TIM_TRGO_ENABLE
	//sMasterConfig.MasterOutputTrigger = TIM_TRGO_ENABLE; //TIM_TRGO_ENABLE
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig);
	//sSlaveConfig.SlaveMode     = TIM_SLAVEMODE_RESET;
  //sSlaveConfig.InputTrigger  = TIM_TS_TI2FP2;
  //if(HAL_TIM_SlaveConfigSynchronization(&htim1, &sSlaveConfig) != HAL_OK)
  //{
    /* Configuration Error */
  //  Error_Handler();
  //}

  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
	//sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
	
	sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_1);
  
	sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
  HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_2);


  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_3);
	
	sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
  HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_4);
	
	//HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_1);
	//HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_2);
	//HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_3);
	//HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_4);
	
	ultragaras.status=ULTRAGARSAS_ST_TRIG;
	//ultragaras.delay=2;

}

void Ultragarsas_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __GPIOE_CLK_ENABLE();
	GPIO_InitTypeDef  GPIO_TRIG;
	//initinam trig autputus;
	GPIO_TRIG.Pin       = ULTRAGARSAS_TRIG1_PIN | ULTRAGARSAS_TRIG2_PIN |
												ULTRAGARSAS_TRIG3_PIN | ULTRAGARSAS_TRIG4_PIN;
  GPIO_TRIG.Mode      = GPIO_MODE_OUTPUT_PP;
  GPIO_TRIG.Pull      = GPIO_PULLDOWN;
  GPIO_TRIG.Speed     = GPIO_SPEED_MEDIUM;
 // GPIO_PCD8544_pap.Alternate = SPIx_MOSI_AF;
    
  HAL_GPIO_Init(ULTRAGARSAS_TRIG_GPIO_PORTs, &GPIO_TRIG);

}

__IO uint32_t   TIM1_IC1Value = 0;
__IO uint32_t   TIM1_IC2Value = 0;
__IO uint32_t   TIM1_IC3Value = 0;
__IO uint32_t   TIM1_IC4Value = 0;


void Ultragarsas_handler(void)
{
	if (ultragaras.delay)
	{
	  ultragaras.delay--;
	}
	
	
	switch (ultragaras.status)
	{
		case ULTRAGARSAS_ST_INIT:
		if (!ultragaras.delay)
		{
		   Ultragarsas_init();
		}			
		break;
		
		case ULTRAGARSAS_ST_TRIG:
			//PRINTF("write");
			//paleidziam trig auksta signala;
			HAL_GPIO_WritePin(ULTRAGARSAS_TRIG_GPIO_PORTs, ultragaras.trig_pin[ultragaras.jutiklis_ptr],GPIO_PIN_SET);
		  
		  //perkonfiguruojame IC kanalus
		  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
      sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
      HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, ultragaras.input_ch_direct[ultragaras.jutiklis_ptr]);
  
	    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
      sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
      HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, ultragaras.input_ch_indirect[ultragaras.jutiklis_ptr]);
		
		  //HAL_GPIO_WritePin(ULTRAGARSAS_TRIG_GPIO_PORTs, ultragaras.trig_pin[ultragaras.jutiklis_ptr+1],GPIO_PIN_SET);
		  //HAL_GPIO_WritePin(ULTRAGARSAS_TRIG_GPIO_PORTs, ultragaras.trig_pin[ultragaras.jutiklis_ptr+2],GPIO_PIN_SET);
		  //for (uint32_t i=0;i<390;i++) {};
		  
		  //trigerinam atitinkama input kanala
		  //HAL_TIM_OnePulse_Start_IT(&htim1, ultragaras.input_ch[ultragaras.jutiklis_ptr]);
		  //HAL_TIM_IC_Start_IT(&htim1, ultragaras.input_ch[ultragaras.jutiklis_ptr]);
		  //HAL_TIM_IC_Start_IT(&htim1, ultragaras.input_ch[ultragaras.jutiklis_ptr+1]);
		  //HAL_TIM_IC_Start_IT(&htim1, ultragaras.input_ch[ultragaras.jutiklis_ptr+2]);
				HAL_TIM_IC_Start_IT(&htim1, ultragaras.input_ch_direct[ultragaras.jutiklis_ptr]); //paleidziame matavima
				HAL_TIM_IC_Start_IT(&htim1, ultragaras.input_ch_indirect[ultragaras.jutiklis_ptr]);
				//HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_3);
				//HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_4);
		    __HAL_TIM_SetCounter(&htim1, 0); //nulinam timer counteri
				TIM1_IC1Value = 0; //nulinam IC value
				TIM1_IC2Value = 0;
				TIM1_IC3Value = 0;
				TIM1_IC4Value = 0;

		 //uzdedtam iterupto negavimo delay
		  ultragaras.delay=50; //70ms
		  ultragaras.status=ULTRAGARSAS_ST_ECHO_GET;
		break;
		
		case ULTRAGARSAS_ST_ECHO_GET:
			//laukiam rezultatu
			if (!ultragaras.delay)
		  {
		   //Ultragarsas_init();
				ultragaras.status=ULTRAGARSAS_ST_TRIG;
				//pasiemama duomenis 
				if (ultragaras.jutiklis_ptr==ULTRAGARSAS_JUTIKLIS1)
				{
					//ch1 
					ultragaras.input_capture_pos[ultragaras.jutiklis_ptr]=TIM1_IC1Value;
					ultragaras.input_capture_neg[ultragaras.jutiklis_ptr]=TIM1_IC2Value;
					//PRINTF("1 jut: %u\r\n",TIM1_IC2Value-TIM1_IC1Value);
					if (ultragaras.input_capture_neg[ultragaras.jutiklis_ptr]>ultragaras.input_capture_pos[ultragaras.jutiklis_ptr])
					{
					  ultragaras.atstumas[ultragaras.jutiklis_ptr]=ultragaras.input_capture_neg[ultragaras.jutiklis_ptr]-ultragaras.input_capture_pos[ultragaras.jutiklis_ptr];
					}
					else
					{
						//erroras arba 0
					  ultragaras.atstumas[ultragaras.jutiklis_ptr]=0;
					}
					
				
				}
				else if (ultragaras.jutiklis_ptr==ULTRAGARSAS_JUTIKLIS2)
				{
					ultragaras.input_capture_pos[ultragaras.jutiklis_ptr]=TIM1_IC2Value;
					ultragaras.input_capture_neg[ultragaras.jutiklis_ptr]=TIM1_IC1Value;
					//PRINTF("2 jut: %u\r\n",TIM1_IC1Value-TIM1_IC2Value);
					if (ultragaras.input_capture_neg[ultragaras.jutiklis_ptr]>ultragaras.input_capture_pos[ultragaras.jutiklis_ptr])
					{
					  ultragaras.atstumas[ultragaras.jutiklis_ptr]=ultragaras.input_capture_neg[ultragaras.jutiklis_ptr]-ultragaras.input_capture_pos[ultragaras.jutiklis_ptr];
					}
					else
					{
						//erroras arba 0
					  ultragaras.atstumas[ultragaras.jutiklis_ptr]=0;
					}
				
				}
				else if (ultragaras.jutiklis_ptr==ULTRAGARSAS_JUTIKLIS3)
				{
					ultragaras.input_capture_pos[ultragaras.jutiklis_ptr]=TIM1_IC3Value;
					ultragaras.input_capture_neg[ultragaras.jutiklis_ptr]=TIM1_IC4Value;
				  //PRINTF("3 jut: %u\r\n",ultragaras.input_capture_neg[ultragaras.jutiklis_ptr]-ultragaras.input_capture_pos[ultragaras.jutiklis_ptr]);
					if (ultragaras.input_capture_neg[ultragaras.jutiklis_ptr]>ultragaras.input_capture_pos[ultragaras.jutiklis_ptr])
					{
					  ultragaras.atstumas[ultragaras.jutiklis_ptr]=ultragaras.input_capture_neg[ultragaras.jutiklis_ptr]-ultragaras.input_capture_pos[ultragaras.jutiklis_ptr];
					}
					else
					{
						//erroras arba 0
					  ultragaras.atstumas[ultragaras.jutiklis_ptr]=0;
					}
				}
				
				//keiciam pointeri
				ultragaras.jutiklis_ptr++;
				if (ultragaras.jutiklis_ptr>=ULTRAGARSAS_JUTIKLIU_SK)
				{
					ultragaras.jutiklis_ptr=0;
				}
				
		  }		
			HAL_GPIO_WritePin(ULTRAGARSAS_TRIG_GPIO_PORTs, ultragaras.trig_pin[0],GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(ULTRAGARSAS_TRIG_GPIO_PORTs, ultragaras.trig_pin[1],GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(ULTRAGARSAS_TRIG_GPIO_PORTs, ultragaras.trig_pin[2],GPIO_PIN_RESET);
			
		break;
		
		
		
		
		default:
		break;
	
	
	
	}

}


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
//	PRINTF("input get");
	if (htim->Instance==TIM1)
	{
		
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
    {
			TIM1_IC1Value = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			//PRINTF("input get1 %u\r\n",TIM1_IC1Value);
		}
		else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
    {
		  //PRINTF("input get2");
			TIM1_IC2Value = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
			//if (TIM1_IC2Value>TIM1_IC1Value)
			//{
			  //PRINTF("input get2 %u\r\n",TIM1_IC2Value-TIM1_IC1Value);
			//}
			
		}
		else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
    {
			TIM1_IC3Value = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
			//PRINTF("input get1 %u\r\n",TIM1_IC1Value);
		}
		else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
    {
			TIM1_IC4Value = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
			//PRINTF("input get1 %u\r\n",TIM1_IC3Value-TIM1_IC4Value);
		}
	
	}

}



uint32_t ultragarsas_get_min_virsus_priekis(void)
{
  if (ultragaras.atstumas[ULTRAGARSAS_JUTIKLIS2]<ultragaras.atstumas[ULTRAGARSAS_JUTIKLIS3])
	{
	  if (ultragaras.atstumas[ULTRAGARSAS_JUTIKLIS2]!=0)
		{
		   return ultragaras.atstumas[ULTRAGARSAS_JUTIKLIS2];
		}
		else
		{
		   if (ultragaras.atstumas[ULTRAGARSAS_JUTIKLIS3]!=0)
			 {
			 return ultragaras.atstumas[ULTRAGARSAS_JUTIKLIS3];
			 }
			 else
			 {
			  return 0xFFFF;
			 }
		}
	}
	else
	{
		if (ultragaras.atstumas[ULTRAGARSAS_JUTIKLIS3]!=0)
		{
		   return ultragaras.atstumas[ULTRAGARSAS_JUTIKLIS3];
		}
		else
		{
		   if (ultragaras.atstumas[ULTRAGARSAS_JUTIKLIS2]!=0)
			 {
			 return ultragaras.atstumas[ULTRAGARSAS_JUTIKLIS2];
			 }
			 else
			 {
			  return 0xFFFF;
			 }
		}
	
	
	}
  
}


uint32_t ultragarsas_get_centras(void)
{
	if (ultragaras.atstumas[ULTRAGARSAS_JUTIKLIS1]>30) //3cm
	{
	  return (uint32_t)ultragaras.atstumas[ULTRAGARSAS_JUTIKLIS1];
	}
	else
	{
	  return 0xFFFF;
	}
  
}

uint32_t ultragarsas_get_kairys(void)
{
  if (ultragaras.atstumas[ULTRAGARSAS_JUTIKLIS2]>30) //3cm
	{
	  return (uint32_t)ultragaras.atstumas[ULTRAGARSAS_JUTIKLIS2];
	}
	else
	{
	  return 0xFFFF;
	}
}

uint32_t ultragarsas_get_desinys(void)
{
  if (ultragaras.atstumas[ULTRAGARSAS_JUTIKLIS3]>30) //3cm
	{
	  return (uint32_t)ultragaras.atstumas[ULTRAGARSAS_JUTIKLIS3];
	}
	else
	{
	  return 0xFFFF;
	}
}

void ultragarsas_print_nokia(void)
{
	  PCD8544_Clear();
		//read_RGB();
		
		PCD8544_GotoXY(0, 0);
	  NOKIA_PRINTF("Ultragarso jut.:");
		PCD8544_GotoXY(0, 9);
	  NOKIA_PRINTF("1: %u.%u cm",ultragaras.atstumas[ULTRAGARSAS_JUTIKLIS1]/10,ultragaras.atstumas[ULTRAGARSAS_JUTIKLIS1]%10);
		PCD8544_GotoXY(0, 18);
	  NOKIA_PRINTF("2: %u.%u cm",ultragaras.atstumas[ULTRAGARSAS_JUTIKLIS2]/10,ultragaras.atstumas[ULTRAGARSAS_JUTIKLIS2]%10);
	  PCD8544_GotoXY(0, 27);
	  NOKIA_PRINTF("3: %u.%u cm",ultragaras.atstumas[ULTRAGARSAS_JUTIKLIS3]/10,ultragaras.atstumas[ULTRAGARSAS_JUTIKLIS3]%10);
		PCD8544_Refresh();

}


