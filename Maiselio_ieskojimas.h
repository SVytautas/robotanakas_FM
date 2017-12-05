#include "stm32f4xx_hal.h"
//#include <stdio.h>
#include <stdbool.h>
#include "stm32f4xx_hal_def.h"

#define GREEN_ATITIKIMAS 50

void Maiselis_tick(void);
void Maiselio_ieskojimas_handler(void);
bool get_green(void);
void flag_ieskoti_set(bool set);