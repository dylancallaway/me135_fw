#include "project.h"
#include "stdlib.h"
//#include<stdio.h>
//#include<time.h>

//CY_ISR(isr_1) /* Button press stop Interrupt*/
//{
//    PWM_1_WritePeriod(0);
//    PWM_1_WriteCompare(0);
//    PWM_2_WritePeriod(0);
//    PWM_2_WriteCompare(0);
//    //UART_PutString("Button pressed");
//    CyDelay(1000);
//}

// UART variable definition
int get[4];
int x2_16;
int y2_16;
float rev_sec = 6; /* INPUT: Change this to change motor speed. 11 max without load */
int ramp_time = 0; /*ms*/
int ramp_steps = 0;
int clock = 50000;           /* Make this the same pmw clock in TopDesign */
float motor_steps_rev = 200; /* motor property (steps/rev) */
int period = 4;              //clock/(rev_sec*motor_steps_rev);
//int sprocket_circumference = 64; /* mm/rev */

int x1 = 0; /* OLD COORDINATE */
int y1 = 0; /* OLD COORDINATE */

// Board is ~1000x1000 steps
int count1 = 1;

CY_ISR(isr_rx) /* UART receive Interrupt*/
{
    int buf_size = UART_GetRxBufferSize();
    if (buf_size)
    {
        //printf("Buffer Size: %d\n", buf_size);
        for (int i = 0; i < 4; i++)
        {
            get[i] = UART_GetChar();
        }
        x2_16 = (get[1] << 8) + get[0]; // X coordinate
        y2_16 = (get[3] << 8) + get[2]; // Y coordinate
                                        //printf("Received Data: %d, %d\n", x2_16, y2_16);

        int x2 = x2_16;        // NEW COORDINATE
        int y2 = y2_16;        // NEW COORDINATE
        int steps_x = x2 - x1; /*  +ve: M1+ M2+, -ve: M1- M2-*/
        int steps_y = y2 - y1; /*  +ve: M1- M2+, -ve: M1+ M2-*/
        int steps_M1;          // steps for motor 1
        int steps_M2;          // steps for motor 2

        if (x2 > 1000)
        {              //ensures motion within what can physically be dne without hitting end
            x2 = 1000; //ensures motion within what can physically be dne without hitting end
        }
        if (y2 > 1000)
        {
            y2 = 1000;
        }

        steps_x = x2 - x1; /*  +ve: M1+ M2+, -ve: M1- M2-*/
        steps_y = y2 - y1; /*  +ve: M1- M2+, -ve: M1+ M2-*/
        CyDelay(20);       ///needed to ensure no skips, so that coordinates match
            //printf("Stepsx, Stepsy: %d, %d\n", steps_x, steps_y); // For debugging comment out

        ////        x2_16 = 1; //UART TEST HEARTBEAT COMMENT OUT
        ////    // UART receive turns on LED
        ////    if (x2_16 != 0)
        ////    {
        ////        Control_Reg_3_Write(1); /*  received, LED on */
        ////    }
        ////    else
        ////    {
        ////        Control_Reg_3_Write(0); /* received, turn LED off */
        ////    }

        // Direction definitions:
        if (steps_x >= 0 && steps_y >= 0)
        {
            steps_M1 = abs(steps_x) - abs(steps_y);
            steps_M2 = abs(steps_x) + abs(steps_y);
        }
        else if (steps_x <= 0 && steps_y >= 0)
        {
            steps_M1 = -abs(steps_x) - abs(steps_y);
            steps_M2 = -abs(steps_x) + abs(steps_y);
        }
        else if (steps_x >= 0 && steps_y <= 0)
        {
            steps_M1 = abs(steps_x) + abs(steps_y);
            steps_M2 = abs(steps_x) - abs(steps_y);
        }
        else if (steps_x <= 0 && steps_y <= 0)
        {
            steps_M1 = -abs(steps_x) + abs(steps_y);
            steps_M2 = -abs(steps_x) - abs(steps_y);
        }

        //printf("StepsM1, StepsM2: %d, %d\n", steps_M1, steps_M2); //for ddebugging comment out

        // Time for each motor to move to move certain amount of steps, at given rev/sec, including ramp (if enabled)
        float time_M1 = (float)(abs(steps_M1)) * (1 / rev_sec) * (1 / motor_steps_rev); /* sec */
        float time_M2 = (float)(abs(steps_M2)) * (1 / rev_sec) * (1 / motor_steps_rev); /* sec */
        //printf("TimeM1, TimeM2: %.2f, %.2f\n", time_M1, time_M2); //for ddebugging comment out

        /* Direction specifications Motor 1*/
        if (steps_M1 > 0)
        {
            Control_Reg_1_Write(0);
        }
        else
        {
            Control_Reg_1_Write(1);
        }

        /* Direction specifications Motor 2*/
        if (steps_M2 > 0)
        {
            Control_Reg_2_Write(0);
        }
        else
        {
            Control_Reg_2_Write(1);
        }

        /* motion with ramp: */

        for (count1 = 1; count1 <= ramp_steps; count1++) //ramping function
        {
            Control_Reg_4_Write(1); // wake up motors
            Control_Reg_3_Write(1); /// turn on LED
            PWM_1_WritePeriod(period * (3.8 - 0.2 * count1));
            PWM_1_WriteCompare(period * 0.5 * (3.8 - 0.2 * count1));
            PWM_2_WritePeriod(period * 3);
            PWM_2_WriteCompare(period * 1.5);
            CyDelay(ramp_time); /*delay in ms*/
        }

        // Move each motor by the time it needs to move certain steps, accounting for difference in time for each motor
        if (count1 == ramp_steps + 1)
        {

            if (time_M1 == time_M2)
            {
                if (time_M1 != 0 || time_M2 != 0)
                {
                    Control_Reg_4_Write(1); // wake up motors
                    Control_Reg_3_Write(1); /// turn on LED
                    PWM_1_WritePeriod(period);
                    PWM_1_WriteCompare(period * .5);
                    PWM_2_WritePeriod(period);
                    PWM_2_WriteCompare(period * .5);
                    CyDelay((time_M1 * 1000) - ramp_steps * ramp_time); /* multiply by 1000 to make delay in ms*/
                }
            }
            else if (time_M1 > time_M2)
            {
                Control_Reg_4_Write(1); // wake up motors
                Control_Reg_3_Write(1); /// turn on LED
                PWM_1_WritePeriod(period);
                PWM_1_WriteCompare(period * .5);
                PWM_2_WritePeriod(period);
                PWM_2_WriteCompare(period * .5);
                CyDelay((time_M2 * 1000) - ramp_steps * ramp_time); /* multiply by 1000 to make delay in ms*/
                PWM_2_WritePeriod(0);
                PWM_2_WriteCompare(0);
                CyDelay(((time_M1 - time_M2) * 1000) - ramp_steps * ramp_time); /* move longer motor by remainder*/
            }
            else if (time_M2 > time_M1)
            {
                Control_Reg_4_Write(1); // wake up motors
                Control_Reg_3_Write(1); /// turn on LED
                PWM_1_WritePeriod(period);
                PWM_1_WriteCompare(period * .5);
                PWM_2_WritePeriod(period);
                PWM_2_WriteCompare(period * .5);
                CyDelay((time_M1 * 1000) - ramp_steps * ramp_time); /* multiply by 1000 to make delay in ms*/
                PWM_1_WritePeriod(0);
                PWM_1_WriteCompare(0);
                CyDelay(((time_M2 - time_M1) * 1000) - ramp_steps * ramp_time); /* move longer motor by remainder*/
            }

            count1 = count1 + 1;
        }
        else
        {
            // Stop motors
            PWM_1_WritePeriod(0);
            PWM_1_WriteCompare(0);
            PWM_2_WritePeriod(0);
            PWM_2_WriteCompare(0);
        }

        // Stop motors
        PWM_1_WritePeriod(0);
        PWM_1_WriteCompare(0);
        PWM_2_WritePeriod(0);
        PWM_2_WriteCompare(0);
        // Motion complete, set new cordinate as old coordinate
        x1 = x2;
        y1 = y2;

        // Sleep motors:
        Control_Reg_4_Write(0);
        Control_Reg_3_Write(0); ///HEARTBEAT TEST COMMENT OUT

        x2 = 0; // TEST. coordinate to go to next
        y2 = 0; //TEST
    }
}
}
// END OF UART receive INTERRUPT

int main(void)
{
    //    isr_1_Start(); /* Start Interrupt*/
    //    isr_1_StartEx(isr_1);  /* set up interrupt vector*/
    //    isr_1_Enable();  /* arm the interrupt*/

    isr_rx_Start();         /* Start UART Interrupt*/
    isr_rx_StartEx(isr_rx); /* set up UART interrupt vector*/
    isr_rx_Enable();        /* arm the UART interrupt*/

    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Initialization/startup code */

    UART_Start();
    PWM_1_Start();
    PWM_1_WritePeriod(0);
    PWM_1_WriteCompare(0);
    PWM_2_Start();
    PWM_2_WritePeriod(0);
    PWM_2_WriteCompare(0);
    Control_Reg_3_Write(0); // Start with LED off

    /* Maximum Motor speed: */ // ramp that has worked: 6,0,0

    // TEST COORDINATES (comment out):
    //x2=1000;
    //y2=800;

    /* Forever loop: */
    for (;;)
    {

    } /*end of forever loop */
}
