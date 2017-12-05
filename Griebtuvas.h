#include <stdint.h>

#define GPIO_DIR1_PORT          GPIOB   
#define GPIO_DIR1_PIN           GPIO_PIN_12


#define GPIO_DIR2_PORT          GPIOB   
#define GPIO_DIR2_PIN           GPIO_PIN_13

#define GPIO_PWM1_PORT          GPIOB   
#define GPIO_PWM1_PIN           GPIO_PIN_14


#define GPIO_PWM2_PORT          GPIOB   
#define GPIO_PWM2_PIN           GPIO_PIN_15


void Griebtuvas_init(void);
void Griebtuvas_isviniot(void);
void Griebtuvas_suviniot(void);
void Griebtuvas_stop(void);
void set_status_griebtuvas(uint8_t statuss);
void Griebtubas_handler(void);
uint8_t get_status_griebtuvas(void);

#define ISVINIOJIMO_LAIKAS 200 //200
#define SUVINIOJIMO_LAIKAS 240
#define SERVO_LAIKAS 100
#define CURRENT_STOP 1250

enum {
 ST_GRIEBTUVAS_STOP=0,
 ST_GRIEBTUVAS_ISVINIOTI,
 ST_GRIEBTUVAS_SUVINIOTI,
 ST_GRIEBTUVAS_NULEISTI,
 ST_GRIEBTUVAS_PAKELTI,
};