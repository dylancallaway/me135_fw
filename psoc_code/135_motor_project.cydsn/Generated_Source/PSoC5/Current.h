/*******************************************************************************
* File Name: Current.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_Current_H) /* Pins Current_H */
#define CY_PINS_Current_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Current_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Current__PORT == 15 && ((Current__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Current_Write(uint8 value);
void    Current_SetDriveMode(uint8 mode);
uint8   Current_ReadDataReg(void);
uint8   Current_Read(void);
void    Current_SetInterruptMode(uint16 position, uint16 mode);
uint8   Current_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Current_SetDriveMode() function.
     *  @{
     */
        #define Current_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Current_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Current_DM_RES_UP          PIN_DM_RES_UP
        #define Current_DM_RES_DWN         PIN_DM_RES_DWN
        #define Current_DM_OD_LO           PIN_DM_OD_LO
        #define Current_DM_OD_HI           PIN_DM_OD_HI
        #define Current_DM_STRONG          PIN_DM_STRONG
        #define Current_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Current_MASK               Current__MASK
#define Current_SHIFT              Current__SHIFT
#define Current_WIDTH              1u

/* Interrupt constants */
#if defined(Current__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Current_SetInterruptMode() function.
     *  @{
     */
        #define Current_INTR_NONE      (uint16)(0x0000u)
        #define Current_INTR_RISING    (uint16)(0x0001u)
        #define Current_INTR_FALLING   (uint16)(0x0002u)
        #define Current_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Current_INTR_MASK      (0x01u) 
#endif /* (Current__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Current_PS                     (* (reg8 *) Current__PS)
/* Data Register */
#define Current_DR                     (* (reg8 *) Current__DR)
/* Port Number */
#define Current_PRT_NUM                (* (reg8 *) Current__PRT) 
/* Connect to Analog Globals */                                                  
#define Current_AG                     (* (reg8 *) Current__AG)                       
/* Analog MUX bux enable */
#define Current_AMUX                   (* (reg8 *) Current__AMUX) 
/* Bidirectional Enable */                                                        
#define Current_BIE                    (* (reg8 *) Current__BIE)
/* Bit-mask for Aliased Register Access */
#define Current_BIT_MASK               (* (reg8 *) Current__BIT_MASK)
/* Bypass Enable */
#define Current_BYP                    (* (reg8 *) Current__BYP)
/* Port wide control signals */                                                   
#define Current_CTL                    (* (reg8 *) Current__CTL)
/* Drive Modes */
#define Current_DM0                    (* (reg8 *) Current__DM0) 
#define Current_DM1                    (* (reg8 *) Current__DM1)
#define Current_DM2                    (* (reg8 *) Current__DM2) 
/* Input Buffer Disable Override */
#define Current_INP_DIS                (* (reg8 *) Current__INP_DIS)
/* LCD Common or Segment Drive */
#define Current_LCD_COM_SEG            (* (reg8 *) Current__LCD_COM_SEG)
/* Enable Segment LCD */
#define Current_LCD_EN                 (* (reg8 *) Current__LCD_EN)
/* Slew Rate Control */
#define Current_SLW                    (* (reg8 *) Current__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Current_PRTDSI__CAPS_SEL       (* (reg8 *) Current__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Current_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Current__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Current_PRTDSI__OE_SEL0        (* (reg8 *) Current__PRTDSI__OE_SEL0) 
#define Current_PRTDSI__OE_SEL1        (* (reg8 *) Current__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Current_PRTDSI__OUT_SEL0       (* (reg8 *) Current__PRTDSI__OUT_SEL0) 
#define Current_PRTDSI__OUT_SEL1       (* (reg8 *) Current__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Current_PRTDSI__SYNC_OUT       (* (reg8 *) Current__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Current__SIO_CFG)
    #define Current_SIO_HYST_EN        (* (reg8 *) Current__SIO_HYST_EN)
    #define Current_SIO_REG_HIFREQ     (* (reg8 *) Current__SIO_REG_HIFREQ)
    #define Current_SIO_CFG            (* (reg8 *) Current__SIO_CFG)
    #define Current_SIO_DIFF           (* (reg8 *) Current__SIO_DIFF)
#endif /* (Current__SIO_CFG) */

/* Interrupt Registers */
#if defined(Current__INTSTAT)
    #define Current_INTSTAT            (* (reg8 *) Current__INTSTAT)
    #define Current_SNAP               (* (reg8 *) Current__SNAP)
    
	#define Current_0_INTTYPE_REG 		(* (reg8 *) Current__0__INTTYPE)
#endif /* (Current__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Current_H */


/* [] END OF FILE */
