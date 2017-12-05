#include <stdint.h>

#define TIM_4                             TIM4   //servo timer
#define TIM_4_CLK_ENABLE                  __HAL_RCC_TIM4_CLK_ENABLE


//PWM isejimas
#define TIM_4_GPIO_CHANNEL1_PORT          GPIOD   
#define TIM_4_GPIO_PIN_CHANNEL1           GPIO_PIN_12

#define GPIOD_CLK_ENABLE                  __HAL_RCC_GPIOD_CLK_ENABLE
#define SERVO_PWM_PRESCALER								83  
#define SERVO_PWM_PERIOD									9999 //18khz



void Servo_init(void);
void set_servo(uint16_t servo);


void servo_nuleisti(void);
void servo_pakelt(void);
void servo_disable();
void servo_nuleisti_griebtuva(void);
void servo_pakelti_griebtuva(void);