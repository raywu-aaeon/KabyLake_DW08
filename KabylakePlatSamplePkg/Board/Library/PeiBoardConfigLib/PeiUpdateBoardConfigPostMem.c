/** @file
  Source file for the Skylake A0 RVP3 board config Pcd Init function.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2017 Intel Corporation.

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
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiPlatformHookLib.h>
// AMI_OVERRIDE_START - For AMI and OEM Feature
#include <Library/AmiTbtOemLib.h>
// AMI_OVERRIDE_END - For AMI and OEM Feature
#include <PchAccess.h>
#include <PchPolicyCommon.h>
#include <PlatformInfo.h>
#include <Board/BoardHook.h>
#include <Board/PeiBoardConfigPostMem.h>
#include <GpioPinsSklLp.h>
#include <GpioPinsSklH.h>

/**
  Init PCH Platform Board Config Block.

  @param[in]  PlatformInfo      a pointer to Platform Info.
  @param[in]  ConfigBlock       a pointer to PEI PreMem structure.

  @retval     EFI_SUCCESS       The function completed successfully.
**/

EFI_STATUS
InitPchPlatformConfigBlockPostMem (
  IN PLATFORM_INFO *PlatformInfo,
  IN CONFIG_BLOCK *ConfigBlock
)
{
  BOARD_CONFIG_BLOCK_PEI_POSTMEM *PeiPostMem;

  PeiPostMem = (BOARD_CONFIG_BLOCK_PEI_POSTMEM *) ConfigBlock;

  //
  // GPIO Table Init.
  //

  PeiPostMem->GpioTable1 = (VOID *) (UINTN) PcdGet32 (PcdBoardGpioTable);
  PeiPostMem->Table1Size = PcdGet16 (PcdBoardGpioTableSize);

  PeiPostMem->GpioTable2 = (VOID *) (UINTN) PcdGet32 (PcdBoardGpioTable2);
  PeiPostMem->Table2Size = PcdGet16 (PcdBoardGpioTable2Size);


  //
  // GpioExpander Init.
  //

  PeiPostMem->GpioExpanderTable1 = (VOID *) (UINTN) PcdGet32 (PcdGpioExpanderTable);
  PeiPostMem->GpioExpanderTable1Size = PcdGet16 (PcdGpioExpanderTableSize);

  PeiPostMem->GpioExpanderTable2 = (VOID *) (UINTN) PcdGet32 (PcdGpioExpanderTable2);
  PeiPostMem->GpioExpanderTable2Size = PcdGet16 (PcdGpioExpanderTable2Size);
#ifndef MINTREE_FLAG
  //
  // USB Debug UART Enable.
  //

  PeiPostMem->Enable.DebugUsbUart = PcdGetBool (PcdDebugUsbUartEnable);
#endif

  //
  // Touch Panel Pad Gpio Init.
  //

  PeiPostMem->TouchpanelPad = (VOID *) (UINTN) PcdGet32 (PcdBoardGpioTableTouchPanel);
#ifndef MINTREE_FLAG
  //
  // SDHC Sideband Card Detect Init.
  //

  PeiPostMem->SdhcSidebandCardDetect = (VOID *) (UINTN) PcdGet32 (PcdBoardGpioTableSdhcSidebandCardDetect);

  //
  // SkyCam GPIO Enable.
  //

  PeiPostMem->Enable.SkyCamGpio = PcdGetBool (PcdSkyCamGpioEnable);

  // Specific Init.

  PeiPostMem->Enable.PhabletPocGpioSpecificInit = PcdGetBool (PcdPhabletPocGpioEnable);
  PeiPostMem->Enable.PantherMtnGpioSpecificInit = PcdGetBool (PcdPantherMtnGpioEnable);
  PeiPostMem->Enable.GrizzlyMtnGpioSpecificInit = PcdGetBool (PcdGrizzlyMtnGpioEnable);
  PeiPostMem->Enable.CoyoteMtnGpioSpecificInit = PcdGetBool(PcdCoyoteMtnGpioEnable);

  //
  // M.2 SATA GPIO Specific Init.
  //

  PeiPostMem->Enable.M2GpioSpecificInit = PcdGetBool (PcdM2GpioEnable);

  //
  // WWAN GPIO Specific Init.
  //
  PeiPostMem->GpioTablePcieWwan              = (VOID *) (UINTN) PcdGet32 (PcdBoardGpioTablePcieWwan);
  PeiPostMem->GpioTablePcieWwanSize          = PcdGet16 (PcdBoardGpioTablePcieWwanSize);
  PeiPostMem->GpioExpanderTablePcieWwan      = (VOID *) (UINTN) PcdGet32 (PcdBoardGpioTableIoExpanderPcieWwan);
  PeiPostMem->GpioExpanderTablePcieWwanSize  = PcdGet16 (PcdBoardGpioTableIoExpanderPcieWwanSize);
  PeiPostMem->ModemReset                     = (VOID *) (UINTN) PcdGet32 (PcdBoardGpioTableModemReset);

  //
  // PCIe Slot1 GPIO Specific Init.
  //
  PeiPostMem->GpioTablePcieSlot1             = (VOID *) (UINTN) PcdGet32 (PcdBoardGpioTablePcieSlot1);
  PeiPostMem->GpioTablePcieSlot1Size         = PcdGet16 (PcdBoardGpioTablePcieSlot1Size);
  PeiPostMem->GpioExpanderTablePcieSlot1     = (VOID *) (UINTN) PcdGet32 (PcdBoardGpioTableIoExpanderPcieSlot1);
  PeiPostMem->GpioExpanderTablePcieSlot1Size = PcdGet16 (PcdBoardGpioTableIoExpanderPcieSlot1Size);
#endif

  //
  // VerbTable
  //

  PeiPostMem->VerbTable1[0] = NULL;
  PeiPostMem->VerbTable1[1] = NULL;
  PeiPostMem->VerbTable1[2] = NULL;
  PeiPostMem->VerbTable2[0] = NULL;
  PeiPostMem->VerbTable2[1] = NULL;
  PeiPostMem->VerbTable2[2] = NULL;

  PeiPostMem->VerbTable1[0] = (VOID *) (UINTN) PcdGet32 (PcdHdaVerbTable);
  PeiPostMem->VerbTable1[1] = (VOID *) (UINTN) PcdGet32 (PcdHdaVerbTable2);
  PeiPostMem->VerbTable2[0] = (VOID *) (UINTN) PcdGet32 (PcdExtHdaVerbTable);

  PeiPostMem->DisplayAudioVerbTable = (VOID *) (UINTN) PcdGet32 (PcdDisplayAudioHdaVerbTable);
  PeiPostMem->CommonVerbTable[0] = (VOID *) (UINTN) PcdGet32 (PcdCommonHdaVerbTable1);
  PeiPostMem->CommonVerbTable[1] = (VOID *) (UINTN) PcdGet32 (PcdCommonHdaVerbTable2);
  PeiPostMem->CommonVerbTable[2] = (VOID *) (UINTN) PcdGet32 (PcdCommonHdaVerbTable3);


  return EFI_SUCCESS;
}

#ifndef MINTREE_FLAG
UINT8
NibbletoAscii(
  IN UINT8     Data8
  )
{
  UINT8 RetVal;

  RetVal = 0;

  if(Data8 <= 9) {
    RetVal = 0x30 + Data8;
  } else if((Data8 >= 0xa) && (Data8 <= 0xf)) {
    RetVal = 0x41 + (Data8 - 0xa);
  }

  return(RetVal);
}

// AMI_OVERRIDE_START - Build Error when disabling the token, "EC_SUPPORT". EIP254801
/**
  Convert 8bit data into equivalent Ascii format

  Eg: 18 will be converted to 0x3138{'1' '8'}
**/
UINT16
Data8toAscii(
  IN UINT8     Data8
  )
{
  UINT16 Data16Ascii;
  UINT8  RetVal;

  Data16Ascii = 0;

  //MSB
  RetVal = NibbletoAscii((Data8>>4) & 0x0F);
  Data16Ascii |= (UINT16)RetVal;
  Data16Ascii <<= 8;

  //LSB
  RetVal = NibbletoAscii((Data8) & 0x0F);
  Data16Ascii |= (UINT16)RetVal;

  return(Data16Ascii);
}
//AMI_OVERRIDE_END - Build Error when disabling the token, "EC_SUPPORT". EIP254801

/**
  Init Common Platform Board Config Block.

  @param[in]  PlatformInfo      a pointer to Platform Info.
  @param[in]  ConfigBlock       a pointer to PEI PreMem structure.

  @retval     EFI_SUCCESS       The function completed successfully.
**/

EFI_STATUS
InitCommonPlatformConfigBlockPostMem (
  IN PLATFORM_INFO *PlatformInfo,
  IN CONFIG_BLOCK *ConfigBlock
)
{
  BOARD_CONFIG_BLOCK_PEI_POSTMEM *PeiPostMem;

//AMI_OVERRIDE_START - Build Error when disabling the token, "EC_SUPPORT". EIP254801
#if defined(Thunderbolt_SUPPORT) && Thunderbolt_SUPPORT
#if (!(defined(TBT_CRB_CONFIG) && TBT_CRB_CONFIG == 1))
  UINT32                          Data32;
  UINT16                          Data16Ascii;  
#endif
#endif
//AMI_OVERRIDE_END - Build Error when disabling the token, "EC_SUPPORT". EIP254801
  
  PeiPostMem = (BOARD_CONFIG_BLOCK_PEI_POSTMEM *) ConfigBlock;

  //
  // TBT Init
  //

  PeiPostMem->TbtSupportedPortsInfo                = PcdGet32 (PcdTbtSupportedPortsInfo);
  PeiPostMem->TbtDefaultSelectorInfo               = PcdGet8  (PcdTbtDefaultSelectorInfo);

  PeiPostMem->ForcePwrGpio.GpioAccessType          = PcdGet8    (PcdTbtForcePwrGpioGpioAccessType     );
  PeiPostMem->ForcePwrGpio.Expander                = PcdGet8    (PcdTbtForcePwrGpioExpander           );
  PeiPostMem->ForcePwrGpio.GpioNumber              = PcdGet32   (PcdTbtForcePwrGpioGpioNumber         );
  PeiPostMem->ForcePwrGpio.GpioMode                = PcdGetBool (PcdTbtForcePwrGpioGpioMode           );
  PeiPostMem->ForcePwrGpio.GpioLevel               = PcdGetBool (PcdTbtForcePwrGpioGpioLevel          );
  PeiPostMem->ForcePwrGpio.AcpiGpeSignature        = PcdGet32   (PcdTbtForcePwrGpioAcpiGpeSignature       );
  PeiPostMem->ForcePwrGpio.AcpiGpeStsAddress       = PcdGet16   (PcdTbtForcePwrGpioAcpiGpeStsAddress      );
  PeiPostMem->ForcePwrGpio.AcpiGpeStsBit           = PcdGet8    (PcdTbtForcePwrGpioAcpiGpeStsBit          );
  PeiPostMem->ForcePwrGpio.AcpiGpioGpeNumber       = PcdGet32   (PcdTbtForcePwrGpioAcpiGpioGpeNumber      );
  PeiPostMem->ForcePwrGpio.AcpiGpeSignaturePorting = PcdGetBool (PcdTbtForcePwrGpioAcpiGpeSignaturePorting);

  PeiPostMem->CioPlugEventGpio.GpioAccessType          = PcdGet8    (PcdTbtCioPlugEventGpioGpioAccessType          );
  PeiPostMem->CioPlugEventGpio.Expander                = PcdGet8    (PcdTbtCioPlugEventGpioExpander                );
  PeiPostMem->CioPlugEventGpio.GpioNumber              = PcdGet32   (PcdTbtCioPlugEventGpioGpioNumber              );
  PeiPostMem->CioPlugEventGpio.GpioMode                = PcdGetBool (PcdTbtCioPlugEventGpioGpioMode                );
  PeiPostMem->CioPlugEventGpio.GpioLevel               = PcdGetBool (PcdTbtCioPlugEventGpioGpioLevel               );
  PeiPostMem->CioPlugEventGpio.AcpiGpeSignature        = PcdGet32   (PcdTbtCioPlugEventGpioAcpiGpeSignature        );
  PeiPostMem->CioPlugEventGpio.AcpiGpeStsAddress       = PcdGet16   (PcdTbtCioPlugEventGpioAcpiGpeStsAddress       );
  PeiPostMem->CioPlugEventGpio.AcpiGpeStsBit           = PcdGet8    (PcdTbtCioPlugEventGpioAcpiGpeStsBit           );
  PeiPostMem->CioPlugEventGpio.AcpiGpioGpeNumber       = PcdGet32   (PcdTbtCioPlugEventGpioAcpiGpioGpeNumber       );
  PeiPostMem->CioPlugEventGpio.AcpiGpeSignaturePorting = PcdGetBool (PcdTbtCioPlugEventGpioAcpiGpeSignaturePorting );

// AMI_OVERRIDE_START - For AMI and OEM Feature
#if defined(Thunderbolt_SUPPORT) && Thunderbolt_SUPPORT
#if (!(defined(TBT_CRB_CONFIG) && TBT_CRB_CONFIG == 1))
//AMI_OVERRIDE_START - EIP299901 GPIO access type on KBL: only 1 and 2 are legal
  //
  // SKL: 0 or 1 //KBL: 1 or 2 (0 is illegal)
  // Set to 1 as the default (CRB will not jump in this code due to "TBT_CRB_CONFIG  == 1")
  //
  PeiPostMem->ForcePwrGpio.GpioAccessType        = 1;
  PeiPostMem->ForcePwrGpio.GpioNumber            = (UINT32)((ConvertGpioGroup(TBT_FORCEPWR_GROUP) << 16) | TBT_FORCEPWR_PAD);
  PeiPostMem->ForcePwrGpio.GpioLevel             = TBT_FORCEPWR_ACTIVE_LEVEL;

  PeiPostMem->CioPlugEventGpio.GpioAccessType    = 1;
//AMI_OVERRIDE_END - EIP299901 GPIO access type on KBL: only 1 and 2 are legal
  PeiPostMem->CioPlugEventGpio.GpioNumber        = (UINT32)((ConvertGpioGroup(TBT_HOTPLUGEVT_GROUP) << 16) | TBT_HOTPLUGEVT_PAD);
  PeiPostMem->CioPlugEventGpio.GpioMode          = 0;
  
  PeiPostMem->CioPlugEventGpio.AcpiGpeSignaturePorting = 1;
  GpioGetGpeNumber(PeiPostMem->CioPlugEventGpio.GpioNumber, &Data32);
  PeiPostMem->CioPlugEventGpio.AcpiGpioGpeNumber = Data32;
  if (Data32 == 0x6F){
    // Thunderbolt GPE event is under 2-Tier
    PeiPostMem->CioPlugEventGpio.AcpiGpeSignature  = SIGNATURE_32 ('X', 'T', 'B', 'T');
    PeiPostMem->CioPlugEventGpio.AcpiGpeStsAddress = (EFI_ACPI_PM1A_EVT_BLK_ADDRESS + 0x8C); //GPE0_STS_127_96
    PeiPostMem->CioPlugEventGpio.AcpiGpeStsBit     = 15; //GPIO_TIER2_SCI_STS
  } else {
    // Thunderbolt GPE event is under 1-Tier
    Data16Ascii = Data8toAscii((UINT8)Data32);
    PeiPostMem->CioPlugEventGpio.AcpiGpeSignature  = SIGNATURE_32 ('_', 'E', ((Data16Ascii>>8) & 0xFF), ((Data16Ascii) & 0xFF));
    PeiPostMem->CioPlugEventGpio.AcpiGpeStsAddress = (UINT16)(EFI_ACPI_GPE0_BLK_ADDRESS + (Data32/8));
    PeiPostMem->CioPlugEventGpio.AcpiGpeStsBit     = (Data32%8);
  }
#endif
#endif
// AMI_OVERRIDE_END - For AMI and OEM Feature

#if 0 // AMI_OVERRIDE_START - For TBT
  PeiPostMem->TbtSupportedPortsInfo1                = PcdGet32 (PcdTbtSupportedPortsInfo1);
  PeiPostMem->TbtDefaultSelectorInfo1               = PcdGet8  (PcdTbtDefaultSelectorInfo1);

  PeiPostMem->ForcePwrGpio1.GpioAccessType          = PcdGet8    (PcdTbtForcePwrGpioGpioAccessType1     );
  PeiPostMem->ForcePwrGpio1.Expander                = PcdGet8    (PcdTbtForcePwrGpioExpander1           );
  PeiPostMem->ForcePwrGpio1.GpioNumber              = PcdGet32   (PcdTbtForcePwrGpioGpioNumber1         );
  PeiPostMem->ForcePwrGpio1.GpioMode                = PcdGetBool (PcdTbtForcePwrGpioGpioMode1           );
  PeiPostMem->ForcePwrGpio1.GpioLevel               = PcdGetBool (PcdTbtForcePwrGpioGpioLevel1          );
  PeiPostMem->ForcePwrGpio1.AcpiGpeSignature        = PcdGet32   (PcdTbtForcePwrGpioAcpiGpeSignature1       );
  PeiPostMem->ForcePwrGpio1.AcpiGpeStsAddress       = PcdGet16   (PcdTbtForcePwrGpioAcpiGpeStsAddress1      );
  PeiPostMem->ForcePwrGpio1.AcpiGpeStsBit           = PcdGet8    (PcdTbtForcePwrGpioAcpiGpeStsBit1          );
  PeiPostMem->ForcePwrGpio1.AcpiGpioGpeNumber       = PcdGet32   (PcdTbtForcePwrGpioAcpiGpioGpeNumber1      );
  PeiPostMem->ForcePwrGpio1.AcpiGpeSignaturePorting = PcdGetBool (PcdTbtForcePwrGpioAcpiGpeSignaturePorting1);

  PeiPostMem->CioPlugEventGpio1.GpioAccessType          = PcdGet8    (PcdTbtCioPlugEventGpioGpioAccessType1          );
  PeiPostMem->CioPlugEventGpio1.Expander                = PcdGet8    (PcdTbtCioPlugEventGpioExpander1                );
  PeiPostMem->CioPlugEventGpio1.GpioNumber              = PcdGet32   (PcdTbtCioPlugEventGpioGpioNumber1              );
  PeiPostMem->CioPlugEventGpio1.GpioMode                = PcdGetBool (PcdTbtCioPlugEventGpioGpioMode1                );
  PeiPostMem->CioPlugEventGpio1.GpioLevel               = PcdGetBool (PcdTbtCioPlugEventGpioGpioLevel1               );
  PeiPostMem->CioPlugEventGpio1.AcpiGpeSignature        = PcdGet32   (PcdTbtCioPlugEventGpioAcpiGpeSignature1        );
  PeiPostMem->CioPlugEventGpio1.AcpiGpeStsAddress       = PcdGet16   (PcdTbtCioPlugEventGpioAcpiGpeStsAddress1       );
  PeiPostMem->CioPlugEventGpio1.AcpiGpeStsBit           = PcdGet8    (PcdTbtCioPlugEventGpioAcpiGpeStsBit1           );
  PeiPostMem->CioPlugEventGpio1.AcpiGpioGpeNumber       = PcdGet32   (PcdTbtCioPlugEventGpioAcpiGpioGpeNumber1       );
  PeiPostMem->CioPlugEventGpio1.AcpiGpeSignaturePorting = PcdGetBool (PcdTbtCioPlugEventGpioAcpiGpeSignaturePorting1 );
#endif // AMI_OVERRIDE_END - For TBT
  PeiPostMem->TbtUsbSplitMode                           = PcdGetBool (PcdTbtUsbSplitMode);
  PeiPostMem->SplitModePchUsb2TbtPort1                  = PcdGet8    (PcdSplitModePchUsb2TbtPort1           );
  PeiPostMem->SplitModePchUsb2TbtPort2                  = PcdGet8    (PcdSplitModePchUsb2TbtPort2           );
  return EFI_SUCCESS;
}
#endif
/**
  Init Platform Board Config Block for PreMem phase.

  @param[in]  ConfigBlock       a pointer to PEI PreMem structure.

  @retval     EFI_SUCCESS       The function completed successfully.
**/


EFI_STATUS
InitPlatformBoardConfigBlockPostMem (
  IN CONFIG_BLOCK *ConfigBlock
)
{
  EFI_STATUS Status;
  BOARD_CONFIG_BLOCK_PEI_POSTMEM *PeiPostMem;
  PLATFORM_INFO *PlatformInfo;
  PEI_BOARD_CONFIG_PCD_INIT BoardConfigPcdInit;

  PlatformInfo = NULL;
  Status = GetPlatformInfo(&PlatformInfo);
  ASSERT_EFI_ERROR (Status);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Init Board Config Pcd.
  //
  BoardConfigPcdInit = (PEI_BOARD_CONFIG_PCD_INIT) (UINTN) PcdGet64 (PcdFuncBoardConfigInit);
  ASSERT (BoardConfigPcdInit != NULL);

  if (BoardConfigPcdInit == NULL) {
    return EFI_INVALID_PARAMETER;
  }


  Status = BoardConfigPcdInit (PlatformInfo);
  ASSERT_EFI_ERROR (Status);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  PeiPostMem = (BOARD_CONFIG_BLOCK_PEI_POSTMEM *) ConfigBlock;
  CopyGuid (&PeiPostMem->Header.GuidHob.Name, &gPlatformBoardConfigPostMemGuid);

  Status = InitPchPlatformConfigBlockPostMem (PlatformInfo, ConfigBlock);
  ASSERT_EFI_ERROR(Status);
#ifndef MINTREE_FLAG
  Status = InitCommonPlatformConfigBlockPostMem (PlatformInfo, ConfigBlock);
  ASSERT_EFI_ERROR(Status);
#endif
  return Status;
}
