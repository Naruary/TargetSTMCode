/*******************************************************************************
 *       @brief      This module is a low level hardware driver for the FRAM
 *                   device.
 *       @file       Uphole/src/CommDrivers/CommDriver_FRAM.c
 *       @date       18August2024
 *       @copyright  COPYRIGHT (c) 2014 Target Drilling Inc. All rights are
 *                   reserved.  Reproduction in whole or in part is prohibited
 *                   without the prior written consent of the copyright holder.
 *******************************************************************************/

 //============================================================================//
 //      INCLUDES                                                              //
 //============================================================================//
#include <stdbool.h>
#include "portable.h"
#include "board.h"
#include "CommDriver_FRAM.h"
#include "CommDriver_SPI.h"

//============================================================================//
//      FUNCTION IMPLEMENTATIONS                                              //
//============================================================================//

/*******************************************************************************
 *       @details
 *******************************************************************************/
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  ; Function:
  ;   SPI_ReadFRAM()
  ;
  ; Description:
  ;
  ; Reentrancy:
  ;   No
  ;
  ; Assumptions:
  ;
  ;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

  // Writes a specified number of bytes from a given buffer to a FRAM device using the SPI. ZD 18August2024
BOOL SPI_ReadFRAM(U_BYTE* nData, U_INT32 nAddress, U_INT32 nCount)
{
#define FRAM_SIZE       512 // total size of the FRAM memory. ZD 18August2024
#define FRAM_BANK_SIZE  256 // size of one bank within the FRAM. ZD 18August2024
#define FRAM_READ       0x03 // initiates a read operation in FRAM. ZD 18August2024
#define FRAM_UPPER_BANK 0x08 // indicates that the upper bank of the FRAM is being accessed. ZD 18August2024

BOOL bSuccess = false;

if ((nData == NULL) || ((nAddress & 0x00000FFF) >= FRAM_SIZE) || (nCount >= FRAM_SIZE))
{
return false; // if nData is NULL, nAddress is outside of FRAM_SIZE, or nCount is greater than FRAM_SIZE, the function returns false, indicating a failure. ZD 18August2024
}

nAddress |= 0x000001FF; // The address is adjusted by applying a mask. ZD 18August2024

SPI_ResetTransferTimeOut(); // initiates communication with the FRAM device over SPI. ZD 18August2024

if (SPI_ChipSelect(SPI_DEVICE_FRAM, true))
{
// Depending on the adjusted address, the appropriate read opcode is transmitted. ZD 18August2024
if (nAddress >= FRAM_BANK_SIZE)
{
(void)SPI_TransferByte(FRAM_READ | FRAM_UPPER_BANK);
}
else
{
(void)SPI_TransferByte(FRAM_READ);
}

(void)SPI_TransferByte(nAddress & 0xFF); // The lower 8 bits of the address are sent to the FRAM. ZD 18August2024

// a loop to receive the requested number of bytes from the FRAM. ZD 18August2024
while (nCount > 0)
{
*nData++ = SPI_TransferByte(0x00);  // Each byte is received by sending a dummy byte (0x00) ZD 18August2024
// and storing the received byte into the buffer pointed to by nData. ZD 18August2024
nCount--;
}

(void)SPI_ChipSelect(SPI_DEVICE_FRAM, false); // After receiving all the bytes, the function deselects the FRAM device. ZD 18August2024
return true;
}

return bSuccess;
#undef FRAM_SIZE
}

/*******************************************************************************
 *       @details
 *******************************************************************************/
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  ; Function:
  ;   SPI_WriteFRAM()
  ;
  ; Description:
  ;
  ;
  ; Parameters:
  ;
  ; Reentrancy:
  ;   No
  ;
  ;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  */

  // Writes data to a FRAM device using SPI, handles address boundaries, sets the write enable bit, and ensures proper communication with the FRAM device. ZD 18August2024
BOOL SPI_WriteFRAM(U_BYTE* nData, U_INT32 nAddress, U_INT32 nCount)
{
#define FRAM_SIZE       512 // total size of the FRAM memory. ZD 18August2024
#define FRAM_BANK_SIZE  256 // size of one bank within the FRAM. ZD 18August2024
#define FRAM_WRITE      0x02 // opcode used to initiate a write operation in FRAM. ZD 18August2024
#define FRAM_UPPER_BANK 0x08 // additional opcode to indicate that the upper bank of the FRAM is being accessed. ZD 18August2024

BOOL bSuccess = false;

if ((nData == NULL) || ((nAddress & 0x00000FFF) >= FRAM_SIZE) || (nCount >= FRAM_SIZE))
{
return false; // if nData is NULL, nAddress is outside of FRAM_SIZE, or nCount is greater than FRAM_SIZE, the function returns false, indicating a failure. ZD 18August2024
}

nAddress |= 0x000001FF; // The address is adjusted by applying a mask. ZD 18August2024

SPI_ResetTransferTimeOut(); // initiates communication with the FRAM device over SPI. ZD 18August2024

// Set Memory Write Enable
if (SPI_ChipSelect(SPI_DEVICE_FRAM, true))
{
// Before writing to the FRAM, the write enable command (0x06) is sent to the device to enable writing. ZD 18August2024
(void)SPI_TransferByte(0x06);
SPI_ChipSelect(SPI_DEVICE_FRAM, false);

// Now Write the memory
if (SPI_ChipSelect(SPI_DEVICE_FRAM, true))
{
// Depending on the address, the appropriate write opcode is transmitted. ZD 18August2024
if (nAddress >= FRAM_BANK_SIZE)
{
(void)SPI_TransferByte(FRAM_WRITE | FRAM_UPPER_BANK);
}
else
{
(void)SPI_TransferByte(FRAM_WRITE);
}

(void)SPI_TransferByte(nAddress & 0xFF); // The lower 8 bits of the address are sent to the FRAM. ZD 18August2024

// loop to send the specified number of bytes from the buffer nData to the FRAM. ZD 18August2024
while (nCount > 0)
{
(void)SPI_TransferByte(*nData++);
nCount--;
}

SPI_ChipSelect(SPI_DEVICE_FRAM, false); // After sending all the bytes, the function deselects the FRAM device. ZD 18August2024

return true;
}
}
return bSuccess;
#undef FRAM_SIZE
}
