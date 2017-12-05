#include <stdint.h>
#include <stdbool.h>


#define PAPILDOMI_LD1_LD2_CLK_ENABLE      __HAL_RCC_GPIOC_CLK_ENABLE //tik chanel 3 ir chanel 4 update turi
#define PAPILDOMI_LD3_LD4_CLK_ENABLE      __HAL_RCC_GPIOA_CLK_ENABLE

#define PAPILDOMI_LD1_GPIO_PORT          GPIOC   
#define PAPILDOMI_LD1_GPIO_PIN           GPIO_PIN_8

#define PAPILDOMI_LD2_GPIO_PORT          GPIOC   
#define PAPILDOMI_LD2_GPIO_PIN           GPIO_PIN_9

#define PAPILDOMI_LD3_GPIO_PORT          GPIOA  
#define PAPILDOMI_LD3_GPIO_PIN           GPIO_PIN_8

#define PAPILDOMI_LD4_GPIO_PORT          GPIOA    //neinitinam neveikia swd
#define PAPILDOMI_LD4_GPIO_PIN           GPIO_PIN_13


#define BEEP_ON				  HAL_GPIO_WritePin(PAPILDOMI_LD1_GPIO_PORT, PAPILDOMI_LD1_GPIO_PIN, GPIO_PIN_SET);
#define BEEP_OFF				HAL_GPIO_WritePin(PAPILDOMI_LD1_GPIO_PORT, PAPILDOMI_LD1_GPIO_PIN, GPIO_PIN_RESET);

#define PAP_BEEP_SIZE 16

typedef struct
{
	bool     beepon;
  uint8_t  beeper[PAP_BEEP_SIZE];
	uint8_t  beeper_ptr;
	//uint8_t  beep_off[PAP_BEEP_SIZE];
	//uint32_t btn_count_long[BTN_BUMP_SK];
  //bool 		 btn_status[BTN_BUMP_SK];
  //bool     btn_event[BTN_BUMP_SK];	
}beep_t;

void pap_beep_one(uint8_t beep_time);
void papildomi_beep_handler(void);
void papildomi_init(void);
void pap_beep_reset(void);
void pap_beep_push(uint8_t beepp);
void pap_beep_start(void);
void pap_beep_mult(uint8_t beep_time, uint8_t beep_k);