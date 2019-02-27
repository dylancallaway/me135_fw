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

void GPIO_Config(void)
{
    GPIO_InitTypeDef gpio_init_structure; // Create GPIO configuration parameters structure

    /**DCMI GPIO Configuration    
    PE4     ------> DCMI_D4
    PA4     ------> DCMI_HSYNC
    PA6     ------> DCMI_PIXCLK
    PA9     ------> DCMI_D0
    PA10     ------> DCMI_D1
    PD3     ------> DCMI_D5
    PG10     ------> DCMI_D2
    PG11     ------> DCMI_D3
    PB7     ------> DCMI_VSYNC
    PB8     ------> DCMI_D6
    PB9     ------> DCMI_D7 
    */

    /* DCMI GPIO configuration **************************************************/
    /*
    D0 -- PC6
    D1 -- PC7
    D2 -- PE0
    D3 -- PE1
    D4 -- PE4
    D5 -- PB6
    D6 -- PB8
    D7 -- PB9
    PCK - PA6
    HS -- PA4
    VS -- PB7
    XCLK -- PA8 // mc0 driven
    Y1,Y0 Not connected
    RST -- PC12 // active low
    PWDN -- PC10 // active hi
*/

    /* Enable GPIO clocks */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

    /*** Configure the GPIO ***/
    /* Configure DCMI GPIO as alternate function */
    gpio_init_structure.Pin = GPIO_PIN_4 | GPIO_PIN_6;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull = GPIO_PULLUP;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init_structure.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOA, &gpio_init_structure);

    gpio_init_structure.Pin = GPIO_PIN_7;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull = GPIO_PULLUP;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init_structure.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOB, &gpio_init_structure);

    gpio_init_structure.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 |
                              GPIO_PIN_9 | GPIO_PIN_11;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull = GPIO_PULLUP;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init_structure.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOC, &gpio_init_structure);

    gpio_init_structure.Pin = GPIO_PIN_3 | GPIO_PIN_6;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull = GPIO_PULLUP;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init_structure.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOD, &gpio_init_structure);

    gpio_init_structure.Pin = GPIO_PIN_5 | GPIO_PIN_6;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull = GPIO_PULLUP;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init_structure.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOE, &gpio_init_structure);
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
    HAL_DMA_Init(&dma_init_structure);

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

    HAL_DCMI_Init(&dcmi_init_structure); // Initialize DCMI with given parameter structure

    /* Enable the DMA ------------------ MAYBE NEEDED ??? */
    // __HAL_DMA_ENABLE(dcmi_init_structure.DMA_Handle);
}

void Camera_Config(void)
{
}

int main(void)
{
    GPIO_Config();
    DMA_Config();
    DCMI_Config();
    HAL_DCMI_Start_DMA(&dcmi_init_structure, DCMI_MODE_SNAPSHOT, DCMI_MEMORY_LOC, DCMI_MEMORY_LEN); // Enable DCMI, DMA and capture a frame
}