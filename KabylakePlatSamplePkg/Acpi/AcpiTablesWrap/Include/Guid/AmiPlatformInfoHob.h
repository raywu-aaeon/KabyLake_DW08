//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//**********************************************************************
// $Header: 
//
// $Revision: 
//
// $Date: 
//**********************************************************************

#ifndef _AMI_PLATFORM_INFO_HOB_GUID_H_
#define _AMI_PLATFORM_INFO_HOB_GUID_H_

typedef struct _AMI_OEM_PLATFORM_INFO {
  UINT16                     BoardId;             // Board ID
  BOOLEAN                    SpdPresent;          // 0 - BIOS hard-codes SPD; 1 - User/factory programs SPD in the onboard EEPROM.
 } AMI_OEM_PLATFORM_INFO;

 
BOOLEAN AmiPlatformInfoPeiOverride (
  IN EFI_PEI_SERVICES     **PeiServices,
  IN AMI_OEM_PLATFORM_INFO   *AmiOemPlatformInfo 
);

#endif
