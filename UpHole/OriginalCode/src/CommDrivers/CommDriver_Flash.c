/*******************************************************************************
 *       @brief      This module is a low level hardware driver for the Flash
 *                   device.
 *       @file       Uphole/src/CommDrivers/CommDriver_Flash.c
 *       @date       December 2014
 *       @copyright  COPYRIGHT (c) 2014 Target Drilling Inc. All rights are
 *                   reserved.  Reproduction in whole or in part is prohibited
 *                   without the prior written consent of the copyright holder.
 *******************************************************************************/

 //============================================================================//
 //      INCLUDES                                                              //
 //============================================================================//
#include <stdbool.h>
#include <string.h>
#include "portable.h"
#include "CommDriver_Flash.h"
#include "CommDriver_SPI.h"
#include "crc.h"
#include "SysTick.h"
#include "timer.h"
// #include "wdt.h"

//============================================================================//
//      CONSTANTS                                                             //
//============================================================================//

#define READ_PAGE_OPCODE            0xD2
#define READ_STATUS_OPCODE          0xD7
#define ERASE_PAGE_OPCODE           0x81
#define WRITE_BUFFER_OPCODE         0x84
#define PROGRAM_PAGE_OPCODE         0x88

#define FLASH_STATUS_BUSY_BIT       0x80
#define FLASH_STATUS_BUSY           0

#define EMPTY_SLOT_CRC              0xCDD54B59

#define MBIT32_DEVICE               0x27
#define MBIT32_LAST_PAGE            8191
#define MBIT16_LAST_PAGE            4095

//============================================================================//
//      DATA DEFINITIONS                                                      //
//============================================================================//

static U_BYTE m_nDeviceSize;
static FLASH_PAGE pageData;

//============================================================================//
//      FUNCTION IMPLEMENTATIONS                                              //
//============================================================================//

/*!
 ********************************************************************************
 *       @details
 *******************************************************************************/

U_BYTE FLASH_ReadStatus(void)
{
SPI_ResetTransferTimeOut(); // resets any timeout mechanisms related to SPI data transfer. ZD 18August2024
if (SPI_ChipSelect(SPI_DEVICE_DATAFLASH, true))
{
U_BYTE status;
SPI_TransferByte(READ_STATUS_OPCODE); // sends the READ_STATUS_OPCODE to the flash memory. ZD 18August2024
status = SPI_TransferByte(0x00); // sends a dummy byte (0x00) to the flash memory while simultaneously receiving status byte from device. ZD 18August2024
SPI_ChipSelect(SPI_DEVICE_DATAFLASH, false); // deselects the flash memory device. ZD 18August2024
return status;
}
return 0; // If the flash memory device cannot be selected, return 0. ZD 18August2024
}

/*!
 ********************************************************************************
 *       @details
 *******************************************************************************/

BOOL FLASH_IsBusy(void)
{
return ((FLASH_ReadStatus() & FLASH_STATUS_BUSY_BIT) == FLASH_STATUS_BUSY); //Returns TRUE if the flash memory is busy; otherwise, returns FALSE. ZD 18August2024
}

/*!
 ********************************************************************************
 *       @details
 *******************************************************************************/

 // sends a specified number of bytes from an array through the SPI interface. ZD 18August2024
static void SendBytes(U_BYTE* bytes, int len)
{
for (int index = 0; index < len; index++)
{
SPI_TransferByte(bytes[index]);
}
}

/*!
 ********************************************************************************
 *       @details
 *******************************************************************************/

 // sends a specified number of zero-value bytes (0x00) via the SPI interface. ZD 18August2024
static void SendEmptyBytes(int len)
{
for (int index = 0; index < len; index++)
{
SPI_TransferByte(0x00);
}
}

/*!
 ********************************************************************************
 *       @details
 *******************************************************************************/

 // constructs a 32-bit value from a command and a page number, ZD 18August2024
 // then sends this value over SPI in a byte-by-byte fashion, ZD 18August2024
 // with the most significant byte being sent last. ZD 18August2024
static void SendCommand(U_BYTE command, U_INT16 pageNumber)
{
U_INT32 arguments = (command << 24) | (pageNumber << 10);
int index = sizeof(arguments);
U_BYTE* args = (U_BYTE*)&arguments;
while (index--)
{
SPI_TransferByte(args[index]);
}
}

/*!
 ********************************************************************************
 *       @details
 *******************************************************************************/

 //  reads a specified number of bytes from an SPI device and stores them in a provided array. ZD 18August2024
static void ReceiveBytes(U_BYTE* bytes, U_INT16 length)
{
for (int index = 0; index < length; index++)
{
bytes[index] = SPI_TransferByte(0x00);
}
}

/*!
 ********************************************************************************
 *       @details
 *******************************************************************************/

 // checks whether a given pageNumber is within the valid range for either a 16-megabit or a 32-megabit memory device. ZD 18August2024
static BOOL IsValidPage(U_INT32 pageNumber)
{
return (pageNumber < MBIT16_LAST_PAGE) || ((m_nDeviceSize == MBIT32_DEVICE) && (pageNumber < MBIT32_LAST_PAGE));
}

/*!
 ********************************************************************************
 *       @details
 *******************************************************************************/

 // erases a specific page in a DataFlash memory device by sending the appropriate command via SPI. ZD 18August2024
static void ErasePage(U_INT32 nPageNumber)
{
if (SPI_ChipSelect(SPI_DEVICE_DATAFLASH, true)) // selects the DataFlash device on the SPI bus. ZD 18August2024
{
SendCommand(ERASE_PAGE_OPCODE, nPageNumber); // sends the erase command along with the target page number to the DataFlash device. ZD 18August2024
SPI_ChipSelect(SPI_DEVICE_DATAFLASH, false); // deselects the DataFlash device. ZD 18August2024
}
}

/*!
 ********************************************************************************
 *       @details
 *******************************************************************************/

 // initiates the process of writing data to a specific page in a DataFlash memory device ZD. 18August2024
static void ProgramPage(U_INT32 nPageNumber)
{
if (SPI_ChipSelect(SPI_DEVICE_DATAFLASH, true)) // select the DataFlash device. ZD 18August2024
{
SendCommand(PROGRAM_PAGE_OPCODE, nPageNumber); // send the programming command along with the target page number to the device. ZD 18August2024
SPI_ChipSelect(SPI_DEVICE_DATAFLASH, false); // deselects the DataFlash device. ZD 18August2024
}
}

/*!
 ********************************************************************************
 *       @details
 *******************************************************************************/

 // reads data from a specified page in a DataFlash memory, verifies its integrity using CRC, and returns the status of the page. ZD 18August2024
FLASH_PAGE_STATUS FLASH_ReadPage(FLASH_PAGE* page, U_INT32 pageNumber)
{
if (page != NULL) //  If the page pointer is not NULL, the memory for the FLASH_PAGE structure is filled with 0xFF. ZD 18August2024
{
memset(page, 0xFF, sizeof(FLASH_PAGE));
}

if (IsValidPage(pageNumber)) // checks if the provided pageNumber is within the valid range. ZD 18August2024
{
SPI_ResetTransferTimeOut(); // resets the SPI transfer timeout, ensuring the SPI communication has enough time to complete without being prematurely terminated. ZD 18August2024

if (SPI_ChipSelect(SPI_DEVICE_DATAFLASH, true)) // selects the DataFlash device to initiate communication. ZD 18August2024
{
U_INT32 calculatedCrc, pageCrc;

SendCommand(READ_PAGE_OPCODE, pageNumber); // instructs the device to read a specific page. ZD 18August2024
SendEmptyBytes(4);
ReceiveBytes(pageData.AsBytes, FLASH_PAGE_SIZE);
ReceiveBytes((U_BYTE*)&pageCrc, sizeof(pageCrc));
SPI_ChipSelect(SPI_DEVICE_DATAFLASH, false); // deselects the DataFlash device, ending the SPI communication. ZD 18August2024

CalculateCRC(pageData.AsBytes, sizeof(pageData.AsBytes), &calculatedCrc); // Computes the CRC of the received data. ZD 18August2024
if (calculatedCrc == pageCrc) // If the calculated CRC matches the CRC read from the flash memory (pageCrc), the data is considered valid ZD 18August2024
{
if (page != NULL) // If the page is not NULL, the data is copied into the provided page structure. ZD 18August2024
{
memcpy(page->AsBytes, pageData.AsBytes, FLASH_PAGE_SIZE);
}
return FLASH_PAGE_GOOD; // indicates the page data is valid. ZD 18August2024
}
else if ((calculatedCrc == EMPTY_SLOT_CRC) && pageCrc == 0xFFFFFFFF) // Check for Empty Slot. ZD 18August2024
{
return FLASH_PAGE_EMPTY; // If calculated CRC is equal to EMPTY_SLOT_CRC and stored CRC is 0xFFFFFFFF, the page is considered empty, and the function returns FLASH_PAGE_EMPTY. ZD 18August2024
}
}
}

return FLASH_PAGE_CORRUPT; // if the CRC does not match and it's not an empty page, return 'FLASH_PAGE_CORRUPT', indicating that the data is corrupted or invalid. ZD 18August2024
}

/*!
 ********************************************************************************
 *       @details
 *******************************************************************************/

 // writes data to a specified page in a DataFlash memory device. ZD 18August2024
FLASH_PAGE_STATUS FLASH_WritePage(FLASH_PAGE* page, U_INT32 nPageNumber)
{
if (IsValidPage(nPageNumber)) // checks if the provided nPageNumber is valid. ZD 18August2024
{
U_INT32 pageCrc;
memcpy(pageData.AsBytes, page->AsBytes, FLASH_PAGE_SIZE); //  Copies the data from the provided page structure into a local buffer. ZD 18August2024
CalculateCRC(pageData.AsBytes, sizeof(pageData.AsBytes), &pageCrc); //  Computes the CRC of the data to be written. ZD 18August2024

SPI_ResetTransferTimeOut(); // resets the SPI transfer timeout to ensure that the SPI operations have enough time to complete. ZD 18August2024
ErasePage(nPageNumber); // clears the contents of the page before writing new data.  ZD 18August2024

if (SPI_ChipSelect(SPI_DEVICE_DATAFLASH, true)) // Selects the DataFlash device. ZD 18August2024
{
SendCommand(WRITE_BUFFER_OPCODE, nPageNumber); // Sends the WRITE_BUFFER_OPCODE command along with the nPageNumber to instruct the device to prepare for writing data. ZD 18August2024
SendBytes(pageData.AsBytes, FLASH_PAGE_SIZE);
SendBytes((U_BYTE*)&pageCrc, sizeof(pageCrc));
SPI_ChipSelect(SPI_DEVICE_DATAFLASH, false); // Deselects the DataFlash device. ZD 18August2024
}

if (FLASH_WaitForReady(TWENTY_FIVE_MILLI_SECONDS)) // Waits for the flash memory to be ready before starting the programming operation. ZD 18August2024
{
ProgramPage(nPageNumber); // initiates the programming of the page with the data in the buffer. ZD 18August2024
if (FLASH_WaitForReady(TWENTY_FIVE_MILLI_SECONDS)) // Waits again for the flash memory to complete the programming operation. Ensures that the write operation has finished. ZD 18August2024
{
return FLASH_PAGE_GOOD; // the page is successfully written and ready. ZD 18August2024
}
}
}
return FLASH_PAGE_CORRUPT; // If any of the wait operations fail, or if the initial page validation fails, the function returns FLASH_PAGE_CORRUPT. ZD 18August2024
}

/*!
 ********************************************************************************
 *       @details
 *******************************************************************************/

 // waits for a flash memory device to complete its current operation and become ready for new commands. ZD 18August2024
BOOL FLASH_WaitForReady(TIME_LR milliseconds)
{
TIME_LR timer = ElapsedTimeLowRes(START_LOW_RES_TIMER); // Starts a low-resolution timer to measure elapsed time. ZD 18August2024
while (FLASH_IsBusy()) // continues as long as the flash memory is busy. ZD 18August2024
{
if (ElapsedTimeLowRes(timer) > milliseconds) // checks if the elapsed time since the timer started has exceeded the specified timeout. ZD 18August2024
{
return false; // returns false if the elapsed time exceeds the timeout value. ZD 18August2024
}
}
return true; // If the flash memory is no longer busy, the function exits the loop and returns true, indicating that the memory is ready for the next operation. ZD 18August2024
}
