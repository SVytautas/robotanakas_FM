#include "OFS.h"
#include "main.h"
#include "debuguart.h"
#include "PD8544.h"
#include "delay_simple.h"
#include "math.h"
#include "color.h"
#include "zingsniniai_varikliai.h"
#include "Trasos_iveikimas.h"


SPI_HandleTypeDef SpiHandle;
uint8_t aTxBuffer[10];
uint8_t aRxBuffer[10];

ofs_t ofs={
  .state=OFS_ST_NONE,
	.delay=20,
	.calib_mm=1,
	.tomm_koef=1.0,
};


void OFS_init(void){

/*##-1- Configure the SPI peripheral #######################################*/
  /* Set the SPI parameters */
  SpiHandle.Instance               = SPIx;
  SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
  SpiHandle.Init.CLKPhase          = SPI_PHASE_2EDGE;
  SpiHandle.Init.CLKPolarity       = SPI_POLARITY_HIGH;
  SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  SpiHandle.Init.CRCPolynomial     = 7;
  SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
  SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
  SpiHandle.Init.NSS               = SPI_NSS_SOFT;
  SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
  
//#ifdef MASTER_BOARD
  SpiHandle.Init.Mode = SPI_MODE_MASTER;
//#else
 // SpiHandle.Init.Mode = SPI_MODE_SLAVE;
//#endif /* MASTER_BOARD */

  if(HAL_SPI_Init(&SpiHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
	
	  read_reg(0x00);
    ofs.state=OFS_ST_INIT;

}

uint8_t read_reg(uint8_t reg)
{
    
		//delay();
		HAL_GPIO_WritePin(SPIx_SS_GPIO_PORT, SPIx_SS_PIN, GPIO_PIN_RESET);
		aTxBuffer[0]=reg;
	  HAL_SPI_Transmit(&SpiHandle, (uint8_t*)aTxBuffer, 1,0xFFFF);
		//delay_us(75);
	  _delay_us(75);
	  HAL_SPI_Receive(&SpiHandle,(uint8_t *)aRxBuffer, 1,0xFFFF);
		HAL_GPIO_WritePin(SPIx_SS_GPIO_PORT, SPIx_SS_PIN, GPIO_PIN_SET);
    //delay();
    return aRxBuffer[0];
}


void OFS_write_reg(uint8_t reg, uint8_t data)
{
	
	 HAL_GPIO_WritePin(SPIx_SS_GPIO_PORT, SPIx_SS_PIN, GPIO_PIN_RESET);
   aTxBuffer[0]=reg | 0x80; //write operation
	 HAL_SPI_Transmit(&SpiHandle, (uint8_t*)aTxBuffer, 1,0xFFFF);
	 _delay_us(75);
	 // delay_us(75);	//palaukiame
   aTxBuffer[0]=data; //data operation
	 HAL_SPI_Transmit(&SpiHandle, (uint8_t*)aTxBuffer, 1,0xFFFF);

   HAL_GPIO_WritePin(SPIx_SS_GPIO_PORT, SPIx_SS_PIN, GPIO_PIN_SET);

}


void ofs_handler(void)
{
	//10hz 
	 if (ofs.delay){ofs.delay--;}
   switch (ofs.state)
	 {
		  case OFS_ST_NONE:
				//nieko neveikiame 
			if (!ofs.delay) {
			//einame i init nieko kito nebelieka
				ofs.state=OFS_ST_INIT;
			}
			break;
			
			case OFS_ST_INIT:
			//skaitome ID 
			 if (read_reg(ADNS3080_PRODUCT_ID)==ADNS3080_PRODUCT_ID_VALUE)
			 {
				 //ok setinam parametrus
				  ofs.state=OFS_ST_READ;
				 PRINTF("ofs init ok\r\n");
			 
			 }
			 else
			 {
			    ofs.delay=10; //laukiam 1s ir vel initinam
				  ofs.state=OFS_ST_NONE;
			 };
			break;
			 
			 case OFS_ST_READ:
			//skaitome deltaX ir deltaY
			 //tikriname over bita
				if (CHECK_BIT(read_reg(ADNS3080_MOTION),4)) {ofs.over=true; PRINTF("over");} else {ofs.over=false;}
				ofs.deltaX=read_reg(ADNS3080_DELTA_X);
				ofs.deltaY=read_reg(ADNS3080_DELTA_Y);
				//sumuojam viska
				ofs.sum_X+=ofs.deltaX;
				ofs.sum_Y+=ofs.deltaY;
				
				ofs.possum_X+=abs(ofs.deltaX);
				ofs.possum_Y+=abs(ofs.deltaY);
				
				//sumuojam mm su koeficientu
				ofs.X_mm+=(float)ofs.deltaX*ofs.tomm_koef;
				ofs.Y_mm+=(float)ofs.deltaY*ofs.tomm_koef;
				//suomuojame kuris gali buti nulinamas
				ofs.deltaX_mm+=(float)ofs.deltaX*ofs.tomm_koef;
				ofs.deltaY_mm+=(float)ofs.deltaY*ofs.tomm_koef;
				//suomuojame tik teigiamai
				ofs.posX_mm+=(float)abs(ofs.deltaX)*ofs.tomm_koef;
				ofs.posY_mm+=(float)abs(ofs.deltaY)*ofs.tomm_koef;
				
				ofs.posdeltaX_mm+=(float)(abs(ofs.deltaX))*ofs.tomm_koef;
				ofs.posdeltaY_mm+=(float)abs(ofs.deltaY)*ofs.tomm_koef;
				trasos_iveikias_Ymm((float)abs(ofs.deltaY)*ofs.tomm_koef);
			 break;
				
			case OFS_ST_KALIB:
			//skaitome deltaX ir deltaY
			 //tikriname over bita
			  if (!ofs.delay) {
					//timeoutas jei kazkas blogai
				  ofs.state=OFS_ST_INIT;
					break;
			  }
			
				if (CHECK_BIT(read_reg(ADNS3080_MOTION),7)) {ofs.over=true;} else {ofs.over=false;}
				ofs.deltaX=read_reg(ADNS3080_DELTA_X);
				ofs.deltaY=read_reg(ADNS3080_DELTA_Y);
				//sumuojam viska
				ofs.calib_sum+=abs(ofs.deltaY); //sumuojam tik tiesiai teigiama
				//skaicuojam kalib koeficienta
				ofs.tomm_koef=ofs.calib_mm/ofs.calib_sum; //calib_mm ivedamas
				
			 break;
			
			
	 
	    default:
				ofs.state=OFS_ST_NONE;
			break;
	 }
 


}


void ofs_reset()
{
	
  //reseto nera ant programvimo pino pajungta...
}

void ofs_start_calib(void)
{
	if (!ofs.calibrating)
	{
	   //pradedame kalibravima
		 ofs.state=OFS_ST_KALIB;
		//paleidziame variklius 10s 10cm/s 1metras
		//nuliname
		ofs.calib_sum=0;
		ofs.delay=2000; //200sekundziu sukalibravimui
		ofs.calibrating=true;
		//setinam variklius veikti kazkoki laika
		zv_set_forward(400); //4s
	
	}


}


float ofs_get_posYmm(void)
{
  return ofs.posY_mm;;
  //return ((float)ofs.possum_Y*ofs.tomm_koef);
}

int32_t ofs_get_possumY(void)
{
   return ofs.possum_Y;
}

void ofs_calib_mm_up(void)
{
  ofs.calib_mm++;
}

void ofs_calib_mm_down(void)
{
  ofs.calib_mm--;
}

void ofs_set_kalib(void)
{
  ofs.tomm_koef=calib_ofs.tomm_koef;
	PRINTF("ofs to mm %f",ofs.tomm_koef);
}


void ofs_reset_save_calib(void)
{
   ofs.calibrating=false;
	 ofs.state=OFS_ST_READ;
   ofs.delay=0;
	 PRINTF("write");
	 calib_ofs.tomm_koef=ofs.tomm_koef;
	 color_ee_write_block((char *)(&calib_ofs),EE_ADD_OFS,sizeof(calib_ofs));
	 
}

void OFS_print_calib(void)
{
	 PCD8544_Clear();
		//read_RGB();
		
		PCD8544_GotoXY(0, 0);
	  NOKIA_PRINTF("OFS calib:");
		PCD8544_GotoXY(0, 9);
	  NOKIA_PRINTF("sum:%u",(uint16_t)ofs.calib_sum);
		PCD8544_GotoXY(0, 18);
	  NOKIA_PRINTF("mm: %u",ofs.calib_mm);
	  PCD8544_GotoXY(0, 27);
	  NOKIA_PRINTF("tomm:%.3f",ofs.tomm_koef);
		PCD8544_Refresh();


}




void OFSXY_print_nokia(void)
{
	  PCD8544_Clear();
		//read_RGB();
		
		PCD8544_GotoXY(0, 0);
	  NOKIA_PRINTF("OFS glob dXdY:");
		PCD8544_GotoXY(0, 9);
	  NOKIA_PRINTF("X: %li",ofs.sum_X);
		PCD8544_GotoXY(0, 18);
	  NOKIA_PRINTF("Y: %li",ofs.sum_Y);
		PCD8544_Refresh();

}






//--------------------atvaidavimas pav i nokia ekraneli nereikia nieko ten gero---------------
  uint8_t iii = 0;
	uint8_t jjj = 0;
	uint8_t regValue=0;
	uint8_t pixelValue=0;
void printPixelData(void) {
  bool isFirstPixel = true;

  // Write to frame capture register to force capture of frame
	OFS_write_reg(ADNS3080_FRAME_CAPTURE, 0x83);

  // Wait 3 frame periods + 10 nanoseconds for frame to be captured
  //delayMicroseconds(1510); // Minimum frame speed is 2000 frames/second so 1 frame = 500 nano seconds. So 500 x 3 + 10 = 1510
	//delay_ms(1);

  // Display the pixel data
	PCD8544_Clear();
  for ( iii = 0; iii < ADNS3080_PIXELS_Y; iii++) {
    for ( jjj = 0; jjj < ADNS3080_PIXELS_X; jjj++) {
       read_reg(ADNS3080_FRAME_CAPTURE);
			 regValue = aRxBuffer[0];
      if (isFirstPixel && !(regValue & 0x40)) {
					PCD8544_Clear();
					//read_RGB();
		
					PCD8544_GotoXY(0, 0);
					NOKIA_PRINTF("Failed find");
					PCD8544_GotoXY(0, 9);
					// PCD8544_Puts("Discovery", PCD8544_Pixel_Set, PCD8544_FontSize_5x7);
		
					NOKIA_PRINTF("break");
					PCD8544_Refresh();
				  break;
				  break;
      }
      isFirstPixel = false;
      pixelValue = regValue << 2; // Only lower 6 bits have data
			//PCD8544_Pixel_Set : 
			if (pixelValue>60)
			{
				PCD8544_DrawPixel(jjj, iii, PCD8544_Pixel_Clear);
			}
			else
			{
				PCD8544_DrawPixel(jjj, iii, PCD8544_Pixel_Set);
			}
			
    }
  }
	PCD8544_Refresh();


}