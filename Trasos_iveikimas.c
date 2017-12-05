#include "main.h"
#include "Lidar.h"
#include "stm32f4xx_it.h"
#include "debuguart.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "Trasos_iveikimas.h"
#include "zingsniniai_varikliai.h"
#include "Maiselio_ieskojimas.h"
#include "Griebtuvas.h"
#include "dstar.h"
#include "gps_compass.h"
#include "btn_bumper.h"
#include "Ultragarsas.h"
#include "PD8544.h"
#include "string.h"
#include "COLOR.h"
#include "papildomi.h"
#include "servo.h"



tipaieska_t ti_paieska={
   .status=0,
	 .atstumas_hor=0,
	 .atstumas_vert=0,
	 .counter_paieska=0,
};

uint32_t X_glob_trasa=10000, Y_glob_trasa=10000; 

ti_t ti={
 .status=ST_NONE,
 .counter_desine=0,
 .counter_kaire=0,
 .delay=0,
 .flag_suktis180=false,
 .iveikinejama_kliutis=false,
 .btn_presed=false,
 .Y_mm=0,
 .X_mm=0,
};

void trasos_iveikias_Ymm(float pliusmm)
{
	//skaiciuojam ka matuoja Ymm
	ti.Y_mm+=pliusmm;
	ti_paieska.atstumas_vert+=pliusmm;;
	ti_paieska.atstumas_hor+=pliusmm;;

}

char print_nok[20];

void Trasos_iveikimas_handler(void)
{

	 // uint16_t kampas_lidar;
		switch (ti.status)
		{
			case ST_IKI_LINIJOS:
			//STATUSAS VAZIUOTI IKI LINIJOS	
				if ((ti.Y_mm>=(IKI_LINIJOS_MAX))
					|| (ti.Y_mm>=IKI_LINIJOS_MIN &&  get_minumas_kaire_priekis()<=IKI_LINIJOS_ATS && get_minumas_kaire_priekis()>=IKI_LINIJOS_ATS_MIN)
				  || (ti.Y_mm>=IKI_LINIJOS_MIN &&  get_minumas_desine_priekis()<=IKI_LINIJOS_ATS && get_minumas_desine_priekis()>=IKI_LINIJOS_ATS_MIN)
					|| (ti.Y_mm>=IKI_LINIJOS_MIN &&  ultragarsas_get_kairys()<=IKI_LINIJOS_ATS && ultragarsas_get_kairys()>=IKI_LINIJOS_ATS_MIN)
				  || (ti.Y_mm>=IKI_LINIJOS_MIN &&  ultragarsas_get_desinys()<=IKI_LINIJOS_ATS && ultragarsas_get_desinys()>=IKI_LINIJOS_ATS_MIN)
				)
				{
					ti.status=ST_IKI_PAIESKOS_LAUKO;
					pap_beep_mult(255,5);
					ti.Y_mm=0;
					//resetinam Zemelapio parametrus
					//pasisukam
					//zv_task_push2(KAMPAS_IKI_PAIESKOS, VAZIUOTI_TIESIAI);
					zv_task_push2(KAMPAS_IKI_LINIJOS, -1500);
				}
				else
				{ 
					//papildomi isvengimai vazuojant
					if(Trasos_iveikimas_kliutys(KAMPAS_IKI_LINIJOS))
					{
					    memset(&print_nok,NULL,sizeof(print_nok));
							sprintf(print_nok,"kliutis");
					}
					else
					{
						if (zv_task_finished())
					  {
							zv_task_push2(KAMPAS_IKI_LINIJOS, VAZIUOTI_TIESIAI);
							PRINTF("tiesiai\r\n");
							memset(&print_nok,NULL,sizeof(print_nok));
							sprintf(print_nok,"Tiesiai");
						}
					
					}
					
				}	
			break;
			
			case ST_IKI_PAIESKOS_LAUKO:
			  if ((ti.Y_mm>=(IKI_PAIESKOS_MAX))
					|| (ti.Y_mm>=IKI_PAIESKOS_MIN &&  get_minumas_kaire_priekis()<=IKI_LINIJOS_ATS && get_minumas_kaire_priekis()>=IKI_LINIJOS_ATS_MIN)
				  || (ti.Y_mm>=IKI_PAIESKOS_MIN &&  get_minumas_desine_priekis()<=IKI_LINIJOS_ATS && get_minumas_desine_priekis()>=IKI_LINIJOS_ATS_MIN)
					|| (ti.Y_mm>=IKI_PAIESKOS_MIN &&  ultragarsas_get_kairys()<=IKI_LINIJOS_ATS && ultragarsas_get_kairys()>=IKI_LINIJOS_ATS_MIN)
				  || (ti.Y_mm>=IKI_PAIESKOS_MIN &&  ultragarsas_get_desinys()<=IKI_LINIJOS_ATS && ultragarsas_get_desinys()>=IKI_LINIJOS_ATS_MIN)
				)
				{
					ti.status=ST_PAIESKOS_LAUKAS;
					color_reset_min_max();
					ti.Y_mm=0;
					ti_paieska.atstumas_vert=0;
					ti_paieska.atstumas_hor=0;
					pap_beep_mult(255,6);
					zv_task_push2(KAMPAS0, 1100);
					zv_task_push2(KAMPAS_IKI_PAIESKOS, 100);
					zv_task_push2(KAMPAS_IKI_PAIESKOS, -2000);
					//resetinam Zemelapio parametrus
					//pasisukam
					//zv_task_push2(KAMPAS_IKI_PAIESKOS, VAZIUOTI_TIESIAI);
					//atgal 2metrus
				}
				else
				{ 
					//papildomi isvengimai vazuojant
					if(Trasos_iveikimas_kliutys(KAMPAS_IKI_PAIESKOS))
					{
					    memset(&print_nok,NULL,sizeof(print_nok));
							sprintf(print_nok,"kliutis");
					}
					else
					{
						if (zv_task_finished())
					  {
							zv_task_push2(KAMPAS_IKI_PAIESKOS, VAZIUOTI_TIESIAI);
							PRINTF("tiesiai\r\n");
							memset(&print_nok,NULL,sizeof(print_nok));
							sprintf(print_nok,"Tiesiai");
						}
					
					}
					
				}	
			break;
					
		  case ST_PAIESKOS_LAUKAS:
				//pasitikrinam ar ne zalia
					if (color_get_max_min()>MAISELIS) //iesko kol randam
					{
						if (ultragarsas_get_centras()<ATSTUMAS_MAISELIS)
						{
						  //setinam paemimo statusa
						  ti.status=ST_PAIMTI_MAISELI;
							pap_beep_mult(255,10);
						  //Trasos_iveikimas_handler();
						  break;
						}
						else
						{
						  color_reset_min_max();
						}
						
					
					}
				switch (ti_paieska.status)
				{
					case 0:
					//vertikalus
					if (ti_paieska.atstumas_vert>=PAIESKA_IKI_PRADINIS_VERTIKLUS && ti_paieska.counter_paieska==0)
					{
					  ti_paieska.status=1;
						ti_paieska.counter_paieska++;
						if (ti_paieska.counter_paieska>PAIESKA_COUNTER)
						{
						  //neradom stabdom ir bandom vel
							ti.status=ST_STOP;
						}
						ti_paieska.atstumas_vert=0;
						ti_paieska.atstumas_hor=0;
						color_reset_min_max();
						
				  }
					else if (ti_paieska.atstumas_vert>=(PAIESKA_IKI_PRADINIS_VERTIKLUS-2000))
					{
						ti_paieska.status=1;
						ti_paieska.counter_paieska++;
						if (ti_paieska.counter_paieska>PAIESKA_COUNTER)
						{
						  //neradom stabdom ir bandom vel
							ti.status=ST_STOP; //vaziuojam atgal
							pap_beep_mult(255,20);
						}
						ti_paieska.atstumas_vert=0;
						ti_paieska.atstumas_hor=0;
						color_reset_min_max();
					
					}
					else
					{
						if (zv_task_finished())
					  {
					  zv_task_push2(KAMPAS0, VAZIUOTI_TIESIAI);}
					}
					break;
					
					case 1:
					if (ti_paieska.atstumas_hor>=PAIESKA_MINUS)
					{
					  ti_paieska.status=2;
						ti_paieska.atstumas_vert=0;
						ti_paieska.atstumas_hor=0;
						color_reset_min_max();
					}
					else
					{
						if (zv_task_finished())
					  {
					  zv_task_push2(KAMPAS1, VAZIUOTI_TIESIAI);}
					}
					break;
					
					case 2:
					if (ti_paieska.atstumas_vert>=(PAIESKA_IKI_PRADINIS_VERTIKLUS-2000))
					{
					  ti_paieska.status=3;
						ti_paieska.atstumas_vert=0;
						ti_paieska.atstumas_hor=0;
						color_reset_min_max();
				  }
					else
					{
						if (zv_task_finished())
					  {
					  zv_task_push2(KAMPAS2, VAZIUOTI_TIESIAI);}
					}
					break;
					
					case 3:
					if (ti_paieska.atstumas_hor>=PAIESKA_MINUS)
					{
					  ti_paieska.status=0;
						ti_paieska.atstumas_vert=0;
						ti_paieska.atstumas_hor=0;
						color_reset_min_max();
					}
					else
					{
						if (zv_task_finished())
					  {
					  zv_task_push2(KAMPAS3, VAZIUOTI_TIESIAI);}
					}	
					break;
					
					default:
					break;
				
				}					
			//	if (zv_task_finished())
				//{
				//			zv_task_push2(KAMPAS_IKI_LINIJOS, VAZIUOTI_TIESIAI);
				//}					
					
				
				//get_minumas_kaire_priekis();
				//get_minumas_desine_priekis();
				
			break;
					
					
			case ST_PAIMTI_MAISELI:
			//imam maiseli
			//pasisukti 180 laipsniu vygdydi paemima
			if (ti.flag_suktis180)
			{
				//nuleidziam giebtuva sukames 180 vaziuojam 1,5m pristumam maiseli
				servo_nuleisti_griebtuva();
				uint16_t degpaimti= get_gps_compass_headingdeg()+180;
				if (degpaimti>=360) {degpaimti-=360;}
				zv_task_push2(degpaimti, 200);
				zv_task_push2(degpaimti, -1500);
			
			}
			else
			{
		    //set_status(ST_STOP_ALL);	
			 // set_status_griebtuvas(ST_GRIEBTUVAS_PAKELTI);
				if (zv_task_finished())
			  {
					servo_pakelti_griebtuva();
			    ti.status=ST_WAIT_KOL_PAKELS;
				  ti.delay=5;
				}
				//Trasos_iveikimas_handler();
			}
			
			break;
			
			
			case ST_WAIT_KOL_PAKELS:
				if (!ti.delay) //laukiam kol pakels
				{
					
					ti.status=ST_IKI_LINIJOS_NAMU;
					ti.Y_mm=0;
					ti_paieska.atstumas_vert=0;
					ti_paieska.atstumas_hor=0;
					pap_beep_mult(255,6);
			 }
				
			break;
				
			case ST_IKI_LINIJOS_NAMU:
				
				  if ((ti.Y_mm>=(IKI_NAMULINIJOS_MAX))
					//|| (ti.Y_mm>=IKI_LINIJOS_MIN &&  get_minumas_kaire_priekis()<=IKI_LINIJOS_ATS && get_minumas_kaire_priekis()>=IKI_LINIJOS_ATS_MIN)
				  //|| (ti.Y_mm>=IKI_LINIJOS_MIN &&  get_minumas_desine_priekis()<=IKI_LINIJOS_ATS && get_minumas_desine_priekis()>=IKI_LINIJOS_ATS_MIN)
					|| (ti.Y_mm>=IKI_NAMULINIJOS_MIN &&  ultragarsas_get_kairys()<=IKI_LINIJOS_ATS && ultragarsas_get_kairys()>=IKI_LINIJOS_ATS_MIN)
				  || (ti.Y_mm>=IKI_NAMULINIJOS_MIN &&  ultragarsas_get_desinys()<=IKI_LINIJOS_ATS && ultragarsas_get_desinys()>=IKI_LINIJOS_ATS_MIN)
				)
				{
					ti.status=ST_IKI_NAMU_LAUKO;
					pap_beep_mult(255,5);
					//resetinam Zemelapio parametrus
					//pasisukam
					zv_task_push2(KAMPAS_IKI_NAMU_LINIJOS, -1500);
				}
				else
				{ 
					//papildomi isvengimai vazuojant
					if(Trasos_iveikimas_kliutys(KAMPAS_IKI_NAMU_LINIJOS))
					{
					    memset(&print_nok,NULL,sizeof(print_nok));
							sprintf(print_nok,"kliutis");
					}
					else
					{
						if (zv_task_finished())
					  {
							zv_task_push2(KAMPAS_IKI_NAMU_LINIJOS, VAZIUOTI_TIESIAI);
							PRINTF("tiesiai\r\n");
							memset(&print_nok,NULL,sizeof(print_nok));
							sprintf(print_nok,"Tiesiai");
						}
					
					}
					
				}	
			break;
					
			case ST_IKI_NAMU_LAUKO:
			  if ((ti.Y_mm>=(IKI_PAIESKOS_MAX))
					|| (ti.Y_mm>=IKI_PAIESKOS_MIN &&  get_minumas_kaire_priekis()<=IKI_LINIJOS_ATS && get_minumas_kaire_priekis()>=IKI_LINIJOS_ATS_MIN)
				  || (ti.Y_mm>=IKI_PAIESKOS_MIN &&  get_minumas_desine_priekis()<=IKI_LINIJOS_ATS && get_minumas_desine_priekis()>=IKI_LINIJOS_ATS_MIN)
					|| (ti.Y_mm>=IKI_PAIESKOS_MIN &&  ultragarsas_get_kairys()<=IKI_LINIJOS_ATS && ultragarsas_get_kairys()>=IKI_LINIJOS_ATS_MIN)
				  || (ti.Y_mm>=IKI_PAIESKOS_MIN &&  ultragarsas_get_desinys()<=IKI_LINIJOS_ATS && ultragarsas_get_desinys()>=IKI_LINIJOS_ATS_MIN)
				)
				{
					ti.status=ST_IKI_FINISO;
					//color_reset_min_max();
					ti.Y_mm=0;
					//ti_paieska.atstumas_vert=0;
					//ti_paieska.atstumas_hor=0;
					pap_beep_mult(255,6);
					zv_task_push2(KAMPAS_IKI_NAMU, -1500);
					//resetinam Zemelapio parametrus
					//pasisukam
					//zv_task_push2(KAMPAS_IKI_PAIESKOS, VAZIUOTI_TIESIAI);
					//atgal 2metrus
				}
				else
				{ 
					//papildomi isvengimai vazuojant
					if(Trasos_iveikimas_kliutys(KAMPAS_IKI_NAMU))
					{
					    memset(&print_nok,NULL,sizeof(print_nok));
							sprintf(print_nok,"kliutis");
					}
					else
					{
						if (zv_task_finished())
					  {
							zv_task_push2(KAMPAS_IKI_NAMU, VAZIUOTI_TIESIAI);
							PRINTF("tiesiai\r\n");
							memset(&print_nok,NULL,sizeof(print_nok));
							sprintf(print_nok,"Tiesiai");
						}
					
					}
					
				}	
			break;
					
			case ST_IKI_FINISO:
				if ((ti.Y_mm>=(IKI_LINIJOS_MAX))
					|| (ti.Y_mm>=IKI_LINIJOS_MIN &&  get_minumas_kaire_priekis()<=IKI_LINIJOS_ATS && get_minumas_kaire_priekis()>=IKI_LINIJOS_ATS_MIN)
				  || (ti.Y_mm>=IKI_LINIJOS_MIN &&  get_minumas_desine_priekis()<=IKI_LINIJOS_ATS && get_minumas_desine_priekis()>=IKI_LINIJOS_ATS_MIN)
					|| (ti.Y_mm>=IKI_LINIJOS_MIN &&  ultragarsas_get_kairys()<=IKI_LINIJOS_ATS && ultragarsas_get_kairys()>=IKI_LINIJOS_ATS_MIN)
				  || (ti.Y_mm>=IKI_LINIJOS_MIN &&  ultragarsas_get_desinys()<=IKI_LINIJOS_ATS && ultragarsas_get_desinys()>=IKI_LINIJOS_ATS_MIN)
				)
				{
					ti.status=ST_STOP;
					pap_beep_mult(255,20);
					ti.Y_mm=0;
					//resetinam Zemelapio parametrus
					//pasisukam
					//zv_task_push2(KAMPAS_IKI_FINISAS, VAZIUOTI_TIESIAI);
				}
				else
				{ 
					//papildomi isvengimai vazuojant
					if(Trasos_iveikimas_kliutys(KAMPAS_IKI_FINISAS))
					{
					    memset(&print_nok,NULL,sizeof(print_nok));
							sprintf(print_nok,"kliutis");
					}
					else
					{
						if (zv_task_finished())
					  {
							zv_task_push2(KAMPAS_IKI_FINISAS, VAZIUOTI_TIESIAI);
							PRINTF("tiesiai\r\n");
							memset(&print_nok,NULL,sizeof(print_nok));
							sprintf(print_nok,"Tiesiai");
						}
					
					}
					
				}	
			
			break;
			
			case ST_STOP:
	//		set_status(ST_STOP_ALL);		
			break;
			
			
			
			default:
			break;
		
		
		
		
		}

}


void Trasos_iveikimas_reset(void)
{
   ti.iveikinejama_kliutis=false;
}

void Trasos_iveikimas_btn_reset(void)
{
   ti.btn_presed=false;
}

bool Trasos_iveikimas_kliutys(float kampas)
{
	if (btn_bump_kairys())
	{
		
		//stabdom atgal ir 45 300mm
		     if(!ti.btn_presed)
				 {
					 ti.btn_presed=true;
					 zv_task_clear_stop();
		     //atgal 
				 //dedam iveikti kliuti
		     zv_task_push2(kampas, -VAZIUOTI_KLIUTIS);
				 ti.iveikinejama_kliutis=true;
				 uint16_t kampass=kampas+90;
				 if (kampass>=360){kampass=kampass-360;}
				 
				 zv_task_push2(kampass, VAZIUOTI_KLIUTIS);
				 PRINTF("kairys btn\r\n");
				 }
				 return true;
		     
	
	}
	else if (btn_bump_desinys())
	{
		
		//stabdom atgal ir 45 300mm
		     if(!ti.btn_presed)
				 {
					 ti.btn_presed=true;
		     zv_task_clear_stop();
		     //atgal 
				 //dedam iveikti kliuti
		     zv_task_push2(kampas, -VAZIUOTI_KLIUTIS);
				 ti.iveikinejama_kliutis=true;
				 uint16_t kampass=kampas+270;
				 if (kampass>=360){kampass=kampass-360;}
				 
				 zv_task_push2(kampass, VAZIUOTI_KLIUTIS);
				 PRINTF("desinys btn\r\n");
				 }
				 return true;
	
	}
	else if (btn_bump_centras())
	{
		
		//stabdom atgal ir 45 300mm
					if(!ti.btn_presed)
				 {
					 ti.btn_presed=true;
		     zv_task_clear_stop();
		     //atgal 
				 //dedam iveikti kliuti
		     zv_task_push2(kampas, -VAZIUOTI_KLIUTIS);
				 ti.iveikinejama_kliutis=true;
				 uint16_t kampass=kampas+270;
				 if (kampass>=360){kampass=kampass-360;}
				 
				 zv_task_push2(kampass, VAZIUOTI_KLIUTIS);
				 PRINTF("centras btn\r\n");
			  }
				 return true;
	
	}
	else if(ultragarsas_get_kairys()<ATSTUMAS_ATSITRAUKTI)
	{
	       //atsitraukiam
		     if(!ti.btn_presed)
				 {
					 ti.btn_presed=true;
					 zv_task_clear_stop();
		     //atgal 
				 //dedam iveikti kliuti
		     zv_task_push2(kampas, -VAZIUOTI_KLIUTIS);
				 ti.iveikinejama_kliutis=true;
				 uint16_t kampass=kampas+90;
				 if (kampass>=360){kampass=kampass-360;}
				 
				 zv_task_push2(kampass, VAZIUOTI_KLIUTIS);
				 PRINTF("kairys ultra %u\r\n",ultragarsas_get_kairys());
				 }
				 return true;
	}
	else if(ultragarsas_get_desinys()<ATSTUMAS_ATSITRAUKTI)
	{
	       //atsitraukiam
		     if(!ti.btn_presed)
				 {
					 ti.btn_presed=true;
		     zv_task_clear_stop();
		     //atgal 
				 //dedam iveikti kliuti
		     zv_task_push2(kampas, -VAZIUOTI_KLIUTIS);
				 ti.iveikinejama_kliutis=true;
				 uint16_t kampass=kampas+270;
				 if (kampass>=360){kampass=kampass-360;}
				 
				 zv_task_push2(kampass, VAZIUOTI_KLIUTIS);
				// PRINTF("desinys ultra");
				 PRINTF("desinys ultra %u\r\n",ultragarsas_get_desinys());
				 }
				 return true;
	}
	else
	{
		//kitu atveju jei nera kliuciu vaziuojam normaliai
	
				if ((get_minumas_kaire_priekis()<=KLIUTIS && get_minumas_kaire_priekis()>=KLIUTIS_MIN)
				   || (ultragarsas_get_kairys()<=KLIUTIS_ULTRA && ultragarsas_get_kairys()>=KLIUTIS_MIN)  	
				)
				{
						//sukames i desine nuo nustatyto kampo 45 tiesiai 500
							if (!ti.iveikinejama_kliutis)
							{
								zv_task_clear_stop();
								//dedam iveikti kliuti
								ti.iveikinejama_kliutis=true;
								uint16_t kampass=kampas+60;
								if (kampass>=360){kampass=kampass-360;}
				 
								zv_task_push2(kampass, VAZIUOTI_KLIUTIS);
								PRINTF("kaire\r\n");
			   
							}
					
							return true;
				}
				else if ((get_minumas_desine_priekis()<=KLIUTIS && get_minumas_desine_priekis()>=KLIUTIS_MIN)
					      || (ultragarsas_get_desinys()<=KLIUTIS_ULTRA && ultragarsas_get_desinys()>=KLIUTIS_MIN) 
				)
				{
					//sukames i kaire ir tiesiai
					 // xxxx++;
						if (!ti.iveikinejama_kliutis)
						{
							zv_task_clear_stop();
							//dedam iveikti kliuti
							ti.iveikinejama_kliutis=true;
							uint16_t kampass=kampas+300;
							if (kampass>=360){kampass=kampass-360;}
				 
							zv_task_push2(kampass, VAZIUOTI_KLIUTIS);
							PRINTF("desine\r\n");
			   
						}
						return true;
				}
				else if (get_minumas_desine_sonas()<=ATSITRAUKIMAS_NUO_SONO && get_minumas_desine_sonas()>=KLIUTIS_MIN)
				{
						//atsitraukiam nuo sono i kaire
						//return true;
					  if (!ti.iveikinejama_kliutis) //atsiraukimas
						{
							zv_task_clear_stop();
							//dedam iveikti kliuti
							ti.iveikinejama_kliutis=true;
							uint16_t kampass=kampas+325; //35laipsniai
							if (kampass>=360){kampass=kampass-360;}
				 
							zv_task_push2(kampass, VAZIUOTI_KLIUTIS);
							PRINTF("desine atsitraukti\r\n");
			   
						}
						return false;	
				}
				else if (get_minumas_kaire_sonas()<=ATSITRAUKIMAS_NUO_SONO && get_minumas_kaire_sonas()>=KLIUTIS_MIN)
				{
					//atsitraukiam nuo sono i desine
					//return true;
					    if (!ti.iveikinejama_kliutis) 
							{
								zv_task_clear_stop();
								//dedam iveikti kliuti
								ti.iveikinejama_kliutis=true;
								uint16_t kampass=kampas+35;
								if (kampass>=360){kampass=kampass-360;}
				 
								zv_task_push2(kampass, VAZIUOTI_KLIUTIS);
								PRINTF("kaire atsiraukti\r\n");
			   
							}
						return false;			
				}
				else
				{
					//grazinam kad kliuties nera
					return false;	
         //Tada veikia vaziavimas tiesiai			
				}	
	
	
	}
	
	
	
	


}




void Trasos_iveikimas_tick_1s(void)
{
  if (ti.delay)
	{
		ti.delay--;
	
	}


}


void trasos_iveikimas_start_iki_linijos(void)
{
    if (!ti.start_prog)
		{
			//leidziam programa nuskaitom kampa ir dedam i dstart_pradini kampa
			//dstar_set_kampas_start(get_gps_compass_headingdeg());
			ti.status=ST_IKI_LINIJOS;
		  ti.start_prog=true;
		
		}
}		


void trasos_iveikimas_paieska(void)
{
    if (!ti.start_prog)
		{
			//leidziam programa nuskaitom kampa ir dedam i dstart_pradini kampa
			//dstar_set_kampas_start(get_gps_compass_headingdeg());
			ti.status=ST_PAIESKOS_LAUKAS;
		  ti.start_prog=true;
		
		}
}	
		
		
		
		
		
		
		
		
		
		
		//IVAIRUS BANDYMAI
		//kitu atveu vaziuojam tiesiai Y
				  //if (zv_task_finished())
					//{
						//perskaiciuojam mapa ir pushinam naujas komandas
						//if (get_zv_dead_time())
						//{
							//aina nahui tas dstar
							//dstar_reset_map();  
						 // lidar_to_XY_add_obstacle(5 , 6, 360-zv_get_last_kampas_dstar(), LIDAR_REZ);
						 // dstar_plan(5, 1, 5, 6);
							//dstar_plan_command(0);
							//dstar_printf_map();
	           // dstar_printf_path();
							//dstar_push_command_to_ZV();
						//}
					  
					//}



void Ti_print_nokia(void)
{

    PCD8544_Clear();
		//read_RGB()	
		PCD8544_GotoXY(0, 0);
	  NOKIA_PRINTF("Vygdoma");
	  PCD8544_GotoXY(0, 9);
		NOKIA_PRINTF("Y: %lu",(uint32_t)ti.Y_mm);
		PCD8544_GotoXY(0, 18);
	  NOKIA_PRINTF("%s",print_nok);
	//	PCD8544_GotoXY(0, 27);
	//  NOKIA_PRINTF("MinY: %i",(int16_t)compass.minY);
	//  PCD8544_GotoXY(0, 36);
	//  NOKIA_PRINTF("MaxY: %i",(int16_t)compass.maxY);
		PCD8544_Refresh();
  




}