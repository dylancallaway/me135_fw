/*******************************************************************************
* File Name: EZI2C.h
* Version 2.0
*
* Description:
*  This is the header file for the EzI2C user module.  It contains function
*  prototypes and constants for the users convenience.
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_EZI2C_EZI2C_H)
#define CY_EZI2C_EZI2C_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h"

/* Check if required defines such as CY_PSOC5LP are available in cy_boot */
#if !defined (CY_PSOC5LP)
    #error Component EZI2C_v2_0 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

/***************************************
*   Initial Parameter Constants
***************************************/

#define EZI2C_DATA_RATE      (100u)
#define EZI2C_BUS_PORT       (0u)
#define EZI2C_ENABLE_WAKEUP  (0u)
#define EZI2C_SUBADDR_WIDTH  (0u)
#define EZI2C_ADDRESSES      (1u)
#define EZI2C_DEFAULT_ADDR1  (8u)
#define EZI2C_DEFAULT_ADDR2  (9u)


/***************************************
*   Conditional Compilation Parameters
***************************************/

#define EZI2C_WAKEUP_ENABLED (0u != EZI2C_ENABLE_WAKEUP)

/* Number of addresses enum. */
#define EZI2C_ONE_ADDRESS    (1u)
#define EZI2C_TWO_ADDRESSES  (2u)

/* Sub-address size enum. */
#define EZI2C_ANY   (0u)
#define EZI2C_I2C0  (1u)
#define EZI2C_I2C1  (2u)

/* Sub-address size enum. */
#define EZI2C_SUBADDR_8BIT   (0u)
#define EZI2C_SUBADDR_16BIT  (1u)


/***************************************
*       Type Definitions
***************************************/

/* Structure to store configuration before enter Sleep */
typedef struct
{
    uint8   enableState;
    uint8   xcfg;
    uint8   adr;
    uint8   cfg;
    uint8   clkDiv1;
    uint8   clkDiv2;

} EZI2C_BACKUP_STRUCT;


/***************************************
*        Function Prototypes
***************************************/

void    EZI2C_Init(void)     ;
void    EZI2C_Enable(void)   ;
void    EZI2C_Start(void)    ;
void    EZI2C_Stop(void)     ;

#define EZI2C_EnableInt()        CyIntEnable      (EZI2C_ISR_NUMBER)
#define EZI2C_DisableInt()       CyIntDisable     (EZI2C_ISR_NUMBER)
#define EZI2C_ClearPendingInt()  CyIntClearPending(EZI2C_ISR_NUMBER)
#define EZI2C_SetPendingInt()    CyIntSetPending  (EZI2C_ISR_NUMBER)

void    EZI2C_Sleep(void)            ;
void    EZI2C_Wakeup(void)           ;
void    EZI2C_SaveConfig(void)       ;
void    EZI2C_RestoreConfig(void)    ;

uint8   EZI2C_GetActivity(void)      ;

void    EZI2C_SetBuffer1(uint16 bufSize, uint16 rwBoundary, volatile uint8 * dataPtr) ;
void    EZI2C_SetAddress1(uint8 address) ;
uint8   EZI2C_GetAddress1(void)          ;

#if (EZI2C_ADDRESSES == EZI2C_TWO_ADDRESSES)
    void    EZI2C_SetBuffer2(uint16 bufSize, uint16 rwBoundary, volatile uint8 * dataPtr) ;
    void    EZI2C_SetAddress2(uint8 address )    ;
    uint8   EZI2C_GetAddress2(void)              ;
#endif /* (EZI2C_ADDRESSES == EZI2C_TWO_ADDRESSES) */

/* EZI2C interrupt handler */
CY_ISR_PROTO(EZI2C_ISR);
#if (EZI2C_WAKEUP_ENABLED)
    CY_ISR_PROTO(EZI2C_WAKEUP_ISR);
#endif /* (EZI2C_WAKEUP_ENABLED) */


/***************************************
*     Vars with External Linkage
***************************************/

extern uint8 EZI2C_initVar;


/***************************************
*              API Constants
***************************************/

/* Status bit definition */
#define EZI2C_STATUS_READ1   (0x01u) /* A read addr 1 operation occurred since last status check */
#define EZI2C_STATUS_WRITE1  (0x02u) /* A Write addr 1 operation occurred since last status check */
#define EZI2C_STATUS_READ2   (0x04u) /* A read addr 2 operation occurred since last status check */
#define EZI2C_STATUS_WRITE2  (0x08u) /* A Write addr 2 operation occurred since last status check */
#define EZI2C_STATUS_BUSY    (0x10u) /* A start has occurred, but a Stop has not been detected */
#define EZI2C_STATUS_RD1BUSY (0x11u) /* Addr 1 read busy  */
#define EZI2C_STATUS_WR1BUSY (0x12u) /* Addr 1 write busy */
#define EZI2C_STATUS_RD2BUSY (0x14u) /* Addr 2 read busy  */
#define EZI2C_STATUS_WR2BUSY (0x18u) /* Addr 2 write busy */
#define EZI2C_STATUS_MASK    (0x1Fu) /* Mask for status bits */
#define EZI2C_STATUS_ERR     (0x80u) /* An Error occurred since last read */

/* Data send to master in case of read overflow */
#define EZI2C_DUMMY_DATA         (0xFFu)

/* Address shift */
#define EZI2C_ADDRESS_SHIFT      (1u)
#define EZI2C_ADDRESS_LSB_SHIFT  (8u)

/* Component state enum. */
#define EZI2C_ENABLED            (0x01u)
#define EZI2C_DISABLED           (0x00u)

/* Return 1 if corresponding bit is set, otherwise 0 */
#define EZI2C_IS_BIT_SET(value, mask) (((mask) == ((value) & (mask))) ? (1u) : (0u))


/***************************************
*              Registers
***************************************/

/* I2C Extended Configuration Register */
#define EZI2C_XCFG_REG       (*(reg8 *) EZI2C_I2C_Prim__XCFG)
#define EZI2C_XCFG_PTR       ( (reg8 *) EZI2C_I2C_Prim__XCFG)

/* I2C Slave Address Register */
#define EZI2C_ADDR_REG       (*(reg8 *) EZI2C_I2C_Prim__ADR)
#define EZI2C_ADDR_PTR       ( (reg8 *) EZI2C_I2C_Prim__ADR)

/* I2C Configuration Register */
#define EZI2C_CFG_REG        (*(reg8 *) EZI2C_I2C_Prim__CFG)
#define EZI2C_CFG_PTR        ( (reg8 *) EZI2C_I2C_Prim__CFG)

/* I2C Control and Status Register */
#define EZI2C_CSR_REG        (*(reg8 *) EZI2C_I2C_Prim__CSR)
#define EZI2C_CSR_PTR        ( (reg8 *) EZI2C_I2C_Prim__CSR)

/* I2C Data Register */
#define EZI2C_DATA_REG       (*(reg8 *) EZI2C_I2C_Prim__D)
#define EZI2C_DATA_PTR       ( (reg8 *) EZI2C_I2C_Prim__D)

/*  8 LSB bits of the 10-bit Clock Divider */
#define EZI2C_CLKDIV1_REG    (*(reg8 *) EZI2C_I2C_Prim__CLK_DIV1)
#define EZI2C_CLKDIV1_PTR    ( (reg8 *) EZI2C_I2C_Prim__CLK_DIV1)

/* 2 MSB bits of the 10-bit Clock Divider */
#define EZI2C_CLKDIV2_REG    (*(reg8 *) EZI2C_I2C_Prim__CLK_DIV2)
#define EZI2C_CLKDIV2_PTR    ( (reg8 *) EZI2C_I2C_Prim__CLK_DIV2)

/* Power System Control Register 1 */
#define EZI2C_PWRSYS_CR1_REG (*(reg8 *) CYREG_PWRSYS_CR1)
#define EZI2C_PWRSYS_CR1_PTR ( (reg8 *) CYREG_PWRSYS_CR1)

/* I2C operation in Active Mode */
#define EZI2C_PM_ACT_CFG_REG (*(reg8 *) EZI2C_I2C_Prim__PM_ACT_CFG)
#define EZI2C_PM_ACT_CFG_PTR ( (reg8 *) EZI2C_I2C_Prim__PM_ACT_CFG)
#define EZI2C_ACT_PWR_EN     ( (uint8)  EZI2C_I2C_Prim__PM_ACT_MSK)

/* I2C operation in Alternate Active (Standby) Mode */
#define EZI2C_PM_STBY_CFG_REG    (*(reg8 *) EZI2C_I2C_Prim__PM_STBY_CFG)
#define EZI2C_PM_STBY_CFG_PTR    ( (reg8 *) EZI2C_I2C_Prim__PM_STBY_CFG)
#define EZI2C_STBY_PWR_EN        ( (uint8)  EZI2C_I2C_Prim__PM_STBY_MSK)


/***************************************
*       Register Constants
***************************************/

/* I2C backup regulator */
#define EZI2C_PWRSYS_CR1_I2C_BACKUP  (0x04u)

/* Interrupt number and priority */
#define EZI2C_ISR_NUMBER         (EZI2C_isr__INTC_NUMBER)
#define EZI2C_ISR_PRIORITY       (EZI2C_isr__INTC_PRIOR_NUM)

/* Block reset constants */
#define EZI2C_CLEAR_REG          (0x00u)
#define EZI2C_BLOCK_RESET_DELAY  (2u)

/* XCFG I2C Extended Configuration Register */
#define EZI2C_XCFG_CLK_EN        (0x80u) /* Clock enable */
#define EZI2C_XCFG_I2C_ON        (0x40u) /* Set before entering sleep mode */
#define EZI2C_XCFG_SLEEP_READY   (0x20u) /* Ready to sleep */
#define EZI2C_XCFG_FORCE_NACK    (0x10u) /* Force nack */
#define EZI2C_XCFG_HDWR_ADDR_EN  (0x01u) /* Hardware address comparison */

/* Data I2C Slave Data Register */
#define EZI2C_SADDR_MASK         (0x7Fu)
#define EZI2C_DATA_MASK          (0xFFu)
#define EZI2C_READ_FLAG          (0x01u)

/* CFG I2C Configuration Register */
#define EZI2C_CFG_SIO_SELECT    (0x80u) /* Pin Select for SCL/SDA lines */
#define EZI2C_CFG_PSELECT       (0x40u) /* Pin Select */
#define EZI2C_CFG_BUS_ERR_IE    (0x20u) /* Bus Error Interrupt Enable */
#define EZI2C_CFG_STOP_ERR_IE   (0x10u) /* Enable Interrupt on STOP condition */
#define EZI2C_CFG_STOP_IE       (0x10u) /* Enable Interrupt on STOP condition */
#define EZI2C_CFG_CLK_RATE      (0x04u) /* Clock rate mask. 1 for 50K, 0 for 100K and 400K */
#define EZI2C_CFG_EN_SLAVE      (0x01u) /* Enable Slave operation */

/* CSR I2C Control and Status Register */
#define EZI2C_CSR_BUS_ERROR     (0x80u) /* Active high when bus error has occurred */
#define EZI2C_CSR_LOST_ARB      (0x40u) /* Set to 1 if lost arbitration in host mode */
#define EZI2C_CSR_STOP_STATUS   (0x20u) /* Set to 1 if Stop has been detected */
#define EZI2C_CSR_ACK           (0x10u) /* ACK response */
#define EZI2C_CSR_NAK           (0x00u) /* NAK response */
#define EZI2C_CSR_LRB_ACK       (0x00u) /* Last received bit was an ACK */
#define EZI2C_CSR_ADDRESS       (0x08u) /* Set in firmware 0 = status bit, 1 Address is slave */
#define EZI2C_CSR_TRANSMIT      (0x04u) /* Set in firmware 1 = transmit, 0 = receive */
#define EZI2C_CSR_LRB           (0x02u) /* Last received bit */
#define EZI2C_CSR_LRB_NAK       (0x02u) /* Last received bit was an NAK */
#define EZI2C_CSR_BYTE_COMPLETE (0x01u) /* Informs that last byte has been sent */

/* I2C state machine constants */
#define  EZI2C_SM_IDLE              (0x00u) /* Wait for Start */

/* Primary slave address states */
#define  EZI2C_SM_DEV1_WR_ADDR      (0x01u) /* Wait for sub-address */
#define  EZI2C_SM_DEV1_WR_ADDR_MSB  (0x01u) /* Wait for sub-address MSB */
#define  EZI2C_SM_DEV1_WR_ADDR_LSB  (0x02u) /* Wait for sub-address LSB */
#define  EZI2C_SM_DEV1_WR_DATA      (0x04u) /* Get data from Master */
#define  EZI2C_SM_DEV1_RD_DATA      (0x08u) /* Send data to Master */

/* Secondary slave address states */
#define  EZI2C_SM_DEV2_WR_ADDR      (0x11u) /* Wait for sub-address */
#define  EZI2C_SM_DEV2_WR_ADDR_MSB  (0x11u) /* Wait for sub-address MSB */
#define  EZI2C_SM_DEV2_WR_ADDR_LSB  (0x12u) /* Wait for sub-address LSB */
#define  EZI2C_SM_DEV2_WR_DATA      (0x14u) /* Get data from Master */
#define  EZI2C_SM_DEV2_RD_DATA      (0x18u) /* Send data to Master */


/***************************************
*    Initialization Register Settings
***************************************/

/* Oversampling rate for data date 50kpbs and less is 32, 16 for others */
#define EZI2C_OVS_32_LIM      (50u)
#define EZI2C_OVERSAMPLE_RATE ((EZI2C_DATA_RATE <= EZI2C_OVS_32_LIM) ? (32u) : (16u))

/* Return default bits depends on configuration */
#define EZI2C_DEFAULT_HDWR_ADDR  ((EZI2C_ONE_ADDRESS == EZI2C_ADDRESSES) ? (1u) : (0u))

#define EZI2C_DEFAULT_PSELECT    ((EZI2C_ANY  != EZI2C_BUS_PORT) ? (1u) : (0u))
#define EZI2C_DEFAULT_SIO_SELECT ((EZI2C_I2C1 == EZI2C_BUS_PORT) ? (1u) : (0u))
#define EZI2C_DEFUALT_CLK_RATE   ((EZI2C_DATA_RATE <= EZI2C_OVS_32_LIM) ? (1u) : (0u))

/* Return bits within registers position */
#define EZI2C_GET_XCFG_HDWR_ADDR_EN(hwAddr) ((0u != (hwAddr)) ? (EZI2C_XCFG_HDWR_ADDR_EN) : (0u))
#define EZI2C_GET_XCFG_I2C_ON(wakeup)       ((0u != (wakeup)) ? (EZI2C_XCFG_I2C_ON) : (0u))

#define EZI2C_GET_CFG_PSELECT(pSel)      ((0u != (pSel)) ? (EZI2C_CFG_PSELECT) : (0u))
#define EZI2C_GET_CFG_SIO_SELECT(sioSel) ((0u != (sioSel)) ? (EZI2C_CFG_SIO_SELECT) : (0u))
#define EZI2C_GET_CFG_CLK_RATE(clkRate)  ((0u != (clkRate)) ? (EZI2C_CFG_CLK_RATE) : (0u))

/* Initial registers settings */
#define EZI2C_DEFAULT_CFG    (EZI2C_GET_CFG_SIO_SELECT (EZI2C_DEFAULT_SIO_SELECT) | \
                                         EZI2C_GET_CFG_PSELECT(EZI2C_DEFAULT_PSELECT)        | \
                                         EZI2C_GET_CFG_CLK_RATE(EZI2C_DEFUALT_CLK_RATE)      | \
                                         EZI2C_CFG_EN_SLAVE)

#define EZI2C_DEFAULT_XCFG   (EZI2C_GET_XCFG_HDWR_ADDR_EN(EZI2C_DEFAULT_HDWR_ADDR) | \
                                         EZI2C_GET_XCFG_I2C_ON(EZI2C_ENABLE_WAKEUP)           | \
                                         EZI2C_XCFG_CLK_EN)

#define EZI2C_DEFAULT_ADDR   (EZI2C_DEFAULT_ADDR1)

/* Divide factor calculation */
#define EZI2C_DIVIDE_FACTOR_WITH_FRACT_BYTE \
    (((uint32) BCLK__BUS_CLK__KHZ << 8u) / ((uint32) EZI2C_DATA_RATE * EZI2C_OVERSAMPLE_RATE))

#define EZI2C_DEFAULT_DIVIDE_FACTOR  (((uint32) EZI2C_DIVIDE_FACTOR_WITH_FRACT_BYTE) >> 8u)

#define EZI2C_DEFAULT_CLKDIV1    LO8(EZI2C_DEFAULT_DIVIDE_FACTOR)
#define EZI2C_DEFAULT_CLKDIV2    HI8(EZI2C_DEFAULT_DIVIDE_FACTOR)


/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/

#define EZI2C_BUS_SPEED_50KHZ    (50u)
#define EZI2C_BUS_SPEED          EZI2C_DATA_RATE
#define EZI2C_OVER_SAMPLE_RATE   EZI2C_OVERSAMPLE_RATE

#define EZI2C_I2C_MASTER_MASK    (0xDDu)

#define EZI2C__ANY    EZI2C_ANY
#define EZI2C__I2C0   EZI2C_I2C0
#define EZI2C__I2C1   EZI2C_I2C1

#define EZI2C_DIVIDE_FACTOR  EZI2C_DEFAULT_DIVIDE_FACTOR

#if (EZI2C_ONE_ADDRESS == EZI2C_ADDRESSES)
    void EZI2C_SlaveSetSleepMode(void)   ;
    void EZI2C_SlaveSetWakeMode(void)    ;
#endif /* (EZI2C_ONE_ADDRESS == EZI2C_ADDRESSES) */

#define EZI2C_State          EZI2C_curState
#define EZI2C_Status         EZI2C_curStatus
#define EZI2C_DataPtr        EZI2C_dataPtrS1

#define EZI2C_RwOffset1      EZI2C_rwOffsetS1
#define EZI2C_RwIndex1       EZI2C_rwIndexS1
#define EZI2C_WrProtect1     EZI2C_wrProtectS1
#define EZI2C_BufSize1       EZI2C_bufSizeS1

#if (EZI2C_TWO_ADDRESSES == EZI2C_ADDRESSES)
    #define EZI2C_DataPtr2   EZI2C_dataPtrS2
    #define EZI2C_Address1   EZI2C_addrS1
    #define EZI2C_Address2   EZI2C_addrS2

    #define EZI2C_RwOffset2  EZI2C_rwOffsetS2
    #define EZI2C_RwIndex2   EZI2C_rwIndexS2
    #define EZI2C_WrProtect2 EZI2C_wrProtectS2
    #define EZI2C_BufSize2   EZI2C_bufSizeS2
#endif /* (EZI2C_TWO_ADDRESSES == EZI2C_ADDRESSES) */

#endif /* CY_EZI2C_EZI2C_H */


/* [] END OF FILE */
