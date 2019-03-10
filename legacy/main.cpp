/**
  ******************************************************************************
  * @file    DCMI/DCMI_SnapshotMode/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describe how to configure the camera interface (DCMI) in snapshot
  *          mode to handle a single image capture in QVGA (320x240) resolution and RGB565
  *          format and display the obtained image on LCD screen.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "mbed.h"
#include "camera.h"
#include "FastPWM.h"

/** @addtogroup STM32F7xx_HAL_Examples
  * @{
  */

/** @addtogroup DCMI_SnapshotMode
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define QVGA_RES_X 320
#define QVGA_RES_Y 240

#define WVGA_RES_X 800
#define WVGA_RES_Y 480

#define ARGB8888_BYTE_PER_PIXEL 4
#define RGB565_BYTE_PER_PIXEL 2
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Camera resolution is QVGA (320x240) */
static uint32_t CameraResX = QVGA_RES_X;
static uint32_t CameraResY = QVGA_RES_Y;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void CPU_CACHE_Enable(void);

#define CAMERA_HORIZ_PIXELS 400
#define CAMERA_VERT_PIXELS 296
uint8_t buff[1024];
uint8_t *CAMERA_FRAME_BUFFER = buff;

// Status LEDs
DigitalOut led1(LED1); // Green <- Good things LED
DigitalOut led2(LED2); // Blue <- Debug LED
DigitalOut led3(LED3); // Red <- Error LED

// Serial communication at 115200 baud
Serial pc(SERIAL_TX, SERIAL_RX, 115200);

/* Exported functions ---------------------------------------------------------*/
void i2c_scan(void)
{
  I2C i2c(PB_11, PB_10); // I2C2_SDA = PB_11, I2C2_SCL = PB_10
  i2c.frequency(100000);

  int error, address;
  int nDevices;

  pc.printf("Scanning I2C bus...\n");

  nDevices = 0;

  char write_data[1];
  write_data[0] = 0x00;

  for (address = 1; address < 127; address++)
  {
    i2c.start();
    error = i2c.write(address << 1, write_data, 1, 0); //We shift it left because mbed takes in 8 bit addreses
    i2c.stop();
    if (error == 0)
    {
      pc.printf("I2C device found at address 0x%X.", address); //Returns 7-bit addres
      nDevices++;
    }
  }
  pc.printf("\nScan complete.\n");

  if (nDevices == 0)
  {
    pc.printf("No I2C devices found.\n");
  }
}

char data;
char send_addr = 0x30;
char send_data = 0x05;
int len;

int i2c_reg_read(const char *addr)
{
  I2C i2c(PB_11, PB_10); // I2C2_SDA = PB_11, I2C2_SCL = PB_10
  i2c.frequency(100000);

  i2c.start();
  wait_us(20);

  len = i2c.write(0x30 << 1, addr, 1, 0);
  wait_us(20);

  len = i2c.read(0x30 << 1, &data, 1, 0);
  wait_us(20);

  i2c.stop();

  return len;
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */

FastPWM fpwm(PE_13, 1);
int main(void)
{
  fpwm.period_ticks(36); // Uses 216MHz PLL clock
  fpwm.pulsewidth_ticks(18);

  /* Enable the CPU Cache */
  CPU_CACHE_Enable();

  // // CameraResX = QVGA_RES_X;
  // // CameraResY = QVGA_RES_Y;

  // /* STM32F7xx HAL library initialization:
  //      - Configure the Flash prefetch
  //      - Systick timer is configured by default as source of time base, but user
  //        can eventually implement his proper time base source (a general purpose
  //        timer for example or other time source), keeping in mind that Time base
  //        duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
  //        handled in milliseconds basis.
  //      - Set NVIC Group Priority to 4
  //      - Low Level Initialization
  //    */
  HAL_Init();

  // /* Configure the system clock to 200 MHz */
  // // SystemClock_Config();

  // /* Initialize GPIO */
  // BSP_IO_Init();

  // /* Reset and power down camera to be sure camera is Off prior start testing BSP */
  BSP_CAMERA_HwReset();
  BSP_CAMERA_PwrDown();

  // /*##-3- Camera Initialization and start capture ############################*/
  // /* Initialize the Camera */
  BSP_CAMERA_Init();

  // /* Wait 1s to let auto-loops in the camera module converge and lead to correct exposure */
  // HAL_Delay(1000);

  // /* Start the Camera Snapshot Capture */
  BSP_CAMERA_ContinuousStart((uint8_t *)CAMERA_FRAME_BUFFER);

  // /* Wait until LCD frame buffer is ready */
  // HAL_Delay(200);
  // /* Stop the camera to avoid having the DMA2D work in parallel of Display */
  // /* which cause perturbation of LTDC                                      */
  // // BSP_CAMERA_Stop();

  // i2c_scan();

  // const char reg = 0x05;
  // const char *reg_ptr = &reg;
  // int d1 = i2c_reg_read(reg_ptr);
  // pc.printf("\t%d  %d", d1, data);

  // pc.printf("\t%d", buff[100]);

  led1.write(1);
  while (1)
  {
  }
}

/**
  * @brief  Camera line event callback
  * @param  None
  * @retval None
  */
// ******************* MIGHT NEED STUFF HERE FOR GETTING IMAGE DATA **************
// void BSP_CAMERA_FrameEventCallback(void)
// {
//     static uint32_t camera_line_nb;
//     if (offset_lcd == 0)
//     {
//         /* If Camera resolution is lower than LCD resolution, set display in the middle of the screen */
//         offset_lcd = ((((LcdResY - CameraResY) / 2) * LcdResX) /* Middle of the screen on Y axis */
//                       + ((LcdResX - CameraResX) / 2))          /* Middle of the screen on X axis */
//                      * sizeof(uint32_t);
//     }

//     for (camera_line_nb = 0; camera_line_nb < CameraResY; camera_line_nb++)
//     {
//         LCD_LL_ConvertLineToARGB8888((uint32_t *)(CAMERA_FRAME_BUFFER + offset_cam),
//                                      (uint32_t *)(LCD_FRAME_BUFFER + offset_lcd));

//         offset_cam = offset_cam + (CameraResX * sizeof(uint16_t));
//         offset_lcd = offset_lcd + (LcdResX * sizeof(uint32_t));
//     }
// }

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 200000000
  *            HCLK(Hz)                       = 200000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 400
  *            PLL_P                          = 2
  *            PLL_Q                          = 8
  *            PLL_R                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 6
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 400;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  RCC_OscInitStruct.PLL.PLLR = 7;

  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if (ret != HAL_OK)
  {
    while (1)
    {
      ;
    }
  }

  /* Activate the OverDrive to reach the 200 MHz Frequency */
  ret = HAL_PWREx_EnableOverDrive();
  if (ret != HAL_OK)
  {
    while (1)
    {
      ;
    }
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6);
  if (ret != HAL_OK)
  {
    while (1)
    {
      ;
    }
  }

  // Output clock on MCO1 -> PA_8 (for OV2640 XCLK)
  // This is needed for any communication with the camera, SCCB included
  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSE, RCC_MCODIV_1);
}

/**
  * @brief  CPU L1-Cache enable.
  * @param  None
  * @retval None
  */
static void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
