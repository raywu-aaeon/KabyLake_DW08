//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file UpdateMemoryRecord.h

**/

#ifndef __UPDATE_MEMORY_RECORD_H__
#define __UPDATE_MEMORY_RECORD_H__

// @todo remove and use the MdePkg\Include\Pi\PiHob.h
#if !defined(_PEI_HOB_H_) && !defined(__PI_HOB_H__)
#ifndef __HOB__H__
typedef struct _EFI_HOB_GENERIC_HEADER {
  UINT16  HobType;
  UINT16  HobLength;
  UINT32  Reserved;
} EFI_HOB_GENERIC_HEADER;

typedef struct _EFI_HOB_GUID_TYPE {
  EFI_HOB_GENERIC_HEADER  Header;
  EFI_GUID                Name;
  ///
  /// Guid specific data goes here
  ///
} EFI_HOB_GUID_TYPE;
#endif
#endif

#define SMBIOS_NOT_FOUND        0xFFFF
#define SMBIOS_UNKNOWN          0xFFFF
#define SMBIOS_NOT_PROVIDE      0xFFFE
#define MAX_HANDLES             0x10
#define SMBIOS_FOR_ALL          0xFF

#define NB_SMBIOS_TYPE17_EXTRA_INFO_DATA_GUID \
  { 0x46006e87, 0x0330, 0x44a3, 0xb8, 0x2b, 0xeb, 0xf9, 0x91, 0x9, 0x47, 0x2e }

// KabylakeSiliconPkg\SystemAgent\MemoryInit\Include\MrcInterface.h
#ifndef _MrcInterface_h_
#define MAX_CONTROLLERS          (1)       ///< The maximum number of memory controllers per CPU socket.
#define MAX_CHANNEL              (2)       ///< The maximum number of channels per memory controller.
#define MAX_DIMMS_IN_CHANNEL     (2)       ///< The maximum number of DIMMs per channel.
///
/// Define the frequencies that may be possible in the memory controller.
/// Note that not all these values may be supported.
///
#define fNoInit     (0)
#define f800        (800)
#define f1000       (1000)
#define f1100       (1100)
#define f1067       (1067)
#define f1200       (1200)
#define f1300       (1300)
#define f1333       (1333)
#define f1400       (1400)
#define f1467       (1467)
#define f1500       (1500)
#define f1600       (1600)
#define f1700       (1700)
#define f1733       (1733)
#define f1800       (1800)
#define f1867       (1867)
#define f1900       (1900)
#define f2000       (2000)
#define f2100       (2100)
#define f2133       (2133)
#define f2200       (2200)
#define f2267       (2267)
#define f2300       (2300)
#define f2400       (2400)
#define f2500       (2500)
#define f2533       (2533)
#define f2600       (2600)
#define f2667       (2667)
#define f2700       (2700)
#define f2800       (2800)
#define f2900       (2900)
#define f2933       (2933)
#define f3000       (3000)
#define f3067       (3067)
#define f3100       (3100)
#define f3200       (3200)
#define f3333       (3333)
#define f3467       (3467)
#define f3600       (3600)
#define f3733       (3733)
#define f3867       (3867)
#define f4000       (4000)
#define f4133       (4133)
#define fInvalid    (0x7FFFFFFF)
#endif

typedef struct {
  UINT8   tCKmin;
  UINT8   VDD;
} NB_TYPE17_EXTRA_DATA;

typedef struct {
  EFI_HOB_GUID_TYPE EfiHobGuidType;
  NB_TYPE17_EXTRA_DATA NbSpdData[MAX_CONTROLLERS * MAX_CHANNEL * MAX_DIMMS_IN_CHANNEL];
} NB_SMBIOS_TYPE_17_DATA_HOB;

UINT16
SMBIOS_FindStructureHandle(
    IN UINT8	Type,
    IN UINT8	Index
);

UINTN
SMBIOS_DeleteStructureByType(
    UINT8	Type,
    UINT8	Index
);

UINT16
SMBIOS_GetStructureTotalSize(
    IN UINT8	*BufferStart
);

VOID
SMBIOS_FixHandleLink(
    IN VOID    *SmbiosBuffer
);

#endif

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
