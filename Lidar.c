#include "main.h"
#include "Lidar.h"
#include "stm32f4xx_it.h"
#include "debuguart.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include "PD8544.h"
#include "gps_compass.h"
#include "dstar.h"
//timerio 9 struktura PE6 PWM isejimas
//Distance mm
//spead *64
TIM_HandleTypeDef    htim9;
TIM_OC_InitTypeDef   hOC9;

GPIO_InitTypeDef   GPIO_E_st;

UART_HandleTypeDef Uart_lidar;

extern DMA_HandleTypeDef hdma_tx;
extern DMA_HandleTypeDef hdma_rx;


//lidaras
#define XV11_PACKAGE_LENGTH	22
#define XV11_START_BYTE	0xFA

uint8_t xv11_dma_rx_buff[DMA_RX_BUFFER_SIZE];

lidar_t lidar={
   .reading=false,
	 .start=false,
	 .rx_buffer_wp=0,
	 .rx_buffer_rp=0,
	 .XV11_Paketas_pt=0,
};

//uint8_t XV11_Fpaketas[2100];
//uint8_t XV11_Fpaketas2[2100];
//uint8_t XV11_Package[22];

uint16_t Distance[360];
uint16_t PackageChecksum(uint8_t * packagePointer);
//const float RAD = (M_TWOPI / 360.0f);
//uint16_t deg;
//float radians, displayDistance;
//uint16_t degSelected[2];
//uint8_t scanEnabled = 1;
//uint8_t pointSelect = 0;




//gauti uart duomenys kaupiam po 110 baitu 5 xv11paketai
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	if(huart->Instance==USART1)
	{
		//perasom i dideli bufferi
	for (uint16_t i=0;i<DMA_RX_BUFFER_SIZE;i++)
	{
		lidar.rx_buffer[lidar.rx_buffer_wp++]=xv11_dma_rx_buff[i];
		if (lidar.rx_buffer_wp>=RX_BUFFER_SIZE) {lidar.rx_buffer_wp=0;}; //PRINTF("prirasem 4K");}; //nulinam wpointeri
	}
	
	}
	else if(huart->Instance==USART6)
	{
	  //PRINTF("get");
		gps_rewrite_dma();
	}
	
}



void Lidar_init(void)
{


  /*##-1- paleidziame periferijos clk #################################*/
  /* TIM9 Peripheral clock enable */
  
    
  /* Enable GPIOE Channels Clock */
  GPIOE_CLK_ENABLE();
  
	GPIO_E_st.Mode = GPIO_MODE_AF_PP;
  GPIO_E_st.Pull = GPIO_PULLUP;
  GPIO_E_st.Speed = GPIO_SPEED_HIGH;
  GPIO_E_st.Alternate = GPIO_AF3_TIM9;
  
  GPIO_E_st.Pin = TIM_9_GPIO_PIN_CHANNEL2;
  HAL_GPIO_Init(TIM_9_GPIO_CHANNEL2_PORT, &GPIO_E_st);


  //sukonfiguruojame pwm
	htim9.Instance = TIM_9;
  
  htim9.Init.Prescaler = MOTOR_PWM_PRESCALER; //preshale kokiu greiciu countins //84/ =50khz
  htim9.Init.Period = MOTOR_PWM_PERIOD; // Top reiksme 237hz
  htim9.Init.ClockDivision = 0;
  htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
  if(HAL_TIM_PWM_Init(&htim9) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
	
	hOC9.OCMode = TIM_OCMODE_PWM1;
  hOC9.OCPolarity = TIM_OCPOLARITY_HIGH;
  hOC9.OCFastMode = TIM_OCFAST_DISABLE;
	hOC9.Pulse = 400;
  if(HAL_TIM_PWM_ConfigChannel(&htim9, &hOC9, TIM_CHANNEL_2) != HAL_OK)
  {
    /* Configuration Error */
    Error_Handler();
  }
	 /* Start channel 2 */
  if(HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_2) != HAL_OK)
  {
    /* PWM Generation Error */
    Error_Handler();
  }
	
  



}


//
void Lidar_uart_init(void)
{
//uart innitinimas
	  /*##-1- Configure the UART peripheral ######################################*/
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* UART1 configured as follow:
      - Word Length = 8 Bits
      - Stop Bit    = One Stop bit
      - Parity      = ODD parity
      - BaudRate    = 9600 baud
      - Hardware flow control disabled (RTS and CTS signals) */
  Uart_lidar.Instance          = USARTx;
  
  Uart_lidar.Init.BaudRate     = 115200;
  Uart_lidar.Init.WordLength   = UART_WORDLENGTH_8B;
  Uart_lidar.Init.StopBits     = UART_STOPBITS_1;
  Uart_lidar.Init.Parity       = UART_PARITY_NONE;
  Uart_lidar.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  Uart_lidar.Init.Mode         = UART_MODE_RX;
  Uart_lidar.Init.OverSampling = UART_OVERSAMPLING_16;
	
  if(HAL_UART_Init(&Uart_lidar) != HAL_OK)
  {
    /* Initialization Error */
    //Error_Handler(); 
  }
  /*##-2- Start the transmission process #####################################*/  
  /* User start transmission data through "TxBuffer" buffer */
  //if(HAL_UART_Transmit_DMA(&Uart_lidar, (uint8_t*)aTxStartMessage, TXSTARTMESSAGESIZE)!= HAL_OK)
  //{
    /* Transfer error in transmission process */
    //Error_Handler();  
  //}
	__HAL_UART_FLUSH_DRREGISTER(&Uart_lidar);
	
	//  if(HAL_UART_Receive_DMA(&Uart_lidar, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK)
	if(HAL_UART_Receive_DMA(&Uart_lidar, (uint8_t *)xv11_dma_rx_buff, DMA_RX_BUFFER_SIZE) != HAL_OK)
  {
    /* Transfer error in reception process */
    //Error_Handler();     
  }
  



}

//0-100
void Lidar_motor_power(uint16_t power)
{
	//if (power>=100) {power=MOTOR_PWM_PERIOD;}
	power=(power*164)>>4; 
	if (power>=MOTOR_PWM_PERIOD) {power=MOTOR_PWM_PERIOD;}
	__HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_2, power);


}



uint16_t Speed;
uint16_t GoodReadings = 0, BadReadings = 0;
uint16_t AnglesCovered = 0;
void ParsePackage(uint8_t * packagePointer)
{
	uint16_t i;
	uint16_t Index;
	
	uint8_t InvalidFlag[4];
	uint8_t WarningFlag[4];
	uint16_t Checksum, ChecksumCalculated;

	Checksum = ((uint16_t)packagePointer[21] << 8) | packagePointer[20];
	ChecksumCalculated=PackageChecksum(packagePointer);
	if (Checksum != ChecksumCalculated) {
		BadReadings += 4;
		//PRINTF("bad");
		//nulinam nuo paskutinio per keturis
	}
	else
	{
		//jeigu geras crc
		Index = (packagePointer[1] - 0xA0) * 4;
		Speed = ((uint16_t)packagePointer[3] << 8) | packagePointer[2];
		//PRINTF("S: %u |",Speed/64);
		InvalidFlag[0] = (packagePointer[5] & 0x80) >> 7;
		InvalidFlag[1] = (packagePointer[9] & 0x80) >> 7;
		InvalidFlag[2] = (packagePointer[13] & 0x80) >> 7;
		InvalidFlag[3] = (packagePointer[17] & 0x80) >> 7;
		WarningFlag[0] = (packagePointer[5] & 0x40) >> 6;
		WarningFlag[1] = (packagePointer[9] & 0x40) >> 6;
		WarningFlag[2] = (packagePointer[13] & 0x40) >> 6;
		WarningFlag[3] = (packagePointer[17] & 0x40) >> 6;

		if (Index == 0) {
			AnglesCovered = 0;
			for (i = 0; i < 360; i++) {
				if (Distance[i] > 0) AnglesCovered++;
			}

			GoodReadings = 0;
			BadReadings = 0;
		}

		for (i = 0; i < 4; i++) {
			if (!InvalidFlag[i])
			{
				if (!WarningFlag[i])
				{
					Distance[Index+i] = packagePointer[4+(i*4)] | ((uint16_t)(packagePointer[5+(i*4)] & 0x3F) << 8);
					lidar.Distance[Index+i] = Distance[Index+i];
					GoodReadings++;
				}
				else
				{
				  Distance[Index+i] = 0;
					lidar.Distance[Index+i] = 0;
					BadReadings++;
					//PRINTF("W");
				}
				
			} else {
				Distance[Index+i] = 0;
				lidar.Distance[Index+i] = 0;
				BadReadings++;
				//PRINTF("!");
			}
			}
	
	}

	

}


uint16_t PackageChecksum(uint8_t * packagePointer)
{
	uint8_t i;
	uint16_t data[10];
	uint16_t checksum;
	uint32_t chk32;

	// group the data by word, little-endian
	for (i = 0; i < 10; i++) {
		data[i] = packagePointer[2*i] | (((uint16_t)packagePointer[2*i+1]) << 8);
	}

	// compute the checksum on 32 bits
	chk32 = 0;
	for (i = 0; i < 10; i++) {
    	chk32 = (chk32 << 1) + data[i];
	}

   // return a value wrapped around on 15bits, and truncated to still fit into 15 bits
   checksum = (chk32 & 0x7FFF) + ( chk32 >> 15 ); // wrap around to fit into 15 bits
  return checksum = checksum & 0x7FFF; // truncate to 15 bits
}


void Lidar_handleris(void)
{
	uint8_t baitas;
	if(lidar.rx_buffer_rp!=lidar.rx_buffer_wp)
	{
		//jeigu yra duomenu apdorojam
		//skaitom baita
		baitas=lidar.rx_buffer[lidar.rx_buffer_rp++];
		if (lidar.rx_buffer_rp>=RX_BUFFER_SIZE) {lidar.rx_buffer_rp=0;}
		if (baitas==XV11_START_BYTE && !lidar.start) //jeigu ne start_end ir Start baitas
		{
			//radom XV11_start baita dedam uzbaitkti 22 baitus paketui
			lidar.XV11_Paketas_pt=0;
			lidar.start=true;
		  lidar.XV11_Paketas[lidar.XV11_Paketas_pt++]=baitas;
		}
		else if (lidar.start)
		{
			//uzbaigiam baitus
			lidar.XV11_Paketas[lidar.XV11_Paketas_pt++]=baitas;
			if (lidar.XV11_Paketas_pt>=XV11_PACKAGE_LENGTH)
			{
				lidar.XV11_Paketas_pt=0; //baigiam ir isparsinam paketa
				lidar.start=false;
				ParsePackage(lidar.XV11_Paketas);
			//	PRINTF("par");
			}
		
		}
		else
		{
			//laukiam start nesomoningi baitai
		  PRINTF("?");
		}
	
	}
	
	
}




uint16_t minimumas;
uint16_t minimumo_kampas;
float min_kampas_rad;
float rad_todeg=0.017453;
float Xzym,Yzym;
float esamas_kampas=1.5708;
uint16_t minimumas_kaire_priekis;
uint16_t minimumas_kaire_priekis_kampas;
uint16_t minimumas_desine_priekis;
uint16_t minimumas_desine_priekis_kampas;

uint16_t minimumas_kaire_sonas;
uint16_t minimumas_kaire_sonas_kampas;
uint16_t minimumas_desine_sonas;
uint16_t minimumas_desine_sonas_kampas;

void objekto_find_handleris(void)
{
	uint16_t kampas;
	minimumas=0xFFFF;
	for (kampas = 0; kampas < 360; kampas++) {
		//min paieska atmetan 0;
			if (Distance[kampas] > 0) 
			{
				if (Distance[kampas]<minimumas)
				{
					minimumas=Distance[kampas];
					minimumo_kampas=kampas;
				
				}
				
			}
			
  }
	//nuskenuojam kliutis priekyje kaireje
	minimumas_kaire_priekis=0xFFFF;
	for (kampas = 10; kampas < 55; kampas++) {
		//min paieska atmetan 0;
			if (Distance[kampas] > 0) 
			{
				if (Distance[kampas]<minimumas_kaire_priekis)
				{
					minimumas_kaire_priekis=Distance[kampas];
					minimumas_kaire_priekis_kampas=kampas;
				
				}
				
			}
			
  }
	//nuskenuojam kliutis priekyje desineje
	minimumas_desine_priekis=0xFFFF;
	for (kampas = 325; kampas < 359; kampas++) {
		//min paieska atmetan 0;
			if (Distance[kampas] > 0) 
			{
				if (Distance[kampas]<minimumas_desine_priekis)
				{
					minimumas_desine_priekis=Distance[kampas];
					minimumas_desine_priekis_kampas=kampas;
				
				}
				
			}
			
  }
	for (kampas = 325; kampas < 10; kampas++) {
		//min paieska atmetan 0;
			if (Distance[kampas] > 0) 
			{
				if (Distance[kampas]<minimumas_desine_priekis)
				{
					minimumas_desine_priekis=Distance[kampas];
					minimumas_desine_priekis_kampas=kampas;
				
				}
				
			}
			
  }
	
	//praskenuojam sonus
	minimumas_kaire_sonas=0xFFFF;
	//buvo 55 120
	for (kampas = 65; kampas < 110; kampas++) {
		//min paieska atmetan 0;
			if (Distance[kampas] > 0) 
			{
				if (Distance[kampas]<minimumas_kaire_sonas)
				{
					minimumas_kaire_sonas=Distance[kampas];
					minimumas_kaire_sonas_kampas=kampas;
				
				}
				
			}
			
  }
	
	minimumas_desine_sonas=0xFFFF;
	// buvo 260 325
	for (kampas = 270; kampas < 315; kampas++) {
		//min paieska atmetan 0;
			if (Distance[kampas] > 0) 
			{
				if (Distance[kampas]<minimumas_desine_sonas)
				{
					minimumas_desine_sonas=Distance[kampas];
					minimumas_desine_sonas_kampas=kampas;
				
				}
				
			}
			
  }
	
	
	//PRINTF("kliutis desine: %u\r\n",minimumas_desine_priekis);
	//PRINTF("kliutis kaire: %u\r\n",minimumas_kaire_priekis);
		//minimumo_kampas+=80;
			//if (minimumo_kampas>=360)
			//{minimumo_kampas-=360;}
			
		//	min_kampas_rad=(float)minimumo_kampas*rad_todeg;
		//	Yzym=minimumas*sinf(min_kampas_rad);
		//	Xzym=minimumas*cosf(min_kampas_rad);

}

uint16_t get_minumas_desine_priekis(void)
{

return minimumas_desine_priekis;

}

uint16_t get_minumas_kaire_priekis(void)
{

return minimumas_kaire_priekis;

}


uint16_t get_minumas_kaire_sonas(void)
{

return minimumas_kaire_sonas;

}
uint16_t get_minumas_desine_sonas(void)
{

return minimumas_desine_sonas;

}


void display_grid(void)
{


}

#define X_centr 24
#define Y_centr 42
#define Y_max_boud 84
#define X_max_boud 48

void lidar_print_nokia(void)
{
	//suskaiciuojam kordinates
	PCD8544_Clear();
	for(uint16_t deg=0;deg<360;deg++)
	{
	   lidar.X[deg]=((float)Distance[deg]/25)*cosf(DEGTORAD(deg));
		 lidar.Y[deg]=((float)Distance[deg]/25)*sinf(DEGTORAD(deg));
		// PRINTF("deg %u: X %i Y %i\r\n",deg,(int16_t)lidar.X[deg],(int16_t)lidar.Y[deg]);
		 
		 //sukonvertuojam i 84x48 matrica nuo centro
		 if (lidar.X[deg]>0 && lidar.Y[deg]>0)
			//if (lidar.X[deg]<0 && lidar.Y[deg]>0)
		 {       //x
			 //|  V |    | 
			 //-----------y
			 //|    |    |
			  if (X_centr>=lidar.X[deg])
				{lidar.X_nok[deg]=X_centr-(uint16_t)lidar.X[deg];
				}
				else
				{
				  lidar.X_nok[deg]=0;
				}
				
				if (Y_centr>=lidar.Y[deg])
				{
				  lidar.Y_nok[deg]=Y_centr-(uint16_t)lidar.Y[deg];
				}
				else
				{
				  lidar.Y_nok[deg]=0;
				}
				
				//lidar.Y_nok[deg]=Y_centr+(uint16_t)lidar.Y[deg];
				//if (lidar.Y_nok[deg]>=Y_max_boud)
				//{ 
				//  lidar.Y_nok[deg]=83;  
				//}   
		  }
		  else if (lidar.X[deg]>0 && lidar.Y[deg]<0)
			//else if (lidar.X[deg]<0 && lidar.Y[deg]<0)
		  {       //x
			 //|    |  V | 
			 //-----------y
			 //|    |    |
			  if (X_centr>=lidar.X[deg])
				{lidar.X_nok[deg]=X_centr-(uint16_t)lidar.X[deg];
				}
				else
				{
				  lidar.X_nok[deg]=0;
				}
				
				lidar.Y_nok[deg]=Y_centr+(uint16_t)abs(lidar.Y[deg]);
				if (lidar.Y_nok[deg]>=Y_max_boud)
				{ 
				  lidar.Y_nok[deg]=83;  
				}   
				
				//if (Y_centr>=abs(lidar.Y[deg]))
				//{lidar.Y_nok[deg]=Y_centr-(uint16_t)abs(lidar.Y[deg]);
				//}
				//else
				//{
				//  lidar.Y_nok[deg]=0;
				//}
				  
		  }
			else if (lidar.X[deg]<0 && lidar.Y[deg]<0)
			//else if (lidar.X[deg]>0 && lidar.Y[deg]<0)
		  {       //x
			 //|    |    | 
			 //-----------y
			 //|    |  V |
			  if (X_centr>=abs(lidar.X[deg]))
				{lidar.X_nok[deg]=X_centr+(uint16_t)abs(lidar.X[deg]);
				}
				else
				{
				  lidar.X_nok[deg]=47;
				}
				
				lidar.Y_nok[deg]=Y_centr+(uint16_t)abs(lidar.Y[deg]);
				if (lidar.Y_nok[deg]>=Y_max_boud)
				{ 
				  lidar.Y_nok[deg]=83;  
				}   
				//if (Y_centr>=abs(lidar.Y[deg]))
				//{lidar.Y_nok[deg]=Y_centr-(uint16_t)abs(lidar.Y[deg]);
				//}
				//else
				//{
				//  lidar.Y_nok[deg]=0;
				//}
				  
		 }
			else if (lidar.X[deg]<0 && lidar.Y[deg]>0)
		  //else if (lidar.X[deg]>0 && lidar.Y[deg]>0)
		  {       //x
			 //|     |    | 
			 //-----------y
			 //|  V  |    |
			  if (X_centr>=abs(lidar.X[deg]))
				{lidar.X_nok[deg]=X_centr+(uint16_t)abs(lidar.X[deg]);
				}
				else
				{
				  lidar.X_nok[deg]=47;
				}
				
				if (Y_centr>=lidar.Y[deg])
				{
				  lidar.Y_nok[deg]=Y_centr-(uint16_t)lidar.Y[deg];
				}
				else
				{
				  lidar.Y_nok[deg]=0;
				}
				
				//lidar.Y_nok[deg]=Y_centr+(uint16_t)lidar.Y[deg];
				//if (lidar.Y_nok[deg]>=Y_max_boud)
				//{
				//   lidar.Y_nok[deg]=83;
				//'} 
				  
		 }
			else
			{
			   lidar.X_nok[deg]=24; 
			   lidar.Y_nok[deg]=42;
			}
			
		// PRINTF("deg %u: X %i Y %i\r\n",deg,(int16_t)lidar.X_nok[deg],(int16_t)lidar.Y_nok[deg]);
		 //PCD8544_DrawPixel(jjj, iii, PCD8544_Pixel_Clear);
		 PCD8544_DrawPixel(lidar.Y_nok[deg], lidar.X_nok[deg], PCD8544_Pixel_Set);
			//PCD8544_DrawPixel(42, 24, PCD8544_Pixel_Set);
			//PCD8544_DrawPixel(0, 47, PCD8544_Pixel_Set);
	}
  PCD8544_Refresh();

}



void lidar_to_XY(uint16_t X_center , uint16_t Y_center, uint16_t plius_kamapas, float rez1)
{
	//suskaiciuojam kordinates
	PCD8544_Clear();
	dstar_reset_map();
	for(uint16_t deg=0;deg<360;deg++)
	{
		 uint16_t kampas=deg+plius_kamapas;
		 lidar.X[deg]=((float)Distance[deg]/rez1)*cosf(DEGTORAD(kampas));
		 lidar.Y[deg]=((float)Distance[deg]/rez1)*sinf(DEGTORAD(kampas));
	  // lidar.X[deg]=((float)Distance[deg]/rez1)*cosf(DEGTORAD((float)deg+plius_kamapas));
		// lidar.Y[deg]=((float)Distance[deg]/rez1)*sinf(DEGTORAD((float)deg+plius_kamapas));
		// PRINTF("deg %u: X %i Y %i\r\n",deg,(int16_t)lidar.X[deg],(int16_t)lidar.Y[deg]);
		 
		 //sukonvertuojam i 84x48 matrica nuo centro
		 if (lidar.X[deg]>0 && lidar.Y[deg]>0)
			//if (lidar.X[deg]<0 && lidar.Y[deg]>0)
		 {       //x
			 //|  V |    | 
			 //-----------y
			 //|    |    |
			  if (X_center>=lidar.X[deg])
				{lidar.X_nok[deg]=X_center-(uint16_t)lidar.X[deg];
				}
				else
				{
				  lidar.X_nok[deg]=UINT8_MAX; //kitu atveju dedam maximuma kad nemaisytu
				}
				
				if (Y_center>=lidar.Y[deg])
				{
				  lidar.Y_nok[deg]=Y_center-(uint16_t)lidar.Y[deg];
				}
				else
				{
				  lidar.Y_nok[deg]=UINT8_MAX;
				}
				
				//lidar.Y_nok[deg]=Y_centr+(uint16_t)lidar.Y[deg];
				//if (lidar.Y_nok[deg]>=Y_max_boud)
				//{ 
				//  lidar.Y_nok[deg]=83;  
				//}   
		  }
		  else if (lidar.X[deg]>0 && lidar.Y[deg]<0)
			//else if (lidar.X[deg]<0 && lidar.Y[deg]<0)
		  {       //x
			 //|    |  V | 
			 //-----------y
			 //|    |    |
			  if (X_center>=lidar.X[deg])
				{lidar.X_nok[deg]=X_center-(uint16_t)lidar.X[deg];
				}
				else
				{
				  lidar.X_nok[deg]=UINT8_MAX;
				}
				
				lidar.Y_nok[deg]=Y_center+(uint16_t)abs(lidar.Y[deg]);
				if (lidar.Y_nok[deg]>=Y_max_boud)
				{ 
				  lidar.Y_nok[deg]=UINT8_MAX;  
				}   
				
				//if (Y_centr>=abs(lidar.Y[deg]))
				//{lidar.Y_nok[deg]=Y_centr-(uint16_t)abs(lidar.Y[deg]);
				//}
				//else
				//{
				//  lidar.Y_nok[deg]=0;
				//}
				  
		  }
			else if (lidar.X[deg]<0 && lidar.Y[deg]<0)
			//else if (lidar.X[deg]>0 && lidar.Y[deg]<0)
		  {       //x
			 //|    |    | 
			 //-----------y
			 //|    |  V |
			  if (X_center>=abs(lidar.X[deg]))
				{lidar.X_nok[deg]=X_center+(uint16_t)abs(lidar.X[deg]);
				}
				else
				{
				  lidar.X_nok[deg]=UINT8_MAX;
				}
				
				lidar.Y_nok[deg]=Y_center+(uint16_t)abs(lidar.Y[deg]);
				if (lidar.Y_nok[deg]>=Y_max_boud)
				{ 
				  lidar.Y_nok[deg]=UINT8_MAX;  
				}   
				//if (Y_centr>=abs(lidar.Y[deg]))
				//{lidar.Y_nok[deg]=Y_centr-(uint16_t)abs(lidar.Y[deg]);
				//}
				//else
				//{
				//  lidar.Y_nok[deg]=0;
				//}
				  
		 }
			else if (lidar.X[deg]<0 && lidar.Y[deg]>0)
		  //else if (lidar.X[deg]>0 && lidar.Y[deg]>0)
		  {       //x
			 //|     |    | 
			 //-----------y
			 //|  V  |    |
			  if (X_center>=abs(lidar.X[deg]))
				{lidar.X_nok[deg]=X_center+(uint16_t)abs(lidar.X[deg]);
				}
				else
				{
				  lidar.X_nok[deg]=UINT8_MAX;
				}
				
				if (Y_center>=lidar.Y[deg])
				{
				  lidar.Y_nok[deg]=Y_center-(uint16_t)lidar.Y[deg];
				}
				else
				{
				  lidar.Y_nok[deg]=UINT8_MAX;
				}
				
				//lidar.Y_nok[deg]=Y_centr+(uint16_t)lidar.Y[deg];
				//if (lidar.Y_nok[deg]>=Y_max_boud)
				//{
				//   lidar.Y_nok[deg]=83;
				//'} 
				  
		 }
			else
			{
			   lidar.X_nok[deg]=UINT8_MAX; 
			   lidar.Y_nok[deg]=UINT8_MAX;
			}
			
		// PRINTF("deg %u: X %i Y %i\r\n",deg,(int16_t)lidar.X_nok[deg],(int16_t)lidar.Y_nok[deg]);
		 //PCD8544_DrawPixel(jjj, iii, PCD8544_Pixel_Clear);
		  PCD8544_DrawPixel(lidar.Y_nok[deg], lidar.X_nok[deg], PCD8544_Pixel_Set);
			dstar_add_obstacle(lidar.Y_nok[deg],lidar.X_nok[deg]);
			//PCD8544_DrawPixel(42, 24, PCD8544_Pixel_Set);
			//PCD8544_DrawPixel(0, 47, PCD8544_Pixel_Set);
	}
  PCD8544_Refresh();
	dstar_plan(6, 0, 6, 12);
	dstar_printf_map();

}


void lidar_to_XY_add_obstacle(uint16_t X_center , uint16_t Y_center, uint16_t plius_kamapas, float rez1)
{

	for(uint16_t deg=0;deg<360;deg++)
	{
		 uint16_t kampas=deg+plius_kamapas;
		 lidar.X[deg]=((float)Distance[deg]/rez1)*cosf(DEGTORAD(kampas));
		 lidar.Y[deg]=((float)Distance[deg]/rez1)*sinf(DEGTORAD(kampas));
		 
		 //sukonvertuojam i 84x48 matrica nuo centro
		 if (lidar.X[deg]>0 && lidar.Y[deg]>0)
			//if (lidar.X[deg]<0 && lidar.Y[deg]>0)
		 {       //x
			 //|  V |    | 
			 //-----------y
			 //|    |    |
			  if (X_center>=lidar.X[deg])
				{lidar.X_nok[deg]=X_center-(uint16_t)lidar.X[deg];
				}
				else
				{
				  lidar.X_nok[deg]=UINT8_MAX; //kitu atveju dedam maximuma kad nemaisytu
				}
				
				if (Y_center>=lidar.Y[deg])
				{
				  lidar.Y_nok[deg]=Y_center-(uint16_t)lidar.Y[deg];
				}
				else
				{
				  lidar.Y_nok[deg]=UINT8_MAX;
				} 
		  }
		  else if (lidar.X[deg]>0 && lidar.Y[deg]<0)
			//else if (lidar.X[deg]<0 && lidar.Y[deg]<0)
		  {       //x
			 //|    |  V | 
			 //-----------y
			 //|    |    |
			  if (X_center>=lidar.X[deg])
				{lidar.X_nok[deg]=X_center-(uint16_t)lidar.X[deg];
				}
				else
				{
				  lidar.X_nok[deg]=UINT8_MAX;
				}
				
				lidar.Y_nok[deg]=Y_center+(uint16_t)abs(lidar.Y[deg]);
				if (lidar.Y_nok[deg]>=Y_max_boud)
				{ 
				  lidar.Y_nok[deg]=UINT8_MAX;  
				}
				  
		  }
			else if (lidar.X[deg]<0 && lidar.Y[deg]<0)
			//else if (lidar.X[deg]>0 && lidar.Y[deg]<0)
		  {       //x
			 //|    |    | 
			 //-----------y
			 //|    |  V |
			  if (X_center>=abs(lidar.X[deg]))
				{lidar.X_nok[deg]=X_center+(uint16_t)abs(lidar.X[deg]);
				}
				else
				{
				  lidar.X_nok[deg]=UINT8_MAX;
				}
				
				lidar.Y_nok[deg]=Y_center+(uint16_t)abs(lidar.Y[deg]);
				if (lidar.Y_nok[deg]>=Y_max_boud)
				{ 
				  lidar.Y_nok[deg]=UINT8_MAX;  
				}
				  
		 }
			else if (lidar.X[deg]<0 && lidar.Y[deg]>0)
		  //else if (lidar.X[deg]>0 && lidar.Y[deg]>0)
		  {       //x
			 //|     |    | 
			 //-----------y
			 //|  V  |    |
			  if (X_center>=abs(lidar.X[deg]))
				{lidar.X_nok[deg]=X_center+(uint16_t)abs(lidar.X[deg]);
				}
				else
				{
				  lidar.X_nok[deg]=UINT8_MAX;
				}
				
				if (Y_center>=lidar.Y[deg])
				{
				  lidar.Y_nok[deg]=Y_center-(uint16_t)lidar.Y[deg];
				}
				else
				{
				  lidar.Y_nok[deg]=UINT8_MAX;
				}
				  
		 }
			else
			{
			   lidar.X_nok[deg]=UINT8_MAX; 
			   lidar.Y_nok[deg]=UINT8_MAX;
			}
			

			dstar_add_obstacle(lidar.Y_nok[deg],lidar.X_nok[deg]);
	}



}







