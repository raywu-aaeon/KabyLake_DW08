//***********************************************************************
//*                                                                     *
//*                      Copyright (c) 1985-2021, AMI.                  *
//*                                                                     *
//*      All rights reserved. Subject to AMI licensing agreement.       *
//*                                                                     *
//***********************************************************************


/** @file ResParPopupMenu.c
    This file implements adding RESTORE_PARTITION_POPUP_MENU_NAME item to the popup menu list.
**/

//---------------------------------------------------------------------------

#include "Token.h"

#if defined(RestorePartition_PopupMenu_SUPPORT) && RestorePartition_PopupMenu_SUPPORT


#include <Efi.h>
#include "bootflow.h"
#include <Protocol/LegacyBiosExt.h>
#include <Protocol/BlockIo.h>
#include <AmiDxeLib.h>
#include "boot.h"
#include "AMIVfr.h"
#include "Protocol/AMIPostMgr.h"
#include <Library/PrintLib.h>

extern BOOT_DATA    *gBootData;
extern UINT16       gBBSPopupSel;
extern UINTN        gPostStatus;
extern BOOLEAN      gPopupMenuShowAllBbsDev;
extern UINTN        gBootOptionCount;
extern UINT32       gBootFlow;
extern BOOLEAN      gEnterSetup;

extern VOID SearchRestorePartition(VOID);
extern VOID BootRestorePartition(VOID);
extern UINT8        gGptRpFound;
extern UINT8        gRpFound;
extern UINT8        gMbrRpFound;

extern BOOLEAN BootNowInBootOrderSupport();

#define POPUP_MENU_NAME_LEN sizeof(RESTORE_PARTITION_POPUP_MENU_NAME) - 2 // this doesn't includes 2 bytes of end of unicode string 
#define POPUP_MENU_NAME_SIZE sizeof(RESTORE_PARTITION_POPUP_MENU_NAME)

BOOLEAN     IsBootDataRestored = FALSE;
BOOLEAN     IsBootDataModified = FALSE;
UINT16      *ModBootOrder = NULL;

/**
    
    //
    // This function get the free BootOption number.
    //

    @param VOID

    @retval UINT16 usable BootOption number

**/

UINT16
GetFreeBootOptionNumber(){
    EFI_STATUS  Status;
    UINTN       Size = 0 , i = 0;
    UINT16      *BootOrder=NULL;
    UINT16      FreeBootOptionNumber = 0x0;
    CHAR16		BootStr[9];
	UINT8		*VarBuf = NULL;
    
    Status = GetEfiVariable(L"BootOrder", &gEfiGlobalVariableGuid, NULL, &Size, (VOID**)&BootOrder);
    if(EFI_ERROR(Status) || Size == 0 || BootOrder == NULL)
        return  (FreeBootOptionNumber + 1);
        
    for(i = 0;i < (Size/sizeof(UINT16)); i++){
        if(BootOrder[i] > FreeBootOptionNumber)
            FreeBootOptionNumber = BootOrder[i];
    }
    
	Size = 0;
	while(FreeBootOptionNumber < 0xFFFC){
	    FreeBootOptionNumber++; // new BootOption Number
        UnicodeSPrint(BootStr, sizeof(BootStr) ,L"Boot%04X",FreeBootOptionNumber);
		Status = GetEfiVariable(BootStr, &gEfiGlobalVariableGuid, NULL, &Size, (VOID**)&VarBuf);
		if(VarBuf){
		    pBS->FreePool(VarBuf);
			VarBuf = NULL;
		}
        if(Status != EFI_SUCCESS){
            // find the free BootOption number
            break;
        }
	}
	
    if(BootOrder)
        pBS->FreePool(BootOrder);
    
    return FreeBootOptionNumber;
}

/**
    
    //
    // This function check if RestorePartition is existed.
    //

    @param VOID

    @retval TRUE RestorePartion is found
    @retval FALSE RestorePartion is not found

**/

BOOLEAN
IsRestorePartitionExisted(){
    SearchRestorePartition();
    return (gGptRpFound || gRpFound || gMbrRpFound)? TRUE : FALSE;
}

/**
    
    // 
    // This function is to restore the BootData that was modified by RestorePartition
	//
	
    @param VOID

    @retval VOID

**/

VOID
RestoreBootData(VOID){
    EFI_STATUS  Status;
    EFI_GUID    BootNowCountGuid = BOOT_NOW_COUNT_GUID;
    UINT32      VarAttr = 0;
    UINTN       VarSize = sizeof(UINT16);
    UINT16      BootNowCount = 0;
    BOOT_DATA   TmpBootData;
    UINT16      *PopupMenuItemName = RESTORE_PARTITION_POPUP_MENU_NAME;
    UINTN       NameLen = POPUP_MENU_NAME_LEN;
    
    UINTN   Size = 0;
    UINT32  Attr = 0;
    UINT16  *BootOrder = NULL;
    UINTN   i = 0;
    BOOLEAN IsFound = FALSE;
    
    if(IsBootDataRestored){
        return;
    }
    
    if(!gBootOptionCount){
        return;
    }
    
	// restore gBootData and gBootOptionCount
    for(i = 0; i < gBootOptionCount; i++){
	    // search the ResParBootOption
        if(!MemCmp(PopupMenuItemName, gBootData[i].Name, NameLen)){
            MemCpy(&TmpBootData, &gBootData[i], sizeof(BOOT_DATA));
            IsFound = TRUE;
            
            // move remaining BootData forward
            for(i++;i < gBootOptionCount;i++){
                MemCpy(&gBootData[i-1], &gBootData[i], sizeof(BOOT_DATA));
            }
        }
    }
    
    if(!IsFound){
        // unexpected error!!
        return;
    }
    
    if(TmpBootData.Name){
        pBS->FreePool(TmpBootData.Name);
    }
    if(TmpBootData.DevicePath){
        pBS->FreePool(TmpBootData.DevicePath);
    }

    gBootOptionCount = gBootOptionCount - 1;    // remove ResParBootData

    // Because BootNowCount variable is updated(in DoBbsPopupInit()) after adding restore partition BootData. 
    Status = pRS->GetVariable(
                 L"BootNowCount",
                 &BootNowCountGuid,
                 &VarAttr,
                 &VarSize,
                 &BootNowCount
             );
    if(EFI_ERROR(Status)) return;
    
    BootNowCount = BootNowCount -1; // remove Restorepartition's 
    
    Status = pRS->SetVariable(
                 L"BootNowCount",
                 &BootNowCountGuid,
                 VarAttr,
                 VarSize,
                 &BootNowCount);

	// restore BootOrder variable.
    // note: only TSE_BOOT_NOW_IN_BOOT_ORDER is enabled.
    if(BootNowInBootOrderSupport()){
        Status = GetEfiVariable(L"BootOrder", &gEfiGlobalVariableGuid, &Attr, &Size, (VOID**)&BootOrder);
        if(EFI_ERROR(Status))
            return;
    
        IsFound = FALSE;
        for(i = 0;i < (Size/sizeof(UINT16));i++){
            if(BootOrder[i] == TmpBootData.Option){
                IsFound = TRUE;
                // move remaining BootOrder forward
                for(i++;i < (Size/sizeof(UINT16));i++){
                    BootOrder[i-1] = BootOrder[i];
                }
            }
        }
    
        if(IsFound){
            pRS->SetVariable (
                    L"BootOrder",
                    &gEfiGlobalVariableGuid,
                    Attr,
                    (Size - sizeof(UINT16)),    // remove RestorePartition's
                    BootOrder
                    );
        }
        if(BootOrder){
            pBS->FreePool(BootOrder);
        }
        if(ModBootOrder){
            pBS->FreePool(ModBootOrder);
            ModBootOrder = NULL;
        }
    }
    
    // update internal flag
    IsBootDataRestored = TRUE;
   
    return;
}

/**
    
    // 
    // This hook function will be executed when ESC is pressed in Popup Menu.
    // *called by DrawBbsPopupMenu() in minisetupext.c 
    //
    // This function will restore the original BootData.
    //

    @param VOID

    @retval VOID

**/

VOID 
RestorePartitionPopupMenuEsc(VOID){
    if( gPostStatus == TSE_POST_STATUS_IN_BBS_POPUP && 
          gBootFlow == BOOT_FLOW_CONDITION_BBS_POPUP ){
        if( IsBootDataModified && !IsBootDataRestored ){
            // Restore BootData
            RestoreBootData();
        }
    }
}

/**
    
    //
    // This function is to restore the original BootData when 
    // choose one boot item to boot from Popup Menu.
    //
    // note: if boot restore partition is failed, it will enter Setup.
    //

    @param VOID

    @retval VOID

**/

VOID 
RestorePartitionBbsBootHook(VOID)
{
    BOOT_DATA   *BootData = NULL;
    UINT16      *PopupMenuItemName = RESTORE_PARTITION_POPUP_MENU_NAME;
    UINTN       NameLen = POPUP_MENU_NAME_LEN;
    UINT16      *BootName = NULL;
    UINTN       i = 0, count = 0;
    BOOLEAN     IsFound = FALSE;
    
	// Only restore BootData when exit from BBS POPUP.
    if( gPostStatus != TSE_POST_STATUS_IN_BBS_POPUP ||
#if defined(TSE_BUILD) && TSE_BUILD <= 0x1242 && defined(TSE_MINOR) && TSE_MINOR < 0x16
        gBootFlow != BOOT_FLOW_CONDITION_BBS_POPUP
#else      
        gBootFlow != BOOT_FLOW_CONDITION_NORMAL
#endif
        ){
        return;
    }
    
    if( !IsBootDataModified || IsBootDataRestored ){
        return;
    }
    
    if(gEnterSetup){
        if( IsBootDataModified && !IsBootDataRestored ){
            // Restore BootData
            RestoreBootData();
            return;
        }
    }
    
    // get the BootData of selected boot item in popup menu
    count = gBBSPopupSel;
    if(gPopupMenuShowAllBbsDev){
        BootData = gBootData;
        for(i=0;i<gBBSPopupSel && count;i++){
            // if valid BBS device path?
            if(BootData->DevicePath->Type == 0x5 && BootData->DevicePath->SubType == 0x1){
                if(count < BootData->LegacyDevCount)
                    break;
                count -= BootData->LegacyDevCount;
            }
            else{
                count--;
            }
            BootData++;
        }
    }
    else{
        BootData = gBootData + gBBSPopupSel;
    }
    BootName = BootData->Name;

    // Check if BootData is created by RestorePartition
    if(!MemCmp(BootName, PopupMenuItemName, NameLen))
        IsFound = TRUE;
    
    // Restore BootData
    RestoreBootData();
    
    if(IsFound){
		// Previous have found where RestorePartition is(in RestorePartitionCreateBootData()). 
        // Directly boot RestorePartition...
        if(gGptRpFound || gRpFound || gMbrRpFound)
            BootRestorePartition();
    }
    
    // go back to BdsBootHook()
    
    return;
}

/**
    
    //
    // This function is to create the BootData for RestorePartition prior to
    // DoBbsPopupInit() collect BootData to create the items in Popup Menu.
    //

    @param VOID

    @retval VOID

**/

VOID
RestorePartitionCreateBootData(VOID){
    BOOT_DATA   *NewBootData = NULL, *BootDataPtr = NULL;
    EFI_STATUS  Status;
    UINTN       NameSize = POPUP_MENU_NAME_SIZE;
    UINT16      *NameBuf = NULL;
    UINT16      *PopupMenuItemName = RESTORE_PARTITION_POPUP_MENU_NAME;
    EFI_DEVICE_PATH_PROTOCOL    *DevPath = NULL;
	EFI_DEVICE_PATH_PROTOCOL	EndOfDp = { 0x7F, 0xFF, {0x4 , 0x0} };
	
    UINTN   Size = 0;
    UINT32  Attr = 0;
    UINT16  *BootOrder = NULL;

    if( gPostStatus != TSE_POST_STATUS_IN_BBS_POPUP || 
          gBootFlow != BOOT_FLOW_CONDITION_BBS_POPUP ){
        return;
    }
    
    if( IsBootDataModified ){
        return;
    }
    
    // Don't create RestorePartition item if RestorePartition is not existed.
    if( !IsRestorePartitionExisted() ){
        return;
    }
    
    // Allocate new buffer and copy original gBootData
    Status = pBS->AllocatePool( EfiBootServicesData, sizeof(BOOT_DATA) * (gBootOptionCount + 1) , (VOID**)&NewBootData );
    if(EFI_ERROR(Status)){
        goto error_exit;
    }
    pBS->SetMem(NewBootData,sizeof(BOOT_DATA) * (gBootOptionCount + 1), 0x0);
    MemCpy(NewBootData, gBootData, sizeof(BOOT_DATA)*gBootOptionCount);
    
    // Move to the last BootData
    BootDataPtr = NewBootData + gBootOptionCount;
    
    // Create RestoreParition's BootData
    BootDataPtr->Active = 0x1;   //LOAD_OPTION_ACTIVE
    BootDataPtr->Option = GetFreeBootOptionNumber(); 
    Status = pBS->AllocatePool( EfiBootServicesData, NameSize , (VOID**)&NameBuf );
    if(EFI_ERROR(Status)){
        goto error_exit;
    }
    MemCpy(NameBuf, PopupMenuItemName, NameSize);
    BootDataPtr->Name = (CHAR16*)NameBuf;
    Status = pBS->AllocatePool( EfiBootServicesData, sizeof(EFI_DEVICE_PATH_PROTOCOL), (VOID**)&DevPath );
    if(EFI_ERROR(Status)){
        goto error_exit;
    }    
    MemCpy(DevPath, &EndOfDp, sizeof(EndOfDp));
    BootDataPtr->DevicePath = DevPath;

    // update BootOrder variable
    // note: only TSE_BOOT_NOW_IN_BOOT_ORDER is enabled.
    // note: It will be restored in RestoreBootData().
    if(BootNowInBootOrderSupport()){
        Status = GetEfiVariable(L"BootOrder", &gEfiGlobalVariableGuid, &Attr, &Size, (VOID**)&BootOrder);
        if(EFI_ERROR(Status)){
            Size = 0;
        }
        
        Status = pBS->AllocatePool( EfiBootServicesData, Size + sizeof(UINT16), (VOID**)&ModBootOrder );	// add the new BootOption
        if(EFI_ERROR(Status)){
            goto error_exit;
        }
        
        MemCpy(ModBootOrder, BootOrder, Size);
        ModBootOrder[(Size / sizeof(UINT16))] = BootDataPtr->Option;
    
        Status = pRS->SetVariable (
                        L"BootOrder",
                        &gEfiGlobalVariableGuid,
                        Attr,
                        (Size + sizeof(UINT16)),
                        ModBootOrder
                        );
        if(EFI_ERROR(Status)){
            goto error_exit;
        }
        
        if(BootOrder){
            pBS->FreePool(BootOrder);
        }
    }
    
    // update gBootData with the NewBootData
    // note: It will be restored in RestoreBootData().
    gBootData = NewBootData;
    gBootOptionCount++;
    
    // update internal flag
    IsBootDataModified = TRUE;
    
    return;
    
error_exit:
    if(NewBootData){
        pBS->FreePool(NewBootData);
    }
    if(NameBuf){
        pBS->FreePool(NameBuf);
    }
    if(DevPath){
        pBS->FreePool(DevPath);
    }
    if(BootOrder){
        pBS->FreePool(BootOrder);
    }
    if(ModBootOrder){
        pBS->FreePool(ModBootOrder);
    }
}

#endif
