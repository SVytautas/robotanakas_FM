#include "main.h"
#include "Lidar.h"
#include "stm32f4xx_it.h"
#include "debuguart.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "Servo.h"

TIM_HandleTypeDef    htim4;
TIM_OC_InitTypeDef   hOC4;

GPIO_InitTypeDef   GPIO_D_servo;



void Servo_init(void)
{


  /*##-1- paleidziame periferijos clk #################################*/
  /* TIM9 Peripheral clock enable */
  TIM_4_CLK_ENABLE();
    
  /* Enable GPIOE Channels Clock */
  GPIOD_CLK_ENABLE();
  
	GPIO_D_servo.Mode = GPIO_MODE_AF_PP;
  GPIO_D_servo.Pull = GPIO_PULLDOWN;
  GPIO_D_servo.Speed = GPIO_SPEED_HIGH;
  GPIO_D_servo.Alternate = GPIO_AF2_TIM4;
  
  GPIO_D_servo.Pin = TIM_4_GPIO_PIN_CHANNEL1;
  HAL_GPIO_Init(TIM_4_GPIO_CHANNEL1_PORT, &GPIO_D_servo);


  //sukonfiguruojame pwm
	htim4.Instance = TIM_4;
  
  htim4.Init.Prescaler = SERVO_PWM_PRESCALER; //preshale kokiu greiciu countins //84/ =50khz
  htim4.Init.Period = SERVO_PWM_PERIOD; // Top reiksme 237hz
  htim4.Init.ClockDivision = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  if(HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
	
	hOC4.OCMode = TIM_OCMODE_PWM1;
  hOC4.OCPolarity = TIM_OCPOLARITY_HIGH;
  hOC4.OCFastMode = TIM_OCFAST_DISABLE;
	hOC4.Pulse = 800;
  if(HAL_TIM_PWM_ConfigChannel(&htim4, &hOC4, TIM_CHANNEL_1) != HAL_OK)
  {
    /* Configuration Error */
    Error_Handler();
  }
	 /* Start channel 2 */
  if(HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1) != HAL_OK)
  {
    /* PWM Generation Error */
    Error_Handler();
  }
}

uint16_t servo_last=0;

void set_servo(uint16_t servo)
{
	if (servo_last!=servo)
	{
   __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, servo);
	}

}


void servo_disable()
{
   __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 0);


}


void servo_nuleisti(void)
{
  set_servo(620);
}


void servo_pakelt(void)
{
  set_servo(1800);
}


void servo_nuleisti_griebtuva(void)
{
    set_servo(1800);
}

void servo_pakelti_griebtuva(void)
{
   set_servo(620);
}