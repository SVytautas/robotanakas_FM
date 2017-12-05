#include <stdint.h>
#include <stdbool.h>
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))
/* Definition for SPIx clock resources */
#define SPIx                             SPI3
#define SPIx_CLK_ENABLE()                __HAL_RCC_SPI3_CLK_ENABLE()
#define SPIx_SCK_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOC_CLK_ENABLE()
#define SPIx_MISO_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE() 
#define SPIx_MOSI_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE() 
#define SPIx_SS_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE() 

#define SPIx_FORCE_RESET()               __HAL_RCC_SPI3_FORCE_RESET()
#define SPIx_RELEASE_RESET()             __HAL_RCC_SPI3_RELEASE_RESET()

/* Definition for SPIx Pins 
#define SPIx_SCK_PIN                     GPIO_PIN_10
#define SPIx_SCK_GPIO_PORT               GPIOC
#define SPIx_SCK_AF                      GPIO_AF6_SPI3
#define SPIx_MISO_PIN                    GPIO_PIN_11
#define SPIx_MISO_GPIO_PORT              GPIOC
#define SPIx_MISO_AF                     GPIO_AF6_SPI3
#define SPIx_MOSI_PIN                    GPIO_PIN_12
#define SPIx_MOSI_GPIO_PORT              GPIOC
#define SPIx_MOSI_AF                     GPIO_AF6_SPI3*/

#define SPIx_SCK_PIN                     GPIO_PIN_10
#define SPIx_SCK_GPIO_PORT               GPIOC
#define SPIx_SCK_AF                      GPIO_AF6_SPI3
#define SPIx_MISO_PIN                    GPIO_PIN_11
#define SPIx_MISO_GPIO_PORT              GPIOC
#define SPIx_MISO_AF                     GPIO_AF6_SPI3
#define SPIx_MOSI_PIN                    GPIO_PIN_12
#define SPIx_MOSI_GPIO_PORT              GPIOC
#define SPIx_MOSI_AF                     GPIO_AF6_SPI3

#define SPIx_SS_PIN                     GPIO_PIN_4
#define SPIx_SS_GPIO_PORT               GPIOC

#define OSF_RESET_PIN                     GPIO_PIN_0
#define OSF_RESET_GPIO_PORT               GPIOC
//#define SPIx_SS_PIN                     GPIO_PIN_15
//#define SPIx_SS_GPIO_PORT               GPIOA

/* Definition for SPIx's NVIC */
#define SPIx_IRQn                        SPI3_IRQn
#define SPIx_IRQHandler                  SPI3_IRQHandler

// Register Map for the ADNS3080 Optical OpticalFlow Sensor
#define ADNS3080_PRODUCT_ID            0x00
#define ADNS3080_MOTION                0x02
#define ADNS3080_DELTA_X               0x03
#define ADNS3080_DELTA_Y               0x04
#define ADNS3080_SQUAL                 0x05
#define ADNS3080_CONFIGURATION_BITS    0x0A
#define ADNS3080_MOTION_CLEAR          0x12
#define ADNS3080_FRAME_CAPTURE         0x13
#define ADNS3080_MOTION_BURST          0x50

// ADNS3080 hardware config
#define ADNS3080_PIXELS_X              30
#define ADNS3080_PIXELS_Y              30

// Id returned by ADNS3080_PRODUCT_ID register
#define ADNS3080_PRODUCT_ID_VALUE      0x17

typedef struct
{
	uint16_t delay;
	int8_t deltaX;
	int8_t deltaY;
	int32_t sum_X;
	int32_t sum_Y;
	int32_t possum_X;
	int32_t possum_Y;
	float   X_mm;
	float   Y_mm;
	float   posX_mm; //tik i +
	float   posY_mm; //tik i +
	float   tomm_koef;
	float   deltaX_mm; //nulinamas
	float   deltaY_mm; //nulinamas
	float   posdeltaX_mm; //nulinamas ir i +
	float   posdeltaY_mm; //nulinamas
	uint8_t state;
	bool    over;
	float   calib_sum;
	uint32_t calib_mm;
	bool    calibrating;
}ofs_t;


enum{
OFS_ST_NONE=0,
OFS_ST_INIT,
OFS_ST_READ,
OFS_ST_KALIB,
};


uint8_t read_reg(uint8_t reg);
void OFS_write_reg(uint8_t reg, uint8_t data);
void OFS_init(void);
void ofs_handler(void);


void printPixelData(void) ;
void OFSXY_print_nokia(void);
void OFS_print_calib(void);
void ofs_start_calib(void);
void ofs_reset_save_calib(void);
void ofs_set_kalib(void);


//reguliavimas
void ofs_calib_mm_up(void);
void ofs_calib_mm_down(void);


float ofs_get_posYmm(void);
int32_t ofs_get_possumY(void);

