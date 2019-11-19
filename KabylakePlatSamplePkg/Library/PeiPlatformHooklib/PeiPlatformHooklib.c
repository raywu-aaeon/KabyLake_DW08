/** @file
  PEI Library Functions. Initialize GPIOs

 @copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2019 Intel Corporation.

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

#include <PiPei.h>
#include <Library/PeiPlatformHookLib.h>
#include <PlatformBoardId.h>
#include <Ppi/ReadOnlyVariable2.h> // AdvancedFeaturesContent
#include <SaPolicyCommon.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/TimerLib.h> // AdvancedFeaturesContent
#include <Library/PchCycleDecodingLib.h>
#include <Library/PeiPlatformLib.h>
#include <Library/PciLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/EcMiscLib.h> // AdvancedFeaturesContent
#include <Library/EcLib.h> // AdvancedFeaturesContent
#include <Library/PchPmcLib.h> // AdvancedFeaturesContent
#include <Library/PeiSaPolicyLib.h>
#include <CmosMap.h> // AdvancedFeaturesContent
#include <PchAccess.h>
#include <SaDataHob.h> // AdvancedFeaturesContent
#include <Guid/ImageAuthentication.h> // AdvancedFeaturesContent
#include <Library/MmPciLib.h> // AdvancedFeaturesContent
#include <Library/CpuPlatformLib.h> // AdvancedFeaturesContent
#include <Library/GpioNativeLib.h>
#include <Library/GpioLib.h>
#include <GpioPinsSklLp.h>
#include <GpioPinsSklH.h>
#include <Library/GpioExpanderLib.h>
#include <SioRegs.h>
#include <Library/BoardConfigLib.h>
#include <PlatformBoardConfig.h>
#include <Library/PchPcrLib.h>
#include <Board/BoardInitTable.h>

extern EFI_GUID gSaDataHobGuid; // AdvancedFeaturesContent

#define ULT_LPC_SIO_CONFIG_DEFAULT_PORT                       0x2E
#define ULT_LPC_SIO_INDEX_DEFAULT_PORT                        0x2E
#define ULT_LPC_SIO_DATA_DEFAULT_PORT                         0x2F
#define ULX_LPC_SIO_CONFIG_DEFAULT_PORT                       0x164E
#define ULX_LPC_SIO_INDEX_DEFAULT_PORT                        0x164E
#define ULX_LPC_SIO_DATA_DEFAULT_PORT                         0x164F
#define SIO_RUNTIME_REG_BASE_ADDRESS                          0x0680

#define RECOVERY_MODE_GPIO_PIN                    0                    // Platform specific @todo use PCD

#define POWER_GATE_XX_GPIO_PIN                    0                    // Platform specific @todo use PCD

#define MANUFACTURE_MODE_GPIO_PIN                 0                    // Platform specific @todo use PCD

#define EXPANDERS                                 2                    // defines expander's quantity

#define UP_ZUMBA_BEACH_MANUFACTURE_MODE_GPIO_PAD              GPIO_SKL_H_GPP_C9

typedef
EFI_STATUS
(EFIAPI *PEI_BOARD_SPECIFIC_INIT_POST_MEM) (
  IN VOID
);

/**
  Get platform information

  @param[out]  PlatformInfo   Get platforminfo from HOB

**/
EFI_STATUS
GetPlatformInfo (
  OUT PLATFORM_INFO                     **PlatformInfo
  )
{
  EFI_STATUS                            Status;
  EFI_PEI_HOB_POINTERS                  Hob;

  *PlatformInfo = NULL;
  //
  // Find the PlatformInfo HOB
  //
  Status = PeiServicesGetHobList ((VOID **) &Hob.Raw);
  while (!END_OF_HOB_LIST (Hob)) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION && (CompareMem (&Hob.Guid->Name, &gPlatformInfoHobGuid, sizeof(gPlatformInfoHobGuid))==0)) {
      *PlatformInfo = (PLATFORM_INFO*) (Hob.Raw + sizeof (EFI_HOB_GENERIC_HEADER) + sizeof (EFI_GUID));
      break;
    }
    Hob.Raw = GET_NEXT_HOB (Hob);
  }

  //
  // PlatformInfo PEIM should provide this HOB data, if not ASSERT and return error.
  //
  ASSERT(*PlatformInfo != NULL);
  if (!(*PlatformInfo)) {
    Status = EFI_NOT_FOUND;
    return Status;
  }

  return Status;
}


/**
  Configures GPIO

  @param[in]  GpioTable       Point to Platform Gpio table
  @param[in]  GpioTableCount  Number of Gpio table entries

**/
VOID
ConfigureGpio (
  IN GPIO_INIT_CONFIG                 *GpioDefinition,
  IN UINT16                           GpioTableCount
  )
{
  EFI_STATUS          Status;

  DEBUG ((DEBUG_INFO, "ConfigureGpio() Start\n"));

  Status = GpioConfigurePads (GpioTableCount, GpioDefinition);

  DEBUG ((DEBUG_INFO, "ConfigureGpio() End\n"));
}
#ifndef MINTREE_FLAG
/**
Function to Enable/Disable BT wireless charging

@param[in]  WCMode  Wireless Charging enable/disable

**/
VOID
ConfBTWirelessCharging(
  IN UINT8  WCMode
  )
{
    //Configure WirelessCharging Based on the Mode
    if (WCMode == 1) {

        //WirelessCharging Enabled, Conf TDB GPIO

    }
    else {

        //Disable Wireless Charging

    }
}
#endif

VOID
SetBit (
  IN OUT UINT32  *Value,
  IN     UINT32  BitNumber,
  IN     BOOLEAN NewBitValue
  )
{
  if (NewBitValue) {
    *Value |= 1 << BitNumber;
  } else {
    *Value &= ~(1 << BitNumber);
  }
}

/**
  Configures IO Expander GPIO device

  @param[in]  IOExpGpioDefinition  Point to IO Expander Gpio table
  @param[in]  IOExpGpioTableCount  Number of Gpio table entries

**/
void
ConfigureIoExpanderGpio (
  IN IO_EXPANDER_GPIO_CONFIG        *IoExpGpioDefinition1,
  IN UINT16                          IoExpGpioTableCount1,
  IN IO_EXPANDER_GPIO_CONFIG        *IoExpGpioDefinition2,
  IN UINT16                          IoExpGpioTableCount2
  )
{
  UINT8               Index;
  UINT32              Direction[EXPANDERS] = {0x00FFFFFF, 0x00FFFFFF};
  UINT32              Level[EXPANDERS] = {0};
  UINT32              Polarity[EXPANDERS] = {0};

   // IoExpander {TCA6424A}
  DEBUG ((DEBUG_INFO, "IO Expander Configuration Start\n"));
  if (IoExpGpioDefinition1 != NULL) {
    for (Index = 0; Index < IoExpGpioTableCount1; Index++) {   //Program IO Expander as per the table defined in PeiPlatformHooklib.c
      SetBit(&Direction[IoExpGpioDefinition1[Index].IoExpanderNumber], IoExpGpioDefinition1[Index].GpioPinNumber, (BOOLEAN)IoExpGpioDefinition1[Index].GpioDirection);
      SetBit(&Level[IoExpGpioDefinition1[Index].IoExpanderNumber], IoExpGpioDefinition1[Index].GpioPinNumber, (BOOLEAN)IoExpGpioDefinition1[Index].GpioLevel);
      SetBit(&Polarity[IoExpGpioDefinition1[Index].IoExpanderNumber], IoExpGpioDefinition1[Index].GpioPinNumber, (BOOLEAN)IoExpGpioDefinition1[Index].GpioInversion);
    }
  }
  if (IoExpGpioDefinition2 != NULL) {
    for (Index = 0; Index < IoExpGpioTableCount2; Index++) {   //Program IO Expander as per the table defined in PeiPlatformHooklib.c
      SetBit(&Direction[IoExpGpioDefinition2[Index].IoExpanderNumber], IoExpGpioDefinition2[Index].GpioPinNumber, (BOOLEAN)IoExpGpioDefinition2[Index].GpioDirection);
      SetBit(&Level[IoExpGpioDefinition2[Index].IoExpanderNumber], IoExpGpioDefinition2[Index].GpioPinNumber, (BOOLEAN)IoExpGpioDefinition2[Index].GpioLevel);
      SetBit(&Polarity[IoExpGpioDefinition2[Index].IoExpanderNumber], IoExpGpioDefinition2[Index].GpioPinNumber, (BOOLEAN)IoExpGpioDefinition2[Index].GpioInversion);
  }
  }
  for (Index = 0; Index < EXPANDERS; Index++) {
    GpioExpBulkConfig(Index, Direction[Index], Polarity[Index], Level[Index]);
  }
  DEBUG ((DEBUG_INFO, "IO Expander Configuration End\n"));
  return;
}
#ifndef MINTREE_FLAG
/**
  Program the changes required for Thunderbolt(TM)

  @param[in]  PeiServices       General purpose services available to every PEIM.
  @param[in]  NotifyDescriptor
  @param[in]  Interface

  @retval     EFI_SUCCESS       Operation success.
**/
EFI_STATUS
EFIAPI
ThunderboltCcUpdateNotify (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Interface
  )
{
  //GpioSetHostSwOwnershipForPad (TBT_GPIO_PIN, 0x0);     // ACPI driver
  //PchGpioGpiRoutSciSet (TBT_GPIO_PIN, V_PCH_GPIO_RX_SCI_ROUTE_EN);   // Route SCI
  return EFI_SUCCESS;
}

/**
  Enable the UART/USB mux for kernel debug on RVP5.

  @param[in]  PeiServices       General purpose services available to every PEIM.

  @retval     VOID
**/
VOID
UartUsbDebugConf (
  IN BOARD_CONFIG_BLOCK_PEI_POSTMEM   *PlatformBoardConfig
  )
{
  EFI_STATUS                      Status;
  SETUP_DATA                      SystemConfiguration;
  UINTN                           VarSize;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;

  //
  // Obtain Platform Info from HOB.
  //
  if (PlatformBoardConfig->Enable.DebugUsbUart) {

    //
    // Locate system configuration variable
    //
    VarSize = sizeof (SETUP_DATA);
    Status = PeiServicesLocatePpi (
               &gEfiPeiReadOnlyVariable2PpiGuid,
               0,
               NULL,
               (VOID **) &VariableServices
               );
    if (EFI_ERROR(Status)) {
      return;
    }

    Status = VariableServices->GetVariable (
                                 VariableServices,
                                 L"Setup",
                                 &gSetupVariableGuid,
                                 NULL,
                                 &VarSize,
                                 &SystemConfiguration
                                 );
    if (EFI_ERROR(Status)) {
      return;
    }

    if (SystemConfiguration.DebugUsbUart == 0) {
      DEBUG((DEBUG_INFO, "RVP5: USB Debug (Low)\n"));
      GpioExpSetOutput(IO_EXPANDER_1, IO_EXPANDER_GPIO_10, IO_EXPANDER_GPO_LEVEL_LOW);
    } else {
      DEBUG((DEBUG_INFO, "RVP5: UART Debug (High)\n"));
      GpioExpSetOutput(IO_EXPANDER_1, IO_EXPANDER_GPIO_10, IO_EXPANDER_GPO_LEVEL_HIGH);
    }
    GpioExpSetDirection(IO_EXPANDER_1, IO_EXPANDER_GPIO_10, IO_EXPANDER_GPIO_OUTPUT);
  }
}

/**
  Configure GPIO behind IoExpander.

  @param[in]  PeiServices       General purpose services available to every PEIM.
  @param[in]  NotifyDescriptor
  @param[in]  Interface

  @retval     EFI_SUCCESS       Operation success.
**/
VOID
ExpanderGpioInitPreMem (
  IN PLATFORM_INFO             *PlatformInfo
  )
{
  return;
}
#endif

/**
  Configure GPIO behind IoExpander.

  @param[in]  PeiServices       General purpose services available to every PEIM.
  @param[in]  NotifyDescriptor
  @param[in]  Interface

  @retval     EFI_SUCCESS       Operation success.
**/
VOID
ExpanderGpioInit (
  IN BOARD_CONFIG_BLOCK_PEI_POSTMEM   *PlatformBoardConfig
  )
{
  ConfigureIoExpanderGpio (PlatformBoardConfig->GpioExpanderTable1,
                           PlatformBoardConfig->GpioExpanderTable1Size,
                           PlatformBoardConfig->GpioExpanderTable2,
                           PlatformBoardConfig->GpioExpanderTable2Size
                           );
  UartUsbDebugConf (PlatformBoardConfig); // AdvancedFeaturesContent
}
#ifndef MINTREE_FLAG
/**
  Configure GPIO group GPE tier.

  @param[in]  PlatformInfo

  @retval     none.
**/
VOID
GpioGroupTierInit (
  IN BOARD_CONFIG_BLOCK_PEI_PREMEM   *PlatformBoardConfig
  )
{
  DEBUG ((DEBUG_INFO, "GpioGroupTierInit Start\n"));

  if (PlatformBoardConfig->Enable.GpioGroupTierInit == 1) {
    GpioSetGroupToGpeDwX (PlatformBoardConfig->GroupToGpeDw0,
                          PlatformBoardConfig->GroupToGpeDw1,
                          PlatformBoardConfig->GroupToGpeDw2);
  }
  DEBUG ((DEBUG_INFO, "GpioGroupTierInit End\n"));
}
#endif

/**
  Configure single GPIO pad for touchpanel interrupt

  @param[in]  PlatformInfo
**/
VOID
TouchpanelGpioInit (
  IN BOARD_CONFIG_BLOCK_PEI_POSTMEM  *PlatformBoardConfig
  )
{
  GPIO_INIT_CONFIG*     TouchpanelPad;
  GPIO_PAD_OWN          PadOwnVal;

  PadOwnVal = 0;
  TouchpanelPad = PlatformBoardConfig->TouchpanelPad;
  if (TouchpanelPad != NULL) {
    GpioGetPadOwnership (TouchpanelPad->GpioPad, &PadOwnVal);
    if (PadOwnVal == GpioPadOwnHost) {
      GpioConfigurePads (1, TouchpanelPad);
    }
  }
}

#ifndef MINTREE_FLAG
/**
  Configure single GPIO pad for SCS SDCard sideband wake interrupt

  @param[in]  PlatformInfo
**/
VOID
ScsSdCardGpioInit (
  IN BOARD_CONFIG_BLOCK_PEI_POSTMEM  *PlatformBoardConfig
  )
{
  PCH_SETUP                       PchSetup;
  UINTN                           VarSize;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  EFI_STATUS                      Status;

  //
  // Locate Setup variables
  //
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &VariableServices
             );
  if (EFI_ERROR(Status)) {
    return;
  }

  VarSize = sizeof (PCH_SETUP);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"PchSetup",
                               &gPchSetupVariableGuid,
                               NULL,
                               &VarSize,
                               &PchSetup
                               );
  if (EFI_ERROR(Status)) {
    return;
  }

  // Check if SDCard enabled
  if (PchSetup.PchScsSdCardEnabled == FALSE) {
    DEBUG((DEBUG_INFO, "ScsSdCardGpioInit: SDCard disabled!\n"));
    return;
  }

  if (PlatformBoardConfig->SdhcSidebandCardDetect != NULL) {
    if (PchSetup.PchScsSdCardSidebandEventsSupport == SDCARD_SIDEBAND_CD_GPP_B17) {
      // Use GPP_B17 as GpioInt
      DEBUG((DEBUG_INFO, "ScsSdCardGpioInit: SDCard sideband events supported (wake from D3) - configure GPP_B17\n"));
      GpioSetPadConfig(PlatformBoardConfig->SdhcSidebandCardDetect->GpioPad, &(PlatformBoardConfig->SdhcSidebandCardDetect->GpioConfig));
    }
    if (PchSetup.PchScsSdCardSidebandEventsSupport == SDCARD_SIDEBAND_CD_GPP_D10) {
      // Use GPP_D10 as GpioInt
      DEBUG((DEBUG_INFO, "ScsSdCardGpioInit: SDCard sideband events supported (wake from D3) - configure GPP_D10\n"));
      GpioSetPadConfig(GPIO_SKL_LP_GPP_D10, &(PlatformBoardConfig->SdhcSidebandCardDetect->GpioConfig));
    }
  }

}

VOID
SkyCamConfigureGpio(
  IN GPIO_CONFIG *GpioConfig
  )
{
  ZeroMem(GpioConfig, sizeof(GPIO_CONFIG));
  GpioConfig->PadMode = GpioPadModeGpio;
  GpioConfig->HostSoftPadOwn = GpioHostOwnGpio;
  GpioConfig->Direction = GpioDirOut;
  GpioConfig->OutputState = GpioOutDefault;
  GpioConfig->InterruptConfig = GpioIntDis;
  GpioConfig->PowerConfig = GpioPlatformReset;
  GpioConfig->ElectricalConfig = GpioTermNone;
}

VOID
SkyCamGpioInit(
  IN BOARD_CONFIG_BLOCK_PEI_POSTMEM  *PlatformBoardConfig
  )
{
  PCH_SETUP                       PchSetup;
  SETUP_DATA                      SetupData;
  UINTN                           VarSize;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  EFI_STATUS                      Status;
  GPIO_CONFIG                     GpioConfig;
  GPIO_PAD                        GpioPad;
  UINT8                           GpioPin;

  //
  // Locate Setup variables
  //
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &VariableServices
             );
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "SkyCamGpioInit: PeiServicesLocatePpi failed\n"));
    return;
  }

  VarSize = sizeof (PCH_SETUP);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"PchSetup",
                               &gPchSetupVariableGuid,
                               NULL,
                               &VarSize,
                               &PchSetup
                               );
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "SkyCamGpioInit: GetVariable (PchSetup) failed\n"));
    return;
  }

  VarSize = sizeof (SETUP_DATA);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"Setup",
                               &gSetupVariableGuid,
                               NULL,
                               &VarSize,
                               &SetupData
                               );
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "SkyCamGpioInit: GetVariable (SetupData) failed\n"));
    return;
  }

  // Check if CIO2 is enabled
  if (PchSetup.PchSkyCamCio2Enable == FALSE) {
    DEBUG((DEBUG_INFO, "SkyCamGpioInit: CIO2 disabled!\n"));
    return;
  }

  DEBUG((DEBUG_INFO, "SkyCamGpioInit: CIO2 enabled\n"));
  SkyCamConfigureGpio (&GpioConfig);
  if ((SetupData.SkyCam_ControlLogic0) && (SetupData.SkyCam_ControlLogic0_Type == (UINT8)1) && (SetupData.SkyCam_ControlLogic0_GpioPinsEnabled)) {
    DEBUG((DEBUG_INFO, "SkyCamGpioInit ControlLogic0\n"));
    for (GpioPin = 0; GpioPin < SetupData.SkyCam_ControlLogic0_GpioPinsEnabled; GpioPin ++) {
      GpioPad = GpioGetGpioPadFromGroupIndexAndPadNumber (
                                   (UINT32)SetupData.SkyCam_ControlLogic0_GpioGroupNumber[GpioPin],
                                   (UINT32)SetupData.SkyCam_ControlLogic0_GpioGroupPadNumber[GpioPin]);
      DEBUG((DEBUG_INFO, "SkyCamGpioInit GpioPad 0x%x\n", GpioPad));
      GpioSetPadConfig (GpioPad, &GpioConfig);
    }
  }
  if ((SetupData.SkyCam_ControlLogic1) && (SetupData.SkyCam_ControlLogic1_Type == (UINT8)1) && (SetupData.SkyCam_ControlLogic1_GpioPinsEnabled)) {
    DEBUG((DEBUG_INFO, "SkyCamGpioInit ControlLogic1\n"));
    for (GpioPin = 0; GpioPin < SetupData.SkyCam_ControlLogic1_GpioPinsEnabled; GpioPin ++) {
      GpioPad = GpioGetGpioPadFromGroupIndexAndPadNumber (
                                   (UINT32)SetupData.SkyCam_ControlLogic1_GpioGroupNumber[GpioPin],
                                   (UINT32)SetupData.SkyCam_ControlLogic1_GpioGroupPadNumber[GpioPin]);
      DEBUG((DEBUG_INFO, "SkyCamGpioInit GpioPad 0x%x\n", GpioPad));
      GpioSetPadConfig (GpioPad, &GpioConfig);
    }
  }
  if ((SetupData.SkyCam_ControlLogic2) && (SetupData.SkyCam_ControlLogic2_Type == (UINT8)1) && (SetupData.SkyCam_ControlLogic2_GpioPinsEnabled)) {
    DEBUG((DEBUG_INFO, "SkyCamGpioInit ControlLogic2\n"));
    for (GpioPin = 0; GpioPin < SetupData.SkyCam_ControlLogic2_GpioPinsEnabled; GpioPin ++) {
      GpioPad = GpioGetGpioPadFromGroupIndexAndPadNumber (
                                   (UINT32)SetupData.SkyCam_ControlLogic2_GpioGroupNumber[GpioPin],
                                   (UINT32)SetupData.SkyCam_ControlLogic2_GpioGroupPadNumber[GpioPin]);
      DEBUG((DEBUG_INFO, "SkyCamGpioInit GpioPad 0x%x\n", GpioPad));
      GpioSetPadConfig (GpioPad, &GpioConfig);
    }
  }
  if ((SetupData.SkyCam_ControlLogic3) && (SetupData.SkyCam_ControlLogic3_Type == (UINT8)1) && (SetupData.SkyCam_ControlLogic3_GpioPinsEnabled)) {
    DEBUG((DEBUG_INFO, "SkyCamGpioInit ControlLogic3\n"));
    for (GpioPin = 0; GpioPin < SetupData.SkyCam_ControlLogic3_GpioPinsEnabled; GpioPin ++) {
      GpioPad = GpioGetGpioPadFromGroupIndexAndPadNumber (
                                   (UINT32)SetupData.SkyCam_ControlLogic3_GpioGroupNumber[GpioPin],
                                   (UINT32)SetupData.SkyCam_ControlLogic3_GpioGroupPadNumber[GpioPin]);
      DEBUG((DEBUG_INFO, "SkyCamGpioInit GpioPad 0x%x\n", GpioPad));
      GpioSetPadConfig (GpioPad, &GpioConfig);
    }
  }
  return;
}
#endif // MINTREE_FLAG

/**
  Configure GPIO Before Memory is not ready.

  @param[in]  PlatformInfo
**/
VOID
GpioInitPreMem (
  IN BOARD_CONFIG_BLOCK_PEI_PREMEM   *PlatformBoardConfig
  )
{
#ifndef MINTREE_FLAG
  GPIO_CONFIG                     BbrstConfig;
  UINT32                          WwanBbrstGpio;
  PCD64_BLOB                      PcdData;

  WwanBbrstGpio = PcdGet32 (PcdWwanBbrstGpio);
  PcdData.Blob = PcdGet64 (PcdWwanFullCardPowerOffGpio);

  if (WwanBbrstGpio) {
    GpioGetPadConfig (WwanBbrstGpio, &BbrstConfig);
    if (BbrstConfig.Direction == GpioDirOut && BbrstConfig.OutputState == GpioOutHigh) {
      //
      // Assert FULL_CARD_POWER_OFF#
      //
      if (PcdData.BoardGpioConfig.Type == BoardGpioTypeExpander) {
        GpioExpSetOutput (PcdData.BoardGpioConfig.u.Expander.Section, PcdData.BoardGpioConfig.u.Expander.Pin, 0);
      }
    }
  }
#endif

  if (PlatformBoardConfig->GpioTablePreMem != NULL && PlatformBoardConfig->GpioTablePreMemSize != 0) {
    ConfigureGpio (PlatformBoardConfig->GpioTablePreMem, PlatformBoardConfig->GpioTablePreMemSize);
  }
}
#ifndef MINTREE_FLAG
#define HaloMd2Variable         L"HaloMd2Variable"
extern EFI_GUID gHaloMd2VariableGuid;
#endif

/**
  Configure GPIO

  @param[in]  PlatformInfo
**/
VOID
GpioInit (
  IN BOARD_CONFIG_BLOCK_PEI_POSTMEM   *PlatformBoardConfig
  )
{
#ifndef MINTREE_FLAG
  BOOLEAN                         MD2Exist;
  UINTN                           VarSize;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  EFI_STATUS                      Status;
  EFI_BOOT_MODE                   BootMode;
  SETUP_DATA                      SystemConfiguration;
  PCH_SETUP                       PchSetup;
  GPIO_INIT_CONFIG                *UcmcGpioTable;
  UINT16                          UcmcGpioTableSize;


     Status = PeiServicesLocatePpi(
      &gEfiPeiReadOnlyVariable2PpiGuid,
      0,
      NULL,
      (VOID **)&VariableServices
      );
  ASSERT_EFI_ERROR(Status);

  VarSize = sizeof (SETUP_DATA);
  Status = VariableServices->GetVariable(
      VariableServices,
      L"Setup",
      &gSetupVariableGuid,
      NULL,
      &VarSize,
      &SystemConfiguration
      );

#endif // MINTREE_FLAG


  ConfigureGpio (PlatformBoardConfig->GpioTable1, PlatformBoardConfig->Table1Size);

  if (PlatformBoardConfig->GpioTable2 != NULL) {
    ConfigureGpio (PlatformBoardConfig->GpioTable2, PlatformBoardConfig->Table2Size);



  }

  TouchpanelGpioInit(PlatformBoardConfig);
#ifndef MINTREE_FLAG
  ScsSdCardGpioInit(PlatformBoardConfig);

  if (PlatformBoardConfig->Enable.PhabletPocGpioSpecificInit) {
    MicroSecondDelay (10000); //Core
    GpioSetOutputValue (GPIO_SKL_LP_GPP_E11, 1); //MIPI_CORE_PWREN2
    MicroSecondDelay (10000); //IO
    DEBUG((DEBUG_INFO, "GraphicsInit: Set MIPI GPIOs for display.\n"));
    GpioSetOutputValue (GPIO_SKL_LP_GPP_E8, 1); //MIPI_IO_PWREN1
    MicroSecondDelay (10000); //RESET
    GpioSetOutputValue (GPIO_SKL_LP_GPP_E5, 1); //MIPI_RESET_N

    //100 ms delay and then put EMMC out of reset
    MicroSecondDelay (100000);
    GpioSetOutputValue (GPIO_SKL_LP_GPP_G1, 1); //EMMC_RST_N   -- check in SDS

    //Wifi WLAN
    MicroSecondDelay (10000);
    GpioSetOutputValue (GPIO_SKL_LP_GPP_D10, 1); //LNP_WLAN_EN
    MicroSecondDelay (10000);
    GpioSetOutputValue (GPIO_SKL_LP_GPP_B3, 1); //WLAN_PERST_N

    //Audio Codec
    MicroSecondDelay (10000);
    GpioSetOutputValue (GPIO_SKL_LP_GPP_C3, 1);//CODEC_RST_N
    //MicroSecondDelay (10000);
    //GpioSetOutputValue (GPIO_SKL_LP_GPP_C4, 1);//DMIC_TO_CODEC_EN --- this needs to be made HIGH for DMIC TO CODEC path , default is DMIC connected to PCH

    //BT_FM_ENABLE
    MicroSecondDelay (10000);
    GpioSetOutputValue (GPIO_SKL_LP_GPP_D21, 1);//BT_FM_ENABLE

    //CAM PMIC RESET
    MicroSecondDelay (10000);
    GpioSetOutputValue (GPIO_SKL_LP_GPP_E3, 0);//CAM PMIC RESET --- this will enable PMIC , logic is inverse

    //GNSS_RESET_N
    MicroSecondDelay (10000);
    GpioSetOutputValue (GPIO_SKL_LP_GPP_G0, 1);//GNSS_RESET_N
  }

  if (PlatformBoardConfig->Enable.PantherMtnGpioSpecificInit) {
    //
    // 100 ms delay, then re-enable SSD_PWREN
    //
    MicroSecondDelay (100000);
    GpioSetOutputValue (GPIO_SKL_LP_GPP_G3, 1);
  }

  if (PlatformBoardConfig->Enable.GrizzlyMtnGpioSpecificInit || PlatformBoardConfig->Enable.CoyoteMtnGpioSpecificInit) {
    //Send EC command to bring Ti ACE out of reset
    SendEcCommand (COYOTE_MTN_TYPEC_HRESET);

    DEBUG((DEBUG_INFO, "Set GMT MIPI GPIOs for display.\n"));
    ///
    /// Pull the RESET line low
    ///
    GpioSetOutputValue(GM_MIPI_RESET_N, 0); //MIPI_RESET_N
    MicroSecondDelay(10000);
    ///
    /// Enable CORE power supply
    ///
    GpioSetOutputValue(GM_MIPI_CORE_PWREN2, 1); //MIPI_CORE_PWREN2
    MicroSecondDelay(10000);
    ///
    /// Enable IO power supply
    ///
    GpioSetOutputValue(GM_MIPI_IO_PWREN1, 1); //MIPI_IO_PWREN1
    MicroSecondDelay(10000);
    ///
    /// Enable DISP power supply
    ///
    GpioSetOutputValue(GM_DISP_PWR_EN, 1); //DISP_PWR_EN
    MicroSecondDelay(10000);
    ///
    /// Reset the bridge chip and release the RESET line high
    ///
    GpioSetOutputValue(GM_MIPI_RESET_N, 1); //MIPI_RESET_N
    MicroSecondDelay(100000);
    DEBUG((DEBUG_INFO, "GMT MIPI GPIOs for display done.\n"));

    ///
    /// Reset the SSD
    ///
    MicroSecondDelay(250000);
    GpioSetOutputValue(GM_SSD_RESET_N, 1);
    MicroSecondDelay(100000);
    GpioSetOutputValue(GM_SSD_RESET_N, 0);
    MicroSecondDelay(100000);
    GpioSetOutputValue(GM_SSD_RESET_N, 1);

    GpioSetOutputValue (GPIO_SKL_LP_GPP_G4, 0);  // Set CP_ON_OFF_N = 0, MODEM_PWR_ON
    MicroSecondDelay(100000);
    GpioSetOutputValue (GPIO_SKL_LP_GPP_G3, 1);  // Set CP_RESET_BB_N = 1, MODEM_RESET_BB_PCH_N/EXTRST_N
    MicroSecondDelay(100000);
    GpioSetOutputValue (GPIO_SKL_LP_GPP_D22, 1); // Set CP_PCIE _PERST_N = 1
    MicroSecondDelay(100000);
    GpioSetOutputValue (GPIO_SKL_LP_GPP_G4, 1);  //Set CP_ON_OFF_N = 1
  }

  if (PlatformBoardConfig->Enable.M2GpioSpecificInit) {
    Status = PeiServicesLocatePpi (
               &gEfiPeiReadOnlyVariable2PpiGuid,
               0,
               NULL,
               (VOID **) &VariableServices
               );
    ASSERT_EFI_ERROR (Status);

    VarSize = sizeof (BOOLEAN);
    Status = VariableServices->GetVariable (
                                 VariableServices,
                                 HaloMd2Variable,
                                 &gHaloMd2VariableGuid,
                                 NULL,
                                 &VarSize,
                                 &MD2Exist
                                 );
    if (EFI_ERROR(Status)) {
      MD2Exist = TRUE;
    }

    VarSize = sizeof (PCH_SETUP);
    Status = VariableServices->GetVariable (
                                 VariableServices,
                                 L"PchSetup",
                                 &gPchSetupVariableGuid,
                                 NULL,
                                 &VarSize,
                                 &PchSetup
                                 );
    if (EFI_ERROR(Status)) {
      MD2Exist = TRUE;
    }

    //Check for RAID Mode
    if (PchSetup.SataInterfaceMode == SATA_MODE_RAID) {
      MD2Exist = TRUE;
    }

    Status = PeiServicesGetBootMode (&BootMode);
    ASSERT_EFI_ERROR (Status);
    if (!((BootMode == BOOT_ON_S3_RESUME) || GetPchPmStatus (WarmBoot)) || MD2Exist) {
      //
      // Bios workaround for M.2 SSD detection on RVP11 and RVP16 RVPs.
      // GPIO GPP_G9 and GPP_G8 (Ideally Native Pins)are used to control Power for the M.2 SSD1 and M.2 SSD2 PCIe Ports.
      // To ensure device detection we are de-asserting the reset and applying delay for proper device enumeration
      //
        GpioSetOutputValue (GPIO_SKL_H_GPP_F17, 0); //De-Asserting M.2 SSD1 RESET
        GpioSetOutputValue (GPIO_SKL_H_GPP_F18, 0); //De-Asserting M.2 SSD2 RESET
        GpioSetOutputValue (GPIO_SKL_H_GPP_D10, 0); //De-Asserting M.2 SSD2 RESET
        MicroSecondDelay (100000);//100ms Delay to ensure proper device enumeration after the reset is de-asserted
    }
  }

  if (PlatformBoardConfig->Enable.SkyCamGpio) {
    SkyCamGpioInit(PlatformBoardConfig);
  }

  // If UCMC setup option is enabled, enable the GPP_B_0 and GPP_B_1 pad to generate interrupts
  VarSize = sizeof (PCH_SETUP);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"PchSetup",
                               &gPchSetupVariableGuid,
                               NULL,
                               &VarSize,
                               &PchSetup
                               );

  if (!EFI_ERROR (Status)) {
    if (PchSetup.PchUcsiUcmcDevice) {
      UcmcGpioTable = (GPIO_INIT_CONFIG *) (UINTN) PcdGet32 (PcdBoardUcmcGpioTable);
      UcmcGpioTableSize = PcdGet16 (PcdBoardUcmcGpioTableSize);
      ConfigureGpio (UcmcGpioTable, UcmcGpioTableSize);
    }
  }
#endif // MINTREE_FLAG

  return;
}

/**
  Configure Super IO

  @param[in]  PlatformInfo
**/
VOID
SioInit (
  IN PLATFORM_INFO             *PlatformInfo,
  IN BOARD_CONFIG_BLOCK_PEI_PREMEM   *PlatformBoardConfig
  )
{
  //
  // Program and Enable Default Super IO Configuration Port Addresses and range
  //
  PchLpcGenIoRangeSet (PcdGet16 (PcdLpcSioConfigDefaultPort) & (~0xF), 0x10);

#ifndef MINTREE_FLAG
  //
  // Enable LPC decode for KCS and mailbox SIO for iBMC communication
  //
  if (PlatformInfo->PlatformFlavor == FlavorUpServer) {
    PchLpcGenIoRangeSet (BMC_KCS_BASE_ADDRESS, 0x10);
    PchLpcGenIoRangeSet (PILOTIII_MAILBOX_BASE_ADDRESS, 0x10);
  } else {
#endif
  //
  // 128 Byte Boundary and SIO Runtime Register Range is 0x0 to 0xF;
  //
    PchLpcGenIoRangeSet (SIO_RUNTIME_REG_BASE_ADDRESS  & (~0x7F), 0x10);
#ifndef MINTREE_FLAG
  }
#endif
#ifndef MINTREE_FLAG
  //
  // We should not depend on SerialPortLib to initialize KBC for legacy USB
  // So initialize KBC for legacy USB driver explicitly here.
  // After we find how to enable mobile KBC, we will add enabling code for mobile then.
  //
  if ((PlatformInfo->PlatformFlavor == FlavorDesktop) ||
      (PlatformInfo->PlatformFlavor == FlavorWorkstation)) {

    //
    // Enable KBC for National PC8374 SIO
    //
    if (PlatformBoardConfig->Enable.Pc8374SioKbc == 1) {
      IoWrite8 (PcdGet16 (PcdDesktopLpcSioIndexDefaultPort), 0x07);
      IoWrite8 (PcdGet16 (PcdDesktopLpcSioDataDefaultPort), 0x06);
      IoWrite8 (PcdGet16 (PcdDesktopLpcSioIndexDefaultPort), 0x30);
      IoWrite8 (PcdGet16 (PcdDesktopLpcSioDataDefaultPort), 0x01);
    }
  }
#endif

  return;
}

#ifndef MINTREE_FLAG
/**
  Configure Power for Optical Disk Drive

  @param[in]  PlatformInfo
**/
VOID
OddPowerInit (
  IN BOARD_CONFIG_BLOCK_PEI_PREMEM   *PlatformBoardConfig
  )
{
  UINT32 InputVal;

  //
  // ODD power is delivered only if ODD is detected
  // In case of ODD hot plug, power will be turn on in the ACPI PME handler
  //
  if (PlatformBoardConfig->Enable.OddPowerInit == 1) {
    GpioGetInputValue(GPIO_SKL_H_GPP_E1, &InputVal);
    if (InputVal == V_PCH_GPIO_RX_STATE_LOW) {
      GpioSetOutputValue (GPIO_SKL_H_GPP_G1, V_PCH_GPIO_TX_STATE_HIGH);
    }
  }
}

// AMI_OVERRIDE_START - AMI not use.
#if 0
/**
  SSIC Modem reboot flow

  @retval  EFI_SUCCESS   Operation success.
**/
VOID
SsicModemReboot (
  VOID
  )
{
  EFI_STATUS                      Status;
  SETUP_DATA                      SystemConfiguration;
  UINTN                           VarSize;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;

  DEBUG ((DEBUG_INFO, "SsicModemReboot() Start\n"));
  //
  // Locate Setup variables
  //
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &VariableServices
             );
  ASSERT_EFI_ERROR (Status);

  VarSize = sizeof (SETUP_DATA);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"Setup",
                               &gSetupVariableGuid,
                               NULL,
                               &VarSize,
                               &SystemConfiguration
                               );
  if (EFI_ERROR(Status)) {
    return;
  }
  if (SystemConfiguration.Ssic7160and7260InitSetup == TRUE) {
    DEBUG ((DEBUG_INFO, "SsicModemReboot() reset flow\n"));
    //
    // FULL_CARD_POWER_OFF_N low
    //
    GpioExpSetOutput(IO_EXPANDER_1, IO_EXPANDER_GPIO_0, IO_EXPANDER_GPO_LEVEL_LOW);
    MicroSecondDelay (8000);
    //
    // RESET_N low
    // GPP A12 pin needs to be configured as Powergood reset.
    //
    GpioSetOutputValue (GPIO_SKL_LP_GPP_A12, 0);
    MicroSecondDelay (20000); //< 20ms for reset
    //
    // RESET_N high
    //
    GpioSetOutputValue (GPIO_SKL_LP_GPP_A12, 1);
    MicroSecondDelay (490000); //< 490 ms delay
    //
    // FULL_CARD_POWER_OFF_N high
    //
    GpioExpSetOutput(IO_EXPANDER_1, IO_EXPANDER_GPIO_0, IO_EXPANDER_GPO_LEVEL_HIGH);
  }
  return;
}
#endif
// AMI_OVERRIDE_END - AMI not use.

/**
  PCIe Modem flow

  @retval  EFI_SUCCESS   Operation success.
**/
VOID
PcieModemConfig (
  IN SETUP_DATA    SystemConfiguration,
  IN PLATFORM_INFO *PlatformInfo
  )
{
  DEBUG ((DEBUG_INFO, "PcieModemConfig() Start\n"));

  //
  // Reset Modem
  //
  if (PlatformInfo->BoardIDOrgvalue == BoardIdKabyLakeYLpddr3Rvp3 ||
      PlatformInfo->BoardIDOrgvalue == BoardIdAmberLakeY42Lpddr3Rvp3 ||
      PlatformInfo->BoardIDOrgvalue == BoardIdAmberLakeLpddr3Rvp3) {
    GpioSetOutputValue (GPIO_SKL_LP_GPP_D9, V_PCH_GPIO_TX_STATE_HIGH);  // Modem reset is set to high
  } else if ((PlatformInfo->BoardIDOrgvalue == BoardIdAmberLakeY42Lpddr3Cff)||
             (PlatformInfo->BoardIDOrgvalue == BoardIdAmberLakeY42Lpddr3CffCc)) {
    GpioSetOutputValue (GPIO_SKL_LP_GPP_A16, V_PCH_GPIO_TX_STATE_HIGH);  // Modem reset is set to high
  } else {
    GpioSetOutputValue (GPIO_SKL_LP_GPP_A12, V_PCH_GPIO_TX_STATE_HIGH); // Modem reset is set to high
  }
  MicroSecondDelay (SystemConfiguration.WwanModemResetDelay * 1000);

  //
  // Deassert PERST
  //
  if (PlatformInfo->BoardIDOrgvalue == BoardIdAmberLakeY42Lpddr3Rvp3) {
      GpioSetOutputValue(GPIO_SKL_LP_GPP_C13, V_PCH_GPIO_TX_STATE_LOW);  // Modem reset is set to low
  } else if ((PlatformInfo->BoardIDOrgvalue == BoardIdAmberLakeY42Lpddr3Cff ) ||
             (PlatformInfo->BoardIDOrgvalue == BoardIdAmberLakeY42Lpddr3CffCc )) {
    GpioSetOutputValue(GPIO_SKL_LP_GPP_C6, V_PCH_GPIO_TX_STATE_HIGH);  // Modem reset is set to high
  } else if (PlatformInfo->BoardIDOrgvalue == BoardIdKabyLakeYLpddr3Rvp3 ||
             PlatformInfo->BoardIDOrgvalue == BoardIdAmberLakeLpddr3Rvp3) {
    GpioExpSetOutput (IO_EXPANDER_0, IO_EXPANDER_GPIO_8, IO_EXPANDER_GPO_LEVEL_LOW);  // M.2_WWAN_RST_CNTRL_R (PERST)
  } else {
    GpioExpSetOutput (IO_EXPANDER_0, IO_EXPANDER_GPIO_22, IO_EXPANDER_GPO_LEVEL_LOW); // M.2_WWAN_RST_CNTRL_R (PERST)
  }
  MicroSecondDelay (SystemConfiguration.WwanPerstDelay * 1000);
}
#endif

/**
  Configure GPIO and SIO before memory ready

  @retval  EFI_SUCCESS   Operation success.
**/
EFI_STATUS
BoardInitPreMem (
  VOID
  )
{
  EFI_STATUS                      Status;
  PLATFORM_INFO                   *PlatformInfo;
  BOARD_CONFIG_BLOCK_PEI_PREMEM   *PlatformBoardConfig;

  //
  // Obtain Platform Info from HOB.
  //
  Status = GetPlatformInfo (&PlatformInfo);
  ASSERT_EFI_ERROR (Status);
  if (PlatformInfo == NULL) {
    return Status;
  }

  Status = GetBoardConfigBlock (&gPlatformBoardConfigPreMemGuid, (VOID **) &PlatformBoardConfig);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  GpioInitPreMem (PlatformBoardConfig);
  GpioGroupTierInit (PlatformBoardConfig); // AdvancedFeaturesContent
  ExpanderGpioInitPreMem (PlatformInfo); // AdvancedFeaturesContent
  SioInit (PlatformInfo, PlatformBoardConfig);
  OddPowerInit (PlatformBoardConfig); // AdvancedFeaturesContent

  return EFI_SUCCESS;
}

/**
  Configure GPIO and SIO

  @retval  EFI_SUCCESS   Operation success.
**/
EFI_STATUS
BoardInit (
  VOID
  )
{
  EFI_STATUS                       Status;
  BOARD_CONFIG_BLOCK_PEI_POSTMEM  *PlatformBoardConfig;
  PEI_BOARD_SPECIFIC_INIT_POST_MEM PeiBoardSpecificInitPostMem; // AdvancedFeaturesContent
  PLATFORM_INFO                   *PlatformInfo;
#ifndef MINTREE_FLAG
  SETUP_DATA                       SystemConfiguration;
  UINTN                            VarSize;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
  BOOLEAN                          IsPcieWwanEnabled;
#endif

  Status = GetPlatformInfo(&PlatformInfo);
  ASSERT_EFI_ERROR(Status);
  if (PlatformInfo == NULL) {
    return Status;
  }

  Status = GetBoardConfigBlock (&gPlatformBoardConfigPostMemGuid, (VOID **) &PlatformBoardConfig);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

#ifndef MINTREE_FLAG
  ZeroMem (&SystemConfiguration, sizeof (SystemConfiguration));
  //
  // Locate Setup variables
  //
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &VariableServices
             );
  ASSERT_EFI_ERROR (Status);

  VarSize = sizeof (SETUP_DATA);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"Setup",
                               &gSetupVariableGuid,
                               NULL,
                               &VarSize,
                               &SystemConfiguration
                               );
  IsPcieWwanEnabled = FALSE;
  if (!EFI_ERROR (Status)) {
    if (PcdGetBool (PcdPcieWwanEnable)) {
      //
      // Configure PCIe WWAN if WWAN is enabled and WWAN interface is not SSIC only.
      //
      if (SystemConfiguration.WWANEnable == 1 && PcdGet8 (PcdWwanInterface) != 2) {
        IsPcieWwanEnabled = TRUE;
        PlatformBoardConfig->GpioExpanderTable2 = PlatformBoardConfig->GpioExpanderTablePcieWwan;
        PlatformBoardConfig->GpioExpanderTable2Size = PlatformBoardConfig->GpioExpanderTablePcieWwanSize;
      } else {
        PlatformBoardConfig->GpioExpanderTable2 = PlatformBoardConfig->GpioExpanderTablePcieSlot1;
        PlatformBoardConfig->GpioExpanderTable2Size = PlatformBoardConfig->GpioExpanderTablePcieSlot1Size;
      }
    }

    //
    // If TBT RTD3 is enabled on KBL-R, configure IO Expander GPIO16 to high.
    //
    if ((SystemConfiguration.Rtd3Tbt) &&
        ((PlatformInfo->BoardIDOrgvalue == BoardIdKabylakeRDdr4) ||
         (PlatformInfo->BoardIDOrgvalue == BoardIdKabylakeRLpddr3) ||
         (PlatformInfo->BoardIDOrgvalue == BoardIdAmberLakeY42Lpddr3Rvp3) ||
         (PlatformInfo->BoardIDOrgvalue == BoardIdAmberLakeLpddr3Rvp3) ||
         (PlatformInfo->BoardIDOrgvalue == BoardIdKabyLakeYLpddr3Rvp3))){
      PlatformBoardConfig->GpioExpanderTable2 = PlatformBoardConfig->GpioExpanderTablePcieSlot1;
      PlatformBoardConfig->GpioExpanderTable2Size = PlatformBoardConfig->GpioExpanderTablePcieSlot1Size;
    }
  }
#endif

  GpioInit (PlatformBoardConfig);
  ExpanderGpioInit (PlatformBoardConfig);

#ifndef MINTREE_FLAG
  PeiBoardSpecificInitPostMem = (PEI_BOARD_SPECIFIC_INIT_POST_MEM) (UINTN) PcdGet32 (PcdFuncPeiBoardSpecificInitPostMem);
  ASSERT (PeiBoardSpecificInitPostMem != NULL);

  if (PeiBoardSpecificInitPostMem) {
    Status = PeiBoardSpecificInitPostMem ();
  }

  if (IsPcieWwanEnabled) {
    if (PlatformBoardConfig->ModemReset != NULL) {
      GpioConfigurePads (1, PlatformBoardConfig->ModemReset);
    }
    if (PlatformBoardConfig->GpioTablePcieWwan != NULL) {
      GpioConfigurePads (PlatformBoardConfig->GpioTablePcieWwanSize, PlatformBoardConfig->GpioTablePcieWwan);
    }
    // If this is KBLR and RTD3 for TBT is NOT enabled, program B18 as output. This will prevent an unexpected S3 bounce back for x4 Slot rework. 
    if (((PlatformBoardConfig->GpioTablePcieSlot1 != NULL) &&
         !SystemConfiguration.Rtd3Tbt ) &&
        ((PlatformInfo->BoardIDOrgvalue == BoardIdKabylakeRDdr4) || (PlatformInfo->BoardIDOrgvalue == BoardIdKabylakeRLpddr3))) 
    {
      GpioConfigurePads (PlatformBoardConfig->GpioTablePcieSlot1Size, PlatformBoardConfig->GpioTablePcieSlot1);
    }
    PcieModemConfig (SystemConfiguration, PlatformInfo);
  } else {
    //
    // PCIe WWAN is enabled through PCIe slot1 rework on KBL-Y RVP3. If WWAN is disabled or WWAN interface is SSIC only,
    // some GPIOs need to be configured for PCIe slot1.
    // In case of KBL-R, if RTD3 is disabled, program B18 as output. This will prevent an unexpected S3 bounce back for x4 Slot rework.
    //
    if ((PlatformInfo->BoardIDOrgvalue == BoardIdKabylakeRDdr4) || (PlatformInfo->BoardIDOrgvalue == BoardIdKabylakeRLpddr3) ) {
      if ((PlatformBoardConfig->GpioTablePcieSlot1 != NULL) && !SystemConfiguration.Rtd3Tbt ) {
        GpioConfigurePads (PlatformBoardConfig->GpioTablePcieSlot1Size, PlatformBoardConfig->GpioTablePcieSlot1);
      }
    } else {
      if (PlatformBoardConfig->GpioTablePcieSlot1 != NULL) {
        GpioConfigurePads (PlatformBoardConfig->GpioTablePcieSlot1Size, PlatformBoardConfig->GpioTablePcieSlot1);
      }
    }
  }
// AMI_OVERRIDE_START - Use Flag to separte the code which use EC command.
#ifdef AMI_CRB_EC_SUPPORT_FLAG

  if (PlatformInfo->PlatformType == TypeUltUlx) {
    Status = VoltageMarginingRoutine ();
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) {
      return Status;
    }
  }
#endif // End of AMI_CRB_EC_SUPPORT_FLAG
// AMI_OVERRIDE_END - Use Flag to separte the code which use EC command.
#endif

  return Status;
}
#ifndef MINTREE_FLAG
// AMI_OVERRIDE_START - Use Flag to separte the code which use EC command.
#ifdef AMI_CRB_EC_SUPPORT_FLAG
/**
  Voltage Margining Routine

  @retval  EFI_SUCCESS   Operation success
**/
EFI_STATUS
VoltageMarginingRoutine(
  VOID
  )
{
  SETUP_DATA                       SystemConfiguration;
  UINTN                            VarSize;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *VariableServices;
  PLATFORM_INFO                    *PlatformInfo;
  EFI_STATUS                       Status;
  GPIO_CONFIG                      PadConfig;
  BOOLEAN                          PremiumPmicPresent;

  //
  // Locate Setup variables
  //
  Status = PeiServicesLocatePpi(
    &gEfiPeiReadOnlyVariable2PpiGuid,
    0,
    NULL,
    (VOID **)&VariableServices
    );
  ASSERT_EFI_ERROR(Status);

  VarSize = sizeof (SETUP_DATA);
  Status = VariableServices->GetVariable(
    VariableServices,
    L"Setup",
    &gSetupVariableGuid,
    NULL,
    &VarSize,
    &SystemConfiguration
    );
  Status = GetPlatformInfo(&PlatformInfo);
  ASSERT_EFI_ERROR(Status);
  if (PlatformInfo == NULL) {
    return Status;
  }

  //
  // Check if Premium PMIC is available
  //
  PremiumPmicPresent = FALSE;
  if (PlatformInfo->EcPresent == TRUE) {
    PmicVoltageSelect(&SystemConfiguration);
    if (IsPremiumPmicPresent()) {
      PremiumPmicDisableSlpS0Voltage();
      PremiumPmicPresent = TRUE;
    }
  }

  //
  // Voltage Margining with Premium PMIC or discrete VR controlled by a GPIO
  //
  PlatformInfo->RunTimeVmControl = 0;
  if (PremiumPmicPresent) {
    PlatformInfo->RunTimeVmControl = 1;
  } else if (PcdGet32 (PcdRunTimeVmVrGpio) != 0) {
    PlatformInfo->RunTimeVmControl = 2;
    if (SystemConfiguration.SlpS0VmSupport) {
      ZeroMem (&PadConfig, sizeof (PadConfig));
      PadConfig.PadMode        = GpioPadModeGpio;
      PadConfig.HostSoftPadOwn = GpioHostOwnAcpi;
      PadConfig.Direction      = GpioDirOut;
      PadConfig.OutputState    = GpioOutHigh;
      Status = GpioSetPadConfig (PcdGet32 (PcdRunTimeVmVrGpio), &PadConfig);
    }
  } else if (PcdGetBool (PcdDvrmVmCardSupport) != 0) {
    //
    // Look for and Initialize Dvrm Card Voltage Margin if and only if NO PMIC
    // and Runtime Vm Vr GPIO support found. 1st Try to find if PMIC is available or NOT.
    // Then Try to find VR GPIO Support. Then Enable DVRM if this supported by Board.
    //
    PlatformInfo->RunTimeVmControl = 3; // Initialize with Default value
    if (SystemConfiguration.DvrmVmIoExpCapabilitySupport == 1) {
      //
      // If DvrmVmIoExpCapabilitySupported BIOS Setup is, 0: RuntimeVmControl =3, 1: RuntimeVmControl =4;
      //
      PlatformInfo->RunTimeVmControl = 4;
    }
  }

  return EFI_SUCCESS;
}
#endif // End of AMI_CRB_EC_SUPPORT_FLAG
// AMI_OVERRIDE_END - Use Flag to separte the code which use EC command.
#endif

/**
  Detect recovery mode

  @retval  EFI_SUCCESS       System in Recovery Mode
  @retval  EFI_UNSUPPORTED   System doesn't support Recovery Mode
  @retval  EFI_NOT_FOUND     System is not in Recovery Mode
**/
EFI_STATUS
IsRecoveryMode (
  VOID
  )
{
#ifndef MINTREE_FLAG
  BOOLEAN               RecJumperFlag;
  UINT32                BitMask;
  EFI_STATUS            Status;
  PLATFORM_INFO         *PlatformInfo;
  UINT32                RecoveryModeGpio;
  BOARD_CONFIG_BLOCK_PEI_PREMEM   *PlatformBoardConfig;

  //
  // Obtain Platform Info from HOB.
  //
  RecoveryModeGpio = 0;
  Status = GetPlatformInfo (&PlatformInfo);
  ASSERT_EFI_ERROR (Status);
  if (PlatformInfo == NULL) {
    return Status;
  }

  Status = GetBoardConfigBlock (&gPlatformBoardConfigPreMemGuid, (VOID **) &PlatformBoardConfig);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  if (PlatformInfo->PlatformFlavor == FlavorUpServer) {
      return EFI_NOT_FOUND;
  } else if (PlatformBoardConfig->Enable.RecoveryModeGpio) {
    if (PlatformBoardConfig->RecoveryModeGpio.Type == BoardGpioTypePch) {
      Status = GpioGetInputValue (PlatformBoardConfig->RecoveryModeGpio.u.Pin, &RecoveryModeGpio);
      ASSERT_EFI_ERROR (Status);
      if (EFI_ERROR(Status)) {
        return Status;
      }
    } else if (PlatformBoardConfig->RecoveryModeGpio.Type == BoardGpioTypeExpander) {
      RecoveryModeGpio = GpioExpGetInput (PlatformBoardConfig->RecoveryModeGpio.u.Expander.Section, PlatformBoardConfig->RecoveryModeGpio.u.Expander.Pin);
    } else {
      DEBUG ((DEBUG_ERROR, "Board Recovery Mode GPIO type is unidentified. Type = %x\n", PlatformBoardConfig->RecoveryModeGpio.Type));
      ASSERT (FALSE);
    }
    DEBUG ((DEBUG_INFO, "Expander value returned  %x\n",RecoveryModeGpio));
  } else {
    //
    // Read the status of the GPIO recovery jumper from the GPIO registers @ to do: GPIO will be assigned by board
    //
    //GpioGetInputValue (RECOVERY_MODE_GPIO_PIN, &RecoveryModeGpio); //@todo, uncomment when RECOVERY_MODE_GPIO_PIN has been assigned correct GpioPad
    RecoveryModeGpio = 0x1; //@todo, remove when RECOVERY_MODE_GPIO_PIN has been assigned correct GpioPad
  }
  //
  // Select/modify the GPIO initialization data based on the Board ID.
  //
  if (PlatformInfo->BoardType == BoardTypeSv || PlatformInfo->BoardType == BoardTypeRvpErb) {
    //
    // There is no Recovery jumper
    //
    return EFI_NOT_FOUND;
  } else {
    BitMask = BIT0;
  }

  RecJumperFlag = ((BOOLEAN) (!(RecoveryModeGpio & BitMask)));
  if (RecJumperFlag) {
    DEBUG ((DEBUG_INFO, "RecJumperFlag SET\n"));
    return EFI_SUCCESS;
  } else {
    DEBUG ((DEBUG_INFO, "RecJumperFlag NOT SET\n"));
    return EFI_NOT_FOUND;
  }
#else
  return EFI_NOT_FOUND;
#endif
}

/**
  Do platform specific programming. For example, EC init, Chipset programming

  @retval  EFI_SUCCESS       The function completed successfully.
**/
EFI_STATUS
PlatformSpecificInitPreMem (
  VOID
  )
{
  EFI_STATUS                      Status;
  EFI_BOOT_MODE                   BootMode;
#ifndef MINTREE_FLAG
// AMI_OVERRIDE_START - Fixed build error when disable RTD3_SUPPORT.
#if defined (RTD3_SUPPORT) && RTD3_SUPPORT == 1 
  UINTN                           Index;
#endif // End of RTD3_SUPPORT
// AMI_OVERRIDE_END - Fixed build error when disable RTD3_SUPPORT.  
  SA_SETUP                        SaSetup;
  CPU_SETUP                       CpuSetup;
  PCH_SETUP                       PchSetup;
// AMI_OVERRIDE_START - Use Flag to separte the code which use EC command.
#ifdef AMI_CRB_EC_SUPPORT_FLAG   
  UINT32                          Data32;
  UINT8                           DebugStatus;
#endif // End of AMI_CRB_EC_SUPPORT_FLAG
// AMI_OVERRIDE_END - Use Flag to separte the code which use EC command.  
  SETUP_DATA                      SystemConfiguration;
  UINTN                           VarSize;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;
#endif // MINTREE_FLAG
  SI_PREMEM_POLICY_PPI            *SiPreMemPolicyPpi;
  SA_MISC_PEI_PREMEM_CONFIG       *MiscPeiPreMemConfig;
  PLATFORM_INFO                   *PlatformInfo;
  BOARD_CONFIG_BLOCK_PEI_PREMEM   *PlatformBoardConfig;

  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

  //
  // Obtain Platform Info from HOB.
  //
  Status = GetPlatformInfo (&PlatformInfo);
  ASSERT_EFI_ERROR (Status);
  if (PlatformInfo == NULL) {
    return Status;
  }

  Status = GetBoardConfigBlock (&gPlatformBoardConfigPreMemGuid, (VOID **) &PlatformBoardConfig);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  if (BootMode == BOOT_ON_FLASH_UPDATE) {
    //
    // Get platform policy settings through the SiPolicy PPI
    //
    Status = PeiServicesLocatePpi (
               &gSiPreMemPolicyPpiGuid,
               0,
               NULL,
               (VOID **) &SiPreMemPolicyPpi
               );
    ASSERT_EFI_ERROR (Status);

    Status = GetConfigBlock ((VOID *) SiPreMemPolicyPpi, &gSaMiscPeiPreMemConfigGuid, (VOID *) &MiscPeiPreMemConfig);
    ASSERT_EFI_ERROR (Status);
    //
    // Set SSKPD before memory initialize when BOOT_ON_FLASH_UPDATE
    //
    MmioWrite8 (MiscPeiPreMemConfig->MchBar + 0x5D10, 1);
  }
#ifndef MINTREE_FLAG
  //
  // Locate Setup variables
  //
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &VariableServices
             );
  ASSERT_EFI_ERROR (Status);

  VarSize = sizeof (PCH_SETUP);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"PchSetup",
                               &gPchSetupVariableGuid,
                               NULL,
                               &VarSize,
                               &PchSetup
                               );
  if (!EFI_ERROR(Status) && (PlatformInfo->EcPresent == TRUE)) {
// AMI_OVERRIDE_START - Use Flag to separte the code which use EC command.
#ifdef AMI_CRB_EC_SUPPORT_FLAG
    EcInit (PchSetup.PchLan, BootMode);
#endif // End of AMI_CRB_EC_SUPPORT_FLAG
// AMI_OVERRIDE_END - Use Flag to separte the code which use EC command.
  }

  VarSize = sizeof (CPU_SETUP);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"CpuSetup",
                               &gCpuSetupVariableGuid,
                               NULL,
                               &VarSize,
                               &CpuSetup
                               );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (!EFI_ERROR(Status)) {
    if ((PlatformInfo->PlatformFlavor == FlavorDesktop) ||
        (PlatformInfo->PlatformFlavor == FlavorWorkstation)) {
      // Program GPIO32 to High if ImonScaling enabled
// AMI_OVERRIDE_START - Fix to build error when it doesn't support OverClock.      
#if defined(OVER_CLOCK_SUPPORT) && (OVER_CLOCK_SUPPORT == 1)         
      if (CpuSetup.ImonScaling == 1) {
        //GpioSetOutputValue (POWER_GATE_XX_GPIO_PIN, V_PCH_GPIO_TX_STATE_HIGH); //@todo, uncomment when POWER_GATE_XX_GPIO_PIN has been assigned correct GpioPad
      }
#endif // End of OVER_CLOCK_SUPPORT
// AMI_OVERRIDE_END - Fix to build error when it doesn't support OverClock.
    }
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
  if (!EFI_ERROR(Status)) {
    //
    // Configure BT wireless charging
    //
    ConfBTWirelessCharging(SystemConfiguration.BtWirelessChargingMode);

// AMI_OVERRIDE_START - Fixed build error when disable RTD3_SUPPORT.
#if defined (RTD3_SUPPORT) && RTD3_SUPPORT == 1    
    //
    // If any touch sensor is enabled and RTD3 isn't, enable power to touch
    // (configure GPPB_14 to output level high)
    //
    if (SystemConfiguration.Rtd3Support == 0) {
      for (Index = 0; Index <= 12; Index++) {
        if (FALSE) {
          // set GPPB_14 high
          GpioSetOutputValue (GPIO_SKL_LP_GPP_B14, 1);
          break;
        }
      }
    }

#endif // End of RTD3_SUPPORT
// AMI_OVERRIDE_END - Fixed build error when disable RTD3_SUPPORT.
// AMI_OVERRIDE_START - Use Flag to separte the code which use EC command.
#ifdef AMI_CRB_EC_SUPPORT_FLAG
    //
    // USBC support: BSSB
    //
    if (PlatformInfo->TypeCSupport == TRUE) {
      if ((PchSetup.PchDciEn == TRUE) && (SystemConfiguration.UsbTypecBssbMode == 0)) {
        // BSSB Status : IOSF-SB port ID 0xB8 offset 0x4h ECTRL[9]
        // Debug Status:
        //    Bit7-1: Reserved
        //    Bit0: BSSB Connected
        PchPcrRead32 (PID_DCI, R_PCH_PCR_DCI_ECTRL, &Data32);
        DebugStatus = !!(Data32 & BIT9);
        SendEcCommand (BSSB_DEBUG_INDICATION);
        SendEcData (DebugStatus);
      }
    }
#endif // End of AMI_CRB_EC_SUPPORT_FLAG
// AMI_OVERRIDE_END - Use Flag to separte the code which use EC command.

    VarSize = sizeof (SA_SETUP);
    Status = VariableServices->GetVariable (
                                 VariableServices,
                                 L"SaSetup",
                                 &gSaSetupVariableGuid,
                                 NULL,
                                 &VarSize,
                                 &SaSetup
                                 );
    if (!EFI_ERROR(Status)) {
      if (PlatformInfo->PlatformType == TypeUltUlx) {
        // Force Valid Sensor Settings depending on the Option selected in Sensor Hub Type Setup Option.
        if(SystemConfiguration.SensorHubType == 0) {
          SystemConfiguration.UsbSensorHub = 0;
        } else if (SystemConfiguration.SensorHubType == 1) {
          SaSetup.AlsEnable = 0;
          SystemConfiguration.UsbSensorHub = 0;
        } else {
          SaSetup.AlsEnable = 0;
          SystemConfiguration.UsbSensorHub = 1;
        }
      }
// AMI_OVERRIDE_START - For supporting CRB I2c IO expander enabled/disabled     	 
#if AMI_CRB_I2C_IO_EXPANDER_SUPPORT_FLAG 
// AMI_OVERRIDE_END - For supporting CRB I2c IO expander enabled/disabled
      if (PlatformBoardConfig->Enable.IvCamSpecificInit){
        if ((SystemConfiguration.CameraType == 1) && PcdGetBool (PcdIoExpanderPresent)) { //IVCAM Selected and IO expander exists
          //Workaround for power leakage issue. Needs to be reverted once board team fixes it
          DEBUG((DEBUG_INFO, "IVCAM Selected in the Setup Menu, Set DFU Pin High on IO Expander\n"));
          GpioExpSetOutput(IO_EXPANDER_0, IO_EXPANDER_GPIO_18, IO_EXPANDER_GPO_LEVEL_HIGH); //Set DFU Pin-18 (IMAGING_DFU_IOEXP) High on IO Expander 0
        }
      }
// AMI_OVERRIDE_START - For supporting CRB I2c IO expander enabled/disabled
#endif // End of AMI_CRB_I2C_IO_EXPANDER_SUPPORT_FLAG   
// AMI_OVERRIDE_END - For supporting CRB I2c IO expander enabled/disabled
    }
  }
#endif
  return EFI_SUCCESS;
}
#ifndef MINTREE_FLAG
/**
  Do platform specific programming post-memory.

  @retval  EFI_SUCCESS       The function completed successfully.
**/
EFI_STATUS
PlatformSpecificInit (
  VOID
  )
{
  EFI_STATUS                      Status;
  CPU_SETUP                       CpuSetup;
  UINTN                           VarSize;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariableServices;

  //
  // Locate Setup variables
  //
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &VariableServices
             );
  ASSERT_EFI_ERROR (Status);


  VarSize = sizeof (CPU_SETUP);
  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"CpuSetup",
                               &gCpuSetupVariableGuid,
                               NULL,
                               &VarSize,
                               &CpuSetup
                               );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (PcdGetBool (PcdCpuVboostEnable)) {
    if (CpuSetup.VboostEnable == 1) {
      // If we want to enable the Vboost circuit on RVP8 boards
      //  * Configure GPP_F23 Output to High
      GpioSetOutputValue (GPIO_SKL_H_GPP_F23, V_PCH_GPIO_TX_STATE_HIGH);
    }
  }

  return EFI_SUCCESS;
}

// AMI_OVERRIDE_START - AMI not use.
#if 0
/**
  figure out the CSM configuration and post it to PcdCsmEnable.

  @param[in]  Event     Event whose notification function is being invoked.
  @param[in]  Context   Pointer to the notification function's context.
**/
VOID
EFIAPI
GetCsmControlStatus (
  VOID
  )
{
  UINT8                   SecureBoot;
  EFI_STATUS              Status;
  SA_DATA_HOB             *SaDataHob;
  UINTN                   VariableSize;
  SETUP_DATA              mSetupData;
  BOOLEAN                 CsmEnabled;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *PeiVar;
#ifdef FSP_WRAPPER_FLAG
  VOID                    **FspHobListPtr;
  EFI_PEI_HOB_POINTERS     Hob;
#endif

  ///
  /// Read EFI variable
  ///
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &PeiVar
             );
  ASSERT_EFI_ERROR (Status);
  VariableSize = sizeof (SecureBoot);
  Status = PeiVar->GetVariable (
                     PeiVar,
                     EFI_SECURE_BOOT_MODE_NAME,
                     &gEfiGlobalVariableGuid,
                     NULL,
                     &VariableSize,
                     &SecureBoot
                     );

  if (!EFI_ERROR (Status) && (SecureBoot == SECURE_BOOT_MODE_ENABLE)) {
     CsmEnabled = FALSE;
  } else {
    VariableSize = sizeof (SETUP_DATA);
    Status = PeiVar->GetVariable (
                       PeiVar,
                       L"Setup",
                       &gSetupVariableGuid,
                       NULL,
                       &VariableSize,
                       &mSetupData
                       );

    if (EFI_ERROR (Status)) {
      mSetupData.CsmControl = EFI_CSM_CONTROL_DETECT_LEGACY_VGA;
    }
    switch (mSetupData.CsmControl) {
    case EFI_CSM_CONTROL_ALWAYS_OFF:
      CsmEnabled = FALSE;
      break;

    case EFI_CSM_CONTROL_ALWAYS_ON:
      CsmEnabled = TRUE;
      break;

    case EFI_CSM_CONTROL_DETECT_LEGACY_VGA:
      SaDataHob = NULL;
#ifndef FSP_WRAPPER_FLAG
      SaDataHob = (SA_DATA_HOB *) GetFirstGuidHob (&gSaDataHobGuid);
#else
      Hob.Raw = GetFirstGuidHob(&gFspHobGuid);
      if (Hob.Raw != NULL) {
        FspHobListPtr = GET_GUID_HOB_DATA(Hob.Raw);
        SaDataHob = GetNextGuidHob(&gSaDataHobGuid, *FspHobListPtr);
      }
#endif
      if (SaDataHob != NULL) {
        if (SaDataHob->LegacyOnlyVgaOpRomDetected) {
          CsmEnabled = TRUE;
        } else {
          CsmEnabled = FALSE;
        }
      } else {
        CsmEnabled = FALSE;
      }
      break;

    default:
      ASSERT (FALSE);
      CsmEnabled = FALSE;
      break;
    }
  }

  PcdSetBoolS (PcdCsmEnable, CsmEnabled);
}

#endif
// AMI_OVERRIDE_END - AMI not use.
/**
  Checks if Premium PMIC present (VendorID == 1Fh)

  @retval  TRUE  if present
  @retval  FALSE it discrete/other PMIC
**/
// AMI_OVERRIDE_START - Use Flag to separte the code which use EC command.
#ifdef AMI_CRB_EC_SUPPORT_FLAG
BOOLEAN
IsPremiumPmicPresent (
  VOID
  )
{
  UINT8                           PmicVendorID;

  PmicVendorID = 0;
  //Send KSC Command to detect vendor ID of PMIC is present on the system(Applicable only for ULT/ULX Platforms)
  DetectPmicVendorID (&PmicVendorID);
  DEBUG((DEBUG_INFO, "Vendor ID of the Pmic Present on the system is: %x\n", PmicVendorID));

  if (PmicVendorID == 0x1F) {
    return TRUE;
  }

  return FALSE;
}

/**
  Pmic Programming to supprort LPAL Feature

  @retval     EFI_SUCCESS       Operation success.
**/
VOID
PremiumPmicDisableSlpS0Voltage (
  VOID
  )
{
  EFI_STATUS                      Status;
  UINT8                           EcDataV085ACNT;
  UINT8                           DataBuffer[2];
  //
  // Disable voltage margining for Premium PMIC by changing V085ACNT[7:6] to [0,0]
  //
  EcDataV085ACNT = 0x3A;
  //
  // Write value 0x3A at offset 0xE8 (EC APCI Name Space) to disable voltage margining
  //
  DataBuffer[1] = EcDataV085ACNT;
  DataBuffer[0] = 0xE8;
  Status = WriteEcRam(DataBuffer);
  ASSERT_EFI_ERROR(Status);
  Status = SetSlpS0Voltage(EcDataV085ACNT);
  ASSERT_EFI_ERROR(Status);
}

/**
  Pmic Voltage Selection as directed by User

  @param[in]  SystemConfiguration
**/
VOID
PmicVoltageSelect (
  IN SETUP_DATA              *SystemConfiguration
  )
{
  EFI_STATUS                      Status;
  UINT8                           EcDataVcc;
  UINT8                           EcDataVddq;

  if ((SystemConfiguration->PmicVccLevel == 7) && (SystemConfiguration->PmicVddqLevel == 8)) {
    return;
  }

  switch (SystemConfiguration->PmicVccLevel) {
    case 0:
      EcDataVcc = 0x01;
      break;
    case 1:
      EcDataVcc = 0x00;
      break;
    case 2:
      EcDataVcc = 0x02;
      break;
    case 3:
      EcDataVcc = 0x03;
      break;
    case 4:
      EcDataVcc = 0x04;
      break;
    case 5:
      EcDataVcc = 0x08;
      break;
    case 6:
      EcDataVcc = 0xDA;
      break;
    default:
      EcDataVcc = 0x1A;
      break;
  }
  switch (SystemConfiguration->PmicVddqLevel) {
    case 0:
      EcDataVddq = 0x00;
      break;
    case 1:
      EcDataVddq = 0x01;
      break;
    case 2:
      EcDataVddq = 0x02;
      break;
    case 3:
      EcDataVddq = 0x03;
      break;
    case 4:
      EcDataVddq = 0x04;
      break;
    case 5:
      EcDataVddq = 0x05;
      break;
    case 6:
      EcDataVddq = 0x06;
      break;
    case 7:
      EcDataVddq = 0x07;
      break;
    default:
      EcDataVddq = 0x0A;
      break;
  }
  Status = SetPmicVoltage (EcDataVcc, EcDataVddq);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return;
  }
}

/**
  Configure EC for specific devices

  @param[in] PchLan       - The PchLan of PCH_SETUP variable.
  @param[in] BootMode     - The current boot mode.
**/
VOID
EcInit (
  IN UINT8                PchLan,
  IN EFI_BOOT_MODE        BootMode
  )
{
  EFI_STATUS              Status;
  PLATFORM_INFO           *PlatformInfo;

  Status = EFI_SUCCESS;
  if (PchLan != PEI_DEVICE_DISABLED) {
    Status = EnableLanPower (TRUE);
  } else {
    Status = EnableLanPower (FALSE);
  }
  ASSERT_EFI_ERROR (Status);

  Status = GetPlatformInfo (&PlatformInfo);
  ASSERT_EFI_ERROR (Status);

  //
  // Clear 10sec PB Override mode in EC so that Power button is always available during POST.
  //
  ClearTenSecPwrButtonMode ();
}
#endif // End of AMI_CRB_EC_SUPPORT_FLAG
// AMI_OVERRIDE_END - Use Flag to separte the code which use EC command.

/**
  This function returns the state of the MFG_MODE jumper.

  @param[in] PlatformInfo   - Record current platform type and BoardId.

  @retval TRUE              - The system is booting in manufacturing mode
  @retval FALSE             - Normal Boot
**/
BOOLEAN
IsManufacturingMode (
  IN PLATFORM_INFO                   *PlatformInfo
  )
{
  BOOLEAN               ManufacturingMode;
  UINT32                ManufactureModeGpioVal;
  UINT8                 MfgModeGpio;

  ManufacturingMode = FALSE;

  if (PlatformInfo->PlatformType == TypeTrad) {
    if (PlatformInfo->PlatformFlavor == FlavorUpServer) {
      GpioGetInputValue (UP_ZUMBA_BEACH_MANUFACTURE_MODE_GPIO_PAD, &ManufactureModeGpioVal);
      ManufacturingMode = (BOOLEAN) ManufactureModeGpioVal;
    } else {
      GpioGetInputValue (MANUFACTURE_MODE_GPIO_PIN, &ManufactureModeGpioVal);
      ManufacturingMode = (BOOLEAN) ManufactureModeGpioVal;
    }
  }

  if (PlatformInfo->PlatformType == TypeUltUlx) {
    MfgModeGpio = IoRead8 (PcdGet16 (PcdSioBaseAddress) + 0x0C);
    ManufacturingMode = (BOOLEAN) ((MfgModeGpio & 0x02) == 0);
  }

  return ManufacturingMode;
}
#endif // MINTREE_FLAG
