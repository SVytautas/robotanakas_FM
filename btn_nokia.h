#include <stdint.h>
#include <stdbool.h>

#define BTN_NOKIA_SK 4

#define BTNS_NOKIA_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOE_CLK_ENABLE() 
//BTN1 pin PE2
#define BTN1_NOKIA_PIN                     GPIO_PIN_2//GPIO_PIN_3
//BTN2 pin PE3
#define BTN2_NOKIA_PIN                     GPIO_PIN_3//GPIO_PIN_3
//BTN1 pin PE1
#define BTN3_NOKIA_PIN                     GPIO_PIN_1//GPIO_PIN_3
//BTN1 pin PE0
#define BTN4_NOKIA_PIN                     GPIO_PIN_0//GPIO_PIN_3
#define BTNS_NOKIA_GPIO_PORT               GPIOE //GPIOC


#define BTN_DEBOUNCE 4  //100hz skaitimas
#define BTN_LONG 200  //100hz skaitimas 2sekundes

typedef struct
{
	uint32_t btn_pinai[BTN_NOKIA_SK];
  uint32_t btn_count[BTN_NOKIA_SK];
	uint32_t btn_count_long[BTN_NOKIA_SK];
  bool 		 btn_status[BTN_NOKIA_SK];
  bool     btn_event[BTN_NOKIA_SK];	
}btn_user_t;


enum {
BTN1_PTR=0,
BTN2_PTR,
BTN3_PTR,
BTN4_PTR

};

void btn_nokia_init(void);
void btn_nokia_handler(void);