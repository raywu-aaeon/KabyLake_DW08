//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
/** @file BiosGuardCpuPolicyOverride.h
    Header file for BiosGuardCpuPolicyOverride sub-component

**/

#ifndef _BIOS_GUARD_CPU_POLICY_OVERRIDE_H_
#define _BIOS_GUARD_CPU_POLICY_OVERRIDE_H_

#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/MmPciLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Pi/PiFirmwareVolume.h>
#include <Pi/PiFirmwareFile.h>
#include <Ppi/ReadOnlyVariable2.h>
#include "BaseCryptLib.h"

#include <Token.h>
#include <Ppi/SiPolicy.h>
#include <Setup.h>

#define BIOS_GUARD_PUB_KEY_FFS_FILE_RAW_GUID \
    {0x8e295870, 0xd377, 0x4b75, 0xbf, 0xdc, 0x9a, 0xe2, 0xf6, 0xdb, 0xde, 0x22}

#define  KBShift  10
#define  MBShift  20
#define  Disable  0
#define  Enable   1

#if defined(BUILD_WITH_GLUELIB)
#undef SetMem
VOID *
SetMem (
  OUT VOID *Buffer,
  IN UINTN Length,
  IN UINT8 Value
  )
{
  return GlueSetMem (Buffer, Length, Value);
}

#undef CopyMem
VOID *
EFIAPI
CopyMem (
  OUT VOID       *DestinationBuffer,
  IN CONST VOID  *SourceBuffer,
  IN UINTN       Length
  )
{
  return GlueCopyMem (DestinationBuffer, SourceBuffer, Length);
}
#endif

EFI_STATUS
EFIAPI
BiosGuardCpuPolicyOverride (
    IN CONST EFI_PEI_SERVICES       **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDesc,
    IN VOID                         *Ppi
);

#endif
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
