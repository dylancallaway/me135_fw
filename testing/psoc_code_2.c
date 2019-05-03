#include "project.h"
#include "stdlib.h"

// UART variable definition
float rev_sec = 6; /* INPUT: Change this to change motor speed. 11 max without load */
int ramp_time = 0; /*ms*/
int ramp_steps = 0;
int clock = 50000;           /* Make this the same pmw clock in TopDesign */
float motor_steps_rev = 200; /* motor property (steps/rev) */
int period = 4;              //clock/(rev_sec*motor_steps_rev);
//int sprocket_circumference = 64; /* mm/rev */

/************ UART RX INTERRUPT ROUTINE *********************/
volatile int uart_recv_buf[4];
volatile int uart_count = 0;
int x1 = 0, y1 = 0;
volatile bool flag = 0;

CY_ISR(isr_rx) /* UART receive Interrupt*/
{
    uart_recv_buf[uart_count] = UART_GetChar();
    uart_count += 1;

    if (uart_count == 4)
    {
        x1 = (uart_recv_buf[1] << 8) + uart_recv_buf[0]; // new coords
        y1 = (uart_recv_buf[3] << 8) + uart_recv_buf[2];
        // printf("Received Data: %d, %d\n", x2, y2);
        flag = 1;
        uart_count = 0;
    }
}

// int dx = x2 - x1; /*  +ve: M1+ M2+, -ve: M1- M2-*/
// int dy = y2 - y1; /*  +ve: M1- M2+, -ve: M1+ M2-*/
// int steps_m1;     // steps for motor 1
// int steps_m2;     // steps for motor 2

// if (x2 > 1000)
// {              //ensures motion within what can physically be dne without hitting end
//     x2 = 1000; //ensures motion within what can physically be dne without hitting end
// }
// if (y2 > 1000)
// {
//     y2 = 1000;
// }

// dx = x2 - x1;                               /*  +ve: M1+ M2+, -ve: M1- M2-*/
// dy = y2 - y1;                               /*  +ve: M1- M2+, -ve: M1+ M2-*/
// CyDelay(200);                               ///needed to ensure no skips, so that coordinates match
// printf("Stepsx, Stepsy: %d, %d\n", dx, dy); // For debugging comment out

// ////        x2_16 = 1; //UART TEST HEARTBEAT COMMENT OUT
// ////    // UART receive turns on LED
// ////    if (x2_16 != 0)
// ////    {
// ////        Control_Reg_3_Write(1); /*  received, LED on */
// ////    }
// ////    else
// ////    {
// ////        Control_Reg_3_Write(0); /* received, turn LED off */
// ////    }

// // Direction definitions:
// if (dx >= 0 && dy >= 0)
// {
//     steps_m1 = abs(dx) - abs(dy);
//     steps_m2 = abs(dx) + abs(dy);
// }
// else if (dx <= 0 && dy >= 0)
// {
//     steps_m1 = -abs(dx) - abs(dy);
//     steps_m2 = -abs(dx) + abs(dy);
// }
// else if (dx >= 0 && dy <= 0)
// {
//     steps_m1 = abs(dx) + abs(dy);
//     steps_m2 = abs(dx) - abs(dy);
// }
// else if (dx <= 0 && dy <= 0)
// {
//     steps_m1 = -abs(dx) + abs(dy);
//     steps_m2 = -abs(dx) - abs(dy);
// }

// printf("StepsM1, StepsM2: %d, %d\n", steps_m1, steps_m2); //for ddebugging comment out

// // Time for each motor to move to move certain amount of steps, at given rev/sec, including ramp (if enabled)
// float time_M1 = (float)(abs(steps_m1)) * (1 / rev_sec) * (1 / motor_steps_rev); /* sec */
// float time_M2 = (float)(abs(steps_m2)) * (1 / rev_sec) * (1 / motor_steps_rev); /* sec */
// printf("TimeM1, TimeM2: %.2f, %.2f\n", time_M1, time_M2);                       //for ddebugging comment out

// /* Direction specifications Motor 1*/
// if (steps_m1 > 0)
// {
//     Control_Reg_1_Write(0);
// }
// else
// {
//     Control_Reg_1_Write(1);
// }

// /* Direction specifications Motor 2*/
// if (steps_m2 > 0)
// {
//     Control_Reg_2_Write(0);
// }
// else
// {
//     Control_Reg_2_Write(1);
// }

// /* motion with ramp: */

// for (count1 = 1; count1 <= ramp_steps; count1++) //ramping function
// {
//     Control_Reg_4_Write(1); // wake up motors
//     Control_Reg_3_Write(1); /// turn on LED
//     PWM_1_WritePeriod(period * (3.8 - 0.2 * count1));
//     PWM_1_WriteCompare(period * 0.5 * (3.8 - 0.2 * count1));
//     PWM_2_WritePeriod(period * 3);
//     PWM_2_WriteCompare(period * 1.5);
//     CyDelay(ramp_time); /*delay in ms*/
// }

// // Move each motor by the time it needs to move certain steps, accounting for difference in time for each motor
// if (count1 == ramp_steps + 1)
// {

//     if (time_M1 == time_M2)
//     {
//         if (time_M1 != 0 || time_M2 != 0)
//         {
//             printf("PMW equal");    //for debugging comment out
//             Control_Reg_4_Write(1); // wake up motors
//             Control_Reg_3_Write(1); /// turn on LED
//             PWM_1_WritePeriod(period);
//             PWM_1_WriteCompare(period * .5);
//             PWM_2_WritePeriod(period);
//             PWM_2_WriteCompare(period * .5);
//             CyDelay((time_M1 * 1000) - ramp_steps * ramp_time); /* multiply by 1000 to make delay in ms*/
//         }
//     }
//     else if (time_M1 > time_M2)
//     {
//         printf("PMW M1>M2");    //for ddebugging comment out
//         Control_Reg_4_Write(1); // wake up motors
//         Control_Reg_3_Write(1); /// turn on LED
//         PWM_1_WritePeriod(period);
//         PWM_1_WriteCompare(period * .5);
//         PWM_2_WritePeriod(period);
//         PWM_2_WriteCompare(period * .5);
//         CyDelay((time_M2 * 1000) - ramp_steps * ramp_time); /* multiply by 1000 to make delay in ms*/
//         PWM_2_WritePeriod(0);
//         PWM_2_WriteCompare(0);
//         CyDelay(((time_M1 - time_M2) * 1000) - ramp_steps * ramp_time); /* move longer motor by remainder*/
//     }
//     else if (time_M2 > time_M1)
//     {
//         printf("PMW M2>M1");    //for ddebugging comment out
//         Control_Reg_4_Write(1); // wake up motors
//         Control_Reg_3_Write(1); /// turn on LED
//         PWM_1_WritePeriod(period);
//         PWM_1_WriteCompare(period * .5);
//         PWM_2_WritePeriod(period);
//         PWM_2_WriteCompare(period * .5);
//         CyDelay((time_M1 * 1000) - ramp_steps * ramp_time); /* multiply by 1000 to make delay in ms*/
//         PWM_1_WritePeriod(0);
//         PWM_1_WriteCompare(0);
//         CyDelay(((time_M2 - time_M1) * 1000) - ramp_steps * ramp_time); /* move longer motor by remainder*/
//     }

//     count1 = count1 + 1;
// }
// else
// {
//     // Stop motors
//     PWM_1_WritePeriod(0);
//     PWM_1_WriteCompare(0);
//     PWM_2_WritePeriod(0);
//     PWM_2_WriteCompare(0);
// }

// // Stop motors
// PWM_1_WritePeriod(0);
// PWM_1_WriteCompare(0);
// PWM_2_WritePeriod(0);
// PWM_2_WriteCompare(0);
// // Motion complete, set new cordinate as old coordinate
// x1 = x2;
// y1 = y2;

// // Sleep motors:
// Control_Reg_4_Write(0);
// Control_Reg_3_Write(0); ///HEARTBEAT TEST COMMENT OUT

// x2 = 0; // TEST. coordinate to go to next
// y2 = 0; //TEST
// }

int main(void)
{
    /*************** SETUP *******************/
    isr_rx_Start();         /* Start UART Interrupt*/
    isr_rx_StartEx(isr_rx); /* set up UART interrupt vector*/
    isr_rx_Enable();        /* arm the UART interrupt*/
    CyGlobalIntEnable;      /* Enable global interrupts. */

    UART_Start();
    Control_Reg_3_Write(0); // Start with LED off
    /******************************************/

    int x0 = 0, y0 = 0; // always start at (0, 0)
    int steps_m1 = 0, steps_m2 = 0;
    int m1_step_count = 0, m2_step_count = 0;
    int m1_dir = 0, m2_dir = 0;
    bool m1_pin = 0, m2_pin = 0;

    while (true)
    {
        // int dx = x1 - x0, dy = y1 - y0;
        Control_Reg_4_Write(1);

        // if (dx >= 0 && dy >= 0)
        // {
        //     steps_m1 = abs(dx) - abs(dy);
        //     steps_m2 = abs(dx) + abs(dy);
        // }
        // else if (dx <= 0 && dy >= 0)
        // {
        //     steps_m1 = -abs(dx) - abs(dy);
        //     steps_m2 = -abs(dx) + abs(dy);
        // }
        // else if (dx >= 0 && dy <= 0)
        // {
        //     steps_m1 = abs(dx) + abs(dy);
        //     steps_m2 = abs(dx) - abs(dy);
        // }
        // else if (dx <= 0 && dy <= 0)
        // {
        //     steps_m1 = -abs(dx) + abs(dy);
        //     steps_m2 = -abs(dx) - abs(dy);
        // }

        // /* Direction specifications Motor 1*/
        // if (steps_m1 > 0)
        // {
        //     Control_Reg_1_Write(0); // Backwards
        //     m1_pin = 1;
        //     m1_dir = -1;
        // }
        // else if (steps_m1 == 0)
        // {
        //     m1_pin = 0;
        //     m1_dir = 0;
        // }
        // else if (steps_m1 < 0)
        // {
        Control_Reg_1_Write(1);
        //     m1_pin = 1;
        //     m1_dir = 1;
        // }

        // /* Direction specifications Motor 2*/
        // if (steps_m2 > 0)
        // {
        //     Control_Reg_2_Write(0);
        //     m2_pin = 1;
        //     m2_dir = -1;
        // }
        // else if (steps_m2 == 0)
        // {
        //     m2_pin = 0;
        //     m2_dir = 0;
        // }
        // else if (steps_m2 < 0)
        // {
        Control_Reg_2_Write(1);
        //     m2_pin = 1;
        //     m2_dir = 1;
        // }

        Control_Reg_5_Write(1); // Motor 1
        Control_Reg_6_Write(1); // Motor 2
        steps_m1 += 1;
        steps_m2 += 1;
        CyDelay(2);
        Control_Reg_5_Write(0); // Motor 1
        Control_Reg_6_Write(0); // Motor 2
        CyDelay(3);
        printf("Motor 1 Steps: %d\t Motor 2 Steps: %d\n", steps_m1, steps_m2);

        // Need to convert back from M1 and M2 steps to X, Y coords
    }
}
