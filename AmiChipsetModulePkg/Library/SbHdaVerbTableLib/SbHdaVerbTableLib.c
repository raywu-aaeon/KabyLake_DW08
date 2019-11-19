//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file SbHdaVerbTableLib.c
    This file contains code for South bridge HDA Verb Table Override  
    function

*/

// Module specific Includes
#include <Efi.h>
#include <Token.h>
#include <Setup.h>
#include <AmiCspLib.h>
#include <AmiPeiLib.h>
#include <SbSetupData.h>
#include <SbHdaVerbTableLib.h>
#include <Library/DebugLib.h>

OEM_HDA_VERB_TABLE_CONTENT
SB_HDA_VERB_TABLE            HdaVerbTbl[] = { OEM_HDA_VERB_TABLE };

typedef struct  {
  PCH_HDA_VERB_TABLE_HEADER  Header;  ///< Azalia codec verb table header.
  UINT32 Data[1];                     ///< Verb table data (variable size),
                                      ///< length must be specified in Header.DataDwords.
} PCH_HDA_VERB_TABLE_PPI;

EFI_STATUS
SbHdaVerbTableOverride (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN OUT SB_HDA_VERB_TABLE  **VerbTable,
    OUT UINT8                 *TotalVerbTableNumber
)
{   
    DEBUG((DEBUG_INFO, "SbHdaVerbTableOverride() Start\n"));
    *VerbTable = HdaVerbTbl;
    *TotalVerbTableNumber = sizeof(HdaVerbTbl) / sizeof(SB_HDA_VERB_TABLE);
    DEBUG((DEBUG_INFO, "SbHdaVerbTableOverride() End\n"));
    return EFI_SUCCESS;
}

VOID AmiInstallOemHdaVerbTables (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN SI_POLICY_PPI     	*SiPolicy
)
{   
    EFI_STATUS              Status;
    PCH_HDA_VERB_TABLE_PPI  *HdaVerbTablePpi;  
    SB_HDA_VERB_TABLE       *VerbTblSourcePtr = NULL;  
    UINT8                   Index, TotalVerbTableNumber; 
    UINT32                  NumOfData;    
    PCH_HDAUDIO_CONFIG      *HdAudioConfig;
    UINT32                  VerbTableArray[32];
    UINT32                  *VerbTablePtr;

    DEBUG((DEBUG_INFO, "AmiInstallOemHdaVerbTables() Start\n"));
    
    Status = GetConfigBlock ((VOID *) SiPolicy, &gHdAudioConfigGuid, (VOID *) &HdAudioConfig);
    if (EFI_ERROR(Status)) {
        DEBUG((DEBUG_ERROR, "Fail to get HdAudioConfig, Skip install verb table.\n"));
        return;
    }
    
    Status = SbHdaVerbTableOverride(PeiServices, &VerbTblSourcePtr, &TotalVerbTableNumber);
    DEBUG((DEBUG_INFO, "TotalVerbTableNumber = %x\n", TotalVerbTableNumber));
    if(EFI_ERROR(Status) || (TotalVerbTableNumber == 0)){
        DEBUG((DEBUG_ERROR, "AmiInstallOemHdaVerbTables() End\n"));
        return;
    } // end if
    
    for (Index = 0;Index < TotalVerbTableNumber;Index++) 
    {
        NumOfData = VerbTblSourcePtr[Index].VerbTableHeader.DataDwords;
        
        Status = (*PeiServices)->AllocatePool ((CONST EFI_PEI_SERVICES**)PeiServices, \
                                               (sizeof (PCH_HDA_VERB_TABLE_HEADER)) + (sizeof (UINT32) * NumOfData), \
                                               (VOID **)&HdaVerbTablePpi);
        if (EFI_ERROR(Status))
        {
            ASSERT_EFI_ERROR (Status);
        }
        
        (*PeiServices)->CopyMem (&HdaVerbTablePpi->Header, &VerbTblSourcePtr[Index].VerbTableHeader, sizeof (PCH_HDA_VERB_TABLE_HEADER));
        DEBUG((DEBUG_INFO, "HdaVerbTablePpi->Header = %x\n", HdaVerbTablePpi->Header));
        
        (*PeiServices)->CopyMem (&HdaVerbTablePpi->Data[0], VerbTblSourcePtr[Index].VerbPtr, sizeof (UINT32) * NumOfData);
        DEBUG((DEBUG_INFO, "HdaVerbTablePpi->Data[0] = %x\n", HdaVerbTablePpi->Data[0]));
        
#if defined (INTEL_DISPLAY_AUDIO_VERB_TABLE_SUPPORT) && (INTEL_DISPLAY_AUDIO_VERB_TABLE_SUPPORT == 1)
        if (HdaVerbTablePpi->Header.VendorId == 0x8086 && 
            (HdaVerbTablePpi->Header.DeviceId == 0x2809 || HdaVerbTablePpi->Header.DeviceId == 0x280B) ) 
        {
            if (HDMI_CODEC_PORT_B == 0) {
                HdaVerbTablePpi->Data[4] |= 0x40;
            }
            
            if (HDMI_CODEC_PORT_C == 0) {
                HdaVerbTablePpi->Data[8] |= 0x40;
            }
            
            if (HDMI_CODEC_PORT_D == 0) {
                HdaVerbTablePpi->Data[12] |= 0x40;
            }               
        }
#endif        
        VerbTableArray[Index] = (UINT32)&HdaVerbTablePpi->Header;
    } // end for loop
    
    HdAudioConfig->VerbTableEntryNum = TotalVerbTableNumber;
    
    Status = (*PeiServices)->AllocatePool ((CONST EFI_PEI_SERVICES**)PeiServices, sizeof (UINT32) * TotalVerbTableNumber, (VOID **)&VerbTablePtr);
    if (EFI_ERROR(Status))
    {    
        ASSERT_EFI_ERROR (Status);
    }
    (*PeiServices)->CopyMem (VerbTablePtr, VerbTableArray, sizeof (UINT32) * TotalVerbTableNumber);
    HdAudioConfig->VerbTablePtr = (UINT32) VerbTablePtr;
    
    DEBUG((DEBUG_INFO, "AmiInstallOemHdaVerbTables() End\n"));
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
