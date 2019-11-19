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
// $Header: /Alaska/BIN/Chipset/Intel/SouthBridge/LynxPoint/Intel Pch SB Board/SbSetupData.h 3     5/03/12 6:33a Victortu $
//
// $Revision: 3 $
//
// $Date: 5/03/12 6:33a $
//*************************************************************************
/** @file SbSetupData.h
    South Bridge setup data header file, define all the South
    Bridge setup items and a structures in this file. 

    @note  The context of the SB_SETUP_DATA may be able to copy from
              SB.SD directly 

**/
//*************************************************************************

#ifndef __SB_POLICY_UPDATE_H__ // To Avoid this header get compiled twice
#define __SB_POLICY_UPDATE_H__

#include <Efi.h>

#include <SbPeiInitElink.h>
#include <Ppi/SiPolicy.h>

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 1)

#pragma pack(pop)

VOID OemPchPolicyUpdateCallBack (
	IN EFI_PEI_SERVICES        **PeiServices,
	IN SI_POLICY_PPI           *SiPolicy,
	IN UINT16                  BoardId
    );

typedef VOID (SB_OEM_PCH_POLICY_CALLBACK) (
    IN VOID				**Services,
    IN SI_POLICY_PPI    *SiPolicy,
    IN UINT16           BoardId
    );
/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif 

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
