#include "main.h"
#include "Ekranas_atv.h"
#include "PD8544.h"
#include "Ekranas_atv_str.h"
#include "debuguart.h"
#include "OFS.h"
#include "ADC.h"
#include "btn_bumper.h"
#include "Ultragarsas.h"
#include "lidar.h"
#include "gps_compass.h"
#include "zingsniniai_varikliai.h"
#include "Trasos_iveikimas.h"
#include "Servo.h"
#include "COLOR.h"



ekranas_t ekranas={
   .status=EKRANAS_ST_NIEKO,
	 .colum_ptr=0,
	 .line_ptr={0,0,0,0},
	 .lock_control=false,
	 .delay=20,
	 .print_timeout=0,
};




void ekranas_atv_handler(void) //10hz
{
	if (ekranas.delay) {ekranas.delay--;}
	if (ekranas.print_timeout) {ekranas.print_timeout--;}
	//ekrano atnaujinimas 100hz
	switch (ekranas.status)
	{
		
		case EKRANAS_ST_INIT:
		//ekranas.status=EKRANAS_ST_MENIU;		
		PCD8544_Clear();
		//PCD8544_GotoXY(0, 0);
		//NOKIA_PRINTF("%s",start_img);
		PCD8544_fillbuff((char *)init_img);
		PCD8544_Refresh();
		if (!ekranas.delay) {ekranas.status=EKRANAS_ST_MENIU;}
		break;
		
		case EKRANAS_ST_MENIU:
		PCD8544_Clear();
		//read_RGB();
	  uint8_t line=0;
	  char buf[40];

	  
		for (uint8_t i=0; i<NOKIA_SCREEN_LINES;i++)
	  {
			
			//sprintf(buf,ekranas_get_str(ekranas.colum_ptr,ekranas.line_ptr[0],ekranas.line_ptr[1]));
		  //NOKIA_PRINTF(" %u.%s",i, (char *)first_layer[0]);
			if (ekranas.colum_ptr==0)
			{
				 	  //surandam ar reikia scrolint
	       if (ekranas.line_ptr[0]>NOKIA_SCREEN_LINES-1)
				 {
					 PCD8544_GotoXY(10, line+i*9);
					 NOKIA_PRINTF("%s", ekranas_get_str(i+(ekranas.line_ptr[0]-(NOKIA_SCREEN_LINES-1))));
					 if (i==NOKIA_SCREEN_LINES-1)
					 {
					    PCD8544_GotoXY(0, line+i*9);
						  NOKIA_PRINTF(">>");
					 }
				 }
				 else
				 {
					 PCD8544_GotoXY(10, line+i*9);
				   NOKIA_PRINTF("%s", ekranas_get_str(i));
					 if (i==ekranas.line_ptr[0])
					 {
					    PCD8544_GotoXY(0, line+i*9);
						  NOKIA_PRINTF(">>");
					 }
				 }
			   
			}
			else if (ekranas.colum_ptr==1)
			{
			  if (ekranas.line_ptr[1]>(NOKIA_SCREEN_LINES-1))
				 {
					 PCD8544_GotoXY(10, line+i*9);
					 NOKIA_PRINTF("%s", ekranas_get_str(i+(ekranas.line_ptr[1]-(NOKIA_SCREEN_LINES-1))));
					 if (i==NOKIA_SCREEN_LINES-1)
					 {
					    PCD8544_GotoXY(0, line+i*9);
						  NOKIA_PRINTF(">>");
					 }
				 }
				 else
				 {
					 PCD8544_GotoXY(10, line+i*9);
				   NOKIA_PRINTF("%s", ekranas_get_str(i));
					 if (i==ekranas.line_ptr[1])
					 {
					    PCD8544_GotoXY(0, line+i*9);
						  NOKIA_PRINTF(">>");
					 }
				 }
				
			}
			
		}
		
		PCD8544_Refresh();
			
		break;
		
		
		case EKRANAS_ST_NIEKO:
			//laukiama INIT
		if (!ekranas.delay) {
		
		ekranas.status=EKRANAS_ST_INIT;
		ekranas.delay=40;
		}
		break;
		
		case EKRANAS_ST_VYGDOMA:
		ekranas.atv_par_status=0;
		ekranas.atv_par_status=(uint32_t)ekranas.line_ptr[0]<<8 | ekranas.line_ptr[1];
		//PRINTF("%u %u",ekranas.line_ptr[0],ekranas.line_ptr[1]);
		switch (ekranas.atv_par_status)
		{
			case (0<<8 | 1):
			//test R
			//iki linijos
			trasos_iveikimas_start_iki_linijos();
			Ti_print_nokia();
       		
			break;
			
			case (0<<8 | 2):
			//test R
			//iki linijos
			trasos_iveikimas_paieska();
			color_print_rgb();
       		
			break;
			
			
			//testai
			case (1<<8 | 1):
			//test R
			test_turn_right();
       		
			break;
			
			case (1<<8 | 2):
			//test R
			test_turn_left();
			break;
			
			case (1<<8 | 3):
			//test R
			test_zv_forfard();
			break;
			
			case (1<<8 | 4):
			//test R
			test_reset();
			break;
			
			case (1<<8 | 5):
			//test R
			servo_pakelt();
			break;
			
			case (1<<8 | 6):
			//test R
			servo_nuleisti();
			break;
			
			case (1<<8 | 7):
			//test R
			servo_disable();
			break;
			
			//Info case ---------------------------------------
			case (2<<8 | 1):
			//OFS XY
      OFSXY_print_nokia();			
			break;
			
			case (2<<8 | 2):
			//OFS XY
      adc_print_nokia_batVoltages();		
			
			break;
			
			case (2<<8 | 3):
			//OFS XY
     // adc_print_nokia_batVoltages();		
			btn_bump_ekranas_print();
			break;
			
			case (2<<8 | 4):
			//ultragrass
     // adc_print_nokia_batVoltages();
 			if (!ekranas.print_timeout)
			{
				ultragarsas_print_nokia();
				ekranas.print_timeout=10; //kas desimta
				
			}
			
			break;
			
			case (2<<8 | 5):
			//lidar
			lidar_print_nokia();
			break;
			
			case (2<<8 | 6):
			//Gps
			gps_print_nokia();
			break;
			
			case (2<<8 | 7):
			//Gps
			gps_nokia_print_compass();
			break;
			
			
			case (2<<8 | 8):
			//Gps
			//gps_nokia_print_compass();
			lidar_to_XY(12 , 6, 0, 333);
			break;
			
			
			//kalibravimai state--------------------------------------
			
			case (3<<8 | 1):
			//compass kalibravimas 
			//setinam klaibruoti
			gps_compass_set_calibrate();
			gps_nokia_print_compass_calib(); //kalibruojam
			break;
			
			case (3<<8 | 2): 
			//OFS kalibravimas
			ofs_start_calib();
			OFS_print_calib();
			break;
			
			case (4<<8 | 1):
			//rgb
			color_print_rgb();
			break;
			
			
			
		
		}
			
		
		break;
		
		default:
			ekranas.status=EKRANAS_ST_MENIU;	
		break;
	
	
	
	}
	
    

}

char * ekranas_get_str(uint8_t numb){
	if (ekranas.colum_ptr==0)
	{
	  //return (char *)first_layer[ekranas.line_ptr[0]+numb];
		return (char *)first_layer[numb];
	}
	else if (ekranas.colum_ptr==1)
	{
	  //return (char *)second_layer[ekranas.line_ptr[0]][ekranas.line_ptr[1]+numb];
		return (char *)second_layer[ekranas.line_ptr[0]][numb];
	}
	else 
	{
	  return (char *)null_strr;
	}

}

uint8_t ekranas_get_cur_size(void)
{
	if (ekranas.colum_ptr==0)
	{
	  //return (char *)first_layer[ekranas.line_ptr[0]+numb];
		//PRINTF("s: %u",sizeof(first_layer));
		return FIRST_LAYER_SIZE;
	}
	else if (ekranas.colum_ptr==1)
	{
	  //return (char *)second_layer[ekranas.line_ptr[0]][ekranas.line_ptr[1]+numb];
		if (second_layer[ekranas.line_ptr[0]][ekranas.line_ptr[1]+1]!=NULL)
		{
		  return 0xFF; //grazinam max 
		}
		else
		{
		  return ekranas.line_ptr[1];
		}
		
	}
	else 
	{
	  return 0;
	}

}

void ekranas_press_ok(void)
{
	if(ekranas.colum_ptr)
	{
		 if(ekranas.line_ptr[ekranas.colum_ptr]==0)
		 {
			 //handlinam grizimus
			       //compasas                    //kalibravimai
			 if (ekranas.line_ptr[1]==1 && ekranas.line_ptr[0]==3)
			 {
				 //jeigu klaibravimas nuimam klaibravimo flag
			    gps_compass_reset_calibrat();
			 }
			 //ofs kalibravimas
			 if (ekranas.line_ptr[1]==2 && ekranas.line_ptr[0]==3)
			 {
				 //jeigu klaibravimas nuimam klaibravimo flag
			    ofs_reset_save_calib();
			 }
			 
			 
			 
			 //ekrano valdimas
		   ekranas.colum_ptr--;
			 if (ekranas.colum_ptr==2)
			 {
			   ekranas.status=EKRANAS_ST_VYGDOMA;
			 }
			 else
			 {
			   ekranas.status=EKRANAS_ST_MENIU;
			 }
		 }
		 else
		 {
			 ekranas.colum_ptr++;
			 if (ekranas.colum_ptr==2)
			 {
			   ekranas.status=EKRANAS_ST_VYGDOMA;
			 }
			 else
			 {
			   ekranas.status=EKRANAS_ST_MENIU;
			 }
		 
		 }
	   
	}
	else
	{
	   ekranas.colum_ptr++;
		   if (ekranas.colum_ptr==3)
			 {
			   ekranas.status=EKRANAS_ST_VYGDOMA;
			 }
			 else
			 {
			   ekranas.status=EKRANAS_ST_MENIU;
			 }
	}
	

}

void ekranas_press_up(void)
{
	if (ekranas.line_ptr[ekranas.colum_ptr])
	{
	  ekranas.line_ptr[ekranas.colum_ptr]--;
	}
	//kitu parametru valdimas //ofs kalibravimas ivedimas
	if (ekranas.line_ptr[1]==2 && ekranas.line_ptr[0]==3)
  {
			ofs_calib_mm_up();
  }
	

}

void ekranas_press_down(void)
{
	if (ekranas.line_ptr[ekranas.colum_ptr]<(ekranas_get_cur_size()-1))
	{
	  ekranas.line_ptr[ekranas.colum_ptr]++;
	}
	
	//kitu parametru valdimas //ofs kalibravimas ivedimas
	if (ekranas.line_ptr[1]==2 && ekranas.line_ptr[0]==3)
  {
			ofs_calib_mm_down();
  }

}


void ekranas_set_status(uint8_t statt)
{
	if (ekranas.status!=statt)
	{
	   ekranas.status=statt;
	}

}


void ekranas_hold_up(void)
{
	if (ekranas.line_ptr[1]==2 && ekranas.line_ptr[0]==3)
  {
			ofs_calib_mm_up();
  }


}

void ekranas_hold_down(void)
{
  if (ekranas.line_ptr[1]==2 && ekranas.line_ptr[0]==3)
  {
			ofs_calib_mm_down();
  }
}
