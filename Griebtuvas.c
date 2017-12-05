#include "main.h"
#include "Lidar.h"
#include "stm32f4xx_it.h"
#include "debuguart.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "Servo.h"
#include "Griebtuvas.h"
#include "ADC.h"
//#include "zingsniniai_varikliai.h"

GPIO_InitTypeDef  GPIO_MOT;

uint8_t griebtuvas_status=ST_GRIEBTUVAS_STOP;
void Griebtuvas_init(void)
{
	//GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
  //visi ant porto D
	GPIO_MOT.Pin = (GPIO_DIR1_PIN   | GPIO_DIR2_PIN 
									| GPIO_PWM1_PIN | GPIO_PWM2_PIN  );
	

	//initinam D portus
  GPIO_MOT.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_MOT.Pull = GPIO_NOPULL;
  GPIO_MOT.Speed = GPIO_SPEED_MEDIUM;
	HAL_GPIO_Init(GPIOB, &GPIO_MOT); 
	
	//Griebtuvas_stop();
  Griebtuvas_suviniot();


}
uint16_t isviniojimo_laikas=ISVINIOJIMO_LAIKAS;
uint16_t suviniojimo_laikas=SUVINIOJIMO_LAIKAS;
uint16_t servo_laikas=SERVO_LAIKAS;
uint16_t current;
bool flag_paviko_pakelti=false;
void Griebtubas_handler(void)
{

   //stop 
	switch (griebtuvas_status)
	{
		case ST_GRIEBTUVAS_STOP:
			Griebtuvas_stop();
		break;
		
		case ST_GRIEBTUVAS_ISVINIOTI:
			isviniojimo_laikas--;
		Griebtuvas_isviniot();
		if (isviniojimo_laikas<=0)
		{
			Griebtuvas_stop();
		  isviniojimo_laikas=ISVINIOJIMO_LAIKAS;
			suviniojimo_laikas=SUVINIOJIMO_LAIKAS;
			griebtuvas_status=ST_GRIEBTUVAS_NULEISTI;
		}
			
		break;
		
		case ST_GRIEBTUVAS_NULEISTI:
		set_servo(800);
		servo_laikas--;
		if (servo_laikas<=0)
		{
			//Griebtuvas_stop();
		  servo_laikas=SERVO_LAIKAS;
			griebtuvas_status=ST_GRIEBTUVAS_SUVINIOTI;
			break;
		}
	  if (servo_laikas<=10)
		{
		Griebtuvas_suviniot();
		}
		break;
		
		
		case ST_GRIEBTUVAS_SUVINIOTI:
		current=get_adc_current();
		//suviniojam tol kol srove nevirsija arba kai praeina laikas
		suviniojimo_laikas--;
		Griebtuvas_suviniot();
		if (suviniojimo_laikas<=0)
		{
			Griebtuvas_stop();
		  isviniojimo_laikas=ISVINIOJIMO_LAIKAS;
			suviniojimo_laikas=SUVINIOJIMO_LAIKAS;
			griebtuvas_status=ST_GRIEBTUVAS_STOP;
			flag_paviko_pakelti=false;
		}
		else if (current>=CURRENT_STOP)
		{
		  Griebtuvas_stop();
		  isviniojimo_laikas=ISVINIOJIMO_LAIKAS;
			griebtuvas_status=ST_GRIEBTUVAS_STOP;
			flag_paviko_pakelti=true;
		}
		PRINTF("ADC: %u\r\n",current);
		break;
		
		case ST_GRIEBTUVAS_PAKELTI:
		Griebtuvas_stop();
		set_servo(1200);
		servo_laikas--;
		if (servo_laikas<=0)
		{
			//Griebtuvas_stop();
		  servo_laikas=SERVO_LAIKAS;
			griebtuvas_status=ST_GRIEBTUVAS_ISVINIOTI;
			break;
		}
		if (servo_laikas<=10)
		{
		  Griebtuvas_isviniot();
		}
		break;
	
		default:
			Griebtuvas_stop();
		break;
	
	}



}

void set_status_griebtuvas(uint8_t statuss)
{
	if (griebtuvas_status!=statuss)
  {griebtuvas_status=statuss;}
}

uint8_t get_status_griebtuvas(void)
{

	return griebtuvas_status;
}


void Griebtuvas_isviniot(void)
{
	HAL_GPIO_WritePin(GPIO_PWM1_PORT ,GPIO_PWM1_PIN,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIO_PWM2_PORT ,GPIO_PWM2_PIN,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIO_DIR1_PORT ,GPIO_DIR1_PIN,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIO_DIR2_PORT ,GPIO_DIR2_PIN,GPIO_PIN_SET);
	
	


}

void Griebtuvas_suviniot(void)
{
	HAL_GPIO_WritePin(GPIO_PWM1_PORT ,GPIO_PWM1_PIN,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIO_PWM2_PORT ,GPIO_PWM2_PIN,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIO_DIR1_PORT ,GPIO_DIR1_PIN,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIO_DIR2_PORT ,GPIO_DIR2_PIN,GPIO_PIN_RESET);



}




void Griebtuvas_stop(void)
{
	HAL_GPIO_WritePin(GPIO_PWM1_PORT ,GPIO_PWM1_PIN,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIO_PWM2_PORT ,GPIO_PWM2_PIN,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIO_DIR1_PORT ,GPIO_DIR1_PIN,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIO_DIR2_PORT ,GPIO_DIR2_PIN,GPIO_PIN_RESET);


}