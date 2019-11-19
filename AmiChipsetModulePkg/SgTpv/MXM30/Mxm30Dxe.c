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
// $Header:        $
//
// $Revision:     $
//
// $Date:         $
//**********************************************************************
// Revision History
// ----------------
// $Log:         $
// 
//**********************************************************************
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:        Mxm30DXE.c
//
// Description:    This file contains the entry point of EFI MXM30 Support driver
//              The function installs MXM EFI, Legacy and Acpi interfaces if 
//              enabled in Setup
//              Installs Acpi Tables for Switchable Graphics
//
// Notes:        MAKE SURE NO PEI OR DXE SPECIFIC CODE IS NEEDED
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>
#include <Token.h>
#include <AmiDxeLib.h>
#include <Dxe.h>
#include <SbInc.h>
#include <Setup.h>
#include <Protocol/Mxm30.h>
#include <NbSetupData.h>

#if ( defined(CSM_SUPPORT) && (CSM_SUPPORT != 0) )
#include <Protocol/LegacyBios.h>
#include <Protocol/LegacyRegion.h>
#include <Protocol/LegacyBiosExt.h>
#endif

extern EFI_BOOT_SERVICES    *pBS;
extern EFI_RUNTIME_SERVICES *pRS;

VOID    *gMxmLegMemAddr        = NULL;
VOID    *gMxmData = NULL;
UINTN   gMxmDataSize = 0;

// extern functions
EFI_STATUS MXM30Interfaces (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE   *SystemTable);
EFI_STATUS NBCIInterfaces (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE   *SystemTable);

#if ( defined(CSM_SUPPORT) && (CSM_SUPPORT != 0) )
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   UpdateInt15MxMData
//
// Description: Get the address of MXM data buffer in legacy bios region
//
// Input:       EFI_LEGACY_BIOS_PROTOCOL            *LegacyBiosProtocol;
//       
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
UpdateInt15MxmSISData (
  IN EFI_LEGACY_BIOS_PROTOCOL         *LegacyBiosProtocol
  )
{
  EFI_STATUS  Status;
  EFI_IA32_REGISTER_SET RegSet;

  // Get the Data Pointer to MXM Structure within Int15 MXM handler
  MemSet(&RegSet, sizeof (EFI_IA32_REGISTER_SET), 0);
  RegSet.X.AX = 0x5f80;  
  RegSet.X.BX = 0xFF01;  
  RegSet.X.CX = EFI30_DataBlockID;  
  RegSet.X.ES = 0;
  RegSet.X.DI = 0;
  LegacyBiosProtocol->Int86(LegacyBiosProtocol, 0x15, &RegSet);    
    
  DEBUG ((DEBUG_INFO, "Mxm30Dxe::RegSet.X.AX ==%x\n", RegSet.X.AX));
  DEBUG ((DEBUG_INFO, "Mxm30Dxe::RegSet.X.ES ==%x\n", RegSet.X.ES));
  Status = (RegSet.X.AX == 0x5F80 || RegSet.X.ES == 0)? EFI_UNSUPPORTED : EFI_SUCCESS;
  ASSERT_EFI_ERROR(Status);
  DEBUG ((DEBUG_INFO, "Mxm30Dxe::RegSet.X.DI ==%x\n", RegSet.X.DI));
  //
  // return updated address
  //
  gMxmLegMemAddr = (UINT8*)((RegSet.X.ES << 4) + (RegSet.X.DI));

  // Expand the routine to obtain the address of _DOD buffer(MXM Int15 5f80, Func 9)

  return Status;
} 

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   LegacyBiosProtocolCallback
//
// Description: Get the address of MXM data buffer in legacy bios region
//
// Input:       EFI_LEGACY_BIOS_PROTOCOL            *LegacyBiosProtocol;
//       
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
VOID
EFIAPI
LegacyBiosProtocolCallback (
  IN EFI_EVENT Event, 
  IN VOID *Context
  )
{
  EFI_STATUS  Status;
  EFI_LEGACY_BIOS_EXT_PROTOCOL    *BiosExtensions;
  EFI_LEGACY_BIOS_PROTOCOL        *LegacyBiosProtocol;
  UINT32      LockUnlockAddr, LockUnlockSize;
  UINTN   MxmLegMemSize        = 0;

  Status = pBS->LocateProtocol(
                  &gEfiLegacyBiosProtocolGuid, 
                  NULL, 
                  &LegacyBiosProtocol
                  );
  DEBUG ((DEBUG_INFO, "==Mxm LegacyBiosProtocol %r====\n", Status));
  if (EFI_ERROR(Status)) { 
    return ;
  }

  Status = pBS->LocateProtocol(
                  &gEfiLegacyBiosExtProtocolGuid, 
                  NULL, 
                  &BiosExtensions
                  );
  DEBUG ((DEBUG_INFO, "==BiosExtensions %r====\n", Status));
  if (EFI_ERROR(Status)) { 
    return ;
  }
    
  gMxmLegMemAddr = (UINT8*)0xE0000;
  MxmLegMemSize = 0x20000;
    
  BiosExtensions->UnlockShadow(
                    (UINT8*)gMxmLegMemAddr, 
                    MxmLegMemSize,
                    &LockUnlockAddr, 
                    &LockUnlockSize
                    );

  //
  // Int15 may update gMxmLegMemAddr 
  // will copy MXM struct to the address returned by Int15 5F80, Func01
  //
  Status = UpdateInt15MxmSISData(LegacyBiosProtocol);
  DEBUG ((DEBUG_INFO, "==UpdateInt15MxmSISData  %r====\n", Status));       

  if (EFI_ERROR(Status)) { 
    BiosExtensions->LockShadow(
                      LockUnlockAddr, 
                      LockUnlockSize
                      );
    return ;
  }

  //
  // Copy the image into the shadow.
  //
  pBS->CopyMem(
         gMxmLegMemAddr, 
         gMxmData, 
         gMxmDataSize
         );
  gMxmData = gMxmLegMemAddr;
    
  BiosExtensions->LockShadow(
                    LockUnlockAddr, 
                    LockUnlockSize
                    );   
  return ;
}
#endif

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:    LoadFile
//
// Description:    Reads the file with the given GUID.
//
// Input:        
//        IN EFI_GUID     *Guid,
//        IN VOID         **Buffer
//        IN UINTN        *BufferSize
//                
// Output:        
//      EFI_STATUS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS LoadFile(
  EFI_GUID        *Guid,
  VOID            **Buffer,
  UINTN            *BufferSize
  )
{
  EFI_STATUS                        Status;
  UINTN                            NumHandles;
  EFI_HANDLE                        *HandleBuffer;
  EFI_FIRMWARE_VOLUME_PROTOCOL    *Fv;
  UINT32                            FvStatus;
  UINTN                            i;
  EFI_GUID    EfiFVProtocolGuid = EFI_FIRMWARE_VOLUME2_PROTOCOL_GUID;

  *Buffer = 0;
  *BufferSize = 0;

  Status = pBS->LocateHandleBuffer(
                  ByProtocol,
                  &EfiFVProtocolGuid,
                  NULL,
                  &NumHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR(Status)){ 
    return Status;
  }    

  for (i = 0; i < NumHandles; ++i) {
    Status = pBS->HandleProtocol(
                    HandleBuffer[i],
                    &EfiFVProtocolGuid,
                    &Fv
                    );
    if (EFI_ERROR(Status)){ 
      continue;
    }
    Status = Fv->ReadSection(
                   Fv,
                   Guid,
                   EFI_SECTION_RAW,
                   0,
                   Buffer,
                   BufferSize,
                   &FvStatus
                   );
    if (Status == EFI_SUCCESS){
      break;
    }
  }

  pBS->FreePool(HandleBuffer);
  return Status;
}
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   MxmDriverEntryPoint
//
// Description: Entry point of EFI MXM Support driver. The function istalls 
// 1. Install MXM protocol; 
// 2. Update data structure to be passed to Int15 binary
//      must support multiple MXM data modules. Dependent on controller's handle
//  Load MXM data block <1Mb area.
//      a. Copy MXM Structure below 1MB
//          1. Allocate Legacy mem space for MXM dat <1MB
//          OR          
//          2. Update the MXM data by the pointer returned by Int15 5F80 Func01(default)
//      b. Update the pointer to MXM data in EFI Protocol API
//      c. Create an ACPI SSDT table and copy MXM block there (ASL MXM logic must handle different MXM modules...)
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
MxmDriverEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  NB_SETUP_DATA   SetupData;
  EFI_GUID        EfiMxmStructGuid = MXMOEM1_GUID; 
#if defined(CSM_SUPPORT) && (CSM_SUPPORT != 0)
  EFI_EVENT       Event;
  VOID            *p;
#endif
  InitAmiLib(ImageHandle, SystemTable);
    
  DEBUG ((DEBUG_INFO, "==============MxmDriverEntryPoint============\n"));

  //
  // Check if the module is enabled in Setup; if not - return EFI_UNSUPPORTED
  //    
  GetNbSetupData (pRS, &SetupData, FALSE);

  //
  // Check to see if Primary Display is set to enable SG
  //
  if ((SetupData.PrimaryDisplay == 1)||  //PEG
      (SetupData.PrimaryDisplay == 3)||  //AUTO
      (SetupData.PrimaryDisplay == 4)) { //SG        
    // TBD. Proper Mxm structure block can be loaded based on matching dGPU's PCI Dev`ice/Vendor ID with the file Guid
    // e.g. EfiMxmStructGuid = Nvidia Mxm, AMD Mxm etc.
    // Also, multiple Mxm structure can be loaded if design supports 2 or more Mxm cards plugged in simultaneusly
    // Latter scenario will require MxmInt15.asm changes

    Status = LoadFile(
               &EfiMxmStructGuid, 
               &gMxmData, 
               &gMxmDataSize
               );
    DEBUG ((DEBUG_INFO, "==Load MxmData %r, Size %x====\n", Status, gMxmDataSize));
    if (EFI_ERROR(Status)) { 
      return Status;
    }
    
#if ( defined(CSM_SUPPORT) && (CSM_SUPPORT != 0) )
    gMxmLegMemAddr = NULL;
    LegacyBiosProtocolCallback (NULL, NULL);
    if (!gMxmLegMemAddr){
      RegisterProtocolCallback(
        &gEfiLegacyBiosProtocolGuid,
        LegacyBiosProtocolCallback,
        NULL,
        &Event,
        &p
        );
    }
#endif        
    //
    // Install NBCI & MXM3_EFI_PROTOCOL only if Primary Display is set to enable SG
    //
#if !defined(SGOEMSSDT_SUPPORT) || SGOEMSSDT_SUPPORT == 0
    if (SetupData.PrimaryDisplay == 4)   //SG
#endif
    {
      //	    
      // Install NBCI_EFI_PROTOCOL, Int15 5F80 handle
      //
      Status = NBCIInterfaces(ImageHandle, SystemTable);
      //
      // Install MXM_EFI_PROTOCOL, Int15 5F80 handle
      //
      Status = MXM30Interfaces(ImageHandle, SystemTable);
      ASSERT_EFI_ERROR (Status);
    }
  }            
  return Status;
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
