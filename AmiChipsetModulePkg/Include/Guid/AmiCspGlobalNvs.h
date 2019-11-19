//*************************************************************************
//**                                                                     **
//**   Copyright (c) 1985-2019, American Megatrends International LLC.   **
//**                                                                     **
//**      All rights reserved. Subject to AMI licensing agreement.       **
//**                                                                     **
//*************************************************************************

/** @file AmiCspGlobalNvs.h
    Chipset Global Nvs function definitions.

*/

#ifndef __AMI_CSP_GLOBALNVS__H__
#define __AMI_CSP_GLOBALNVS__H__
#ifdef __cplusplus
extern "C" {
#endif

#include <Uefi.h>

#define AMI_CSP_GLOBALNVS_PTR_VARIABLE      L"AmiCspGlobalNvsPtrVar"

#pragma pack(push,1)

typedef struct {
    UINT8   NotifyPowerBtn;         // Offset 0        Determine if notify power button in Sx Transition
    UINT8   Reserved[31];           // Offset 1:31
} AMI_CSP_NVS_AREA;

#pragma pack(pop)

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

