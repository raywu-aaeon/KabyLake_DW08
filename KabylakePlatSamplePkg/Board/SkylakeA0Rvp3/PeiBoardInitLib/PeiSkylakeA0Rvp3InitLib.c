/** @file

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

#include "PeiSkylakeA0Rvp3InitLib.h"

#include "SkylakeA0Rvp3GpioTable.h"
#include "SkylakeA0Rvp3HdaVerbTables.h"

#ifndef MINTREE_FLAG
#include <SetupVariable.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/BaseMemoryLib.h>
#endif

//
// RVP3 Init Table
//
BOARD_INIT_TABLE mRvp3BoardInit [] = {
  {BoardIdSkylakeA0Rvp3,           PeiSkylakeA0Rvp3Init},
  {BoardIdKabyLakeYLpddr3Rvp3,     PeiSkylakeA0Rvp3Init},
  {BoardIdAmberLakeY42Lpddr3Rvp3,  PeiSkylakeA0Rvp3Init},
  {BoardIdAmberLakeLpddr3Rvp3,     PeiSkylakeA0Rvp3Init},
  {BoardIdAmberLakeY42Lpddr3Cff,   PeiSkylakeA0Rvp3Init},
  {BoardIdAmberLakeY42Lpddr3CffCc, PeiSkylakeA0Rvp3Init},
#ifndef MINTREE_FLAG
  {BoardIdSkylakeA0Rvp3Ppv,       PeiSkylakeA0Rvp3PpvInit},
#endif
  { 0, 0 }
};

#ifndef MINTREE_FLAG
/**
  Board Specific Init function hook for PEI post memory phase.

  @retval EFI_SUCCESS   The function completed successfully.
**/
EFI_STATUS
PeiSkylakeA0Rvp3SpecificInitPostMem (
  VOID
  )
{
  return EFI_SUCCESS;
}
#endif

/**
  SkylaeA0Rvp3 board configuration init function for PEI post memory phase.

  @param  Content  pointer to the buffer contain init information for board init.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   The parameter is NULL.
**/
EFI_STATUS
EFIAPI //AMI_OVERRIDE - EIP431111 [KBL] EFIAPI function qualifiers for Linux Build.
PeiSkylakeA0Rvp3Init (
  IN VOID *Content
  )
{
  EFI_STATUS    Status;
  PLATFORM_INFO *PlatformInfo;
  UINT16        BoardId;
  UINT16        BoardIdOrgValue;
#ifndef MINTREE_FLAG
  UINT16                            Index;
  UINTN                             VarSize;
  UINT16                            GpioTableCount;
  GPIO_INIT_CONFIG                  *GpioDefinition;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI   *VariableServices;
  SETUP_DATA                        SetupData;

  ZeroMem(&SetupData, sizeof(SETUP_DATA));
#endif

  PlatformInfo = (PLATFORM_INFO *) Content;
  ASSERT (PlatformInfo != NULL);

  if (PlatformInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetBoardId (&BoardId, &BoardIdOrgValue);
  ASSERT_EFI_ERROR (Status);

  //
  // Gpio Table
  //
  PcdSet32S (PcdBoardGpioTable, (UINTN) mGpioTableLpDdr3Rvp3);
  PcdSet16S (PcdBoardGpioTableSize, mGpioTableLpDdr3Rvp3Size);

#ifndef MINTREE_FLAG
  if (BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Rvp3) { //Customized change needed for AML Y42 RVP3 in contrast to SKL RVP3
    PcdSet32S (PcdBoardGpioTable, (UINTN) mGpioTable1AmberLakeY42LpDdr3Rvp3);
    PcdSet16S (PcdBoardGpioTableSize, mGpioTable1AmberLakeY42LpDdr3Rvp3Size);
  } else if ((BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Cff)||
             (BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3CffCc)) { //Customized change needed for AML Y42 AEP in contrast to SKL RVP3
    PcdSet32S (PcdBoardGpioTable, (UINTN) mGpioTable1AmberLakeY42LpDdr3Cff);
    PcdSet16S (PcdBoardGpioTableSize, mGpioTable1AmberLakeY42LpDdr3CffSize);
  }
#endif

#ifndef MINTREE_FLAG
  //
  // Locate system configuration variable
  //
  Status = PeiServicesLocatePpi(
      &gEfiPeiReadOnlyVariable2PpiGuid,
      0,
      NULL,
      (VOID **)&VariableServices
  );
  ASSERT_EFI_ERROR(Status);
  if (!EFI_ERROR(Status)) {
    VarSize = sizeof(SETUP_DATA);
    Status = VariableServices->GetVariable(
      VariableServices,
      L"Setup",
      &gSetupVariableGuid,
      NULL,
      &VarSize,
      &SetupData
    );
    DEBUG((DEBUG_INFO, "Setup variable status %r\n", Status));
  }

  if (PcdGet32(PcdBoardGpioTable) != 0) {
    GpioDefinition = (GPIO_INIT_CONFIG *)(UINTN)PcdGet32(PcdBoardGpioTable);
    GpioTableCount = PcdGet16(PcdBoardGpioTableSize);
    if ((BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Cff)||
        (BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3CffCc)) {
      for (Index = 0; Index < GpioTableCount; Index++) {

        if (GpioDefinition->GpioPad == GPIO_SKL_LP_GPP_B16) {
          if (SetupData.Rtd3Tbt == 1 && SetupData.Rtd3Support == 1) {
            //
            // TBT_CIO_PLUG_EVENT only need to be GpioIntDis if TBT-RTD3 is enabled.
            // If TBT-RTD3 is enable no SCI event required. TBT FW will takecare of Everything.
            //
            GpioDefinition->GpioConfig.InterruptConfig = GpioIntDis;
          }
          break;
        }
        GpioDefinition++;
      }
    } else {
      for (Index = 0; Index < GpioTableCount; Index++) {

        if (GpioDefinition->GpioPad == GPIO_SKL_LP_GPP_B17) {
          if (SetupData.Rtd3Tbt == 1 && SetupData.Rtd3Support == 1) {
            //
            // TBT_CIO_PLUG_EVENT only need to be GpioIntDis if TBT-RTD3 is enabled.
            // If TBT-RTD3 is enable no SCI event required. TBT FW will takecare of Everything.
            //
            GpioDefinition->GpioConfig.InterruptConfig = GpioIntDis;
          }
          break;
        }
        GpioDefinition++;
      }
    }
  }
#endif

  PcdSet32S (PcdBoardGpioTable2, (UINTN) mGpioTableSkyLakeYLpddr3Rvp3);
  PcdSet16S (PcdBoardGpioTable2Size, mGpioTableSkyLakeYLpddr3Rvp3Size);

#ifndef MINTREE_FLAG
  if (BoardIdOrgValue == BoardIdKabyLakeYLpddr3Rvp3 ||
      BoardIdOrgValue == BoardIdAmberLakeLpddr3Rvp3) { //Customized change needed for KBL RVP3 in contrast to SKL RVP3
    PcdSet32S (PcdBoardGpioTable2, (UINTN) mGpioTableKabyLakeYLpddr3Rvp3);
    PcdSet16S (PcdBoardGpioTable2Size, mGpioTableKabyLakeYLpddr3Rvp3Size);
  }

  if (BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Rvp3) { //Customized change needed for AML Y42 RVP3 in contrast to SKL RVP3
    PcdSet32S (PcdBoardGpioTable2, (UINTN) mGpioTable2AmberLakeY42LpDdr3Rvp3);
    PcdSet16S (PcdBoardGpioTable2Size, mGpioTable2AmberLakeY42LpDdr3Rvp3Size);
  }
  if ((BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Cff)||
      (BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3CffCc)) { //Customized change needed for AML Y42 AEP in contrast to SKL RVP3
    PcdSet32S (PcdBoardGpioTable2, (UINTN) mGpioTable2AmberLakeY42LpDdr3Cff);
    PcdSet16S (PcdBoardGpioTable2Size, mGpioTable2AmberLakeY42LpDdr3CffSize);
  }

  //
  // ExpanderGpio
  //
  if (BoardIdOrgValue == BoardIdKabyLakeYLpddr3Rvp3 ||
      BoardIdOrgValue == BoardIdAmberLakeLpddr3Rvp3) { // Customized change needed for KBL RVP3 in contrast to SKL RVP3
    PcdSet32S (PcdGpioExpanderTable, (UINTN) mGpioTableIoExpanderKabylakeRvp3);
    PcdSet16S (PcdGpioExpanderTableSize, mGpioTableIoExpanderKabylakeRvp3Size);
  } else if (BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Rvp3) { // Customized change needed for KBL RVP3 in contrast to SKL RVP3
    PcdSet32S (PcdGpioExpanderTable, (UINTN) mGpioTableIoExpanderAmberLakeY42Rvp3);
    PcdSet16S (PcdGpioExpanderTableSize, mGpioTableIoExpanderAmberLakeY42Rvp3Size);
  } else {
#endif
    PcdSet32S (PcdGpioExpanderTable, (UINTN) mGpioTableIoExpander);
    PcdSet16S (PcdGpioExpanderTableSize, mGpioTableIoExpanderSize);
#ifndef MINTREE_FLAG
  }
#endif

  //
  // Touch Panel, SDHC Card Detect, HDA VerbTable.
  //
  if ((BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Cff)||
      (BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3CffCc)) {
    PcdSet32S (PcdBoardGpioTableTouchPanel, (UINTN) &mGpioTableAmberLakeY42LpDdr3CffTouchpanel);
    PcdSet32S (PcdHdaVerbTable, (UINTN) &HdaVerbTableAlc700Cff);
  } else {
    PcdSet32S (PcdBoardGpioTableTouchPanel, (UINTN) &mGpioTableLpDdr3Rvp3Touchpanel);
    PcdSet32S (PcdBoardGpioTableSdhcSidebandCardDetect, (UINTN) &mGpioTableLpDdr3Rvp3SdhcSidebandCardDetect); // AdvancedFeaturesContent
    PcdSet32S (PcdHdaVerbTable, (UINTN) &HdaVerbTableAlc286Rvp3);
  }

  //
  // Assign the GPIO table with pin configs to be used for UCMC
  //
  PcdSet32S (PcdBoardUcmcGpioTable, (UINTN)mGpioTableLpddr3Rvp3UcmcDevice);
  PcdSet16S (PcdBoardUcmcGpioTableSize, mGpioTableLpddr3Rvp3UcmcDeviceSize);

#ifndef MINTREE_FLAG
  //
  // Misc
  //
  PcdSetBoolS (PcdDebugUsbUartEnable, FALSE);
  PcdSetBoolS (PcdM2GpioEnable, FALSE);
  PcdSetBoolS (PcdSkyCamGpioEnable, TRUE);

  PcdSetBoolS (PcdPhabletPocGpioEnable, FALSE);
  PcdSetBoolS (PcdPantherMtnGpioEnable, FALSE);

  //
  // TBT
  //
  PcdSet32S (PcdTbtSupportedPortsInfo, 0x000001);
  PcdSet8S  (PcdTbtDefaultSelectorInfo, 1);

  if ((BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Cff)||
      (BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3CffCc)){
    PcdSet8S    (PcdTbtForcePwrGpioGpioAccessType        , 1);
    PcdSet8S    (PcdTbtForcePwrGpioExpander              , 0);
    PcdSet32S   (PcdTbtForcePwrGpioGpioNumber            , GPIO_SKL_LP_GPP_C4);
    PcdSetBoolS (PcdTbtForcePwrGpioGpioLevel             , 1);
  } else {
    PcdSet8S    (PcdTbtForcePwrGpioGpioAccessType, 2);
    PcdSet8S    (PcdTbtForcePwrGpioExpander              , 0);
    PcdSet32S   (PcdTbtForcePwrGpioGpioNumber            , 13); //P15 pin
    PcdSetBoolS (PcdTbtForcePwrGpioGpioLevel             , 1);
  }
  PcdSet8S    (PcdTbtCioPlugEventGpioGpioAccessType    , 1);
  PcdSet32S   (PcdTbtCioPlugEventGpioGpioNumber        , GPIO_SKL_LP_GPP_B17);
  if ((BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Cff)||
      (BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3CffCc)){
    PcdSet32S   (PcdTbtCioPlugEventGpioGpioNumber      , GPIO_SKL_LP_GPP_B16);
  }
  PcdSetBoolS (PcdTbtCioPlugEventGpioGpioMode          , 0);

  PcdSet32S   (PcdTbtCioPlugEventGpioAcpiGpeSignature  , SIGNATURE_32 ('_', 'E', '1', '1'));
  PcdSet16S   (PcdTbtCioPlugEventGpioAcpiGpeStsAddress , (EFI_ACPI_PM1A_EVT_BLK_ADDRESS + 0x8C)); //GPE0_STS_127_96
  PcdSet8S    (PcdTbtCioPlugEventGpioAcpiGpeStsBit, 15);                                          //GPIO_TIER2_SCI_STS
  PcdSetBoolS (PcdTbtCioPlugEventGpioAcpiGpeSignaturePorting, 0);
  PcdSet32S   (PcdTbtCioPlugEventGpioAcpiGpioGpeNumber, 0x6F);                                    // 2-tier GPE will all generate GPE_111

  if (BoardIdOrgValue == BoardIdKabyLakeYLpddr3Rvp3 ||
      BoardIdOrgValue == BoardIdAmberLakeLpddr3Rvp3) {
    if (SetupData.TbtSupport == 1) {
      PcdSetBoolS(PcdTbtUsbSplitMode, TRUE);
      PcdSet8S(PcdSplitModePchUsb2TbtPort1, 2);
      PcdSet8S(PcdSplitModePchUsb2TbtPort2, 5);
    }
  }

  if ( BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Rvp3 ) {
    if (SetupData.TbtSupport == 1) {
      PcdSetBoolS (PcdUsbTypeCSupport, TRUE);
      PcdSetBoolS(PcdTbtUsbSplitMode, TRUE);
      PcdSetBoolS (PcdUsbTypeCMultiPortSupport, TRUE);
      PcdSet8S(PcdSplitModePchUsb2TbtPort1, 2);
      PcdSet8S(PcdSplitModePchUsb2TbtPort2, 7);
    }
  }

  // Enabling DVRM Card Support for AML Y42 board.
  if (BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Rvp3) {
    PcdSetBoolS(PcdDvrmVmCardSupport, TRUE);
  }

  PcdSet32S (PcdFuncPeiBoardSpecificInitPostMem, (UINTN) PeiSkylakeA0Rvp3SpecificInitPostMem);

  if (BoardIdOrgValue == BoardIdKabyLakeYLpddr3Rvp3 ||
      BoardIdOrgValue == BoardIdAmberLakeLpddr3Rvp3) {
    PcdSetBoolS (PcdPcieWwanEnable, TRUE);
    PcdSet8S (PcdWwanInterface, SetupData.WwanInterface);
    PcdSet32S (PcdBoardGpioTablePcieWwan, (UINTN) mGpioTableKabyLakeYLpDdr3Rvp3PcieWwan);
    PcdSet16S (PcdBoardGpioTablePcieWwanSize, mGpioTableKabyLakeYLpDdr3Rvp3PcieWwanSize);
    PcdSet32S (PcdBoardGpioTableIoExpanderPcieWwan, (UINTN) mGpioTableIoExpanderKabylakeRvp3PcieWwan);
    PcdSet16S (PcdBoardGpioTableIoExpanderPcieWwanSize, mGpioTableIoExpanderKabylakeRvp3PcieWwanSize);
    PcdSet32S (PcdBoardGpioTablePcieSlot1, (UINTN) mGpioTableKabyLakeYLpDdr3Rvp3PcieSlot1);
    PcdSet16S (PcdBoardGpioTablePcieSlot1Size, mGpioTableKabyLakeYLpDdr3Rvp3PcieSlot1Size);
    PcdSet32S (PcdBoardGpioTableIoExpanderPcieSlot1, (UINTN) mGpioTableIoExpanderKabylakeRvp3PcieSlot1);
    PcdSet16S (PcdBoardGpioTableIoExpanderPcieSlot1Size, mGpioTableIoExpanderKabylakeRvp3PcieSlot1Size);
  }

  if (BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Rvp3) {
    PcdSetBoolS (PcdPcieWwanEnable, TRUE);
    PcdSet8S (PcdWwanInterface, 0);
    PcdSet32S (PcdBoardGpioTablePcieWwan, (UINTN) mGpioTableAmberLakeY42LpDdr3Rvp3PcieWwan);
    PcdSet16S (PcdBoardGpioTablePcieWwanSize, mGpioTableAmberLakeY42LpDdr3Rvp3PcieWwanSize);
    PcdSet32S (PcdBoardGpioTableIoExpanderPcieWwan, (UINTN) mGpioTableIoExpanderAmberLakeY42Rvp3PcieWwanTbt);
    PcdSet16S (PcdBoardGpioTableIoExpanderPcieWwanSize, mGpioTableIoExpanderAmberLakeY42Rvp3PcieWwanTbtSize);
    PcdSet32S (PcdBoardGpioTableIoExpanderPcieSlot1, (UINTN) mGpioTableIoExpanderAmberLakeY42Rvp3PcieSlot1);
    PcdSet16S (PcdBoardGpioTableIoExpanderPcieSlot1Size, mGpioTableIoExpanderAmberLakeY42Rvp3PcieSlot1Size);
  }
  if ((BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Cff)||
      (BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3CffCc)) {
    PcdSetBoolS (PcdPcieWwanEnable, TRUE);
    PcdSet8S (PcdWwanInterface, 0);
    PcdSet32S (PcdBoardGpioTablePcieWwan, (UINTN) mGpioTableAmberLakeY42LpDdr3CffPcieWwan);
    PcdSet16S (PcdBoardGpioTablePcieWwanSize, mGpioTableAmberLakeY42LpDdr3CffPcieWwanSize);
    PcdSet32S (PcdBoardGpioTablePcieSlot1, (UINTN) mGpioTableAmberLakeY42LpDdr3CffPcieSlot1);
    PcdSet16S (PcdBoardGpioTableIoExpanderPcieSlot1Size, mGpioTableAmberLakeY42LpDdr3CffPcieSlot1Size);
  }

  if (BoardIdOrgValue == BoardIdKabyLakeYLpddr3Rvp3 ||
      BoardIdOrgValue == BoardIdAmberLakeLpddr3Rvp3) {
    if ((SetupData.TbtSupport == 1) && (SetupData.Rtd3Tbt == 1 && SetupData.Rtd3Support == 1)) {
      PcdSet32S (PcdBoardGpioTableIoExpanderPcieSlot1, (UINTN) mGpioTableIoExpanderKabylakeRvp3AmlPcieSlot1Tbt);
      PcdSet16S (PcdBoardGpioTableIoExpanderPcieSlot1Size, mGpioTableIoExpanderKabylakeRvp3AmlPcieSlot1TbtSize);
    }
  }

  if (BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Rvp3) {
    if ((SetupData.TbtSupport == 1) && (SetupData.Rtd3Tbt == 1 && SetupData.Rtd3Support == 1)) {
      PcdSet32S (PcdBoardGpioTableIoExpanderPcieSlot1, (UINTN) mGpioTableIoExpanderAmberLakeY42Rvp3PcieWwanTbt);
      PcdSet16S (PcdBoardGpioTableIoExpanderPcieSlot1Size, mGpioTableIoExpanderAmberLakeY42Rvp3PcieWwanTbtSize);
    }
  }
  if ((BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Cff) ||
      (BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3CffCc)){
    if ((SetupData.TbtSupport == 1) && (SetupData.Rtd3Tbt == 1 && SetupData.Rtd3Support == 1)) {
    PcdSet32S (PcdBoardGpioTablePcieSlot1, (UINTN) mGpioTableAmberLakeY42LpDdr3CffPcieSlot1Tbt);
    PcdSet16S (PcdBoardGpioTableIoExpanderPcieSlot1Size, mGpioTableAmberLakeY42LpDdr3CffPcieSlot1TbtSize);
    }
  }
#endif

  return EFI_SUCCESS;

}

#ifndef MINTREE_FLAG
/**
  SkylaeA0Rvp3Ppv board configuration init function for PEI post memory phase.

  @param  Content  pointer to the buffer contain init information for board init.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   The parameter is NULL.
**/
EFI_STATUS
EFIAPI //AMI_OVERRIDE - EIP431111 [KBL] EFIAPI function qualifiers for Linux Build.
PeiSkylakeA0Rvp3PpvInit (
  IN VOID *Content
  )
{
  return PeiSkylakeA0Rvp3Init (Content);
}
#endif

/**
  The constructor function for SkylakeA0Rvp3 Board Init Libray.

  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services.

  @retval  EFI_SUCCESS            Table initialization successfully.
  @retval  EFI_OUT_OF_RESOURCES   No enough memory to initialize table.
**/
EFI_STATUS
EFIAPI
PeiSkylakeA0Rvp3BoardInitLibConstructor (
  IN EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  return CommonInitInternalLibConstructor (mRvp3BoardInit, TABLE_LIST_NUM);
}
