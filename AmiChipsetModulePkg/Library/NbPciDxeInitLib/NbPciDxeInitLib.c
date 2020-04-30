//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
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
// $Revision: $
//
// $Date: $
//*************************************************************************

//*************************************************************************
/** @file NbPciDxeInitLib.c
    This file provide whole NB devices Pci init routine for PciBus
    call out.

    This template just provide two example.
    If you want append or modify please follow below naming rule.

    1. Append CspNbPciInit prior to your particular device name.
    for example : CspNbPciInitPciRoot, CspNbPciInitXXX

**/
//*************************************************************************

#include <AmiDxeLib.h>
#include <Token.h>
#include <PciBus.h>
#include <PciHostBridge.h>
#include <Setup.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciIo.h>
#include <Protocol/AmiBoardInitPolicy.h>
#include <Library/AmiPciBusLib.h>
#include <Nb.h>
#include <PlatformInfo.h>
#include <PlatformBoardId.h>
#include <Protocol/LegacyBiosExt.h>
#include <Protocol/AmiCsmOpromPolicy.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PciLib.h>
#include <NbSetupData.h>
#if defined(CSM_SUPPORT) && (CSM_SUPPORT == 1)
#include <SaRegs.h>
#endif
#if defined(SKYLAKE_ALL_VBIOS_AND_VBT_SUPPORT) && (SKYLAKE_ALL_VBIOS_AND_VBT_SUPPORT == 1)
#include <AmiGetBoardInfoLib.h>
#endif

extern EFI_RUNTIME_SERVICES *pRS;

typedef struct {
  BOOLEAN    isGetRomDone;
  ///
  /// The size, in bytes, of the ROM image.
  ///
  UINT64     RomSize;
  ///
  /// A pointer to the in memory copy of the ROM image. The PCI Bus Driver is responsible 
  /// for allocating memory for the ROM image, and copying the contents of the ROM to memory. 
  /// The contents of this buffer are either from the PCI option ROM that can be accessed 
  /// through the ROM BAR of the PCI controller, or it is from a platform-specific location. 
  /// The Attributes() function can be used to determine from which of these two sources 
  /// the RomImage buffer was initialized.
  /// 
  VOID       *RomImage;
} NB_PEGROM_TEMPORARY;

typedef enum {
  IGD               = 0,
  PEG,
  PCI,
  AUTO,
  SG,
  DISPLAY_DEVICE_MAX
} SETUP_PRIMARY_DISPLAY_SELECTION;



BOOLEAN ExecuteUefiRom(PCI_DEV_INFO *Dev)
{
    EFI_STATUS Status;
    AMI_OPROM_POLICY_PROTOCOL *AmiOpromPolicyProtocol;
    AMI_CSM_OPROM_POLICY_PROTOCOL *AmiOpromPolicyByPciHandleProtocol;

    Status = pBS->LocateProtocol(&gAmiOpromPolicyProtocolGuid, NULL, (VOID**)&AmiOpromPolicyProtocol); 
    if(EFI_ERROR(Status))   //if CSM OptOut is disabled we should always launch UEFI OpROM
        return TRUE;

    Status = pBS->LocateProtocol(&gAmiCsmOpromPolicyProtocolGuid, NULL, (VOID**)&AmiOpromPolicyByPciHandleProtocol);
    if (!EFI_ERROR(Status)) {
        Status = AmiOpromPolicyByPciHandleProtocol->CheckOpromPolicy(AmiOpromPolicyByPciHandleProtocol, Dev->Handle);
        return (EFI_ERROR(Status)) ? FALSE : TRUE;
    }

    Status = AmiOpromPolicyProtocol->CheckUefiOpromPolicy(AmiOpromPolicyProtocol, Dev->Class.BaseClassCode);
    return (EFI_ERROR(Status)) ? FALSE : TRUE;
}

/**
    An example for Lpc bridge.

    @param AMI_BOARD_INIT_PROTOCOL*
        UINTN*
        AMI_BOARD_INIT_PARAMETER_BLOCK*

    @retval EFI_STATUS

**/
EFI_STATUS EFIAPI CspNbPciInitPciRoot (
    IN AMI_BOARD_INIT_PROTOCOL               *This,
    IN UINTN                                 *Function,
    IN OUT AMI_BOARD_INIT_PARAMETER_BLOCK    *ParameterBlock
)
{
    // Update Standard parameter block
    AMI_BOARD_INIT_PARAMETER_BLOCK  *Args    = ParameterBlock;
    PCI_INIT_STEP                   InitStep = (PCI_INIT_STEP)Args->InitStep;
    PCI_DEV_INFO                    *dev     = (PCI_DEV_INFO*)Args->Param1;
    EFI_STATUS                      Status   = EFI_UNSUPPORTED;
    EFI_PCI_IO_PROTOCOL             *PciIo   = &dev->PciIo;
#if defined(SKYLAKE_ALL_VBIOS_AND_VBT_SUPPORT) && (SKYLAKE_ALL_VBIOS_AND_VBT_SUPPORT == 1)
    UINTN                         FvProtocolCount;
    EFI_HANDLE                    *FvHandles;
    EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;
    UINTN                         Index;
    UINT32                        AuthenticationStatus;
    EFI_FV_FILETYPE               FileType;
    EFI_FV_FILE_ATTRIBUTES        FileAttributes;
    PLATFORM_INFO                 *PlatformInfo;
    VOID                          *ImageStart;
    UINTN                          ImageSize;
    UINT16                        BoardId;
    UINT16                        BoardIdOrgValue;

    EFI_GUID  RVP3_RVP7_ImageGuid         = { 0x58B6E809, 0x808B, 0x4c26, 0xBC, 0x0A, 0xBF, 0xD7, 0xB7, 0x70, 0xD7, 0x21 };
    EFI_GUID  RVP8_ImageGuid              = { 0xB981F167, 0x9D2B, 0x4ca0, 0x82, 0xE9, 0x63, 0xDF, 0x0E, 0x3C, 0x3B, 0xF3 };
    EFI_GUID  RVP10_ImageGuid             = { 0x9AECBA5E, 0x37ED, 0x4a37, 0x83, 0xC7, 0x0F, 0x73, 0xFB, 0x34, 0xB5, 0xDB };
    EFI_GUID  RVP11_ImageGuid             = { 0xB92571E7, 0x739B, 0x4b2c, 0xB6, 0xAF, 0xCE, 0xC0, 0x2A, 0x20, 0x9C, 0x86 };
    EFI_GUID  KBL_S_uDIMM_RVP_ImageGuid   = { 0x9B703B6F, 0x90B2, 0x4E8A, 0xA8, 0x0B, 0x93, 0x2E, 0xCD, 0x1F, 0x88, 0x01 };
    EFI_GUID  VbiosImageGuid;
#endif

#if defined(CSM_SUPPORT) && (CSM_SUPPORT == 1)
    UINTN                         NbIGDBaseAddress;
    UINT8                         isPrefMem;
    UINTN                         Count;
#endif

    //---------------------------------
    // Check if parameters passed are VALID and
    if(Args->Signature != AMI_PCI_PARAM_SIG) return EFI_INVALID_PARAMETER;
    if(InitStep>=isPciMaxStep) return EFI_INVALID_PARAMETER;

    // ==========================
    // !!!  Porting required !!!!
    // ==========================
    if (InitStep == isPciGetOptionRom) {
        // An example when PciBus call this device in "isPciGetOptionRom" case.
        // ....
        //  This is a sample for On Board device.
        //  Just a sample looking for video ROM/GOP
        if (dev->PciIo.RomImage == NULL) {
            // ==========================
            // !!!  Porting required !!!!
            // ==========================
            //   .....
            /*
            UINTN  i;

            for (i = 0; i < dev->SdlDevCount; i++) {
                // find legacy rom.
                Status = PciBusReadNextEmbeddedRom(dev, i, &romSdlData, &RomFile, &RomSize);
                if(EFI_ERROR(Status)) break;
                //####ifdef AMI_MODULE_PKG_VERSION
                //####if AMI_MODULE_PKG_VERSION > 18
                if (romSdlData->PciDevFlags.Bits.UefiDriverBin == 1) continue;

                StdRomHdr = (PCI_STD_OPT_ROM_HEADER*)RomFile;
                //STD ROM Image... read VID/DID info from PCIR struct.
                PcirStruct = (PCI_DATA_STRUCTURE*)(((UINT8*)RomFile) + StdRomHdr->PcirOffset);
                // update DID in the PCIR header.
                PcirStruct->DeviceId = dev->DevVenId.DevId;

                dev->PciIo.RomImage = RomFile;
                dev->PciIo.RomSize = RomSize;
                //Update Capabilities...
                dev->Capab |= (EFI_PCI_IO_ATTRIBUTE_EMBEDDED_ROM);
                dev->Attrib  |= (EFI_PCI_IO_ATTRIBUTE_EMBEDDED_ROM);
                break;
            }*/
        }

        if( !ExecuteUefiRom(dev) ) {
#if defined(SKYLAKE_ALL_VBIOS_AND_VBT_SUPPORT) && SKYLAKE_ALL_VBIOS_AND_VBT_SUPPORT == 1
               VbiosImageGuid = RVP8_ImageGuid;

               Status = pBS->LocateProtocol (
                               &gPlatformInfoProtocolGuid,
                               NULL,
                               (VOID **)&PlatformInfo
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
                        VbiosImageGuid = RVP3_RVP7_ImageGuid;
                        break;
                    case BoardIdSkylakeDtRvp8Crb:
                        VbiosImageGuid = RVP8_ImageGuid;
                        break;
                    case BoardIdSkylakeAioRvp9Crb:
                    case BoardIdSkylakeAioRvp10Crb:
                        VbiosImageGuid = RVP10_ImageGuid;
                        break;
                    case BoardIdSkylakeHaloDdr4Rvp11:
                    case BoardIdKabylakeDdr4Rvp17: // KblGContent                    
                        VbiosImageGuid = RVP11_ImageGuid;
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
                        VbiosImageGuid = KBL_S_uDIMM_RVP_ImageGuid;
                        break;
                 }

               }

               Fv           = NULL;
               FvHandles    = NULL;
               Status = pBS->LocateHandleBuffer (
                               ByProtocol,
                               &gEfiFirmwareVolume2ProtocolGuid,
                               NULL,
                               &FvProtocolCount,
                               &FvHandles
                               );

               if (!EFI_ERROR (Status)) {
                 for (Index = 0; Index < FvProtocolCount; Index++) {
                   Status = pBS->HandleProtocol (
                                   FvHandles[Index],
                                   &gEfiFirmwareVolume2ProtocolGuid,
                                   (VOID **) &Fv
                                   );
                   if (EFI_ERROR(Status)) continue;

                   ImageSize = 0;
                   ImageStart = NULL;
//raydebug >>
DEBUG_RAYDEBUG((-1, "Before Load VBIOS\n"));
DEBUG_RAYDEBUG((-1, "VbiosImageGuid = %g\n", VbiosImageGuid));
//raydebug <<
                   Status = Fv->ReadFile (
                                   Fv,
                                   &VbiosImageGuid,
                                   &ImageStart,
                                   &ImageSize,
                                   &FileType,
                                   &FileAttributes,
                                   &AuthenticationStatus
                                   );

//raydebug >>
{
DEBUG_RAYDEBUG((-1, "(UINT32)ImageStart = 0x%X", (UINT32)ImageStart));
DEBUG_RAYDEBUG((-1, "&ImageStart = 0x%X", &ImageStart));
  {
    UINT32 i ;

    DEBUG_RAYDEBUG((-1, "Image Data Dump = \n")) ;
    for (i = 0; i < 75; i++)
    {
      if ( i % 16 == 0 && i != 0 )
      {
        DEBUG_RAYDEBUG((-1, "\n"));
      }
      else
      {
        /* code */
      }
      DEBUG_RAYDEBUG(( -1, "0x%X  ", *((UINT8 *)ImageStart + ((0xB30) + (0x30) + (0x7F2)) + i) ));
    }
  }

// Override EFP2_DTD
  {
    UINT8  EFP2_DTD_1024_768[18] = {0x64, 0x19, 0x00, 0x40, 0x41, 0x00, 0x26, 0x30, 0x18, 0x88, 0x36, 0x00, 0x06, 0x4D, 0x21, 0x00, 0x00, 0x18};

    pBS->CopyMem ( ((UINT32 *)ImageStart + ((0xB30) + (0x30) + (0x7F2) + (0x03) + (0x18))), &EFP2_DTD_1024_768, 18 );
  }

  {
    UINT32 i ;

    DEBUG_RAYDEBUG((-1, "===== After Override Image Data Dump ===== \n")) ;
    for (i = 0; i < 75; i++)
    {
      if ( i % 16 == 0 && i != 0 )
      {
        DEBUG_RAYDEBUG((-1, "\n"));
      }
      else
      {
        /* code */
      }
      DEBUG_RAYDEBUG(( -1, "0x%X  ", *((UINT8 *)ImageStart + ((0xB30) + (0x30) + (0x7F2)) + i) ));
    }
  }
}
//raydebug <<

                   if (!EFI_ERROR (Status)) {
                       dev->PciIo.RomImage = ImageStart;
                       dev->PciIo.RomSize = ImageSize;
                       break;
                   }

                 } // for (Index = 0; Index < FvProtocolCount; Index++)
               } // if (!EFI_ERROR (Status))

               if (FvHandles != NULL) {
                 FreePool (FvHandles);
                 FvHandles = NULL;
               }
#endif
        } else { // EFI
            dev->PciIo.RomImage = NULL;
            dev->PciIo.RomSize = 0;
        } // if( !ExecuteUefiRom(dev) )

        Status = EFI_SUCCESS;
    }

#if defined(CSM_SUPPORT) && (CSM_SUPPORT == 1)
    // 
    // When video is VBIOS and enabled Above 4GB MMIO, GMADR must keep 4GB MMIO below.
    // 
    if (InitStep == isPciQueryDevice) {
      if( !ExecuteUefiRom(dev) ) { // is VBIOS
        for(Count = 0; Count <= PCI_MAX_BAR_NO; Count++) {
          if( dev->Bar[Count].Offset == R_SA_IGD_GMADR ) {
            NbIGDBaseAddress = NB_PCIE_CFG_ADDRESS(SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, R_SA_IGD_GMADR);
            isPrefMem = (MmioRead8(NbIGDBaseAddress) & BIT3); // Check PREFMEM (refer SKL processor EDS v2-2)
            if ( isPrefMem ) { // Allocate type is prefetchable memory
              dev->Bar[Count].Type = tBarMmio32pf;
            } else {
              dev->Bar[Count].Type = tBarMmio32;
            }
            return EFI_SUCCESS;
          }
        }
      }
    }
#endif

        // For other case please refer to AmiBoardInitPolicy.h
        //  typedef enum {
        //      isPciNone                 = 0,  //0
        //      isPciGetSetupConfig,            //1
        //      isPciSkipDevice,                //2
        //      isPciSetAttributes,             //3
        //      isPciProgramDevice,             //4
        //      isPcieInitLink,                 //5
        //      isPcieSetAspm,                  //6
        //      isPcieSetLinkSpeed,             //7
        //      isPciGetOptionRom,              //8
        //      isPciOutOfResourcesCheck,       //9
        //      isPciReadyToBoot,               //10
        //      isPciQueryDevice,               //11
        //      isHbBasicInit,                  //12
        //      isRbCheckPresence,              //13
        //      isRbBusUpdate,                  //14
        //      isPciMaxStep                    //15...so far
        //  } PCI_INIT_STEP;

    // ....

    return Status;
}

/**
    An OEM Device at PEG2 callback Function Sample
        
    @param  AMI_BOARD_INIT_PROTOCOL*
            UINTN*
            AMI_BOARD_INIT_PARAMETER_BLOCK*

    @retval EFI_STATUS
**/

EFI_STATUS CspNbPciInitPeg012 (
    IN AMI_BOARD_INIT_PROTOCOL               *This,
    IN UINTN                                 *Function,
    IN OUT AMI_BOARD_INIT_PARAMETER_BLOCK    *ParameterBlock
)
{
    AMI_BOARD_INIT_PARAMETER_BLOCK  *Args    = ParameterBlock;
    PCI_INIT_STEP                   InitStep = (PCI_INIT_STEP)Args->InitStep;
    PCI_DEV_INFO                    *dev     = (PCI_DEV_INFO*)Args->Param1;
    EFI_STATUS                      Status   = EFI_UNSUPPORTED;
    EFI_PCI_IO_PROTOCOL             *PciIo   = &dev->PciIo;
    NB_SETUP_DATA                   SetupData;
    STATIC NB_PEGROM_TEMPORARY      PegRom_Temporary;


    //---------------------------------
    // Check if parameters passed are VALID.
    if( Args->Signature != AMI_PCI_PARAM_SIG ) return EFI_INVALID_PARAMETER;
    if( InitStep >= isPciMaxStep ) return EFI_INVALID_PARAMETER;

    //
    // Check if the module is enabled in Setup; if not - return EFI_UNSUPPORTED
    //
    if(!pRS) return EFI_UNSUPPORTED;
    GetNbSetupData (pRS, &SetupData, FALSE);

    // ==========================
    // !!!  Porting required !!!!
    // ==========================
    if (InitStep == isPciGetOptionRom) {

        if( ExecuteUefiRom(dev) ) { // EFI
          if((dev->Class.BaseClassCode == PCI_CL_DISPLAY) && (dev->Class.SubClassCode == PCI_CL_DISPLAY_SCL_VGA)) {
            if (SetupData.PrimaryDisplay == SG
#if defined(IGFX_DGPU_DISPLAY_DUPLICATE) && (IGFX_DGPU_DISPLAY_DUPLICATE == 0)
              || SetupData.PrimaryDisplay == IGD
#endif
            ) { 
                //
                // Let Pci RomImage and its size pass to a temporary space.
                //
                PegRom_Temporary.RomImage = PciIo->RomImage;
                PegRom_Temporary.RomSize = PciIo->RomSize;
                PegRom_Temporary.isGetRomDone = TRUE;

                //
                // Clear RomImage and its size, then PciBus driver will not execute its OptionRom.
                //
                PciIo->RomImage = NULL;
                PciIo->RomSize = 0;
                Status = EFI_SUCCESS;
            }
          }
        } // if( ExecuteUefiRom(dev) )

    } // isPciGetOptionRom

    if (InitStep == isPciProgramDevice) {
        if( ExecuteUefiRom(dev) ) { // EFI
          if((dev->Class.BaseClassCode == PCI_CL_DISPLAY) && (dev->Class.SubClassCode == PCI_CL_DISPLAY_SCL_VGA)) {
            if ((SetupData.PrimaryDisplay == SG) && (PegRom_Temporary.isGetRomDone)) { 
              //
              // Restore Pci RomImage and Size from temporary space.
              //
              PciIo->RomImage = PegRom_Temporary.RomImage;
              PciIo->RomSize = PegRom_Temporary.RomSize;

              //
              // Clear Temporary data.
              //
              PegRom_Temporary.isGetRomDone = FALSE;
              PegRom_Temporary.RomImage = NULL;
              PegRom_Temporary.RomSize = 0;

              Status = EFI_SUCCESS;
            }
          }
        } // if( ExecuteUefiRom(dev) )
    } // isPciProgramDevice

    return Status;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
