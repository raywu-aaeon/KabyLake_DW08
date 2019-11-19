//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

#include <Token.h>
#include <AmiDxeLib.h>
#include <PciBus.h>
#include <Setup.h>
#include <NbSetupData.h>
#if CSM_SUPPORT
#include <Protocol/LegacyBiosPlatform.h>
#endif

#if CSM_SUPPORT
/**
    Returns a buffer of handles for the requested subfunction.

    @param 
        This            Indicates the EFI_LEGACY_BIOS_PLATFORM_PROTOCOL instance.
        Mode            Specifies what handle to return.
        GetVgaHandle
        GetIdeHandle
        GetIsaBusHandle
        GetUsbHandle
        Type            Handle Modifier - Mode specific
        HandleBuffer    Pointer to buffer containing all Handles matching the 
        specified criteria. Handles are sorted in priority order.
        Type EFI_HANDLE is defined in InstallProtocolInterface()
        in the EFI 1.10 Specification.
        HandleCount     Number of handles in HandleBuffer.
        AdditionalData  Pointer to additional data returned - mode specific..

    @retval 
        EFI_SUCCESS The handle is valid.
        EFI_UNSUPPORTED Mode is not supported on this platform.
        EFI_NOT_FOUND   The handle is not known.

**/

EFI_STATUS
NBGetPlatformHandle (
  IN  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL     *This,
  IN  EFI_GET_PLATFORM_HANDLE_MODE          Mode,
  IN  UINT16                                Type,
  OUT EFI_HANDLE                            **HandleBuffer,
  OUT UINTN                                 *HandleCount,
  OUT VOID                                  OPTIONAL **AdditionalData
)
{
    EFI_STATUS             Status = EFI_SUCCESS;
    UINTN                  sz = 0, i;
    UINT8                  PriVideo=0;      //3=AUTO 0=IGD 1=PEG 2=PCI
    UINT8                  PrimaryPeg=0;    //0=AUTO 1=PEG11 2=PEG12
    UINT8                  PrimaryPcie=0;   //0=AUTO 1=PCIE1 2=PCIE2....
    EFI_HANDLE             *hb, *vh, peg=NULL, peg00=NULL, pcie0=NULL, igd=NULL, pcie=NULL;
    T_ITEM_LIST            VgaList={0,0,NULL};
    PCI_DEV_INFO           *dev;
    EFI_PCI_IO_PROTOCOL    *pciio;
    BOOLEAN                ChangeIgfxToAuto = FALSE;
    BOOLEAN                ChangePegToAuto = FALSE;
    BOOLEAN                ChangepcieToAuto = FALSE;
    BOOLEAN                ChangePrimaryPeg = FALSE;
    BOOLEAN                ChangePrimaryPcie = FALSE;
    NB_SETUP_DATA          *NbSetupData = NULL;
    UINTN                  VariableSize = sizeof(NB_SETUP_DATA);
    
    switch (Mode) {
        case EfiGetPlatformVgaHandle:  
                Status = pBS->AllocatePool (EfiBootServicesData, VariableSize, (VOID **)&NbSetupData);
                ASSERT_EFI_ERROR(Status);

                GetNbSetupData (pRS, NbSetupData, FALSE);

                PriVideo    = NbSetupData->PrimaryDisplay;
                PrimaryPeg  = NbSetupData->PrimaryPeg;
                PrimaryPcie = NbSetupData->PrimaryPcie;

                // Free memory used for setup data
                pBS->FreePool (NbSetupData);

                // Get all handles with PCI IO Protocol..
                Status = pBS->LocateHandleBuffer (ByProtocol, &gEfiPciIoProtocolGuid, NULL, &sz, &hb);
                if(EFI_ERROR(Status)) return Status;

                for(i = 0; i < sz; i++) {
                    Status = pBS->HandleProtocol (hb[i], &gEfiPciIoProtocolGuid, (VOID **)&pciio);
                    if (EFI_ERROR(Status)) {
                        pBS->FreePool(hb);
                        return Status;
                    }

                    dev = (PCI_DEV_INFO*)pciio;
                    //
                    // When a graphics card with internal PCI-PCI bridges (GTX 295) is attached to 
                    // PCH PCIe ports video is not displayed.
                    // PCI to PCI bridge has another device with Baseclasscode as 0x03 and Sub Classcode 
                    // as 0x02. And in CSMBsp.c file, it was seeing this as Video controller.
                    //
                    if((dev->Class.BaseClassCode == PCI_CL_DISPLAY) && (dev->Class.SubClassCode == 0x00)) {
                        Status = AppendItemLst(&VgaList, (VOID*)dev);
                        if(EFI_ERROR(Status)) return Status;
                    }
                }

                pBS->FreePool(hb);

                vh = Malloc(sizeof(EFI_HANDLE));
                *vh = NULL;

                for(i = 0; i < VgaList.ItemCount; i++) {
                    PCI_DEV_INFO               *brg;
                    //--------------------------------
                    //This is an Easy task we know exactly where it is
                    dev = VgaList.Items[i];
                    if (igd == NULL) {
                        if((dev->ParentBrg->Type == tPciRootBrg) 
                        && (dev->Address.Addr.Device == 2) 
                        && (dev->Address.Addr.Function == 0)) 
                            igd=dev->Handle;
                    }
                    
                    //Some Video cards Implement a MULTY FUNCTIONAL Devices;
                    //Some - use PCI 2 PCI bridg(es) to multiply itself
                    //to Avoid further complications regarding such situation 
                    //we'll try to handle this situation!
                    //We will assume for Primary Video Device Selection that it always
                    //will be the function 0 of multifunctional Device.
                    brg = dev->ParentBrg;
                    
                    if (peg == NULL) {
                        while (brg->Type == tPci2PciBrg) {
                            if ((brg->DevVenId.VenId == 0x8086)       //Intel's Vendor ID 
                            && (brg->ParentBrg->Type != tPci2PciBrg)  //Must be connected to the HOST
                            && (brg->Address.Addr.Device == 1)        //Device #1
                            && (brg->Address.Addr.Function >= 0)      //Function #0 ~ 2
                            && ((dev->PciIo.RomImage != NULL )        //Device behind this Bridge must have ROM
                            || (dev->Bar[PCI_MAX_BAR_NO].Type != tBarUnused))  
                            && (dev->Address.Addr.Function == 0)
                            && (dev->Capab & EFI_PCI_IO_ATTRIBUTE_VGA_MEMORY)
                            && (dev->Capab & EFI_PCI_IO_ATTRIBUTE_VGA_IO)) 
                            {
                                if (PrimaryPeg != 0) {
                                   if (brg->Address.Addr.Device == 1) {
                                      if (brg->Address.Addr.Function == 1 && PrimaryPeg == 1) 
                                         peg = dev->Handle;
                                      else if (brg->Address.Addr.Function == 2 && PrimaryPeg == 2)
                                         peg = dev->Handle;
                                      else if (brg->Address.Addr.Function == 0)
                                         peg00 = dev->Handle;
                                   }
                                } else peg=dev->Handle;
                            }
                    
                            if (peg != NULL) break;
                            brg = brg->ParentBrg;
                        } //#### while
                    } //#### if(peg == NULL)
                    
                    //Do the same for Video that might be behind PCI 2 PCI bridge device 1Eh
                    brg = dev->ParentBrg;
                    if (pcie == NULL) {
                        while (brg->Type == tPci2PciBrg) {
                            if ((brg->DevVenId.VenId == 0x8086)         //Intel's Vendor ID
                            && (brg->ParentBrg->Type != tPci2PciBrg)    //Must be connected to the HOST
                            && ((brg->Address.Addr.Device == 0x1B)      //0:1B:x - PCIe root ports on PCH
                            ||  (brg->Address.Addr.Device == 0x1C)      //0:1C:x - PCIe root ports on PCH
                            ||  (brg->Address.Addr.Device == 0x1D))      //0:1D:x - PCIe root ports on PCH
                            && ((dev->PciIo.RomImage != NULL )          //Device must have an Option ROM
                            || (dev->Bar[PCI_MAX_BAR_NO].Type != tBarUnused))  
                            && (dev->Address.Addr.Function == 0))
                            {
                                if (PrimaryPcie != 0) {
                                   if (brg->Address.Addr.Device == 0x1C) {
                                      if (brg->Address.Addr.Function == 1 && PrimaryPcie == 1) 
                                         pcie = dev->Handle;
                                      else if (brg->Address.Addr.Function == 2 && PrimaryPcie == 2)
                                         pcie = dev->Handle;
                                      else if (brg->Address.Addr.Function == 3 && PrimaryPcie == 3) 
                                         pcie = dev->Handle;
                                      else if (brg->Address.Addr.Function == 4 && PrimaryPcie == 4) 
                                         pcie = dev->Handle;
                                      else if (brg->Address.Addr.Function == 5 && PrimaryPcie == 5)
                                         pcie = dev->Handle;
                                      else if (brg->Address.Addr.Function == 6 && PrimaryPcie == 6)
                                         pcie = dev->Handle;
                                      else if (brg->Address.Addr.Function == 7 && PrimaryPcie == 7)
                                         pcie = dev->Handle;
                                      else if (brg->Address.Addr.Function == 0)
                                         pcie0 = dev->Handle;
                                   }
                                   if (brg->Address.Addr.Device == 0x1D) {
                                      if (brg->Address.Addr.Function == 0 && PrimaryPcie == 8) 
                                         pcie = dev->Handle;
                                      else if (brg->Address.Addr.Function == 1 && PrimaryPcie == 9)
                                         pcie = dev->Handle;
                                      else if (brg->Address.Addr.Function == 2 && PrimaryPcie == 10) 
                                         pcie = dev->Handle;
                                      else if (brg->Address.Addr.Function == 3 && PrimaryPcie == 11) 
                                         pcie = dev->Handle;
                                      else if (brg->Address.Addr.Function == 4 && PrimaryPcie == 12) 
                                         pcie = dev->Handle;
                                      else if (brg->Address.Addr.Function == 5 && PrimaryPcie == 13) 
                                         pcie = dev->Handle;
                                      else if (brg->Address.Addr.Function == 6 && PrimaryPcie == 14) 
                                         pcie = dev->Handle;
                                      else if (brg->Address.Addr.Function == 7 && PrimaryPcie == 15) 
                                         pcie = dev->Handle;
                                   }
                                   if (brg->Address.Addr.Device == 0x1B) {
                                      if (brg->Address.Addr.Function == 0 && PrimaryPcie == 16) 
                                         pcie = dev->Handle;
                                      else if (brg->Address.Addr.Function == 1 && PrimaryPcie == 17)
                                         pcie = dev->Handle;
                                      else if (brg->Address.Addr.Function == 2 && PrimaryPcie == 18) 
                                         pcie = dev->Handle;
                                      else if (brg->Address.Addr.Function == 3 && PrimaryPcie == 19) 
                                         pcie = dev->Handle;
                                      else if (brg->Address.Addr.Function == 4 && PrimaryPcie == 20) 
                                         pcie = dev->Handle;
                                      else if (brg->Address.Addr.Function == 5 && PrimaryPcie == 21) 
                                         pcie = dev->Handle;
                                      else if (brg->Address.Addr.Function == 6 && PrimaryPcie == 22) 
                                         pcie = dev->Handle;
                                      else if (brg->Address.Addr.Function == 7 && PrimaryPcie == 23) 
                                         pcie = dev->Handle;
                                   }
                                } else pcie = dev->Handle;
                            }
                    
                            if (pcie != NULL) break;
                            brg = brg->ParentBrg;
                        } //#### while
                    } //#### if(pcie == NULL)
                } //#### for;
            
                if (PrimaryPeg != 0 && peg == NULL) {
                    peg = peg00;
                    ChangePrimaryPeg = TRUE;
                }
                
                if (PrimaryPcie != 0 && pcie == NULL) {
                    pcie = pcie0;
                    ChangePrimaryPcie = TRUE;
                }
                
                //Clear Temp Buffer we had for VGA Handles; 
                ClearItemLst(&VgaList, FALSE);
                
                //3=AUTO 0=IGD 1=PEG 2=pcie
                switch (PriVideo) {
                    case 3 : //0 = auto PEG -> pcie -> IGD
                        if (peg) {
                            *vh = peg;
                            break;
                        }
                
                        if (pcie) {
                            *vh = pcie;
                             break;
                        }
                
                        if (igd) *vh = igd;
                        break;
                
                    case 4 : //SG: 1=IGD
                    case 0 : //1=IGD  b0|d2|f0
                        if (igd) {
                            *vh = igd;
                        } else {
                            ChangeIgfxToAuto = TRUE;
                        }
                        break;
                
                    case 1 : //1=PEG Bridge b0|d1|f0
                        if (peg) {
                            *vh = peg;
                        } else {
                            ChangePegToAuto = TRUE;
                        }
                        break;
                
                    case 2 : //1=PCI2 PCI bridge  b0|d1E|f0
                        if (pcie) {
                            *vh = pcie;
                        } else {
                            ChangepcieToAuto = TRUE;
                        }
                        break;
                } // switch
                
                if(*vh == NULL) {
                    Status = EFI_UNSUPPORTED;
                    pBS->FreePool(vh);
                } else {
                    Status = EFI_SUCCESS;
                    *HandleCount = 1;
                    *HandleBuffer = vh;
                }
                
                if (ChangeIgfxToAuto || ChangePegToAuto || ChangepcieToAuto || ChangePrimaryPeg || ChangePrimaryPcie) {
                    EFI_STATUS             SetupStatus;
                    SETUP_DATA             *SetupData = NULL;
                    EFI_GUID               SetupGuid  = SETUP_GUID;
                    UINT32                 Attributes = 0;
                
                    sz = sizeof(SETUP_DATA);
                    SetupStatus = GetEfiVariable(L"Setup", &SetupGuid, &Attributes, &sz, (VOID **)&SetupData);
                    if (!EFI_ERROR(SetupStatus)) 
                    {
                       if (ChangePrimaryPeg) SetupData->PrimaryPeg = 0; // change to AUTO
                       if (ChangePrimaryPcie) SetupData->PrimaryPcie = 0; // change to AUTO
                
                       if(ChangeIgfxToAuto || ChangePegToAuto || ChangepcieToAuto)
                          SetupData->PrimaryDisplay = 3; // change to AUTO
                
                       SetupStatus = pRS->SetVariable (
                                           L"Setup",
                                           &SetupGuid,
                                           Attributes,
                                           sizeof(SETUP_DATA),
                                           SetupData);
                       ASSERT_EFI_ERROR (SetupStatus);
                    }
                }
                break;
        default: 
                Status = EFI_UNSUPPORTED;
    } // switch

    return Status;
}

#endif

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
