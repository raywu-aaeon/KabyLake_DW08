//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
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
// $Header: /Alaska/BIN/Chipset/Intel/SouthBridge/WildcatPoint/Intel Pch SB Chipset/PchWrap/WdtApp/WdtAppPei/WdtAppPei.c 1     8/14/13 9:18a Littleyan $
//
// $Revision: 1 $
//
// $Date: 8/14/13 9:18a $
//*************************************************************************
// Revision History
// ----------------
// $Log: /Alaska/BIN/Chipset/Intel/SouthBridge/WildcatPoint/Intel Pch SB Chipset/PchWrap/WdtApp/WdtAppPei/WdtAppPei.c $
// 
//*************************************************************************

#include <Efi.h>
#include <Token.h>
#include <Setup.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <Setup.h>

EFI_STATUS PlatSampleSetVariable(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data
)
{
//  EFI_STATUS              Status;
//  EFI_GUID mSetupGuid = SETUP_GUID;
  
//  if ( (StrCmp (VariableName, L"SetupVolatileData") == 0) &&
//       (guidcmp (VendorGuid, &mSetupGuid) == 0)) 
//  {
      // Move SetupVolatileData runtime flag for Intel Sample code.
//      if (Attributes & (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS)) {

//      }
// }

       
   return EFI_UNSUPPORTED;	  

}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
