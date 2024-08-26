/*******************************************************************************
 *       @brief      This module is a low level hardware driver for the SPI
 *                   device.
 *       @file       Uphole/src/CommDrivers/CommDriver_SPI.c
 *       @date       18August2024
 *       @copyright  COPYRIGHT (c) 2014 Target Drilling Inc. All rights are
 *                   reserved.  Reproduction in whole or in part is prohibited
 *                   without the prior written consent of the copyright holder.
 *******************************************************************************/

 //============================================================================//
 //      INCLUDES                                                              //
 //============================================================================//
#include <stdbool.h>
#include <stm32f4xx.h>
#include "portable.h"
#include "board.h"
#include "CommDriver_SPI.h"
#include "NVIC.h"
#include "SysTick.h"

//============================================================================//
//      DATA DEFINITIONS                                                      //
//============================================================================//

static U_INT16 m_nTimeout = 0;

//============================================================================//
//      FUNCTION IMPLEMENTATIONS                                              //
//============================================================================//

/*!
 ********************************************************************************
 *       @details
 *******************************************************************************/
 /*
  ;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  ; Function:
  ;   SPI_InitPins()
  ;
  ; Description:
  ;   Initializes the UART peripherals at the module level, configures
  ;   the UART peripherals and initializes the IRQ handlers for the
  ;   peripheral and associated DMA channels.
  ;
  ; Reentrancy:
  ;   No
  ;
  ; Assumptions:
  ;   This function is called only on reboot or power-up.
  ;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  */

void SPI_InitPins(void)
{
GPIO_InitTypeDef GPIO_InitStructure; // holds the configuration settings for the GPIO pins. ZD 18August2024

GPIO_StructInit(&GPIO_InitStructure); // Initializes the GPIO_InitStructure with default values. ZD 18August2024

// Common SPI set up parameters.
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; // Sets the GPIO pins to output mode. ZD 18August2024
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // Configures the speed of the GPIO pins to 50 MHz. ZD 18August2024
GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // Sets the output type to push-pull, meaning the pin can actively drive the output high or low. ZD 18August2024
GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; // No internal pull-up or pull-down resistors are enabled. ZD 18August2024

// DATAFLASH Chip Select: Port B Pin 6
// FRAM Chip Select: Port B Pin 5
GPIO_InitStructure.GPIO_Pin = DATAFLASH_CS_PIN | FRAM_CS_PIN | EEP_CS_PIN; // Configures the chip select pins for various SPI devices (DATAFLASH, FRAM, EEPROM). ZD 18August2024
GPIO_Init(SPI_CS_PORT, &GPIO_InitStructure); // Initializes the specified GPIO pins (for chip select) on the SPI CS port with the settings in GPIO_InitStructure. ZD 18August2024

GPIO_WriteBit(SPI_CS_PORT, DATAFLASH_CS_PIN, Bit_SET); // Sets the chip select pin for the DATAFLASH to high (inactive state). ZD 18August2024
GPIO_WriteBit(SPI_CS_PORT, FRAM_CS_PIN, Bit_SET); // Sets the chip select pin for the FRAM to high (inactive state). ZD 18August2024
GPIO_WriteBit(SPI_CS_PORT, EEP_CS_PIN, Bit_SET); // Sets the chip select pin for the EEPROM to high (inactive state). ZD 18August2024

// SPI1: Pin A5 is CLK for real board
// SPI1: Pin A6 is MISO for real board
// SPI1: Pin A7 is MOSI for real board
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; // Sets the GPIO pins to alternate function mode, allowing them to be used for SPI communication. ZD 18August2024
GPIO_InitStructure.GPIO_Pin = SPI_MISO | SPI_MOSI | SPI_SCK; // Configures the pins for MISO (Master In Slave Out), MOSI (Master Out Slave In), and SCK (Serial Clock). ZD 18August2024
GPIO_Init(SPI_DATA_PORT, &GPIO_InitStructure); // Initializes the SPI data pins on the SPI data port with the settings in GPIO_InitStructure. ZD 18August2024

GPIO_PinAFConfig(SPI_DATA_PORT, GPIO_PinSource5, GPIO_AF_SPI1); // Configures pin A5 as the SPI clock (SCK) for SPI1. ZD 18August2024
GPIO_PinAFConfig(SPI_DATA_PORT, GPIO_PinSource6, GPIO_AF_SPI1); // Configures pin A6 as the SPI MISO for SPI1. ZD 18August2024
GPIO_PinAFConfig(SPI_DATA_PORT, GPIO_PinSource7, GPIO_AF_SPI1); // Configures pin A7 as the SPI MOSI for SPI1. ZD 18August2024
}    //end SPI_InitPins

/*!
 ********************************************************************************
 *       @details
 *******************************************************************************/
 /*
  ;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  ; Function:
  ;   SPI_Initialize()
  ;
  ; Description:
  ;   Configures the pins and bus settings.
  ;
  ; Reentrancy:
  ;   No.
  ;
  ;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  */

void SPI_Initialize(void)
{
SPI_InitTypeDef SPI_InitStructure; // Declares a structure SPI_InitStructure of type SPI_InitTypeDef to hold the configuration settings for the SPI1 peripheral. ZD 18August2024

// Reset SPI1 with default values
SPI_Cmd(SPI1, DISABLE);  // Disables the SPI1 peripheral to reset it before configuration. ZD 18August2024
SPI_DeInit(SPI1); // Deinitializes the SPI1 peripheral, resetting all its registers to their default values. ZD 18August2024

SPI_StructInit(&SPI_InitStructure); // Initializes the SPI_InitStructure with default values. ZD 18August2024
SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // Configures SPI for full-duplex communication, meaning data can be transmitted and received simultaneously. ZD 18August2024
SPI_InitStructure.SPI_Mode = SPI_Mode_Master; // Sets the SPI1 peripheral to operate in master mode. ZD 18August2024
SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; // Configures the data frame size to 8 bits. ZD 18August2024
SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low; // Sets the clock polarity to low. ZD 18August2024
SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge; // Configures the clock phase to capture data on the first clock edge. ZD 18August2024
SPI_InitStructure.SPI_NSS = SPI_NSS_Soft | SPI_NSSInternalSoft_Set; // Configures the NSS signal to be handled by software rather than hardware. The internal NSS signal is set high. ZD 18August2024
SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; // Sets the baud rate prescaler to 16, which divides the system clock by 16 to generate the SPI clock. ZD 18August2024   // Should be about 5MHz
SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; // Configures the data to be transmitted with the Most Significant Bit (MSB) first. ZD 18August2024
SPI_InitStructure.SPI_CRCPolynomial = 7; // Sets the polynomial used for CRC to 7. ZD 18August2024
SPI_Init(SPI1, &SPI_InitStructure); // Initializes the SPI1 peripheral with the settings specified in SPI_InitStructure. ZD 18August2024

// Enable SPI1
SPI_CalculateCRC(SPI1, DISABLE); // Disables the CRC calculation feature of the SPI1 peripheral. ZD 18August2024
SPI_Cmd(SPI1, ENABLE); // Enables the SPI1 peripheral, making it ready for communication with connected devices. ZD 18August2024
}     // End SPI_Initialize()

/*!
 ********************************************************************************
 *       @details
 *******************************************************************************/
 /*
  ;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  ; Function:
  ;   SPI_ResetTransferTimeOut()
  ;
  ; Description:
  ;
  ;
  ; Parameters:
  ;
  ;
  ; Reentrancy:
  ;   No
  ;
  ;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  */

  // resets the timeout counter to 65000. ZD 18August2024
void SPI_ResetTransferTimeOut(void)
{
m_nTimeout = 65000; // sets the static variable m_nTimeout to 65000. ZD 18August2024
}     //end SPI_ResetTransferTimeOut

/*!
 ********************************************************************************
 *       @details
 *******************************************************************************/
 /*
  ;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  ; Function:
  ;   SPI_ChipSelect()
  ;
  ; Description:
  ;   Assert/deassert chip select for the SPI device selected.
  ;
  ; Parameters:
  ;   SPI_DEVICE nDevice => enum argument of device selected
  ;   BOOL bSelect => true if chip select should be asserted
  ;
  ; Reentrancy:
  ;   No
  ;
  ;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  */

  // manages the Chip Select (CS) line for various SPI devices, ensuring that only one device is selected (active) at a time on the SPI bus. ZD 18August2024
  // also tracks which device currently has control of the SPI bus to prevent conflicts. ZD 18August2024
BOOL SPI_ChipSelect(SPI_DEVICE nDevice, BOOL bSelect)
{
GPIO_TypeDef* nPort; // the GPIO port associated with the selected device's Chip Select (CS) pin. ZD 18August2024
uint16_t nPin; // The GPIO pin number associated with the device's CS line. ZD 18August2024

static SPI_DEVICE nDeviceHasBus = SPI_DEVICE_NONE; // keeps track of which device currently has control of the SPI bus. ZD 18August2024

// determines which GPIO port and pin correspond to the selected device (nDevice). ZD 18August2024
switch (nDevice)
{
case SPI_DEVICE_DATAFLASH:
nPort = SPI_CS_PORT;
nPin = DATAFLASH_CS_PIN;
break;
case SPI_DEVICE_FRAM:
nPort = SPI_CS_PORT;
nPin = FRAM_CS_PIN;
break;
default:
break;
}

// If bSelect is true, the function checks if any device currently has control of the SPI bus. ZD 18August2024
if (bSelect == true)
{
// If another device is already selected, the function returns false to indicate that the bus is already in use. ZD 18August2024
if (nDeviceHasBus != SPI_DEVICE_NONE)
{
return false;
}
else
{
nDeviceHasBus = nDevice;
}
}
// If bSelect is false, the function sets nDeviceHasBus to SPI_DEVICE_NONE, indicating that no device has control of the bus. ZD 18August2024
else
{
nDeviceHasBus = SPI_DEVICE_NONE;
}
// sets the GPIO pin associated with the CS line to either Bit_RESET (low) to select the device or Bit_SET (high) to deselect it. ZD 18August2024
GPIO_WriteBit(nPort, nPin, (bSelect ? Bit_RESET : Bit_SET)); //

return true;
}     // End SPI_ChipSelect()

/*!
 ********************************************************************************
 *       @details
 *******************************************************************************/
 /*
  ;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  ; Function:
  ;   SPI_TransferByte()
  ;
  ; Description:
  ;   Write a single byte to card via SPI and return read data.
  ;
  ; Parameters:
  ;   U_BYTE   nDataByte => Byte to write
  ;   U_INT16* pTimeout  => Countdown value
  ;
  ; Returns:
  ;   U_BYTE => Byte read back from card
  ;
  ; Reentrancy:
  ;   No
  ;
  ;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  */

U_BYTE SPI_TransferByte(U_BYTE nDataByte)
{
// checks if m_nTimeout is zero. If it is, the function immediately returns 0, indicating a failure or timeout condition. ZD 18August2024
if (m_nTimeout == 0)
{
return 0;
}

SPI_SendData(SPI1, nDataByte); // sends the nDataByte to the SPI peripheral (SPI1), initiating the data transfer over the SPI bus. ZD 18August2024

// waits for the SPI peripheral to signal that data has been received. ZD 18August2024
// The SPI_FLAG_RXNE flag is set when the receive buffer is not empty, ZD 18August2024
// meaning data has been received and is ready to be read. ZD 18August2024
while (SPI_GetFlagStatus(SPI1, SPI_FLAG_RXNE) == RESET)
{
if (--m_nTimeout == 0) // During each loop iteration, m_nTimeout is decremented by 1. ZD 18August2024
{
return 0; // if the timeout value reaches zero before data is received, the function returns 0, indicating a timeout failure. ZD 18August2024
}
}

return (SPI_ReceiveData(SPI1)); // If the data reception is successful, the function reads and returns the received byte from the SPI peripheral. ZD 18August2024
}     // End SPI_TransferByte()
