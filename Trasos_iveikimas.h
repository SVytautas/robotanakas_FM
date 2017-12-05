#include <stdint.h>
#include <stdbool.h>
#define IKI_LINIJOS_MIN 19000//25000 // 10000+10000 //ivedame 
#define IKI_LINIJOS_MAX 25000 // 10000+10000 //ivedame 
#define IKI_PAIESKOS_MIN 300000 //iki vydurio 132k //truko apie +30000
#define IKI_PAIESKOS_MAX 370000

//iki namu linijos
#define IKI_NAMULINIJOS_MIN 3000//25000 // 10000+10000 //ivedame 
#define IKI_NAMULINIJOS_MAX 20000//25000 // 10000+10000 //ivedame 


#define ATSITRAUKIMAS_NUO_SONO 800

#define IKI_LINIJOS_ATS 400 //buvo
#define IKI_LINIJOS_ATS_MIN 60 //buvo

#define COUNTER_KAIRE_WRONG 2
#define COUNTER_DESINE_WRONG 2

#define KLIUTIS 300 //900
#define KLIUTIS_ULTRA 250 //900
#define KLIUTIS_MIN 50 //5cm

//#define LIDAR_REZ 500 //50cm

#define VAZIUOTI_KLIUTIS 300 //30cm
#define VAZIUOTI_TIESIAI 500 //30cm

#define KAMPAS_IKI_LINIJOS 220 //pakeisti
#define KAMPAS_IKI_PAIESKOS 315
#define KAMPAS_IKI_NAMU_LINIJOS 220
#define KAMPAS_IKI_NAMU 145
#define KAMPAS_IKI_FINISAS 47

#define ATSTUMAS_ATSITRAUKTI 100 //10cm

#define PAIESKA_IKI_PRADINIS_VERTIKLUS 12000
#define PAIESKA_IKI_PRADINIS_HORIZONTALUS 2500
#define PAIESKA_MINUS 500 //1m
#define PAIESKA_COUNTER 2 //1m

#define KAMPAS0 24
#define KAMPAS1 126
#define KAMPAS2 240
#define KAMPAS3 126

#define MAISELIS 3000
#define ATSTUMAS_MAISELIS 200 //20cm
#define VAZUOTI_PAIESKA 1500 //2m

typedef struct
{
	uint8_t status;
	uint16_t counter_desine;
	uint16_t counter_kaire;
	uint32_t delay;
	bool   flag_suktis180;
	uint32_t atstumas_mm;
	float  X_mm;
	float  Y_mm;
	float    last_kampas_ivygd;
	bool   start_prog;
	bool   iveikinejama_kliutis;
	bool   btn_presed;
}ti_t;


typedef struct
{
	uint8_t status;
	float atstumas_hor;
	float atstumas_vert;
	uint16_t counter_paieska;
}tipaieska_t;


enum {
 ST_IKI_LINIJOS=0,
 ST_IKI_PAIESKOS_LAUKO,
 ST_PAIESKOS_LAUKAS,
 ST_PAIMTI_MAISELI,
 ST_WAIT_KOL_PAKELS,
 ST_IKI_LINIJOS_NAMU,
 ST_IKI_NAMU_LAUKO,
 ST_IKI_FINISO,
 ST_STOP,
 ST_NONE,
};


void Trasos_iveikimas_handler(void);
void Trasos_iveikimas_tick_1s(void);
void Trasos_iveikimas_reset(void);
bool Trasos_iveikimas_kliutys(float kampas);
void Trasos_iveikimas_btn_reset(void);
void trasos_iveikimas_start_iki_linijos(void);

void Ti_print_nokia(void);
void trasos_iveikias_Ymm(float pliusmm);
void trasos_iveikimas_paieska(void);