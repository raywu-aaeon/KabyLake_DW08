//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header: 
//
// $Revision: 1 $
//
// $Date: 
//**********************************************************************
// Revision History
// ----------------
// 
// 
//
// 
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  PreserveIPConfigVar.c.c
//
// Description:	To preserve IP4 Configuration variable data after flashing the bios.
//
//<AMI_FHDR_END>
//**********************************************************************

#include "PreserveIPConfigVar.h"

VOID *gAllMACsIp4Config2Data = NULL;
UINTN gIp4Config2DataVarSize = 0;
UINTN           MACListCount;
UINT16          *MACList;

/**
    Function to preserve IP4 Configuration variable data in a global variable of type UINT16*.

    @param IN VOID
    
    @retval OUT EFI_STATUS
*/ 
EFI_STATUS 
PreserveIP4ConfigurationVar ( 
  void
  )
{
    EFI_STATUS      Status;
    UINTN           MACListSize, MACStringIndex, IP4ConfigDataIndex, i; 
    VOID            *gIp4Config2Data;
    UINTN           gIp4Config2DataSize;
    
    EFI_GUID        Ip4Config2DataGuid = IP4_CONFIG2_NVDATA_GUID;
    CHAR16          Ip4Config2VarName[64] = {0};
    
    MACStringIndex     = 0;
    IP4ConfigDataIndex = 0;
    gIp4Config2Data    = 0;
    gIp4Config2DataSize= 1;
    i                  = 0;
     
    DEBUG_RAYDEBUG((-1, "\nPreserveIP4ConfigurationVar Entry 1"));
    if(pSmst == NULL)
    {
        DEBUG_RAYDEBUG((-1, "\nPreserveIP4ConfigurationVar: SMM system table not found"));
        return EFI_NOT_FOUND;
    }
    
    MACListSize = 1;
    Status = pSmst->SmmAllocatePool (
            EfiRuntimeServicesData,
            MACListSize, &MACList);
    if(NULL == MACList || EFI_ERROR(Status))
    {
        DEBUG_RAYDEBUG((-1, "\nPreserveIP4ConfigurationVar 1 SmmAllocatePool() failed"));
        return EFI_OUT_OF_RESOURCES;
    }
    
    Status = pRS->GetVariable(L"MACNamesListVar",
                              &MACNamesListVarDataGuid, 
                              NULL, 
                              &MACListSize, 
                              MACList);
    if (Status == EFI_BUFFER_TOO_SMALL)
    {
        if (MACList != NULL) {
            pSmst->SmmFreePool(MACList);
            MACList = 0;
        }
        Status = pSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    MACListSize, &MACList);

        // Get the Previous MAC List
        Status = pRS->GetVariable(L"MACNamesListVar",
                &MACNamesListVarDataGuid, 
                NULL, 
                &MACListSize, 
                MACList); 
    }
    MACListCount = MACListSize/MAC_STRING_SIZE;
    
    while( i++ < MACListCount )
    { 
        gIp4Config2DataSize = 1;
            Status = pSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    gIp4Config2DataSize, &gIp4Config2Data);
            if(NULL == gIp4Config2Data )
            {
                DEBUG_RAYDEBUG((-1, "\nPreserveIP4ConfigurationVar 2 SmmAllocatePool() failed"));
                return EFI_OUT_OF_RESOURCES;
            }
        CopyMem(Ip4Config2VarName, (CHAR16*)((UINT8*)MACList + MACStringIndex), MAC_STRING_SIZE);

        Status = pRS->GetVariable(Ip4Config2VarName, &Ip4Config2DataGuid, NULL, &gIp4Config2DataSize, gIp4Config2Data);

        if(Status == EFI_BUFFER_TOO_SMALL)
        {
            if(NULL != gIp4Config2Data)
                pSmst->SmmFreePool(gIp4Config2Data);
            Status = pSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    gIp4Config2DataSize + 1, &gIp4Config2Data);
            Status = pRS->GetVariable(Ip4Config2VarName, &Ip4Config2DataGuid, NULL, &gIp4Config2DataSize, gIp4Config2Data);
            
            if( Status == EFI_SUCCESS)
            {
                if( gAllMACsIp4Config2Data == NULL) // Allocate Memorty Only once
                {
                    Status = pSmst->SmmAllocatePool (
                                        EfiRuntimeServicesData,
                                        gIp4Config2DataSize * MACListCount, &gAllMACsIp4Config2Data);
                    if(NULL == gAllMACsIp4Config2Data || EFI_ERROR(Status))
                    {
                        DEBUG_RAYDEBUG((-1, "\nPreserveIP4ConfigurationVar 3 SmmAllocatePool() failed"));
                        return EFI_OUT_OF_RESOURCES;
                    }
                }
                CopyMem( (CHAR16*)((UINT8*)gAllMACsIp4Config2Data + IP4ConfigDataIndex), gIp4Config2Data, gIp4Config2DataSize);
                IP4ConfigDataIndex = IP4ConfigDataIndex + gIp4Config2DataSize;
                if( gIp4Config2DataVarSize == 0 ) // Assign only once
                  gIp4Config2DataVarSize = gIp4Config2DataSize;
            }
        }
        MACStringIndex = MACStringIndex + MAC_STRING_SIZE;
        
        if( gIp4Config2Data != 0 )
        {
            pSmst->SmmFreePool(gIp4Config2Data);
            gIp4Config2Data = NULL;
        }
    } // while()
 
    
    DEBUG_RAYDEBUG((-1, "\nPreserveIP4ConfigurationVar: Return status = %r\n",Status));
    return Status;
}

/**
    Function to restore IP4 Configuration variable data with values saved in global variable of type UINT16*.

    @param IN VOID
    
    @retval OUT EFI_STATUS
*/ 
EFI_STATUS 
RestoreIP4ConfigurationVar (
  void
  )
{
    EFI_STATUS      Status;   
    UINT32          Attributes;
    UINTN           MACStringIndex, IP4ConfigDataIndex, i;
    VOID            *gIp4Config2Data;
    CHAR16          Ip4Config2VarName[64] = {0};
    EFI_GUID        Ip4Config2DataGuid = IP4_CONFIG2_NVDATA_GUID;
    
    MACStringIndex      = 0;
    IP4ConfigDataIndex  = 0;
    gIp4Config2Data     = 0;
    i                   = 0;
    
#if RT_ACCESS_SUPPORT_IN_HPKTOOL
    Attributes = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS;
#else
    Attributes = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS;
#endif
    DEBUG_RAYDEBUG((-1, "\nRestoreIP4ConfigurationVar Entry"));

    if( gAllMACsIp4Config2Data != NULL )
    {
        while( i++ < MACListCount )
        {
            Status = pSmst->SmmAllocatePool (
                        EfiRuntimeServicesData,
                        gIp4Config2DataVarSize, &gIp4Config2Data);
            if(NULL == gIp4Config2Data || EFI_ERROR(Status))
            {
                DEBUG_RAYDEBUG((-1, "\nRestoreIP4ConfigurationVar: SmmAllocatePool() failed"));
                Status = EFI_OUT_OF_RESOURCES;
                goto ON_EXIT;
            }

            CopyMem(Ip4Config2VarName, (CHAR16*)((UINT8*)MACList + MACStringIndex), MAC_STRING_SIZE);
            CopyMem(gIp4Config2Data, (CHAR16*)((UINT8*)gAllMACsIp4Config2Data + IP4ConfigDataIndex), gIp4Config2DataVarSize);
            
            Status = pRS->SetVariable (Ip4Config2VarName, &Ip4Config2DataGuid, Attributes, gIp4Config2DataVarSize, gIp4Config2Data);
            
            IP4ConfigDataIndex = IP4ConfigDataIndex + gIp4Config2DataVarSize;
            MACStringIndex = MACStringIndex + MAC_STRING_SIZE;
            
            if( gIp4Config2Data != 0 )
            {
                pSmst->SmmFreePool(gIp4Config2Data);
                gIp4Config2Data = NULL;
            }
        } // while()
    } // if()

ON_EXIT:   
    if (MACList != NULL) {
        pSmst->SmmFreePool(MACList);
        MACList = 0;
    }
    
    if (gAllMACsIp4Config2Data != NULL) {
        pSmst->SmmFreePool(gAllMACsIp4Config2Data);
        MACList = 0;
    }
    
    return Status;
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
