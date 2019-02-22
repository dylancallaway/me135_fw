/**
 * Firmware for Bair Hockey image processing and motor control
 * Written using MBED OS 5
 * Running on NUCLEO-F767Z1 Arm Cortex M7 microcontroller
 * Authors: Dylan Callaway
**/

#include "mbed.h"

// STM32F7 DCMI driver
#include "stm32f7xx_hal_dcmi.h"

DCMI_HandleTypeDef hdcmi;

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
void camera_init()
{
    /**
     * Program the required configuration through following parameters:
     * horizontal and vertical polarity, pixel clock polarity, Capture Rate,
     * Synchronization Mode, code of the frame delimiter and data width 
     * using HAL_DCMI_Init() function.
    **/
    hdcmi.Instance = DCMI;
    hdcmi.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE; // Synchronization mode
    hdcmi.Init.PCKPolarity = DCMI_PCKPOLARITY_RISING; // Pixel clock polarity
    hdcmi.Init.VSPolarity = DCMI_VSPOLARITY_LOW; // Vertical polarity
    hdcmi.Init.HSPolarity = DCMI_HSPOLARITY_HIGH; // Horizontal polarity
    hdcmi.Init.CaptureRate = DCMI_CR_ALL_FRAME; // Capture rate
    hdcmi.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B; // Data width
    hdcmi.Init.JPEGMode = DCMI_JPEG_DISABLE; // JPEG mode

    HAL_DCMI_Init(&hdcmi);
}

int main()
{
    camera_init();
}