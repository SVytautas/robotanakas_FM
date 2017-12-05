#include "main.h"
//#include "stm32f4xx_hal.h"
#include "zingsniniai_varikliai.h"
//#include "arm_math.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "debuguart.h"
#include "OFS.h"
#include "Trasos_iveikimas.h"
#include <string.h>
#include "gps_compass.h"
//#include <stdio.h> /* for printf */

TIM_HandleTypeDef    htim2;
TIM_HandleTypeDef    htim3;

/* Timer Output Compare Configuration Structure declaration */
TIM_OC_InitTypeDef hOC2; //otput compare timer2
TIM_OC_InitTypeDef hOC3; //output compare timer3

//dma sturktura timer2
DMA_HandleTypeDef  hdma_tim2;
//dma struktura timer3
DMA_HandleTypeDef  hdma_tim3;

zv_t zv={
  .status=ZV_ST_STOP,
	.test_on=false,
	.greitis_esamas=ZV_GREITIS_MIN,
};

zv_task_t zv_task={
 .task_wp=0,
 .task_rp=0,
 .next_task=false,
 .last_kampas=0,

};



//isejimu strukturos
static GPIO_InitTypeDef  GPIO_D_st;
static GPIO_InitTypeDef  GPIO_B_st;

static uint8_t steper_varikliu_rezoliucija=SET_ZV_EIGHT_STEP;

uint16_t greitis_to=380;
uint16_t greitis_esamas=600;
bool init_ok=false;


void zv_init(void){

	zv_gpio_init();
	//initinam timer2 OC kanala 1
  htim2.Instance = TIM_2;
  htim2.Init.Period            = 0xFFFF;
  htim2.Init.RepetitionCounter = 0;
  htim2.Init.Prescaler         = TIMER_REZOLIUCIJA;
  htim2.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1 ;
  htim2.Init.CounterMode       = TIM_COUNTERMODE_UP;
	
	if(HAL_TIM_OC_Init(&htim2) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
	
	hOC2.OCMode     = TIM_OCMODE_TOGGLE;
  hOC2.OCPolarity = TIM_OCPOLARITY_HIGH;
  hOC2.Pulse      = 0x0;
  //if(HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1) != HAL_OK)
	if(HAL_TIM_OC_ConfigChannel(&htim2, &hOC2, TIM_CHANNEL_3) != HAL_OK)
  {
    /* Configuration Error */
    Error_Handler();
  }
	
	 /*##-3- Start PWM signal generation in DMA mode ############################*/ 
  if(  HAL_TIM_OC_Start(&htim2, TIM_CHANNEL_3) != HAL_OK)
  {
    /* Starting PWM generation Error */
    Error_Handler();
  }
	
	//initinam timer3 kanala 4
	htim3.Instance = TIM_3;
  
  htim3.Init.Period            = 0xFFFF;
  htim3.Init.RepetitionCounter = 0;
  htim3.Init.Prescaler         = TIMER_REZOLIUCIJA; //nustatomas 1mhz
  htim3.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.CounterMode       = TIM_COUNTERMODE_UP;
	
	if(HAL_TIM_OC_Init(&htim3) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
	
	hOC3.OCMode     = TIM_OCMODE_TOGGLE;
  hOC3.OCPolarity = TIM_OCPOLARITY_HIGH;
  hOC3.Pulse      = 0;
  //if(HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1) != HAL_OK)
	if(HAL_TIM_OC_ConfigChannel(&htim3, &hOC3, TIM_CHANNEL_4) != HAL_OK)
  {
    /* Configuration Error */
    Error_Handler();
  }
	
	 /*##-3- Start PWM signal generation in DMA mode ############################*/ 
  if(  HAL_TIM_OC_Start(&htim3, TIM_CHANNEL_4) != HAL_OK)
  {
    /* Starting PWM generation Error */
    Error_Handler();
  }
	//pradedame ARR reiksmes perkelimus is DMA 
	//naikinam dedam reguliuojama per nustatima
	//HAL_TIM_Base_Start(&htim2);
	//HAL_TIM_Base_Start(&htim3);
	//HAL_TIM_Base_Start_DMA(&htim2, (uint32_t*)zingsniu_buferisR, 4);
	//HAL_TIM_Base_Start_DMA(&htim3, (uint32_t*)zingsniu_buferisL, 4);
	//greicio testas
	  //setinam variklius sustabdytus
	  zv.status=ZV_ST_STOP;
	//zv_enable();
	//__HAL_TIM_SET_AUTORELOAD(&htim3,600);
	//__HAL_TIM_SET_AUTORELOAD(&htim2,600);
	//init_ok=true;
}

void zv_handler(void) //100Hz
{
	 if (zv.time) {zv.time--;}
	 if (zv.stop_turn_time) {zv.stop_turn_time--;}
	 if (zv.initial_turn_time) {zv.initial_turn_time--;}
	 if (zv_task.timeout_atstumas) {zv_task.timeout_atstumas--;}
   switch(zv.status)
	 {
		 
		 case ZV_ST_INIT:
			 //initinam 
		 break;
		 
		 case ZV_ST_NONE:
			 //nieko neveikiame
		 //varikliai disablinti
		  zv_disable(); //disablinam variklius
		 break;
		 
		 case ZV_ST_STOP:
			 //initinam 
		  zv_disable(); //disablinam variklius
		 //setinam kad greitis lygus 0
		  
		 if (zv.greitis_esamas!=ZV_GREITIS_MIN)
		 {
			  zv.greitis_esamas=ZV_GREITIS_MIN;
		    update_tim2_period(zv.greitis_esamas);
			  update_tim3_period(zv.greitis_esamas);
		 }
		 
		 if (!zv.time) {
			 //laikas praejo setinam disable
			 zv.status=ZV_ST_NONE; 
		 }
		  
		  //__HAL_TIM_SET_AUTORELOAD(&htim3,zv.greitis_esamas);
	    //__HAL_TIM_SET_AUTORELOAD(&htim2,zv.greitis_esamas);
		 break;
		 
		 case ZV_ST_FORWARD:
		 zv.greitis_to=ZV_GREITIS_VAZIAVIMAS; //vaziavimo greitis
		 zv_enable(); //enablinam ZV
		 set_R_dir_For(); //setinam forward
		 set_L_dir_For(); //setinam forward
		 //jeigu greitis mazesnis greitejam
		 if (zv.greitis_esamas>zv.greitis_to)
		 {
			 zv.greitis_esamas-=ZV_GREITIS_STEP; //didinam po biskiai
			 update_tim2_period(zv.greitis_esamas);
			 update_tim3_period(zv.greitis_esamas);
		   //__HAL_TIM_SET_AUTORELOAD(&htim3,zv.greitis_esamas);
			 //__HAL_TIM_SET_AUTORELOAD(&htim2,zv.greitis_esamas);
		 }
		 
		 if (!zv.time) {
			 //laikas praejo setinam disable
			 zv.status=ZV_ST_STOP; 
			 zv.time=ZV_SUKTI_STOP_TIME; //sustojimo laikas
		 }
		 break;
		 
		 case ZV_ST_FORWARD_OFS:
		 zv.greitis_to=ZV_GREITIS_VAZIAVIMAS; //vaziavimo greitis
		 zv_enable(); //enablinam ZV
		 set_R_dir_For(); //setinam forward
		 set_L_dir_For(); //setinam forward
		 //jeigu greitis mazesnis greitejam
		 if (zv.greitis_esamas>zv.greitis_to)
		 {
			 zv.greitis_esamas-=ZV_GREITIS_STEP; //didinam po biskiai
			 update_tim2_period(zv.greitis_esamas);
			 update_tim3_period(zv.greitis_esamas);
			 //__HAL_TIM_SET_AUTORELOAD(&htim3,zv.greitis_esamas);
		   //__HAL_TIM_SET_AUTORELOAD(&htim2,zv.greitis_esamas);
		 
		 }
		 
		 zv.atstumas_is_mm=ofs_get_posYmm();
		 if (zv.astumas_to_mm<zv.atstumas_is_mm) {
			 //laikas praejo setinam disable
			 zv.status=ZV_ST_STOP; 
			 zv.time=ZV_SUKTI_STOP_TIME; //sustojimo laikas
			// PRINTF("atstumas pasiektas %f\r\n",zv.atstumas_is_mm);
			// PRINTF("stoped %f\r\n int32 %i",ofs_get_posYmm(),ofs_get_possumY());
		 }
		 break;
		 
		 case ZV_ST_BACKWARD_OFS:
		 zv.greitis_to=ZV_GREITIS_VAZIAVIMAS; //vaziavimo greitis
		 zv_enable(); //enablinam ZV
		 set_R_dir_Rev(); //setinam forward
		 set_L_dir_Rev(); //setinam forward
		 //jeigu greitis mazesnis greitejam
		 if (zv.greitis_esamas>zv.greitis_to)
		 {
			 zv.greitis_esamas-=ZV_GREITIS_STEP; //didinam po biskiai
			 update_tim2_period(zv.greitis_esamas);
			 update_tim3_period(zv.greitis_esamas);
			 //__HAL_TIM_SET_AUTORELOAD(&htim3,zv.greitis_esamas);
		   //__HAL_TIM_SET_AUTORELOAD(&htim2,zv.greitis_esamas);
		 
		 }
		 
		 zv.atstumas_is_mm=ofs_get_posYmm();
		 if (zv.astumas_to_mm<zv.atstumas_is_mm) {
			 //laikas praejo setinam disable
			 zv.status=ZV_ST_STOP; 
			 zv.time=ZV_SUKTI_STOP_TIME; //sustojimo laikas
			// PRINTF("atstumas pasiektas bck %f\r\n",zv.atstumas_is_mm);
			 //PRINTF("stoped %f\r\n int32 %i",ofs_get_posYmm(),ofs_get_possumY());
			 Trasos_iveikimas_btn_reset();
			 zv.greitis_esamas=ZV_GREITIS_SUKIMAS;
		 }
		 else if (!zv_task.timeout_atstumas)
		 {
			 zv_task.next_task=true;
			 zv_task.finished=true;
			// PRINTF("atstumas tiemout %f\r\n",zv.atstumas_is_mm);
			// PRINTF("stoped %f\r\n int32 %i",ofs_get_posYmm(),ofs_get_possumY());
			 Trasos_iveikimas_btn_reset();
			 zv.greitis_esamas=ZV_GREITIS_SUKIMAS;
		 }
		 break;
		 
		 case ZV_ST_FORWARD_OFS_NOSTOP:
		 zv.greitis_to=ZV_GREITIS_VAZIAVIMAS; //vaziavimo greitis
		 zv_enable(); //enablinam ZV
		 set_R_dir_For(); //setinam forward
		 set_L_dir_For(); //setinam forward
		 //jeigu greitis mazesnis greitejam
		 if (zv.greitis_esamas>zv.greitis_to)
		 {
			 zv.greitis_esamas-=ZV_GREITIS_STEP; //didinam po biskiai
			 update_tim2_period(zv.greitis_esamas);
			 update_tim3_period(zv.greitis_esamas);
			 //__HAL_TIM_SET_AUTORELOAD(&htim3,zv.greitis_esamas);
		   //__HAL_TIM_SET_AUTORELOAD(&htim2,zv.greitis_esamas);
		 
		 }
		 
		 zv.atstumas_is_mm=ofs_get_posYmm();
		 if (zv.astumas_to_mm<zv.atstumas_is_mm) {
			 //laikas praejo setinam disable
			 //zv.status=ZV_ST_STOP; 
			// zv.time=ZV_SUKTI_STOP_TIME; //sustojimo laikas
			 zv_task.next_task=true;
			 zv_task.finished=true;
			 //PRINTF("atstumas pasiektas %f\r\n",zv.atstumas_is_mm);
			 //PRINTF("stoped %f\r\n int32 %i",ofs_get_posYmm(),ofs_get_possumY());
			 Trasos_iveikimas_reset();
		 }
		 else if (!zv_task.timeout_atstumas)
		 {
			 zv_task.next_task=true;
			 zv_task.finished=true;
			 //PRINTF("atstumas tiemout %f\r\n",zv.atstumas_is_mm);
			 //PRINTF("stoped %f\r\n int32 %i",ofs_get_posYmm(),ofs_get_possumY());
			 Trasos_iveikimas_reset();
		 }
		 break;
		 
		 case ZV_ST_TURN:
		// zv.greitis_to=ZV_GREITIS_VAZIAVIMAS; //vaziavimo greitis
		 zv_enable(); //enablinam ZV
		 set_R_dir_Rev(); //setinam forward
		 set_L_dir_For(); //setinam forward
		 //jeigu greitis mazesnis greitejam
		 if (zv.greitis_esamas!=ZV_GREITIS_SUKIMAS)
		 {
			 zv.greitis_esamas=ZV_GREITIS_SUKIMAS; //didinam po biskiai
			 update_tim2_period(zv.greitis_esamas);
			 update_tim3_period(zv.greitis_esamas);
			//__HAL_TIM_SET_AUTORELOAD(&htim3,zv.greitis_esamas);
			//__HAL_TIM_SET_AUTORELOAD(&htim2,zv.greitis_esamas);
		 
		 }
		 if (!zv.time) {
			 //laikas praejo setinam disable
			 zv.status=ZV_ST_STOP; 
			 zv.time=ZV_SUKTI_STOP_TIME; //sustojimo laikas
		 }
		 break;
		 
		 case ZV_ST_TURN_LEFT:
			 //atliekam jeigu kamppas_to!=+-kampas_esamas stabdom ir sukam
		   //stabdom variklius apibrezta laika
		   if (!zv.stop_turn_time) //jeigu jau sustojom atliekam sukima
			 {
				 //zv.greitis_to=ZV_GREITIS_SUKIMAS;
				 //sukames maziausio kampo kriptimi
				 //nustatome prisinga kripti
				 //setinam kriptis
				 set_R_dir_For();
				 set_L_dir_Rev();
				 zv_enable(); //enablinam ZV
				 //didinam greiti iki sukimo
				// if (zv.greitis_esamas>zv.greitis_to)
		     //{
			   //  zv.greitis_esamas-=ZV_GREITIS_STEP; //didinam po biskiai
			     
		 
		    // }
				 if (zv.greitis_esamas!=ZV_GREITIS_SUKIMAS)
					 {
						 zv.greitis_esamas=ZV_GREITIS_SUKIMAS;
						 update_tim2_period(zv.greitis_esamas);
			       update_tim3_period(zv.greitis_esamas);
					   //__HAL_TIM_SET_AUTORELOAD(&htim3,ZV_GREITIS_SUKIMAS);
			       //__HAL_TIM_SET_AUTORELOAD(&htim2,ZV_GREITIS_SUKIMAS);
					 }
				 
				 //enablinam variklius
				 zv.heading_is=get_gps_compass_headingdeg();
				
				 zv.trik_kampas=zv_GetHeadingError(zv.heading_is,zv.heading_to);
				 //sukames kola trik kampas maziau uz 1laipsni
				 if (!zv.initial_turn_time)
				 {
				 if (abs(zv.trik_kampas)<ZV_SUKTI_TRIK_MIN)
				 {
				   zv.status=ZV_ST_STOP;
					 zv_disable(); //disablinam variklius
					 zv.time=ZV_SUKTI_STOP_TIME; //sustojimo laikas
					// PRINTF("stop %i",(int16_t)zv.trik_kampas);
				 }
				 else if (zv.trik_kampas>0)
				 {
					 //stabdom jei persiverte
				   zv.status=ZV_ST_STOP;
					 zv.time=ZV_SUKTI_STOP_TIME; //sustojimo laikas
					 zv_disable(); //disablinam variklius
					// PRINTF("stop %i",(int16_t)zv.trik_kampas);
				 }
			   }
				 
				 
			 
			 }
			 else
			 {
				 zv_disable(); //disablinam variklius
		    //setinam kad greitis lygus 0
		    zv.greitis_esamas=ZV_GREITIS_MIN;
				 update_tim2_period(zv.greitis_esamas);
			   update_tim3_period(zv.greitis_esamas);
		    //__HAL_TIM_SET_AUTORELOAD(&htim3,zv.greitis_esamas);
	      //__HAL_TIM_SET_AUTORELOAD(&htim2,zv.greitis_esamas);
			 
			 }
		 
		 break;
			 
		 case ZV_ST_TURN_RIGHT:
			 //atliekam jeigu kamppas_to!=+-kampas_esamas stabdom ir sukam
		   //stabdom variklius apibrezta laika
		   if (!zv.stop_turn_time) //jeigu jau sustojom atliekam sukima
			 {
				 //zv.greitis_to=ZV_GREITIS_SUKIMAS;
				 //sukames maziausio kampo kriptimi
				 //nustatome prisinga kripti
				 //setinam kriptis
				 set_R_dir_Rev();
				 set_L_dir_For();
				 zv_enable(); //enablinam ZV
				 //didinam greiti iki sukimo
				// if (zv.greitis_esamas>zv.greitis_to)
		     //{
			   //  zv.greitis_esamas-=ZV_GREITIS_STEP; //didinam po biskiai
			     
		 
		    // }
				 if (zv.greitis_esamas!=ZV_GREITIS_SUKIMAS)
					 {
						 zv.greitis_esamas=ZV_GREITIS_SUKIMAS;
						 update_tim2_period(zv.greitis_esamas);
			       update_tim3_period(zv.greitis_esamas);
					   //__HAL_TIM_SET_AUTORELOAD(&htim3,ZV_GREITIS_SUKIMAS);
			       //__HAL_TIM_SET_AUTORELOAD(&htim2,ZV_GREITIS_SUKIMAS);
					 }
				 
				 //enablinam variklius
				 zv.heading_is=get_gps_compass_headingdeg();
				
				 zv.trik_kampas=zv_GetHeadingError(zv.heading_is,zv.heading_to);
				 //sukames kola trik kampas maziau uz 1laipsni
				 if (!zv.initial_turn_time)
				 {
					 if (abs(zv.trik_kampas)<ZV_SUKTI_TRIK_MIN)
				 {
				   zv.status=ZV_ST_STOP;
					 zv.time=ZV_SUKTI_STOP_TIME; //sustojimo laikas
					 zv_disable(); //disablinam variklius
					 //PRINTF("Rstop %i",(int16_t)zv.trik_kampas);
				 }
				 else if (zv.trik_kampas<0)
				 {
					 //stabdom jei persiverte
				   zv.status=ZV_ST_STOP;
					 zv.time=ZV_SUKTI_STOP_TIME; //sustojimo laikas
					 zv_disable(); //disablinam variklius
					 //PRINTF("Rstop %i",(int16_t)zv.trik_kampas);
				 }
				 
				 }
				 
				 
				 
			 
			 }
			 else
			 {
				 zv_disable(); //disablinam variklius
		    //setinam kad greitis lygus 0
		    zv.greitis_esamas=ZV_GREITIS_MIN;
				update_tim2_period(zv.greitis_esamas);
			  update_tim3_period(zv.greitis_esamas);
		    //__HAL_TIM_SET_AUTORELOAD(&htim3,zv.greitis_esamas);
	      //__HAL_TIM_SET_AUTORELOAD(&htim2,zv.greitis_esamas);
			 
			 }
		 
		 break;
	 
	 
	   default:
			zv.status=ZV_ST_STOP;
      zv.time=ZV_SUKTI_STOP_TIME; //sustojimo laikas		 
		 break;
	 
	 }
	 //greicio testas
	 /*
	  if (init_ok)
	  {
			if (greitis_esamas>greitis_to)
			{
			greitis_esamas-=10;
			__HAL_TIM_SET_AUTORELOAD(&htim3,greitis_esamas);
			__HAL_TIM_SET_AUTORELOAD(&htim2,greitis_esamas);
			}	
		}
  */


}

void zv_task_handler(void) //while ciklas
{
	if (zv.status==ZV_ST_NONE || zv_task.next_task) //jeigu ZV_ST_NONE arba reikia naujo task
	{
		if (zv_task.task_rp!=zv_task.task_wp)
		{
			zv_task.finished=false;
		  //jeigu turim tasku vygdom
			 zv_task.next_task=false; //uzdeda kai ivygdo Zv_ST_FORWARD NO STOP
		   if (zv_kampas(zv_task.kampas_to[zv_task.task_rp])) //jeigu reikia suktis pirma sukam
			 {
			   //sukames
				 if (zv_task.atstumas[zv_task.task_rp]<1) //jeigu mazesnis vaziuojam atgal
					{
						//atgal atstuma
						zv_task.timeout_atstumas=(abs(zv_task.atstumas[zv_task.task_rp])*100)/ROBOTO_GREITIS_TIESIAI;
						zv.astumas_to_mm=ofs_get_posYmm()+abs(zv_task.atstumas[zv_task.task_rp++]); //pliusuojam pointeri
	          zv.status=ZV_ST_BACKWARD_OFS;
						zv_task.zv_task_time=ZV_TASK_DEAD_TIME;
					
					}
					else
					{
					   zv_task.zv_task_time=ZV_TASK_DEAD_TIME_SUKIMAS;
				     zv_task.last_kampas=zv_task.kampas_dstar[zv_task.task_rp];
					
					}
				 
			 }
			 else
			 {
				 //vaziuojam tiesiai be stop
				  if (zv_task.atstumas[zv_task.task_rp]>1)
					{
						zv_task.timeout_atstumas=(zv_task.atstumas[zv_task.task_rp]*100)/ROBOTO_GREITIS_TIESIAI;
						zv.astumas_to_mm=ofs_get_posYmm()+zv_task.atstumas[zv_task.task_rp++]; //pliusuojam pointeri
	          zv.status=ZV_ST_FORWARD_OFS_NOSTOP;
						zv_task.zv_task_time=ZV_TASK_DEAD_TIME;
						
					}
					else if (zv_task.atstumas[zv_task.task_rp]<1)
					{
						//atgal atstuma
						zv_task.timeout_atstumas=(abs(zv_task.atstumas[zv_task.task_rp])*100)/ROBOTO_GREITIS_TIESIAI;
						zv.astumas_to_mm=ofs_get_posYmm()+abs(zv_task.atstumas[zv_task.task_rp++]); //pliusuojam pointeri
	          zv.status=ZV_ST_BACKWARD_OFS;
						zv_task.zv_task_time=ZV_TASK_DEAD_TIME;
					
					}
					else
					{
						//stabdom
						zv.status=ZV_ST_STOP;
					  zv_task.task_rp++;
						zv_task.zv_task_time=0;
					}
			    
				  
			 }
		}
		else if (zv_task.next_task)
		{
		   //stabdoma nes neturim nauju ir nesustabde
			 zv.status=ZV_ST_STOP;
			 zv.time=ZV_SUKTI_STOP_TIME; //sustojimo laikas
			 zv_task.next_task=false;
			 zv_task.zv_task_time=0;
			zv_task.finished=true;
		}
		else 
		{ 
			//ir papildomai stabdoma
		   zv.status=ZV_ST_STOP; //bet nesetinam laiko
			 zv_task.zv_task_time=0;
			zv_task.finished=true;
		}
	
	}

}

bool zv_task_finished(void)
{
  if (zv_task.task_rp==zv_task.task_wp && zv_task.finished)
	{
	  return true;
	}
	else
	{
	  return false;
	}

}

bool get_zv_dead_time(void)
{
  if (!zv_task.zv_task_time)
	{
	  return true;
	}
	else 
	{
	  return false;
	}
}

void zv_tick_10hz(void)
{
  if (zv_task.zv_task_time)
	{
	 zv_task.zv_task_time--;
	}
}

void zv_task_push(float kampas_to, float atstumas, float dstar_kampas)
{
   zv_task.kampas_to[zv_task.task_wp]=kampas_to;
	 zv_task.kampas_dstar[zv_task.task_wp]=dstar_kampas;
	 zv_task.atstumas[zv_task.task_wp++]=atstumas;
	 
}

void zv_task_push2(float kampas_to, float atstumas)
{
   zv_task.kampas_to[zv_task.task_wp]=kampas_to;
	 zv_task.atstumas[zv_task.task_wp++]=atstumas;
	 
}

float zv_get_last_kampas_dstar(void)
{
  return zv_task.last_kampas;
}

void zv_task_clear_stop(void)
{
   zv_task.task_rp=zv_task.task_wp;
	// zv_disable();
	 zv.status=ZV_ST_STOP;
	 zv.time=ZV_SUKTI_STOP_TIME; //sustojimo laikas
	 zv_task.finished=true;
}


void zv_set_forward(uint16_t time10ms)
{

  zv.status=ZV_ST_FORWARD;
  zv.time=time10ms;	
}


void zv_set_turn(uint16_t time10ms)
{

  zv.status=ZV_ST_TURN;
  zv.time=time10ms;	
}


void zv_gpio_init(void)
{
	//enable isejimus D ir B portai
	GPIOD_CLK_ENABLE();
	GPIOB_CLK_ENABLE();
	
  //visi ant porto D
	GPIO_D_st.Pin = (R_VARIKLIS_MS1_GPIO_PIN  | L_VARIKLIS_MS1_GPIO_PIN
									| R_VARIKLIS_MS2_GPIO_PIN | L_VARIKLIS_MS2_GPIO_PIN 
									| R_VARIKLIS_MS2_GPIO_PIN | L_VARIKLIS_MS2_GPIO_PIN
									|  L_VARIKLIS_ENABLE_PIN  | R_VARIKLIS_DIR_PIN );
	

	//initinam D portus
  GPIO_D_st.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_D_st.Pull = GPIO_NOPULL;
  GPIO_D_st.Speed = GPIO_SPEED_MEDIUM;
	HAL_GPIO_Init(GPIOD, &GPIO_D_st); 
	
	GPIO_B_st.Pin = (R_VARIKLIS_ENABLE_PIN   | L_VARIKLIS_DIR_PIN);
	GPIO_B_st.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_B_st.Pull = GPIO_NOPULL;
  GPIO_B_st.Speed = GPIO_SPEED_MEDIUM;
	HAL_GPIO_Init(GPIOB, &GPIO_B_st);
	
	
	ZV_change_step_rez(steper_varikliu_rezoliucija);
	
	
	//pradine DIR reiksme
	HAL_GPIO_WritePin(R_VARIKLIS_DIR_PORT,R_VARIKLIS_DIR_PIN ,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(L_VARIKLIS_DIR_PORT	,L_VARIKLIS_DIR_PIN ,GPIO_PIN_SET);
	
	//pradine disable reiksme
	HAL_GPIO_WritePin(R_VARIKLIS_ENABLE_PORT,R_VARIKLIS_ENABLE_PIN,GPIO_PIN_SET);
	HAL_GPIO_WritePin(L_VARIKLIS_ENABLE_PORT,L_VARIKLIS_ENABLE_PIN,GPIO_PIN_SET);

}

void ZV_change_step_rez(uint8_t res_status)
{
	switch (res_status){
	
		case SET_ZV_FULL_STEP: //full step
			//setinam MS1 ir MS2 low
		//pirmas variklis
		 HAL_GPIO_WritePin(R_VARIKLIS_MS1_GPIO_PORT ,R_VARIKLIS_MS1_GPIO_PIN,GPIO_PIN_RESET);
		 HAL_GPIO_WritePin(R_VARIKLIS_MS2_GPIO_PORT ,R_VARIKLIS_MS2_GPIO_PIN,GPIO_PIN_RESET);
		 HAL_GPIO_WritePin(R_VARIKLIS_MS3_GPIO_PORT ,R_VARIKLIS_MS3_GPIO_PIN,GPIO_PIN_RESET);
		//antras variklis
		 HAL_GPIO_WritePin(L_VARIKLIS_MS1_GPIO_PORT ,L_VARIKLIS_MS1_GPIO_PIN,GPIO_PIN_RESET);
		 HAL_GPIO_WritePin(L_VARIKLIS_MS2_GPIO_PORT ,L_VARIKLIS_MS2_GPIO_PIN,GPIO_PIN_RESET);
		 HAL_GPIO_WritePin(L_VARIKLIS_MS3_GPIO_PORT ,L_VARIKLIS_MS3_GPIO_PIN,GPIO_PIN_RESET);
			
		break;
	  
		case SET_ZV_HALF_STEP:
			//setinam MS1 H ir MS2 L  MS3 L
		//pirmas variklis
		 HAL_GPIO_WritePin(R_VARIKLIS_MS1_GPIO_PORT ,R_VARIKLIS_MS1_GPIO_PIN,GPIO_PIN_SET);
		 HAL_GPIO_WritePin(R_VARIKLIS_MS2_GPIO_PORT ,R_VARIKLIS_MS2_GPIO_PIN,GPIO_PIN_RESET);
		 HAL_GPIO_WritePin(R_VARIKLIS_MS3_GPIO_PORT ,R_VARIKLIS_MS3_GPIO_PIN,GPIO_PIN_RESET);
		//antras variklis
		 HAL_GPIO_WritePin(L_VARIKLIS_MS1_GPIO_PORT ,L_VARIKLIS_MS1_GPIO_PIN,GPIO_PIN_SET);
		 HAL_GPIO_WritePin(L_VARIKLIS_MS2_GPIO_PORT ,L_VARIKLIS_MS2_GPIO_PIN,GPIO_PIN_RESET);
		 HAL_GPIO_WritePin(L_VARIKLIS_MS3_GPIO_PORT ,L_VARIKLIS_MS3_GPIO_PIN,GPIO_PIN_RESET);
			
		break;
		
		case SET_ZV_QUATER_STEP:
			//setinam MS1 L ir MS2 H MS3 L
		//pirmas variklis
		 HAL_GPIO_WritePin(R_VARIKLIS_MS1_GPIO_PORT ,R_VARIKLIS_MS1_GPIO_PIN,GPIO_PIN_RESET);
		 HAL_GPIO_WritePin(R_VARIKLIS_MS2_GPIO_PORT ,R_VARIKLIS_MS2_GPIO_PIN,GPIO_PIN_SET);
		 HAL_GPIO_WritePin(R_VARIKLIS_MS3_GPIO_PORT ,R_VARIKLIS_MS3_GPIO_PIN,GPIO_PIN_RESET);
		//antras variklis
		 HAL_GPIO_WritePin(L_VARIKLIS_MS1_GPIO_PORT ,L_VARIKLIS_MS1_GPIO_PIN,GPIO_PIN_RESET);
		 HAL_GPIO_WritePin(L_VARIKLIS_MS2_GPIO_PORT ,L_VARIKLIS_MS2_GPIO_PIN,GPIO_PIN_SET);
		 HAL_GPIO_WritePin(L_VARIKLIS_MS3_GPIO_PORT ,L_VARIKLIS_MS3_GPIO_PIN,GPIO_PIN_RESET);
			
		break;
		
		case SET_ZV_EIGHT_STEP:
			//setinam MS1 H ir MS2 H MS3 L
		//pirmas variklis
		 HAL_GPIO_WritePin(R_VARIKLIS_MS1_GPIO_PORT ,R_VARIKLIS_MS1_GPIO_PIN,GPIO_PIN_SET);
		 HAL_GPIO_WritePin(R_VARIKLIS_MS2_GPIO_PORT ,R_VARIKLIS_MS2_GPIO_PIN,GPIO_PIN_SET);
		 HAL_GPIO_WritePin(R_VARIKLIS_MS3_GPIO_PORT ,R_VARIKLIS_MS3_GPIO_PIN,GPIO_PIN_RESET);
		//antras variklis
		 HAL_GPIO_WritePin(L_VARIKLIS_MS1_GPIO_PORT ,L_VARIKLIS_MS1_GPIO_PIN,GPIO_PIN_SET);
		 HAL_GPIO_WritePin(L_VARIKLIS_MS2_GPIO_PORT ,L_VARIKLIS_MS2_GPIO_PIN,GPIO_PIN_SET);
		 HAL_GPIO_WritePin(L_VARIKLIS_MS3_GPIO_PORT ,L_VARIKLIS_MS3_GPIO_PIN,GPIO_PIN_RESET);
			
		break;
		
		case SET_ZV_SIXTEENTH_STEP:
			//setinam MS1 H ir MS2 H MS3 L
		//pirmas variklis
		 HAL_GPIO_WritePin(R_VARIKLIS_MS1_GPIO_PORT ,R_VARIKLIS_MS1_GPIO_PIN,GPIO_PIN_SET);
		 HAL_GPIO_WritePin(R_VARIKLIS_MS2_GPIO_PORT ,R_VARIKLIS_MS2_GPIO_PIN,GPIO_PIN_SET);
		 HAL_GPIO_WritePin(R_VARIKLIS_MS3_GPIO_PORT ,R_VARIKLIS_MS3_GPIO_PIN,GPIO_PIN_RESET);
		//antras variklis
		 HAL_GPIO_WritePin(L_VARIKLIS_MS1_GPIO_PORT ,L_VARIKLIS_MS1_GPIO_PIN,GPIO_PIN_SET);
		 HAL_GPIO_WritePin(L_VARIKLIS_MS2_GPIO_PORT ,L_VARIKLIS_MS2_GPIO_PIN,GPIO_PIN_SET);
		 HAL_GPIO_WritePin(L_VARIKLIS_MS3_GPIO_PORT ,L_VARIKLIS_MS3_GPIO_PIN,GPIO_PIN_RESET);
			
		break;
		
		
		default:
			//full step default
		 //pirmas variklis
		 HAL_GPIO_WritePin(R_VARIKLIS_MS1_GPIO_PORT ,R_VARIKLIS_MS1_GPIO_PIN,GPIO_PIN_RESET);
		 HAL_GPIO_WritePin(R_VARIKLIS_MS2_GPIO_PORT ,R_VARIKLIS_MS2_GPIO_PIN,GPIO_PIN_RESET);
		 HAL_GPIO_WritePin(R_VARIKLIS_MS3_GPIO_PORT ,R_VARIKLIS_MS3_GPIO_PIN,GPIO_PIN_RESET);
		//antras variklis
		 HAL_GPIO_WritePin(L_VARIKLIS_MS1_GPIO_PORT ,L_VARIKLIS_MS1_GPIO_PIN,GPIO_PIN_RESET);
		 HAL_GPIO_WritePin(L_VARIKLIS_MS2_GPIO_PORT ,L_VARIKLIS_MS2_GPIO_PIN,GPIO_PIN_RESET);
		 HAL_GPIO_WritePin(L_VARIKLIS_MS3_GPIO_PORT ,L_VARIKLIS_MS3_GPIO_PIN,GPIO_PIN_RESET);
		
		break;
	
	}

}



//

void zv_enable(void){
  HAL_GPIO_WritePin(R_VARIKLIS_ENABLE_PORT,R_VARIKLIS_ENABLE_PIN,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(L_VARIKLIS_ENABLE_PORT,L_VARIKLIS_ENABLE_PIN,GPIO_PIN_RESET);
}



void zv_disable(void){
  HAL_GPIO_WritePin(R_VARIKLIS_ENABLE_PORT,R_VARIKLIS_ENABLE_PIN,GPIO_PIN_SET);
	HAL_GPIO_WritePin(L_VARIKLIS_ENABLE_PORT,L_VARIKLIS_ENABLE_PIN,GPIO_PIN_SET);
}

void set_R_dir_For(void)
{
   HAL_GPIO_WritePin(R_VARIKLIS_DIR_PORT,R_VARIKLIS_DIR_PIN ,GPIO_PIN_RESET);
}
void set_R_dir_Rev(void)
{
   HAL_GPIO_WritePin(R_VARIKLIS_DIR_PORT,R_VARIKLIS_DIR_PIN ,GPIO_PIN_SET);
}

void set_L_dir_For(void)
{
   HAL_GPIO_WritePin(L_VARIKLIS_DIR_PORT	,L_VARIKLIS_DIR_PIN ,GPIO_PIN_SET);
}
void set_L_dir_Rev(void)
{
   HAL_GPIO_WritePin(L_VARIKLIS_DIR_PORT	,L_VARIKLIS_DIR_PIN ,GPIO_PIN_RESET);
}

void test_turn_right(void)
{
	   if (!zv.test_on)
		 {
			 //zv.test_on=true;
			 zv.status=ZV_ST_TURN_RIGHT; 
	     zv.stop_turn_time=ZV_SUKTI_STOP_TIME;
	     zv.heading_to=330;
		 
		 }
     
}

void test_turn_left(void)
{
	   if (!zv.test_on)
		 {
			 zv.test_on=true;
			 
	     zv.stop_turn_time=ZV_SUKTI_STOP_TIME;
	     zv.heading_to=330;
			 zv.heading_is=get_gps_compass_headingdeg();
			 zv.trik_kampas=zv_GetHeadingError(zv.heading_is, zv.heading_to);
			   if (abs(zv.trik_kampas)>150)
				 {
				   zv.initial_turn_time=ZV_INITIAL_TURN_TIME;
				 }
				 else
				 {
				   zv.initial_turn_time=0;
				 }
				 
				 
			 if(zv.trik_kampas<0)
			 {
				 //sukam i kaire
				 
			   zv.status=ZV_ST_TURN_LEFT; 
			 
			 }
			 else
			 {
			   //sukam i desine
				 zv.status=ZV_ST_TURN_RIGHT; 
			 }
		 
		 }
     
}

bool zv_kampas(float kampas_to) //jei reikia suktis grazinam true
{
	
			zv.heading_to=kampas_to;
			zv.heading_is=get_gps_compass_headingdeg();
			zv.trik_kampas=zv_GetHeadingError(zv.heading_is, zv.heading_to);
	
	   if (abs(zv.trik_kampas)>KAMPAS_SUKTI_ERROR)
		 {
			   zv.stop_turn_time=ZV_SUKTI_STOP_TIME;
	     
			   if (abs(zv.trik_kampas)>150)
				 {
				   zv.initial_turn_time=ZV_INITIAL_TURN_TIME;
				 }
				 else
				 {
				   zv.initial_turn_time=0;
				 }
				 
				 
			   if(zv.trik_kampas<0)
			   {
				   //sukam i kaire
				 
			    zv.status=ZV_ST_TURN_LEFT; 
			 
			   }
			   else
			   {
			   //sukam i desine
				 zv.status=ZV_ST_TURN_RIGHT; 
			  }
		    return true;
		 
		 }
		 else
		 {
		    return false;
		 }
	
	     


}



void zv_forward(float mm)
{
   zv.astumas_to_mm=ofs_get_posYmm()+mm;
	// PRINTF("startes %f\r\n int32 %i",ofs_get_posYmm(),ofs_get_possumY());
	 zv.status=ZV_ST_FORWARD_OFS;

}

void test_zv_forfard(void)
{
	if (!zv.test_on)
	{
	  zv_forward(1000);
		zv.test_on=true;
	}
  
}

void test_reset(void)
{
  zv.test_on=false;
}





float zv_GetHeadingError(float initial, float final)
{
    bool flipped = false;
    if (initial > final)
    {
        float temp;
        temp = final;
        final = initial;
        initial = temp;
        flipped = true;
    }
    float error;
    if (final - initial > 180)
        final = final - 360;

    error = final - initial;

    if (flipped == true) {error = -error;};
    return error;
}


void update_tim2_period(uint16_t period)
{
	htim2.Instance = TIM_2;
  htim2.Init.Period            = period;
  htim2.Init.RepetitionCounter = 0;
  htim2.Init.Prescaler         = TIMER_REZOLIUCIJA;
  htim2.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1 ;
  htim2.Init.CounterMode       = TIM_COUNTERMODE_UP;
  HAL_TIM_OC_Init(&htim2);
	hOC2.OCMode     = TIM_OCMODE_TOGGLE;
  hOC2.OCPolarity = TIM_OCPOLARITY_HIGH;
  hOC2.Pulse      = 0x0;
	HAL_TIM_OC_ConfigChannel(&htim2, &hOC2, TIM_CHANNEL_3);
  HAL_TIM_OC_Start(&htim2, TIM_CHANNEL_3);
}

void update_tim3_period(uint16_t period)
{
	htim3.Instance = TIM_3;
  htim3.Init.Period            = period;
  htim3.Init.RepetitionCounter = 0;
  htim3.Init.Prescaler         = TIMER_REZOLIUCIJA; //nustatomas 1mhz
  htim3.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.CounterMode       = TIM_COUNTERMODE_UP;
  HAL_TIM_OC_Init(&htim3);
  hOC3.OCMode     = TIM_OCMODE_TOGGLE;
  hOC3.OCPolarity = TIM_OCPOLARITY_HIGH;
  hOC3.Pulse      = 0;
	HAL_TIM_OC_Start(&htim3, TIM_CHANNEL_4);
}


