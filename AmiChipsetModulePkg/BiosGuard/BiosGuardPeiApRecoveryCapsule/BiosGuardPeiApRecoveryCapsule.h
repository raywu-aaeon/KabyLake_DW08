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

/** @file BiosGuardPeiApRecoveryCapsule.h
    This file is used for BIOS Guard flash tool executed "/recovery" and "/capsule".

**/

#ifndef _BIOS_GUARD_PEI_AP_RECOVERY_CAPSULE_H_
#define _BIOS_GUARD_PEI_AP_RECOVERY_CAPSULE_H_

#include <PiPei.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <AmiHobs.h>
#include <Capsule.h>
#include <Ppi/Capsule.h>
#include <Guid/CapsuleVendor.h>
#include <BiosGuard.h>
#include "AmiCertificate.h"
#include <FlashUpd.h>
#include <CpuRegs.h>
#include <Setup.h>
#include "Token.h"
#include <Ppi/SiPolicy.h>
#include <Library/ConfigBlockLib.h>
#include <Library/AmiBiosGuardMiscLib.h>
#include <Library/MmPciLib.h>
#include <Library/IoLib.h>
#include <AmiBiosGuard.h>
#include <Library/PciSegmentLib.h>
#include <Register/PchRegsSpi.h>
#include <Register/PchRegsPmc.h>
#include <Register/PchRegsLpc.h>
#include <Library/SiPolicyLib.h>

EFI_GUID FWCapsuleGuid                     = APTIO_FW_CAPSULE_GUID;
EFI_GUID BiosGuardRecoveryGuid             = BIOS_GUARD_RECOVERY_GUID;
static EFI_GUID FwCapFfsGuid               = AMI_FW_CAPSULE_FFS_GUID;
static EFI_GUID FwCapSectionGuid           = AMI_FW_CAPSULE_SECTION_GUID;
extern EFI_GUID gFlashUpdBootModePpiGuid;
extern EFI_GUID gAmiPeiEndOfMemDetectGuid;
extern EFI_GUID gAmiPeiAfterMrcGuid;

#define BIOSGUARD_PACKAGE_CERT   524
#define Disable  0
#define Enable   1

EFI_STATUS
EFIAPI
BiosGuardCapsuleChangeBootModeAfterEndofMrc (
    IN EFI_PEI_SERVICES                   **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDescriptor,
    IN VOID                               *InvokePpi
);

EFI_STATUS
EFIAPI
BiosGuardGetCapsuleImage (
    IN EFI_PEI_SERVICES                  **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR         *NotifyDescriptor,
    IN VOID                              *InvokePpi
);

EFI_STATUS
EFIAPI
BiosGuardSiPreMemPolicyOverride (
    IN EFI_PEI_SERVICES                   **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDescriptor,
    IN VOID                               *InvokePpi
);

EFI_STATUS
EFIAPI
BiosGuardMeFwBootOnFlashUpdateNotify(
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
    IN  VOID                        *NullPpi
);

static EFI_PEI_PPI_DESCRIPTOR FlashUpdateModePpi = {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gFlashUpdBootModePpiGuid, NULL
};

static EFI_PEI_PPI_DESCRIPTOR RecoveryModePpi = {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiBootInRecoveryModePpiGuid, NULL
};

static EFI_PEI_NOTIFY_DESCRIPTOR BiosGuardCapsuleChangeBootModeAfterEndofMrcNotifyList[] = {
    {   EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK |  EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
        &gAmiPeiAfterMrcGuid,
        BiosGuardCapsuleChangeBootModeAfterEndofMrc
    },
};

static EFI_PEI_NOTIFY_DESCRIPTOR BiosGuardGetCapsuleImageNotifyList[] = {
    {   EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK |  EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
        &gAmiPeiEndOfMemDetectGuid,
        BiosGuardGetCapsuleImage
    },
};

static EFI_PEI_NOTIFY_DESCRIPTOR BiosGuardSiPreMemPolicyOverrideNotifyList[] = {
    {   EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK |  EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
        &gSiPreMemPolicyPpiGuid,
        BiosGuardSiPreMemPolicyOverride
    },
};

static EFI_PEI_NOTIFY_DESCRIPTOR BiosGuardMeFwBootOnFlashUpdateNotifyList[] = {
    { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
      &gFlashUpdBootModePpiGuid, 
      BiosGuardMeFwBootOnFlashUpdateNotify 
    },
};

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
