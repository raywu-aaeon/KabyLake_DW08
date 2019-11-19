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
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		SgTpvDXE.c
//
// Description:	This file contains the entry point of SgTpv Support driver
//
// Notes:		MAKE SURE NO PEI OR DXE SPECIFIC CODE IS NEEDED
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>
//
// External include files do NOT need to be explicitly specified in real EDKII
// environment
//
#include <Efi.h>
#include <Token.h>
#include <Dxe.h>
#include <AmiDxeLib.h>
#include <AmiHobs.h>
#include <Setup.h>
#include <SaAccess.h>
#if ( defined(CSM_SUPPORT) && (CSM_SUPPORT != 0) )
#include <Protocol/LegacyBios.h>
#endif
#include <Protocol/SaPolicy.h>
#include <Library/UefiLib.h>
#include <Private/SaConfigHob.h>
#include <Guid/HobList.h>
#include <Library/HobLib.h>
#include <Library/PchInfoLib.h>
#include <Library/AmiChipsetIoLib.h>
#include <CommonDefinitions.h>

#define AMD_VID     0x1002
#define NVIDIA_VID  0x10DE

#define EFI_TPL_CALLBACK            8

SA_CONFIG_HOB         *SaConfigHob;
PCH_SERIES            PchSeries;
UINT8                 RootPortDev;
UINT8                 RootPortFun;
SETUP_DATA            SetupDataBuffer;

//
// External Declaration(s)
//
extern EFI_STATUS CountTime (
  IN UINTN            DelayTime, 
  IN UINT16           BaseAddr
  );

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   SgTpvDxeInitCallback
//
// Description: Update  DxePlatformSaPolicy with SgTpv settings.
//
// Input:       EFI_EVENT           Event,
//              VOID     *Context
//
// Output:      VOID
//
// Modified:
//
// Referrals:
//
// Notes:
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID
EFIAPI 
SgTpvDxeInitCallback (
  IN EFI_EVENT Event, 
  IN VOID *Context
  )
{
  EFI_STATUS                      Status;
  SA_POLICY_PROTOCOL              *DxePlatformSaPolicy;
  VBIOS_DXE_CONFIG                *VbiosDxeConfig;    
  UINT8                           EndpointBus;
  UINT32                          Attributes = 0;
  EFI_GUID                        gSetupGuid = SETUP_GUID; 
  UINTN                           VariableSize = 0;
  EFI_HANDLE                      Handle = NULL;

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
  DEBUG ((DEBUG_INFO, " SgTpvDxe::dGPU Rootport info[B/D/F] : [0x00/0x%x/0x%x]\n", RootPortDev, RootPortFun));  
    
  //
  // Locate the SG Policy Protocol
  //
  Status = pBS->LocateProtocol (
                  &gSaPolicyProtocolGuid,
                  NULL,
                  &DxePlatformSaPolicy
                  );  
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, " Locate DxePlatformSaPolicy Protocol.. %r\n",Status));  
    return;
  }
  
  Status = GetConfigBlock ((VOID *)DxePlatformSaPolicy, &gVbiosDxeConfigGuid, (VOID *)&VbiosDxeConfig);
  ASSERT_EFI_ERROR (Status);    
    
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **) &SaConfigHob);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, " SgTpvDxe:: GetSystem Configuration Table..%r\n",Status));   
    return;
  }

  SaConfigHob = GetNextGuidHob (&gSaConfigHobGuid, SaConfigHob);
  if (SaConfigHob == NULL) {
    DEBUG ((DEBUG_ERROR, " SgTpvDxe::Get Sa Data Hob.. %r\n",Status));   
    return;
  }
    
  pBS->CloseEvent (Event);
    
  //
  // Update SG DXE Policy -
  //
  VbiosDxeConfig->LoadVbios = LOADVBIOS;
  VbiosDxeConfig->ExecuteVbios = EXECUTEVBIOS;
#if (defined(SGOEMSSDT_SUPPORT) && (SGOEMSSDT_SUPPORT != 0))
  VbiosDxeConfig->ExecuteVbios = 0;
#endif
  //
  // PEG Endpoint Base Addresses and Capability Structure Offsets for ASL usage
  //	
  WRITE_PCI32 (0, RootPortDev, RootPortFun, PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET, 0x00010100);
  WRITE_PCI16 (1, 0, 0, PCI_VENDOR_ID_OFFSET, 0);
  CountTime(4000, PM_BASE_ADDRESS); // 1ms
	
  EndpointBus  = READ_PCI8 (0, RootPortDev, RootPortFun, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
  DEBUG ((DEBUG_INFO, " SgTpvDxe::EndpointBus = %x,TPV card VID = %x \n",EndpointBus,READ_PCI16 (EndpointBus,0, 0, PCI_VENDOR_ID_OFFSET)));
  if ((EndpointBus == 0xff) ||
         ((READ_PCI16 (EndpointBus,0, 0, PCI_VENDOR_ID_OFFSET) != NVIDIA_VID) && 
          (READ_PCI16 (EndpointBus,0, 0, PCI_VENDOR_ID_OFFSET) != AMD_VID))
        )
    {
	  DEBUG ((DEBUG_INFO, " SgTpvDxe::nV or ATI dGPU device doesn't exist,disable SG mode..\n"));
      SaConfigHob->SaRtd3.SgMode = SgModeDisabled;
    } else {
    	  DEBUG ((DEBUG_INFO, " SgTpvDxe::Found nV or ATI dGPU device..\n"));	    
      if (EndpointBus != 0xff && 
            READ_PCI16 (EndpointBus,0, 0, PCI_VENDOR_ID_OFFSET) == NVIDIA_VID) 
      {
        VbiosDxeConfig->ExecuteVbios = 0;
      }

      VariableSize  = sizeof (SETUP_DATA);
      Status = pRS->GetVariable (
                      L"Setup",
                      &gSetupGuid,
                      &Attributes,
                      &VariableSize,
                      &SetupDataBuffer
                      );
      DEBUG ((DEBUG_INFO, " SgTpvDxe::Attributes = %x \n",Attributes));
#if (defined(CSM_SUPPORT) && (CSM_SUPPORT != 0))
      VariableSize = sizeof(EFI_HANDLE);
      Status = pBS->LocateHandle (
                      ByProtocol, 
                      &gEfiLegacyBiosProtocolGuid, 
                      NULL, 
                      &VariableSize, 
                      &Handle
                      );  
      if (EFI_ERROR(Status)){
        VbiosDxeConfig->ExecuteVbios = 0;
      }
#else
      VbiosDxeConfig->ExecuteVbios = 0;
#endif 
/*        if (EndpointBus != 0xff && 
                READ_PCI16 (EndpointBus,0, 0, PCI_VID) == AMD_VID)               
        {
            SetupDataBuffer.TPVCardCheck = 1;
        } else
            SetupDataBuffer.TPVCardCheck = 0;

        Status = pRS->SetVariable (
                                L"Setup",
                                &gSetupGuid,
                                Attributes,
                                sizeof(SETUP_DATA),
                                &SetupDataBuffer
        );*/
    }
  WRITE_PCI32 (0, RootPortDev, RootPortFun, PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET, 0x00000000);  
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   SgTpvDxeEntryPoint
//
// Description: Entry point of EFI SG Support driver.
//
// Input:       EFI_HANDLE           ImageHandle,
//              EFI_SYSTEM_TABLE     *SystemTable
//
// Output:      EFI_STATUS
//
// Modified:
//
// Referrals:
//
// Notes:
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
EFIAPI
SgTpvDxeEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_EVENT       Event;
  VOID            *p;
  InitAmiLib(ImageHandle, SystemTable);
  //SgTpvDxeInitCallback (NULL, NULL);
    
  RegisterProtocolCallback(
    &gSaPolicyProtocolGuid,
    SgTpvDxeInitCallback,
    NULL,
    &Event,
    &p
    );
  
   return EFI_SUCCESS;
}

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
