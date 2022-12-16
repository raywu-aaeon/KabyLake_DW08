/** @file
  This file initialises and Installs GopPolicy Protocol.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2011 - 2017 Intel Corporation.

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

@par Specification Reference:
**/

#include "GopPolicyInitDxe.h"
#include <Protocol/GopPolicy.h>

GLOBAL_REMOVE_IF_UNREFERENCED GOP_POLICY_PROTOCOL        mGOPPolicy;
GLOBAL_REMOVE_IF_UNREFERENCED UINT32                     mVbtSize = 0;
GLOBAL_REMOVE_IF_UNREFERENCED EFI_PHYSICAL_ADDRESS       mVbtAddress = 0;

//AMI_OVERRIDE_START - Support Runtime load VBT bin file of the RVP sku
#if AMI_SKYLAKE_ALL_VBT_SUPPORT_FLAG
#include <AmiGetBoardInfoLib.h>
#endif
//AMI_OVERRIDE_END - Support Runtime load VBT bin file of the RVP sku

#include <SetupVariable.h>

//
// Function implementations
//

/**

  @param[out] CurrentLidStatus

  @retval     EFI_SUCCESS
  @retval     EFI_UNSUPPORTED
**/
EFI_STATUS
EFIAPI
GetPlatformLidStatus (
  OUT LID_STATUS *CurrentLidStatus
  )
{
#ifndef MINTREE_FLAG
  EFI_STATUS              Status;
  UINT8                   PortDataOut;
  PLATFORM_INFO           *PlatformInfo;
  UINT8                   DataBuffer[1];

  Status = gBS->LocateProtocol (
                &gPlatformInfoProtocolGuid,
                NULL,
                (VOID **) &PlatformInfo
                );
  if (!EFI_ERROR (Status)) {
    //
    // If the platform does not support a lid, the function must return EFI_UNSUPPORTED
    //
    if (PlatformInfo->PlatformType == TypeTrad && PlatformInfo->PlatformFlavor == FlavorDesktop) {
      DEBUG ((DEBUG_INFO, "Returning Lid status as unsupported to GOP for DT/AIO board\n"));
      return EFI_UNSUPPORTED;
    }
    if (PlatformInfo->EcPresent) {
      DataBuffer[0] = EC_D_LID_STATE;
      Status = ReadEcRam (DataBuffer);
      if (Status == EFI_SUCCESS) {
        PortDataOut = DataBuffer[0];
        if ((PortDataOut & EC_B_LID_STATUS_OPEN) == EC_B_LID_STATUS_OPEN) {
          *CurrentLidStatus = LidOpen;
        } else {
          *CurrentLidStatus = LidClosed;
        }
        return EFI_SUCCESS;
      }
    }
  } else {
    DEBUG ((DEBUG_ERROR, "Failed to locate PlatformInfo Protocol!\n"));
    ASSERT_EFI_ERROR (Status);
  }
#endif
  return EFI_UNSUPPORTED;
}
/**

  @param[out] CurrentDockStatus

  @retval     EFI_SUCCESS
  @retval     EFI_UNSUPPORTED
**/
EFI_STATUS
EFIAPI
GetPlatformDockStatus (
  OUT DOCK_STATUS  CurrentDockStatus
  )
{
    return EFI_UNSUPPORTED;
}


/**

  @param[out] VbtAddress
  @param[out] VbtSize

  @retval     EFI_SUCCESS
  @retval     EFI_NOT_FOUND
**/
EFI_STATUS
EFIAPI
GetVbtData (
  OUT EFI_PHYSICAL_ADDRESS *VbtAddress,
  OUT UINT32               *VbtSize
  )
{
  EFI_STATUS                    Status;
  UINTN                         FvProtocolCount;
  EFI_HANDLE                    *FvHandles;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;
  UINTN                         Index;
  UINT32                        AuthenticationStatus;
  UINT8                         *Buffer;
  UINTN                         VbtBufferSize;
//AMI_OVERRIDE_START >>> Fixed not read Vbt data bin file 
  EFI_FV_FILETYPE               FileType;
  EFI_FV_FILE_ATTRIBUTES        FileAttributes;
//AMI_OVERRIDE_END >>> Fixed not read Vbt data bin file

//AMI_OVERRIDE_START - Support Runtime load VBT bin file of the RVP sku
  EFI_GUID  BmpImageGuid         = gIntelGraphicsVbtGuid;
#ifdef AMI_SKYLAKE_ALL_VBT_SUPPORT_FLAG
  UINT16                        BoardId;
  UINT16                        BoardIdOrgValue;
  PLATFORM_INFO                 *PlatformInfo;
  EFI_GUID  RVP3_RVP7_ImageGuid        = { 0xefcb9c70, 0x8705, 0x41ef, 0x93, 0x9d, 0xb7, 0x81, 0x8c, 0x80, 0xe6, 0xe };
  EFI_GUID  RVP8_ImageGuid             = { 0xb3e7d3f1, 0xd9f8, 0x4703, 0xac, 0xfd, 0x5c, 0xbc, 0xfb, 0xd3, 0xda, 0xd };
  EFI_GUID  RVP10_ImageGuid            = { 0xa523895d, 0x84fd, 0x4de3, 0xae, 0x98, 0x4b, 0xe, 0xef, 0x69, 0x91, 0x15 };
  EFI_GUID  RVP11_ImageGuid            = { 0x155935db, 0x435b, 0x431d, 0x9b, 0x82, 0xe0, 0x53, 0xb2, 0xbb, 0xc4, 0xfb };
  EFI_GUID  KBL_S_uDIMM_RVP_ImageGuid  = { 0xD1741F57, 0xDC1C, 0x4197, 0xAC, 0x37, 0x4,  0xBC, 0xE4, 0x75, 0xD6, 0x83 };

  BmpImageGuid = RVP8_ImageGuid;
  Status = gBS->LocateProtocol (
                  &gPlatformInfoProtocolGuid,
                  NULL,
                  &PlatformInfo
                  );
  if (!EFI_ERROR (Status)) {

    Status = AmiGetBoardInfo (&BoardId, &BoardIdOrgValue);
    ASSERT_EFI_ERROR (Status);

    switch (PlatformInfo->BoardID) {
       case BoardIdSkylakeA0Rvp3:
       case BoardIdSkylakeULpddr3Rvp5:
       case BoardIdSkylakeURvp7:
       case BoardIdSkylakeURvp15:
       case BoardIdKabylakeRDdr4:
       case BoardIdKabylakeRLpddr3:
       case BoardIdKabylakeKcDdr3:
           BmpImageGuid = RVP3_RVP7_ImageGuid;
           break;
       case BoardIdSkylakeDtRvp8Crb:
           BmpImageGuid = RVP8_ImageGuid;
           break;
       case BoardIdSkylakeAioRvp9Crb:
       case BoardIdSkylakeAioRvp10Crb:
           BmpImageGuid = RVP10_ImageGuid;
           break;
       case BoardIdSkylakeHaloDdr4Rvp11:
       case BoardIdKabylakeDdr4Rvp17: // KblGContent           
           BmpImageGuid = RVP11_ImageGuid;
           break;
       default:
           //
           // BoardId isn't match in mentioned above. Create an exception for developer.
           //
           Status = EFI_INVALID_PARAMETER;
           ASSERT_EFI_ERROR(Status);
           break;
    }

    switch (BoardIdOrgValue) {
        case BoardIdKabyLakeSDdr4UdimmEvErb:
        case BoardIdKabyLakeSDdr4UdimmEvCrb:
        case BoardIdKabyLakeSDdr4UdimmCrb:
           BmpImageGuid = KBL_S_uDIMM_RVP_ImageGuid;
           break;
    }

  }  
#endif 
//AMI_OVERRIDE_END - Support Runtime load VBT bin file of the RVP sku
  
  Status = EFI_NOT_FOUND;
  if (mVbtAddress == 0) {
    Fv           = NULL;
    Buffer = 0;
    FvHandles       = NULL;
    Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiFirmwareVolume2ProtocolGuid,
                    NULL,
                    &FvProtocolCount,
                    &FvHandles
                    );
    if (!EFI_ERROR (Status)) {
      for (Index = 0; Index < FvProtocolCount; Index++) {
        Status = gBS->HandleProtocol (
                        FvHandles[Index],
                        &gEfiFirmwareVolume2ProtocolGuid,
                        (VOID **) &Fv
                        );
//AMI_OVERRIDE_START - Fixed not read Vbt data bin file        
#if 0
        VbtBufferSize = 0;
        Status = Fv->ReadSection (
                       Fv,
#ifndef MINTREE_FLAG
                       &gIntelGraphicsVbtGuid,
#else
                       &gIntelPeiGraphicsVbtGuid,
#endif
                       EFI_SECTION_RAW,
                       0,
                       (VOID **) &Buffer,
                       &VbtBufferSize,
                       &AuthenticationStatus
                       );
#else
        if (EFI_ERROR(Status)) continue;
        
        VbtBufferSize = 0;        
        Buffer = NULL;
		   
        Status = Fv->ReadFile (
                      Fv,
                      &BmpImageGuid,
                      &Buffer,
                      &VbtBufferSize,
                      &FileType,
                      &FileAttributes,
                      &AuthenticationStatus
                      );        
#endif
//AMI_OVERRIDE_END - Fixed not read Vbt data bin file 
        if (!EFI_ERROR (Status)) {

// EDID Less >>
          {
            SETUP_DATA		SetupData;
            UINTN			VariableSize = sizeof(SETUP_DATA);
            UINT32			Attribute = 0;
            UINT8 DTD_800_480[18] = {0xFE, 0x0C, 0x20, 0x00, 0x31, 0xE0, 0x2D, 0x10, 0x28, 0x80, 0x22, 0x00, 0x2C, 0xC8, 0x10, 0x00, 0x00, 0x1A} ;
            UINT8 DTD_800_600[18] = {0x8B, 0x0F, 0x20, 0x00, 0x31, 0x58, 0x1C, 0x20, 0x28, 0x80, 0x14, 0x00, 0x2C, 0xC8, 0x10, 0x00, 0x00, 0x1A} ;
            UINT8 DTD_1024_768[18] = {0x64, 0x19, 0x00, 0x40, 0x41, 0x00, 0x26, 0x30, 0x18, 0x88, 0x36, 0x00, 0x06, 0x4D, 0x21, 0x00, 0x00, 0x18} ;
            UINT8 DTD_1280_800[18] = {0xBC, 0x1B, 0x00, 0xA0, 0x50, 0x20, 0x17, 0x30, 0x30, 0x50, 0x36, 0x00, 0x2C, 0xC8, 0x10, 0x00, 0x00, 0x1A} ;
            UINT8 DTD_1280_1024[18] = {0x2C, 0x2A, 0x00, 0x98, 0x51, 0x00, 0x2A, 0x40, 0x30, 0x70, 0x13, 0x00, 0x00, 0x2C, 0x21, 0x00, 0x00, 0x1E} ;
            UINT8 DTD_1366_768[18] = {0x3D, 0x1B, 0x56, 0x78, 0x50, 0x00, 0x0E, 0x30, 0x20, 0x20, 0x24, 0x00, 0x2C, 0xC8, 0x10, 0x00, 0x00, 0x1A} ;
            UINT8 DTD_1440_900[18] = {0x9A, 0x29, 0xA0, 0xD0, 0x51, 0x84, 0x22, 0x30, 0x50, 0x98, 0x36, 0x00, 0x2C, 0xC8, 0x10, 0x00, 0x00, 0x1A} ;
            UINT8 DTD_1600_900[18] = {0x44, 0x2F, 0x40, 0x30, 0x62, 0x84, 0x22, 0x30, 0x40, 0xC0, 0x36, 0x00, 0x2C, 0xC8, 0x10, 0x00, 0x00, 0x1E} ;
            UINT8 DTD_1600_1200[18] = {0x48, 0x3F, 0x40, 0x30, 0x62, 0xB0, 0x32, 0x40, 0x40, 0xC0, 0x13, 0x00, 0x2C, 0xC8, 0x10, 0x00, 0x00, 0x1A} ;
            UINT8 DTD_1680_1050[18] = {0x21, 0x39, 0x90, 0x30, 0x62, 0x1A, 0x27, 0x40, 0x68, 0x80, 0x36, 0x00, 0x2C, 0xC8, 0x10, 0x00, 0x00, 0x1C} ;
            UINT8 DTD_1920_1080[18] = {0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2D, 0x36, 0x00, 0x2C, 0xC8, 0x10, 0x00, 0x00, 0x1A} ;
            UINT8 DTD_1920_1200[18] = {0x28, 0x3C, 0x80, 0xA0, 0x70, 0xB0, 0x23, 0x40, 0x30, 0x20, 0x36, 0x00, 0x07, 0xC8, 0x20, 0x00, 0x00, 0x1A} ;
            UINT8 DTD_1280_720[18] = {0x01, 0x1D, 0x00, 0x72, 0x51, 0xD0, 0x1E, 0x20, 0x6E, 0x28, 0x55, 0x00, 0x0F, 0x48, 0x22, 0x00, 0x00, 0x1E} ;

            Status = gRT->GetVariable( L"Setup", &gSetupVariableGuid, &Attribute, &VariableSize, &SetupData );

            if ( SetupData.EfpEdidLessMode[0] )
            {
              switch ( SetupData.EfpEdidLessTypeSelection[0] )
              {
                case 0:
                pBS->CopyMem ( (VOID *)((UINT32)Buffer + 0x912), &DTD_800_480, 18 );
                break;
                case 1:
                pBS->CopyMem ( (VOID *)((UINT32)Buffer + 0x912), &DTD_800_600, 18 );
                break;
                case 2:
                default:
                pBS->CopyMem ( (VOID *)((UINT32)Buffer + 0x912), &DTD_1024_768, 18 );
                break;
                case 3:
                pBS->CopyMem ( (VOID *)((UINT32)Buffer + 0x912), &DTD_1280_800, 18 );
                break;
                case 4:
                pBS->CopyMem ( (VOID *)((UINT32)Buffer + 0x912), &DTD_1280_1024, 18 );
                break;
                case 5:
                pBS->CopyMem ( (VOID *)((UINT32)Buffer + 0x912), &DTD_1366_768, 18 );
                break;
                case 6:
                pBS->CopyMem ( (VOID *)((UINT32)Buffer + 0x912), &DTD_1440_900, 18 );
                break;
                case 7:
                pBS->CopyMem ( (VOID *)((UINT32)Buffer + 0x912), &DTD_1600_900, 18 );
                break;
                case 8:
                pBS->CopyMem ( (VOID *)((UINT32)Buffer + 0x912), &DTD_1600_1200, 18 );
                break;
                case 9:
                pBS->CopyMem ( (VOID *)((UINT32)Buffer + 0x912), &DTD_1680_1050, 18 );
                break;
                case 10:
                pBS->CopyMem ( (VOID *)((UINT32)Buffer + 0x912), &DTD_1920_1080, 18 );
                break;
                case 11:
                pBS->CopyMem ( (VOID *)((UINT32)Buffer + 0x912), &DTD_1920_1200, 18 );
                break;
                case 12:
                pBS->CopyMem ( (VOID *)((UINT32)Buffer + 0x912), &DTD_1280_720, 18 );
                break;
              }
              *((UINT8 *)Buffer + 0x1A9) |= BIT0 ;
            }
            else
            {
              *((UINT8 *)Buffer + 0x1A9) &= ~BIT0 ;
            }
                                  
            if ( SetupData.EfpEdidLessMode[1] )
            {
              switch ( SetupData.EfpEdidLessTypeSelection[1] )
              {
                case 0:
                pBS->CopyMem ( (VOID *)((UINT32)Buffer + 0x924), &DTD_800_480, 18 );
                break;
                case 1:
                pBS->CopyMem ( (VOID *)((UINT32)Buffer + 0x924), &DTD_800_600, 18 );
                break;
                case 2:
                default:
                pBS->CopyMem ( (VOID *)((UINT32)Buffer + 0x924), &DTD_1024_768, 18 );
                break;
                case 3:
                pBS->CopyMem ( (VOID *)((UINT32)Buffer + 0x924), &DTD_1280_800, 18 );
                break;
                case 4:
                pBS->CopyMem ( (VOID *)((UINT32)Buffer + 0x924), &DTD_1280_1024, 18 );
                break;
                case 5:
                pBS->CopyMem ( (VOID *)((UINT32)Buffer + 0x924), &DTD_1366_768, 18 );
                break;
                case 6:
                pBS->CopyMem ( (VOID *)((UINT32)Buffer + 0x924), &DTD_1440_900, 18 );
                break;
                case 7:
                pBS->CopyMem ( (VOID *)((UINT32)Buffer + 0x924), &DTD_1600_900, 18 );
                break;
                case 8:
                pBS->CopyMem ( (VOID *)((UINT32)Buffer + 0x924), &DTD_1600_1200, 18 );
                break;
                case 9:
                pBS->CopyMem ( (VOID *)((UINT32)Buffer + 0x924), &DTD_1680_1050, 18 );
                break;
                case 10:
                pBS->CopyMem ( (VOID *)((UINT32)Buffer + 0x924), &DTD_1920_1080, 18 );
                break;
                case 11:
                pBS->CopyMem ( (VOID *)((UINT32)Buffer + 0x924), &DTD_1920_1200, 18 );
                break;
                case 12:
                pBS->CopyMem ( (VOID *)((UINT32)Buffer + 0x924), &DTD_1280_720, 18 );
                break;
              }
              *((UINT8 *)Buffer + 0x1D0) |= BIT0 ;
            }
            else
            {
              *((UINT8 *)Buffer + 0x1D0) &= ~BIT0 ;
            }
          }
// EDID Less <<

          *VbtAddress = (EFI_PHYSICAL_ADDRESS)Buffer;
          *VbtSize = (UINT32)VbtBufferSize;
          mVbtAddress = *VbtAddress;
          mVbtSize = *VbtSize;
          Status = EFI_SUCCESS;
          break;
        }
      }
    } else {
      Status = EFI_NOT_FOUND;
    }

    if (FvHandles != NULL) {
      FreePool (FvHandles);
      FvHandles = NULL;
    }
  } else {
    *VbtAddress = mVbtAddress;
    *VbtSize = mVbtSize;
    Status = EFI_SUCCESS;
  }

  return Status;
}

/**
Initialize GOP DXE Policy

@param[in] ImageHandle          Image handle of this driver.

@retval EFI_SUCCESS             Initialization complete.
@retval EFI_UNSUPPORTED         The chipset is unsupported by this driver.
@retval EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver.
@retval EFI_DEVICE_ERROR        Device error, driver exits abnormally.
**/

EFI_STATUS
EFIAPI
GopPolicyInitDxe (
  IN EFI_HANDLE       ImageHandle
  )
{
  EFI_STATUS         Status;

  //
  // Initialize the EFI Driver Library
  //
  SetMem (&mGOPPolicy, sizeof (GOP_POLICY_PROTOCOL), 0);

  mGOPPolicy.Revision                = GOP_POLICY_PROTOCOL_REVISION_03;
  mGOPPolicy.GetPlatformLidStatus    = GetPlatformLidStatus;
  mGOPPolicy.GetVbtData              = GetVbtData;
  mGOPPolicy.GetPlatformDockStatus   = GetPlatformDockStatus;
#ifndef MINTREE_FLAG
  if (PcdGetBool(PcdGopOverrideDriverEnable)) {
    CopyGuid(&mGOPPolicy.GopOverrideGuid, &gGopOverrideProtocolGuid);
    DEBUG ((DEBUG_INFO, "DxeGopOverride Enabled\n"));
  } else {
    DEBUG ((DEBUG_INFO, "DxeGopOverride Disabled\n"));
   }
#endif //MINTREE_FLAG

  //
  // Install protocol to allow access to this Policy.
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gGopPolicyProtocolGuid,
                  &mGOPPolicy,
                  NULL
                  );

  return Status;
}
