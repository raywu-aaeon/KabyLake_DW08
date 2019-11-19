/** @file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2016 - 2017 Intel Corporation.

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

#include "PeiSkylakeURvp7InitLib.h"

#include "GpioTableDdr3LrRvp7.h"
#include "SkylakeURvp7HdaVerbTables.h"
#include <../../Others/PeiBoardInitLib/PchHdaVerbTables.h>
#include <Library/GpioExpanderLib.h>
#include <Library/PeiPlatformHookLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/BaseMemoryLib.h>

extern GPIO_INIT_CONFIG mGpioTableIoExpander [];
extern UINT16 mGpioTableIoExpanderSize;
extern GPIO_INIT_CONFIG mGpioTableIoExpanderKabylakeRDdr4[];
extern UINT16 mGpioTableIoExpanderSizeKabylakeRDdr4;
extern GPIO_INIT_CONFIG mGpioTableBoardIdKabylakeRDdr4PcieSlot1[];
extern UINT16 mGpioTableBoardIdKabylakeRDdr4PcieSlot1Size;
extern IO_EXPANDER_GPIO_CONFIG mGpioTableIoExpanderKabylakeRDdr4PcieSlot1[];
extern UINT16 mGpioTableIoExpanderSizeKabylakeRDdr4PcieSlot1;
extern IO_EXPANDER_GPIO_CONFIG mGpioTableIoExpanderKabylakeRDdr4PcieSlot1S4Resume[];
extern UINT16 mGpioTableIoExpanderSizeKabylakeRDdr4PcieSlot1S4Resume;
extern IO_EXPANDER_GPIO_CONFIG mGpioTableIoExpanderKabylakeRDdr4S4Resume[];
extern UINT16 mGpioTableIoExpanderSizeKabylakeRDdr4S4Resume;
extern GPIO_INIT_CONFIG mGpioTableIoExpanderKabylakeKcDdr3[];
extern UINT16 mGpioTableIoExpanderSizeKabylakeKcDdr3;

EFI_STATUS
GetBoardId (
  OUT UINT16 *BoardId,
  OUT UINT16 *BoardIdOrgValue
  );

//
// RVP7 Init Table
//
BOARD_INIT_TABLE mRvp7BoardInit [] = {
  {BoardIdSkylakeURvp7, PeiSkylakeURvp7Init},
  {BoardIdKabylakeRDdr4, PeiSkylakeURvp7Init},
  {BoardIdKabylakeRLpddr3, PeiSkylakeURvp7Init},
  {BoardIdKabylakeKcDdr3, PeiSkylakeURvp7Init},
  {BoardIdSkylakeURvp7Ppv, PeiSkylakeURvp7PpvInit},
  { 0, 0 }
};

/**
  Board Specific Init function hook for PEI post memory phase.

  @retval EFI_SUCCESS   The function completed successfully.
**/
EFI_STATUS
PeiSkylakeURvp7SpecificInitPostMem (
  VOID
  )
{
  return EFI_SUCCESS;
}

/**
  SkylaeURvp7 board configuration init function for PEI pre-memory phase.

  @param  Content  pointer to the buffer contain init information for board init.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   The parameter is NULL.
**/
EFI_STATUS
EFIAPI //AMI_OVERRIDE - EIP431111 [KBL] EFIAPI function qualifiers for Linux Build.
PeiSkylakeURvp7Init (
  IN VOID *Content
  )
{
  EFI_STATUS Status;
  PLATFORM_INFO *PlatformInfo;
  UINT16 BoardId;
  UINT16 BoardIdOrgValue;
  EFI_BOOT_MODE BootMode;
  SETUP_DATA SystemConfiguration;
  UINTN VarSize;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;

  //
  // Locate Setup variables
  //
  ZeroMem (&SystemConfiguration, sizeof (SystemConfiguration));
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &VariableServices
             );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  VarSize = sizeof (SETUP_DATA);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"Setup",
                               &gSetupVariableGuid,
                               NULL,
                               &VarSize,
                               &SystemConfiguration
                               );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Get the Current Boot Mode
  //
  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (Content == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetBoardId (&BoardId, &BoardIdOrgValue);
  ASSERT_EFI_ERROR (Status);

  if (EFI_ERROR (Status)) {
    return Status;
  }
  PlatformInfo = (PLATFORM_INFO *) Content;

  PcdSetBoolS (PcdSkyCamGpioEnable, TRUE);

  PcdSet32S (PcdBoardGpioTable, (UINTN) mGpioTableDdr3LrRvp7Common);
  PcdSet16S (PcdBoardGpioTableSize, mGpioTableDdr3LrRvp7CommonSize);

  if ((BoardIdOrgValue == BoardIdSkylakeURvp7) || (BoardIdOrgValue == BoardIdSkylakeURvp7Ppv)) {
    PcdSet32S (PcdBoardGpioTable2, (UINTN) mGpioTableDdr3LrRvp7);
    PcdSet16S (PcdBoardGpioTable2Size, mGpioTableDdr3LrRvp7Size);
  } else if ((BoardIdOrgValue == BoardIdKabylakeRDdr4) || (BoardIdOrgValue == BoardIdKabylakeRLpddr3)) { //Customized change needed for KBL RVP R in contrast to SKL RVP7
    PcdSet32S (PcdBoardGpioTable2, (UINTN) mGpioTableBoardIdKabylakeRDdr4);
    PcdSet16S (PcdBoardGpioTable2Size, mGpioTableBoardIdKabylakeRDdr4Size);
  } else if (BoardIdOrgValue == BoardIdKabylakeKcDdr3) { //Customized change needed for KBL KC RVP in contrast to SKL RVP7
    PcdSet32S (PcdBoardGpioTable2, (UINTN) mGpioTableBoardIdKabylakeKcDdr3);
    PcdSet16S (PcdBoardGpioTable2Size, mGpioTableBoardIdKabylakeKcDdr3Size);
  } else if (BoardIdOrgValue == BoardIdKabylakeUDdr3lRvp7) {
    PcdSet32S (PcdBoardGpioTable2, (UINTN) mGpioTableKabylakeUDdr3lRvp7);
    PcdSet16S (PcdBoardGpioTable2Size, mGpioTableKabylakeUDdr3lRvp7Size);
  }

  if((BoardIdOrgValue == BoardIdKabylakeRDdr4) ||
    (BoardIdOrgValue == BoardIdKabylakeRLpddr3)) {//Customized change needed for KBL RVP R in contrast to SKL RVP7
    PcdSet32S (PcdGpioExpanderTable, (UINTN) mGpioTableIoExpanderKabylakeRDdr4);
    PcdSet16S (PcdGpioExpanderTableSize, mGpioTableIoExpanderSizeKabylakeRDdr4);

    PcdSet32S (PcdBoardGpioTableIoExpanderPcieSlot1, (UINTN) mGpioTableIoExpanderKabylakeRDdr4PcieSlot1);
    PcdSet16S (PcdBoardGpioTableIoExpanderPcieSlot1Size, mGpioTableIoExpanderSizeKabylakeRDdr4PcieSlot1);
    if (BootMode != BOOT_ON_S3_RESUME || SystemConfiguration.Rtd3WwanDsm == 1) {
      PcdSet32S (PcdBoardGpioTableModemReset, (UINTN) &mGpioTableKabylakeRDdr4ModemReset);
    }
    PcdSet32S (PcdBoardGpioTablePcieSlot1, (UINTN) mGpioTableBoardIdKabylakeRDdr4PcieSlot1);
    PcdSet16S (PcdBoardGpioTablePcieSlot1Size, mGpioTableBoardIdKabylakeRDdr4PcieSlot1Size);
  } else if(BoardIdOrgValue == BoardIdKabylakeKcDdr3) { //Customized change needed for KBL RVP KC in contrast to SKL RVP7
    PcdSet32S (PcdGpioExpanderTable, (UINTN) mGpioTableIoExpanderKabylakeKcDdr3);
    PcdSet16S (PcdGpioExpanderTableSize, mGpioTableIoExpanderSizeKabylakeKcDdr3);
  } else {
    PcdSet32S (PcdGpioExpanderTable, (UINTN) mGpioTableIoExpander);
    PcdSet16S (PcdGpioExpanderTableSize, mGpioTableIoExpanderSize);
  }

  PcdSet32S (PcdBoardGpioTableTouchPanel, (UINTN) &mGpioTableDdr3LrRvp7Touchpanel);

  PcdSet32S (PcdBoardGpioTableSdhcSidebandCardDetect, (UINTN) &mGpioTableDdr3LrRvp7SdhcSidebandCardDetect);

  PcdSet32S (PcdHdaVerbTable, (UINTN) &HdaVerbTableAlc286Rvp7);

  if((BoardIdOrgValue == BoardIdKabylakeRDdr4) || (BoardIdOrgValue == BoardIdKabylakeRLpddr3)) { //Customized change needed for KBL RVP-R in contrast to KBL RVP7
    PcdSet32S (PcdHdaVerbTable, (UINTN) &HdaVerbTableAlc298KblR);
  }

  //
  // TBT
  //


//AMI_OVERRIDE_START - EIP325488 - Thunderbolt Security Level can't be changed on KBL-Refresh U CRB
  if ((BoardIdOrgValue == BoardIdKabylakeRDdr4) || (BoardIdOrgValue == BoardIdKabylakeRLpddr3)){
    PcdSet32S   (PcdTbtSupportedPortsInfo  , 0x000001);
    PcdSet8S    (PcdTbtDefaultSelectorInfo , 01);
//    PcdSet8S    (PcdTbtForcePwrGpioGpioAccessType        , 1);
//    PcdSet8S    (PcdTbtForcePwrGpioExpander              , 0);
//    PcdSet32S   (PcdTbtForcePwrGpioGpioNumber            , GPIO_SKL_LP_GPP_E0); //P15 pin
//    PcdSetBoolS (PcdTbtForcePwrGpioGpioLevel             , 1);
  } else {
    PcdSet32S   (PcdTbtSupportedPortsInfo  , 0x000100);
    PcdSet8S    (PcdTbtDefaultSelectorInfo , 9);
  }
    PcdSet8S    (PcdTbtForcePwrGpioGpioAccessType        , 2);
    PcdSet8S    (PcdTbtForcePwrGpioExpander              , 0);
    PcdSet32S   (PcdTbtForcePwrGpioGpioNumber            , 13); //P15 pin
    PcdSetBoolS (PcdTbtForcePwrGpioGpioLevel             , 1);
//  }
//AMI_OVERRIDE_END - EIP325488 - Thunderbolt Security Level can't be changed on KBL-Refresh U CRB
  PcdSet8S    (PcdTbtCioPlugEventGpioGpioAccessType    , 1);
  PcdSet32S   (PcdTbtCioPlugEventGpioGpioNumber        , GPIO_SKL_LP_GPP_B17);
  PcdSetBoolS (PcdTbtCioPlugEventGpioGpioMode          , 0);

  PcdSet32S   (PcdTbtCioPlugEventGpioAcpiGpeSignature  , SIGNATURE_32 ('_', 'E', '1', '1'));
  PcdSet16S   (PcdTbtCioPlugEventGpioAcpiGpeStsAddress , (EFI_ACPI_PM1A_EVT_BLK_ADDRESS + 0x8C)); //GPE0_STS_127_96
  PcdSet8S    (PcdTbtCioPlugEventGpioAcpiGpeStsBit     , 15); //GPIO_TIER2_SCI_STS
  PcdSetBoolS (PcdTbtCioPlugEventGpioAcpiGpeSignaturePorting , 0);
  PcdSet32S   (PcdTbtCioPlugEventGpioAcpiGpioGpeNumber , 0x6F); // 2-tier GPE will all generate GPE_111

  if(BoardIdOrgValue == BoardIdKabylakeKcDdr3) {
    PcdSet32S   (PcdTbtSupportedPortsInfo  , 0x000100);
    PcdSet8S    (PcdTbtDefaultSelectorInfo , 9);
    PcdSet8S    (PcdTbtForcePwrGpioGpioAccessType        , 1);
    PcdSet8S    (PcdTbtForcePwrGpioExpander              , 0);
    PcdSet32S   (PcdTbtForcePwrGpioGpioNumber            , GPIO_SKL_LP_GPP_A8);
    PcdSetBoolS (PcdTbtForcePwrGpioGpioLevel             , 1);

    PcdSet8S    (PcdTbtCioPlugEventGpioGpioAccessType    , 1);
    PcdSet32S   (PcdTbtCioPlugEventGpioGpioNumber        , GPIO_SKL_LP_GPP_D10);
    PcdSetBoolS (PcdTbtCioPlugEventGpioGpioMode          , 0);

    PcdSet32S   (PcdTbtCioPlugEventGpioAcpiGpeSignature  , SIGNATURE_32 ('_', 'E', '1', '1'));
    PcdSet16S   (PcdTbtCioPlugEventGpioAcpiGpeStsAddress , (EFI_ACPI_PM1A_EVT_BLK_ADDRESS + 0x8C)); //GPE0_STS_127_96
    PcdSet8S    (PcdTbtCioPlugEventGpioAcpiGpeStsBit     , 15); //GPIO_TIER2_SCI_STS
    PcdSetBoolS (PcdTbtCioPlugEventGpioAcpiGpeSignaturePorting , 0);
    PcdSet32S   (PcdTbtCioPlugEventGpioAcpiGpioGpeNumber , 0x6F); // 2-tier GPE will all generate GPE_111

    PcdSet32S   (PcdTbtSupportedPortsInfo1 , 0x000010);
    PcdSet8S    (PcdTbtDefaultSelectorInfo1 , 5);

    PcdSet8S    (PcdTbtForcePwrGpioGpioAccessType1        , 1);
    PcdSet8S    (PcdTbtForcePwrGpioExpander1              , 0);
    PcdSet32S   (PcdTbtForcePwrGpioGpioNumber1            , GPIO_SKL_LP_GPP_A6);
    PcdSetBoolS (PcdTbtForcePwrGpioGpioLevel1             , 1);

    PcdSet8S    (PcdTbtCioPlugEventGpioGpioAccessType1    , 1);
    PcdSet32S   (PcdTbtCioPlugEventGpioGpioNumber1        , GPIO_SKL_LP_GPP_D9);
    PcdSetBoolS (PcdTbtCioPlugEventGpioGpioMode1          , 0);

    PcdSet32S   (PcdTbtCioPlugEventGpioAcpiGpeSignature1  , SIGNATURE_32 ('_', 'E', '1', '1'));
    PcdSet16S   (PcdTbtCioPlugEventGpioAcpiGpeStsAddress1 , (EFI_ACPI_PM1A_EVT_BLK_ADDRESS + 0x8C)); //GPE0_STS_127_96
    PcdSet8S    (PcdTbtCioPlugEventGpioAcpiGpeStsBit1     , 15); //GPIO_TIER2_SCI_STS
    PcdSetBoolS (PcdTbtCioPlugEventGpioAcpiGpeSignaturePorting1 , 0);
    PcdSet32S   (PcdTbtCioPlugEventGpioAcpiGpioGpeNumber1 , 0x6F); // 2-tier GPE will all generate GPE_111
  }
  if((BoardIdOrgValue == BoardIdKabylakeRDdr4) || (BoardIdOrgValue == BoardIdKabylakeRLpddr3)) {
    PcdSetBoolS(PcdTbtUsbSplitMode,TRUE);
    PcdSet8S(PcdSplitModePchUsb2TbtPort1,1);
    PcdSet8S(PcdSplitModePchUsb2TbtPort2,4);
  }

  if (BoardIdOrgValue == BoardIdKabylakeRDdr4) {
    // WWAN for KBL-R
    PcdSet32S (PcdWwanGnssUartReset, GPIO_SKL_LP_GPP_A12);
    PcdSetBoolS (PcdPcieWwanEnable, TRUE);
    PcdSet8S (PcdWwanInterface, 0);
    // GPIO for VM VR control
    PcdSet32S (PcdRunTimeVmVrGpio, GPIO_SKL_LP_GPP_E12);
  }
  PcdSet32S (PcdFuncPeiBoardSpecificInitPostMem, (UINTN) PeiSkylakeURvp7SpecificInitPostMem);

  return EFI_SUCCESS;
}

/**
  SkylaeURvp7 board configuration init function for PEI pre-memory phase.

  @param  Content  pointer to the buffer contain init information for board init.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   The parameter is NULL.
**/
EFI_STATUS
EFIAPI //AMI_OVERRIDE - EIP431111 [KBL] EFIAPI function qualifiers for Linux Build.
PeiSkylakeURvp7PpvInit (
  IN VOID *Content
  )
{
  EFI_STATUS Status;

  Status = PeiSkylakeURvp7Init (Content);

  return EFI_SUCCESS;
}

/**
  The constructor function for SkylakeA0Rvp3 Board Init Libray.

  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services.

  @retval  EFI_SUCCESS            Table initialization successfully.
  @retval  EFI_OUT_OF_RESOURCES   No enough memory to initialize table.
**/
EFI_STATUS
EFIAPI
PeiSkylakeURvp7BoardInitLibConstructor (
  IN EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  return CommonInitInternalLibConstructor (mRvp7BoardInit, TABLE_LIST_NUM);
}

