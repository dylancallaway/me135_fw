/*******************************************************************************
* File Name: Sleep1.h  
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

#if !defined(CY_PINS_Sleep1_H) /* Pins Sleep1_H */
#define CY_PINS_Sleep1_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Sleep1_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Sleep1__PORT == 15 && ((Sleep1__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Sleep1_Write(uint8 value);
void    Sleep1_SetDriveMode(uint8 mode);
uint8   Sleep1_ReadDataReg(void);
uint8   Sleep1_Read(void);
void    Sleep1_SetInterruptMode(uint16 position, uint16 mode);
uint8   Sleep1_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Sleep1_SetDriveMode() function.
     *  @{
     */
        #define Sleep1_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Sleep1_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Sleep1_DM_RES_UP          PIN_DM_RES_UP
        #define Sleep1_DM_RES_DWN         PIN_DM_RES_DWN
        #define Sleep1_DM_OD_LO           PIN_DM_OD_LO
        #define Sleep1_DM_OD_HI           PIN_DM_OD_HI
        #define Sleep1_DM_STRONG          PIN_DM_STRONG
        #define Sleep1_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Sleep1_MASK               Sleep1__MASK
#define Sleep1_SHIFT              Sleep1__SHIFT
#define Sleep1_WIDTH              1u

/* Interrupt constants */
#if defined(Sleep1__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Sleep1_SetInterruptMode() function.
     *  @{
     */
        #define Sleep1_INTR_NONE      (uint16)(0x0000u)
        #define Sleep1_INTR_RISING    (uint16)(0x0001u)
        #define Sleep1_INTR_FALLING   (uint16)(0x0002u)
        #define Sleep1_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Sleep1_INTR_MASK      (0x01u) 
#endif /* (Sleep1__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Sleep1_PS                     (* (reg8 *) Sleep1__PS)
/* Data Register */
#define Sleep1_DR                     (* (reg8 *) Sleep1__DR)
/* Port Number */
#define Sleep1_PRT_NUM                (* (reg8 *) Sleep1__PRT) 
/* Connect to Analog Globals */                                                  
#define Sleep1_AG                     (* (reg8 *) Sleep1__AG)                       
/* Analog MUX bux enable */
#define Sleep1_AMUX                   (* (reg8 *) Sleep1__AMUX) 
/* Bidirectional Enable */                                                        
#define Sleep1_BIE                    (* (reg8 *) Sleep1__BIE)
/* Bit-mask for Aliased Register Access */
#define Sleep1_BIT_MASK               (* (reg8 *) Sleep1__BIT_MASK)
/* Bypass Enable */
#define Sleep1_BYP                    (* (reg8 *) Sleep1__BYP)
/* Port wide control signals */                                                   
#define Sleep1_CTL                    (* (reg8 *) Sleep1__CTL)
/* Drive Modes */
#define Sleep1_DM0                    (* (reg8 *) Sleep1__DM0) 
#define Sleep1_DM1                    (* (reg8 *) Sleep1__DM1)
#define Sleep1_DM2                    (* (reg8 *) Sleep1__DM2) 
/* Input Buffer Disable Override */
#define Sleep1_INP_DIS                (* (reg8 *) Sleep1__INP_DIS)
/* LCD Common or Segment Drive */
#define Sleep1_LCD_COM_SEG            (* (reg8 *) Sleep1__LCD_COM_SEG)
/* Enable Segment LCD */
#define Sleep1_LCD_EN                 (* (reg8 *) Sleep1__LCD_EN)
/* Slew Rate Control */
#define Sleep1_SLW                    (* (reg8 *) Sleep1__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Sleep1_PRTDSI__CAPS_SEL       (* (reg8 *) Sleep1__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Sleep1_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Sleep1__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Sleep1_PRTDSI__OE_SEL0        (* (reg8 *) Sleep1__PRTDSI__OE_SEL0) 
#define Sleep1_PRTDSI__OE_SEL1        (* (reg8 *) Sleep1__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Sleep1_PRTDSI__OUT_SEL0       (* (reg8 *) Sleep1__PRTDSI__OUT_SEL0) 
#define Sleep1_PRTDSI__OUT_SEL1       (* (reg8 *) Sleep1__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Sleep1_PRTDSI__SYNC_OUT       (* (reg8 *) Sleep1__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Sleep1__SIO_CFG)
    #define Sleep1_SIO_HYST_EN        (* (reg8 *) Sleep1__SIO_HYST_EN)
    #define Sleep1_SIO_REG_HIFREQ     (* (reg8 *) Sleep1__SIO_REG_HIFREQ)
    #define Sleep1_SIO_CFG            (* (reg8 *) Sleep1__SIO_CFG)
    #define Sleep1_SIO_DIFF           (* (reg8 *) Sleep1__SIO_DIFF)
#endif /* (Sleep1__SIO_CFG) */

/* Interrupt Registers */
#if defined(Sleep1__INTSTAT)
    #define Sleep1_INTSTAT            (* (reg8 *) Sleep1__INTSTAT)
    #define Sleep1_SNAP               (* (reg8 *) Sleep1__SNAP)
    
	#define Sleep1_0_INTTYPE_REG 		(* (reg8 *) Sleep1__0__INTTYPE)
#endif /* (Sleep1__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Sleep1_H */


/* [] END OF FILE */
