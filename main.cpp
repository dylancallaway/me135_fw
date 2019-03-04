/**
 * Firmware for Bair Hockey image processing and motor control
 * Written with MBED OS 5
 * Running on NUCLEO-F767ZI Arm Cortex M7 microcontroller
 * Authors: Dylan Callaway
**/

#include "mbed.h"

// STM32F7 DCMI driver
#include "stm32f7xx_hal_dcmi.h"

// STM32F7 DMA driver
#include "stm32f7xx_hal_dma.h"

/** 
 * Initialize DCMI and DMA interfaces
 * 
 * DCMI = Digital Camera Interface
 * Powers and runs the camera module
 * 
 * DMA = Direct Memory Access
 * Transfers data between peripherals (camera module)
 * and main memory without intervention of the CPU
**/

// Memory destination for DCMI DMA. "destination memory buffer address (LCD Frame buffer)"
// How tf do you specify this?? *******
#define DCMI_MEMORY_LOC 0xC0177000 // Copied this from the STM32F769I camera example ???

// Output format RGB565: 5 bit red, 6 bit green, 5 bit blue.
// Output size = 400x296x2 x 5 bits + 400x296x1 x 6 bits = 1894400 bits = 236800 bytes = 59200 words = 0xE740 words
// Length of DCMI data to be transferred ***IN WORDS***
// For a 32bit CPU: 1 word = 32 bits = 4 bytes
#define DCMI_MEMORY_LEN 0xE740

DCMI_HandleTypeDef dcmi_init_structure; // Create DCMI configuration parameters structure

// Create timers
Timer timer1;

// Create tickers
Ticker heartbeat_ticker;

// Create threads
Thread thread1;

// Status LEDs
DigitalOut led1(LED1); // Green <- Good things LED
DigitalOut led2(LED2); // Blue <- Debug LED
DigitalOut led3(LED3); // Red <- Error LED

// Camera config pins
DigitalOut camera_reset();

// Serial communication
Serial pc(SERIAL_TX, SERIAL_RX, 115200);

void GPIO_Config(void)
{
    GPIO_InitTypeDef gpio_init_structure; // Create GPIO configuration parameters structure

    /*
   Creating a GPIO for the DCMI works like this:
   Call this:
   __HAL_RCC_GPIOX_CLK_ENABLE(); // X is A-Z here
   
    Then run:
    gpio_init_structure.Pin = GPIO_PIN_4 | GPIO_PIN_6;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull = GPIO_PULLUP;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init_structure.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOX, &gpio_init_structure);

    // This initializes pins X4 and X6 where X is any letter A-Z
   */

    // TODO Figure out what pins to set as GPIO for DCMI
    // Is it arbitrary or what?

    /**DCMI GPIO Configuration
     * https://www.st.com/resource/en/datasheet/stm32f767zi.pdf -> Pinouts and pin description
    STM37F767xx Pin Name -> DCMI Pin Name

    PA6  -> PIXCLK
    PA4  -> HSYNC
    PG9  -> VSYNC
    
    PC6  -> D0
    PC7 -> D1
    PC8  -> D2
    PC9  -> D3
    PC11 -> D4
    PB6  -> D5
    PB8  -> D6
    PB9  -> D7

    -----Unused-----
    PC10 -> D8
    PH7  -> D9
    PB5  -> D10
    PD2  -> D11
    PC12 -> D12
    PG7  -> D13
    */

    /* Enable GPIO clocks */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();

    /*** Configure the GPIO ***/
    /* Configure DCMI GPIO as alternate function */
    gpio_init_structure.Pin = GPIO_PIN_4 | GPIO_PIN_6;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull = GPIO_PULLUP;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init_structure.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOA, &gpio_init_structure);

    gpio_init_structure.Pin = GPIO_PIN_6 | GPIO_PIN_8 | GPIO_PIN_9;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull = GPIO_PULLUP;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init_structure.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOB, &gpio_init_structure);

    gpio_init_structure.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_11;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull = GPIO_PULLUP;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init_structure.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOC, &gpio_init_structure);

    gpio_init_structure.Pin = GPIO_PIN_9;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull = GPIO_PULLUP;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init_structure.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOG, &gpio_init_structure);
}

void DMA_Config(void)
{
    DMA_HandleTypeDef dma_init_structure; // Create DMA configuration parameters structure

    /*** Configure the DMA ***/

    /* Enable DMA2 clock */
    __HAL_RCC_DMA2_CLK_ENABLE();

    /* Set the parameters to be configured */
    dma_init_structure.Init.Channel = DMA_CHANNEL_1;
    dma_init_structure.Init.Direction = DMA_PERIPH_TO_MEMORY;
    dma_init_structure.Init.PeriphInc = DMA_PINC_DISABLE;
    dma_init_structure.Init.MemInc = DMA_MINC_ENABLE;
    dma_init_structure.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    dma_init_structure.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    dma_init_structure.Init.Mode = DMA_CIRCULAR; // or DMA_NORMAL ?
    dma_init_structure.Init.Priority = DMA_PRIORITY_HIGH;
    dma_init_structure.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    dma_init_structure.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    dma_init_structure.Init.MemBurst = DMA_MBURST_SINGLE;
    dma_init_structure.Init.PeriphBurst = DMA_PBURST_SINGLE;

    dma_init_structure.Instance = DMA2_Stream1;

    /* Associate the initialized DMA handle to the DCMI handle */
    __HAL_LINKDMA(&dcmi_init_structure, DMA_Handle, dma_init_structure);

    /*** Configure the NVIC for DCMI and DMA ***/
    /* NVIC configuration for DCMI transfer complete interrupt */
    HAL_NVIC_SetPriority(DCMI_IRQn, 0x0F, 0);
    HAL_NVIC_EnableIRQ(DCMI_IRQn); // Always seems to be 78

    /* NVIC configuration for DMA2D transfer complete interrupt */
    HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0x0F, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn); // Always seems to be 57

    /* Configure the DMA stream */
    led3 = HAL_DMA_Init(&dma_init_structure);

    __HAL_DCMI_ENABLE(&dma_init_structure);
}

void DCMI_Config(void)
{
    /*** Configures the DCMI to interface with the camera module ***/

    /* Enable DCMI clock ------------------ MAYBE NEEDED ??? */
    // __HAL_RCC_DCMI_CLK_ENABLE();

    /* DCMI configuration */
    dcmi_init_structure.Init.CaptureRate = DCMI_CR_ALL_FRAME;        // Capture rate
    dcmi_init_structure.Init.HSPolarity = DCMI_HSPOLARITY_HIGH;      // Horizontal polarity
    dcmi_init_structure.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;    // Synchronization mode
    dcmi_init_structure.Init.VSPolarity = DCMI_VSPOLARITY_HIGH;      // Vertical polarity
    dcmi_init_structure.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B; // Data width
    dcmi_init_structure.Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;  // Pixel clock polarity

    dcmi_init_structure.Instance = DCMI;

    led3 = HAL_DCMI_Init(&dcmi_init_structure); // Initialize DCMI with given parameter structure

    /* Enable the DMA ------------------ MAYBE NEEDED ??? */
    // __HAL_DMA_ENABLE(dcmi_init_structure.DMA_Handle);
}

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
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
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
}

void led1_heartbeat(void)
{
    led1 = !led1;
}

int main(void)
{
    /* Enable I-Cache */
    SCB_EnableICache();

    /* Enable D-Cache */
    SCB_EnableDCache();

    SystemClock_Config();

    GPIO_Config();
    DMA_Config();
    DCMI_Config();
    led3 = HAL_DCMI_Start_DMA(&dcmi_init_structure, DCMI_MODE_SNAPSHOT, DCMI_MEMORY_LOC, DCMI_MEMORY_LEN); // Enable DCMI, DMA and capture a frame

    // int revalue = *DCMI_MEMORY_LOC;
    // pc.printf("%d", revalue);

    // Turn on heartbeat
    heartbeat_ticker.attach(led1_heartbeat, 0.5);
}