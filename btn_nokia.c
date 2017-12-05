#include "btn_nokia.h"
#include "main.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "debuguart.h"
#include "Ekranas_atv.h"


static btn_user_t btn_user={
.btn_pinai={BTN1_NOKIA_PIN,BTN2_NOKIA_PIN,BTN3_NOKIA_PIN,BTN4_NOKIA_PIN},
.btn_count={0,0,0,0},
.btn_status={false,false,false,false},
.btn_event={false,false,false,false},
};

void btn_nokia_init(void)
{
	
	//inicilizuojam button GPIO_portus
	GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  BTNS_NOKIA_GPIO_CLK_ENABLE();

  /* Configure GPIO pin: PA0 (USER) */
  GPIO_InitStruct.Pin = BTN1_NOKIA_PIN | BTN2_NOKIA_PIN |
												BTN3_NOKIA_PIN | BTN4_NOKIA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BTNS_NOKIA_GPIO_PORT, &GPIO_InitStruct);


}


void btn_nokia_handler(void)
{
	for (uint8_t i=0;i<BTN_NOKIA_SK;i++)
	{
		if (HAL_GPIO_ReadPin(BTNS_NOKIA_GPIO_PORT, btn_user.btn_pinai[i])) {
		   
			if (btn_user.btn_count[i]>=BTN_DEBOUNCE)
			{
				//setinam true ir laukiam atleidimo su debounce
			  btn_user.btn_status[i]=true;
				
				if (btn_user.btn_count_long[i]==BTN_LONG)
				 {
					 if (i==BTN1_PTR)
					 {
					   PRINTF("BTN1 long");
						 btn_user.btn_count_long[i]++;
						 btn_user.btn_status[i]=false;
					 }
					 if (i==BTN2_PTR)
					 {
					   PRINTF("BTN2 long");
						 btn_user.btn_count_long[i]++;
						 btn_user.btn_status[i]=false;
					 }
					 if (i==BTN3_PTR)
					 {
					   PRINTF("BTN3 long");
						 btn_user.btn_count_long[i]++;
						 btn_user.btn_status[i]=false;
					 }
				 
				 }
				 else
				 {
				   btn_user.btn_count_long[i]++;
					 if (i==BTN1_PTR && btn_user.btn_count_long[i]>BTN_LONG)
					 {
						 btn_user.btn_status[i]=false;
					 }
					 
					 if (i==BTN2_PTR && btn_user.btn_count_long[i]>BTN_LONG)
					 {
						 btn_user.btn_status[i]=false;
						 //buton long++
						 ekranas_hold_down();
					 }
					 
					 if (i==BTN3_PTR && btn_user.btn_count_long[i]>BTN_LONG)
					 {
						 btn_user.btn_status[i]=false;
						 ekranas_hold_up();
					 }
				 }
			}
			else
			{
			   btn_user.btn_count[i]++;
				btn_user.btn_count_long[i]=0;
				 
			}
		
		}
		else
		{
			if (btn_user.btn_count[i])
			{
			  btn_user.btn_count[i]--;
			}
			if (btn_user.btn_status[i])
			{
				if (!btn_user.btn_count[i])
				{
					//dedam status false ir uzdegam eventa
				  btn_user.btn_status[i]=false;
					btn_user.btn_event[i]=true;
				}
			}
		}
	}
	
	
	//handlinam mygtuku eventus
	if (btn_user.btn_event[BTN1_PTR])
	{
		//paspaustas pirmas mygtukas
		PRINTF("BTN1");
		btn_user.btn_event[BTN1_PTR]=false;
		ekranas_press_ok();
	}
	
		if (btn_user.btn_event[BTN2_PTR])
	{
		//paspaustas antras mygtukas
		PRINTF("BTN2");
		btn_user.btn_event[BTN2_PTR]=false;
		ekranas_press_down();
		
	}
		if (btn_user.btn_event[BTN3_PTR])
	{
		//paspaustas trecias mygtukas
		PRINTF("BTN3");
		btn_user.btn_event[BTN3_PTR]=false;
		ekranas_press_up();
		
	}
	
		if (btn_user.btn_event[BTN4_PTR])
	{
		//paspaustas ketvirtas mygtukas
		PRINTF("BTN4");
		btn_user.btn_event[BTN4_PTR]=false;
		
	}
		

	
	
	

}







