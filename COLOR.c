#include "main.h"
#include "Lidar.h"
#include "stm32f4xx_it.h"
#include "debuguart.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
//#include "Servo.h"
#include "Griebtuvas.h"
#include "COLOR.h"
#include "stm32f4xx_hal.h"
#include "PD8544.h"

//------------------------Color jutiklis-----------------------------------------------
#define I2C_ADDRESS        0x39 //TC3414
/* I2C handler declaration */
I2C_HandleTypeDef I2cHandle;

/* Buffer used for transmission */
uint8_t aTxBuffer_i2c[10] = {0x84};

/* Buffer used for reception */
uint8_t aRxBuffer_i2c[10];
uint8_t read_reg_data;
/* Private function prototypes -----------------------------------------------*/
static uint16_t Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength);
//void delay(uint32_t delay);
/* Private functions ---------------------------------------------------------*/
uint16_t Red,Green,Blue;

//----------------------EEpromas 24C02 A2 A1 A0 i zeme 1010 000 RW -> 0x50
#define I2C_ADDRESS_EE        0x50 //TC3414
uint8_t TxBuffer_i2c_EE[10];
/* Buffer used for reception */
uint8_t RxBuffer_i2c_EE[256];


uint16_t max_red, max_green, max_blue;
uint16_t min_red, min_green, min_blue;

void COLOR_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
  
  /*##-1- Enable GPIO Clocks #################################################*/
  /* Enable GPIO TX/RX clock */
  I2Cx_SCL_GPIO_CLK_ENABLE();
  I2Cx_SDA_GPIO_CLK_ENABLE();
  
  /*##-2- Configure peripheral GPIO ##########################################*/  
  /* I2C TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = I2Cx_SCL_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
  GPIO_InitStruct.Alternate = I2Cx_SCL_AF;
  HAL_GPIO_Init(I2Cx_SCL_GPIO_PORT, &GPIO_InitStruct);
    
  /* I2C RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = I2Cx_SDA_PIN;
  GPIO_InitStruct.Alternate = I2Cx_SDA_AF;
  HAL_GPIO_Init(I2Cx_SDA_GPIO_PORT, &GPIO_InitStruct);
  
  /*##-3- Enable I2C peripheral Clock ########################################*/
  /* Enable I2C1 clock */
  I2Cx_CLK_ENABLE(); 
	
	
	 /*##-1- Configure the I2C peripheral ######################################*/
  I2cHandle.Instance             = I2Cx;
  
  I2cHandle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
  I2cHandle.Init.ClockSpeed      = 200000;
  I2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  I2cHandle.Init.DutyCycle       = I2C_DUTYCYCLE_2;
  I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  I2cHandle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
  I2cHandle.Init.OwnAddress1     = 0x00;
  I2cHandle.Init.OwnAddress2     = 0x00;
  
  if(HAL_I2C_Init(&I2cHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();    
  }
  
  read_reg_data=read_register(0x84);
	Color_sensor_init();



}






uint8_t read_register(uint8_t reg)
{
	aTxBuffer_i2c[0]=reg | CMD_WRITE;
	HAL_I2C_Master_Transmit(&I2cHandle, I2C_ADDRESS<<1, (uint8_t*)aTxBuffer_i2c, 1, 100);
  HAL_I2C_Master_Receive(&I2cHandle, I2C_ADDRESS<<1, (uint8_t *)aRxBuffer_i2c, 1, 100);
  return aRxBuffer_i2c[0];
}

void write_register(uint8_t reg,uint8_t data)
{
	aTxBuffer_i2c[0]=reg | CMD_WRITE;
	aTxBuffer_i2c[1]=data;
	HAL_I2C_Master_Transmit(&I2cHandle, I2C_ADDRESS<<1, (uint8_t*)aTxBuffer_i2c, 2, 100);
  //HAL_I2C_Master_Receive(&I2cHandle, I2C_ADDRESS<<1, (uint8_t *)aRxBuffer, 1, 10000);
  //return aRxBuffer[0];
}

void Color_sensor_init(void)
{
    //_writeReg(REG_TIMING, INTEG_MODE_FREE | INTEG_PARAM_INTTIME_12MS);
    //_writeReg(REG_INT, INTR_CTL_DISABLE);
	write_register(REG_TIMING, INTEG_MODE_FREE | INTEG_PARAM_INTTIME_100MS);
	write_register(REG_INT, INTR_CTL_DISABLE);
	//write_register(REG_GAIN, GAIN_1 | PRESCALER_1);
	write_register(REG_GAIN, GAIN_4 | PRESCALER_1);
	//_writeReg(REG_GAIN, gain | prescaler);
   // setGain(GAIN_1, PRESCALER_1); // set default gain value and prescaler value
  write_register(REG_TIMING, INTEG_PARAM_INTTIME_12MS);
	write_register(REG_GAIN, GAIN_16 | PRESCALER_1);
	//enablina jutikli
	//_writeReg(REG_CTL, CTL_ADC_EN | CTL_POWER);
  write_register(REG_CTL, CTL_ADC_EN | CTL_POWER);
	color_reset_min_max();
}
float santikis_red;
float santikis_blue;
void read_RGB(void)
{
	  
//Wire.beginTransmission(TCS3414_ADDR);
  //Wire.write(CMD_WRITE | REG_BLOCK_READ);
  //Wire.endTransmission();
    aTxBuffer_i2c[0]=CMD_WRITE | REG_BLOCK_READ;
	  HAL_I2C_Master_Transmit(&I2cHandle, I2C_ADDRESS<<1, (uint8_t*)aTxBuffer_i2c, 1, 100); //100ms	  //delay(1000);
	  HAL_I2C_Master_Receive(&I2cHandle, I2C_ADDRESS<<1, (uint8_t *)aRxBuffer_i2c, 9, 100);
	  Green=aRxBuffer_i2c[2]<<8 | aRxBuffer_i2c[1];
	  Red=aRxBuffer_i2c[4]<<8 | aRxBuffer_i2c[3];
	  Blue=aRxBuffer_i2c[6]<<8 | aRxBuffer_i2c[5];
 // delay(20);
  //Wire.beginTransmission(TCS3414_ADDR);
  //Wire.requestFrom(TCS3414_ADDR, 8);
    //apdorojam duomenis
	  santikis_blue=(float)Green/ (float) Blue;
	  santikis_red=(float)Green/ (float)Red;

}


void color_find_min_calc_santikis(void)
{
    if (Red>max_red)
		{
		   max_red=Red;
		}
		
		if (Blue>max_blue)
		{
		  max_blue=Blue;
		}
		
		if (Green>max_green)
		{
		 max_green=Green;
		}
		
		if (Red<min_red)
		{
		   min_red=Green;
		}
		
		if (Blue<min_blue)
		{
		  min_blue=Blue;
		}
		
		if (Green<min_green)
		{
		 min_green=Green;
		}
		
		

}

void color_reset_min_max(void)
{
   max_red=0;
	 max_green=0;
	 max_blue=0;
	 min_red=INT16_MAX;
	 min_green=INT16_MAX;
	 min_blue=INT16_MAX;

}

uint16_t color_get_max_min(void)
{
   if (max_red>min_red && min_red!=INT16_MAX)
	 {
	   return max_red-min_red;
	 }
	 else
	 {
	   return 0;
	 }
}


float get_santikis_red(void)
{
   return santikis_red;

}

float get_santikis_blue(void)
{
   return santikis_blue;

}


void color_ee_write_block(char * source, uint8_t ee_start_adress, uint8_t source_size)
{
	uint8_t page=ee_start_adress/EE_PAGE_SIZE; //esamas page
	uint8_t page_to;
	uint8_t j=1;
	uint8_t ee_adrr_plius=0;
	for (uint8_t i=0; i<source_size;i++) //prabegam per page
	{
		TxBuffer_i2c_EE[j++]=*(source++);
		PRINTF("tx %u",TxBuffer_i2c_EE[j-1]);
		page_to=(ee_start_adress+i+1)/EE_PAGE_SIZE;
		if (page_to>page)
		{
			page=page_to;
			//jei uzsipilde page rasom
			//adresas
			TxBuffer_i2c_EE[0]=ee_start_adress+ee_adrr_plius;
			HAL_I2C_Master_Transmit(&I2cHandle, I2C_ADDRESS_EE<<1, (uint8_t*)TxBuffer_i2c_EE, j, 500);
			//HAL_Delay(20); //plaukiam 5ms
			ee_adrr_plius+=j-1;
		  j=1; //new data pildimas
			PRINTF("siunciam page %u adress %u\r\n",page-1,TxBuffer_i2c_EE[0]);
			
		}
	}
	if (j>1)
	{
		//jei nedapildem page pagiam rasyti
		TxBuffer_i2c_EE[0]=ee_start_adress+ee_adrr_plius;
		HAL_I2C_Master_Transmit(&I2cHandle, I2C_ADDRESS_EE<<1, (uint8_t*)TxBuffer_i2c_EE, j, 500);
		//siunciam
		PRINTF("siunciam\r\n");
	  //HAL_Delay(20); //plaukiam 5ms
	}
	

}

void color_ee_read_block(char * dest, uint8_t ee_start_adress, uint8_t dest_size)
{
	TxBuffer_i2c_EE[0]=ee_start_adress;
	HAL_I2C_Master_Transmit(&I2cHandle, I2C_ADDRESS_EE<<1, (uint8_t*)TxBuffer_i2c_EE, 1, 100);
  HAL_I2C_Master_Receive(&I2cHandle, I2C_ADDRESS_EE<<1, (uint8_t *)dest, dest_size, 100);
	//HAL_I2C_Master_Receive(&I2cHandle, I2C_ADDRESS_EE<<1, (uint8_t *)aRxBuffer_i2c, dest_size, 100);
	

}

void color_print_rgb(void)
{
	 //samplas
	//adc_user.adc_sample[adc_user.adc_kanalas_ptr]//
    PCD8544_Clear();
		//read_RGB();
		
		PCD8544_GotoXY(0, 0);
	  NOKIA_PRINTF("RGB jut:");
		PCD8544_GotoXY(0, 9);
	  //NOKIA_PRINTF("EL: %u",adc_user.adc_sample[ADC_SAMPLE_7_4BAT]);
	  NOKIA_PRINTF("R: %u",Red);
		PCD8544_GotoXY(0, 18);
	  NOKIA_PRINTF("G: %u",Green);
	  PCD8544_GotoXY(0, 27);
	  NOKIA_PRINTF("B: %u",Blue);
	  PCD8544_GotoXY(0, 34);
	  NOKIA_PRINTF("DD: %u",color_get_max_min());
		PCD8544_Refresh();

}

