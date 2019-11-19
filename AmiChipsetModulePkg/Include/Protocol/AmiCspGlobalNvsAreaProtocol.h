//*************************************************************************
//**                                                                     **
//**   Copyright (c) 1985-2019, American Megatrends International LLC.   **
//**                                                                     **
//**      All rights reserved. Subject to AMI licensing agreement.       **
//**                                                                     **
//*************************************************************************

/** @file AmiCspGlobalNvsAreaProtocol.h
    The header file for Csp GlobalNvs Area Protocol in DXE.

*/

#ifndef __AMI_CSP_GLOBALNVS_AREA_PROTOCOL_H__
#define __AMI_CSP_GLOBALNVS_AREA_PROTOCOL_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <Guid/AmiCspGlobalNvs.h>


#define AMI_CSP_GLOBALNVS_AREA_PROTOCOL_GUID \
  { \
    0x81c6c521, 0x47d0, 0x47ba, { 0x94, 0xdf, 0x4, 0x80, 0xd4, 0x9b, 0x27, 0x5d } \
  }

extern EFI_GUID gAmiCspGlobalNvsAreaProtocolGuid;

typedef struct _AMI_CSP_GLOBALNVS_AREA_PROTOCOL {
    AMI_CSP_NVS_AREA    *Area;
} AMI_CSP_GLOBALNVS_AREA_PROTOCOL;

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

