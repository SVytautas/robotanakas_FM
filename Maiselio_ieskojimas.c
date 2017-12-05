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
#include "COLOR.h"
#include "Maiselio_ieskojimas.h"


float santikis_green;
float santikis_green_ref;
float green_atitikimas=0;
uint16_t maiselis_tick=10;
bool flag_turn_on=true;
uint8_t counter=0;
uint16_t griebtuvas_tick=25;

bool flag_set_ieskoti=false;
bool green_yra=false;

void Maiselio_ieskojimas_handler(void)
{
  

  santikis_green=get_santikis_red();
  green_atitikimas=(santikis_green/santikis_green_ref)-1;
	if (green_atitikimas<0)
	{
		green_atitikimas=0;
	}
	else
	{
	  green_atitikimas=green_atitikimas*100;
	}
  if (!maiselis_tick && (green_atitikimas<GREEN_ATITIKIMAS))
	{
		counter++;
		if (counter>200)
		{
			counter=0;
	  santikis_green_ref=santikis_green;
		maiselis_tick=5;
		}
		//maiselis_tick=1;
		
	}
	
	if (flag_turn_on && !maiselis_tick)
	{
	  santikis_green_ref=santikis_green;
		//if (maiselis_tick==1)
		//{
			flag_turn_on=false;
		//}
	}
	
	
	//if (flag_set_ieskoti)
	//{
	if (green_atitikimas>GREEN_ATITIKIMAS && !griebtuvas_tick)
	{
		green_yra=true;
		if (get_status_griebtuvas()==ST_GRIEBTUVAS_STOP)
		{
	  set_status_griebtuvas(ST_GRIEBTUVAS_PAKELTI);}
	}
	//else
	//{
	  //green_yra=false;
	
	//}
  }

//}


void Maiselis_tick(void)
{
   if (maiselis_tick>0)
	 {
	 
	   maiselis_tick--;
	 }
	 if (griebtuvas_tick>0)
	 {
	   griebtuvas_tick--;
	 }

}

void flag_ieskoti_set(bool set)
{

flag_set_ieskoti=set;

}

bool get_green(void)
{

return green_yra;
}

