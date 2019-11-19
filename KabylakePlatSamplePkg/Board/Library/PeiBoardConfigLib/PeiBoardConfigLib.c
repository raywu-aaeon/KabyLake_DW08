/** @file
  Implementation of BaseBoardConfigLib.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2019 Intel Corporation.

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Intel Corporation or its suppliers or
  licensors. Title to the Material remains with Intel Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary and
  confidential information of Intel Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Intel in writing.

  Unless otherwise agreed by Intel in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Intel or
  Intel's suppliers or licensors in any way.

  This file contains a 'Sample Driver' and is licensed as such under the terms
  of your license agreement with Intel or your vendor. This file may be modified
  by the user, subject to the additional terms of the license agreement.

@par Specification Reference:
**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/EcMiscLib.h>
#ifndef MINTREE_FLAG
#include <Library/CmosAccessLib.h>
#endif //MINTREE_FLAG
#include <Library/PostCodeLib.h>
#include <Library/TimerLib.h>
#include <PchAccess.h>
#include <PlatformInfo.h>
#include <Library/BoardConfigLib.h>
#include <PlatformBoardId.h>
#ifndef MINTREE_FLAG
#include <CmosMap.h>
#endif
#include <Library/PchPmcLib.h>
#include <Library/PchInfoLib.h>
#include <Library/PchCycleDecodingLib.h>
#include <SioRegs.h>
#include <Library/PeiPlatformHookLib.h>

#include <PlatformBoardConfig.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ConfigBlockLib.h>
#include <OemSetup.h>
#include <Board/BoardConfigInit.h>
#include <Library/MultiPlatSupportLib.h>
//AMI_OVERRIDE_START - No need to show SETUP data revision in AMI setup items
#if 0
#include <SetupVariable.h>
#endif
//AMI_OVERRIDE_END - No need to show SETUP data revision in AMI setup items
#include <Library/PlatformHookLib.h>

#ifndef MINTREE_FLAG
extern VOID It8628SioParallelPortInit (VOID);
#endif

extern BOARD_CONFIG_BLOCK mSampleBoardConfigPeiPreMem;
extern BOARD_CONFIG_BLOCK mSampleBoardConfigPeiPostMem;

EFI_STATUS
InitPlatformBoardConfigBlockPreMem (
  IN CONFIG_BLOCK *ConfigBlock
);

EFI_STATUS
InitPlatformBoardConfigBlockPostMem (
  IN CONFIG_BLOCK *ConfigBlock
);

//AMI_OVERRIDE_START - No need to show SETUP data revision in AMI setup items
#if 0
#ifndef MINTREE_FLAG
typedef union {
  SETUP_DATA SetupData;
  CPU_SETUP  CpuSetup;
  ME_SETUP   MeSetup;
  SA_SETUP   SaSetup;
  PCH_SETUP  PchSetup;
} SETUP_VAR_DATA;
#endif // MINTREE_FLAG

EFI_STATUS
EFIAPI
GetVariableFromHob (
  IN     CHAR16                 *VariableName,
  IN     EFI_GUID               *VendorGuid,
  OUT    UINT32                 *Attributes, OPTIONAL
  IN OUT UINTN                  *DataSize,
  OUT    VOID                   *Data
  );

EFI_STATUS
EFIAPI
SetVariableToHob (
  IN CHAR16                     *VariableName,
  IN EFI_GUID                   *VendorGuid,
  IN UINT32                     *Attributes, OPTIONAL
  IN UINTN                      DataSize,
  IN VOID                       *Data
  );
#endif
//AMI_OVERRIDE_END - No need to show SETUP data revision in AMI setup items

//
// Definitions
//
#define STALL_PERIOD                10      // 10 microseconds
#define STALL_TIME                  100     // 100 microseconds

#ifndef MINTREE_FLAG
#define BUF_LEN                     0x16
#define ERROR_CODE_INVALID_PLATFORM_TYPE    0xBD10
#define ERROR_CODE_INVALID_PLATFORM_FLAVOR  0xBD20
#define ERROR_CODE_EC_NOT_PRESENT           0xBD30
#define ERROR_CODE_INVALID_SV_BOARD         0xBD40

typedef struct {
  UINT8 Length :4;
  UINT8 Type   :4;
} TYPE_LENGTH;

EFI_STATUS
InternalI2cRead (
  IN     UINT16   SmBusBarAddr,
  IN     UINT8    SlaveAddress,
  IN     UINT8    Offset,
  IN     UINT8    Length,
  IN OUT UINT8    *Buffer
 )
{
  UINT8     Data8;
  UINT8     Index;
  UINTN     StallIndex;
  UINTN     StallTries;

  StallTries = STALL_TIME*1000;

  //
  // Clear Host Data Registers
  //
  IoWrite8(SmBusBarAddr + R_PCH_SMBUS_HD0, 0);
  IoWrite8(SmBusBarAddr + R_PCH_SMBUS_HD1, 0);
  IoWrite8(SmBusBarAddr + R_PCH_SMBUS_HBD, 0);

  for (StallIndex = 0; StallIndex < StallTries; StallIndex++) {
    //
    //  Check for transaction errors
    //
    Data8 = IoRead8 (SmBusBarAddr + R_PCH_SMBUS_HSTS);
    if (Data8 & (B_PCH_SMBUS_DERR | B_PCH_SMBUS_BERR | B_PCH_SMBUS_INTR | B_PCH_SMBUS_HBSY)) {
      //
      // Clear Status Register
      //
      IoWrite8 (SmBusBarAddr + R_PCH_SMBUS_HSTS, B_PCH_SMBUS_HSTS_ALL);
      MicroSecondDelay (STALL_PERIOD);
     } else {
       break;
     }
   }

  //
  // Check if we timed out waiting to Status bits to clear
  //
  if (StallIndex == StallTries) {
    return EFI_TIMEOUT;
  }

  //
  // Set Auxiliary Control register
  //
  IoWrite8 (SmBusBarAddr + R_PCH_SMBUS_AUXC, 0);

  //
  // Reset the pointer of the internal buffer
  //
  IoRead8 (SmBusBarAddr + R_PCH_SMBUS_HCTL);

  //
  // Set SMBus slave address for the device to send/receive from
  // For I2c Read the value written into bit 0 of the Transmit Slave
  // SMB_Base + 04h[Bit 0 - Direction bit] should be a don't care.
  //But do to LPT PCH still interpreting this bit, even for I2C reads,
  //we need to ensure it is set to 1=read. Otherwise, all I2C Reads
  //will be blocked, if target address is Ax.
  //
  IoWrite8 (SmBusBarAddr + R_PCH_SMBUS_TSA, (SlaveAddress | B_PCH_SMBUS_READ));

  //
  // Set Host Data 1 Register
  // Provide offset address to read since FRU is an EEPROM.
  //
  IoWrite8 (SmBusBarAddr + R_PCH_SMBUS_HD1, Offset);

  //
  // Set Control Register (Initiate Operation, Interrupt disabled)
  //
  IoWrite8 (SmBusBarAddr + R_PCH_SMBUS_HCTL, (UINT8) (V_PCH_SMBUS_SMB_CMD_IIC_READ + B_PCH_SMBUS_START));

  //
  // Read data from HD0 since this FRU is an I2C device instead of SMBUS.
  //
  for (Index = 0; Index < Length; Index++) {
    for (StallIndex = 0; StallIndex < StallTries; StallIndex++) {
      //
      //  Check for transaction errors
      //
      Data8 = IoRead8 (SmBusBarAddr + R_PCH_SMBUS_HSTS);
      if (Data8 & (B_PCH_SMBUS_DERR | B_PCH_SMBUS_BERR)) {
        return EFI_DEVICE_ERROR;
      } else {
        //
        // Check for transaction completed
        //
        Data8 = IoRead8 (SmBusBarAddr + R_PCH_SMBUS_HSTS);
        if (Data8 & B_PCH_SMBUS_BYTE_DONE_STS) {
          //
          // Read one byte of Data from Host Block Data Register
          //
          *(Buffer)  = IoRead8 (SmBusBarAddr + R_PCH_SMBUS_HBD);
          break;
        } else {
          MicroSecondDelay (STALL_PERIOD);
        }
      }
    }

    //
    // Check this read is the n-1 byte or first byte is the last byte
    //
    if(Index == (Length - 1)) {
      //
      // Set the Last Byte Bit in the Host Control Register
      //
      Data8 = IoRead8(SmBusBarAddr + R_PCH_SMBUS_HCTL);
      Data8 |= B_PCH_SMBUS_LAST_BYTE;
      IoWrite8 (SmBusBarAddr + R_PCH_SMBUS_HCTL, Data8);
    }

    //
    // Clear the Byte Done Status bit in the Host Status Register
    //
    IoWrite8 (SmBusBarAddr + R_PCH_SMBUS_HSTS, B_PCH_SMBUS_BYTE_DONE_STS);
    Buffer++;
  }

  //
  // Clear Status Registers and exit
  //
  IoWrite8 (SmBusBarAddr + R_PCH_SMBUS_HSTS, B_PCH_SMBUS_HSTS_ALL);

  return EFI_SUCCESS;
}

/**
  Update Board ID

  @param[in]  OsProfile
  @param[in]  ActualBoardId

  @retval     ActualBoardId
**/
UINT16
UpdateBoardId (
  IN UINT8  OsProfile,
  IN UINT16 ActualBoardId
 )
{
  if (ActualBoardId == BoardIdSkylakeSds) {
    switch (OsProfile) {
      case OsAndroid:
        return BoardIdSkylakeSdsAndroid;

      case OsWinCs:
        return BoardIdSkylakeSdsWinCs;

      case OsWinNonCs:
        return BoardIdSkylakeSdsWinNonCs;

      default:
        return ActualBoardId;
    } // switch (OsProfile)
  } // if (ActualBoardId == BoardIdSkylakeSds)

  return ActualBoardId;
}

EFI_STATUS
InternalSmbusInit (
  UINT16 SmbusIoBar
  )
{
  UINTN  SmbusPciBase;

  //
  // Temporary initialize SMBUS
  //
  SmbusPciBase = (UINTN)PcdGet64 (PcdPciExpressBaseAddress) + \
    PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SMBUS, PCI_FUNCTION_NUMBER_PCH_SMBUS, 0);

  //
  // Initialize I/O BAR
  //
  MmioWrite16(SmbusPciBase + R_PCH_SMBUS_BASE, SmbusIoBar);

  //
  // Enable the Smbus I/O Enable
  //
  MmioOr8 (SmbusPciBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_IO_SPACE);

  //
  // Enable the Smbus host controller
  //
  MmioAndThenOr8 (
    SmbusPciBase + R_PCH_SMBUS_HOSTC,
    (UINT8) (~(B_PCH_SMBUS_HOSTC_SMI_EN | B_PCH_SMBUS_HOSTC_I2C_EN)),
    B_PCH_SMBUS_HOSTC_HST_EN
    );

  return EFI_SUCCESS;
}

VOID
InternalSmbusDone (
  VOID
  )
{
  UINTN  SmbusPciBase;

  SmbusPciBase = (UINTN)PcdGet64 (PcdPciExpressBaseAddress) + \
    PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SMBUS, PCI_FUNCTION_NUMBER_PCH_SMBUS, 0);

  //
  // Restore Smbus device config
  // Disable the Smbus I/O Enable
  //
  MmioAnd8 (SmbusPciBase + PCI_COMMAND_OFFSET, (UINT8)~EFI_PCI_COMMAND_IO_SPACE);

  //
  // Clear I/O BAR
  //
  MmioWrite32(SmbusPciBase + R_PCH_SMBUS_BASE, 0);

  //
  // Disable the Smbus host controller
  //
  MmioAnd8 (SmbusPciBase + R_PCH_SMBUS_HOSTC, (UINT8)~B_PCH_SMBUS_HOSTC_HST_EN);
}

/**
  This function initializes the Smbus Registers to perform I2C transaction to read
  Board Product Name from the FRU EEPROM.

  @param[in]      SmbusIoBar    Smbus Io Bar
  @param[in][out] Buffer        Pointer of Board Product Name read from FRU EEPROM.
  @param[in]      BufLen        Lenght of Buffer

  @retval     Status       Status of the I2C transaction.
**/
EFI_STATUS
InternalReadFruDevice (
  IN     UINT16   SmbusIoBar,
  IN OUT UINT8    *Buffer,
  IN     UINT8    BufLen
  )
{
  EFI_STATUS    Status;
  UINT8         FruDeviceAddress;
  UINT8         Offset;
  UINT8         Length;

  FruDeviceAddress = 0xAE;

  //
  // Step 1.
  // Read the Board Info Area Starting offset located at
  // offset 0x03 in the Common Header section as per FRU spec.
  //
  Offset = 0x03;
  Length = 0x01;

  //
  // Read one byte
  //
  Status = InternalI2cRead (
             SmbusIoBar,
             FruDeviceAddress,
             Offset,
             Length,
             Buffer
             );

  if(!EFI_ERROR(Status) && (*Buffer != 0x0)) {
   //
   // Board Area Starting Offset (in multiples of 8 bytes).
   //
   *Buffer = *Buffer * 0x08;

   // Step 1A.
   // Read Board Product Name from the Board Info Area offset 0x0D.
   //
   Offset = (UINT8) *Buffer + 0x06;
   Length = 0x01;

   ZeroMem ((VOID*)Buffer, sizeof (Buffer));

   //
   // Read 1 byte. Board Manufacturer type/length
   //
   Status = InternalI2cRead (
              SmbusIoBar,
              FruDeviceAddress,
              Offset,
              Length,
              Buffer
              );

   //
   // Step 2.
   // Read Board Product Name from the Board Info Area .
   //
   // read length of Board Product Name field
   Offset += ((UINT8) (*(TYPE_LENGTH *)Buffer).Length + 0x01); //  +1 Board Manufacturer type/length field; *Buffer - length of this field
   Length = 0x01;

   ZeroMem ((VOID*)Buffer, sizeof (Buffer));

   //
   // Read 1 byte.  Board Product Name
   //
   Status = InternalI2cRead (
              SmbusIoBar,
              FruDeviceAddress,
              Offset,
              Length,
              Buffer
              );
   // Read Board Product Name from the Board Info Area offset 0x0D.

   Length = (UINT8) (*(TYPE_LENGTH *)Buffer).Length;
   if( Length >= BufLen )
     Length=BufLen;
   Offset++;

   ZeroMem ((VOID*)Buffer, sizeof (Buffer));

   Status = InternalI2cRead (
              SmbusIoBar,
              FruDeviceAddress,
              Offset,
              Length,
              Buffer
              );
  } else {
    Status = EFI_NOT_FOUND;
  }

  return Status;
}

/**
  Updates BoardConfig hob with SV platform Information

  @param[in]  BoardId       BoardId ID as determined through the FRU.
  @param[out] BoardConfig  BoardConfig pointer with PlatformFlavor specified.
**/
VOID
InternalUpdateSvBoardConfig (
  IN  UINT16          BoardId,
  OUT BOARD_CONFIG    *BoardConfig,
  OUT UINT16              *BoardIdRet
  )
{
  *BoardIdRet                     = BoardId;
  BoardConfig->BoardRev           = 0;
  BoardConfig->BoardBomId         = 0;
  BoardConfig->PlatformGeneration = 0;
  BoardConfig->EcPresent          = FALSE;
  BoardConfig->BoardType          = BoardTypeSv;
}

/**
  Determine type of HSIO card used in Karkom.

  @param[out]  BoardConfig   Platform information structure to be updated
  @parem[in]   SmbusIoBar     IO BAR for SMBUS controller (must be enabled)
**/
VOID
InternalUpdateHsioCardType (
  IN OUT BOARD_CONFIG   *BoardConfig,
  IN     UINT16         SmbusIoBar
  )
{
   UINT8        HsioId[2];
   UINT8        CardType;
   UINT8        CardEco;
   EFI_STATUS   Status;

   //
   // Read HSIO board ID from PCA9555 on HSIO board at address 0x4A,
   // command 0 with two bytes of data
   //
   Status = InternalI2cRead (SmbusIoBar, 0x4A, 0, 2, HsioId);
   if (Status == EFI_SUCCESS) {
     CardType = HsioId[0] >> 4;
     CardEco  = HsioId[0] & 0xF;
     DEBUG ((DEBUG_INFO, "HSIO: board type: 0x%02x, ECO %d\n", CardType, CardEco));
     BoardConfig->HsioBoardPresent = TRUE;
     BoardConfig->HsioBoardType = CardType;
   } else {
     DEBUG ((DEBUG_INFO, "HSIO: Failed to detect HSIO board (%r)!\n", Status));
     BoardConfig->HsioBoardPresent = FALSE;
   }
}

/**
  This function Detects the Presence of SvBoard

  @param[in]  BoardConfig    Pointer to Platform Info

  @retval     TRUE            Sv Board Present
  @retval     FALSE           Sv Board not Present
**/
BOOLEAN
InternalDetectSvBoard (
  OUT BOARD_CONFIG    *BoardConfig,
  IN  UINTN           BoardNameSize,
  IN OUT UINT16       *BoardId
  )
{
  EFI_STATUS          Status;
  CHAR8               BoardName[BUF_LEN] = {0};
  BOOLEAN             SvBoard;
  UINT16              SmbusIoBar;

  SmbusIoBar = 0xEFA0;

  SvBoard = FALSE;

  InternalSmbusInit(SmbusIoBar);

  //
  // Check for SV board via FRU EEPROM
  //
  Status = InternalReadFruDevice (SmbusIoBar, (UINT8 *) BoardName, BUF_LEN);

  if(!EFI_ERROR(Status)){
    DEBUG((DEBUG_INFO, "InternalDetectSvBoard::InternalReadFruDevice returned %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x \"%a\"\n",
      BoardName[0], BoardName[1], BoardName[2], BoardName[3], BoardName[4], BoardName[5], BoardName[6], BoardName[7], BoardName[8], BoardName[9],
      BoardName[10], BoardName[11],BoardName[12],BoardName[13],BoardName[14],BoardName[15], BoardName));
    if (AsciiStrnCmp(&BoardName[0], "VernalFalls", AsciiStrLen("VernalFalls")) == 0) {
      InternalUpdateSvBoardConfig (BoardIdPineValley, BoardConfig, BoardId);
      SvBoard = TRUE;
      BoardConfig->PlatformFlavor = FlavorDesktop;
      BoardConfig->PlatformType   = TypeTrad;
      AsciiStrCatS (BoardConfig->BoardName , BoardNameSize, "Pine Valley");
      DEBUG ((DEBUG_INFO, "    Vernal Falls --> Pine Valley\n"));
    } else {
      //
      // Invalid SV Board
      //
      SvBoard = FALSE;
      IoWrite16 (0x80, ERROR_CODE_INVALID_SV_BOARD);
      DEBUG ((DEBUG_INFO, "Invalid Sv Board\n"));
      ASSERT_EFI_ERROR (EFI_INVALID_PARAMETER);
    }
  }

  if (SvBoard) {
    InternalUpdateHsioCardType (BoardConfig, SmbusIoBar);
  }

  InternalSmbusDone ();

  return SvBoard;
}

/**
  This routine detects if the current platform is an UP Server system or not

  @retval   TRUE      Board is an UP Server system.
  @retval   FALSE     Board is NOT an UP Server system.
**/
BOOLEAN
InternalCheckForUpServerPlatform (
  VOID
  )
{
  BOOLEAN Status = FALSE;

  UINT16  LpcIoeOrg;
  UINT16  LpcIoe;
  UINTN   LpcBaseAddress;

  //
  // Enable LPC IO decode temporarily for SIO access
  //
  LpcBaseAddress = (UINTN)PcdGet64 (PcdPciExpressBaseAddress) + \
  PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_LPC, 0);
  LpcIoeOrg = MmioRead16 (LpcBaseAddress + R_PCH_LPC_IOE);
  LpcIoe = LpcIoeOrg | B_PCH_LPC_IOE_ME2;
  PchLpcIoEnableDecodingSet (LpcIoe);

  IoWrite8 (PILOTIII_SIO_INDEX_PORT, PILOTIII_UNLOCK); // Enter Config
  if (IoRead8 (PILOTIII_SIO_INDEX_PORT) != 0xFF) {
    //
    // Check ID for SIO PILOT3
    //
    IoWrite8 (PILOTIII_SIO_INDEX_PORT, PILOTIII_CHIP_ID_REG);
    if (IoRead8 (PILOTIII_SIO_DATA_PORT) == PILOTIII_CHIP_ID) {
      Status = TRUE;
    }
  }
  IoWrite8 (PILOTIII_SIO_INDEX_PORT, PILOTIII_LOCK); // Exit Config
  //
  // This disables decoding on the LPC for the super I/O
  //
  PchLpcIoEnableDecodingSet (LpcIoeOrg);

  return Status;
}

/**
  This routine detects if the current platform is an Embedded Platform  or not

  @param[out] BoardConfig  Pointer to Platform Info

  @retval     TRUE          Board is an Embedded Platform.
  @retval     FALSE         Board is NOT an Embedded Platform.
**/
BOOLEAN
InternalCheckForEmbeddedPlatform (
  OUT BOARD_CONFIG    *BoardConfig
  )
{
  BOOLEAN Status = FALSE;
#ifdef EMBEDDED_FLAG
  CHAR8 SioId[2] = {0};

  //
  // Try to open SIO configuration space by writing 0x87 twice to port 0x2e.
  //
  IoWrite8 (0x2e, 0x87);
  IoWrite8 (0x2e, 0x87);

  //
  // Read SioID (registers 0x20 and 0x21 upper nibble)
  //
  IoWrite8 (0x2e, 0x20);
  SioId[0] = IoRead8 (0x2f);
  IoWrite8 (0x2e, 0x21);
  SioId[1] = (CHAR8) (0xf0 & IoRead8 (0x2f));

  //
  // If NCT6776F id found then we've detected an Embedded Platform
  //
  DEBUG ((DEBUG_INFO, "SIO data[0]= 0x%X\n",SioId[0]));
  DEBUG ((DEBUG_INFO, "SIO data[1]= 0x%X\n",SioId[1]));
  if (((0xff & SioId[0]) == 0xC3) && (SioId[1] == 0x30)) {
    DEBUG ((DEBUG_INFO, "SIO NCT6776F found\n"));
    Status = TRUE;
    IoWrite8 (0x2e, 0x2a);
    IoWrite8 (0x2f, 0x40);
  }

  //
  // Try to close the SIO. If there's no SIO or if it was not opened by
  // writting 0x87 twice to port 0x2e then this code will do nothing and
  // is harmless
  //
  IoWrite8 (0x2e, 0xaa);

#endif // EMBEDDED_FLAG

  return Status;
}

/**
  Check if the Platform is OC platform or not

  @retval     TRUE      Platform is OC type.
  @retval     FALSE     Platform is NOT OC type.
**/

BOOLEAN
InternalDetectOcBoard (
  VOID
  )
{
  UINT16       BoardId;
  GPIO_CONFIG  GpioConfig;
  UINT32       BoardIdBit7GpioVal = 0;
  UINT32       BoardIdBit6GpioVal = 0;
  UINT32       BoardIdBit5GpioVal = 0;
  UINT32       BoardIdBit4GpioVal = 0;
  UINT32       BoardIdBit3GpioVal = 0;
  UINT32       BoardIdBit2GpioVal = 0;
  UINT32       BoardIdBit1GpioVal = 0;
  UINT32       BoardIdBit0GpioVal = 0;

  //
  // Configure the GPIO before reading
  //
  ZeroMem(&GpioConfig, sizeof(GPIO_CONFIG));
  GpioConfig.PadMode = GpioPadModeNative1;
  GpioConfig.HostSoftPadOwn = GpioHostOwnGpio;
  GpioConfig.Direction = GpioDirIn;
  GpioConfig.OutputState = GpioOutDefault;
  GpioConfig.InterruptConfig = GpioIntDis;
  GpioConfig.PowerConfig = GpioPlatformReset;
  GpioConfig.ElectricalConfig = GpioTermNone;

  GpioSetPadConfig (GPIO_SKL_H_GPP_H6, &GpioConfig);
  GpioSetPadConfig (GPIO_SKL_H_GPP_H7, &GpioConfig);
  GpioSetPadConfig (GPIO_SKL_H_GPP_H5, &GpioConfig);
  GpioSetPadConfig (GPIO_SKL_H_GPP_H4, &GpioConfig);
  GpioSetPadConfig (GPIO_SKL_H_GPP_B17, &GpioConfig);
  GpioSetPadConfig (GPIO_SKL_H_GPP_B18, &GpioConfig);
  GpioSetPadConfig (GPIO_SKL_H_GPP_B22, &GpioConfig);
  GpioSetPadConfig (GPIO_SKL_H_GPP_H12, &GpioConfig);

  //
  // Read Gpio to get the BoardId
  // GPP_H4 - bit 4,  GPP_H5 - bit 5,  GPP_H7 - bit 6,  GPP_H6 - bit 7
  // GPP_H12 - bit 0, GPP_B22 - bit 1, GPP_B18 - bit 2, GPP_B17 - bit 3
  //
  GpioGetInputValue (GPIO_SKL_H_GPP_H6, &BoardIdBit7GpioVal);
  GpioGetInputValue (GPIO_SKL_H_GPP_H7, &BoardIdBit6GpioVal);
  GpioGetInputValue (GPIO_SKL_H_GPP_H5, &BoardIdBit5GpioVal);
  GpioGetInputValue (GPIO_SKL_H_GPP_H4, &BoardIdBit4GpioVal);
  GpioGetInputValue (GPIO_SKL_H_GPP_B17, &BoardIdBit3GpioVal);
  GpioGetInputValue (GPIO_SKL_H_GPP_B18, &BoardIdBit2GpioVal);
  GpioGetInputValue (GPIO_SKL_H_GPP_B22, &BoardIdBit1GpioVal);
  GpioGetInputValue (GPIO_SKL_H_GPP_H12, &BoardIdBit0GpioVal);

  BoardId = (UINT16) ((BoardIdBit7GpioVal << 7) |(BoardIdBit6GpioVal << 6) |(BoardIdBit5GpioVal << 5) | (BoardIdBit4GpioVal << 4) | (BoardIdBit3GpioVal << 3) |(BoardIdBit2GpioVal << 2) |(BoardIdBit1GpioVal << 1) | BoardIdBit0GpioVal);

  if (BoardId == BoardIdKabyLakeOc) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Updates BoardConfig hob with SV platform Information

  @param[out] BoardConfig  BoardConfig pointer with PlatformFlavor specified.
**/
VOID
InternalUpdateUpServerBoardConfig (
  OUT BOARD_CONFIG    *BoardConfig,
  IN OUT UINT16       *BoardId,
  IN OUT UINT16       *BoardIdOrgValue
  )
{
  UINT32    BoardIdBit0GpioVal;
  UINT32    BoardIdBit2GpioVal;
  UINT32    BoardIdBit1GpioVal;
  UINT8     ZBPinBoardId ;

  // Zumba Beach Board REV PINs are:
  //FM_BOARD_REV_ID0, GPP_G_15
  //FM_BOARD_REV_ID1, GPP_G_16
  UINT32    BoardRevBit1GpioVal;
  UINT32    BoardRevBit0GpioVal;

  BoardConfig->EcPresent          = FALSE;
  BoardConfig->BoardType          = BoardTypeUpServer;
  BoardConfig->PlatformFlavor = FlavorUpServer;
  BoardConfig->PlatformType = TypeTrad;

  // Zumba Beach SKU Board ID PINs are:
  // FM_BOARD_SKU_ID0 : GPP_G_12
  // FM_BOARD_SKU_ID1 : GPP_G_13
  // FM_BOARD_SKU_ID2 : GPP_G_14 //UP_ZUMBA_BEACH Workstation not POR ID2 is depreciated

  // ---  ---  ---  --------------------------
  // ID2  ID1  ID0  BOARD
  // ---  ---  ---  --------------------------
  //  1    1    0   Server DDR4, EV
  //  1    1    1   PPO,  not available in SPT
  //  1    0    1   Server DDR4, CRB
  //  1    0    0   Reserved

  GpioConfigurePads (sizeof (UpZumbaBeachGpioIdTable) / sizeof (GPIO_INIT_CONFIG),UpZumbaBeachGpioIdTable);

  //Default Configuration for GPP_G_16 GPP_G_15 GPP_G_14 GPP_G_13 and GPP_G_12  is GP-In
  GpioGetInputValue (UP_B_ID_BIT2_GPIO_PAD,  &BoardIdBit2GpioVal);
  GpioGetInputValue (UP_B_ID_BIT1_GPIO_PAD,  &BoardIdBit1GpioVal);
  GpioGetInputValue (UP_B_ID_BIT0_GPIO_PAD,  &BoardIdBit0GpioVal);

  GpioGetInputValue (UP_REV_ID_BIT1_GPIO_PAD,  &BoardRevBit1GpioVal);
  GpioGetInputValue (UP_REV_ID_BIT0_GPIO_PAD,  &BoardRevBit0GpioVal);

  BoardConfig->BoardRev = (UINT8) ( (BoardRevBit1GpioVal<< 1) | (BoardRevBit0GpioVal) );

  ZBPinBoardId = (UINT8) ( (BoardIdBit2GpioVal<< 2) | (BoardIdBit1GpioVal<< 1) | (BoardIdBit0GpioVal) );
  DEBUG ((DEBUG_INFO, "ZBPinBoardId = 0x%x\n",ZBPinBoardId));
  switch (ZBPinBoardId) {
    case 0x6:
      *BoardId = BoardIdZumbaBeachServerEv;
      DEBUG ((DEBUG_INFO, "UP Server, Zumba Beach EV detected!!!!\n"));
      break;
    case 0x5:
      *BoardId = BoardIdZumbaBeachServerCrb;
      DEBUG ((DEBUG_INFO, "UP Server, Zumba Beach CRB detected!!!!\n"));
      break;
    default:
      *BoardId =  BoardIdZumbaBeachServerEv;
      DEBUG ((DEBUG_INFO, "BoardTypeUpServer,  BoardIdUnknown detected , Select Zumba Beach EV as default!!!!\n"));
       // Reserved
      break;
  }
  *BoardIdOrgValue = *BoardId;
}

/**
  Updates BoardConfig hob with Embedded platform Information

  @param[out] BoardConfig  BoardConfig pointer with PlatformFlavor specified.
**/
VOID
InternalUpdateEmbeddedBoardConfig (
  OUT BOARD_CONFIG    *BoardConfig,
  IN OUT UINT16       *BoardId,
  IN OUT UINT16       *BoardIdOrgValue
  )
{
#ifdef EMBEDDED_FLAG
  *BoardId            = BoardIdSkylakeSdlBrk;
#else
  *BoardId            = BoardIdUnknown1;
#endif
  *BoardIdOrgValue    = *BoardId;
  BoardConfig->BoardRev           = 0;
  BoardConfig->BoardBomId         = 0;
  BoardConfig->PlatformGeneration = 0;
  BoardConfig->EcPresent          = FALSE;
  BoardConfig->BoardType          = BoardTypeRvp;
  BoardConfig->PlatformType       = TypeTrad;
  BoardConfig->PlatformFlavor     = FlavorEmbedded;
}

#define B_ID_BIT3_GPIO_PIN                              0                    // Platform specific @ TBD
#define B_ID_BIT2_GPIO_PIN                              0                    // Platform specific @ TBD
#define B_ID_BIT1_GPIO_PIN                              0                    // Platform specific @ TBD
#define B_ID_BIT0_GPIO_PIN                              0                    // Platform specific @ TBD

/**
  Check if the Platform is Cpv or Ppv platform

  @param[out] BoardConfig     BoardConfig pointer with PlatformFlavor specified.
**/
VOID
InternalUpdateCpvPpvPlatform (
  OUT BOARD_CONFIG    *BoardConfig,
  IN OUT UINT16       *BoardIdRet
  )
{
  UINT16 BoardId;

  UINT32    BoardIdBit3GpioVal;
  UINT32    BoardIdBit2GpioVal;
  UINT32    BoardIdBit1GpioVal;
  UINT32    BoardIdBit0GpioVal;

  BoardIdBit3GpioVal = 0;
  BoardIdBit2GpioVal = 0;
  BoardIdBit1GpioVal = 0;
  BoardIdBit0GpioVal = 0;

  //
  // Read Gpio to get the BoardId
  //
  //GpioGetInputValue (B_ID_BIT3_GPIO_PIN,  &BoardIdBit3GpioVal); //@todo, uncomment when B_ID_BIT3_GPIO_PIN has been assigned correct GpioPad
  //GpioGetInputValue (B_ID_BIT2_GPIO_PIN,  &BoardIdBit2GpioVal); //@todo, uncomment when B_ID_BIT2_GPIO_PIN has been assigned correct GpioPad
  //GpioGetInputValue (B_ID_BIT1_GPIO_PIN,  &BoardIdBit1GpioVal); //@todo, uncomment when B_ID_BIT1_GPIO_PIN has been assigned correct GpioPad
  //GpioGetInputValue (B_ID_BIT0_GPIO_PIN,  &BoardIdBit0GpioVal); //@todo, uncomment when B_ID_BIT0_GPIO_PIN has been assigned correct GpioPad
  BoardId = (UINT16) ((BoardIdBit3GpioVal << 3) |(BoardIdBit2GpioVal<< 2) |(BoardIdBit1GpioVal<< 1) | BoardIdBit0GpioVal);

  switch (BoardId) {
    default:
      *BoardIdRet = BoardIdUnknown1;
      break;
  }
}
#endif // MINTREE_FLAG

/**
  Returns the BoardId ID of the platform from the EC for Mobile.

  @param[in]  BoardConfig      BoardConfig pointer with PlatformFlavor specified.
  @param[in]  BoardId           BoardId ID as determined through the EC.

  @retval     EFI_SUCCESS       The function completed successfully.
  @retval     EFI_DEVICE_ERROR  EC fails to respond.
**/
EFI_STATUS
EFIAPI
InternalGetBoardId (
  IN  BOARD_CONFIG    *BoardConfig,
  OUT UINT16          *BoardId
  )
{
  EFI_STATUS    Status;
  UINT16        EcBoardInfo;
  UINT8         DataBuffer[2];

  Status = EFI_SUCCESS;

  //
  // Return BoardIdUnknown1 in case of error.
  //
  *BoardId = BoardIdUnknown1;

  Status = GetBoardFabId (DataBuffer);
  if (Status == EFI_SUCCESS) {
    EcBoardInfo = DataBuffer[0];
    EcBoardInfo = (EcBoardInfo << 8) | DataBuffer[1];
    //
    // Get the following data:
    // [7:0]  -  BOARD_IDx
    // [8]    -  GEN_ID
    // [11:9] -  REV_FAB_IDx
    // [12]   -  TP_SPD_PRSNT
    // [15:13] - BOM_IDx
    //
    *BoardId = (UINT8) (EcBoardInfo & BOARD_ID_MASK_8BIT);
    if (*BoardId == BoardIdAmberLakeY42Lpddr3Cff) {
      //
      // Get the following data:
      // [4:0]  -  BOARD_IDx  // Here BOM Id will be a part of Board ID i;e; BOARD_IDx = [7:0]
      // [7:5]  -  BOM_IDx
      // [9:8]  -  REV_FAB_IDx
      // [10]   -  MAF_SAF_DETECT
      // [12]   -  VIRTUAL_BATTERY
      // [14]   -  RETIMER_BYPASS_STRAP
      DEBUG ((DEBUG_INFO, "Board is detected as AML AEP board with Board ID = %x\n",*BoardId));
      BoardConfig->BoardRev = (UINT8) ((EcBoardInfo >> 8) & (0x3));

      //BoardBomId is assinged with Memory Vendor for AML Y42 AEP
      //Bit 4 Bit 6   Memory Part
      // 1      0     K44922-001, SKYNIX 8GB
      // 1      1     Micron MT52L512M64D4PQ-093 WT:B 4G
      // 0      1     Micron MT52L1G64D8QC-107 WT:B, 8GB
      BoardConfig->BoardBomId = (UINT8) ((((EcBoardInfo >>14)& (0x1))<<(0x1))|(((EcBoardInfo >>12)& (0x1))));
      BoardConfig->SpdPresent = 0;
    } else if (*BoardId == BoardIdAmberLakeY42Lpddr3CffCc) {//to do if EC send data in diffenrt format from AEP
      DEBUG ((DEBUG_INFO, "Board is detected as AML AEP CC board with Board ID = %x\n",*BoardId));
      BoardConfig->BoardRev = (UINT8) ((EcBoardInfo >> 8) & (0x3));

      //BoardBomId is assinged with Memory Vendor for AML Y42 AEP
      //Bit 4 Bit 6   Memory Part
      // 1      0     K44922-001, SKYNIX 8GB
      // 1      1     Micron MT52L512M64D4PQ-093 WT:B 4G
      // 0      1     Micron MT52L1G64D8QC-107 WT:B, 8GB
      BoardConfig->BoardBomId = (UINT8) ((((EcBoardInfo >>14)& (0x1))<<(0x1))|(((EcBoardInfo >>12)& (0x1))));
      BoardConfig->SpdPresent = 0;
    } else {
      BoardConfig->BoardRev = (UINT8) ((EcBoardInfo >> 9) & (FAB_ID_MASK));
      BoardConfig->BoardBomId = (UINT8) ((EcBoardInfo >> 13) & (BOM_ID_MASK));
      BoardConfig->PlatformGeneration = (UINT8) ((EcBoardInfo & GENERATION_MASK) >> 8);
      BoardConfig->SpdPresent = (BOOLEAN) ((EcBoardInfo & BIT12) >> 12);
    }
  }
  return Status;
}

VOID
EFIAPI
InternalUpdateRvpBoardConfig (
  IN OUT BOARD_CONFIG   *BoardConfig,
  IN OUT UINT16         *BoardId,
  IN OUT UINT16         *BoardIdOrgValue
  )
{
  EFI_STATUS    Status;
  UINT16        RvpBoardId;
#ifndef MINTREE_FLAG
  UINT8         EcData;
  UINT16        LpcDeviceId;
#endif // MINTREE_FLAG

  DEBUG ((DEBUG_INFO, "Reading Board ID from EC ...\n"));
  do {
    Status = InternalGetBoardId (BoardConfig, &RvpBoardId);
  } while (Status != EFI_SUCCESS);
  DEBUG ((DEBUG_INFO, "Got Board ID from EC: 0x%x  \n", RvpBoardId));
  *BoardId = RvpBoardId;
  *BoardIdOrgValue = RvpBoardId;
#ifndef MINTREE_FLAG
  if (RvpBoardId == BoardIdKabylakeUDdr3lRvp7) { // If Board ID matches to Kabylake U Ddr3l Rvp7 then override it to Skylake U Rvp7 Board ID
    *BoardId = BoardIdSkylakeURvp7;
  }
  if (RvpBoardId == BoardIdKabyLakeYLpddr3Rvp3 ||
      RvpBoardId == BoardIdAmberLakeY42Lpddr3Rvp3 ||
      RvpBoardId == BoardIdAmberLakeLpddr3Rvp3) { // If Board ID matches to AmberLake, AmberLake-Y(4+2)  or KabyLake Y Lpddr3 Rvp3 then override it to Skylake Y Rvp3 Board ID
    *BoardId = BoardIdSkylakeA0Rvp3;
  }
  if ((RvpBoardId == BoardIdAmberLakeY42Lpddr3Cff) ||
      (RvpBoardId == BoardIdAmberLakeY42Lpddr3CffCc)) { // If Board ID matches to AML Y 42  AEP
    *BoardId = BoardIdSkylakeA0Rvp3;
  }
  if (RvpBoardId == BoardIdHaloDdr4uSffCpv) { // If Board ID matches to KabyLake uSFF Cpv Platform then override it to Skylake Y Rvp10 Board ID
    *BoardId = BoardIdSkylakeAioRvp10Crb;
  }
  if ((RvpBoardId == BoardIdKabyLakeSDdr4UdimmEvErb) ||
      (RvpBoardId == BoardIdKabyLakeSDdr4UdimmEvCrb) ||
      (RvpBoardId == BoardIdKabyLakeSDdr4UdimmCrb)) { // If Board ID matches to KabyLake S EV ERB / CRB then override it to Skylake AIO Rvp8 Board ID
    *BoardId = BoardIdSkylakeDtRvp8Crb;
  }
  if ((RvpBoardId == BoardIdKabylakeULpddr3Doe) ||
      (RvpBoardId == BoardIdKabylakeULpddr3Doe1) ||
      (RvpBoardId == BoardIdCannonlakeULpddr3Rvp5)) {
    *BoardId = BoardIdSkylakeULpddr3Rvp5;
  }
  if ((RvpBoardId == BoardIdSkylakeHaloDdr4Rvp11) || (RvpBoardId == BoardIdSkylakeHaloLpddr3Rvp16)) { // If Board ID matches to Kabylake Halo RVPs, we need to check if SPT-H or uSFF PCH is connected on the board
    LpcDeviceId = GetLpcDid();
    switch (LpcDeviceId) {
      case V_SKL_PCH_H_LPC_DEVICE_ID_A14B:
        *BoardIdOrgValue = BoardIdHaloDdr4uSFF;
        break;

      default:
        break;
    }
  }
  RvpBoardId = *BoardId;
  //
  // Update Platform Flavor
  //
  if (RvpBoardId <= BoardIdMobileMax) {
    BoardConfig->PlatformFlavor = FlavorMobile;
  } else if ((RvpBoardId > BoardIdMobileMax) && (RvpBoardId <= BoardIdDesktopMax)) {
    BoardConfig->PlatformFlavor = FlavorDesktop;
  }

  //
  // Update Platform Type
  //
  if ((RvpBoardId <= BoardIdTradMobileMax) || ((RvpBoardId > BoardIdMobileMax) && (RvpBoardId <= BoardIdDesktopMax))) {
    BoardConfig->PlatformType = TypeTrad;
  } else if ((RvpBoardId > BoardIdTradMobileMax) && (RvpBoardId <= BoardIdMobileMax)) {
    BoardConfig->PlatformType = TypeUltUlx;
  }
#endif // MINTREE_FLAG

  switch (RvpBoardId) {
#ifndef MINTREE_FLAG
    case BoardIdSkylakeA0Lpddr3Crb:
    case BoardIdSkylakeA0Ddr4Rvp2:
    case BoardIdSkylakeA0Rvp3:
    case BoardIdSkylakeULpddr3Rvp5:
    case BoardIdSkylakeLpdimmDoe:
    case BoardIdSkylakeURvp7:
    case BoardIdKabylakeRDdr4:
    case BoardIdKabylakeRLpddr3:
    case BoardIdKabylakeKcDdr3:
    case BoardIdSkylakeYRvp13:
    case BoardIdAmberLakeLpddr3Rvp3:
    case BoardIdAmberLakeLpddr3Rvp13:
    case BoardIdSkylakeURvp15:
      BoardConfig->BoardType = BoardTypeRvp;
      BoardConfig->PlatformType = TypeUltUlx;
      BoardConfig->PlatformFlavor = FlavorMobile;
      break;
    case BoardIdSkylakeDtRvp8Crb:
    case BoardIdSkylakeAioRvp9Crb:
    case BoardIdSkylakeAioRvp10Crb:
    case BoardIdSkylakeAioRvp10Evp:
    case BoardIdSkylakeAioRvp10Erb:
    case BoardIdSkylakeSlpfPpv:
      BoardConfig->BoardType      = BoardTypeRvp;
      BoardConfig->PlatformType   = TypeTrad;
      BoardConfig->PlatformFlavor = FlavorDesktop;
      break;

    case BoardIdSkylakeHaloDdr4Rvp11:
    case BoardIdKabylakeDdr4Rvp17: // KblGContent
      BoardConfig->BoardType = BoardTypeRvp;
      BoardConfig->PlatformType = TypeTrad;
      BoardConfig->PlatformFlavor = FlavorMobile;
      break;

    case BoardIdSkylakeHaloLpddr3Rvp16:
      BoardConfig->BoardType = BoardTypeRvp;
      BoardConfig->PlatformType = TypeTrad;
      BoardConfig->PlatformFlavor = FlavorMobile;
      BoardConfig->TypeCSupport   = TRUE;
      break;

    case BoardIdBasinFallsCrb:
    case BoardIdBasinFallsEv:
      BoardConfig->BoardType = BoardTypeRvp;
      BoardConfig->PlatformType = TypeTrad;
      BoardConfig->PlatformFlavor = FlavorWorkstation;
      break;

    case BoardIdSkylakeSds:
    case BoardIdSkylakePantherMtn:
    case BoardIdSkylakeStarbrook:
    case BoardIdKabylakeGrizzlyMtn:
    case BoardIdSkylakePhabletPoc:
      BoardConfig->BoardType = BoardTypeSds;
      break;

    case BoardIdKabylakeCoyoteMtn:
      BoardConfig->BoardType = BoardTypeSds;
      BoardConfig->PlatformType = TypeUltUlx;
      BoardConfig->PlatformFlavor = FlavorMobile;
      break;
#endif // MINTREE_FLAG
    default:
      BoardConfig->BoardType = BoardTypeRvp;
      BoardConfig->PlatformType = TypeUnknown;
      BoardConfig->PlatformFlavor = FlavorUnknown;
      break;
  }

#ifndef MINTREE_FLAG
  //
  // Check dock status
  //
  if (BoardConfig->PlatformFlavor == FlavorMobile) {
    Status = GetPcieDockStatus (&EcData);
    if ((Status == EFI_SUCCESS) && (EcData & EC_B_DOCK_STATUS_ATTACH)) {
      BoardConfig->DockAttached = 1;
    }
  }
#endif // MINTREE_FLAG
}

#ifndef MINTREE_FLAG
VOID
EFIAPI
InternalUpdateOcBoardConfig (
  IN OUT BOARD_CONFIG   *BoardConfig,
  IN OUT UINT16         *BoardId,
  IN OUT UINT16         *BoardIdOrgValue
  )
{
  *BoardId = BoardIdSkylakeDtRvp8Crb;
  *BoardIdOrgValue = BoardIdKabyLakeOc;
  //
  // Update Platform Flavor
  //
  BoardConfig->PlatformFlavor = FlavorDesktop;
  BoardConfig->BoardType      = BoardTypeRvp;
  BoardConfig->PlatformType   = TypeTrad;
}
#endif // MINTREE_FLAG

EFI_STATUS
CreateBoardPrivateData (
  IN UINT16 BoardId,
  IN UINT16 BoardIdOrgValue
  )
{
  BOARD_PRIVATE_DATA                BoardInfo;
  UINTN                             Size;

  Size = sizeof (BOARD_PRIVATE_DATA);
  ZeroMem (&BoardInfo, Size);

  BoardInfo.BoardId = BoardId;
  BoardInfo.BoardIdOrgValue = BoardIdOrgValue;

  PcdSetPtrS (PcdBoardPrivateData, &Size, &BoardInfo);

  return EFI_SUCCESS;
}

/**
  Procedure to detect current board HW configuration.

  @param[out] BoardConfig   Pointer to BOARD_CONFIG.

**/
VOID
EFIAPI //AMI_OVERRIDE - EIP431111 [KBL] EFIAPI function qualifiers for Linux Build.
GetBoardConfig (
  IN OUT BOARD_CONFIG   *BoardConfig,
  IN UINT32             SleepType,
  IN UINTN              BoardNameSize
  )
{
  UINT16          BoardId;
  UINT16          BoardIdOrgValue;
  RETURN_STATUS   Status;
  UINT8           DataBuffer[2];
#ifndef MINTREE_FLAG
  BOOLEAN         SvBoardDetected;
#endif // MINTREE_FLAG
  UINT16           Retry;

  //
  // Get Platform Info and fill the Hob
  //
  BoardConfig->RevisonId = PLATFORM_INFO_HOB_REVISION;
  BoardId   = BoardIdUnknown1;
  BoardConfig->BoardType = BoardTypeMax;
  BoardIdOrgValue = 0;

  //
  // Detect EC Revision
  //
  if (SleepType != V_PCH_ACPI_PM1_CNT_S3) {
    Retry = 100; // Reducing retry count, so that in "worst-case scenario" Max Loop time will not be more that WDT expire time.
    do {
      Status = DetectEcRevision ((UINT8 *)DataBuffer);
      Retry--;
      if ((RETURN_ERROR (Status)) && (Retry != 0)) {
        MicroSecondDelay (STALL_TIME);
      }
    } while ((RETURN_ERROR (Status)) && (Retry != 0));
    if (RETURN_ERROR (Status)) {
      BoardConfig->EcPresent = FALSE;
    } else {
      BoardConfig->EcPresent = TRUE;
      BoardConfig->EcMajorRevision = DataBuffer[0];
      BoardConfig->EcMinorRevision = DataBuffer[1];
    }
  }

#ifndef MINTREE_FLAG
  SvBoardDetected = InternalDetectSvBoard (BoardConfig, BoardNameSize, &BoardId);
  if (!SvBoardDetected) {
#endif // MINTREE_FLAG
    if (BoardConfig->EcPresent == TRUE) {
      if (SleepType != V_PCH_ACPI_PM1_CNT_S3) {
        InternalUpdateRvpBoardConfig (BoardConfig, &BoardId, &BoardIdOrgValue);
        BoardConfig->BoardID = BoardId;
        BoardConfig->BoardIDOrgvalue = BoardIdOrgValue;
      }
      else{
        BoardId = BoardConfig->BoardID;
        BoardIdOrgValue = BoardConfig->BoardIDOrgvalue;
      }
#ifndef MINTREE_FLAG
    } else {
      if(InternalDetectOcBoard ()) {
        InternalUpdateOcBoardConfig (BoardConfig, &BoardId, &BoardIdOrgValue);
// AMI_OVERRIDE_START - AMI SIO module no need this init function.
#ifdef AMI_CRB_EC_SUPPORT_FLAG        
#if !(defined SIO_SUPPORT) || SIO_SUPPORT == 0        
        It8628SioParallelPortInit ();
#endif
#endif
// AMI_OVERRIDE_END - AMI SIO module no need this init function.
      } else if (InternalDetectSvBoard (BoardConfig, BoardNameSize, &BoardId) == FALSE) {
        if (InternalCheckForEmbeddedPlatform (BoardConfig) == TRUE) {
          //
          // Define Embedded Platform Info.
          //
          InternalUpdateEmbeddedBoardConfig (BoardConfig, &BoardId, &BoardIdOrgValue);
        } else if (InternalCheckForUpServerPlatform () == TRUE) {
          //
          // Define UpServer Platform.
          //
          InternalUpdateUpServerBoardConfig (BoardConfig, &BoardId, &BoardIdOrgValue);
        } else {
          InternalUpdateCpvPpvPlatform (BoardConfig, &BoardId);
        }
      } else {
        DEBUG ((DEBUG_INFO, "SV Platform detected\n"));
      }
    }
  } else {
#endif // MINTREE_FLAG
  }

//AMI_OVERRIDE_START 
  BoardConfig->BoardID = BoardId;
//AMI_OVERRIDE_END 
  DEBUG ((DEBUG_INFO, "Platform Information:\nPlatformType: %x\n", BoardConfig->PlatformType));
  DEBUG ((DEBUG_INFO, "PlatformFlavor: %x\nBoardID: 0x%x\n", BoardConfig->PlatformFlavor, BoardId));
  DEBUG ((DEBUG_INFO, "BoardRev: %x\nBoardBomId: %x\nBoardType: %x\n", BoardConfig->BoardRev, BoardConfig->BoardBomId, BoardConfig->BoardType));
  DEBUG ((DEBUG_INFO, "PlatformGeneration: %x\nEcPresent: %d\nSpdPresent: %d\n", BoardConfig->PlatformGeneration, BoardConfig->EcPresent, BoardConfig->SpdPresent));

#ifndef MINTREE_FLAG
  if (BoardConfig->BoardType == BoardTypeRvpPpv) {
    DEBUG ((DEBUG_INFO, "This is PPV platform\n"));
    PcdSetBoolS (PcdBoardTypeRvpPpv, TRUE);
    PcdSetBoolS (PcdResetOnMemoryTypeInformationChange, FALSE);
  }
#endif // MINTREE_FLAG

  //
  // Halt the system if BoardID unknown
  //
  if (BoardId == BoardIdUnknown1) {
    UINT32 code = 0;

#ifndef MINTREE_FLAG
    switch (BoardConfig->BoardType) {
      case BoardTypeRvp:
        code = 0xBD10;
        break;
      case BoardTypeSv:
        code = 0xBD20;
        break;
      default:
        code = 0xBD30;
        break;
    }
#endif // MINTREE_FLAG
    DEBUG ((DEBUG_ERROR, "Platform BoardID unknown, set PostCode=0x%x and halt the system !\n", code));

    PostCode (code);
    CpuDeadLoop ();
  }
  CreateBoardPrivateData (BoardId, BoardIdOrgValue);
}

/**
  Procedure to install Platform Board Config Ppi.

  @param[in] PlatformBoardConfig   Pointer to CONFIG_BLOCK_TABLE_HEADER.

  @retval     EFI_SUCCESS       The function completed successfully.
**/
EFI_STATUS
InstallPlatformBoardConfigPpi (
  IN CONFIG_BLOCK_TABLE_HEADER *PlatformBoardConfig,
  IN EFI_GUID *PpiGuid
  )
{
  EFI_STATUS                  Status;
  EFI_PEI_PPI_DESCRIPTOR      *PlatformConfigPpiDesc;

  PlatformConfigPpiDesc = (EFI_PEI_PPI_DESCRIPTOR *) AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
  ASSERT (PlatformConfigPpiDesc != NULL);
  if (PlatformConfigPpiDesc == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Initialize the PPI
  //
  PlatformConfigPpiDesc->Flags  = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  PlatformConfigPpiDesc->Guid   = PpiGuid;
  PlatformConfigPpiDesc->Ppi    = PlatformBoardConfig;

  //
  // Install the PPI
  //
  Status = PeiServicesInstallPpi (PlatformConfigPpiDesc);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Procedure to install Platform Board Config Ppi.

  @retval    EFI_SUCCESS       The function completed successfully.
**/
EFI_STATUS
InitBoardConfigBlockPreMem (
  VOID
  )
{
  EFI_STATUS Status;
  UINT16 BlockSize;
  CONFIG_BLOCK_TABLE_HEADER *PlatformConfigBlockPreMem;
  CONFIG_BLOCK *ConfigBlockPointer;

  BlockSize = (UINT32) mSampleBoardConfigPeiPreMem.Header.GuidHob.Header.HobLength + sizeof (CONFIG_BLOCK_TABLE_HEADER);

  PlatformConfigBlockPreMem = NULL;

  Status = CreateConfigBlockTable (BlockSize, (VOID **) &PlatformConfigBlockPreMem);
  ASSERT_EFI_ERROR (Status);

  //
  // Initialize config block Revision
  //
  PlatformConfigBlockPreMem->Header.Revision = BOARD_CONFIG_BLOCK_PEI_PREMEM_VERSION;

  ConfigBlockPointer = (VOID *) &mSampleBoardConfigPeiPreMem;
  Status = AddConfigBlock ((VOID *)PlatformConfigBlockPreMem, (VOID *)&ConfigBlockPointer);

  InitPlatformBoardConfigBlockPreMem (ConfigBlockPointer);

  Status = InstallPlatformBoardConfigPpi (PlatformConfigBlockPreMem, &gPlatformBoardConfigPreMemPpiGuid);

  return Status;
}

/**
  Procedure to init Platform Board Config Block in Post Memory phase.

  @retval    EFI_SUCCESS       The function completed successfully.
**/
EFI_STATUS
InitBoardConfigBlockPostMem (
  VOID
  )
{
  EFI_STATUS Status;
  UINT16 BlockSize;
  CONFIG_BLOCK_TABLE_HEADER *PlatformConfigBlockPostMem;
  CONFIG_BLOCK *ConfigBlockPointer;

  BlockSize = (UINT32) mSampleBoardConfigPeiPostMem.Header.GuidHob.Header.HobLength + sizeof (CONFIG_BLOCK_TABLE_HEADER);

  PlatformConfigBlockPostMem = NULL;

  Status = CreateConfigBlockTable (BlockSize, (VOID **) &PlatformConfigBlockPostMem);
  ASSERT_EFI_ERROR (Status);

  //
  // Initialize config block Revision
  //
  PlatformConfigBlockPostMem->Header.Revision = BOARD_CONFIG_BLOCK_PEI_POSTMEM_VERSION;

  ConfigBlockPointer = (VOID *) &mSampleBoardConfigPeiPostMem;
  Status = AddConfigBlock ((VOID *)PlatformConfigBlockPostMem, (VOID *)&ConfigBlockPointer);

  InitPlatformBoardConfigBlockPostMem (ConfigBlockPointer);

  Status = InstallPlatformBoardConfigPpi (PlatformConfigBlockPostMem, &gPlatformBoardConfigPostMemPpiGuid);

  return Status;
}


/**
  Procedure to get Board Config Block.

  @param[in] ConfigBlockGuid      Pointer to the GUID for specific config block.
  @param[out] ConfigBlockAddress  Pointer to the specific config block.

  @retval     EFI_SUCCESS       The function completed successfully.
**/
EFI_STATUS
GetBoardConfigBlock (
  IN     EFI_GUID  *ConfigBlockGuid,
  OUT    VOID      **ConfigBlockAddress
  )
{
  EFI_STATUS                Status;
  CONFIG_BLOCK_TABLE_HEADER *PlatformConfigBlock;
  VOID                      *BoardConfigBlock;

  //
  // Locate Board Config Ppi
  //
  Status = PeiServicesLocatePpi(
             &gPlatformBoardConfigPreMemPpiGuid,    // GUID
             0,                                     // INSTANCE
             NULL,                                  // EFI_PEI_PPI_DESCRIPTOR
             (VOID **) &PlatformConfigBlock         // PPI
             );

  if (!EFI_ERROR (Status)) {
    BoardConfigBlock = NULL;
    DEBUG ((DEBUG_INFO, "Searching PreMemConfigBlock table for {%g}\n", ConfigBlockGuid));
    Status = GetConfigBlock((VOID *)PlatformConfigBlock, ConfigBlockGuid, (VOID *)&BoardConfigBlock);
    if (!EFI_ERROR (Status)) {
      *ConfigBlockAddress = BoardConfigBlock;
      return EFI_SUCCESS;
    }
  }

  Status = PeiServicesLocatePpi(
             &gPlatformBoardConfigPostMemPpiGuid,    // GUID
             0,                                      // INSTANCE
             NULL,                                   // EFI_PEI_PPI_DESCRIPTOR
             (VOID **) &PlatformConfigBlock          // PPI
             );
  ASSERT_EFI_ERROR(Status);

  BoardConfigBlock = NULL;
  DEBUG ((DEBUG_INFO, "Searching PostMemConfigBlock table for {%g}\n", ConfigBlockGuid));
  Status = GetConfigBlock((VOID *)PlatformConfigBlock, ConfigBlockGuid, (VOID *)&BoardConfigBlock);
  ASSERT_EFI_ERROR(Status);

  *ConfigBlockAddress = BoardConfigBlock;

  return EFI_SUCCESS;
}

#ifndef MINTREE_FLAG
//AMI_OVERRIDE_START - No need to show SETUP data revision in AMI setup items
#if 0
EFI_STATUS
UpdateSetupVariableRevision (
  IN UINTN      Offset,
  IN UINT8      Revision,
  IN CHAR16     *VarName,
  IN EFI_GUID   *VarGuid,
  IN UINTN      VarSize
  )
{
  EFI_STATUS     Status;
  UINTN          VarDataSize;
  SETUP_VAR_DATA VarData;
  UINT8          *RevisionPtr;

  VarDataSize = VarSize;

  Status = GetVariableFromHob (
                  VarName,
                  VarGuid,
                  NULL,
                  &VarDataSize,
                  &VarData
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Get variable from hob failed : %r\n", Status));
    DEBUG ((DEBUG_INFO, "Variable Name : %s, GUID : %g\n", VarName, VarGuid));
    return Status;
  }

  RevisionPtr = (UINT8 *) (VOID *) &VarData + Offset;
  *RevisionPtr = Revision;

  Status = SetVariableToHob (
                  VarName,
                  VarGuid,
                  NULL,
                  VarDataSize,
                  &VarData
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Set variable to hob failed : %r\n", Status));
    DEBUG ((DEBUG_INFO, "Variable Name : %s, GUID : %g\n", VarName, VarGuid));
  }

  return Status;
}
#endif
//AMI_OVERRIDE_END - No need to show SETUP data revision in AMI setup items
#endif // MINTREE_FLAG

EFI_STATUS
CreateBoardDefaultVariableHob (
  IN UINT16  DefaultId,
  IN UINT8   OsProfile
  )
{
  EFI_STATUS         Status;
  UINT16             BoardId;
  BOARD_PRIVATE_DATA *BoardInfo;

  BoardInfo = (BOARD_PRIVATE_DATA *) PcdGetPtr (PcdBoardPrivateData);
  BoardId = 0;
#ifndef MINTREE_FLAG
  BoardId = UpdateBoardId (OsProfile, BoardInfo->BoardId);
#endif // MINTREE_FLAG

  Status = CreateDefaultVariableHob (DefaultId, BoardId);

#ifndef MINTREE_FLAG
//AMI_OVERRIDE_START - No need to show SETUP data revision in AMI setup items
#if 0
  Status = UpdateSetupVariableRevision (
             OFFSET_OF (SETUP_DATA, Revision),
             SETUP_DATA_REVISION,
             PLATFORM_SETUP_VARIABLE_NAME,
             &gSetupVariableGuid,
             sizeof (SETUP_DATA)
             );

  Status = UpdateSetupVariableRevision (
             OFFSET_OF (ME_SETUP, Revision),
             ME_SETUP_REVISION,
             L"MeSetup",
             &gMeSetupVariableGuid,
             sizeof (ME_SETUP)
             );

  Status = UpdateSetupVariableRevision (
             OFFSET_OF (CPU_SETUP, Revision),
             CPU_SETUP_REVISION,
             L"CpuSetup",
             &gCpuSetupVariableGuid,
             sizeof (CPU_SETUP)
             );

  Status = UpdateSetupVariableRevision (
             OFFSET_OF (SA_SETUP, Revision),
             SA_SETUP_REVISION,
             L"SaSetup",
             &gSaSetupVariableGuid,
             sizeof (SA_SETUP)
             );

  Status = UpdateSetupVariableRevision (
             OFFSET_OF (PCH_SETUP, Revision),
             PCH_SETUP_REVISION,
             L"PchSetup",
             &gPchSetupVariableGuid,
             sizeof (PCH_SETUP)
             );
#endif
//AMI_OVERRIDE_END - No need to show SETUP data revision in AMI setup items
#endif // MINTREE_FLAG

  return Status;
}

