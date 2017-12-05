#include "stm32f4xx_hal.h"
//#include <stdio.h>
#include <stdbool.h>
#include "stm32f4xx_hal_def.h"
//#include "stm32f4xx_hal.h"
//XV-11 lazerinio skenerio apibrezimai
//timeris pwm variklio valdimui
#define TIM_9                              TIM9   //PWM variklio greicio reguliavimas
#define TIM_9_CLK_ENABLE                  __HAL_RCC_TIM9_CLK_ENABLE

//PWM isejimas
#define TIM_9_GPIO_CHANNEL2_PORT          GPIOE   
#define TIM_9_GPIO_PIN_CHANNEL2           GPIO_PIN_6

#define GPIOE_CLK_ENABLE                  __HAL_RCC_GPIOE_CLK_ENABLE
#define MOTOR_PWM_PRESCALER								8  
#define MOTOR_PWM_PERIOD									1025 //18khz


//Uart
#define USARTx                           USART1
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE();
#define DMAx_CLK_ENABLE()                __HAL_RCC_DMA2_CLK_ENABLE()
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE() 

#define USARTx_FORCE_RESET()             __HAL_RCC_USART1_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART1_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_9
#define USARTx_TX_GPIO_PORT              GPIOA  
#define USARTx_TX_AF                     GPIO_AF7_USART1
#define USARTx_RX_PIN                    GPIO_PIN_10
#define USARTx_RX_GPIO_PORT              GPIOA 
#define USARTx_RX_AF                     GPIO_AF7_USART1

/* Definition for USARTx's DMA */
#define USARTx_TX_DMA_CHANNEL            DMA_CHANNEL_4
#define USARTx_TX_DMA_STREAM             DMA2_Stream7         
#define USARTx_RX_DMA_CHANNEL            DMA_CHANNEL_4
#define USARTx_RX_DMA_STREAM             DMA2_Stream2

/* Definition for USARTx's NVIC */
#define USARTx_DMA_TX_IRQn               DMA2_Stream7_IRQn
#define USARTx_DMA_RX_IRQn               DMA2_Stream2_IRQn
#define USARTx_DMA_TX_IRQHandler         DMA2_Stream7_IRQHandler
#define USARTx_DMA_RX_IRQHandler         DMA2_Stream2_IRQHandler
#define USARTx_IRQn                      USART1_IRQn
#define USARTx_IRQHandler                USART1_IRQHandler


#define DMA_RX_BUFFER_SIZE 242 //5 po 22paketus priimam
#define RX_BUFFER_SIZE 4000 //4 kilobaitai rx buferis


//koverteriai
#define VMD_PI      		 3.14159265358979323846
#define RADTODEG(a)     (a*180.0/VMD_PI)
#define DEGTORAD(a)     (a*VMD_PI/180.0)



typedef struct
{
  bool reading;
	bool start;
	uint16_t Distance[360];
	uint8_t XV11_Paketas[22];
	uint8_t XV11_Paketas_pt;
	uint8_t rx_buffer[RX_BUFFER_SIZE];
	uint16_t rx_buffer_wp;
	uint16_t rx_buffer_rp;
	float X[360];
	float Y[360];
	uint16_t X_nok[360];
	uint16_t Y_nok[360];
}lidar_t;




void Lidar_uart_init(void);
void Lidar_init(void);
void Lidar_motor_power(uint16_t power);
void Lidar_handleris(void);
uint16_t PackageChecksum(uint8_t * packagePointer);
void ParsePackage(uint8_t * packagePointer);
bool get_flag_can_parse(void);
bool get_flag_find_min_object(void);
void objekto_find_handleris(void);
uint16_t get_minumas_desine_priekis(void);
uint16_t get_minumas_kaire_priekis(void);

uint16_t get_minumas_kaire_sonas(void);
uint16_t get_minumas_desine_sonas(void);

void lidar_print_nokia(void);
void lidar_to_XY(uint16_t X_center , uint16_t Y_center, uint16_t plius_kamapas, float rez1);
void lidar_to_XY_add_obstacle(uint16_t X_center , uint16_t Y_center, uint16_t plius_kamapas, float rez1);

