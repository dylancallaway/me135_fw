#include "project.h"
#include "stdlib.h"

/************* PIN DEFINITONS **************
Motor 1 Step: Reg 5
Motor 1 Direction: Reg 1, 0 is pos, 1 is neg
Motor 1 Sleep: Reg 4
********************************************
Motor 2 Step: Reg 6
Motor 2 Direction: Reg 2, 0 is pos, 1 is neg
Motor 2 Sleep: Reg 4
********************************************
LED: Reg 3
*******************************************/
/******************************************/

// Gantry MAX travels
// 1200 steps

// Camera coord MAX
// X = 8 to X = 139
// Y = 3 to Y = 175

#define steps_per_pixel 8.14

/************ FUNCTION PROTOTYPES *******************/
int printf(const char * format, ...);
double fabs(double);
double floor(double);
/**************************************************/

/************ UART RX INTERRUPT *********************/
volatile int uart_recv_buf[4];
volatile int uart_recv_count = 0;
int x_1 = 0; // New point
int y_1 = 0;

CY_ISR(isr_rx)
{
    uart_recv_buf[uart_recv_count] = UART_GetChar();
    uart_recv_count += 1;

    if (uart_recv_count > 3)
    {
        x_1 = (uart_recv_buf[1] << 8) + uart_recv_buf[0]; // new coords
        y_1 = (uart_recv_buf[3] << 8) + uart_recv_buf[2];
        uart_recv_count = 0;
    }
}
/*************************************************/

/************** MOTOR PULSE INTERRUPT ***************/
float x_0 = 0, y_0 = 0, dx = 0, dy = 0; // always start at (0, 0)
float m1_steps = 0, m2_steps = 0;
int m1_dir = 0, m2_dir = 0;
int m1_dir_pin = 0, m2_dir_pin = 0;
int m1_step_pin = 0, m2_step_pin = 0;

CY_ISR(pulse_ready_isr)
{
    Control_Reg_5_Write(m1_step_pin); // Motor step
    Control_Reg_6_Write(m2_step_pin);
    
    x_0 += -((float)m1_dir + (float)m2_dir) / (2 * steps_per_pixel);
    y_0 += ((float)m1_dir - (float)m2_dir) / (2 * steps_per_pixel);
    
    Control_Reg_5_Write(0); // Motor reset
    Control_Reg_6_Write(0);
}    
   
/***********************************************/

/****************** (0, 0) ROUTINE ****************/
CY_ISR(pos_reset)
{
    x_1 = 0;
    y_1 = 0;
    x_0 = 0;
    y_0 = 0;
    Pin_1_ClearInterrupt();
}
/************************************************/

int main(void)
{
    CyDelay(1000); // Wait 1 second for "safety"
    
    UART_Start(); // Obvi must call before next line
    printf("Program started...");

    /****************** PWM INIT **************************/
    /* Must happen before PULSE CLOCK INIT or might miss pulses */
    PWM_1_Start();
    PWM_2_Start();
    
    /****************** PULSE CLOCK INIT *********************/
    /* The period between motor pulses is set according to:
    PULSE_PERIOD = BUS_CLK_FREQUENCY_HZ * DESIRED_PERIOD_SEC */
    Timer_1_Start(); // PULSE_PERIOD is set in Timer_1 block
    pulse_ready_isr_StartEx(pulse_ready_isr);
    pulse_ready_isr_Enable();
    /*********************************************************/
    
    /****************** UART INTERRUPT INIT *********************/
    isr_rx_StartEx(isr_rx); /* set up UART interrupt vector*/
    isr_rx_Enable();        /* arm the UART interrupt*/
    /**************************************************/
    
    /********************** (0, 0) INIT ***********************/
    pos_reset_StartEx(pos_reset);
    pos_reset_Enable();
    /************************************************/
    
    CyGlobalIntEnable;      // Enable global interrupts
    
    Control_Reg_3_Write(0); // Start with LED off

    while (1)
    {
        // printf("\t(%.2f, %.2f)\t", x_0, y_0);
        dx = (float)x_1 - x_0;
        dy = (float)y_1 - y_0;
        if ( (dx != 0 && x_1 <= 131) || (dy != 0 && y_1 <= 100) )
        {   
            Control_Reg_4_Write(m1_step_pin || m2_step_pin); // Motor wake/sleep
            Control_Reg_3_Write(1); // LED on
            
            m1_steps = -(dx - dy) * steps_per_pixel; // Negative because 0 is pos rotation
            m2_steps = -(dx + dy) * steps_per_pixel; // and 1 is neg rotation (use RHR)
                
            m1_dir = m1_steps / fabs(m1_steps); // Dir for increment; -1, 0, 1
            m2_dir = m2_steps / fabs(m2_steps);
            
            m1_dir_pin = 1 == m1_dir; // Dir pin; 0, 1
            m2_dir_pin = 1 == m2_dir;
            
            m1_step_pin = 1 == m1_dir || -1 == m1_dir; // Step pin; 0, 1
            m2_step_pin = 1 == m2_dir || -1 == m2_dir;
            
            // Dir pins
            Control_Reg_1_Write(m1_dir_pin);
            Control_Reg_2_Write(m2_dir_pin);

            #define FP_TOL 0.06 // Fixes floating point imprecision
            if (x_0 > (float)x_1 - FP_TOL && x_0 < (float)x_1 + FP_TOL)
            {
                x_0 = x_1;
            }
            if (y_0 > (float)y_1 - FP_TOL && y_0 < (float)y_1 + FP_TOL)
            {
                y_0 = y_1;
            }
        }
        else
        {
            m1_dir = 0; m2_dir = 0;
            m1_step_pin = 0, m2_step_pin = 0;
            Control_Reg_4_Write(0); // Motor sleep
            Control_Reg_3_Write(0); // LED off if no movement
        }
    }
}

