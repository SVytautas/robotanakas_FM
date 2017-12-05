#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#define ULTRAGARSAS_JUTIKLIU_SK 3

//CE pin PE7
#define ULTRAGARSAS_TRIG_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOE_CLK_ENABLE() 
#define ULTRAGARSAS_TRIG1_PIN                     GPIO_PIN_8//GPIO_PIN_3
#define ULTRAGARSAS_TRIG2_PIN                     GPIO_PIN_10//GPIO_PIN_3
#define ULTRAGARSAS_TRIG3_PIN                     GPIO_PIN_12//GPIO_PIN_3
#define ULTRAGARSAS_TRIG4_PIN                     GPIO_PIN_15//GPIO_PIN_3
#define ULTRAGARSAS_TRIG_GPIO_PORTs               GPIOE //GPIOC


enum{
  ULTRAGARSAS_ST_INIT,
	ULTRAGARSAS_ST_TRIG,
	ULTRAGARSAS_ST_ECHO_GET,
	ULTRAGARSAS_ST_NIEKO,
	
};

enum 
{
	ULTRAGARSAS_JUTIKLIS1=0,
	ULTRAGARSAS_JUTIKLIS2,
	ULTRAGARSAS_JUTIKLIS3,
};


typedef struct
{
	uint8_t status;
	uint16_t input_capture_pos[ULTRAGARSAS_JUTIKLIU_SK];
	uint16_t input_capture_neg[ULTRAGARSAS_JUTIKLIU_SK];
	uint16_t atstumas[ULTRAGARSAS_JUTIKLIU_SK];
	uint16_t delay;
	uint32_t trig_pin[ULTRAGARSAS_JUTIKLIU_SK];
	uint8_t  jutiklis_ptr;
	uint32_t input_ch_direct[ULTRAGARSAS_JUTIKLIU_SK];
	uint32_t input_ch_indirect[ULTRAGARSAS_JUTIKLIU_SK];
}ultragarsas_t;

void Ultragarsas_init(void);
void Ultragarsas_GPIO_Init(void);
void Ultragarsas_handler(void);
void ultragarsas_print_nokia(void);
uint32_t ultragarsas_get_min_virsus_priekis(void);
uint32_t ultragarsas_get_desinys(void);
uint32_t ultragarsas_get_kairys(void);
uint32_t ultragarsas_get_centras(void);
