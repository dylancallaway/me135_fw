/*******************************************************************************
* File Name: pos_reset.h
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
#if !defined(CY_ISR_pos_reset_H)
#define CY_ISR_pos_reset_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void pos_reset_Start(void);
void pos_reset_StartEx(cyisraddress address);
void pos_reset_Stop(void);

CY_ISR_PROTO(pos_reset_Interrupt);

void pos_reset_SetVector(cyisraddress address);
cyisraddress pos_reset_GetVector(void);

void pos_reset_SetPriority(uint8 priority);
uint8 pos_reset_GetPriority(void);

void pos_reset_Enable(void);
uint8 pos_reset_GetState(void);
void pos_reset_Disable(void);

void pos_reset_SetPending(void);
void pos_reset_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the pos_reset ISR. */
#define pos_reset_INTC_VECTOR            ((reg32 *) pos_reset__INTC_VECT)

/* Address of the pos_reset ISR priority. */
#define pos_reset_INTC_PRIOR             ((reg8 *) pos_reset__INTC_PRIOR_REG)

/* Priority of the pos_reset interrupt. */
#define pos_reset_INTC_PRIOR_NUMBER      pos_reset__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable pos_reset interrupt. */
#define pos_reset_INTC_SET_EN            ((reg32 *) pos_reset__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the pos_reset interrupt. */
#define pos_reset_INTC_CLR_EN            ((reg32 *) pos_reset__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the pos_reset interrupt state to pending. */
#define pos_reset_INTC_SET_PD            ((reg32 *) pos_reset__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the pos_reset interrupt. */
#define pos_reset_INTC_CLR_PD            ((reg32 *) pos_reset__INTC_CLR_PD_REG)


#endif /* CY_ISR_pos_reset_H */


/* [] END OF FILE */
