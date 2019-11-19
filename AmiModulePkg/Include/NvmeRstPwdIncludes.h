//***********************************************************************
//*                                                                     *
//*   Copyright (c) 1985-2019, American Megatrends International LLC.   *
//*                                                                     *
//*      All rights reserved. Subject to AMI licensing agreement.       *
//*                                                                     *
//***********************************************************************

/** @file NvmeRstPwdIncludes.h
    Header file for the Ahci Bus Driver
**/

#ifndef _NvmeRstPwdIncludes_
#define _NvmeRstPwdIncludes_

#ifdef __cplusplus
extern "C" {
#endif

#include <Base.h>
#include <Library/UefiLib.h>
#include <Protocol/PciIo.h>
#include <Protocol/NvmExpressPassthru.h>

#define NVME_SECURITY_LOCK_MASK                      0x004
#define RST_PWD_SIGNATURE  SIGNATURE_32 ('$','R','S','T')

#define NVME_RST_PASSWORD_PROTOCOL_GUID \
        {0x8f91b30f, 0x622f, 0x4b96,{ 0x82, 0xe6, 0xbb, 0xc0, 0xce, 0xd3, 0x68, 0xa0}}

#pragma pack(1)
typedef struct {
    UINT8    Reserved;
    UINT8    SecRecvBufferSize;  // Nvme Security Receive Data Size
    UINT16   SecEraseTime;       // Security erase Time MSB value
    UINT16   EnhancedSecEraseT;  // Enhanced Security erase Time MSB value
    UINT16   MasterPwdIdentifier;
    UINT16   NvmeSecStatus;
    UINT16   Reserved1[3];
} NVME_RST_SECURITY_RECEIVE_BUFFER;

typedef
struct {
    UINT32                           Signature;
    EFI_PCI_IO_PROTOCOL              *PciIO;
    VOID                             *EfiStorageSecInterface;
    VOID                             *EfiNvmExpressPassThru;    
    UINT16                            OACS;
    VOID                             *SecurityInterface;               // Pointer to AMI_HDD_SECURITY_INTERFACE structure
    NVME_RST_SECURITY_RECEIVE_BUFFER  SecReceiveBuffer;
    EFI_UNICODE_STRING_TABLE          *UDeviceName;
    UINT32                            RstNamespaceId;
    BOOLEAN                           RstControlled;
    EFI_HANDLE                        RstControllerHandle;
    EFI_HANDLE                        ControllerHandle;
    VOID                              *OpalConfig;                         // Pointer to OPAL_DEVICE
}NVME_RST_INTERFACE;
#pragma pack()
/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif

#endif
