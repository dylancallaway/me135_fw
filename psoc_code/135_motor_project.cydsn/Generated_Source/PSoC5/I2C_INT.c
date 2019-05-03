/*******************************************************************************
* File Name: I2CINT.c
* Version 2.0
*
* Description:
*  This file contains the code that operates during the interrupt service
*  routine.  For this component, most of the runtime code is located in
*  the ISR.
*
*******************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "I2C_PVT.h"
#include "cyapicallbacks.h"


/*******************************************************************************
* Function Name: I2C_ISR
********************************************************************************
*
* Summary:
*  Handle Interrupt Service Routine.
*
* Parameters:
*  I2C_dataPtrS1 - The global variable which stores the pointer to the
*  data exposed to an I2C master for the first slave address.
*
*  I2C_rwOffsetS1 - The global variable which stores an offset for read
*  and write operations, is set at each write sequence of the first slave
*  address.
*
*  I2C_rwIndexS1: global variable, which stores pointer to the next
*  value to be read or written for the first slave address.
*
* I2C_wrProtectS1 - The global variable which stores an offset where data
*  is read only for the first slave address.
*
* I2C_bufSizeS1 - The global variable which stores the size of a data array
*  exposed to the I2C master for the first slave address.
*
*  I2C_dataPtrS2 - The global variable which stores a pointer to the
*  data exposed to the I2C master for the second slave address.
*
*  I2C_rwOffsetS2 - The global variable which stores an offset for read
*  and write operations, is set at each write sequence of the second slave
*  device.
*
*  I2C_rwIndexS2 - The global variable which stores a pointer to the next
*  value to be read or written for the second slave address.
*
* I2C_wrProtectS2 - The global variable which stores an offset where data
*  is "Read only" for the second slave address.  ??
*
* I2C_bufSizeS2 - The global variable which stores the size of a data array
*  exposed to the I2C master for the second slave address.
*
* I2C_curState - The global variable which stores a current state of an
*  I2C state machine.
*
*  I2C_curStatus - The global variable which stores the current status of
*  the component.
*
* Return:
*  I2C_rwOffsetS1 - The global variable which stores an offset for read
*  and write operations, is set at each write sequence of the first slave
*  address and is reset if a received slave address matches the first slave address
*  and a next operation will be read.
*
*  I2C_rwIndexS1 - The global variable which stores a pointer to the next
*  value to be read or written for the first slave address. Is set to
*  I2C_rwOffsetS1 and than incremented if a received slave address
*  matches the first slave address and a next operation will be read.
*
*  I2C_rwOffsetS2 - The global variable which stores an offset for read
*  and write operations, is set at each write sequence of the second slave
*  address. This variable changes if a new sub-address is passed to the slave.
*
*  I2C_rwIndexS2 - The global variable which stores a pointer to the next
*  value to be read or written for the second slave address. This variable
*  changes if a new sub-address is passed to the slave.
*
*******************************************************************************/
CY_ISR(I2C_ISR)
{
    static uint8  tmp8;
    static uint8  tmpCsr;

#if (I2C_SUBADDR_WIDTH == I2C_SUBADDR_16BIT)
    static uint16 tmp16;
#endif /* (I2C_SUBADDR_WIDTH == I2C_SUBADDR_16BIT) */

#ifdef I2C_ISR_ENTRY_CALLBACK
    I2C_ISR_EntryCallback();
#endif /* I2C_ISR_ENTRY_CALLBACK */
    
    /* Entry from interrupt
    *  In the hardware address compare mode, we can assume we only get interrupted
    *  when a valid address is recognized. In the software address compare mode,
    *  we have to check every address after a start condition.
    */

    /* Make copy to check Stop condition after bus has been released */
    tmpCsr = I2C_CSR_REG;

    /* Check for address phase of the transaction */
    if (I2C_IS_BIT_SET(tmpCsr, I2C_CSR_ADDRESS))
    {
        #if (I2C_ADDRESSES == I2C_TWO_ADDRESSES)

            /* Get slave address from data register */
            tmp8 = ((I2C_DATA_REG >> I2C_ADDRESS_SHIFT) & I2C_SADDR_MASK);

            if (tmp8 == I2C_addrS1)   /* Check for address 1  */
            {
                if (I2C_IS_BIT_SET(I2C_DATA_REG, I2C_READ_FLAG))
                {  /* Prepare next read op, get data and place in register */

                    /* Load first data byte  */
                    I2C_DATA_REG = I2C_dataPtrS1[I2C_rwOffsetS1];

                    /* ACK and transmit */
                    I2C_CSR_REG = (I2C_CSR_ACK | I2C_CSR_TRANSMIT);

                    /* Set index to offset */
                    I2C_rwIndexS1 = I2C_rwOffsetS1;

                    /* Advance to data location */
                    ++I2C_rwIndexS1;

                    /* Set Read busy status */
                    I2C_curStatus |= I2C_STATUS_RD1BUSY;

                    /* Prepare for read transaction */
                    I2C_curState = I2C_SM_DEV1_RD_DATA;
                }
                else  /* Start of a Write transaction, reset pointers, first byte is address */
                {  /* Prepare next operation to write offset */

                    /* ACK and ready to receive sub address */
                    I2C_CSR_REG = I2C_CSR_ACK;

                    /* Set Write busy status */
                    I2C_curStatus |= I2C_STATUS_WR1BUSY;

                    /* Prepare for read transaction */
                    I2C_curState = I2C_SM_DEV1_WR_ADDR;

                    /* Stop Interrupt Enable */
                    I2C_CFG_REG  |= I2C_CFG_STOP_IE;

                }  /* Prepared for next Write transaction */
            }   /* Slave address #1 is processed */
            else if (tmp8 == I2C_addrS2)   /* Check for address 2  */
            {
                if (I2C_IS_BIT_SET(I2C_DATA_REG, I2C_READ_FLAG))
                {  /* Prepare next read op, get data and place in register */

                    /* Load first data byte  */
                    I2C_DATA_REG = I2C_dataPtrS2[I2C_rwOffsetS2];

                    /* ACK and transmit */
                    I2C_CSR_REG = (I2C_CSR_ACK | I2C_CSR_TRANSMIT);

                    /* Reset pointer to previous offset */
                    I2C_rwIndexS2 = I2C_rwOffsetS2;

                    /* Advance to data location */
                    ++I2C_rwIndexS2;

                    /* Set read busy status */
                    I2C_curStatus |= I2C_STATUS_RD2BUSY;

                    /* Prepare for read transaction */
                    I2C_curState = I2C_SM_DEV2_RD_DATA;

                }  /* Prepared for next Read transaction */
                else  /* Start of write transfer, reset ptrs, 1st byte is address */
                {  /* Prepare next operation to write offset */

                    /* ACK and ready to receive address */
                    I2C_CSR_REG = I2C_CSR_ACK;

                    /* Set Write busy status */
                    I2C_curStatus |= I2C_STATUS_WR2BUSY;

                    /* Prepare for read transaction */
                    I2C_curState = I2C_SM_DEV2_WR_ADDR;

                    /* Enable interrupt on Stop */
                    I2C_CFG_REG  |= I2C_CFG_STOP_IE;
                } /* Prepared for the next Write transaction */
            }
            else   /* No address match */
            {   /* NAK address Match  */
                I2C_CSR_REG = I2C_CSR_NAK;
            }
        #else /* One slave address - hardware address matching */

            if (I2C_IS_BIT_SET(I2C_DATA_REG, I2C_READ_FLAG))
            {   /* Prepare next read op, get data and place in register */

                /* Load first data byte  */
                I2C_DATA_REG = I2C_dataPtrS1[I2C_rwOffsetS1];

                /* ACK and transmit */
                I2C_CSR_REG = (I2C_CSR_ACK | I2C_CSR_TRANSMIT);

                /* Reset pointer to previous offset */
                I2C_rwIndexS1 = I2C_rwOffsetS1;

                /* Advance to data location */
                ++I2C_rwIndexS1;

                /* Set read busy status */
                I2C_curStatus |= I2C_STATUS_RD1BUSY;

                /* Prepare for read transaction */
                I2C_curState = I2C_SM_DEV1_RD_DATA;
            }
            else  /* Start of write transfer, reset ptrs, 1st byte is address */
            {   /* Prepare next operation to write offset */

                /* ACK and ready to receive address */
                I2C_CSR_REG = I2C_CSR_ACK;

                /* Set Write activity */
                I2C_curStatus |= I2C_STATUS_WR1BUSY;

                /* Prepare for read transaction */
                I2C_curState = I2C_SM_DEV1_WR_ADDR;

                /* Enable interrupt on stop */
                I2C_CFG_REG |= I2C_CFG_STOP_IE;
            }
        #endif  /* (I2C_ADDRESSES == I2C_TWO_ADDRESSES) */
    }
    else if (I2C_IS_BIT_SET(tmpCsr, I2C_CSR_BYTE_COMPLETE))
    {   /* Check for data transfer */

        /* Data transfer state machine */
        switch (I2C_curState)
        {
            /* Address written from Master to Slave. */
            case I2C_SM_DEV1_WR_ADDR:

                /* If 8-bit interface, Advance to WR_Data, else to ADDR2 */
                #if (I2C_SUBADDR_WIDTH == I2C_SUBADDR_8BIT)
                    tmp8 = I2C_DATA_REG;
                    if (tmp8 < I2C_bufSizeS1)
                    {
                        /* ACK and ready to receive data */
                        I2C_CSR_REG = I2C_CSR_ACK;

                        /* Set offset to new value */
                        I2C_rwOffsetS1 = tmp8;

                        /* Reset index to offset value */
                        I2C_rwIndexS1 = tmp8;

                        /* Prepare for write transaction */
                        I2C_curState = I2C_SM_DEV1_WR_DATA;
                    }
                    else    /* Out of range, NAK data and don't set offset */
                    {
                        /* NAK master */
                        I2C_CSR_REG = I2C_CSR_NAK;
                    }

                #else   /* 16-bit */
                    /* Save MSB of address */
                    tmp16 = I2C_DATA_REG;

                    /* ACK and ready to receive address */
                    I2C_CSR_REG = I2C_CSR_ACK;

                    /* Prepare to get LSB of address */
                    I2C_curState = I2C_SM_DEV1_WR_ADDR_LSB;

                #endif  /* (I2C_SUBADDR_WIDTH == I2C_SUBADDR_8BIT) */

            break;  /* case I2C_SM_DEV1_WR_ADDR */

            #if (I2C_SUBADDR_WIDTH == I2C_SUBADDR_16BIT)

                /* Only used with 16-bit interface */
                case I2C_SM_DEV1_WR_ADDR_LSB:

                    /* Create offset */
                    tmp16 = (uint16) (tmp16 << I2C_ADDRESS_LSB_SHIFT) | I2C_DATA_REG;

                    /* Check range */
                    if(tmp16 < I2C_bufSizeS1)
                    {
                        /* ACK and ready to receive address */
                        I2C_CSR_REG = I2C_CSR_ACK;

                        /* Set offset to new value */
                        I2C_rwOffsetS1 = tmp16;

                        /* Reset index to offset value */
                        I2C_rwIndexS1 = tmp16;

                        /* Prepare for write transaction */
                        I2C_curState = I2C_SM_DEV1_WR_DATA;
                    }
                    else    /* Out of range, NAK data and don't set offset */
                    {
                        /* NAK master */
                        I2C_CSR_REG = I2C_CSR_NAK;
                    }
                break; /* case I2C_SM_DEV1_WR_ADDR_LSB */

            #endif  /* (I2C_SUBADDR_WIDTH == I2C_SUBADDR_16BIT) */


            /* Data written from master to slave. */
            case I2C_SM_DEV1_WR_DATA:

                /* Check for valid range */
                if (I2C_rwIndexS1 < I2C_wrProtectS1)
                {
                    /* Get data, to ACK quickly */
                    tmp8 = I2C_DATA_REG;

                    /* ACK and ready to receive sub address */
                    I2C_CSR_REG = I2C_CSR_ACK;

                    /* Write data to array */
                    I2C_dataPtrS1[I2C_rwIndexS1] = tmp8;

                    /* Increment pointer */
                    I2C_rwIndexS1++;
                }
                else
                {
                    /* NAK cause beyond write area */
                    I2C_CSR_REG = I2C_CSR_NAK;
                }
            break;  /* I2C_SM_DEV1_WR_DATA */


            /* Data read by master from slave */
            case I2C_SM_DEV1_RD_DATA:

                /* Check ACK/NAK */
                if ((tmpCsr & I2C_CSR_LRB) == I2C_CSR_LRB_ACK)
                {
                    /* Check for valid range */
                    if (I2C_rwIndexS1 < I2C_bufSizeS1)
                    {
                        /* Get data from array */
                        I2C_DATA_REG = I2C_dataPtrS1[I2C_rwIndexS1];

                        /* Send Data */
                        I2C_CSR_REG = I2C_CSR_TRANSMIT;

                        /* Increment pointer */
                        ++I2C_rwIndexS1;
                    }
                    else    /* No valid range */
                    {
                        /* Out of range send FFs */
                        I2C_DATA_REG = I2C_DUMMY_DATA;

                        /* Send Data */
                        I2C_CSR_REG = I2C_CSR_TRANSMIT;
                    }
                }
                else    /* Data was NAKed */
                {
                    /* Send dummy data at the end of read transaction */
                    I2C_DATA_REG = I2C_DUMMY_DATA;

                    /* Clear transmit bit at the end of read transaction */
                    I2C_CSR_REG = I2C_CSR_NAK;

                    /* Clear Busy Flag */
                    I2C_curStatus &= ((uint8) ~I2C_STATUS_BUSY);

                    /* Error or Stop, reset state */
                    I2C_curState = I2C_SM_IDLE;

                }
            break;  /* I2C_SM_DEV1_RD_DATA */

            /* Second Device Address */
            #if (I2C_ADDRESSES == I2C_TWO_ADDRESSES)

                case I2C_SM_DEV2_WR_ADDR:

                    /* If 8-bit interface, Advance to WR_Data, else to ADDR2 */
                    #if (I2C_SUBADDR_WIDTH == I2C_SUBADDR_8BIT)

                        tmp8 = I2C_DATA_REG;
                        if (tmp8 < I2C_bufSizeS2)
                        {
                            /* ACK and ready to receive address */
                            I2C_CSR_REG = I2C_CSR_ACK;

                            /* Set offset to new value */
                            I2C_rwOffsetS2 = tmp8;

                            /* Reset index to offset value */
                            I2C_rwIndexS2 = tmp8;

                            /* Prepare for write transaction */
                            I2C_curState = I2C_SM_DEV2_WR_DATA;
                        }
                        else    /* Out of range, NAK data and don't set offset */
                        {
                            /* NAK master */
                            I2C_CSR_REG = I2C_CSR_NAK;
                        }
                    #else
                        /* Save LSB of address */
                        tmp16 = I2C_DATA_REG;

                        /* ACK and ready to receive address */
                        I2C_CSR_REG = I2C_CSR_ACK;

                        /* Prepare to get LSB of address */
                        I2C_curState = I2C_SM_DEV2_WR_ADDR_LSB;
                    #endif  /* (I2C_SUBADDR_WIDTH == I2C_SUBADDR_8BIT) */

                break;  /* I2C_SM_DEV2_WR_ADDR */

                #if (I2C_SUBADDR_WIDTH == I2C_SUBADDR_16BIT)

                    /* Only used with 16-bit interface */
                    case I2C_SM_DEV2_WR_ADDR_LSB:
                        /* Create offset */
                        tmp16 = (uint16) (tmp16 << 8u) | I2C_DATA_REG;
                        if (tmp16 < I2C_bufSizeS2)
                        {
                            /* ACK and ready to receive address */
                            I2C_CSR_REG = I2C_CSR_ACK;

                            /* Set offset to new value */
                            I2C_rwOffsetS2 = tmp16;

                            /* Reset index to offset value */
                            I2C_rwIndexS2 = tmp16;

                            /* Prepare for write transaction */
                            I2C_curState = I2C_SM_DEV2_WR_DATA;
                        }
                        else    /* Out of range, NAK data and don't set offset */
                        {
                            /* NAK master */
                            I2C_CSR_REG = I2C_CSR_NAK;
                        }
                        break; /* I2C_SM_DEV2_WR_ADDR_LSB */

                #endif   /* (I2C_SUBADDR_WIDTH == I2C_SUBADDR_16BIT) */


                /* Data written from master to slave. */
                case I2C_SM_DEV2_WR_DATA:

                    /* Check for valid range */
                    if (I2C_rwIndexS2 < I2C_wrProtectS2)
                    {
                        /* Get data, to ACK quickly */
                        tmp8 = I2C_DATA_REG;

                        /* ACK and ready to receive sub address */
                        I2C_CSR_REG = I2C_CSR_ACK;

                        /* Write data to array */
                        I2C_dataPtrS2[I2C_rwIndexS2] = tmp8;

                        /* Inc pointer */
                        ++I2C_rwIndexS2;
                    }
                    else
                    {
                        /* NAK cause beyond write area */
                        I2C_CSR_REG = I2C_CSR_NAK;
                    }
                    break;  /* I2C_SM_DEV2_WR_DATA */

                    /* Data read by master from slave */
                    case I2C_SM_DEV2_RD_DATA:

                        if ((tmpCsr & I2C_CSR_LRB) == I2C_CSR_LRB_ACK)
                        {   /* ACKed */
                            /* Check for valid range */
                            if (I2C_rwIndexS2 < I2C_bufSizeS2)
                            {   /* Check ACK/NAK */
                                /* Get data from array */
                                I2C_DATA_REG = I2C_dataPtrS2[I2C_rwIndexS2];

                                /* Send Data */
                                I2C_CSR_REG = I2C_CSR_TRANSMIT;

                                /* Increment pointer */
                                I2C_rwIndexS2++;
                            }
                            else    /* Not valid range */
                            {
                                /* Out of range send FFs */
                                I2C_DATA_REG = I2C_DUMMY_DATA;

                                /* Send Data */
                                I2C_CSR_REG = I2C_CSR_TRANSMIT;
                            }
                        }
                        else    /* NAKed */
                        {
                            /* Out of range send FFs */
                            I2C_DATA_REG = I2C_DUMMY_DATA;

                            /* Send Data */
                            I2C_CSR_REG = I2C_CSR_TRANSMIT;

                            /* Clear busy status */
                            I2C_curStatus &= ((uint8) ~I2C_STATUS_BUSY);

                            /* Error or Stop, reset state */
                            I2C_curState = I2C_SM_IDLE;
                        }   /* End if ACK/NAK */

                        break;  /* I2C_SM_DEV2_RD_DATA */

            #endif  /* (I2C_ADDRESSES == I2C_TWO_ADDRESSES) */

            default:
                /* Invalid state, reset state to idle */
                I2C_curState = I2C_SM_IDLE;

                /* Reset offsets and index */
                I2C_rwOffsetS1 = 0u;
                I2C_rwIndexS1  = 0u;

                /* Dummy NAK to release bus */
                I2C_CSR_REG = I2C_CSR_NAK;
                break;

        }  /* End switch/case I2C_curState */
    }
    else
    {
        /* Intentional blank line */
    }

    /* Check if Stop was detected */
    if (I2C_IS_BIT_SET(I2C_CSR_REG, I2C_CSR_STOP_STATUS))
    {
        /* Clear Busy flag */
        I2C_curStatus &= ((uint8) ~I2C_STATUS_BUSY);

        /* error or stop - reset state */
        I2C_curState = I2C_SM_IDLE;

        /* Disable interrupt on Stop */
        I2C_CFG_REG &= ((uint8) ~I2C_CFG_STOP_IE);
    }
#ifdef I2C_ISR_EXIT_CALLBACK
    I2C_ISR_ExitCallback();
#endif /* I2C_ISR_EXIT_CALLBACK */    
}


#if (I2C_WAKEUP_ENABLED)
    /*******************************************************************************
    * Function Name: I2C_WAKEUP_ISR
    ********************************************************************************
    *
    * Summary:
    *  The interrupt handler to trigger after a wakeup.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    CY_ISR(I2C_WAKEUP_ISR)
    {
    #ifdef I2C_WAKEUP_ISR_ENTRY_CALLBACK
        I2C_WAKEUP_ISR_EntryCallback();
    #endif /* I2C_WAKEUP_ISR_ENTRY_CALLBACK */         
        
        I2C_wakeupSource = 1u;  /* I2C was wakeup source */

        /* SCL is stretched until EZI2C_Wakeup() is called */
        
    #ifdef I2C_ISR_EXIT_CALLBACK
        I2C_ISR_ExitCallback();
    #endif /* I2C_ISR_EXIT_CALLBACK */         
    }
#endif /* (I2C_WAKEUP_ENABLED) */


/* [] END OF FILE */
