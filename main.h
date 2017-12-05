/**
  ******************************************************************************
  * @file    Templates/Inc/main.h 
  * @author  MCD Application Team
  * @version V1.2.2
  * @date    09-October-2015
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

#define SYS_TIMERx TIM5
#define SYS_TIMER_PERIOD 99
#define SYS_TIMER_PRESCALER 839 //100kz

#define SYS_TIMERx_CLK_ENABLE                __HAL_RCC_TIM5_CLK_ENABLE

/* Definition for TIMx's NVIC */
#define SYS_TIMERx_IRQn                      TIM5_IRQn
#define SYS_TIMERx_IRQHandler                TIM5_IRQHandler

#define EE_ADD_ZV 0
#define EE_ADD_CCOMPSS (EE_ADD_ZV+ sizeof(nust_zv_t))
#define EE_ADD_OFS     (EE_ADD_CCOMPSS+ sizeof(calib_comp_t))	



#define OFS_HANDLER_30ms 1

typedef struct
{
	uint16_t max_greitis;
	uint16_t max_sukimas;
  uint16_t acc_step;
	uint64_t X;
}nust_zv_t;

//suagojamos sturkturos
typedef struct
{
	float xOffset;
	float yOffset;
}calib_comp_t;

typedef struct
{
	float tomm_koef;
}calib_ofs_t;

extern calib_comp_t calib_comp;
extern calib_ofs_t calib_ofs;
//funkcijos
void SystemClock_Config(void);
void Error_Handler(void);
void sys_tick_init(void);
void sys_tick_calback(void);








#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
