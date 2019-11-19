//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2017, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************

//****************************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//****************************************************************************
/** @file CrbSmbios.h
    CrbSmbios Header file.

**/
//****************************************************************************

#ifndef __CRB_SMBIOS__H__
#define __CRB_SMBIOS__H__
#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------

#include <Protocol/Smbios.h>
#include <Protocol/AmiSmbios.h>
#include <Register/PchRegsPcie.h>

//------------------------------------------------------------------------


#pragma pack (1)

#define PCIE_CFG_ADDRESS(bus, dev, func, reg) \
            ((UINTN)(PCIEX_BASE_ADDRESS + ((UINT8)(bus) << 20) + \
            ((UINT8)(dev) << 15) + ((UINT8)(func) << 12) + (reg)))

#define DEV_FUN_NUMBER(DEV,FUN) (DEV << 3 | FUN)

typedef struct {
    SMBIOS_SYSTEM_SLOTS_INFO  SlotInfo;
    CHAR8*                    sSlotDes;
    UINT8                     RootPortIndex;    //0-based, set to 0xFF if it's NorthBridge
} SMBIOS_TYPE_9;

#define PCIE_BUS 0

#define PCIE_DEV_1 PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1
#define PCIE_DEV_2 PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2
#define PCIE_DEV_3 PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_3

#define PCIE_FUN_1 PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_1
#define PCIE_FUN_2 PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_2
#define PCIE_FUN_3 PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_3
#define PCIE_FUN_4 PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_4
#define PCIE_FUN_5 PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_5
#define PCIE_FUN_6 PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_6
#define PCIE_FUN_7 PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_7
#define PCIE_FUN_8 PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_8

#pragma pack()


/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2017, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
