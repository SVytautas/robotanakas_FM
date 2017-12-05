#include <stdint.h>
#include <stdbool.h>

#define BTN_BUMP_SK 3

#define BTNS_BUMP_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOC_CLK_ENABLE() 
//BTN1 pin PE2
#define BTN1_BUMP_PIN                     GPIO_PIN_3//GPIO_PIN_3
//BTN2 pin PE3
#define BTN2_BUMP_PIN                     GPIO_PIN_2//GPIO_PIN_3
//BTN1 pin PE1
#define BTN3_BUMP_PIN                     GPIO_PIN_0//GPIO_PIN_3
//BTN1 pin PE0
#define BTNS_BUMP_GPIO_PORT               GPIOC //GPIOC


#define BTN_BUMP_DEBOUNCE 4  //100hz skaitimas
//#define BTN_LONG 200  //100hz skaitimas 2sekundes

typedef struct
{
	uint32_t btn_pinai[BTN_BUMP_SK];
  uint32_t btn_count[BTN_BUMP_SK];
	uint32_t btn_count_long[BTN_BUMP_SK];
  bool 		 btn_status[BTN_BUMP_SK];
  bool     btn_event[BTN_BUMP_SK];	
}btnbump_user_t;


enum {
BTN1_BUM_PTR=0,
BTN2_BUM_PTR,
BTN3_BUM_PTR,

};

void btn_bump_init(void);
void btn_bump_handler(void);
void btn_bump_ekranas_print(void);
bool btn_bump_get_presed(void);


bool btn_bump_kairys(void);
bool btn_bump_centras(void);
bool btn_bump_desinys(void);
