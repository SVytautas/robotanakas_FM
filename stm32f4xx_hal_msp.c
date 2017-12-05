/**
  ******************************************************************************
  * @file    Templates/Src/stm32f4xx_hal_msp.c
  * @author  MCD Application Team
  * @version V1.2.2
  * @date    09-October-2015
  * @brief   HAL MSP module.
  *         
  @verbatim
 ===============================================================================
                     ##### How to use this driver #####
 ===============================================================================
    [..]
    This file is generated automatically by STM32CubeMX and eventually modified 
    by the user

  @endverbatim
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

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "main.h"
#include "zingsniniai_varikliai.h"
#include "Lidar.h"
#include "OFS.h"
#include "PD8544.h"
#include "gps_compass.h"
#include "debuguart.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @defgroup HAL_MSP
  * @brief HAL MSP module.
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */
extern DMA_HandleTypeDef  hdma_tim2;
extern DMA_HandleTypeDef  hdma_tim3;

static DMA_HandleTypeDef hdma_tx;
static DMA_HandleTypeDef hdma_rx;


extern DMA_HandleTypeDef hdma_usart6_rx;
/**
  * @brief  Initializes the Global MSP.
  * @param  None
  * @retval None
  */
void HAL_MspInit(void)
{
  /* NOTE : This function is generated automatically by STM32CubeMX and eventually  
            modified by the user
   */
__IO uint32_t tmpreg = 0x00;
  UNUSED(tmpreg);	
}

/**
  * @brief  DeInitializes the Global MSP.
  * @param  None  
  * @retval None
  */
void HAL_MspDeInit(void)
{
  /* NOTE : This function is generated automatically by STM32CubeMX and eventually  
            modified by the user
   */
}


void HAL_TIM_OC_MspInit(TIM_HandleTypeDef *htim)
{
	GPIO_InitTypeDef   GPIO_TIM3;
	GPIO_InitTypeDef   GPIO_TIM2; 
 if(htim->Instance==TIM2){
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* TIMx clock enable */
  TIM_2_CLK_ENABLE();
    
  /* Enable GPIO CHANNEL1/3N Clocks */
  TIM_2_CHANNEL3_GPIO_CLK_ENABLE();

  /* Enable DMA2 clock */
  //DMA_TIM_2ir3_CLK_ENABLE
  
  
  /* sukonfiguruojame TIM2 chanel1 isejimo pina definintas main.h */
  GPIO_TIM2.Pin = TIM_2_GPIO_PIN_CHANNEL1;
  GPIO_TIM2.Mode = GPIO_MODE_AF_PP;
  GPIO_TIM2.Pull = GPIO_PULLUP;
  GPIO_TIM2.Speed = GPIO_SPEED_HIGH;
  GPIO_TIM2.Alternate = GPIO_AF_TIM_2;
  HAL_GPIO_Init(TIM_2_GPIO_CHANNEL1_PORT, &GPIO_TIM2);


	/*naikiname
  // sukonfiguruojame dma TIM2 
  hdma_tim2.Init.Channel  = DMA_CHANNEL_TIM_2;
  hdma_tim2.Init.Direction = DMA_MEMORY_TO_PERIPH;
  hdma_tim2.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_tim2.Init.MemInc = DMA_MINC_ENABLE;
  hdma_tim2.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hdma_tim2.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
  hdma_tim2.Init.Priority = DMA_PRIORITY_HIGH;
  hdma_tim2.Init.FIFOMode = DMA_FIFOMODE_DISABLE;         
  hdma_tim2.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
  hdma_tim2.Init.MemBurst = DMA_MBURST_SINGLE;
  hdma_tim2.Init.PeriphBurst = DMA_PBURST_SINGLE; 

  // Set hdma_tim instance 
  hdma_tim2.Instance = TIM_2_DMA_STREAM;

  // Link hdma_tim to hdma[TIM_DMA_ID_UPDATE] (Update) 
  __HAL_LINKDMA(htim, hdma[TIM_DMA_ID_UPDATE], hdma_tim2); //linkas kai viskas perkelta 
  
  // Initialize TIMx DMA handle 
  HAL_DMA_Init(htim->hdma[TIM_DMA_ID_UPDATE]); 
	//HAL_DMA_Start_IT(&hdma_tim, (uint32_t)aSRC_Buffer2, TIM_DMABASE_ARR, 5);
  
  //##-4- Configure the NVIC for DMA #########################################
  // NVIC configuration for DMA transfer complete interrupt 
  HAL_NVIC_SetPriority(TIM_2_DMA_IRQn, 0, 0);   
  HAL_NVIC_EnableIRQ(TIM_2_DMA_IRQn);
	*/
	
  }
	else if (htim->Instance==TIM3)
	{
	 /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* TIMx clock enable */
  TIM_3_CLK_ENABLE();
    
  /* Enable GPIO CHANNEL1/3N Clocks */
  TIM_3_CHANNEL4_GPIO_CLK_ENABLE();

  /* Enable DMA2 clock */
  //DMA_TIM_2ir3_CLK_ENABLE
  
  
  /* sukonfiguruojame TIM2 chanel1 isejimo pina definintas main.h */
  GPIO_TIM3.Pin = TIM_3_GPIO_PIN_CHANNEL4;
  GPIO_TIM3.Mode = GPIO_MODE_AF_PP;
  GPIO_TIM3.Pull = GPIO_PULLUP;
  GPIO_TIM3.Speed = GPIO_SPEED_HIGH;
  GPIO_TIM3.Alternate = GPIO_AF_TIM_3;
  HAL_GPIO_Init(TIM_3_GPIO_CHANNEL4_PORT, &GPIO_TIM3);


  // sukonfiguruojame dma TIM2 naikiname
	/*------------------------------------------------------------------
  hdma_tim3.Init.Channel  = DMA_CHANNEL_TIM_3;
  hdma_tim3.Init.Direction = DMA_MEMORY_TO_PERIPH;
  hdma_tim3.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_tim3.Init.MemInc = DMA_MINC_ENABLE;
  hdma_tim3.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hdma_tim3.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
  hdma_tim3.Init.Priority = DMA_PRIORITY_HIGH;
  hdma_tim3.Init.FIFOMode = DMA_FIFOMODE_DISABLE;         
  hdma_tim3.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
  hdma_tim3.Init.MemBurst = DMA_MBURST_SINGLE;
  hdma_tim3.Init.PeriphBurst = DMA_PBURST_SINGLE; 

  // Set hdma_tim instance 
  hdma_tim3.Instance = TIM_3_DMA_STREAM;

  // Link hdma_tim to hdma[TIM_DMA_ID_UPDATE] (Update) 
  __HAL_LINKDMA(htim, hdma[TIM_DMA_ID_UPDATE], hdma_tim3); //linkas kai viskas perkelta 
  
  // Initialize TIMx DMA handle 
  HAL_DMA_Init(htim->hdma[TIM_DMA_ID_UPDATE]); 
	//HAL_DMA_Start_IT(&hdma_tim, (uint32_t)aSRC_Buffer2, TIM_DMABASE_ARR, 5);
  
  //##-4- Configure the NVIC for DMA #########################################
  //NVIC configuration for DMA transfer complete interrupt 
  HAL_NVIC_SetPriority(TIM_3_DMA_IRQn, 0, 1);   
  HAL_NVIC_EnableIRQ(TIM_3_DMA_IRQn);
	------------------------------------------------------------------------------------*/
	
	
	}
		
}

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
GPIO_InitTypeDef GPIO_InitStruct;	
  //initinam dma ir portus pagal taimeri
	
	if(htim->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM1_MspInit 0 */

  /* USER CODE END TIM1_MspInit 0 */
    /* Peripheral clock enable */
    //__TIM1_CLK_ENABLE();
    __HAL_RCC_TIM1_CLK_ENABLE();
    /**TIM1 GPIO Configuration    
    PE9     ------> TIM1_CH1
    PE11     ------> TIM1_CH2
    PE13     ------> TIM1_CH3
    PE14     ------> TIM1_CH4 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_11|GPIO_PIN_13|GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* Peripheral interrupt init*/
    HAL_NVIC_SetPriority(TIM1_CC_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
  /* USER CODE BEGIN TIM1_MspInit 1 */

  /* USER CODE END TIM1_MspInit 1 */
  }
}


void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
 /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* TIMx Peripheral clock enable */
  SYS_TIMERx_CLK_ENABLE();
  
  /*##-2- Configure the NVIC for TIMx ########################################*/
  /* Set Interrupt Group Priority */ 
  HAL_NVIC_SetPriority(SYS_TIMERx_IRQn, 1, 0);
  
  /* Enable the TIMx global Interrupt */
  HAL_NVIC_EnableIRQ(SYS_TIMERx_IRQn);
	

}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{

   TIM_9_CLK_ENABLE();

}




//UArtas
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  
  
  GPIO_InitTypeDef  GPIO_InitStruct;
  if(huart->Instance==USART1)
	{
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO clock */
  USARTx_TX_GPIO_CLK_ENABLE();
  USARTx_RX_GPIO_CLK_ENABLE();
  /* Enable USARTx clock */
  USARTx_CLK_ENABLE(); 
  /* Enable DMA2 clock */
  DMAx_CLK_ENABLE();   
  
  /*##-2- Configure peripheral GPIO ##########################################*/
  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = USARTx_TX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
  GPIO_InitStruct.Alternate = USARTx_TX_AF;
  
  HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);
    
  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = USARTx_RX_PIN;
  GPIO_InitStruct.Alternate = USARTx_RX_AF;
    
  HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStruct);
    
  /*##-3- Configure the DMA streams ##########################################*/
  /* Configure the DMA handler for Transmission process */
  hdma_tx.Instance                 = USARTx_TX_DMA_STREAM;
  
  hdma_tx.Init.Channel             = USARTx_TX_DMA_CHANNEL;
  hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma_tx.Init.Mode                = DMA_NORMAL;
  hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;
  hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
  hdma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  hdma_tx.Init.MemBurst            = DMA_MBURST_INC4;
  hdma_tx.Init.PeriphBurst         = DMA_PBURST_INC4;
  
  HAL_DMA_Init(&hdma_tx);   
  
  /* Associate the initialized DMA handle to the UART handle */
  __HAL_LINKDMA(huart, hdmatx, hdma_tx);
	HAL_NVIC_SetPriority(USARTx_DMA_TX_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(USARTx_DMA_TX_IRQn);
    
  /* Configure the DMA handler for reception process */
  hdma_rx.Instance                 = USARTx_RX_DMA_STREAM;
  
  hdma_rx.Init.Channel             = USARTx_RX_DMA_CHANNEL;
  hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  //hdma_rx.Init.Mode                = DMA_CIRCULAR;
	hdma_rx.Init.Mode                = DMA_CIRCULAR;
  hdma_rx.Init.Priority            = DMA_PRIORITY_MEDIUM;
  hdma_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
  hdma_rx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  hdma_rx.Init.MemBurst            = DMA_MBURST_INC4;
  hdma_rx.Init.PeriphBurst         = DMA_PBURST_INC4; 

  HAL_DMA_Init(&hdma_rx);
    
  /* Associate the initialized DMA handle to the the UART handle */
  __HAL_LINKDMA(huart, hdmarx, hdma_rx);
  //__HAL_LINKDMA(huart, hdmarx, hdma_usart1_rx);  
  /*##-4- Configure the NVIC for DMA #########################################*/
  /* NVIC configuration for DMA transfer complete interrupt (USARTx_TX) */
  
    
  /* NVIC configuration for DMA transfer complete interrupt (USARTx_RX) */
  HAL_NVIC_SetPriority(USARTx_DMA_RX_IRQn, 2, 0);   
  HAL_NVIC_EnableIRQ(USARTx_DMA_RX_IRQn);
  
  /* NVIC configuration for USART TC interrupt */
  HAL_NVIC_SetPriority(USARTx_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(USARTx_IRQn);
  }
	else if(huart->Instance==USART6)
  {
  /* USER CODE BEGIN USART6_MspInit 0 */

  /* USER CODE END USART6_MspInit 0 */
    /* Peripheral clock enable */
    __USART6_CLK_ENABLE();
  
    /**USART6 GPIO Configuration    
    PC6     ------> USART6_TX
    PC7     ------> USART6_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* Peripheral DMA init*/
  
    hdma_usart6_rx.Instance = DMA2_Stream1;
    hdma_usart6_rx.Init.Channel = DMA_CHANNEL_5;
    hdma_usart6_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart6_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart6_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart6_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart6_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
   // hdma_usart6_rx.Init.Mode = DMA_NORMAL;
		hdma_usart6_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart6_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart6_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&hdma_usart6_rx);

    __HAL_LINKDMA(huart,hdmarx,hdma_usart6_rx);

  /* USER CODE BEGIN USART6_MspInit 1 */

  /* USER CODE END USART6_MspInit 1 */
  }
	
	
	//HAL_NVIC_SetPriority(USARRx_IRQn, 1, 1);
  //HAL_NVIC_EnableIRQ(USARRx_IRQn);
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
  

  /*##-1- Reset peripherals ##################################################*/
  USARTx_FORCE_RESET();
  USARTx_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks #################################*/
  /* Configure UART Tx as alternate function  */
  HAL_GPIO_DeInit(USARTx_TX_GPIO_PORT, USARTx_TX_PIN);
  /* Configure UART Rx as alternate function  */
  HAL_GPIO_DeInit(USARTx_RX_GPIO_PORT, USARTx_RX_PIN);
   
  /*##-3- Disable the DMA Streams ############################################*/
  /* De-Initialize the DMA Stream associate to transmission process */
  HAL_DMA_DeInit(&hdma_tx); 
  /* De-Initialize the DMA Stream associate to reception process */
  HAL_DMA_DeInit(&hdma_rx);
  
  /*##-4- Disable the NVIC for DMA ###########################################*/
  HAL_NVIC_DisableIRQ(USARTx_DMA_TX_IRQn);
  HAL_NVIC_DisableIRQ(USARTx_DMA_RX_IRQn);
}


/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

/**
  * @brief SPI MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  *           - NVIC configuration for SPI interrupt request enable
  * @param hspi: SPI handle pointer
  * @retval None
  */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
	if(hspi->Instance==SPI3)
  {
  GPIO_InitTypeDef  GPIO_InitStruct;
  GPIO_InitTypeDef  GPIO_InitStructA;
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  SPIx_SCK_GPIO_CLK_ENABLE();
  SPIx_MISO_GPIO_CLK_ENABLE();
  SPIx_MOSI_GPIO_CLK_ENABLE();
	SPIx_SS_GPIO_CLK_ENABLE();
  /* Enable SPI clock */
  SPIx_CLK_ENABLE(); 
  
  /*##-2- Configure peripheral GPIO ##########################################*/  
  /* SPI SCK GPIO pin configuration  */
  GPIO_InitStruct.Pin       = SPIx_SCK_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
  GPIO_InitStruct.Alternate = SPIx_SCK_AF;
  
  HAL_GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStruct);
    
  /* SPI MISO GPIO pin configuration  */
  GPIO_InitStruct.Pin = SPIx_MISO_PIN;
  GPIO_InitStruct.Alternate = SPIx_MISO_AF;
  
  HAL_GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStruct);
  
  /* SPI MOSI GPIO pin configuration  */
  GPIO_InitStruct.Pin = SPIx_MOSI_PIN;
  GPIO_InitStruct.Alternate = SPIx_MOSI_AF;
    
  HAL_GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = SPIx_SS_PIN;
  //GPIO_InitStruct.Alternate = SPIx_MOSI_AF;
    
  HAL_GPIO_Init(SPIx_SS_GPIO_PORT, &GPIO_InitStruct);

  /*##-3- Configure the NVIC for SPI #########################################*/
  /* NVIC for SPI */
  HAL_NVIC_SetPriority(SPIx_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(SPIx_IRQn);
}
else if (hspi->Instance==SPI1)
{
	//initinam SPI IO
	GPIO_InitTypeDef  GPIO_PCD8544;
	GPIO_InitTypeDef  GPIO_PCD8544_pap;
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  PCD8544_SCK_GPIO_CLK_ENABLE();
  //PCD8544_MISO_GPIO_CLK_ENABLE();
  PCD8544_MOSI_GPIO_CLK_ENABLE();
	PCD8544_SS_GPIO_CLK_ENABLE();
	//papildomi pinai
	PCD8544_RST_CLK_ENABLE();
	PCD8544_DC_RCC();
  /* Enable SPI clock */
  PCD8544_SPI_CLK_ENABLE(); 
   
  /* SPI SCK GPIO pin configuration  */
  GPIO_PCD8544.Pin       = PCD8544_SCK_PIN;
  GPIO_PCD8544.Mode      = GPIO_MODE_AF_PP;
  GPIO_PCD8544.Pull      = GPIO_PULLUP;
  GPIO_PCD8544.Speed     = GPIO_SPEED_HIGH;
  GPIO_PCD8544.Alternate = PCD8544_SCK_AF;
  
  HAL_GPIO_Init(PCD8544_SCK_GPIO_PORT, &GPIO_PCD8544);
    
  /* SPI MISO GPIO pin configuration  */
  //GPIO_PCD8544.Pin = SPIx_MISO_PIN;
  //GPIO_PCD8544.Alternate = SPIx_MISO_AF;
  
  //HAL_GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStruct);
  
  /* SPI MOSI GPIO pin configuration  */
  GPIO_PCD8544.Pin = PCD8544_MOSI_PIN;
  GPIO_PCD8544.Alternate = PCD8544_MOSI_AF;
    
  HAL_GPIO_Init(PCD8544_MOSI_GPIO_PORT, &GPIO_PCD8544);
	
	
	//initiname kitus portus
	GPIO_PCD8544_pap.Pin       = PCD8544_SS_PIN;
  GPIO_PCD8544_pap.Mode      = GPIO_MODE_OUTPUT_PP;
  GPIO_PCD8544_pap.Pull      = GPIO_PULLUP;
  GPIO_PCD8544_pap.Speed     = GPIO_SPEED_HIGH;
 // GPIO_PCD8544_pap.Alternate = SPIx_MOSI_AF;
    
  HAL_GPIO_Init(PCD8544_SS_GPIO_PORT, &GPIO_PCD8544_pap);
	
	//DC ir RST
	GPIO_PCD8544_pap.Pin = PCD8544_RST_PIN;
 // GPIO_PCD8544.Alternate = PCD8544_MOSI_AF;
    
  HAL_GPIO_Init(PCD8544_RST_PORT, &GPIO_PCD8544_pap);
	
	GPIO_PCD8544_pap.Pin = PCD8544_DC_PIN;
 // GPIO_PCD8544.Alternate = PCD8544_MOSI_AF;
    
  HAL_GPIO_Init(PCD8544_DC_PORT, &GPIO_PCD8544_pap);
  //HAL_NVIC_SetPriority(PCD8544_IRQn, 0, 1);
  //HAL_NVIC_EnableIRQ(PCD8544_IRQn);
}
	
}



void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(hi2c->Instance==I2C2)
  {
  /* USER CODE BEGIN I2C2_MspInit 0 */

  /* USER CODE END I2C2_MspInit 0 */
  
    /**I2C2 GPIO Configuration    
    PB10     ------> I2C2_SCL
    PB11     ------> I2C2_SDA 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __I2C2_CLK_ENABLE();
  /* USER CODE BEGIN I2C2_MspInit 1 */
     PRINTF("init i2c");
  /* USER CODE END I2C2_MspInit 1 */
  }

}






/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
