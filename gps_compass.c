#include "gps_compass.h"
#include "stm32f4xx_hal.h"
#include "debuguart.h"
#include "PD8544.h"
#include "math.h"
#include "color.h"
#include "main.h"
#include "zingsniniai_varikliai.h"

gps_t gps={
  .rx_buffer_wp=0,
	.rx_buffer_rp=0,
	.msg_parse=GPS_ST_MSG_NONE

};

compass_t compass={

	.mgPerDigit=1.3,
	.delay=1000, //10s
	.state=COMPASS_ST_MSG_NONE,
	.xOffset=0,
	.yOffset=0,
	.calibrating=false,
};


I2C_HandleTypeDef hi2c2;

UART_HandleTypeDef huart6;
DMA_HandleTypeDef hdma_usart6_rx;

#define GPS_RX_DMA_SIZE 25 //priimam po 25 baitus paskui apdorojam
uint8_t rx_dma_gps_buff[GPS_RX_DMA_SIZE];


//I2c
uint8_t aTxBuff_comp[10];
uint8_t aRxBuff_comp[10];


void gps_rewrite_dma(void)
{
   for (uint16_t i=0;i<GPS_RX_DMA_SIZE;i++)
	{
		gps.rx_buffer[gps.rx_buffer_wp++]=rx_dma_gps_buff[i];
		if (gps.rx_buffer_wp>=GPS_RX_BUFF_SIZE) {gps.rx_buffer_wp=0;};// PRINTF("prirasem 512");}; //nulinam wpointeri
	}
}

void gps_compass_init(void)
{
	//* GPIO Ports Clock Enable */
	//initinam uart ir I2c
  __GPIOB_CLK_ENABLE();
  __GPIOC_CLK_ENABLE();
  /* DMA controller clock enable */
  __DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 4, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
  //initinam uarta
	huart6.Instance = USART6;
  huart6.Init.BaudRate = 9600; //gps baudratas
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart6);
	
	if(HAL_UART_Receive_DMA(&huart6, (uint8_t *)rx_dma_gps_buff, GPS_RX_DMA_SIZE) != HAL_OK)
  {
    /* Transfer error in reception process */
    //Error_Handler();     
  }
	
	//initinam I2c
	hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 200000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
  HAL_I2C_Init(&hi2c2);
	
	uint8_t readreg=gps_read_register_comp(HMC5983L_REG_IDENT_A);
	PRINTF("HMC5983 %u",readreg);
	readreg=gps_read_register_comp(HMC5983L_REG_IDENT_B);
	PRINTF("HMC5983 %u",readreg);
	readreg=gps_read_register_comp(HMC5983L_REG_IDENT_C);
	PRINTF("HMC5983 %u",readreg);
	
	compass.state=COMPASS_ST_INIT; //initinam kompasas
}


void gps_handler(void)
{
	uint8_t baitas;
	if(gps.rx_buffer_rp!=gps.rx_buffer_wp) //jeigu yra duomenu
	{
		//skaitom po viena 
		baitas=gps.rx_buffer[gps.rx_buffer_rp++];
		if (gps.rx_buffer_rp>=GPS_RX_BUFF_SIZE) {gps.rx_buffer_rp=0;}; 
		
		//dedam i headeri
		gps.header=(gps.header<<8) | baitas;
		if (gps.header==NAV_POSLLH_HEADER )
		{
			//28 baitia + 2crc
		  //PRINTF("head found");
			//pazimim pointeri
			gps.pointer_start=gps.rx_buffer_rp;
			gps.msg_byte_count=0;
			gps.msg_parse=GPS_ST_MSG_POSLLH;
			gps.gps_data[0]=0x01; //NAV hederis paskui pildom is baitu
		}
		else if (gps.header==NAV_STATUS_HEADER )
		{
			//PRINTF("head found st");
			//pazimim pointeri
			gps.pointer_start=gps.rx_buffer_rp;
			gps.msg_byte_count=0;
			gps.msg_parse=GPS_ST_MSG_STATUS;
			gps.gps_data[0]=0x01; //NAV hederis paskui pildom is baitu
		
		}
		
		
		switch(gps.msg_parse)
		{
			case GPS_ST_MSG_NONE:
				//praleidziam
			break;
			
			case GPS_ST_MSG_POSLLH:
			//skaicuojam baitus
      
      if (gps.msg_byte_count>=32)
			{
				//gauti visi baitai tikriname crc ir pasiemam reikiamus duomenis
				gps.msg_byte_count++;
        gps.gps_data[gps.msg_byte_count]=baitas;	//uzbaigiam CRC
				
				//PRINTF("%x \r\n",baitas);
				//PRINTF("crc %x==%x\r\n",gps_cacl_crc(32),(uint16_t)gps.gps_data[32]<<8 | gps.gps_data[33]); //crc be crc+ID
				if (gps_cacl_crc(32)==((uint16_t)gps.gps_data[32]<<8 | gps.gps_data[33]))
				{
				  //PRINTF("crc lygu"); //crc lygu pasimam duomenis
					gps.lon=(uint32_t)gps.gps_data[6]<<24 | (uint32_t)gps.gps_data[7]<<16 | (uint32_t)gps.gps_data[8]<<8 | (uint32_t)gps.gps_data[9]<<0;
					gps.lat=(uint32_t)gps.gps_data[10]<<24 | (uint32_t)gps.gps_data[11]<<16 | (uint32_t)gps.gps_data[12]<<8 | (uint32_t)gps.gps_data[13]<<0;
					gps.hAcc=(uint32_t)gps.gps_data[22]<<24 | (uint32_t)gps.gps_data[23]<<16 | (uint32_t)gps.gps_data[24]<<8 | (uint32_t)gps.gps_data[25]<<0;
					//PRINTF("lon: %li",gps.lon);
					//PRINTF("lat: %li",gps.lat);
					//PRINTF("hAcc: %lu",gps.hAcc);
				}
			  gps.msg_parse=GPS_ST_MSG_NONE;
			}
			else
			{
			   gps.msg_byte_count++;
         gps.gps_data[gps.msg_byte_count]=baitas;				
				//PRINTF("%x ",baitas);
			}
      		
			break;
			
			case GPS_ST_MSG_STATUS:
			//skaicuojam baitus
      
      if (gps.msg_byte_count>=20)
			{
				//gauti visi baitai tikriname crc ir pasiemam reikiamus duomenis
				gps.msg_byte_count++;
        gps.gps_data[gps.msg_byte_count]=baitas;	//uzbaigiam CRC
				
				//PRINTF("%x \r\n",baitas);
				//PRINTF("crc %x==%x\r\n",gps_cacl_crc(32),(uint16_t)gps.gps_data[32]<<8 | gps.gps_data[33]); //crc be crc+ID
				if (gps_cacl_crc(20)==((uint16_t)gps.gps_data[20]<<8 | gps.gps_data[21]))
				{
					
				  //PRINTF("St\r\n"); //crc lygu pasimam duomenis
					gps.gpsFix=gps.gps_data[6];
					gps.flags=gps.gps_data[7];
					if (gps.gpsFix==0x02)
					{
					  // PRINTF("2D fix");
					}
					else
					{
					  // PRINTF("no fix");
					}
					
					if (CHECK_BIT(gps.flags,0))
					{
					  //PRINTF("GpsFixOK");
					}
					else
					{
					 // PRINTF("GpsFixNO");
					}
				}
			  gps.msg_parse=GPS_ST_MSG_NONE;
			}
			else
			{
			   gps.msg_byte_count++;
         gps.gps_data[gps.msg_byte_count]=baitas;				
				//PRINTF("%x ",baitas);
			}
      		
			break;
			
			default :
			gps.msg_parse=GPS_ST_MSG_NONE;
			break;
		
		}
		
		
	}



}

uint16_t gps_cacl_crc( uint8_t msg_size)
{
	//uint16_t pointer;
	uint8_t CK_A = 0; 
	uint8_t CK_B = 0;
 for(uint8_t I=0;I<msg_size;I++)
 {
	  //pointer=pointer_start+I;
	  //if (pointer>=GPS_RX_BUFF_SIZE) {
		//pointer} 
		CK_A = CK_A + gps.gps_data[I];
		CK_B = CK_B + CK_A;
 }
 return ((uint16_t)CK_A<<8 | CK_B);
}

void gps_print_nokia(void)
{
    PCD8544_Clear();
		//read_RGB();
		
		PCD8544_GotoXY(0, 0);
	  NOKIA_PRINTF("GPS:");
	  PCD8544_GotoXY(0, 9);
	  if (gps.gpsFix==0x02 || gps.gpsFix==0x03)
		{
		   NOKIA_PRINTF("gpsFix: 2D");
		}
		else
		{
		   NOKIA_PRINTF("gpsFix: noFix");
		}
	  
		PCD8544_GotoXY(0, 18);
	  NOKIA_PRINTF("lon%li",gps.lon);
		PCD8544_GotoXY(0, 27);
	  NOKIA_PRINTF("lat%li",gps.lat);
		PCD8544_Refresh();

}


//--------------------------Compasas------------------------------

uint8_t gps_read_register_comp(uint8_t reg)
{
	aTxBuff_comp[0]=reg;
	HAL_I2C_Master_Transmit(&hi2c2, HMC5983_ADDRESS<<1, (uint8_t*)aTxBuff_comp, 1, 100);
  HAL_I2C_Master_Receive(&hi2c2, HMC5983_ADDRESS<<1, (uint8_t *)aRxBuff_comp, 1, 100);
  return aRxBuff_comp[0];
}

void gps_write_register_comp(uint8_t reg, uint8_t data)
{
	aTxBuff_comp[0]=reg;
	aTxBuff_comp[1]=data;
	HAL_I2C_Master_Transmit(&hi2c2, HMC5983_ADDRESS<<1, (uint8_t*)aTxBuff_comp, 2, 200);
}


void HMC5983L_setRange(hmc5983l_range_t range)
{
    switch(range)
    {
	case HMC5983L_RANGE_0_88GA:
	    compass.mgPerDigit = 0.073f;
	    break;

	case HMC5983L_RANGE_1_3GA:
	    compass.mgPerDigit = 0.92f;
	    break;

	case HMC5983L_RANGE_1_9GA:
	    compass.mgPerDigit = 1.22f;
	    break;

	case HMC5983L_RANGE_2_5GA:
	    compass.mgPerDigit = 1.52f;
	    break;

	case HMC5983L_RANGE_4GA:
	    compass.mgPerDigit = 2.27f;
	    break;

	case HMC5983L_RANGE_4_7GA:
	    compass.mgPerDigit = 2.56f;
	    break;

	case HMC5983L_RANGE_5_6GA:
	    compass.mgPerDigit= 3.03f;
	    break;

	case HMC5983L_RANGE_8_1GA:
	    compass.mgPerDigit = 4.35f;
	    break;

	default:
	    break;
    }

   // writeRegister8(HMC5883L_REG_CONFIG_B, range << 5); //vienas nustatimas tai galim  rasyt su 0
		gps_write_register_comp(HMC5983L_REG_CONFIG_B, range << 5);
}


hmc5983l_range_t HMC5983L_getRange(void)
{
    //return (hmc5983l_range_t)((readRegister8(HMC5883L_REG_CONFIG_B) >> 5));
	    return (hmc5983l_range_t) (gps_read_register_comp(HMC5983L_REG_CONFIG_B)>>5);
} 

void HMC5983L_setMeasurementMode(hmc5983l_mode_t mode)
{
    uint8_t value;

    //value = readRegister8(HMC5883L_REG_MODE);
	  value = gps_read_register_comp(HMC5983L_REG_MODE);
    value &= 0xFC;//0b11111100;
    value |= mode;
    
	  gps_write_register_comp(HMC5983L_REG_MODE, value);
    //writeRegister8(HMC5883L_REG_MODE, value);
}

hmc5983l_mode_t HMC5983L_getMeasurementMode(void)
{
    uint8_t value;
    
	  value = gps_read_register_comp(HMC5983L_REG_MODE);
    //value = readRegister8(HMC5883L_REG_MODE);
    value &= 0x03;//0b00000011;

    return (hmc5983l_mode_t)value;
}

void HMC5983L_setDataRate(hmc5983l_dataRate_t dataRate)
{
    uint8_t value;

    //value = readRegister8(HMC5883L_REG_CONFIG_A);
	  value = gps_read_register_comp(HMC5983L_REG_CONFIG_A);
    value &= 0xE3;//0b11100011;
    value |= (dataRate << 2);

   // writeRegister8(HMC5883L_REG_CONFIG_A, value);
	  gps_write_register_comp(HMC5983L_REG_CONFIG_A, value);
}

hmc5983l_dataRate_t HMC5983L_getDataRate(void)
{
    uint8_t value;

    //value = readRegister8(HMC5883L_REG_CONFIG_A);
	  value = gps_read_register_comp(HMC5983L_REG_CONFIG_A);
    value &= 0x1C;//0b00011100;
    value >>= 2;

    return (hmc5983l_dataRate_t)value;
}

void HMC5983L_setSamples(hmc5983l_samples_t samples)
{
    uint8_t value;

    //value = readRegister8(HMC5883L_REG_CONFIG_A);
	  value = gps_read_register_comp(HMC5983L_REG_CONFIG_A);
    value &= 0x9F;//0b10011111;
    value |= (samples << 5);

    gps_write_register_comp(HMC5983L_REG_CONFIG_A, value);
}

hmc5983l_samples_t HMC5983L_getSamples(void)
{
    uint8_t value;

    value = gps_read_register_comp(HMC5983L_REG_CONFIG_A);
    value &= 0x60; //0b01100000;
    value >>= 5;

    return (hmc5983l_samples_t)value;
}

void gps_compass_handler(void)
{
	//100Hz
	if (compass.delay){compass.delay--;}
	switch(compass.state)
	{
		case COMPASS_ST_MSG_NONE: //nieko neveikiame
		if (!compass.delay)	
		{
		   //reinitnimas
			 compass.state=COMPASS_ST_INIT;
		}
		break;
		
		case COMPASS_ST_INIT:
			
		  //nukaitom ir tikrinam ar atitinka ID
		  if (gps_read_register_comp(HMC5983L_REG_IDENT_A)!='H')
			{
			   compass.delay=2000; //2sekundes ir bandom vel
				 compass.state=COMPASS_ST_MSG_NONE;
			}
			
			//nustatom parametrus
			HMC5983L_setRange(HMC5983L_RANGE_1_3GA);
      HMC5983L_setMeasurementMode(HMC5983L_CONTINOUS);
      //HMC5983L_setDataRate(HMC5983L_DATARATE_15HZ);
			HMC5983L_setDataRate(HMC5983L_DATARATE_75HZ); //75hz
      HMC5983L_setSamples(HMC5983L_SAMPLES_8);
			PRINTF("compas_init ok");
			compass.state=COMPASS_ST_READ_XYZ;
			
		break;
		
		case COMPASS_ST_READ_XYZ: //skaitom duomenis
			//skaitom ir skaiciuojam heading
		aTxBuff_comp[0]=HMC5983L_REG_OUT_X_M; //skaitom 6 nuo X
		HAL_I2C_Master_Transmit(&hi2c2, HMC5983_ADDRESS<<1, (uint8_t*)aTxBuff_comp, 1, 100);
		HAL_I2C_Master_Receive(&hi2c2, HMC5983_ADDRESS<<1, (uint8_t *)aRxBuff_comp, 6, 600);
		//sudedam nuskaitytus
		compass.xaxis=(int16_t)(((uint16_t)aRxBuff_comp[0]<<8) | aRxBuff_comp[1]);
		compass.zaxis=(int16_t)(((uint16_t)aRxBuff_comp[2]<<8) | aRxBuff_comp[3]);
    compass.yaxis=(int16_t)(((uint16_t)aRxBuff_comp[4]<<8) | aRxBuff_comp[5]);
    //PRINTF("x:%i y:%i\r\n",compass.xaxis,compass.yaxis);
    //suskaiciuojam normalizuotus
		compass.xnorm=((float)compass.xaxis-compass.xOffset)*compass.mgPerDigit;
		compass.ynorm=((float)compass.yaxis-compass.yOffset)*compass.mgPerDigit;
		
		//suskaicuojam heading
		  compass.heading=atan2(compass.xnorm, compass.ynorm);
			if (compass.heading < 0)
			{
				compass.heading += 2 * COMP_PI;
			}

			if (compass.heading > 2 * COMP_PI)
			{
				compass.heading -= 2 * COMP_PI;
			}
			
			compass.headingdeg=compass.heading*180/COMP_PI;
			//PRINTF("deg: %u\r\n",(uint16_t)compass.headingdeg);
		
		break;
			
			
		case COMPASS_ST_CALIBRATE:
			//kalibruojam uzduota laika kompasa
			//skaitom ir skaiciuojam heading
		  aTxBuff_comp[0]=HMC5983L_REG_OUT_X_M; //skaitom 6 nuo X
		  HAL_I2C_Master_Transmit(&hi2c2, HMC5983_ADDRESS<<1, (uint8_t*)aTxBuff_comp, 1, 100);
		  HAL_I2C_Master_Receive(&hi2c2, HMC5983_ADDRESS<<1, (uint8_t *)aRxBuff_comp, 6, 600);
		  //sudedam nuskaitytus
		  compass.xaxis=(int16_t)(((uint16_t)aRxBuff_comp[0]<<8) | aRxBuff_comp[1]);
		  compass.zaxis=(int16_t)(((uint16_t)aRxBuff_comp[2]<<8) | aRxBuff_comp[3]);
      compass.yaxis=(int16_t)(((uint16_t)aRxBuff_comp[4]<<8) | aRxBuff_comp[5]);
		  //skaicuojam min max
		  if (compass.xaxis < compass.minX) {compass.minX = compass.xaxis;}
      if (compass.xaxis > compass.maxX) {compass.maxX = compass.xaxis;}
      if (compass.yaxis < compass.minY) {compass.minY = compass.yaxis;}
      if (compass.yaxis > compass.maxY) {compass.maxY = compass.yaxis;}
			//skaicuojam ofsetus
			compass.xOffset=(float)((float)compass.maxX + (float)compass.minX)/2;
		  compass.yOffset=(float)((float)compass.maxY + (float)compass.minY)/2;
			if (!compass.delay)
			{
			  //baigiam kalibravima griztam i matavima
				compass.state=COMPASS_ST_READ_XYZ;
				//isaugom calibravima i eeprom
				calib_comp.xOffset=compass.xOffset;
				calib_comp.yOffset=compass.yOffset;
				PRINTF("write");
				color_ee_write_block((char *)(&calib_comp),EE_ADD_CCOMPSS,sizeof(calib_comp));
			}
	  break;
		
		default:
			   compass.delay=200; //2sekundes ir bandom vel
				 compass.state=COMPASS_ST_MSG_NONE;
	  break;
	
	
	}




}

void gps_nokia_print_compass(void)
{
    PCD8544_Clear();
		//read_RGB();
		
		PCD8544_GotoXY(0, 0);
	  NOKIA_PRINTF("COMPASS:");
	  PCD8544_GotoXY(0, 9);
		NOKIA_PRINTF("Heddeg: %u",(uint16_t)compass.headingdeg);
		PCD8544_GotoXY(0, 18);
	  NOKIA_PRINTF("Xoffset: %i",(int16_t)compass.xOffset);
		PCD8544_GotoXY(0, 27);
	  NOKIA_PRINTF("Yoffset: %i",(int16_t)compass.yOffset);
		PCD8544_Refresh();
  

}

void gps_compass_set_calibrate(void)
{
  if (!compass.calibrating)
	{
	  //setinam calibruoti
	   compass.state=COMPASS_ST_CALIBRATE;
		 compass.delay=3000; //kiek laiko kalibruosim 60s 2min
		 compass.calibrating=true;
		 zv_set_turn(3000);
		 compass.minX=INT16_MAX;
		 compass.minY=INT16_MAX;
		 compass.maxX=INT16_MIN;
		 compass.maxY=INT16_MIN;
	}

}


void gps_nokia_print_compass_calib(void)
{
    PCD8544_Clear();
		//read_RGB();
		
		PCD8544_GotoXY(0, 0);
	  NOKIA_PRINTF("Calib: %u",compass.delay/100);
	  PCD8544_GotoXY(0, 9);
		NOKIA_PRINTF("MinX: %i",(int16_t)compass.minX);
		PCD8544_GotoXY(0, 18);
	  NOKIA_PRINTF("MaxX: %i",(int16_t)compass.maxX);
		PCD8544_GotoXY(0, 27);
	  NOKIA_PRINTF("MinY: %i",(int16_t)compass.minY);
	  PCD8544_GotoXY(0, 36);
	  NOKIA_PRINTF("MaxY: %i",(int16_t)compass.maxY);
		PCD8544_Refresh();
  

}

void gps_compass_reset_calibrat(void)
{

  compass.calibrating=false;
}

void gps_compass_set_calib(void)
{
  compass.xOffset=calib_comp.xOffset;
	compass.yOffset=calib_comp.yOffset;

}


float get_gps_compass_headingdeg(void)
{
    return compass.headingdeg;
}










