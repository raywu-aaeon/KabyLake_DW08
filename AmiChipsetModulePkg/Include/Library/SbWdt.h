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
/** @file SbWdt.h

**/
//*************************************************************************

#ifndef __SB_WDT_H__ // To Avoid this header get compiled twice
#define __SB_WDT_H__

#include <Efi.h>

#ifdef __cplusplus
extern "C" {
#endif

EFI_STATUS AmiWdtReloadAndStart
(
  IN  VOID    *Services,
  IN  UINT32  TimeoutValue,
  IN  BOOLEAN Pei
);

UINT8 AmiWdtCheckStatus
(
  IN     VOID       *Services,
  IN     BOOLEAN    Pei,
  IN OUT EFI_STATUS *Status
);

EFI_STATUS AmiWdtDisable
(
  IN  VOID    *Services,
  IN  BOOLEAN Pei
);

EFI_STATUS AmiWdtAllowKnownReset
(
  IN  VOID    *Services,
  IN  BOOLEAN Pei
);

UINT8 AmiIsWdtRequired
(
  IN      VOID       *Services,
  IN      BOOLEAN    Pei,
  IN  OUT EFI_STATUS *Status
);

UINT8 AmiIsWdtEnabled (
  IN      VOID       *Services,
  IN      BOOLEAN    Pei,
  IN  OUT EFI_STATUS *Status
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
