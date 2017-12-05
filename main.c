/**
  ******************************************************************************
  * @file    main.c 
  * @author  Vytautas Senvaitis magistrinis
  * @version V1.2.2
  * @date    09-October-2015
  * @brief   Main program body
  *
  * @note    roboto main programa
  ******************************************************************************
  * @attention
  * STM32F407
  * naudojama TIM2 TIM3 timeriai su DMA ir OC toggle 1 kanalo isejimu PA
	* UART XV-11 jutikliui
	* UART sujungimui su kompiuteriu debuginimui
  ******************************************************************************
  */
//
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "zingsniniai_varikliai.h"
#include "Board_Buttons.h"
#include "Board_LED.h"
#include "Lidar.h"
#include "debuguart.h"
//#include "Kalman_filtras.h"
#include "OFS.h"
#include "Servo.h"
#include "griebtuvas.h"
#include "Trasos_iveikimas.h"
#include "ADC.h"
#include "COLOR.h"
#include "Maiselio_ieskojimas.h"
#include "PD8544.h"
#include "btn_nokia.h"
#include "eeprom.h"
#include <stdio.h>
#include "Ekranas_atv.h"
#include "btn_bumper.h"
#include "Ultragarsas.h"
#include "papildomi.h"
#include "gps_compass.h"
#include "delay_simple.h"
#include "dstar.h"

extern UART_HandleTypeDef Uart_lidar;
extern DMA_HandleTypeDef hdma_tx;
extern DMA_HandleTypeDef hdma_rx;


nust_zv_t nust_zv={
  .max_greitis=380,
	.max_sukimas=500,
	.acc_step=7,
	.X=500001,
};

calib_comp_t calib_comp;
calib_ofs_t calib_ofs;
//-------------------------main programos pradzia--------------------------------
int main(void)
{
	//--------------procesoriaus initas--------------------
  _delay_us(300);
  HAL_Init();
  
  /* Configure the system clock to 168 MHz */
  SystemClock_Config();
	sys_tick_init();
	HAL_Delay(100);
	Delay_Init();
	//-----------------------------------------------------
	debuguartinit();
	PRINTF("start mcu\r\n");
	PRINTF("CPU clk:%lu\r\n",HAL_RCC_GetSysClockFreq());
	
	PCD8544_Init(0x30);
	ekranas_set_status(EKRANAS_ST_INIT);
	//userio initai
	btn_nokia_init(); //btn inicilizavimas nokia keranelis
	btn_bump_init(); //btn bumper inicilizavimas
	
	Ultragarsas_init(); //ultragarso inicializavimas
	papildomi_init();
	//pap_beep_mult(255,5);
	//Zv initas
	zv_init();
	//eeprom initas su color i2c
	COLOR_init();
	//color_ee_write_block((char *)(&nust_zv),0,sizeof(nust_zv));
	//nuskaitom isaugotus parametrus
	color_ee_read_block((char *)(&nust_zv),EE_ADD_ZV,sizeof(nust_zv));
	//----------------------Compaso nustatimai-----------------------------------
	color_ee_read_block((char *)(&calib_comp),EE_ADD_CCOMPSS,sizeof(calib_comp));
	PRINTF("calib compas\r\n");
	PRINTF("xoffs: %f\r\n",calib_comp.xOffset);
	PRINTF("yoffs: %f\r\n",calib_comp.yOffset);
	//setinam x_offs ir y_offs
	gps_compass_set_calib();
	//---------------------------------------------------------------------------
	//---------------------Ofs kalibravimas--------------------------------------
	color_ee_read_block((char *)(&calib_ofs),EE_ADD_OFS,sizeof(calib_ofs));
	PRINTF("calib ofs\r\n");
	PRINTF("ofs: %f\r\n",calib_ofs.tomm_koef);
	ofs_set_kalib();
	//---------------------------------------------------------------------------
	
	PRINTF("greitis %u sukimas %u acc %u",nust_zv.max_greitis,nust_zv.max_sukimas,nust_zv.acc_step);
  PRINTF("X %llu\r\n",nust_zv.X);
  //Buttons_Initialize();
	LED_Initialize ();
	//set_steps(100, 100, 100);
	Init_ADC();
	
	
  Lidar_init();
	//LED_On (3);
	Lidar_uart_init();
	//Lidar_motor_power(85);
	//Lidar_motor_power(85);
	Lidar_motor_power(65); //50-180 
  /* Infinite loop */
	
	gps_compass_init();
	
	
	OFS_init();
	
	//Griebtuvas_init();
	//servo variklio init
	//Servo_init();
	//griebtuvas pin init
	//Griebtuvas_init();
	//COLOR_init();
  //btn_nokia_init(); //btn inicilizavimas nokia keranelis
	
	
	
	//#ifdef ROBOTAS
	//while(read_count);
	//#endif
	//stepper_init();
	
	//Kalman_Init();
	//Kalman_handler();
	
	//dstart testai
	//dstar_reset_map();
	//dstar_add_obstacle(0,0);
	//dstar_add_obstacle(5,4);
	//dstar_add_obstacle(4,4);
	//dstar_add_obstacle(3,4);
	//dstar_add_obstacle(5,4);
	//dstar_add_obstacle(6,4);
	//dstar_add_obstacle(9,4);
	//dstar_add_obstacle(4,9);
	//dstar_plan(6, 0, 6, 11);
	//dstar_plan_command(0);
	//dstar_printf_map();
	//dstar_printf_path();
	//dstar_push_command_to_ZV();
	
	
	//zv_task_push(230, 500);
	//zv_task_push(140, 500);
	
	Servo_init();
	
  while (1)
  {
		Lidar_handleris(); //parsinam jei turim duomenu
		gps_handler();
		zv_task_handler();
		Trasos_iveikimas_handler();

  }
	
}



//---------------------------main programos pabaiga--------------------------

uint16_t sys_tick_counter=0;
bool a,c,d;

uint8_t ofs_handler_30ms=3;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==SYS_TIMERx)
	{
  //BSP_LED_Toggle(LED1);
	switch (sys_tick_counter)
	{
		case 1: //1s eventas
		if (d) {LED_On(1);} else {LED_Off(1);} d=!d;	//blink led
     

    
		//Maiselis_tick();
		Trasos_iveikimas_tick_1s();
		
		break;
		
    //	10ms eventas	
		case 10: case 20: case 30: case 40: case 50: case 60: case 70: case 80: case 90: 
		case 110: case 120: case 130: case 140: case 150: case 160: case 170: case 180: case 190: case 100:
		case 210: case 220: case 230: case 240: case 250: case 260: case 270: case 280: case 290: case 200:
		case 310: case 320: case 330: case 340: case 350: case 360: case 370: case 380: case 390: case 300:
    case 410: case 420: case 430: case 440: case 450: case 460: case 470: case 480: case 490: case 400:
    case 510: case 520: case 530: case 540: case 550: case 560: case 570: case 580: case 590: case 500:
    case 610: case 620: case 630: case 640: case 650: case 660: case 670: case 680: case 690: case 600:
    case 710: case 720: case 730: case 740: case 750: case 760: case 770: case 780: case 790: case 700:
    case 810: case 820: case 830: case 840: case 850: case 860: case 870: case 880: case 890: case 800:
    case 910: case 920: case 930: case 940: case 950: case 960: case 970: case 980: case 990: case 900:	
    case 1000:
		if (c) {LED_On(2);} else {LED_Off(2);} c=!c;//blink led
		
		//Maiselio_ieskojimas_handler();
		//Griebtubas_handler();
		
		
		btn_nokia_handler();
		btn_bump_handler();
		zv_handler();
		
		gps_compass_handler();
		zv_tick_10hz();
		break;
		
		//	100ms eventas	
		case 5: case 105: case 205: case 305: case 405: case 505: case 605: case 705: case 805: case 905: 
		if (a) {LED_On(3);} else {LED_Off(3);}	a=!a;//blink led
		
		ofs_handler();
		ekranas_atv_handler();
		read_RGB();
		color_find_min_calc_santikis();
		objekto_find_handleris();
		//zv_handler();
		break;
		
		
		default:	
		break;
	
	
	}
	
	sys_tick_counter++;
	HAL_IncTick();
	if (sys_tick_counter>=1000) {sys_tick_counter=0;}
	//-----------------------1khz
	Ultragarsas_handler();
  papildomi_beep_handler();
	adc_handler();
	//---------------------------
	
	}
	
	

}
































//
#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
