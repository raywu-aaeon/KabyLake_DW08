/**@file
  TBT DXE phase - Disable Oprom, update ACPI AML code, initialize HR status variable

 @copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2018 Intel Corporation.

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
//AMI_OVERRIDE_START - Remove all unnecessary & duplicated header files
//#include <Protocol/AcpiTable.h>
//#include <IndustryStandard/Acpi.h>
//#include <Protocol/AcpiSystemDescriptionTable.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
// AMI_OVERRIDE_TBT0001 - Add for OpRom support >>
#include <IndustryStandard/Pci.h>
#include <PciBus.h>
//#include <Library/AslUpdateLib.h>
// AMI_OVERRIDE_TBT0001 - Add for OpRom support <<
#include <SetupVariable.h>
#include <CommonDefinitions.h>
#include <Library/TbtCommonLib.h>
#include <TbtBoardInfo.h>
#include <Protocol/GlobalNvsArea.h>
//#include <Protocol/FirmwareVolume2.h>
//AMI_OVERRIDE_START - This included header is duplicated
//#include <Protocol/AcpiTable.h>
//AMI_OVERRIDE_END - This included header is duplicated
//#include <IndustryStandard/Acpi10.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/IoLib.h>
//#include <Library/MemoryAllocationLib.h>
#include <Library/CpuPlatformLib.h>
#include <Guid/HobList.h>
#include <Library/UefiLib.h>
#include <Uefi/UefiSpec.h>
//#include <Library/PcdLib.h>
#include <Library/MmPciLib.h>
#include <Library/GpioExpanderLib.h>
//#include <GpioPinsSklLp.h>
//#include <CpuRegs.h>
//#include <PcieRegs.h>
//#include <Library/TbtCommonLib.h>
//#include <Protocol/SaGlobalNvsArea.h>
//#include <Protocol/GlobalNvsArea.h>
//AMI_OVERRIDE_END - Remove all unnecessary & duplicated header files
//AMI_OVERRIDE_START >>> EIP357796 : Set TBT as enable as the default, some related settings are not applied
#include <Library/IoLib.h>
//AMI_OVERRIDE_END <<< EIP357796 : Set TBT as enable as the default, some related settings are not applied
// AMI_OVERRIDE_TBT0001 - Add for OpRom support >>
#if CSM_SUPPORT
#include <Protocol/LegacyBiosExt.h>
#endif
// AMI_OVERRIDE_TBT0001 - Add for OpRom support <<
//AMI_OVERRIDE_START >>> EIP342457 : TBT DXE must be dispatched before TBT SMM
#include <Protocol/AmiTbtProtocol.h>
//AMI_OVERRIDE_END <<< EIP342457 : TBT DXE must be dispatched before TBT SMM
#include <Library/DxeTbtSecurityLib.h>

#define TBT_DEBUG_AREA_SIZE_IN_KB     64

//AMI_OVERRIDE_START >>> EIP357796 : Set TBT as enable as the default, some related settings are not applied
#define PCH_PCIE_NUM  (24)
//AMI_OVERRIDE_END <<< EIP357796 : Set TBT as enable as the default, some related settings are not applied

GLOBAL_REMOVE_IF_UNREFERENCED TBT_INFO_HOB            *gTbtInfoHob;
GLOBAL_REMOVE_IF_UNREFERENCED SETUP_DATA              mSetupData;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_PHYSICAL_ADDRESS    gTbtDebugBaseAddressMem = 0;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_PHYSICAL_ADDRESS    gTbtDgpuPciBaseAddressMem;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_PHYSICAL_ADDRESS    gTbtDeviceTreeBuffer;
GLOBAL_REMOVE_IF_UNREFERENCED VOID                    *VbiosAddress = NULL;
GLOBAL_REMOVE_IF_UNREFERENCED UINT32                  VbiosSize;
//AMI_OVERRIDE_START - EIP322154 - Remove Thunderbolt DGPU related setup items
GLOBAL_REMOVE_IF_UNREFERENCED UINT8                   TbtXConnectSupport = 0;
//AMI_OVERRIDE_END - EIP322154 - Remove Thunderbolt DGPU related setup items
GLOBAL_REMOVE_IF_UNREFERENCED UINT8                   TbtRpSelector;
GLOBAL_REMOVE_IF_UNREFERENCED CPU_SKU                 CpuSku;
// AMI_OVERRIDE_TBT0001 - Add for OpRom support >>
VOID                                *CsmOpromPolicyRegs;
STATIC UINT8 TbtHostLocationBus  = TBT_UP_PORT_BUS;
STATIC UINT8 TbtHostLocationDev  = TBT_UP_PORT_DEV;
STATIC UINT8 TbtHostLocationFun  = TBT_UP_PORT_FUN;
STATIC UINT8 TbtSkipPciOprom     = DEFAULT_SKIP_TBT_OPTIONROM;
// AMI_OVERRIDE_TBT0001 - Add for OpRom support <<
//AMI_OVERRIDE_START - RC3.6.5 add this in TbtCommonLib, so remove it.
#if 0
//AMI_OVERRIDE_START - EIP333764 - Enable TBT and disable "Skip Option Rom", but Graphic card is not workable with Legacy Mode
/**
  Check this Device ID is TBT HR or not.

  @param  DeviceID              Device ID

  @retval TRUE                  TBT HR
  @retval FALSE                 Not TBT HR
**/
BOOLEAN
IsTbtHostRouter (
  IN    UINT16                  DeviceID
  )
{

  switch (DeviceID) {
  case LR_HR:
  case ER_SFF_HR:
  case ER_HR:
  case CR_HR_4C:
  case CR_HR_2C:
  case RR_HR_2C:
  case RR_HR_4C:
  case FR_HR_2C:
  case FR_HR_4C:
  case WR_HR_2C:
  case AR_HR_2C:
  case AR_HR_4C:
  case AR_HR_LP:
  case AR_HR_C0_2C:
  case AR_HR_C0_4C:
  case TR_HR_2C:
  case TR_HR_4C:
    return TRUE;
  }

  return FALSE;
} // IsTbtHostRouter
//AMI_OVERRIDE_END - EIP333764 - Enable TBT and disable "Skip Option Rom", but Graphic card is not workable with Legacy Mode
#endif
//AMI_OVERRIDE_START - RC3.6.5 add this in TbtCommonLib, so remove it.

// AMI_OVERRIDE_TBT0001 - Add for OpRom support >>
#if defined CSM_SUPPORT && CSM_SUPPORT == 1
/**
  Callback function for CSM OPROM policy

  @param  Event                 Event that caused the call
  @param  Context               Context at which event was caused

  @return                       No return value
**/
VOID DisableTbtDeviceRomCallback (
  IN EFI_EVENT                  Event,
  IN VOID                       *Context
  )
{
    EFI_STATUS                  Status;
    EFI_HANDLE                  Handle;
    UINTN                       Size = sizeof(EFI_HANDLE);
    UINTN                       CheckedDev, CheckedFun;
    CSM_PLATFORM_POLICY_DATA    *CsmOpromPolicyData;
    EFI_PCI_IO_PROTOCOL         *PciIo;
    PCI_DEV_INFO                *Device;
//AMI_OVERRIDE_START - EIP333764 - Enable TBT and disable "Skip Option Rom", but Graphic card is not workable with Legacy Mode
    UINT8                       TbtHrSbus;
    UINT16                      DeviceId;
//AMI_OVERRIDE_END - EIP333764 - Enable TBT and disable "Skip Option Rom", but Graphic card is not workable with Legacy Mode

    DEBUG ((DEBUG_INFO, "TbtDxe: DisableTbtDeviceRomCallback() Entry\n"));

    Status = gBS->LocateHandle(ByRegisterNotify, NULL, CsmOpromPolicyRegs, &Size, &Handle);
    if (EFI_ERROR(Status)) return;

    Status = gBS->HandleProtocol(Handle, &gOpromStartEndProtocolGuid, &CsmOpromPolicyData);
    if (EFI_ERROR(Status)) return;

    if(CsmOpromPolicyData == NULL) return; //post-process OpROM callback
    if(CsmOpromPolicyData->PciIo == NULL) return; // OEM Service ROM

    Status = GetTbtRpDevFun((UINTN)(gTbtInfoHob->TbtSelector - 1), &CheckedDev, &CheckedFun);

    DEBUG ((DEBUG_INFO, "TbtDxe: Host at B:%x|D:%x|F:%x\n", gTbtInfoHob->Bus, CheckedDev, CheckedFun));
    TbtHostLocationBus = gTbtInfoHob->Bus;
    TbtHostLocationDev = (UINT8)CheckedDev;
    TbtHostLocationFun = (UINT8)CheckedFun;

    PciIo = CsmOpromPolicyData->PciIo;
    Device = (PCI_DEV_INFO*)PciIo;

//AMI_OVERRIDE_START - EIP333764 - Enable TBT and disable "Skip Option Rom", but Graphic card is not workable with Legacy Mode
    if ((MmioRead32 (MmPciBase (gTbtInfoHob->Bus, (UINT32)CheckedDev, (UINT32)CheckedFun) + PCI_RID)) == 0XFFFFFFFF) {
      DEBUG ((DEBUG_INFO, "TbtDxe: No device found at B:%x|D:%x|F:%x\n", gTbtInfoHob->Bus, CheckedDev, CheckedFun));
      return;
    }

    TbtHrSbus = MmioRead8 (MmPciBase (gTbtInfoHob->Bus, (UINT32)CheckedDev, (UINT32)CheckedFun) + PCI_SBUS);
    if ((MmioRead32 (MmPciBase (TbtHrSbus, 0, 0) + PCI_RID)) == 0XFFFFFFFF) {
      DEBUG ((DEBUG_INFO, "TbtDxe: No device found at B:%x|D:0|F:0\n", TbtHrSbus));
      return;
    }

    DeviceId = MmioRead16 (MmPciBase (TbtHrSbus, 0, 0) + PCI_DID);
    if (!IsTbtHostRouter (DeviceId)) {
      DEBUG ((DEBUG_INFO, "TbtDxe: TBT HR not found.\n"));
      return;
    }
//AMI_OVERRIDE_END - EIP333764 - Enable TBT and disable "Skip Option Rom", but Graphic card is not workable with Legacy Mode

    while ((Device->Type != tPciRootBrg) && (Device->ParentBrg != NULL)) {
        Device = Device->ParentBrg;
        if (Device->Address.Addr.Bus != TbtHostLocationBus) continue;
        if (Device->Address.Addr.Device != TbtHostLocationDev) continue;
        if (Device->Address.Addr.Function != TbtHostLocationFun) continue;
        if (TbtSkipPciOprom){
            CsmOpromPolicyData->ExecuteThisRom = FALSE;  //this attritube default is TRUE
            DEBUG ((DEBUG_INFO, "TbtDxe: ExecuteThisRom is setted FALSE.\n"));
        }

        break;
    }

    DEBUG ((DEBUG_INFO, "TbtDxe: DisableTbtDeviceRomCallback() Exit\n"));
}
#endif

//AMI_OVERRIDE_START >>> EIP357796 : Set TBT as enable as the default, some related settings are not applied
/**
  Config SetupVariable for TBT enabled in default when first boot

  @param  VOID                  No parameter

  @return                       No return value
**/
VOID
AmiConfigSetupForDefaultTbtEnable (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINTN                         DataSize;
  UINT32                        SetupAttr;
  SETUP_DATA                    SetupData;

  DataSize = sizeof (SETUP_DATA);
  Status = gRT->GetVariable (
                  L"Setup",
                  &gSetupVariableGuid,
                  &SetupAttr,
                  &DataSize,
                  &SetupData
                  );

  if (Status != EFI_SUCCESS) {
    return;
  }

  SetupData.TbtSupport = DEFAULT_TBT_ENABLE;

  //
  // TBT is enabled as the default: Below code sync with TbtSetup.c
  //
  if (SetupData.TbtSupport == 1) {
    SetupData.TbtHostLocationGroup = DEFAULT_TBT_AIC_LOCATION_GROUP;

    SetupData.TbtSelector    = gTbtInfoHob->TbtSelector;

    SetupData.PciExpNative   = 0;
    SetupData.Rtd3Support    = 0;
    SetupData.TbtPcieSupport = 0;

    SetupData.TBThostRouter = TBT_HOST_ROUTER;

    if(SetupData.TBThostRouter == 2) {
      SetupData.MaxTolud = TBT_DEFAULT_TOLUD_TWO_PORT;
    } else {
      SetupData.MaxTolud = TBT_DEFAULT_TOLUD_ONE_PORT;
    }

    //
    // Set PCIe resource
    //
    if (SetupData.TBThostRouter == 2) {
      SetupData.TbtPcieExtraBusRsvd   = 106;
      SetupData.TbtPcieMemRsvd        = 737;
      SetupData.TbtPcieMemAddrRngMax  = 26;
      SetupData.TbtPciePMemRsvd       = 1184;
      SetupData.TbtPciePMemAddrRngMax = 28;
      SetupData.TbtPcieIoRsvd         = TBT_DEFAULT_PCIE_IO_RESERVED;
    } else {
      SetupData.TbtPcieExtraBusRsvd   = 56;
      SetupData.TbtPcieMemRsvd        = 353;
      SetupData.TbtPcieMemAddrRngMax  = 26;
      SetupData.TbtPciePMemRsvd       = 544;
      SetupData.TbtPciePMemAddrRngMax = 28;
      SetupData.TbtPcieIoRsvd         = TBT_DEFAULT_PCIE_IO_RESERVED;
    }

    if (SetupData.TbtBootOn == 1) {
      SetupData.AssignIo      = 1;
      SetupData.TbtPcieIoRsvd = TBT_PCIE_IO_RESERVED_WHEN_BOOT_ON_SUPPORT;
    }

    //
    // Enable PCIe HPE & Set Max Payload Size
    //
    if (SetupData.TbtSelector == PCH_PCIE_NUM+1) {
      SetupData.Peg0Enable               = 1;
      SetupData.PegMaxPayload[0]         = 0;
      SetupData.PegRootPortHPE[0]        = 1;
      SetupData.Peg0PowerDownUnusedLanes = 0;
    } else if (SetupData.TbtSelector == PCH_PCIE_NUM+2) {
      SetupData.Peg1Enable               = 1;
      SetupData.PegMaxPayload[1]         = 0;
      SetupData.PegRootPortHPE[1]        = 1;
      SetupData.Peg1PowerDownUnusedLanes = 0;
    } else if (SetupData.TbtSelector == PCH_PCIE_NUM+3) {
      SetupData.Peg2Enable               = 1;
      SetupData.PegMaxPayload[2]         = 0;
      SetupData.PegRootPortHPE[2]        = 1;
      SetupData.Peg2PowerDownUnusedLanes = 0;
    } else if (SetupData.TbtSelector <= PCH_PCIE_NUM) {
      SetupData.PcieRootPortHPE[SetupData.TbtSelector] = 1;
    }

  }

  Status = gRT->SetVariable (
                  L"Setup",
                  &gSetupVariableGuid,
                  SetupAttr,
                  DataSize,
                  &SetupData
                  );

  if (Status == EFI_SUCCESS) {
    //
    // Full reset to take effect if TbtSupport is enabled in default
    //
    if (SetupData.TbtSupport == 1) {
      IoWrite8 (0xCF9, 0xE);
    }
  }

  mSetupData.TbtSupport = SetupData.TbtSupport;

}
//AMI_OVERRIDE_END <<< EIP357796 : Set TBT as enable as the default, some related settings are not applied

/**
  TBT DXE driver init. function

  @param  ImageHandle           Image handle.
  @param  SystemTable           Pointer to the EFI system table.
  @param  TbtInfoHob            Pointer to TBT HOB
  @param  SetupData             Pointer to Setup date

  @return                       No return value
**/
EFI_STATUS
AmiTbtDxeInit (
  IN     EFI_HANDLE             ImageHandle,
  IN     EFI_SYSTEM_TABLE       *SystemTable,
  IN     TBT_INFO_HOB           *TbtInfoHob,
  IN OUT SETUP_DATA             *SetupData
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    Handle = NULL;
  AMI_TBT_HR_STATUS_DATA        TbtHrStatusData;
  UINTN                         TbtHrStatusSize = sizeof(AMI_TBT_HR_STATUS_DATA);
  UINT16                        counter = 0;

  DEBUG ((DEBUG_INFO, "AmiTbtDxeInitEntryPoint\n"));

  // Initial Host Router information
  Status = gRT->GetVariable( AMI_TBT_HR_STATUS_VARIABLE, \
                             &gAmiTbtHrStatusGuid, \
                             NULL, \
                             &TbtHrStatusSize, \
                             &TbtHrStatusData );
  if (EFI_ERROR(Status)){
    DEBUG ((DEBUG_INFO, "Can not get Thunderbolt Host Router Information !!! \n"));

    TbtHrStatusData.TbtHrStatus   = 0;
    //Get Thunderbolt host Series
    TbtHrStatusData.TbtHrSeries = 3;
    Status = gRT->SetVariable( AMI_TBT_HR_STATUS_VARIABLE, \
                               &gAmiTbtHrStatusGuid, \
                               EFI_VARIABLE_NON_VOLATILE | \
                               EFI_VARIABLE_BOOTSERVICE_ACCESS | \
                               EFI_VARIABLE_RUNTIME_ACCESS, \
                               TbtHrStatusSize, \
                               &TbtHrStatusData );
    DEBUG ((DEBUG_INFO, "Create Thunderbolt Host Router Information %r !!!\n", Status));

//AMI_OVERRIDE_START >>> EIP357796 : Set TBT as enable as the default, some related settings are not applied
    AmiConfigSetupForDefaultTbtEnable ();
//AMI_OVERRIDE_END <<< EIP357796 : Set TBT as enable as the default, some related settings are not applied
  }

  TbtHostLocationBus = TbtInfoHob->Bus;
  TbtHostLocationDev = TbtInfoHob->Dev;
  TbtHostLocationFun = TbtInfoHob->Fun;
  TbtSkipPciOprom    = TbtInfoHob->TbtSkipPciOprom;

  return EFI_SUCCESS;
}
// AMI_OVERRIDE_TBT0001 - Add for OpRom support <<

/**
  Callback function when ReadyToBoot event occured.

  @param  Event                 Event that caused the call
  @param  Context               Context at which event was caused

  @return                       No return value
**/
VOID
TbtCallback (
  IN EFI_EVENT                  Event,
  IN VOID                       *Context
  )
{
  EFI_GLOBAL_NVS_AREA_PROTOCOL  *GlobalNvsAreaProtocol;
  EFI_STATUS                    Status;
  UINTN                         DataSize;
  EFI_GLOBAL_NVS_AREA           *mGlobalNvsAreaPtr;
  SETUP_DATA                    SetupData;
// AMI_OVERRIDE_TBT0001 - Add for OpRom support >>
  UINT8                         TbtUpBUS;
  UINTN                         Dev, Fun, Address;
  EFI_ACPI_DESCRIPTION_HEADER   *DsdtTable;
  UINT32                        Length;
  UINT8                         *ptr = NULL;
  UINT8                         *DsdtPtr = NULL;
  ASL_OBJ_INFO                  ObjInfo;
// AMI_OVERRIDE_TBT0001 - Add for OpRom support <<
//AMI_OVERRIDE_START - (EIP304315+EIP309181) Fix S3 resume issue for Redhat OS
  EFI_ACPI_SUPPORT_PROTOCOL     *AcpiSupport;
  UINTN                         Index = 0;
  EFI_ACPI_TABLE_VERSION        Version = 0;
  UINTN                         TableHandle;
  static EFI_GUID               gEfiAcpiSupportProtocolGuid = EFI_ACPI_SUPPORT_GUID;
//AMI_OVERRIDE_END - (EIP304315+EIP309181) Fix S3 resume issue for Redhat OS

  DataSize = sizeof (SETUP_DATA);

  SetMem (&SetupData, DataSize, 0);

  Status = gRT->GetVariable (
                  L"Setup",
                  &gSetupVariableGuid,
                  NULL,
                  &DataSize,
                  &SetupData
                  );
//AMI_OVERRIDE_START - Code is executed when Tbt Support is enabled
  if (!EFI_ERROR (Status) && SetupData.TbtSupport == 1) {   // 2 = Fully Disable, Do NOT jump in
//AMI_OVERRIDE_END - Code is executed when Tbt Support is enabled
    Status = gBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, (VOID **) &GlobalNvsAreaProtocol);
    ASSERT_EFI_ERROR (Status);
    mGlobalNvsAreaPtr = GlobalNvsAreaProtocol->Area;
    GlobalNvsAreaProtocol->Area->ThunderboltHotSmi    = SetupData.TBTHotSMI;
    GlobalNvsAreaProtocol->Area->ThunderboltHotNotify = SetupData.TBTHotNotify;
// AMI_OVERRIDE_START - EIP 261796 Update NVS's ThunderboltSmiFunction in DXE and remove some unnecessary codes.
    GlobalNvsAreaProtocol->Area->ThunderboltSmiFunction = TBT_SWSMI_ENUMERATE_FUNCTION;
// AMI_OVERRIDE_END - EIP 261796 Update NVS's ThunderboltSmiFunction in DXE and remove some unnecessary codes.

// AMI_OVERRIDE_START EIP 261796 Update NVS's ThunderboltSmiFunction in DXE and remove some unnecessary codes.
#if 0
// AMI_OVERRIDE_START - Update ThunderboltSmiFunction value by Token.
    if (SetupData.CallSmiBeforeBoot & RESET_HR_BIT) {
      //
      // Reset PCIe tree under Host Router
      //
// AMI_OVERRIDE_START -
#if 0
      GlobalNvsAreaProtocol->Area->ThunderboltSmiFunction = 22;
#else
      GlobalNvsAreaProtocol->Area->ThunderboltSmiFunction = TBT_SWSMI_DISABLE_DEVICE_BRIDGAE_FUNCTION;
#endif
// AMI_OVERRIDE_END -
      IoWrite8 (SW_SMI_IO_ADDRESS, SW_SMI_TBT_ENUMERATE);
    }

    if (SetupData.CallSmiBeforeBoot & ENUMERATE_HR_BIT) {
      //
      // Call SMI handler to enumerate devices under Host Router properly
      //
// AMI_OVERRIDE_START -
#if 0
      GlobalNvsAreaProtocol->Area->ThunderboltSmiFunction = 21;
#else
      GlobalNvsAreaProtocol->Area->ThunderboltSmiFunction = TBT_SWSMI_ENUMERATE_FUNCTION;
#endif
// AMI_OVERRIDE_END -
      IoWrite8 (SW_SMI_IO_ADDRESS, SW_SMI_TBT_ENUMERATE);
    }
// AMI_OVERRIDE_START - Removed due to Intel doesn't verify it yet.
#if 0
    if (SetupData.Multicontroller) {
      if (SetupData.CallSmiBeforeBoot & RESET_HR_BIT) {
      //
      // Reset PCIe tree under Host Router
      //
      GlobalNvsAreaProtocol->Area->ThunderboltSmiFunction = 25;
      IoWrite8 (SW_SMI_IO_ADDRESS, SW_SMI_TBT_ENUMERATE);
    }

    if (SetupData.CallSmiBeforeBoot & ENUMERATE_HR_BIT) {
      //
      // Call SMI handler to enumerate devices under Host Router properly
      //
      GlobalNvsAreaProtocol->Area->ThunderboltSmiFunction = 24;
      IoWrite8 (SW_SMI_IO_ADDRESS, SW_SMI_TBT_ENUMERATE);
    }
}
#endif
// AMI_OVERRIDE_END - Removed due to Intel doesn't verify it yet.
// AMI_OVERRIDE_END - Update ThunderboltSmiFunction value by Token. <<
#endif
// AMI_OVERRIDE_END EIP 261796 Update NVS's ThunderboltSmiFunction in DXE and remove some unnecessary codes.

// AMI_OVERRIDE_START - Add for OpRom support.
   // Update TBUS Name object
   Status = GetTbtRpDevFun ((SetupData.TbtSelector - 1), &Dev, &Fun);
   ASSERT_EFI_ERROR (Status);

   Address = MmPciBase(0, (UINT32)Dev, (UINT32)Fun);
   TbtUpBUS = (UINT8)((MmioRead16(Address + PCI_PBUS) & 0xFF00) >> 8);

   DEBUG ((DEBUG_INFO, "TbtDxe:Thunderbotl Upstream Port Bus = %x !!!\n", TbtUpBUS));

//AMI_OVERRIDE_START - (EIP304315+EIP309181) Fix S3 resume issue for Redhat OS
  //
  // Locate ACPI tables
  //
  DEBUG ((DEBUG_INFO, "<Thunderbolt TbtDxe> Update DSDT At Boot\n"));
  Status = gBS->LocateProtocol (&gEfiAcpiSupportProtocolGuid, NULL, (VOID **) &AcpiSupport);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "<Thunderbolt TbtDxe> Locate gEfiAcpiSupportProtocolGuid error\n"));
    return;
  }

  do {
    Status = AcpiSupport->GetAcpiTable (
                            AcpiSupport,
                            Index,
                            &DsdtTable,
                            &Version,
                            &TableHandle
                            );
    if (Status == EFI_NOT_FOUND) {
      break;
    }
    Index++;
  } while (DsdtTable->Signature != EFI_ACPI_2_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE);

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "<Thunderbolt TbtDxe> DSDT table is not found\n"));
    return;
  }
//AMI_OVERRIDE_END - (EIP304315) Fix S3 resume issue for Redhat OS

   Length  = DsdtTable->Length - sizeof (EFI_ACPI_DESCRIPTION_HEADER);
   DsdtPtr = (UINT8 *) DsdtTable + sizeof (EFI_ACPI_DESCRIPTION_HEADER);

   // Locate TBUS
   Status = GetAslObj(DsdtPtr, Length, "TBUS", otName, &ObjInfo);
   if (EFI_ERROR(Status)) {
     DEBUG ((DEBUG_ERROR,"TbtDxe:Name TBUS not found.\n"));
     DEBUG ((DEBUG_ERROR,"TbtDxe:Name TBUS not found's status = %r.\n",Status));
   }

   DEBUG ((DEBUG_ERROR,"Read to change the value\n"));

   ptr = (UINT8*)ObjInfo.ObjName;

   //Fill TbtUpBUS
   *(UINT8*)(ptr + 5) = TbtUpBUS;

   //Use EFI_ACPI_TABLE_PROTOCOL to install DSDT START

   Status = AcpiSupport->SetAcpiTable (
                           AcpiSupport,
                           DsdtTable,
                           TRUE,
                           Version,
                           &TableHandle
                           );
   if (EFI_ERROR (Status)) {
     DEBUG ((DEBUG_INFO, "<Thunderbolt TbtDxe> SetAcpiTable error\n"));
     return;
   }
//AMI_OVERRIDE_END - (EIP304315+EIP309181) Fix S3 resume issue for Redhat OS
// AMI_OVERRIDE_END - Add for OpRom support.
  }

  gBS->CloseEvent (Event);
}

/**
  GPIO write function

  @param  GpioAccessType        Type01: call GpioSetOutputValue;                    Type02: call GpioExpSetOutput
  @param  Expander              Type01: don't care;                                 Type02: Expander Value with in the Contoller
  @param  GpioNumber            Type01: GPIO pad;                                   Type02: Pin with in the Expnader Value
  @param  Value                 Type01: Output value: 0: OutputLow, 1: OutputHigh;  Type02: none

  @return                       No return value
**/
VOID
GpioWrite (
  IN  UINT8                     GpioAccessType,
  IN  UINT8                     Expander,
  IN  UINT32                    GpioNumber,
  IN  BOOLEAN                   Value
  )
{
  if (GpioAccessType == 0x01) {
    // PCH
    GpioSetOutputValue (GpioNumber, (UINT32)Value);
  } else if (GpioAccessType == 0x02) {
    // IoExpander {TCA6424A}

    GpioExpSetOutput(Expander, (UINT8)GpioNumber, (UINT8)Value);
  }
}

/**
  GPIO read function

  @param  GpioAccessType        01: call GpioSetOutputValue;                   02: call GpioExpSetOutput
  @param  Expander              01: don't care                                 02: Expander Value with in the Contoller
  @param  GpioNumber            01: GPIO pad;                                  02: Pin with in the Expnader Value
  @param  PadState              {0: GPO [GPIO TX State], 1: GPI [GPIO RX State]}

  @return                       Return data
**/
UINT8
GpioRead (
  IN  UINT8                     GpioAccessType,
  IN  UINT8                     Expander,
  IN  UINT32                    GpioNumber,
  IN  UINT8                     PadState
  )
{
  UINT8                         Data8;
  UINT32                        Data32;

  Data8 = 0;
  Data32 = 0;

  if (GpioAccessType == 0x01) {
    // PCH
    if (PadState == 0x01) {
      // GPIO RX State
      GpioGetInputValue (GpioNumber, &Data32);
      Data8 = (UINT8) Data32;
    }
  } else if (GpioAccessType == 0x02) {
    // IoExpander {TCA6424A}

    Data8 = GpioExpGetInput (Expander, (UINT8)GpioNumber);
  }

  return Data8;
}

/**
  Based on the Security Mode Selection, BIOS drives FORCE_PWR.

  Force Power function.

  @param  GpioAccessType        Type01: call GpioSetOutputValue;                    Type02: call GpioExpSetOutput
  @param  Expander              Type01: don't care;                                 Type02: Expander Value with in the Contoller
  @param  GpioNumber            Type01: GPIO pad;                                   Type02: Pin with in the Expnader Value
  @param  Value                 Type01: Output value: 0: OutputLow, 1: OutputHigh;  Type02: none

  @return                       No return value
**/
VOID
ForcePower(
  IN  UINT8                     GpioAccessType,
  IN  UINT8                     Expander,
  IN  UINT32                    GpioNumber,
  IN  BOOLEAN                   Value
  )
{
  GpioWrite(GpioAccessType, Expander, GpioNumber, Value);
}

/**
  This function contains PCIE2TBT <-> TBT2PCIE handshake
  procedure and all related methods called directly or underectly
  by TbtSetPCIe2TBTCommand.

  @param  command               Thunderbolt FW setting command
  @param  TBT_US_BUS            Thunderbolt host location

  @return                       No return value
**/
VOID
TbtSetPCIe2TBTCommand (
  IN UINT8                      command,
  IN UINT8                      TBT_US_BUS
  )
{
  volatile UINT32               REG_VAL;
  volatile UINT32               max_wait_Iter;
  volatile UINT8                RetCode;
  UINTN                         DeviceBaseAddress;
  //
  // Wait 5 sec
  //
  max_wait_Iter     = 50;
  RetCode           = 0x52;

  DeviceBaseAddress = MmPciBase (TBT_US_BUS, 0x00, 0x00);
  MmioWrite32 (DeviceBaseAddress + PCIE2TBT_R, command | PCIE2TBT_VLD_B);

  IoWrite8 (0x80, 0x50);

  while (max_wait_Iter-- > 0) {
    REG_VAL = MmioRead32 (DeviceBaseAddress + TBT2PCIE_R);
    if (0xFFFFFFFF == REG_VAL) {
      //
      // Device is not here return now
      //
      RetCode = 0x5F;
      break;
    }

    if (REG_VAL & TBT2PCIE_DON_R) {
      RetCode = 0x51;
      break;
    }

    gBS->Stall (100 * 1000);
  }

  MmioWrite32 (DeviceBaseAddress + PCIE2TBT_R, 0);
  IoWrite8 (0x80, RetCode);
}

/**
  Thunderbolt(TM) DXE driver entry point.

  @param  ImageHandle           Image handle.
  @param  SystemTable           Pointer to the EFI system table.

  @retval EFI_SUCCESS           Initialized successfully
  @retval others                Initialized failed
**/
EFI_STATUS
EFIAPI
TbtDxeEntryPoint (
  IN EFI_HANDLE                 ImageHandle,
  IN EFI_SYSTEM_TABLE           *SystemTable
  )
{
  EFI_STATUS                    Status;
  EFI_EVENT                     Event;
// AMI_OVERRIDE_START - Add for OpRom support.
  EFI_EVENT                     CsmOpromPolicyEvent = NULL;
// AMI_OVERRIDE_END - Add for OpRom support.
  UINTN                         DataSize;
  SA_SETUP    					SaSetup;

  UINT32                        SetupAttr;
  VOID                          *MemoryBuffer;
  EFI_EVENT   					ExitBootServiceEvent;
  EFI_EVENT   					ReadyToLockEvent;
  EFI_EVENT   					EndOfDxeEvent;
  EFI_GLOBAL_NVS_AREA_PROTOCOL  *GlobalNvsAreaProtocol;
  EFI_GLOBAL_NVS_AREA           *mGlobalNvsAreaPtr;

  Status = EFI_SUCCESS;
  ExitBootServiceEvent = NULL;
  ReadyToLockEvent = NULL;
  EndOfDxeEvent = NULL;

  CpuSku      = GetCpuSku ();
  Status = gBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, (VOID **) &GlobalNvsAreaProtocol);
  ASSERT_EFI_ERROR (Status);
  mGlobalNvsAreaPtr = GlobalNvsAreaProtocol->Area;

  DEBUG ((DEBUG_INFO, "TbtDxeEntryPoint\n"));

  //
  // Get TBT INFO HOB
  //
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **) &gTbtInfoHob);
  if (!EFI_ERROR (Status) && (gTbtInfoHob != NULL)) {
    gTbtInfoHob = GetNextGuidHob (&gTbtInfoHobGuid, gTbtInfoHob);
    if (gTbtInfoHob == NULL) {
      DEBUG ((DEBUG_INFO, "TBT HOB not found\n"));
      return EFI_NOT_FOUND;
    }
  } else {
    return EFI_NOT_FOUND;
  }


  DataSize = sizeof (mSetupData);
  Status = gRT->GetVariable (
                  L"Setup",
                  &gSetupVariableGuid,
                  &SetupAttr,
                  &DataSize,
                  &mSetupData
                  );

  if (EFI_ERROR (Status)) {
    SetupAttr = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS;
  }
  if (mSetupData.TbtSupport && mSetupData.TbtSkipPciEnumeration) {
    PcdSetBoolS (PcdTbtSkipPciEnumeration, TRUE); // Skip bios boot time PCI enumeration for Tbt devices
    DEBUG ((DEBUG_INFO, "PcdTbtSkipPciEnumeration = TRUE\n"));
  } else {
    PcdSetBoolS (PcdTbtSkipPciEnumeration, FALSE); // Allow bios boot time PCI enumeration for Tbt devices
    DEBUG ((DEBUG_INFO, "PcdTbtSkipPciEnumeration = FALSE\n"));
  }

// AMI_OVERRIDE_START - EIP260096 Boot from Thunderbolt device fail
  AmiTbtDxeInit(ImageHandle, SystemTable, gTbtInfoHob, &mSetupData);
// AMI_OVERRIDE_END - EIP260096 Boot from Thunderbolt device fail

  if (!EFI_ERROR (Status) && mSetupData.TbtSupport) {
    //
    // Reserve TBT_DEBUG_AREA_SIZE_IN_KB buffer of system memory to store Tbt Debug data.
    //
    gTbtDebugBaseAddressMem = 0xFFFFFFFF;
    Status = gBS->AllocatePages (
                    AllocateMaxAddress,
                    EfiReservedMemoryType,
                    EFI_SIZE_TO_PAGES (TBT_DEBUG_AREA_SIZE_IN_KB*1024),
                    &gTbtDebugBaseAddressMem
                    );
    ASSERT_EFI_ERROR(Status);

    //
    //  Initialize TBT Debug Area to lower case 'x'
    //
    SetMem ((VOID *)(UINTN) gTbtDebugBaseAddressMem, TBT_DEBUG_AREA_SIZE_IN_KB*1024, 0x78);

    mSetupData.TbtDebugBaseAddressMem = (UINT32) gTbtDebugBaseAddressMem;
    DEBUG ((DEBUG_INFO, "<TbtDxe> TbtDebugBaseAddressMem = %lx \n", gTbtDebugBaseAddressMem));

//AMI_OVERRIDE_START - EIP322154 - Remove Thunderbolt DGPU related setup items
    TbtXConnectSupport = mSetupData.TbtXConnectSupport;
    mGlobalNvsAreaPtr->TbtXConnectSupport = mSetupData.TbtXConnectSupport;
    DEBUG ((DEBUG_INFO, "<TbtDxe> TbtXConnectSupport = %x \n", TbtXConnectSupport));
    DEBUG ((DEBUG_INFO, "<TbtDxe> TbtXConnectSupport = %x \n", mGlobalNvsAreaPtr->TbtXConnectSupport));
    if(TbtXConnectSupport) {
//AMI_OVERRIDE_END - EIP322154 - Remove Thunderbolt DGPU related setup items
        gTbtDgpuPciBaseAddressMem = 0xFFFFFFFF;
        Status = (gBS->AllocatePages) (
                        AllocateMaxAddress,
                        EfiReservedMemoryType,
                        EFI_SIZE_TO_PAGES (4*1024),
                        &gTbtDgpuPciBaseAddressMem
                        );
       ASSERT_EFI_ERROR(Status);

      //
      //Initialize with '0xFF'
      //
        SetMem ((VOID *)(UINTN)gTbtDgpuPciBaseAddressMem, 4*1024, 0xFF);
        mGlobalNvsAreaPtr->DGfxPciBaseAddressMem = (UINT32) gTbtDgpuPciBaseAddressMem;

        mSetupData.TbtDgpuPciBaseAddressMem = (UINT32) gTbtDgpuPciBaseAddressMem;
        DEBUG ((DEBUG_INFO, "<TbtDXE> TbtDgpuPciBaseAddressMem = %lx \n", gTbtDgpuPciBaseAddressMem));

      //
      // Tbt Device Tree buffer
      //

      Status = (gBS->AllocatePool) (
                          EfiReservedMemoryType,
                          256,
                          &MemoryBuffer
                          );
      ASSERT_EFI_ERROR(Status);

      gTbtDeviceTreeBuffer = (EFI_PHYSICAL_ADDRESS) MemoryBuffer;
      //
      //Initialize with '0x00'
      //
      SetMem ((VOID *)(UINTN)gTbtDeviceTreeBuffer, 256, 0x00);

      mGlobalNvsAreaPtr->DGfxTbtDeviceTreeBuffer = (UINT32) gTbtDeviceTreeBuffer;

      mSetupData.TbtDeviceTreeBuffer = (UINT32) gTbtDeviceTreeBuffer;
      DEBUG ((DEBUG_INFO, "<TbtDXE> TbtDeviceTreeBuffer = %lx \n", gTbtDeviceTreeBuffer));
    }
    Status = gRT->SetVariable (
                    L"Setup",
                    &gSetupVariableGuid,
                    SetupAttr,
                    DataSize,
                    &mSetupData
                    );
    ASSERT_EFI_ERROR(Status);
  }

  Status = EfiCreateEventReadyToBootEx (
            TPL_CALLBACK,
            TbtCallback,
            NULL,
            &Event
            );

// AMI_OVERRIDE_START - Add for OpRom support.
#if defined CSM_SUPPORT && CSM_SUPPORT == 1
  if (gTbtInfoHob->TbtSupport == 1) {
    Status = gBS->CreateEvent(EVT_NOTIFY_SIGNAL, TPL_CALLBACK,
                              DisableTbtDeviceRomCallback,
                              NULL, &CsmOpromPolicyEvent);
    if (!EFI_ERROR(Status)){
        Status = gBS->RegisterProtocolNotify(&gOpromStartEndProtocolGuid, CsmOpromPolicyEvent, &CsmOpromPolicyRegs);
    } else DEBUG ((DEBUG_INFO, "TbtDxe: Create disable TBT OPROM event fail!!!\n"));

    DEBUG ((DEBUG_INFO, "TbtDxe: Create disable TBT OPROM Callback %r!!!\n", Status));
    ASSERT_EFI_ERROR(Status);
  }
#endif
// AMI_OVERRIDE_END - Add for OpRom support.

//AMI_OVERRIDE_START >>> EIP342457 : TBT DXE must be dispatched before TBT SMM
  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gAmiTbtDxeInit,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );
//AMI_OVERRIDE_END <<< EIP342457 : TBT DXE must be dispatched before TBT SMM
  DataSize = sizeof (SA_SETUP);
  Status = gRT->GetVariable (
                  L"SaSetup",
                  &gSaSetupVariableGuid,
                  NULL,
                  &DataSize,
                  &SaSetup
                  );

  if (!EFI_ERROR (Status) && SaSetup.EnableVtd == FALSE) {
    //
    // Register an End of DXE event for extended a TPM log to PCR[7]
    //
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    TbtSecurityEndOfDxeCallBackFunction,
                    NULL,
                    &gEfiEndOfDxeEventGroupGuid,
                    &EndOfDxeEvent
                    );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Failed to egister an End of DXE event for extended a TPM log to PCR[7], Status: %d\n", Status));
      gBS->CloseEvent (EndOfDxeEvent);
      goto Exit;
    }
  }

  if (mSetupData.TbtVtdBaseSecurity == TRUE) {
    //
    // Register a Ready to Lock event for set DMAR Flag
    //
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    TbtSecurityReadyToLockCallBackFunction,
                    NULL,
                    &gEfiEndOfDxe2EventGuid,
                    &ReadyToLockEvent
                    );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Failed to Register a Ready to Lock event for set DMAR Flag, Status: %d\n", Status));
      gBS->CloseEvent (ReadyToLockEvent);
      goto Exit;
    }

    //
    // Register a Exit Boot Service for disable TBT BME
    //
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    TbtSecurityExitBootCallBackFunction,
                    NULL,
                    &gEfiEventExitBootServicesGuid,
                    &ExitBootServiceEvent
                    );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Failed to Register a Exit Boot Service for disable TBT BME, Status: %d\n", Status));
      gBS->CloseEvent (ExitBootServiceEvent);
      goto Exit;
    }

    //
    // Install TBT DisableBme for Shell testing purpose
    //
    InstallDisableBmeProtocol ();
  }
Exit:
  return Status;
}
