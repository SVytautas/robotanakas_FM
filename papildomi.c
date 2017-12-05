#include "papildomi.h"
#include "main.h"
#include <stdio.h>
#include <string.h>

beep_t beep={
.beepon=false,
};

void papildomi_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;	
	//initinam timerius ir isejimus
	PAPILDOMI_LD1_LD2_CLK_ENABLE();
	PAPILDOMI_LD3_LD4_CLK_ENABLE();
  
	
	  GPIO_InitStruct.Pin = PAPILDOMI_LD1_GPIO_PIN|PAPILDOMI_LD2_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(PAPILDOMI_LD1_GPIO_PORT, &GPIO_InitStruct);
    
	  GPIO_InitStruct.Pin = PAPILDOMI_LD3_GPIO_PIN;//|PAPILDOMI_LD4_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(PAPILDOMI_LD3_GPIO_PORT, &GPIO_InitStruct);

}


void papildomi_beep_handler(void)
{
	
	if (beep.beepon)
	{
		//beepinam jei yra
		if (beep.beeper_ptr%2)
		{
			//lyginis
			BEEP_OFF;
			if (beep.beeper[beep.beeper_ptr])
			{
			  beep.beeper[beep.beeper_ptr]--;
			}
			else
			{
			  beep.beeper_ptr++;
				if (beep.beeper_ptr>=PAP_BEEP_SIZE)
				{
				  beep.beeper_ptr=0;
					beep.beepon=false;
				}
			}
		
		}
		else
		{ //nelyginis
		  BEEP_ON;
			if (beep.beeper[beep.beeper_ptr])
			{
			  beep.beeper[beep.beeper_ptr]--;
			}
			else
			{
			  beep.beeper_ptr++;
				if (beep.beeper_ptr>=PAP_BEEP_SIZE)
				{
				  beep.beeper_ptr=0;
					beep.beepon=false;
				}
			}
		}
	
	}
	else
	{
	  BEEP_OFF;
		
	}

}


void pap_beep_one(uint8_t beep_time)
{
    if (!beep.beepon)
		{
		  beep.beeper[0]=beep_time;
			beep.beepon=true;
		}
}
void pap_beep_mult(uint8_t beep_time, uint8_t beep_k)
{
    if (!beep.beepon)
		{
			for (uint8_t i=0;i<beep_k*2;i++)
			{
				if (i<PAP_BEEP_SIZE)
				{
				  beep.beeper[i]=beep_time;
				}
			}
		  
			beep.beepon=true;
		}
}

void pap_beep_reset(void)
{
	if (!beep.beepon)
	{
    beep.beeper_ptr=0;
	  memset(&beep.beeper,0,sizeof(beep.beeper));
	}
}

void pap_beep_push(uint8_t beepp)
{
	if (!beep.beepon)
	{
   beep.beeper[beep.beeper_ptr]=beepp;
		beep.beeper_ptr++;
	}
}

void pap_beep_start(void)
{
  if (!beep.beepon)
	{
     beep.beepon=true;
		beep.beeper_ptr=0;
	}
}