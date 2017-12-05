#include <stdint.h>
#include <stdbool.h>



enum{
EKRANAS_ST_INIT=0,
EKRANAS_ST_MENIU,
EKRANAS_ST_VYGDOMA,
EKRANAS_ST_NIEKO,
};

typedef struct
{
	uint8_t status;
	uint8_t colum_ptr;
	uint8_t line_ptr[4];
  bool lock_control;
	uint32_t delay;
	uint32_t atv_par_status;
	uint8_t print_timeout;
}ekranas_t;


void ekranas_atv_handler(void);
//char * ekranas_get_str(uint8_t columm, uint8_t line0, uint8_t line1);
char * ekranas_get_str(uint8_t numb);

//valdimas
void ekranas_press_ok(void);
void ekranas_press_up(void);
void ekranas_press_down(void);
uint8_t ekranas_get_cur_size(void);
void ekranas_set_status(uint8_t statt);

void ekranas_hold_up(void);
void ekranas_hold_down(void);