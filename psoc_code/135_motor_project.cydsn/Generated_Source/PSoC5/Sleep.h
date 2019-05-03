/*******************************************************************************
* File Name: Sleep.h  
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

#if !defined(CY_PINS_Sleep_H) /* Pins Sleep_H */
#define CY_PINS_Sleep_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Sleep_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Sleep__PORT == 15 && ((Sleep__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Sleep_Write(uint8 value);
void    Sleep_SetDriveMode(uint8 mode);
uint8   Sleep_ReadDataReg(void);
uint8   Sleep_Read(void);
void    Sleep_SetInterruptMode(uint16 position, uint16 mode);
uint8   Sleep_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Sleep_SetDriveMode() function.
     *  @{
     */
        #define Sleep_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Sleep_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Sleep_DM_RES_UP          PIN_DM_RES_UP
        #define Sleep_DM_RES_DWN         PIN_DM_RES_DWN
        #define Sleep_DM_OD_LO           PIN_DM_OD_LO
        #define Sleep_DM_OD_HI           PIN_DM_OD_HI
        #define Sleep_DM_STRONG          PIN_DM_STRONG
        #define Sleep_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Sleep_MASK               Sleep__MASK
#define Sleep_SHIFT              Sleep__SHIFT
#define Sleep_WIDTH              1u

/* Interrupt constants */
#if defined(Sleep__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Sleep_SetInterruptMode() function.
     *  @{
     */
        #define Sleep_INTR_NONE      (uint16)(0x0000u)
        #define Sleep_INTR_RISING    (uint16)(0x0001u)
        #define Sleep_INTR_FALLING   (uint16)(0x0002u)
        #define Sleep_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Sleep_INTR_MASK      (0x01u) 
#endif /* (Sleep__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Sleep_PS                     (* (reg8 *) Sleep__PS)
/* Data Register */
#define Sleep_DR                     (* (reg8 *) Sleep__DR)
/* Port Number */
#define Sleep_PRT_NUM                (* (reg8 *) Sleep__PRT) 
/* Connect to Analog Globals */                                                  
#define Sleep_AG                     (* (reg8 *) Sleep__AG)                       
/* Analog MUX bux enable */
#define Sleep_AMUX                   (* (reg8 *) Sleep__AMUX) 
/* Bidirectional Enable */                                                        
#define Sleep_BIE                    (* (reg8 *) Sleep__BIE)
/* Bit-mask for Aliased Register Access */
#define Sleep_BIT_MASK               (* (reg8 *) Sleep__BIT_MASK)
/* Bypass Enable */
#define Sleep_BYP                    (* (reg8 *) Sleep__BYP)
/* Port wide control signals */                                                   
#define Sleep_CTL                    (* (reg8 *) Sleep__CTL)
/* Drive Modes */
#define Sleep_DM0                    (* (reg8 *) Sleep__DM0) 
#define Sleep_DM1                    (* (reg8 *) Sleep__DM1)
#define Sleep_DM2                    (* (reg8 *) Sleep__DM2) 
/* Input Buffer Disable Override */
#define Sleep_INP_DIS                (* (reg8 *) Sleep__INP_DIS)
/* LCD Common or Segment Drive */
#define Sleep_LCD_COM_SEG            (* (reg8 *) Sleep__LCD_COM_SEG)
/* Enable Segment LCD */
#define Sleep_LCD_EN                 (* (reg8 *) Sleep__LCD_EN)
/* Slew Rate Control */
#define Sleep_SLW                    (* (reg8 *) Sleep__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Sleep_PRTDSI__CAPS_SEL       (* (reg8 *) Sleep__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Sleep_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Sleep__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Sleep_PRTDSI__OE_SEL0        (* (reg8 *) Sleep__PRTDSI__OE_SEL0) 
#define Sleep_PRTDSI__OE_SEL1        (* (reg8 *) Sleep__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Sleep_PRTDSI__OUT_SEL0       (* (reg8 *) Sleep__PRTDSI__OUT_SEL0) 
#define Sleep_PRTDSI__OUT_SEL1       (* (reg8 *) Sleep__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Sleep_PRTDSI__SYNC_OUT       (* (reg8 *) Sleep__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Sleep__SIO_CFG)
    #define Sleep_SIO_HYST_EN        (* (reg8 *) Sleep__SIO_HYST_EN)
    #define Sleep_SIO_REG_HIFREQ     (* (reg8 *) Sleep__SIO_REG_HIFREQ)
    #define Sleep_SIO_CFG            (* (reg8 *) Sleep__SIO_CFG)
    #define Sleep_SIO_DIFF           (* (reg8 *) Sleep__SIO_DIFF)
#endif /* (Sleep__SIO_CFG) */

/* Interrupt Registers */
#if defined(Sleep__INTSTAT)
    #define Sleep_INTSTAT            (* (reg8 *) Sleep__INTSTAT)
    #define Sleep_SNAP               (* (reg8 *) Sleep__SNAP)
    
	#define Sleep_0_INTTYPE_REG 		(* (reg8 *) Sleep__0__INTTYPE)
#endif /* (Sleep__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Sleep_H */


/* [] END OF FILE */
