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

#ifndef __SYSTEM_AGENT_LINK_PEI_LIB_H__  // To Avoid this header get compiled twice
#define __SYSTEM_AGENT_LINK_PEI_LIB_H__

#include <Library/BaseLib.h>


#ifdef __cplusplus
extern "C" {
#endif

VOID SaSetupCallbacks (
    IN VOID              *Services,
    IN OUT NB_SETUP_DATA *NbSetupData,
    IN SETUP_DATA        *SetupData,
    IN BOOLEAN           Pei );
  
/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif 
