//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//*************************************************************************
// $Header: 
//
// $Revision: $
//
// $Date: 
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:        PlatformInfoPei.C
//
// Description: This file contains code for Platform Information initialization
//              in the PEI stage
//
//<AMI_FHDR_END>
//*************************************************************************

//----------------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------------
#include <Efi.h>
#include <Pei.h>
#include <Token.h>
#include <Library/DebugLib.h>
#include <AmiCspLib.h>
#include <Register/PchRegsLpc.h>
#include <PchAccess.h>
#include <SaRegs.h>
#include <Guid/AmiPlatformInfoHob.h>
#include <PlatformBoardId.h>
#include <Library/PchInfoLib.h>
#include <PlatformBoardType.h>
#include <PlatformInfo.h>

#define KSC_C_PORT        0x66

//----------------------------------------------------------------------------

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   PlatformInfoPeiInit
//
// Description: This function is the entry point for this PEI and detect platform 
//              information and create a GUID Hob storing it.
//
// Input:       FfsHeader   Pointer to the FFS file header
//              PeiServices Pointer to the PEI services table
//
// Output:      Return Status based on errors that occurred while waiting for
//              time to expire.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
EFIAPI
PlatformInfoPeiInit (
  IN EFI_PEI_FILE_HANDLE    FileHandle,
  IN CONST EFI_PEI_SERVICES       **PeiServices)
{
    EFI_STATUS          Status;
    PLATFORM_INFO       PlatformInfo;
    UINT16              McDeviceId;
    UINT16              LpcDeviceId;
    UINT16              LpcIoe;
    UINT16              LpcIoeOrg;
    EFI_HOB_GUID_TYPE   *Hob;
    PCH_SERIES          PchSeries;    
    AMI_OEM_PLATFORM_INFO AmiOemPlatformInfo = {0};
    
    Status = EFI_SUCCESS;
      
    //
    // Enable LPC IO decode temporarily for EC(KSC) access
    //
    LpcIoeOrg = MmioRead16 (PciD31F0RegBase + R_PCH_LPC_IOE);
    
    LpcIoe = LpcIoeOrg | B_PCH_LPC_IOE_ME1;
    
    MmioWrite16 (PciD31F0RegBase + R_PCH_LPC_IOE, LpcIoe);
    
    //
    // Get Platform Info and fill the Hob
    //
    PlatformInfo.RevisonId  = PlatformInfo_RevisonId;
    PlatformInfo.BoardID    = PlatformInfo_BoardId;
    PlatformInfo.BoardRev   = PlatformInfo_BoardRev;
    PlatformInfo.EcPresent  = FALSE;
    PlatformInfo.SpdPresent = TRUE;  
#ifdef MEM_DOWN_FLAG
    PlatformInfo.SpdPresent = FALSE;  
#endif
    
    PchSeries = GetPchSeries();
    if (PchSeries == PchH) {
      PlatformInfo.PlatformType = TypeTrad;   
    }
    else if (PchSeries == PchLp) {
      PlatformInfo.PlatformType = TypeUltUlx;
    }
    else {
      PlatformInfo.PlatformType = TypeUnknown;    
    }
    
#ifdef AMI_CRB_EC_SUPPORT_FLAG      
    // Read the EC Ports 0x62/0x66. If return value is other than 0xFF, set ECPresent=TRUE.
    if (IoRead8 (KSC_C_PORT) != 0xff) {
      PlatformInfo.EcPresent = TRUE;  
    }
#endif
    
    McDeviceId = READ_PCI16_NB (R_SA_MC_DEVICE_ID);
    LpcDeviceId = READ_PCI16_SB(PCI_DEVICE_ID_OFFSET);
    PlatformInfo.BoardType = BoardTypeRvp;
    
    if (IS_SA_DEVICE_ID_MOBILE(McDeviceId)) {
      //
      // Define Mobile Platform.
      //
      PlatformInfo.PlatformFlavor = FlavorMobile;
    }
    else if (IS_SA_DEVICE_ID_DESKTOP(McDeviceId) || IS_SA_DEVICE_ID_SERVER(McDeviceId)) {
      //
      // Define Desktop Platform.
      //
      PlatformInfo.PlatformFlavor = FlavorDesktop;
      
      if (IS_PCH_LPC_DEVICE_ID_SERVER(LpcDeviceId)) { 
        PlatformInfo.BoardType = BoardTypeUpServer;
        PlatformInfo.PlatformFlavor = FlavorUpServer;
      } else if (IS_PCH_LPC_DEVICE_ID_MOBILE(LpcDeviceId)) {
          PlatformInfo.PlatformFlavor = FlavorMobile;
      }
    }
    else {
      DEBUG ((DEBUG_ERROR, "Hang if unknown System Agent.\n"));      
#ifdef AMI_TMP_RM
      ASSERT (FALSE);
#endif
    }
    
    //
    // Restore LPC IO decode setting
    //
    MmioWrite16 (PciD31F0RegBase + R_PCH_LPC_IOE, LpcIoeOrg);
    
    //
    // Build HOB for setup memory information
    //
    Status = (*PeiServices)->CreateHob (
                               PeiServices,
                               EFI_HOB_TYPE_GUID_EXTENSION,
                               (UINT16) (sizeof (EFI_HOB_GUID_TYPE) + sizeof (PLATFORM_INFO)),
                               &Hob
                               );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Cannot Build HOB.\n"));        
      return Status;
    }
    
    Hob->Name = gPlatformInfoHobGuid;
    
    Hob++;
    
    AmiOemPlatformInfo.BoardId = PlatformInfo.BoardID;
    AmiOemPlatformInfo.SpdPresent = PlatformInfo.SpdPresent;
    
    if(AmiPlatformInfoPeiOverride (PeiServices, &AmiOemPlatformInfo)) {
    	PlatformInfo.BoardID = AmiOemPlatformInfo.BoardId;
#ifdef MEM_DOWN_FLAG    	
    	PlatformInfo.SpdPresent = AmiOemPlatformInfo.SpdPresent;
#endif    	
    }

    (*PeiServices)->CopyMem (Hob, &(PlatformInfo), sizeof (PLATFORM_INFO));
    
    return EFI_SUCCESS;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

