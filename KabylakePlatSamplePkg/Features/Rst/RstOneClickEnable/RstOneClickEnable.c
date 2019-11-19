/**@file
  RST one click BIOS support module.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2018 Intel Corporation.

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Intel Corporation or its suppliers or
  licensors. Title to the Material remains with Intel Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary and
  confidential information of Intel Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Intel in writing.

  Unless otherwise agreed by Intel in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Intel or
  Intel's suppliers or licensors in any way.

  This file contains a 'Sample Driver' and is licensed as such under the terms
  of your license agreement with Intel or your vendor. This file may be modified
  by the user, subject to the additional terms of the license agreement.
**/

#include "RstOneClickEnable.h"
// AMI_OVERRIDE_START - Since token ALWAYS_PUBLISH_HII_RESOURCES is 0, prepare SetupVolatileData for Rst remap from PchSetup.c.
#include <Library/HobLib.h>
#include <PchPcieStorageDetectHob.h>
// AMI_OVERRIDE_END - Since token ALWAYS_PUBLISH_HII_RESOURCES is 0, prepare SetupVolatileData for Rst remap from PchSetup.c.

/**
  Entry point for RstOneClickEnable module

  @param ImageHandle  Handle for this image
  @param SystemTable  Pointer to system table
**/
EFI_STATUS
EFIAPI
RstOneClickEnableEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  RST_CONFIG_VARIABLE  RstConfigVariable;
  UINTN                Size;
  UINT32               RstConfigVarAttr;
  EFI_STATUS           Status;
  RST_MODE             RstMode;

  DEBUG ((DEBUG_INFO, "RstOneClickEnableEntryPoint() Start\n"));

  RstMode = GetSupportedRstMode ();

  if (RstMode == RstUnsupported) {
    return EFI_UNSUPPORTED;
  }

  gRS = SystemTable->RuntimeServices;

  Size = sizeof (RST_CONFIG_VARIABLE);
  Status = gRS->GetVariable (
                  RstConfigVariableName,
                  &gRstConfigVariableGuid,
                  &RstConfigVarAttr,
                  &Size,
                  &RstConfigVariable
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Failed to fetch RstConfigVariable\n Atempting to create RstConfigVariable\n"));
    Status = CreateRstConfigVariable ();
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "Failed to create RstConfigVariable\n"));
      return Status;
    } else {
      DEBUG ((DEBUG_INFO, "RstConfigVariable created successfully\n"));
      return EFI_SUCCESS;
    }
  }

  //
  //  CreateRstConfigVariable can also be used for reseting the variable
  //
  Status = CreateRstConfigVariable ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Failed to clean RstConfigVariable\n"));
    return Status;
  }

  Status = UpdateSystemConfiguration (&RstConfigVariable);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "Failed to update system configuration\n"));
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Creates an instance of RST_CONFIG_VARIABLE.
  Since this function initializes variable to 0 it can also be used to reseting the variable

  @retval EFI_STATUS  Returns status of SetVariable runtime service
**/
EFI_STATUS
CreateRstConfigVariable (
  VOID
  )
{
  RST_CONFIG_VARIABLE  RstConfigVariable;
  UINT32  RstConfigVarAttr;
  EFI_STATUS  Status;

  ZeroMem (&RstConfigVariable, sizeof (RstConfigVariable));

  RstConfigVarAttr = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS;

  Status = gRS->SetVariable (
                  RstConfigVariableName,
                  &gRstConfigVariableGuid,
                  RstConfigVarAttr,
                  sizeof (RstConfigVariable),
                  &RstConfigVariable
                  );

  return Status;
}

/**
  Updates system configuration based on RST_CONFIG_VARIABLE recieved from OS

  @param RstConfig  A pointer to an instance of RST_CONFIG_VARIABLE received from OS

  @retval EFI_STATUS  Returns EFI_SUCCESS only if update wasn't necessary, otherwise returns last error
**/
EFI_STATUS
UpdateSystemConfiguration (
  IN RST_CONFIG_VARIABLE  *RstConfig
  )
{
  EFI_STATUS        Status;

  if (!RstConfig->ModeSwitch && !RstConfig->RemapEnable) {
    return EFI_SUCCESS;
  }

  Status = UpdatePchSetupVariable (RstConfig);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "Failed to update PchSetup variable\n"));
    return Status;
  }
  gRS->ResetSystem (
         EfiResetWarm,
         EFI_SUCCESS,
         0,
         NULL
         );

  return EFI_SUCCESS;

}

/**
  Updates PchSetup variable based on information passed in RST_CONFIG_VARIABLE

  @param[in] RstConfig  RST config variable

  @retval EFI_STATUS  Returns EFI_SUCCESS if PchSetup was updated successfully, returns last error otherwise
**/
EFI_STATUS
UpdatePchSetupVariable (
  IN RST_CONFIG_VARIABLE  *RstConfig
  )
{
  PCH_SETUP   PchSetup;
  UINT32      PchSetupAttributes;
  UINTN       Size;
  EFI_STATUS  Status;

  Size = sizeof (PCH_SETUP);
  Status = gRS->GetVariable (
                  L"PchSetup",
                  &gPchSetupVariableGuid,
                  &PchSetupAttributes,
                  &Size,
                  &PchSetup
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (RstConfig->ModeSwitch) {
    SwitchSataControllerToRaid (&PchSetup);
    //
    // For cases when we enable only RAID mode we have to protect against any
    // misconfiguration that might be present on the system. If remapping has been enabled
    // but remapping enabling has not been requested by RST installer we might end up in a situation
    // when we remap the drive with OS partition while RST driver with remapping support has not been installed yet.
    // To prevent that we disable remapping on all root ports and enable it only if installer has requested it.
    //
    DisableRemapOnAllCapableRp (&PchSetup);
  } else if (RstConfig->RemapEnable) {
    if (IsIntegratedSataControllerInRaidMode ()) {
      EnableRemapOnAllCapableRp (&PchSetup);
    } else {
      DEBUG((DEBUG_ERROR, "Integrated SATA not in RAID mode, can't enable remapping\n"));
    }
  }

  Status = gRS->SetVariable (
                  L"PchSetup",
                  &gPchSetupVariableGuid,
                  PchSetupAttributes,
                  Size,
                  &PchSetup
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Checks if the integrated SATA controller is in RAID mode

  @retval BOOLEAN  True if SATA controller is in RAID mode
**/
BOOLEAN
IsIntegratedSataControllerInRaidMode (
  VOID
  )
{
  UINTN  SataRegBase;
  UINT8  SataMode;

  SataRegBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SATA, PCI_FUNCTION_NUMBER_PCH_SATA);
  SataMode = MmioRead8 (SataRegBase + R_PCI_SCC_OFFSET);

  if (SataMode == PCI_CLASS_MASS_STORAGE_RAID) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Disables NVMe remapping on all remap capable root ports

  @param[in, out] PchSetup  An instance of PchSetup to be updated
**/
VOID
DisableRemapOnAllCapableRp (
  IN OUT PCH_SETUP  *PchSetup
  )
{
  UINT32  RpNumber;

  for (RpNumber = 0; RpNumber < GetPchMaxPciePortNum (); RpNumber++) {
    PchSetup->RstPcieRemapEnabled[RpNumber] = 0;
  }
}

// AMI_OVERRIDE_START - Since token ALWAYS_PUBLISH_HII_RESOURCES is 0, prepare SetupVolatileData for Rst remap from PchSetup.c. 
VOID
PrepareRemapOnAllInfo (
  SETUP_VOLATILE_DATA  *VolatileData
  )
{
  VOID                   *Hob;
  PCIE_STORAGE_INFO_HOB  *PcieStorageInfoHob; 
  UINTN                  Index;  
  
  Hob = NULL;
  PcieStorageInfoHob = NULL;
  
  Hob = GetFirstGuidHob (&gPchPcieStorageDetectHobGuid);
  if (Hob != NULL) {
    PcieStorageInfoHob = (PCIE_STORAGE_INFO_HOB *) GET_GUID_HOB_DATA (Hob);
  }
    
  for (Index = 0; Index < PCH_MAX_PCIE_ROOT_PORTS; Index++) {
    if (PcieStorageInfoHob != NULL) {
      VolatileData->PcieStorageMap[Index] = PcieStorageInfoHob->PcieStorageLinkWidth[Index];
      VolatileData->PcieStorageProgrammingInterface[Index] = PcieStorageInfoHob->PcieStorageProgrammingInterface[Index];
    } else {
      VolatileData->PcieStorageMap[Index] = 0;
      VolatileData->PcieStorageProgrammingInterface[Index] = 0;
    }
  }    
  
  for (Index = 0; Index < PCH_MAX_PCIE_CONTROLLERS; Index++) {
    if (PcieStorageInfoHob != NULL) {
      VolatileData->CycleRouterMap[Index] = PcieStorageInfoHob->RstCycleRouterMap[Index];
      DEBUG ((DEBUG_INFO, "CycleRouterMap[%d] = %d\n", Index, VolatileData->CycleRouterMap[Index]));
    } else {
      VolatileData->CycleRouterMap[Index] = 0;
    }
  }    
}
// AMI_OVERRIDE_END - Since token ALWAYS_PUBLISH_HII_RESOURCES is 0, prepare SetupVolatileData for Rst remap from PchSetup.c. 

/**
  Enables NVMe remapping on all remap capable root ports

  @param[out] PchSetup  An instance of PchSetup to be updated

  @retval EFI_STATUS  Returns EFI_SUCCESS if remapping has been successfully enabled, returns last error otherwise
**/
VOID
EnableRemapOnAllCapableRp (
  IN OUT PCH_SETUP  *PchSetup
  )
{
  SETUP_VOLATILE_DATA  SetupData;
  UINT32  SetupDataAttributes;
  UINTN  Size;
  UINT32  RpNumber;
  EFI_STATUS  Status;

  Size = sizeof (SETUP_VOLATILE_DATA);

  Status = gRS->GetVariable (
             L"SetupVolatileData",
             &gSetupVariableGuid,
             &SetupDataAttributes,
             &Size,
             &SetupData
             );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO,"Failed to fetch SetupVolatileData variable\n"));
    return;
  }

// AMI_OVERRIDE_START - Since token ALWAYS_PUBLISH_HII_RESOURCES is 0, prepare SetupVolatileData for Rst remap from PchSetup.c. 
  PrepareRemapOnAllInfo (&SetupData);
// AMI_OVERRIDE_END - Since token ALWAYS_PUBLISH_HII_RESOURCES is 0, prepare SetupVolatileData for Rst remap from PchSetup.c. 
  
  for (RpNumber = 0; RpNumber < GetPchMaxPciePortNum (); RpNumber++) {
    if (CheckIfRootPortIsRemapCapable (&SetupData, RpNumber)) {
      //
      //  Check if remap has not been already enabled on this PCIe controller
      //
      if (RpNumber > 1 && PchSetup->RstPcieRemapEnabled[RpNumber - 2] != 1) {
        DEBUG ((DEBUG_INFO, "Enabling remapping on RP# %d\n", RpNumber));
        PchSetup->RstPcieRemapEnabled[RpNumber] = 1;
      }
    }
  }

}

/**
  Checks if given root port is remap capable based on information in SETUP_VOLATILE_DATA

  @param  SetupData  A pointer to an instance of SETUP_VOLATILE_DATA
  @param  RpNumber  Root port number to check

  @retval  BOOLEAN  Returns TRUE if root port is remap capable, returns false otherwise
**/
BOOLEAN
CheckIfRootPortIsRemapCapable (
  SETUP_VOLATILE_DATA  *SetupData,
  UINT32  RpNumber
  )
{
  if (SetupData->PcieStorageMap[RpNumber] == 0 || SetupData->PcieStorageProgrammingInterface[RpNumber] == 0 || SetupData->CycleRouterMap[RpNumber / 4] == 99) {
    return FALSE;
  }

  return TRUE;
}

/**
  Switches SATA controller to RAID mode

  @param PchSetup  A pointer to PCH_SETUP instance
**/
VOID
SwitchSataControllerToRaid (
  OUT  PCH_SETUP  *PchSetup
  )
{
  PchSetup->SataInterfaceMode = SATA_MODE_RAID;
// AMI_OVERRIDE_START - Intel RST Uefi driver is need for RstOneClick
  PchSetup->SataLegacyOrom = 0;
// AMI_OVERRIDE_END - Intel RST Uefi driver is need for RstOneClick
  DEBUG ((DEBUG_INFO, "Sata controller interface chaged to RAID\n"));

}

