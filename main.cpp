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
#define DCMI_MEMORY_LOC NULL

// Output format RGB565: 5 bit red, 6 bit green, 5 bit blue.
// Output size = 400x296x2 x 5 bits + 400x296x1 x 6 bits = 1894400 bits = 236800 bytes = 59200 words = 0xE740 words
// Length of DCMI data to be transferred ***IN WORDS***
// For a 32bit CPU: 1 word = 32 bits = 4 bytes
#define DCMI_MEMORY_LEN 0xE740

DCMI_HandleTypeDef dcmi_handle; // Create DCMI configuration parameters structure
DMA_HandleTypeDef dma_handle;   // Create DMA configuration parameters structure

void Camera_Config(void)
{
}

void DMA_Config(void)
{
    // Enable DMA2 clock
    // __DMA2_CLK_ENABLE();

    // Set parameters in dma_handle structure
    dma_handle.Instance = DMA2_Stream1;
    dma_handle.Init.Channel = DMA_CHANNEL_1;
    dma_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;
    dma_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    dma_handle.Init.MemInc = DMA_MINC_ENABLE;
    dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    dma_handle.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    dma_handle.Init.Mode = DMA_CIRCULAR; // or DMA_NORMAL ?
    dma_handle.Init.Priority = DMA_PRIORITY_HIGH;
    dma_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    dma_handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    dma_handle.Init.MemBurst = DMA_MBURST_SINGLE;
    dma_handle.Init.PeriphBurst = DMA_PBURST_SINGLE;

    // Need to use this function to initialize DMA2_Stream1 channel1.
    // Example: HAL_DMA_Init(DMA_HandleTypeDef *dma_handle);
    HAL_DMA_Init(&dma_handle);
}

void DCMI_Config(void)
{
    // Set parameters in dcmi_handle structure
    dcmi_handle.Instance = DCMI;
    dcmi_handle.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;    // Synchronization mode
    dcmi_handle.Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;  // Pixel clock polarity
    dcmi_handle.Init.VSPolarity = DCMI_VSPOLARITY_HIGH;      // Vertical polarity
    dcmi_handle.Init.HSPolarity = DCMI_HSPOLARITY_HIGH;      // Horizontal polarity
    dcmi_handle.Init.CaptureRate = DCMI_CR_ALL_FRAME;        // Capture rate
    dcmi_handle.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B; // Data width
    dcmi_handle.Init.JPEGMode = DCMI_JPEG_DISABLE;           // JPEG mode

    HAL_DCMI_Init(&dcmi_handle); // Initialize DCMI with given parameter structure
}

int main(void)
{
    HAL_DCMI_Start_DMA(&dcmi_handle, DCMI_MODE_SNAPSHOT, DCMI_MEMORY_LOC, DCMI_MEMORY_LEN); // Enable DCMI, DMA and capture a frame
}