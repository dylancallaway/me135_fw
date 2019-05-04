/*******************************************************************************
* File Name: pulse_ready.h
* Version 1.70
*
*  Description:
*   Provides the function definitions for the Interrupt Controller.
*
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/
#if !defined(CY_ISR_pulse_ready_H)
#define CY_ISR_pulse_ready_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void pulse_ready_Start(void);
void pulse_ready_StartEx(cyisraddress address);
void pulse_ready_Stop(void);

CY_ISR_PROTO(pulse_ready_Interrupt);

void pulse_ready_SetVector(cyisraddress address);
cyisraddress pulse_ready_GetVector(void);

void pulse_ready_SetPriority(uint8 priority);
uint8 pulse_ready_GetPriority(void);

void pulse_ready_Enable(void);
uint8 pulse_ready_GetState(void);
void pulse_ready_Disable(void);

void pulse_ready_SetPending(void);
void pulse_ready_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the pulse_ready ISR. */
#define pulse_ready_INTC_VECTOR            ((reg32 *) pulse_ready__INTC_VECT)

/* Address of the pulse_ready ISR priority. */
#define pulse_ready_INTC_PRIOR             ((reg8 *) pulse_ready__INTC_PRIOR_REG)

/* Priority of the pulse_ready interrupt. */
#define pulse_ready_INTC_PRIOR_NUMBER      pulse_ready__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable pulse_ready interrupt. */
#define pulse_ready_INTC_SET_EN            ((reg32 *) pulse_ready__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the pulse_ready interrupt. */
#define pulse_ready_INTC_CLR_EN            ((reg32 *) pulse_ready__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the pulse_ready interrupt state to pending. */
#define pulse_ready_INTC_SET_PD            ((reg32 *) pulse_ready__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the pulse_ready interrupt. */
#define pulse_ready_INTC_CLR_PD            ((reg32 *) pulse_ready__INTC_CLR_PD_REG)


#endif /* CY_ISR_pulse_ready_H */


/* [] END OF FILE */
