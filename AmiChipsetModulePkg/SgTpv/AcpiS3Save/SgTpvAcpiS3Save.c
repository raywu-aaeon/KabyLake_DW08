//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//**********************************************************************
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		SgTpvAcpiS3Save.c
//
// Description:	This file will save SG TPV related offset date for S3 resume restore.
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>
#include <Token.h>
#include <AmiCspLib.h>
#include <AmiDxeLib.h>
#include <Setup.h>
#include <Protocol/SaGlobalNvsArea.h>
#include <Protocol/BootScriptSave.h>
#include <NbSetupData.h>
#include <Library/PchInfoLib.h>

//----------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//----------------------------------------------------------------------------
// Constant Definition(s)
#define NVIDIA_VID              0x10DE
#define NVOPT_SSID_OFFSET       0x40

#define AMD_SVID_OFFSET         0x4C
#define AMD_SDID_OFFSET         0x4E
#define AMD_VID                 0x1002

SETUP_DATA            *SetupData = NULL;
PCH_SERIES            PchSeries;

 // Function Prototype(s)
VOID SgTpvAcpiS3Save (
  IN EFI_EVENT        Event,
  IN VOID             *Context
);

//----------------------------------------------------------------------------
// Variable and External Declaration(s)
//----------------------------------------------------------------------------
// Variable Declaration(s)
EFI_EVENT                       gEvent;
SYSTEM_AGENT_GLOBAL_NVS_AREA_PROTOCOL    *SaGlobalNvsArea;

// GUID Definition(s)
EFI_GUID gSetupGuid                      = SETUP_GUID;


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: SgTpvAcpiS3SaveInit
//
// Description: This function is the entry point for this DXE. This function
//              will save SG TPV related offset date for S3 resume restore.
//
// Input:       ImageHandle - Image handle
//              SystemTable - Pointer to the system table
//
// Output:      Return Status based on errors that occurred while waiting for
//              time to expire.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
EFIAPI
SgTpvAcpiS3SaveInit (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS          Status;
  UINTN              	VariableSize = 0;
  NB_SETUP_DATA       NbSetupData;    

  InitAmiLib(ImageHandle,SystemTable);
    
  // Initialize Setup Data 
  Status = GetEfiVariable(
             L"Setup",
             &gSetupGuid,
             NULL,
             &VariableSize,
             &SetupData
             );

  GetNbSetupData (pRS, &NbSetupData, FALSE);    
  
  // If Select SG mode
  if (NbSetupData.PrimaryDisplay == 4) {
    Status = pBS->CreateEventEx (
    	            EVT_NOTIFY_SIGNAL,
    	            TPL_CALLBACK,
    	            SgTpvAcpiS3Save,
    	            NULL,
    	            &gEfiEndOfDxeEventGroupGuid,
    	            &gEvent
    	            );
    ASSERT_EFI_ERROR(Status);
  }

  return EFI_SUCCESS;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   SgTpvAcpiS3Save
//
// Description: This callback function is called when ReadyToBoot is
//              installed.
//
// Input:       Event   - Event of callback
//              Context - Context of callback.
//
// Output:      EFI_SUCCESS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID SgTpvAcpiS3Save (
  IN EFI_EVENT        Event,
  IN VOID             *Context 
  )
{
  EFI_STATUS              Status;
  UINT16                  VendorId;
  UINT8                   EndpointBus;
  UINT16                  Data16;
  UINT32                  Data32;
  UINT16                  Count;
#if defined(NV_HDA_SUPPORT) && (NV_HDA_SUPPORT == 0)
  UINTN                   Index;
#endif  
  UINTN                   VariableSize = 0;
  UINT8                   RootPortDev;
  UINT8                   RootPortFun;
  //CPU_FAMILY              CpuFamilyId;

  EFI_BOOT_SCRIPT_SAVE_PROTOCOL   *BootScriptSave;

  DEBUG ((DEBUG_INFO, "==============SgTpvAcpiS3Save==========\n"));
  //
  //  Locate the SA Global NVS Protocol.
  //
  Status = pBS->LocateProtocol (
                  &gSaGlobalNvsAreaProtocolGuid,
                  NULL,
                  &SaGlobalNvsArea
                  );
  if (EFI_ERROR (Status)) {
    return ;
  }

  Status = pBS->LocateProtocol(
                  &gEfiS3SaveStateProtocolGuid,
                  NULL,
                  &BootScriptSave
                  );
  if (EFI_ERROR (Status)) {
    return ;
  }
    
  PchSeries  = GetPchSeries();    
  if (PchSeries == PchLp) {
    /// For SwitchableGraphics support the dGPU is present on PCH RootPort
    RootPortDev = SG_ULT_PORT_DEV;
    RootPortFun = SG_ULT_PORT_FUNC;
  } else {
    /// For SwitchableGraphics support the dGPU is present on PEG RootPort by default
    RootPortDev = SG_H_PORT_DEV;
    RootPortFun = SG_H_PORT_FUNC;	  
  }

  EndpointBus  = READ_PCI8 (0, RootPortDev, RootPortFun, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
  //
  //  Endpoint Device Not found
  //
  if (EndpointBus == 0xFF) { 
    return ;
  }

  VendorId = READ_PCI16 (EndpointBus, 0, 0, PCI_VENDOR_ID_OFFSET); 
  //
  //  if Device Not found or Devide is not AMD and Nvidia VGA device
  //
  if (VendorId == 0xFFFF || ((VendorId != NVIDIA_VID) && (VendorId != AMD_VID))) { 
    return ;
  }

  //
  // Store the Root port Bus assignemnt for S3 resume path
  //
  Data32 = READ_PCI32 (0, RootPortDev, RootPortFun, PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET);
  BOOT_SCRIPT_S3_MEM_WRITE_MACRO (
    BootScriptSave, 
    EfiBootScriptWidthUint32, 
    NB_PCIE_CFG_ADDRESS(0, RootPortDev, RootPortFun, PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET),
    1,
    &Data32
    );

  Data16 = READ_PCI16 (0, RootPortDev, RootPortFun, R_BASE_ADDRESS_OFFSET_3);
  BOOT_SCRIPT_S3_MEM_WRITE_MACRO (
    BootScriptSave, 
    EfiBootScriptWidthUint16, 
    NB_PCIE_CFG_ADDRESS(0, RootPortDev, RootPortFun, R_BASE_ADDRESS_OFFSET_3),
    1,
    &Data16
    );

  //
  // Store the Generic PCI config space of dGPU for S3 resume path 
  //

  for (Count = 4; Count < 0x40; Count+=4) {
    Data32 = READ_PCI32 (EndpointBus, 0, 0, Count);
    BOOT_SCRIPT_S3_MEM_WRITE_MACRO (
      BootScriptSave, 
      EfiBootScriptWidthUint32, 
      NB_PCIE_CFG_ADDRESS(EndpointBus, 0, 0, Count),
      1,
      &Data32
      );
  } 

  switch(VendorId){
    //
    // If dGPU card vendor is NVIDIA.
    //  
    case NVIDIA_VID:
      if (SetupData->SgMuxMode == 2) {  
        //
        // Set a unique SSID on the Nv Optimus Graphics
        //
        Data32 = SG_MUXLESS_SSVID_DID;
        WRITE_PCI32 (EndpointBus, 0, 0, NVOPT_SSID_OFFSET, Data32);
        
        SaGlobalNvsArea->Area->DgpuSsid = READ_PCI32 (EndpointBus, 0, 0, NVOPT_SSID_OFFSET); // DEBUG
        DEBUG ((DEBUG_INFO, "DgpuSsid in SgTpvAcpiS3Save =0x%X\n", SaGlobalNvsArea->Area->DgpuSsid));

        BOOT_SCRIPT_S3_MEM_WRITE_MACRO (
          BootScriptSave, 
          EfiBootScriptWidthUint32, 
          NB_PCIE_CFG_ADDRESS(EndpointBus, 0, 0, NVOPT_SSID_OFFSET),
          1,
          &Data32
          );

        Data16 = READ_PCI16(EndpointBus, 0, 0, PCI_COMMAND_OFFSET);
        Data16 = (Data16 & (!(BIT0+BIT1+BIT2)));
        Data16 = (Data16 | (BIT1+BIT2));
        WRITE_PCI16 (EndpointBus, 0, 0, PCI_COMMAND_OFFSET, Data16);

#if defined(NV_HDA_SUPPORT) && (NV_HDA_SUPPORT == 0)
        //
        // Disable Peg Endpoint Function 1
        //        
        Data32 = READ_PCI32 (EndpointBus, 0, 0, R_BASE_ADDRESS_OFFSET_0); // Resd Bar 0 MMIO address
        Data32 += 0x88488;
        Index = (UINT32)Data32;  
        *(UINT32*)(Data32) &= ~BIT25;       // Clear Bit 25
        Data32 = ((SaGlobalNvsArea->Area->SgFeatureList & 0x2) << 24); // dGPUAudioCodec bit2
        BOOT_SCRIPT_S3_MEM_WRITE_MACRO (
          BootScriptSave, 
          EfiBootScriptWidthUint32, 
          (UINTN)(Index),  
          1,
          &Data32
          );
#endif        
      }
      break;
    //
    // If dGPU card vendor is AMD.
    //
    case AMD_VID:
      //
      // Set a unique SSID on the AMD MXM
      //
      Data32 = SG_MUXLESS_SSVID_DID;        
      WRITE_PCI32 (EndpointBus, 0, 0, AMD_SVID_OFFSET, Data32);        
      SaGlobalNvsArea->Area->DgpuSsid = READ_PCI32 (EndpointBus, 0, 0, AMD_SVID_OFFSET); // DEBUG
      DEBUG ((DEBUG_INFO, "DgpuSsid in SgTpvAcpiS3Save =0x%X\n", SaGlobalNvsArea->Area->DgpuSsid));

      BOOT_SCRIPT_S3_MEM_WRITE_MACRO (
        BootScriptSave, 
        EfiBootScriptWidthUint32, 
        NB_PCIE_CFG_ADDRESS(EndpointBus, 0, 0, AMD_SVID_OFFSET),
        1,
        &Data32
        );

      break;
    default:
      //
      // either means the Device ID is not on the list of devices we know - we return from this function
      //
      break;
  }

  pBS->CloseEvent(Event);
  
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016 American Megatrends, Inc.          **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
