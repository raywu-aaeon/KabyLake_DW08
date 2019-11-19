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
// $Header: $
//
// $ReviGpion: $
//
// $Date: $
//*************************************************************************
// ReviGpion History
// ----------------
// $Log: $
// 
//*************************************************************************
/** @file AmiTbtOemLib.h
    AMI Thunderbolt OEM library header file, define all the Thunderbolt 
    function prototype OEM porting required.

**/

#ifndef _AMITBTOEMLIB_H_
#define _AMITBTOEMLIB_H_

#include <Library/TbtCommonLib.h>
#include <TbtBoardInfo.h>

#define TBT_CFG_ADDRESS(bus, dev, func, reg) \
    ((VOID*) (UINTN)(PcdGet64 (PcdPciExpressBaseAddress) + ((bus) << 20) + ((dev) << 15) + ((func) << 12) + reg))

EFI_STATUS TbtCountTime (
    IN UINTN        DelayTime,
    IN UINT16       BaseAddr 
);

BOOLEAN AmiTbtSetPCIe2TBTCommand(
  IN UINT8             UpPortBus,
  IN UINT8             Data,
  IN UINT8             Command,
  IN UINTN             Timeout
);

UINT8 GetTbtHrInfo( VOID );

VOID PullHighTbtForcePowerPin( VOID );

VOID PullDownTbtForcePowerPin( VOID );

UINT8 SynchSecurityLevel(
  IN UINT8             BiosSecurityLevel,
  IN UINT8             TbtHostLocation
);

VOID ProgramTbtSecurityLevel(
  IN UINT8             *TbtSecurityLevel,
  IN UINT8             TBTHostSeries,
  IN UINT8             TbtHostLocation,
  IN BOOLEAN           IsPei
);

UINT16
ConvertGpioGroup( 
  IN UINT8   SdlSetting
);

#endif // _AMITBTOEMLIB_H_
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
