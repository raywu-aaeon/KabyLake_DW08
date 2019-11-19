//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file AmiTbtDxeSmmLib.c
    Provide function for PCI Bus Driver to check which PCI device should assign resource or skip.

**/

#include <Token.h>
//AMI_OVERRIDE_START - EIP333764 - Enable TBT and disable "Skip Option Rom", but Graphic card is not workable with Legacy Mode
#include <IndustryStandard/Pci.h>
//AMI_OVERRIDE_END - EIP333764 - Enable TBT and disable "Skip Option Rom", but Graphic card is not workable with Legacy Mode
#include <PciBus.h>
#include <Library/DebugLib.h>
#include <AmiCspLib.h>
#include <Library/TbtCommonLib.h>
#include <Guid/HobList.h>
#include <Library/IoLib.h>
#include <Library/HobLib.h>
#include <Library/UefiLib.h>
#include <Library/AmiChipsetIoLib.h>
#include <Library/AmiTbtDxeSmmLib.h>
#include <Library/PchPcieRpLib.h>
#include <TbtBoardInfo.h>
#include <Protocol/GlobalNvsArea.h> // AMI_OVERRIDE EIP249248 - SW SMI for TBT boot in POST is abnormal

//AMI_OVERRIDE_START - EIP324304 - System assert at AMI USB when S4 resume with Thunderbolt USB device attached.
#include <Library/MmPciLib.h>
#include <Library/TimerLib.h>
//AMI_OVERRIDE_END - EIP324304 - System assert at AMI USB when S4 resume with Thunderbolt USB device attached.

//AMI_OVERRIDE_START >>> EIP360837 : Fix NVIDIA DGPU issue
#define PCI_DEVICE_MAX_BAR PCI_MAX_BAR
#define PCI_BRIDGE_MAX_BAR 2
//AMI_OVERRIDE_END <<< EIP360837 : Fix NVIDIA DGPU issue

EFI_GUID  gEfiGlobalNvsAreaProtocolGuid = EFI_GLOBAL_NVS_AREA_PROTOCOL_GUID; ///< AMI_OVERRIDE EIP249248 - SW SMI for TBT boot in POST is abnormal

//AMI_OVERRIDE_START - EIP324304 - System assert at AMI USB when S4 resume with Thunderbolt USB device attached.
/**
  This function contains PCIE2TBT <-> TBT2PCIE handshake
  procedure and all related methods called directly or underectly
  by TbtSetPCIe2TBTCommand.

  @param  Command               Thunderbolt FW setting command
  @param  HrBus                 Thunderbolt host location

  @retval TRUE                  Set Thunderbolt FW success
  @retval FALSE                 Set Thunderbolt FW fail
**/
STATIC
BOOLEAN
TbtSetPCIe2TbtCommand (
  IN  UINT8                     Command,
  IN  UINTN                     HrBus
  )
{
  UINTN                         TbtUs;
  UINTN                         Pcie2Tbt;
  UINTN                         Tbt2Pcie;
  UINT32                        RegVal;
  UINT32                        MaxLoopCount;
  UINTN                         Delay;
  UINT8                         RetCode;
  BOOLEAN                       Ret;

  TbtUs         = MmPciBase ((UINT32)HrBus, 0, 0);
  Pcie2Tbt      = TbtUs + PCIE2TBT_R;
  Tbt2Pcie      = TbtUs + TBT2PCIE_R;
  MaxLoopCount  = 10; // Wait 1 sec
  Delay         = 100 * 1000;
  RetCode       = 0x52;
  Ret           = FALSE;

  RegVal        = Command | PCIE2TBT_VLD_B;
  MmioWrite32 (Pcie2Tbt, RegVal);


  while (MaxLoopCount-- > 0) {
    RegVal = MmioRead32 (Tbt2Pcie);
    if (0xFFFFFFFF == RegVal) {
      //
      // Device is not here return now
      //
      RetCode = 0x5F;
      break;
    }

    if (RegVal & TBT2PCIE_DON_R) {
      RetCode = 0x51;
      Ret     = TRUE;
      break;
    }

    MicroSecondDelay (Delay);
  }

  MmioWrite32 (Pcie2Tbt, 0);
  return Ret;
}

/**
  Set the Sleep Mode if the HR is up.

  @param  HrBus                 The BUS of host router

  @return                       No return value
**/
STATIC
VOID
TbtSetSxMode (
  IN UINTN                      HrBus
  )
{
  UINTN                         TbtUs;
  UINTN                         TbtUsDevId;
  UINTN                         Tbt2Pcie;
  UINT32                        RegVal;
  UINT32                        MaxLoopCount;
  UINTN                         Delay;
  UINT8                         RetCode;

  TbtUs         = MmPciBase ((UINT32)HrBus, 0, 0);
  TbtUsDevId    = TbtUs;
  Tbt2Pcie      = TbtUs + TBT2PCIE_R;
  MaxLoopCount  = 150;  // Wait 15 sec
  Delay         = 100 * 1000;
  RetCode       = 0x62;

  if (!TbtSetPCIe2TbtCommand (PCIE2TBT_SX_EXIT_TBT_CONNECTED, HrBus)) {
    //
    // Nothing to wait, HR is not responsive
    //

    DEBUG ((DEBUG_INFO, "TbtDxe: HR is not responsive  (TbtSetSxMode) \n"));

    return;
  }

  DEBUG ((DEBUG_INFO, "TbtDxe: Wait for Dev ID != 0xFF\n"));
  IoWrite8(0x80, 0x60);

  while (MaxLoopCount-- > 0) {
    //
    // Check what HR still here
    //
    RegVal = MmioRead32 (Tbt2Pcie);
    if (0xFFFFFFFF == RegVal) {
      RetCode = 0x6F;
      break;
    }
    //
    // Check completion of TBT link
    //
    RegVal = MmioRead32 (TbtUsDevId);
    if (0xFFFFFFFF != RegVal) {
      RetCode = 0x61;
      break;
    }

    MicroSecondDelay (Delay);
  }

  DEBUG ((DEBUG_INFO, "TbtDxe: Return code = 0x%x\n", RetCode));
  IoWrite8(0x80, RetCode);
}
//AMI_OVERRIDE_END - EIP324304 - System assert at AMI USB when S4 resume with Thunderbolt USB device attached.


/**
  Invoke SW SMI befor boot for reaource adjust.

  @param  VOID                  No parameter

  @return                       No return value
**/
VOID
TbtDxeInvokeSmiHandler (
  VOID
  )
{
    EFI_STATUS                  Status;
    TBT_INFO_HOB                *gTbtInfoHob;
    EFI_PCI_IO_PROTOCOL         *PciIo;
    PCI_DEV_INFO                *Device, *Parent;
    EFI_HANDLE                  *HandleBuffer;
//AMI_OVERRIDE_START >>> EIP349739 : Provide gAmiThunderboltInfoProtocolGuid into Kabylake RC sample package
    EFI_HANDLE                  Handle = NULL;
//AMI_OVERRIDE_END <<< EIP349739 : Provide gAmiThunderboltInfoProtocolGuid into Kabylake RC sample package
    AMI_TBT_HR_STATUS_DATA      TbtHrStatusData;
    UINTN                       TbtHrStatusSize = sizeof(AMI_TBT_HR_STATUS_DATA);
    UINTN                       HandleCount;
    UINTN                       Index, ParentAddr;
    UINTN                       HostDev, HostFun;
    UINT16                      RegVal;
    UINT8                       i;
    UINT8                       k;// AMI_OVERRIDE - Black screen&hang up when TBT card and AMD Video card are connect with CSM disable - EIP242895
    UINT8                       PciOffset;
    UINT64                      PciMmBase;
    INT8                        PciClass;
//AMI_OVERRIDE_START >>> EIP360837 : Fix NVIDIA DGPU issue
    UINT8                       MaxBar;
//AMI_OVERRIDE_END <<< EIP360837 : Fix NVIDIA DGPU issue
//AMI_OVERRIDE_START - EIP318487 - Avoid TBT initialization during POST using Global eLink, ANY_PCI_DEV_CALLBACK
    EFI_GUID                    gEfiPciIoProtocolGuid = EFI_PCI_IO_PROTOCOL_GUID;
//AMI_OVERRIDE_END - EIP318487 - Avoid TBT initialization during POST using Global eLink, ANY_PCI_DEV_CALLBACK
//AMI_OVERRIDE_START - EIP324304 - System assert at AMI USB when S4 resume with Thunderbolt USB device attached.
    EFI_BOOT_MODE               BootMode;
    UINTN                       HrBus;
//AMI_OVERRIDE_END - EIP324304 - System assert at AMI USB when S4 resume with Thunderbolt USB device attached.
//AMI_OVERRIDE_START - EIP333764 - Enable TBT and disable "Skip Option Rom", but Graphic card is not workable with Legacy Mode
    UINT64                      PciMmBaseUpper;
//AMI_OVERRIDE_END - EIP333764 - Enable TBT and disable "Skip Option Rom", but Graphic card is not workable with Legacy Mode
//AMI_OVERRIDE_START >>> EIP349739 : Provide gAmiThunderboltInfoProtocolGuid into Kabylake RC sample package
    EFI_GUID                    TbtHookDoneGuid = {0x12124545, 0x033c, 0x4783, 0x55, 0x2b, 0x84, 0xab, 0x74, 0xa6, 0x97, 0xb7};
//AMI_OVERRIDE_END <<< EIP349739 : Provide gAmiThunderboltInfoProtocolGuid into Kabylake RC sample package
    EFI_GLOBAL_NVS_AREA_PROTOCOL  *GlobalNvsAreaProtocol; // AMI_OVERRIDE EIP249248 - SW SMI for TBT boot in POST is abnormal
    DEBUG ((DEBUG_INFO, "TbtDxe: TbtDxeInvokeSmiHandler\n"));
    Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **) &gTbtInfoHob);
    DEBUG ((DEBUG_INFO, "TbtDxe: Get Hob list %r\n", Status));
    if (!EFI_ERROR (Status) && (gTbtInfoHob != NULL)) {
      gTbtInfoHob = GetNextGuidHob (&gTbtInfoHobGuid, gTbtInfoHob);
      if (gTbtInfoHob == NULL) {
        DEBUG ((DEBUG_INFO, "TBT HOB not found\n"));
        return;
      }
    } else {
      return;
    }

    if (gTbtInfoHob->TbtSupport == 1){
//AMI_OVERRIDE_START - EIP324304 - System assert at AMI USB when S4 resume with Thunderbolt USB device attached.
      //
      // Get TBT host location
      //
      Status = GetTbtRpDevFun((UINTN)(gTbtInfoHob->TbtSelector - 1), &HostDev, &HostFun);

//AMI_OVERRIDE_START >>> EIP360837 : Fix NVIDIA DGPU issue
      BootMode = GetBootMode ();
//AMI_OVERRIDE_END <<< EIP360837 : Fix NVIDIA DGPU issue

      //
      // Sx mode at BDS because
      // 1. we skip AMI PCI Bus from TBT
      // 2. due to (1), USB driver private is not update after TBT SMI.
      //
      // This solution is used to bypass SMM buffer security issue from USB module
      //
      if ((gTbtInfoHob->TbtUsbOn == 0) && (gTbtInfoHob->TbtBootOn == 0)) {
        if (BootMode == BOOT_ON_S4_RESUME) {

          //
          // Get Thunderbolt Upstream Port
          //
          HrBus = MmioRead8 (MmPCIeAddress (0x00, HostDev, HostFun, PCI_SBUS));
          if (HrBus != 0) {
//AMI_OVERRIDE_START - EIP335678 - Remove ELINK requirement for AMI_ANY_DEV_HOOK_PCI_DEVICE_SKIP
            Status = pRS->GetVariable( AMI_TBT_HR_STATUS_VARIABLE, \
                                       &gAmiTbtHrStatusGuid, \
                                       NULL, \
                                       &TbtHrStatusSize, \
                                       &TbtHrStatusData );
            DEBUG ((DEBUG_INFO, "TbtDxe: Get AMI TBT variable %r before triggering TbtSMI\n", Status));
            if (!EFI_ERROR (Status) && TbtHrStatusData.TbtHrStatus) {
              //
              // No need to assign temp bus due to AMI PCI driver has already programmed Secondary Bus
              //
              TbtSetSxMode (HrBus);
            }
//AMI_OVERRIDE_END - EIP335678 - Remove ELINK requirement for AMI_ANY_DEV_HOOK_PCI_DEVICE_SKIP
          } else {
            DEBUG ((DEBUG_INFO, "TbtDxe: AMI PCI driver didn't program Secondary Bus\n"));
          }
        }
      }
//AMI_OVERRIDE_END - EIP324304 - System assert at AMI USB when S4 resume with Thunderbolt USB device attached.

// AMI_OVERRIDE_START EIP249248 - SW SMI for TBT boot in POST is abnormal
	// Update Thunderbolt SWSMI enumerate function number
        Status = pBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, &GlobalNvsAreaProtocol);
        DEBUG ((DEBUG_INFO, "TbtDxe: Locate EFI GLOBAL NVS AREA PROTOCOL %r \n", Status));
        GlobalNvsAreaProtocol->Area->ThunderboltSmiFunction = TBT_SWSMI_ENUMERATE_FUNCTION;
// AMI_OVERRIDE_END EIP249248 - SW SMI for TBT boot in POST is abnormal

//AMI_OVERRIDE_START >>> EIP369615 : Hot-plug can't workable on KBL-R when TBT FW mode is native with RTD3 support
        DEBUG ((DEBUG_INFO, "TbtDxe: Thunderbolt Feature is enable, trigger TbtSMI\n"));
        IoWrite8 (SW_SMI_IO_ADDRESS, TBT_SWSMI_VALUE); //trigger SwSMI for Thunderbolt
//AMI_OVERRIDE_END <<< EIP369615 : Hot-plug can't workable on KBL-R when TBT FW mode is native with RTD3 support

//AMI_OVERRIDE_START - EIP335678 - Remove ELINK requirement for AMI_ANY_DEV_HOOK_PCI_DEVICE_SKIP
//        Status = pRS->GetVariable( AMI_TBT_HR_STATUS_VARIABLE, \
//                                   &gAmiTbtHrStatusGuid, \
//                                   NULL, \
//                                   &TbtHrStatusSize, \
//                                   &TbtHrStatusData );
//        DEBUG ((DEBUG_INFO, "TbtDxe: Get AMI TBT variable %r after triggering TbtSMI\n", Status));
//AMI_OVERRIDE_END - EIP335678 - Remove ELINK requirement for AMI_ANY_DEV_HOOK_PCI_DEVICE_SKIP
        //if(!EFI_ERROR(Status) && TbtHrStatusData.TbtHrStatus){

            // Get all PCI IO protocols
            Status = pBS->LocateHandleBuffer( ByProtocol,
                                              &gEfiPciIoProtocolGuid,
                                              NULL,
                                              &HandleCount,
                                              &HandleBuffer);
            if (!EFI_ERROR(Status)) {
                DEBUG ((DEBUG_INFO, "TbtDxe: Get all PCI IO Protocols %r Number of handle: %x\n", Status, HandleCount));
                for (Index = 0; Index < HandleCount; Index++){
                    Status = pBS->HandleProtocol( HandleBuffer[Index],
                                                  &gEfiPciIoProtocolGuid,
                                                  (VOID **) &PciIo );

                    if (!EFI_ERROR(Status)) {
                        Device = (PCI_DEV_INFO*)PciIo;
                        DEBUG ((DEBUG_INFO, "\nTbtDxe: Handle[%x]->B:%x|D:%x|F:%x\n", Index, Device->Address.Addr.Bus, Device->Address.Addr.Device, Device->Address.Addr.Function));

                        if ((Device->Type != tPciRootBrg) && (Device->ParentBrg != NULL) && (Device->Address.Addr.Bus != 0)){
                            Parent = Device->ParentBrg;
                            ParentAddr = MmPCIeAddress(Parent->Address.Addr.Bus, Parent->Address.Addr.Device, Parent->Address.Addr.Function, PCI_PBUS);
                            RegVal = (MMIO_READ16(ParentAddr) & 0xFF00) >> 8;
                            DEBUG ((DEBUG_INFO, "TbtDxe: |-->Parent @ B:%x|D:%x|F:%x SubBus:%x\n", Parent->Address.Addr.Bus, Parent->Address.Addr.Device, Parent->Address.Addr.Function, RegVal));
                            if(Device->Address.Addr.Bus != (UINT8)RegVal){
                                Device->Address.Addr.Bus = (UINT8)RegVal;
                            }

                            // Update MMIO base address in the PCI_BAR under PCI_DEV_INFO on devices.
                            DEBUG ((DEBUG_INFO, "TbtDxe: Start to update MMIO base addresses\n"));

//AMI_OVERRIDE_START - EIP333764 - Enable TBT and disable "Skip Option Rom", but Graphic card is not workable with Legacy Mode
                            //Get Base Class Code
                            Status = PciIo->Pci.Read (
                                                  PciIo,
                                                  EfiPciIoWidthUint8,
                                                  PCI_BCC,
                                                  1,
                                                  &PciClass
                                                  );

//AMI_OVERRIDE_START >>> EIP360837 : Fix NVIDIA DGPU issue
                            MaxBar = (PciClass == PCI_CLASS_BRIDGE) ? PCI_BRIDGE_MAX_BAR : PCI_DEVICE_MAX_BAR;

                            //
                            // Class Code == PCI_CLASS_BRIDGE: Bridge >> Update BAR0 ~ BAR1 & Expansion ROM (offset 38h) to AMI PCI_DEV_INFO global data
                            // Class Code != PCI_CLASS_BRIDGE: Device >> Update BAR0 ~ BAR5 & Expansion ROM (offset 30h) to AMI PCI_DEV_INFO global data
                            //
                            for (i = 0; i <= MaxBar ; i++) {
                              PciMmBaseUpper = 0;
                              PciMmBase = 0;

                              if (i == MaxBar)
                              {
                                PciOffset = (PciClass == PCI_CLASS_BRIDGE) ? PCI_BRIDGE_ROMBAR : PCI_EXPANSION_ROM_BASE;
                              } else {
                                //
                                // BAR is 32-bit Register
                                //
                                PciOffset = PCI_BASE_ADDRESSREG_OFFSET + (i * sizeof (UINT32));
                              }

                              //
                              // Get PCI BAR base address
                              //
                              Status = PciIo->Pci.Read (
                                                    PciIo,
                                                    EfiPciIoWidthUint32,
                                                    PciOffset,
                                                    1,
                                                    &PciMmBase
                                                    );

                              DEBUG ((DEBUG_INFO, "\n TbtDxe: MMIO Bar[%2X] address  0x%2X\n", i, PciMmBase));

                              if ((PciMmBase & BIT0) == 0 &&
                                  (PciMmBase & BIT2) == BIT2 &&
                                  i != MaxBar) {
                                //
                                // PCI BAR is not IO BAR.
                                // PCI BAR is memory BAR and 64-bit wide.
                                // Not Expansion ROM
                                //
                                Status = PciIo->Pci.Read (
                                                      PciIo,
                                                      EfiPciIoWidthUint32,
                                                      (PciOffset + sizeof (UINT32)),
                                                      1,
                                                      &PciMmBaseUpper
                                                      );

                                PciMmBase = ((PciMmBaseUpper << 32) | PciMmBase);

                                DEBUG ((DEBUG_INFO, "\n TbtDxe: MMIO Bar[%2X] and MMIO Bar[%2X] is combined to  0x%2X\n", i, (i+1), PciMmBase));
                              }

                              if (PciClass != PCI_CLASS_BRIDGE && i != MaxBar) {
                                if ((PciMmBase & BIT0) == BIT0) {
                                  //
                                  // IO BAR: BIT0 & BIT1 are flag & Reserved respectively
                                  //
                                  PciMmBase = PciMmBase & 0x00000000FFFFFFFC;
                                } else {
                                  //
                                  // Memory BAR: BIT0 ~ 3 are flags
                                  //
                                  PciMmBase = PciMmBase & 0xFFFFFFFFFFFFFFF0;
                                }
                              }

                              //
                              // Expansion ROM BAR: BIT0~10 are Reserved
                              //
                              if (i == MaxBar) {
                                PciMmBase = PciMmBase & 0x00000000FFFFF800;
                              }

                              if (PciMmBaseUpper != 0) {
                                //
                                //  PCI BAR index need jump to next index due to 64-bit address using two 32-bit PCI BAR
                                //
                                i++;
                              }

//AMI_OVERRIDE_END - EIP333764 - Enable TBT and disable "Skip Option Rom", but Graphic card is not workable with Legacy Mode
// AMI_OVERRIDE_START - Black screen&hang up when TBT card and AMD Video card are connect with CSM disable - EIP242895
                              for(k = 0; k <= PCI_MAX_BAR; k++){
                                if(Device->Bar[k].Offset == PciOffset){
                                  if (Device->Bar[k].Base != PciMmBase) {
                                    DEBUG ((DEBUG_INFO, "\n TbtDxe: PCI_DEV_INFO.Bar[%2X] value 0x%2X; need to update\n", k, Device->Bar[k].Base));

                                    Device->Bar[k].Base = PciMmBase;

                                    DEBUG ((DEBUG_INFO, "\n TbtDxe: PCI_DEV_INFO.Bar[%2X] value is updated to 0x%2X \n", k, Device->Bar[k].Base));
                                  }
                                  break;
                                }
                              }
// AMI_OVERRIDE_END - Black screen&hang up when TBT card and AMD Video card are connect with CSM disable - EIP242895

//AMI_OVERRIDE_END <<< EIP360837 : Fix NVIDIA DGPU issue
                            }//for(i = 0; i <= 0x6 ; i++)
                        }
                        //if (Device->Address.Addr.Bus == 6) Device->Address.Addr.Bus = 0x39;
                        //if (Device->Address.Addr.Bus == 7) Device->Address.Addr.Bus = 0x3A;
                        //if (Device->Address.Addr.Bus == 8) Device->Address.Addr.Bus = 0x3B;
                    } //  end of if
                } // end of for loop
            } // end of if
        //}
//AMI_OVERRIDE_START >>> EIP349739 : Provide gAmiThunderboltInfoProtocolGuid into Kabylake RC sample package
      pBS->InstallProtocolInterface (&Handle, &TbtHookDoneGuid, EFI_NATIVE_INTERFACE, NULL);
//AMI_OVERRIDE_END <<< EIP349739 : Provide gAmiThunderboltInfoProtocolGuid into Kabylake RC sample package
    }
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
