/**
 * Firmware for Bair Hockey image processing and motor control
 * Written using MBED OS 5
 * Running on NUCLEO-F767Z1 Arm Cortex M7 microcontroller
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

// Output resolution = 400x296x3 x 8 bits = 355200 bytes = 0x56B80 bytes
// Length of DCMI data to be transferred ***IN WORDS*** ??? MAYBE how tf do you specify this??
// For a 32bit CPU: 1 word = 32 bits = 4 bytes
#define DCMI_MEMORY_LEN NULL

DCMI_HandleTypeDef hdcmi; // Create DCMI configuration parameters structure
DMA_HandleTypeDef hdma;   // Create DMA configuration parameters structure

void camera_init()
{

    //--------------HDCMI Setup------------------------------------------------------
    // Set parameters in hdcmi structure
    hdcmi.Instance = DCMI;                             // Instance
    hdcmi.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;    // Synchronization mode
    hdcmi.Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;  // Pixel clock polarity
    hdcmi.Init.VSPolarity = DCMI_VSPOLARITY_LOW;       // Vertical polarity
    hdcmi.Init.HSPolarity = DCMI_HSPOLARITY_HIGH;      // Horizontal polarity
    hdcmi.Init.CaptureRate = DCMI_CR_ALL_FRAME;        // Capture rate
    hdcmi.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B; // Data width
    hdcmi.Init.JPEGMode = DCMI_JPEG_DISABLE;           // JPEG mode

    HAL_DCMI_Init(&hdcmi); // Initialize DCMI with given parameter structure
    //---------------------------------------------------------------------------

    //--------------------HDMA Setup-----------------------------------------
    // Set parameters in hdma structure
    hdma.Instance = DMA2_Stream1;
    hdma.Init.Channel = DMA_CHANNEL_1;
    hdma.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma.Init.MemInc = DMA_MINC_ENABLE;
    hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma.Init.Mode = DMA_NORMAL;
    hdma.Init.Priority = DMA_PRIORITY_LOW;
    hdma.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

    // Need to use this function to initialize DMA2_Stream1 channel1.
    // Something like: HAL_DMA_Init(DMA_HandleTypeDef *hdma);
    HAL_DMA_Init(&hdma);
    //----------------------------------------------------------------------

    HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_SNAPSHOT, DCMI_MEMORY_LOC, DCMI_MEMORY_LEN); // Enable DCMI DMA and enable capture
}

int main()
{
    camera_init(); // Run camera initialization
}