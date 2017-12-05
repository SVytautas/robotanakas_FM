#include "btn_bumper.h"
#include "main.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "debuguart.h"
#include "Ekranas_atv.h"
#include "PD8544.h"

static btnbump_user_t btnbump_user={
.btn_pinai={BTN1_BUMP_PIN,BTN2_BUMP_PIN,BTN3_BUMP_PIN},
.btn_count={0,0,0},
.btn_status={false,false,false},
.btn_event={false,false,false},
};



void btn_bump_init(void)
{
	
	//inicilizuojam button GPIO_portus
	GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  BTNS_BUMP_GPIO_CLK_ENABLE();

  /* Configure GPIO pin: PA0 (USER) */
  GPIO_InitStruct.Pin = BTN1_BUMP_PIN | BTN2_BUMP_PIN |
												BTN3_BUMP_PIN ;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(BTNS_BUMP_GPIO_PORT, &GPIO_InitStruct);


}



void btn_bump_handler(void)
{
	for (uint8_t i=0;i<BTN_BUMP_SK;i++)
	{
		if (HAL_GPIO_ReadPin(BTNS_BUMP_GPIO_PORT, btnbump_user.btn_pinai[i])) {
		   
			if (btnbump_user.btn_count[i]>=BTN_BUMP_DEBOUNCE)
			{
				//setinam true ir laukiam atleidimo su debounce
			  btnbump_user.btn_status[i]=true;
				//PRINTF("PRESED %u\r\n",i);
				
			}
			else
			{
			   btnbump_user.btn_count[i]++;
			}
		
		}
		else
		{
			if (btnbump_user.btn_count[i])
			{
			  btnbump_user.btn_count[i]--;
			}
			if (btnbump_user.btn_status[i])
			{
				if (!btnbump_user.btn_count[i])
				{
					//dedam status false ir uzdegam eventa
				  btnbump_user.btn_status[i]=false;
					btnbump_user.btn_event[i]=true;
				}
			}
		}
	}
	
	
	//handlinam mygtuku eventus
}


bool btn_bump_get_presed(void)
{
   //grazinam true jei kuris nors paspaustas
	if(btnbump_user.btn_status[0] || btnbump_user.btn_status[1] || btnbump_user.btn_status[2])
	{
	  return true;
	}
	else
	{
	  return false;
	}
}

bool btn_bump_kairys(void)
{
 return btnbump_user.btn_status[0];
}

bool btn_bump_centras(void)
{
 return btnbump_user.btn_status[2];
}

bool btn_bump_desinys(void)
{
 return btnbump_user.btn_status[1];
}


void btn_bump_ekranas_print(void)
{
	
	  PCD8544_Clear();
		//read_RGB();
		
		PCD8544_GotoXY(0, 0);
	  NOKIA_PRINTF("Mygtukai:");
		PCD8544_GotoXY(0, 9);
	  NOKIA_PRINTF("BTN1_BUMP: %u",btnbump_user.btn_status[0]);
		PCD8544_GotoXY(0, 18);
	  NOKIA_PRINTF("BTN2_BUMP: %u",btnbump_user.btn_status[1]);
	  PCD8544_GotoXY(0, 27);
	  NOKIA_PRINTF("BTN3_BUMP: %u",btnbump_user.btn_status[2]);
		PCD8544_Refresh();


}