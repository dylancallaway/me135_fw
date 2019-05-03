/*******************************************************************************
* File Name: Heartbeat.h  
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

#if !defined(CY_PINS_Heartbeat_H) /* Pins Heartbeat_H */
#define CY_PINS_Heartbeat_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Heartbeat_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Heartbeat__PORT == 15 && ((Heartbeat__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Heartbeat_Write(uint8 value);
void    Heartbeat_SetDriveMode(uint8 mode);
uint8   Heartbeat_ReadDataReg(void);
uint8   Heartbeat_Read(void);
void    Heartbeat_SetInterruptMode(uint16 position, uint16 mode);
uint8   Heartbeat_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Heartbeat_SetDriveMode() function.
     *  @{
     */
        #define Heartbeat_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Heartbeat_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Heartbeat_DM_RES_UP          PIN_DM_RES_UP
        #define Heartbeat_DM_RES_DWN         PIN_DM_RES_DWN
        #define Heartbeat_DM_OD_LO           PIN_DM_OD_LO
        #define Heartbeat_DM_OD_HI           PIN_DM_OD_HI
        #define Heartbeat_DM_STRONG          PIN_DM_STRONG
        #define Heartbeat_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Heartbeat_MASK               Heartbeat__MASK
#define Heartbeat_SHIFT              Heartbeat__SHIFT
#define Heartbeat_WIDTH              1u

/* Interrupt constants */
#if defined(Heartbeat__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Heartbeat_SetInterruptMode() function.
     *  @{
     */
        #define Heartbeat_INTR_NONE      (uint16)(0x0000u)
        #define Heartbeat_INTR_RISING    (uint16)(0x0001u)
        #define Heartbeat_INTR_FALLING   (uint16)(0x0002u)
        #define Heartbeat_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Heartbeat_INTR_MASK      (0x01u) 
#endif /* (Heartbeat__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Heartbeat_PS                     (* (reg8 *) Heartbeat__PS)
/* Data Register */
#define Heartbeat_DR                     (* (reg8 *) Heartbeat__DR)
/* Port Number */
#define Heartbeat_PRT_NUM                (* (reg8 *) Heartbeat__PRT) 
/* Connect to Analog Globals */                                                  
#define Heartbeat_AG                     (* (reg8 *) Heartbeat__AG)                       
/* Analog MUX bux enable */
#define Heartbeat_AMUX                   (* (reg8 *) Heartbeat__AMUX) 
/* Bidirectional Enable */                                                        
#define Heartbeat_BIE                    (* (reg8 *) Heartbeat__BIE)
/* Bit-mask for Aliased Register Access */
#define Heartbeat_BIT_MASK               (* (reg8 *) Heartbeat__BIT_MASK)
/* Bypass Enable */
#define Heartbeat_BYP                    (* (reg8 *) Heartbeat__BYP)
/* Port wide control signals */                                                   
#define Heartbeat_CTL                    (* (reg8 *) Heartbeat__CTL)
/* Drive Modes */
#define Heartbeat_DM0                    (* (reg8 *) Heartbeat__DM0) 
#define Heartbeat_DM1                    (* (reg8 *) Heartbeat__DM1)
#define Heartbeat_DM2                    (* (reg8 *) Heartbeat__DM2) 
/* Input Buffer Disable Override */
#define Heartbeat_INP_DIS                (* (reg8 *) Heartbeat__INP_DIS)
/* LCD Common or Segment Drive */
#define Heartbeat_LCD_COM_SEG            (* (reg8 *) Heartbeat__LCD_COM_SEG)
/* Enable Segment LCD */
#define Heartbeat_LCD_EN                 (* (reg8 *) Heartbeat__LCD_EN)
/* Slew Rate Control */
#define Heartbeat_SLW                    (* (reg8 *) Heartbeat__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Heartbeat_PRTDSI__CAPS_SEL       (* (reg8 *) Heartbeat__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Heartbeat_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Heartbeat__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Heartbeat_PRTDSI__OE_SEL0        (* (reg8 *) Heartbeat__PRTDSI__OE_SEL0) 
#define Heartbeat_PRTDSI__OE_SEL1        (* (reg8 *) Heartbeat__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Heartbeat_PRTDSI__OUT_SEL0       (* (reg8 *) Heartbeat__PRTDSI__OUT_SEL0) 
#define Heartbeat_PRTDSI__OUT_SEL1       (* (reg8 *) Heartbeat__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Heartbeat_PRTDSI__SYNC_OUT       (* (reg8 *) Heartbeat__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Heartbeat__SIO_CFG)
    #define Heartbeat_SIO_HYST_EN        (* (reg8 *) Heartbeat__SIO_HYST_EN)
    #define Heartbeat_SIO_REG_HIFREQ     (* (reg8 *) Heartbeat__SIO_REG_HIFREQ)
    #define Heartbeat_SIO_CFG            (* (reg8 *) Heartbeat__SIO_CFG)
    #define Heartbeat_SIO_DIFF           (* (reg8 *) Heartbeat__SIO_DIFF)
#endif /* (Heartbeat__SIO_CFG) */

/* Interrupt Registers */
#if defined(Heartbeat__INTSTAT)
    #define Heartbeat_INTSTAT            (* (reg8 *) Heartbeat__INTSTAT)
    #define Heartbeat_SNAP               (* (reg8 *) Heartbeat__SNAP)
    
	#define Heartbeat_0_INTTYPE_REG 		(* (reg8 *) Heartbeat__0__INTTYPE)
#endif /* (Heartbeat__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Heartbeat_H */


/* [] END OF FILE */
