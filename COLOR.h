#include "stm32f4xx_hal.h"
//#include <stdio.h>
#include <stdbool.h>
#include "stm32f4xx_hal_def.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* User can use this section to tailor I2Cx/I2Cx instance used and associated 
   resources */
/* Definition for I2Cx clock resources */
#define I2Cx                             I2C1
#define I2Cx_CLK_ENABLE()                __HAL_RCC_I2C1_CLK_ENABLE()
#define I2Cx_SDA_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2Cx_SCL_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE() 

#define I2Cx_FORCE_RESET()               __HAL_RCC_I2C1_FORCE_RESET()
#define I2Cx_RELEASE_RESET()             __HAL_RCC_I2C1_RELEASE_RESET()

/* Definition for I2Cx Pins */
#define I2Cx_SCL_PIN                    GPIO_PIN_6
#define I2Cx_SCL_GPIO_PORT              GPIOB
#define I2Cx_SCL_AF                     GPIO_AF4_I2C1
#define I2Cx_SDA_PIN                    GPIO_PIN_7
#define I2Cx_SDA_GPIO_PORT              GPIOB
#define I2Cx_SDA_AF                     GPIO_AF4_I2C1

uint8_t read_register(uint8_t reg);
void write_register(uint8_t reg,uint8_t data);
void Color_sensor_init(void);
void read_RGB(void);
void COLOR_init(void);
float get_santikis_red(void);
float get_santikis_blue(void);
void color_print_rgb(void);
void color_find_min_calc_santikis(void);
void color_reset_min_max(void);
uint16_t color_get_max_min(void);


//Definai EEPROM
#define EE_PAGE_SIZE 8  //0-255 page
#define EE_HEADER_PAGE 0 //header informacija

void color_ee_write_block(char * source, uint8_t ee_start_adress, uint8_t source_size);
void color_ee_read_block(char * dest, uint8_t ee_start_adress, uint8_t dest_size);
//testas
//color_ee_write_block((char *)(&testa[0]),7,sizeof(testa));
//color_ee_write_block((char *)(&testa[0]),7,sizeof(testa));
//color_ee_read_block((char *)(&testa[0]),7,sizeof(testa));
//for (uint8_t i=0; i<sizeof(testa);i++)
//{PRINTF("t %u |",testa[i]);}
//color_ee_write_block((char *)(&testa[0]),7+32,sizeof(testa));
//color_ee_read_block((char *)(&testa[0]),7+32,sizeof(testa));
//for (uint8_t i=0; i<sizeof(testa);i++)
//{PRINTF("t %u |",testa[i]);}
//color_ee_write_block((char *)(&testa[0]),7+64,sizeof(testa));
//((char *)(&testa[0]),7+64,sizeof(testa));
//for (uint8_t i=0; i<sizeof(testa);i++)
//{PRINTF("t %u |",testa[i]);}
//color_ee_write_block((char *)(&testa[0]),7+96,sizeof(testa));
//color_ee_read_block((char *)(&testa[0]),7+96,sizeof(testa));
//for (uint8_t i=0; i<sizeof(testa);i++)
//{PRINTF("t %u |",testa[i]);}
////testas
//uint8_t testa[32]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};
//testa[32]

//registai COLOR
#define TCS3414_ADDR 0x39  //the I2C address for the TCS3414

// *** COMMAND REGISTER
// Commands
#define CMD_WRITE  0x80    // Command for writing bytes into registers
#define CMD_CLEARINT 0xE0  // Clear any pending interrupt and is a writeonce-to-clear field

// Selects type of transaction to follow in subsequent data transfer.
#define CMD_TRANSACTION_BYTE     0x00
#define CMD_TRANSACTION_WORD     0x20
#define CMD_TRANSACTION_BLOCK    0x40
#define CMD_TRANSACTION_INTCLEAR 0x60

// Registers
#define REG_CTL        0x00    // control register
#define REG_TIMING     0x01    // timing register
#define REG_INT        0x02    // interrupt control
#define REG_INT_SOURCE 0x03    // interrupt source
#define REG_ID         0x04    // id register
#define REG_GAIN       0x07    // gain register
#define REG_LOW_THRESH            0x08
#define REG_LOW_THRESH_LOW_BYTE   0x08  // ADC interrupt source lower byte of the low threshold.
#define REG_LOW_THRESH_HIGH_BYTE  0x09  // ADC interrupt source upper byte of the low threshold.
#define REG_HIGH_THRESH           0x0A
#define REG_HIGH_THRESH_LOW_BYTE  0x0A  // ADC interrupt source lower byte of the high threshold.
#define REG_HIGH_THRESH_HIGH_BYTE 0x0B  // ADC interrupt source upper byte of the high threshold.
#define REG_BLOCK_READ 0x0F    // write in this block to read the following registers in one block message
#define REG_GREEN_LOW  0x10    // green channel LSB
#define REG_GREEN_HIGH 0x11    // green channel MSB
#define REG_RED_LOW    0x12    // red channel LSB
#define REG_RED_HIGH   0x13    // red channel MSB
#define REG_BLUE_LOW   0x14    // blue channel LSB
#define REG_BLUE_HIGH  0x15    // blue channel MSB
#define REG_CLEAR_LOW  0x16    // clear channel LSB
#define REG_CLEAR_HIGH 0x17    // clear channel MSB


// *** REG_CTL : Control Register

#define CTL_ADC_EN     0x03  // This field enables the four ADC channels to begin integration.
#define CTL_ADC_VALID  0x10  // This read-only field indicates that the ADC channel has completed an integration cycle.
#define CTL_POWER      0x01  // Writing a 1 powers on the device, and writing a 0 turns it off.

//#define CLR_INT 0xE0


// *** REG_TIMING : Timing Register

#define SYNC_EDGE 0x40  // If SYNC_EDGE is low, the falling edge of the sync pin is used to stop an integration cycle when INTEG_MODE is 11. If SYNC_EDGE is high, the rising edge of the sync pin is used to stop an integration cycle when INTEG_MODE is 11.
#define INTEG_MODE_FREE 0x00  // the integrator is free-running and one of the three internally-generated Nominal Integration Times is selected for each conversion
#define INTEG_MODE_MANUAL 0x10  // Manually start/stop integration through serial bus using ADC_EN field in Control Register.
#define INTEG_MODE_SYN_SINGLE 0x20  // Synchronize exactly one internally-timed integration cycle as specified in the NOMINAL INTEGRATION TIME beginning 2.4 µs after being initiated by the SYNC IN pin.
#define INTEG_MODE_SYN_MULTI 0x30  // Integrate over specified number of pulses on SYNC IN pin (See SYNC IN PULSE COUNT table below). Minimum width of sync pulse is 50 µs. SYNC IN must be low at least 3.6 µs.

// Uses single, multipurpose bitmapped field to select one of three predefined integration times or set the number of SYNC IN pulses to count when the INTEG_MODE accumulate mode (11) is selected.
// NOTE: INTEG_MODE and TIME/COUNTER fields should be written before ADC_EN is asserted.
// Below, NOMINAL INTEGRATION TIME
#define INTEG_PARAM_INTTIME_12MS   0x00
#define INTEG_PARAM_INTTIME_100MS  0x01
#define INTEG_PARAM_INTTIME_400MS  0x02

// Below, SYNC IN PULSE COUNT
#define INTEG_PARAM_PULSE_COUNT1   0x00
#define INTEG_PARAM_PULSE_COUNT2   0x01
#define INTEG_PARAM_PULSE_COUNT4   0x02
#define INTEG_PARAM_PULSE_COUNT8   0x03
#define INTEG_PARAM_PULSE_COUNT16  0x04
#define INTEG_PARAM_PULSE_COUNT32  0x05
#define INTEG_PARAM_PULSE_COUNT64  0x06
#define INTEG_PARAM_PULSE_COUNT128 0x07


// *** REG_INT : Interrupt Control Register

#define INTR_STOP 0x40  // Stop ADC integration on interrupt. When high, ADC integration will stop once an interrupt is asserted. To resume operation (1) de-assert ADC_EN using CONTROL register, (2) clear interrupt using COMMAND register, and (3) re-assert ADC_EN using CONTROL register. Note: Use this bit to isolate a particular condition when the sensor is continuously integrating.

// INTR Control Select. This field determines mode of interrupt logic according to the table below:
#define INTR_CTL_DISABLE  0x00  // Interrupt output disabled.
#define INTR_CTL_LEVEL    0x10  // Level Interrupt.
#define INTR_CTL_SMB      0x20  // SMB-Alert compliant.
#define INTR_CTL_SETINTR  0x30  // Sets an interrupt and functions as mode 10.

// Interrupt persistence. Controls rate of interrupts to the host processor:
#define INTR_PERSIST_EVERY  0x00 // Every ADC cycle generates interrupt
#define INTR_PERSIST_SINGLE 0x01 // Any value outside of threshold range.
#define INTR_PERSIST_01SEC  0x02 // Consecutively out of range for 0.1 second
#define INTR_PERSIST_1SEC   0x03 // Consecutively out of range for 1 second


// *** REG_INT_SOURCE : Interrupt Souce Register
// Interrupt Source. Selects which ADC channel to use to generate an interrupt:
#define INT_SOURCE_GREEN 0x00
#define INT_SOURCE_RED   0x01
#define INT_SOURCE_BLUE  0x10
#define INT_SOURCE_CLEAR 0x03


// *** REG_GAIN : Gain Register
// Analog Gain Control. This field switches the common analog gain of the four ADC channels. Four gain modes are provided:
#define GAIN_1 0x00
#define GAIN_4 0x10
#define GAIN_16 0x20
#define GAIN_64 0x30

// Prescaler. This field controls a 6-bit digital prescaler and divider. The prescaler reduces the sensitivity of each ADC integrator as shown in the table below:
#define PRESCALER_1 0x00
#define PRESCALER_2 0x01
#define PRESCALER_4 0x02
#define PRESCALER_8 0x03
#define PRESCALER_16 0x04
#define PRESCALER_32 0x05
#define PRESCALER_64 0x06

