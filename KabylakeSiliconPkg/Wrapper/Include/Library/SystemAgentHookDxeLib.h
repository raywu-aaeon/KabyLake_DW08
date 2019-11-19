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
// $Header: /Alaska/BIN/Chipset/Intel/NorthBridge/Haswell/Intel SystemAgent NB Board/NbSetupData.h 3     4/26/12 3:00a Yurenlai $
//
// $Revision: 3 $
//
// $Date: 4/26/12 3:00a $
//*************************************************************************

#ifndef __SYSTEM_AGENT_HOOK_DXE_LIB_H__  // To Avoid this header get compiled twice
#define __SYSTEM_AGENT_HOOK_DXE_LIB_H__

#include <Library/BaseLib.h>

#ifdef __cplusplus
extern "C" {
#endif

EFI_STATUS
EFIAPI
AmiUpdateDxeSaPolicy (
  IN OUT  VOID  *DxeSaPolicyPtr,
  IN VOID       *NbSetupData
  );
  
/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif 
