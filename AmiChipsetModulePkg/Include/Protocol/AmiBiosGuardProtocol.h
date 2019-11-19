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

//*************************************************************************
// $Header: $
//
// $Revision:  $
//
// $Date: $
//*************************************************************************
/** @file SmmIoTrapDispatch.h
    The header file for I/O Trap SMM Dispatch Protocol.

**/
//*************************************************************************

#ifndef __AMI_BIOSGUARD_PROTOCOL_H__
#define __AMI_BIOSGUARD_PROTOCOL_H__

#define AMI_BIOSGUARD_SEC_SMI_FLASH_PROTOCOL_GUID \
    { 0x736b3f63, 0x4cf7, 0x4946, 0x8a, 0x4a, 0x04, 0xbe, 0x41, 0x47, 0x02, 0xd0 }

typedef struct _AMI_BIOSGUARD_SEC_SMI_FLASH_PROTOCOL AMI_BIOSGUARD_SEC_SMI_FLASH_PROTOCOL;

typedef EFI_STATUS (EFIAPI *LOAD_BIOSGUARD_FLASH_IMAGE) (
  IN OUT FUNC_BLOCK           *FuncBlock
);

typedef EFI_STATUS (EFIAPI *SET_BIOSGUARD_FLASH_UPD_METHOD) (
  IN OUT FUNC_FLASH_SESSION_BLOCK  *InfoBlock
);

typedef EFI_STATUS (EFIAPI *CHECK_ME_STATE_DISABLE) (
  VOID
);

struct _AMI_BIOSGUARD_SEC_SMI_FLASH_PROTOCOL {
  LOAD_BIOSGUARD_FLASH_IMAGE        LoadBiosGuardFwImage;
  SET_BIOSGUARD_FLASH_UPD_METHOD    SetBiosGuardFlUpdMethod;
  CHECK_ME_STATE_DISABLE            CheckMeStateDisable;
};

#ifdef PCIEX_BASE_ADDRESS 
#define ME_PCIE_CFG_ADDRESS(bus, dev, func, reg) \
            ((UINTN)(PCIEX_BASE_ADDRESS + ((UINT8)(bus) << 20) + \
            ((UINT8)(dev) << 15) + ((UINT8)(func) << 12) + (reg)))
#endif

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
