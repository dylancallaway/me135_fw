/*******************************************************************************
* File Name: DIR.h  
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

#if !defined(CY_PINS_DIR_H) /* Pins DIR_H */
#define CY_PINS_DIR_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "DIR_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 DIR__PORT == 15 && ((DIR__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    DIR_Write(uint8 value);
void    DIR_SetDriveMode(uint8 mode);
uint8   DIR_ReadDataReg(void);
uint8   DIR_Read(void);
void    DIR_SetInterruptMode(uint16 position, uint16 mode);
uint8   DIR_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the DIR_SetDriveMode() function.
     *  @{
     */
        #define DIR_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define DIR_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define DIR_DM_RES_UP          PIN_DM_RES_UP
        #define DIR_DM_RES_DWN         PIN_DM_RES_DWN
        #define DIR_DM_OD_LO           PIN_DM_OD_LO
        #define DIR_DM_OD_HI           PIN_DM_OD_HI
        #define DIR_DM_STRONG          PIN_DM_STRONG
        #define DIR_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define DIR_MASK               DIR__MASK
#define DIR_SHIFT              DIR__SHIFT
#define DIR_WIDTH              1u

/* Interrupt constants */
#if defined(DIR__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in DIR_SetInterruptMode() function.
     *  @{
     */
        #define DIR_INTR_NONE      (uint16)(0x0000u)
        #define DIR_INTR_RISING    (uint16)(0x0001u)
        #define DIR_INTR_FALLING   (uint16)(0x0002u)
        #define DIR_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define DIR_INTR_MASK      (0x01u) 
#endif /* (DIR__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define DIR_PS                     (* (reg8 *) DIR__PS)
/* Data Register */
#define DIR_DR                     (* (reg8 *) DIR__DR)
/* Port Number */
#define DIR_PRT_NUM                (* (reg8 *) DIR__PRT) 
/* Connect to Analog Globals */                                                  
#define DIR_AG                     (* (reg8 *) DIR__AG)                       
/* Analog MUX bux enable */
#define DIR_AMUX                   (* (reg8 *) DIR__AMUX) 
/* Bidirectional Enable */                                                        
#define DIR_BIE                    (* (reg8 *) DIR__BIE)
/* Bit-mask for Aliased Register Access */
#define DIR_BIT_MASK               (* (reg8 *) DIR__BIT_MASK)
/* Bypass Enable */
#define DIR_BYP                    (* (reg8 *) DIR__BYP)
/* Port wide control signals */                                                   
#define DIR_CTL                    (* (reg8 *) DIR__CTL)
/* Drive Modes */
#define DIR_DM0                    (* (reg8 *) DIR__DM0) 
#define DIR_DM1                    (* (reg8 *) DIR__DM1)
#define DIR_DM2                    (* (reg8 *) DIR__DM2) 
/* Input Buffer Disable Override */
#define DIR_INP_DIS                (* (reg8 *) DIR__INP_DIS)
/* LCD Common or Segment Drive */
#define DIR_LCD_COM_SEG            (* (reg8 *) DIR__LCD_COM_SEG)
/* Enable Segment LCD */
#define DIR_LCD_EN                 (* (reg8 *) DIR__LCD_EN)
/* Slew Rate Control */
#define DIR_SLW                    (* (reg8 *) DIR__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define DIR_PRTDSI__CAPS_SEL       (* (reg8 *) DIR__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define DIR_PRTDSI__DBL_SYNC_IN    (* (reg8 *) DIR__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define DIR_PRTDSI__OE_SEL0        (* (reg8 *) DIR__PRTDSI__OE_SEL0) 
#define DIR_PRTDSI__OE_SEL1        (* (reg8 *) DIR__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define DIR_PRTDSI__OUT_SEL0       (* (reg8 *) DIR__PRTDSI__OUT_SEL0) 
#define DIR_PRTDSI__OUT_SEL1       (* (reg8 *) DIR__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define DIR_PRTDSI__SYNC_OUT       (* (reg8 *) DIR__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(DIR__SIO_CFG)
    #define DIR_SIO_HYST_EN        (* (reg8 *) DIR__SIO_HYST_EN)
    #define DIR_SIO_REG_HIFREQ     (* (reg8 *) DIR__SIO_REG_HIFREQ)
    #define DIR_SIO_CFG            (* (reg8 *) DIR__SIO_CFG)
    #define DIR_SIO_DIFF           (* (reg8 *) DIR__SIO_DIFF)
#endif /* (DIR__SIO_CFG) */

/* Interrupt Registers */
#if defined(DIR__INTSTAT)
    #define DIR_INTSTAT            (* (reg8 *) DIR__INTSTAT)
    #define DIR_SNAP               (* (reg8 *) DIR__SNAP)
    
	#define DIR_0_INTTYPE_REG 		(* (reg8 *) DIR__0__INTTYPE)
#endif /* (DIR__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_DIR_H */


/* [] END OF FILE */
