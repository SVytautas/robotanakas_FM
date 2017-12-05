#include <stdint.h>
#include <stdbool.h>
//timer definitions ir DMA-----------------------------------------
//devifitions //steper 1 ir 2 portai ant tim2 ir tim3
#define TIM_2                              TIM2   //steperis1
#define TIM_3                              TIM3   //steperis1

/* Definition for TIMx clock resources */
#define TIM_2_CLK_ENABLE                  __HAL_RCC_TIM2_CLK_ENABLE
#define TIM_3_CLK_ENABLE                  __HAL_RCC_TIM3_CLK_ENABLE
//#define DMA_TIM_2ir3_CLK_ENABLE           __HAL_RCC_DMA1_CLK_ENABLE();

/* Definition for TIMx Pins */
#define TIM_2_CHANNEL3_GPIO_CLK_ENABLE    __HAL_RCC_GPIOA_CLK_ENABLE //tik chanel 3 ir chanel 4 update turi
#define TIM_3_CHANNEL4_GPIO_CLK_ENABLE    __HAL_RCC_GPIOB_CLK_ENABLE

//pinas stm32f4 GPIOA2 timer 2
//Steper pinai PA2
#define TIM_2_GPIO_CHANNEL1_PORT          GPIOA   
#define TIM_2_GPIO_PIN_CHANNEL1           GPIO_PIN_2
//Steper pinas PB1
//pinas stm32f4 GPIOB1
#define TIM_3_GPIO_CHANNEL4_PORT          GPIOB   
#define TIM_3_GPIO_PIN_CHANNEL4           GPIO_PIN_1


#define GPIO_AF_TIM_2                     GPIO_AF1_TIM2
//GPIO_AF2_TIM3
#define GPIO_AF_TIM_3                     GPIO_AF2_TIM3


/* Definition for TIMx's DMA */
//#define DMA_CHANNEL_TIM_2                  DMA_CHANNEL_3
//#define TIM_2_DMA_STREAM              		 DMA1_Stream1

//#define DMA_CHANNEL_TIM_3                  DMA_CHANNEL_5
//#define TIM_3_DMA_STREAM              		 DMA1_Stream2

/* Interuptas tim2 dma */
//#define TIM_2_DMA_IRQn                   DMA1_Stream1_IRQn
//#define TIM_2_DMA_IRQHandler             DMA1_Stream1_IRQHandler
/* interuptas tim3 dma */
//#define TIM_3_DMA_IRQn                   DMA1_Stream2_IRQn
//#define TIM_3_DMA_IRQHandler             DMA1_Stream2_IRQHandler
//timer-------------------------------------------------------------



//----------------steper valdikliu rezoliucijos nustatimas
//  |	MS1	| MS2	| resoliucija	  |
//	|	 L	|	 L	| FULL step			|
//	|	 H	|	 L	| HALF step			|
//	|	 L	|	 H	| QUATER step		|
//	|	 H	|	 H	| EIGHT step		|
//   MS1 ir MS2 pinai ant stm plokstes A3977 Right variklis TIM2
#define GPIOD_CLK_ENABLE      __HAL_RCC_GPIOD_CLK_ENABLE //tik chanel 3 ir chanel 4 update turi
#define GPIOB_CLK_ENABLE      __HAL_RCC_GPIOB_CLK_ENABLE
//pinas stm32f4 GPIOA2 timer 2
//1steperis
#define R_VARIKLIS_MS1_GPIO_PORT          GPIOD   
#define R_VARIKLIS_MS1_GPIO_PIN           GPIO_PIN_7
//pinas stm32f4 GPIOB1
#define R_VARIKLIS_MS2_GPIO_PORT          GPIOD   
#define R_VARIKLIS_MS2_GPIO_PIN           GPIO_PIN_5

#define R_VARIKLIS_MS3_GPIO_PORT          GPIOD   
#define R_VARIKLIS_MS3_GPIO_PIN           GPIO_PIN_3

//2steperis
#define L_VARIKLIS_MS1_GPIO_PORT          GPIOD   
#define L_VARIKLIS_MS1_GPIO_PIN           GPIO_PIN_4
//pinas stm32f4 GPIOB1
#define L_VARIKLIS_MS2_GPIO_PORT          GPIOD   
#define L_VARIKLIS_MS2_GPIO_PIN           GPIO_PIN_2

#define L_VARIKLIS_MS3_GPIO_PORT          GPIOD   
#define L_VARIKLIS_MS3_GPIO_PIN           GPIO_PIN_6

//
enum {
SET_ZV_FULL_STEP=0,
SET_ZV_HALF_STEP,	
SET_ZV_QUATER_STEP,
SET_ZV_EIGHT_STEP,
SET_ZV_SIXTEENTH_STEP
};

#define ZV_GREITIS_MIN 700 //minimalus gretis nuo kurio didinam 
#define ZV_GREITIS_VAZIAVIMAS 400 //minimalus gretis nuo kurio didinam
//#define ZV_GREITIS_SUKIMAS_MIN 1000
#define ZV_GREITIS_SUKIMAS 1400 //mazas sukimo greitis //sukimas mazu greiciu
#define ZV_GREITIS_STEP 10


//sukimo nusttimai
#define ZV_SUKTI_TRIK_MIN 3//sukam kol kampas sumazeja iki 1laipsnio
#define KAMPAS_SUKTI_ERROR 15 // +-5laipsniai neatitikimas ir vygdom sukima
#define ZV_SUKTI_TRIK_MIN_SPEED 10//kai lieka 10 laipsniu mazinam greiti
#define ZV_SUKTI_STOP_TIME 3//30ms//sukam kol kampas sumazeja iki 1laipsnio
#define ZV_INITIAL_TURN_TIME 60//600ms//60ms//sukam kol kampas sumazeja iki 1laipsnio

typedef struct
{
	uint8_t status;
	uint16_t greitis_esamas;
	uint16_t greitis_to;
	int16_t greitisR;
	int16_t greitisL;
	uint8_t  dir;
	uint16_t time;
	uint16_t stop_turn_time;
	uint16_t initial_turn_time;
	float heading_to;
	float heading_is;
	float astumas_to_mm;
	float atstumas_is_mm;
	float trik_kampas;
	float heading_is_other;
	bool  test_on;
	
	//bool  test_on_tiesiai;
}zv_t;


#define ZV_TASK_SIZE 256

#define ZV_TASK_DEAD_TIME 20 //200ms
#define ZV_TASK_DEAD_TIME_SUKIMAS 800 //8s //nevygdom kliuciu paiesko atnaujinmo mapp
#define ROBOTO_GREITIS_TIESIAI 230 //240mm/s

typedef struct
{
	float kampas_to[ZV_TASK_SIZE];
	float atstumas[ZV_TASK_SIZE];
	float kampas_dstar[ZV_TASK_SIZE];
	uint8_t task_wp;
	uint8_t task_rp;
	bool    next_task;
	uint32_t zv_task_time;
	bool   finished;
	float  last_kampas;
	uint16_t timeout_atstumas;
}zv_task_t;


enum{
ZV_ST_INIT=0,
ZV_ST_NONE,
ZV_ST_STOP,
ZV_ST_FORWARD,
ZV_ST_BACK,
ZV_ST_TURN,
ZV_ST_TURN_RIGHT,
ZV_ST_TURN_LEFT,
ZV_ST_FORWARD_OFS,
ZV_ST_FORWARD_OFS_NOSTOP,
ZV_ST_BACKWARD_OFS,
	
};
//
//---------------------------------------------------------------------------


//---------------steper ENABLE pinai------------------------------------------
//1steperis
#define R_VARIKLIS_ENABLE_PORT	          GPIOB   
#define R_VARIKLIS_ENABLE_PIN             GPIO_PIN_4  // dar reikies pataisyti kad nesikirstu su periferija
//2steperis
#define L_VARIKLIS_ENABLE_PORT	          GPIOD   
#define L_VARIKLIS_ENABLE_PIN             GPIO_PIN_0
//-----------------------------------------------------------------------------

//1steperis
//---------------Steper DIR pinai-----------------------------------------------
#define R_VARIKLIS_DIR_PORT	           GPIOD   
#define R_VARIKLIS_DIR_PIN             GPIO_PIN_1  // dar reikies pataisyti kad nesikirstu su periferija
//2steperis
#define L_VARIKLIS_DIR_PORT	           GPIOB   
#define L_VARIKLIS_DIR_PIN             GPIO_PIN_3
//----------------steper Dir end


//nustatimai greitejimo letejimo parametras
#define LETEJIMAS_GREITEJIMAS_PROC 20


//timer rezoliucija
//rezoliucija= 84Mhz / divider.
//nustatome 250ns rezoliucija
#define TIMER_REZOLIUCIJA 20

//DMA interuptas ms
//#define DMA_INTERUPT_TIME 100
//#define VARIKLIO_DID_GREITIS_MS 500 // variklio didziausiais greitis 0.6ms
//#define VARIKLIO_DID_GREITIS_MS 4000 // variklio didziausiais greitis 0.6ms
//variklio didziausias greitis 0.2ms
//625Hz
//#define b_atstumas 282//370 //mm 168 //282 kai sustojes
//#define ZINGSNIU_MM 41;
//#define ZINGSNIU_MM_MAZ 39;
//#define SUKIMO_GREITIS 120;
//#define VAZIAVIMO_GREITIS 250;


//funkcijos
void update_tim2_period(uint16_t period);
void update_tim3_period(uint16_t period);

void zv_init(void);
void zv_gpio_init(void);
void ZV_change_step_rez(uint8_t res_status);

void zv_enable(void);
void zv_disable(void);

//funkcija steper L variklio poslinkis R variklio poslinkis
void set_R_dir_For(void);
void set_R_dir_Rev(void);
void set_L_dir_For(void);
void set_L_dir_Rev(void);


void zv_handler(void);
void zv_set_forward(uint16_t time10ms);
void zv_set_turn(uint16_t time10ms);

float zv_GetHeadingError(float initial, float final);
void zv_forward(float mm);

//zv task vygdymas
bool zv_kampas(float kampas_to); //jei reikia suktis grazinam true
void zv_task_handler(void); //while ciklas


//sukimo tesetia
void test_turn_right(void);
void test_turn_left(void);
//teisiai test

void test_zv_forfard(void);

void test_reset(void);
void zv_task_push(float kampas_to, float atstumas, float dstar_kampas);
void zv_task_push2(float kampas_to, float atstumas);
void zv_task_clear_stop(void);

void zv_tick_10hz(void);
bool get_zv_dead_time(void);
bool zv_task_finished(void);
float zv_get_last_kampas_dstar(void);