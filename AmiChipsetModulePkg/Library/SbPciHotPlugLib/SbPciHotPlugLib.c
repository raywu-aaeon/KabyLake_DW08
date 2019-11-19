//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
//
//*************************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//*************************************************************************
//-------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------
#include <AmiLib.h>
#include <Library/DebugLib.h>
#include <AcpiRes.h>
#include <PchAccess.h>
#include <SaAccess.h>
#include <PciBus.h>
#include <PciHostBridge.h>
#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciHotPlugInit.h>
#include <Protocol/DevicePath.h>
#include <Library/AmiPciBusLib.h>
#include <Library/AmiPciExpressLib.h>
#include <Library/AmiSdlLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Setup.h>
#include <SbPciHotPlugLib.h>
#include <Library/PchPcieRpLib.h>
//AMI_OVERRIDE_START Code will build error after disabling "Thunderbolt_SUPPORT" - EIP262081
#include <Library/PchPcieRpLib.h>
//AMI_OVERRIDE_END Code will build error after disabling "Thunderbolt_SUPPORT" - EIP262081

#define SB_PORT8_NUM   8
#define SB_PORT16_NUM  16
#define SB_PORT24_NUM  24
#define SB_PCIE_NUM  (24)
#define NB_PEG_NUM   (3)
#define PCIE_NUM  (SB_PCIE_NUM + NB_PEG_NUM)
#define NUMBER_OF_PCH_PCIE (PCIE_NUM - 3)    // EIP279338

//-------------------------------------------------------------------------
// Constants, Macros and Type Definitions
//-------------------------------------------------------------------------
//ONLY ONE INSTANCE ALOUD IN THE SYSTEM!!!
EFI_PCI_HOT_PLUG_INIT_PROTOCOL    *gRhpcInitProtocol = NULL;
BOOLEAN                           gRhpcNotFound = FALSE;

PCIE_HOT_PLUG_DEVICE_PATH mPcieList[PCIE_NUM] = {
  {
    ACPI,
    PCI(PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_1),
    END
  },   // PCI Express 0 (Port 1)
  {
    ACPI,
    PCI(PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_2),
    END
  },   // PCI Express 1 (Port 2)
  {
    ACPI,
    PCI(PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_3),
    END
  },   // PCI Express 2 (Port 3)
  {
    ACPI,
    PCI(PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_4),
    END
  },   // PCI Express 3 (Port 4)
  {
    ACPI,
    PCI(PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_5),
    END
  },   // PCI Express 4 (Port 5)
  {
    ACPI,
    PCI(PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_6),
    END
  },   // PCI Express 5 (Port 6)
  {
    ACPI,
    PCI(PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_7),
    END
  },   // PCI Express 6 (Port 7)
  {
    ACPI,
    PCI(PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_8),
    END
  },   // PCI Express 7 (Port 8)
  {
    ACPI,
    PCI(PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_9),
    END
  },   // PCI Express 8 (Port 9)
  {
    ACPI,
    PCI(PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_10),
    END
  },   // PCI Express 9 (Port 10)
  {
    ACPI,
    PCI(PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_11),
    END
  },   // PCI Express 10 (Port 11)
  {
    ACPI,
    PCI(PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_12),
    END
  },   // PCI Express 11 (Port 12)
  {
    ACPI,
    PCI(PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_13),
    END
  },   // PCI Express 12 (Port 13)
  {
    ACPI,
    PCI(PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_14),
    END
  },   // PCI Express 13 (Port 14)
  {
    ACPI,
    PCI(PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_15),
    END
  },   // PCI Express 14 (Port 15)
  {
    ACPI,
    PCI(PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_16),
    END
  },   // PCI Express 15 (Port 16)
  {
    ACPI,
    PCI(PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_3, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_17),
    END
  },   // PCI Express 16 (Port 17)
  {
    ACPI,
    PCI(PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_3, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_18),
    END
  },   // PCI Express 17 (Port 18)
  {
    ACPI,
    PCI(PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_3, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_19),
    END
  },   // PCI Express 18 (Port 19)
  {
    ACPI,
    PCI(PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_3, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_20),
    END
  },   // PCI Express 19 (Port 20)
  {
    ACPI,
    PCI(PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_3, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_21),
    END
  },   // PCI Express 20 (Port 21)
  {
    ACPI,
    PCI(PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_3, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_22),
    END
  },   // PCI Express 21 (Port 22)
  {
    ACPI,
    PCI(PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_3, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_23),
    END
  },   // PCI Express 22 (Port 23)
  {
    ACPI,
    PCI(PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_3, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_24),
    END
  },   // PCI Express 23 (Port 24)
  {
    ACPI,
    PCI(SA_PEG_DEV_NUM, SA_PEG10_FUN_NUM),
    END
  },   // NB PCI Express 0 (PEG 0)
  {
    ACPI,
    PCI(SA_PEG_DEV_NUM, SA_PEG11_FUN_NUM),
    END
  },   // NB PCI Express 1 (PEG 1)
  {
    ACPI,
    PCI(SA_PEG_DEV_NUM, SA_PEG12_FUN_NUM),
    END
  },   // NB PCI Express 2 (PEG 2)
};

EFI_HPC_LOCATION          mPcieLocation[PCIE_NUM] = {
  {
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[0],
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[0]
  },
  {
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[1],
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[1]
  },
  {
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[2],
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[2]
  },
  {
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[3],
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[3]
  },
  {
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[4],
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[4]
  },
  {
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[5],
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[5]
  },
  {
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[6],
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[6]
  },
  {
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[7],
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[7]
  },
  {
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[8],
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[8]
  },
  {
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[9],
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[9]
  },
  {
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[10],
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[10]
  },
  {
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[11],
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[11]
  },
  {
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[12],
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[12]
  },
  {
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[13],
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[13]
  },
  {
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[14],
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[14]
  },
  {
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[15],
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[15]
  },
  {
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[16],
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[16]
  },
  {
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[17],
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[17]
  },
  {
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[18],
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[18]
  },
  {
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[19],
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[19]
  },
  {
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[20],
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[20]
  },
  {
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[21],
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[21]
  },
  {
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[22],
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[22]
  },
  {
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[23],
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[23]
  },
  {
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[24],
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[24]
  },
  {
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[25],
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[25]
  },
  {
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[26],
    (EFI_DEVICE_PATH_PROTOCOL *) &mPcieList[26]
  },
};

//AMI_OVERRIDE_START Code will build error after disabling "Thunderbolt_SUPPORT" - EIP262081
/**
  Get Pch/Peg Pcie Root Port Device and Function Number for TBT by Root Port physical Number

  @param[in]  RpNumber              Root port physical number. (0-based)
  @param[out] RpDev                 Return corresponding root port device number.
  @param[out] RpFun                 Return corresponding root port function number.

  @retval     EFI_SUCCESS           Root port device and function is retrieved
**/
EFI_STATUS
EFIAPI
GetTbtRpDevFun (
  IN  UINTN   RpNumber,
  OUT UINTN   *RpDev,
  OUT UINTN   *RpFunc
  )
{
    UINTN                 TbtRpDev;
    UINTN                 TbtRpFunc;
    
// EIP279338 >> Code change START
  //
  // Total is 23 PCIE, out of the range
  //
  if (RpNumber >= PCIE_NUM) {
    return EFI_INVALID_PARAMETER;
  }

  if (RpNumber >= NUMBER_OF_PCH_PCIE)
  { 
    //
    //  PEG Rootport      RpNumber = 20, 21, 22
    //
    *RpDev  =   0x01;
    *RpFunc =   RpNumber - NUMBER_OF_PCH_PCIE;
  } else {
    //
    //  PCH Rootport      RpNumber = 0 ~ 19
    //
    GetPchPcieRpDevFun (RpNumber, &TbtRpDev, &TbtRpFunc);
    *RpDev  = TbtRpDev;
    *RpFunc = TbtRpFunc;
  }
// EIP279338 << Code change END

  return EFI_SUCCESS;
}
//AMI_OVERRIDE_END Code will build error after disabling "Thunderbolt_SUPPORT" - EIP262081

/*
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:    HpcCheckHpCompatibleHost()
//
// Description: This function will check if "HostBrg" passed is the Bridge
// with hotplug support.
//
// Input:
//  PCI_HOST_BRG_DATA *HostBrg     Pointer to PCI HB Private Data structure.
//
// Output:  BOOLEAN
//  TRUE    If "Device" is a Bridge with HPC on it.
//  FALSE   Otherwice.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN HpcCheckHpCompatibleHost(PCI_HOST_BRG_DATA *HostBrg){
    if(    HostBrg->HpcData->HpcLocCount > 0 ) return TRUE;
    return FALSE;
}
*/
/**
    This function will check if "Device" passed is the Bridge
    Type Device with hotplug support.

    @param 
        PCI_DEV_INFO    *Device     Pointer to PCI Device Private Data structure.

    @retval BOOLEAN
        TRUE    If "Device" is a Bridge with HPC on it.
        FALSE   Otherwice.

**/
BOOLEAN HpIsHpb(PCI_DEV_INFO *Device){

    //check if it is the right type of device to have Hotplug capabilities
    if(!((Device->Type==tPci2PciBrg)||(Device->Type==tPci2CrdBrg)||(Device->Type==tPciRootBrg))){
        DEBUG((DEBUG_INFO, "PciHP: Device @ B%X|D%X|F%X is NOT HP CAPABLE (Brg Type)\n", Device->Address.Addr.Bus,Device->Address.Addr.Device, Device->Address.Addr.Function));
        return FALSE;
    }

    //Check if it has Root Hpc or PciExpress capabilities
    if((Device->HotPlug==NULL) && !PcieCheckPcieCompatible(Device)){
        DEBUG((DEBUG_INFO, "PciHP: Device @ B%X|D%X|F%X is NOT HP CAPABLE (SHPC/PCIe)\n", Device->Address.Addr.Bus,Device->Address.Addr.Device, Device->Address.Addr.Function));
        return FALSE;
    }

//DEBUG    all PCIe is HP Capable for debugging. Uncomment later
    //If Device is PciExpress, check if SLOT CAPABILITIES Register supports Hotplug
    if(PcieCheckPcieCompatible(Device) && (!Device->PciExpress->SlotCap.HpCapable)){
        DEBUG((DEBUG_INFO, "PciHP: Device @ B%X|D%X|F%X is NOT HP CAPABLE (SltCap.HpCap)\n", Device->Address.Addr.Bus,Device->Address.Addr.Device, Device->Address.Addr.Function));
        DEBUG((DEBUG_INFO, "DEBUG: Device @ B%X|D%X|F%X become HP CAPABLE (SltCap.HpCap)\n", Device->Address.Addr.Bus,Device->Address.Addr.Device, Device->Address.Addr.Function));
//        return FALSE;
    }
//DEBUG    
    DEBUG((DEBUG_INFO, "PciHP: Device @ B%X|D%X|F%X is HP CAPABLE returning TRUE\n", Device->Address.Addr.Bus,Device->Address.Addr.Device, Device->Address.Addr.Function));
    return TRUE;
}

/**
    This function will update pointer to PCI_RHPC_INFO of
    Bridge Type device which creates a hot plug bus. Also if "Device"
    creates the 'home bus' for Root HPC it will initialize Root HPC and
    record the HPC state;

     
    @param PCI_DEV_INFO    *Device     Pointer to PCI Device Private Data structure.

    
    @retval EFI_SUCCESS             When everything is going on fine!
    @retval EFI_NOT_FOUND           When Device not present in the system.
    @retval EFI_OUT_OF_RESOURCES    When system runs out of resources.
    @retval EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS HpCheckRootHotplug(PCI_DEV_INFO *Device, UINT8 MaxBusFound){
    EFI_STATUS            Status=EFI_SUCCESS;
    EFI_DEVICE_PATH_PROTOCOL *dp=NULL;
    PCI_HPC_INFO        *rhpc;
    UINTN                i;
//---------------
    if(Device->Type==tPci2PciBrg || Device->Type==tPci2CrdBrg || Device->Type==tPciRootBrg){
        //we have identify and Init all the Root Hpc and HPB
        if(Device->HostData->InitRhpcCount==Device->HostData->RhpcCount) return EFI_SUCCESS;
        PROGRESS_CODE(DXE_PCI_BUS_HPC_INIT);
        //here we must check if 2 things:
        //    1.If "Device" Device Path mutches with one of Root Hpc Device pathes
        //    2.If "Device" Device Path mutches with one of Root Hpb Device pathes
        for(i=0; i<Device->HostData->RhpcCount; i++){
            rhpc=Device->HostData->RhpcList[i];
            //if this controller already has been initialized - keep going
            if(rhpc->Initialized && rhpc->BusFound)    continue;


            //check if it is a secondary interface of the bridge where RHPC is sitting
            if(!rhpc->BusFound){
                if(!DPCmp(Device->DevicePath, rhpc->HpcLocation->HpbDevicePath)){
                    rhpc->HpbBridge=Device;
                    rhpc->BusFound=TRUE;
                    Device->HotPlug=rhpc;
                    DEBUG((DEBUG_INFO, "PciBus: Found HP Bus Bridge @ B%X|D%X|F%X \n\n", Device->Address.Addr.Bus,Device->Address.Addr.Device, Device->Address.Addr.Function));
                }
            }

            if(!rhpc->Initialized){
                //We will cut the last node of the HPC device path
                //resulting Device Path will correspond to the ParentBridge.DevicePath of the HPC
                dp=DPCut(rhpc->HpcLocation->HpcDevicePath);
                //if the HPC sits behind this bridge get the secondary I/F bus number
                if(!DPCmp(Device->DevicePath,dp)){
                    EFI_DEVICE_PATH_PROTOCOL    *tdp;
                //-----------------------------------
                    rhpc->HpcPciAddr.Addr.Bus=MaxBusFound;
                    tdp=DPGetLastNode(rhpc->HpcLocation->HpcDevicePath);

                    rhpc->HpcPciAddr.Addr.Device=((PCI_DEVICE_PATH*)tdp)->Device;
                    rhpc->HpcPciAddr.Addr.Function=((PCI_DEVICE_PATH*)tdp)->Function;

                    //we will not set up an event to signal when HPC initialization is ready
                    //that will give us a possibility to enumerate PCI BUS in one pass
                    Status=Device->HostData->HpInitProt->InitializeRootHpc(
                            Device->HostData->HpInitProt, rhpc->HpcLocation->HpcDevicePath,
                            rhpc->HpcPciAddr.ADDR, NULL, &rhpc->HpcState);

                    ASSERT_EFI_ERROR(Status);
                    rhpc->Owner=Device;
                    rhpc->Initialized=TRUE;
                    DEBUG((DEBUG_INFO, "PciBus: Found RHPC @ B%X|D%X|F%X \n", Device->Address.Addr.Bus,Device->Address.Addr.Device, Device->Address.Addr.Function));
                }
            }

            //if we come here we did some step of the initialization process
            //let's check if this controller has been completly initialized.
            //if so we did our job. there are no reasons to stay in this loop any longer
            if(rhpc->Initialized && rhpc->BusFound)    {
                Device->HostData->InitRhpcCount++;
                break;
            }

        }//for
    }
    if(dp)pBS->FreePool(dp);
    return Status;
}

/**
    This function will get and apply resource padding
    requirements for the PCI to PCI Bridge or Card Bus Bridge, if this
    Bridge is supports hot plug.

    
    @param PCI_DEV_INFO    *Device     Pointer to PCI Device Private Data structure.

    
    @retval EFI_SUCCESS             When everything is going on fine!
    @retval EFI_NOT_FOUND           When Device not present in the system.
    @retval EFI_OUT_OF_RESOURCES    When system runs out of resources.
    @retval EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS HpGetHpbResPadding(PCI_DEV_INFO *Device){
    EFI_STATUS        Status=EFI_SUCCESS;
//-----------------------------
    if(HpIsHpb(Device)){
        PCI_BRG_EXT        *ext=(PCI_BRG_EXT*)(Device+1);
        ASLR_QWORD_ASD    *cnf=NULL, *res=NULL;
        PCI_BAR            *bar;
    //----------------------------
        Status=Device->HostData->HpInitProt->GetResourcePadding(Device->HostData->HpInitProt,
                        Device->HotPlug->HpcLocation->HpcDevicePath,
                        Device->HotPlug->HpcPciAddr.ADDR,
                        (EFI_HPC_STATE *)&Device->HotPlug->HpcState,
                        (VOID **)&cnf,
                        (EFI_HPC_PADDING_ATTRIBUTES *)(UINTN)&ext->PaddAttr);

        ASSERT_EFI_ERROR(Status);
        if(EFI_ERROR(Status)) return Status;

        if(ext->PaddAttr==EfiPaddingPciRootBridge){
            pBS->FreePool(cnf);
            return Status;
        }
        
        DEBUG((DEBUG_INFO, "PciHp Padding: Processing QWORD RD for [B%X|D%X|F%X]:", Device->Address.Addr.Bus,Device->Address.Addr.Device, Device->Address.Addr.Function));
        //Check if we got a valid descriptors in cfg buffer
        if(!ValidateDescriptorBlock(cnf, tResAll, FALSE)){
            DEBUG((DEBUG_ERROR, " !!!INVALID DESCRIPTOR!!!\n"));
            return EFI_INVALID_PARAMETER;
        }

        DEBUG((DEBUG_INFO, "\n"));

        res=cnf;
        while(res->Hdr.HDR!=ASLV_END_TAG_HDR) {

            DEBUG((DEBUG_INFO, "\tType=0x%X; TFlags=0x%X; _MIN=0x%X; _MAX=0x%X; _LEN=0x%X; _GRA=0x%X;\n", 
                    res->Type,res->TFlags,res->_MIN,res->_MAX,res->_LEN,res->_GRA));

            
            bar=NULL;
            switch(res->Type){
                case ASLRV_SPC_TYPE_BUS    :
                    bar=&ext->Pad[rtBus];
                break;
                case ASLRV_SPC_TYPE_IO    :
                        bar=&ext->Pad[rtIo16];
                        bar->Type=tBarIo16;
                        if(res->_GRA==32){
                            bar=&ext->Pad[rtIo32];
                            bar->Type=tBarIo32;
                        }
                break;
                case ASLRV_SPC_TYPE_MEM    :
                    if(res->_GRA==32){
                        if(res->TFlags.MEM_FLAGS._MEM==ASLRV_MEM_CEPF)    {
                            bar=&ext->Pad[rtMmio32p];
                            bar->Type=tBarMmio32pf;
                        } else {
                            bar=&ext->Pad[rtMmio32];
                            bar->Type=tBarMmio32;
                        }
                    } else {
                        if(res->_GRA==64){
                            if(res->TFlags.MEM_FLAGS._MEM==ASLRV_MEM_CEPF){
                                bar=&ext->Pad[rtMmio64p];
                                bar->Type=tBarMmio64pf;
                            } else {
                                bar=&ext->Pad[rtMmio64];
                                bar->Type=tBarMmio64;
                            }
                        } else {
                            Status=EFI_INVALID_PARAMETER; //no other options alloud
                            goto ExitLbl;
                        }
                    }
                break;
                default: {
                    Status=EFI_INVALID_PARAMETER; //no other options alloud
                    goto ExitLbl;
                }
            } //switch

            bar->Length=res->_LEN;
            bar->Gran=res->_MAX;
            
            res++;
        } //while
ExitLbl:    
        if(cnf!=NULL)pBS->FreePool(cnf);
        DEBUG((DEBUG_INFO, "PciHp: Padding for [B%X|D%X|F%X] Updated...Status=%r\n", 
        Device->Address.Addr.Bus,Device->Address.Addr.Device, Device->Address.Addr.Function, Status));
        
    }

    return Status;
}


/**

    @param 

    @retval 

**/
BOOLEAN HpCheckHpCompatible(PCI_DEV_INFO *Device){
    if(Device->HotPlug!=NULL) return TRUE;
    else return FALSE;
}

/**

    @param 

    @retval 

**/
EFI_STATUS HpAllocateInitHpData(PCI_DEV_INFO *Device, UINT16 HpCapOffset){
    //we may process this controller as root HPC so check that.
    if(Device->HotPlug==NULL){
        Device->HotPlug=MallocZ(sizeof(PCI_HPC_INFO));
        ASSERT(Device->HotPlug);
        if(!Device->HotPlug) return EFI_OUT_OF_RESOURCES;

        //Save Current Offset of PCIE Std Compatibility Registers Block
        Device->HotPlug->HpcLocation->HpcDevicePath=Device->DevicePath;
        Device->HotPlug->HpcLocation->HpcDevicePath=Device->DevicePath;
        Device->HotPlug->HpcPciAddr.ADDR=Device->Address.ADDR;
        Device->HotPlug->HpbBridge=Device;
        Device->HotPlug->BusFound=TRUE;
        //For SHPC it must be initialized and enabled by hardware
        Device->HotPlug->HpcState=EFI_HPC_STATE_INITIALIZED|EFI_HPC_STATE_ENABLED;
        Device->HotPlug->HpcOffs=HpCapOffset;
        Device->HotPlug->Owner=Device;
    }

    return EFI_SUCCESS;
}


/**

    @param 

    @retval 

**/
EFI_STATUS HpLocateProtocolSortRhpc(PCI_HOST_INFO    *PciHpcHost, PCI_DEV_INFO    *RootBridge){
    EFI_STATUS                    Status;
    UINTN                        ghpccnt=0, j;
    EFI_HPC_LOCATION            *ghpclst=NULL;
    EFI_DEVICE_PATH_PROTOCOL    *rbdp;
//------------------------------
    //if we were there and not found what was looking for just exit...
    if(gRhpcNotFound==TRUE) return EFI_SUCCESS;

    if(gRhpcInitProtocol==NULL){
        Status=pBS->LocateProtocol(&gEfiPciHotPlugInitProtocolGuid, NULL, (VOID **)&gRhpcInitProtocol);
        if(EFI_ERROR(Status)){
            if(Status == EFI_NOT_FOUND){
                gRhpcNotFound=TRUE;
                return EFI_SUCCESS;
            } else return Status;
        }
    }

    //If we here, we got gRhpcInterface
    //It MUST be only one INSTANCE in the system, even if system has multiple hosts!
    //now we must initialize internal data with of PCI_HOST_DATA information.
    if(PciHpcHost->HpInitProt==NULL) PciHpcHost->HpInitProt=gRhpcInitProtocol;
    rbdp=RootBridge->DevicePath;

    //Get Root HPC List
    Status=gRhpcInitProtocol->GetRootHpcList(gRhpcInitProtocol,&ghpccnt,&ghpclst);
    ASSERT_EFI_ERROR(Status);


    //sort out Root Hot Plug Controlers list items to where they belongs
    DEBUG((DEBUG_INFO, "PciBus: Get Location - HpcLocCount=%d; RbDp=[HID(%X)UID(%X)]\n",
        ghpccnt,((ACPI_HID_DEVICE_PATH*)rbdp)->HID,((ACPI_HID_DEVICE_PATH*)rbdp)->UID));
    DEBUG((DEBUG_INFO, "-------------------------------------------------\n"));

    for (j=0; j<ghpccnt; j++){
        if( !MemCmp(rbdp, ghpclst[j].HpcDevicePath, sizeof(ACPI_HID_DEVICE_PATH)) ){
            PCI_HPC_INFO    *rhpc;
        //-----------------------------
            rhpc=MallocZ(sizeof(PCI_HPC_INFO));
            ASSERT(rhpc);
            if(!rhpc) return EFI_OUT_OF_RESOURCES;
            //Init PCI_ROOT_HPC structure
            rhpc->Root=TRUE;
            rhpc->HpcLocation=&ghpclst[j];

            DEBUG((DEBUG_INFO, "HpcDP=[HID(%X)UID(%X)]; HpbDP[HID(%X)UID(%X)]\n",
            ((ACPI_HID_DEVICE_PATH*)rhpc->HpcLocation->HpcDevicePath)->HID,((ACPI_HID_DEVICE_PATH*)rhpc->HpcLocation->HpcDevicePath)->UID,
            ((ACPI_HID_DEVICE_PATH*)rhpc->HpcLocation->HpbDevicePath)->HID,((ACPI_HID_DEVICE_PATH*)rhpc->HpcLocation->HpbDevicePath)->UID));

            Status=AppendItemLst((T_ITEM_LIST*)&PciHpcHost->RhpcInitCnt,rhpc);
            ASSERT_EFI_ERROR(Status);
        }
    }
    DEBUG((DEBUG_INFO, "-------------------------------------------------\n"));

    return Status;
}

/**

    @param 

    @retval 

**/
VOID HpClearPaddingData(PCI_BAR *PaddingBar){
    PCI_DEV_INFO        *owner;
//---------------------------
    //Here we about to clear PCI_BAR Object w/o touching Owner field.
    owner=PaddingBar->Owner;
    pBS->SetMem(PaddingBar, sizeof(PCI_BAR),0);
    PaddingBar->Owner=owner;
}


/**

    @param 

    @retval 

**/
EFI_STATUS HpApplyResPadding(PCI_BAR *PaddingBar, PCI_BAR *BridgeBar){

    if(PaddingBar->Length){
        if(PaddingBar->Length > BridgeBar->Length) BridgeBar->Length = PaddingBar->Length;
        if(BridgeBar->Gran < PaddingBar->Gran) BridgeBar->Gran=PaddingBar->Gran;
        BridgeBar->Type=PaddingBar->Type;
        return EFI_SUCCESS;
    }

    return EFI_UNSUPPORTED;
}


/**

    @param 

    @retval 

**/
EFI_STATUS HpApplyBusPadding(PCI_DEV_INFO *Brg, UINT8 OldMaxBus, UINT8 *MaxBusFound){
    PCI_BRG_EXT        *ext;
//-----------------------
    if(!HpCheckHpCompatible(Brg)) return EFI_SUCCESS;

    ext=(PCI_BRG_EXT*)(Brg+1);

    HpGetHpbResPadding(Brg);

    //Here we come with information about resources padding
    //Brg->Hotplug->Padd[rtBus].Length will have Bus numbers needed to pad for this bridge
    //mMaxBusFound will have number of buses actualy present behind this bridge
    //take care of buses now the rest of resources will be taken care of when
    //CalculateBridge resources will be called
    if(ext->PaddAttr==EfiPaddingPciBus){
        //EIP 19106 If 1 Bus set to padding in Setup it did not do the padding.
        if((ext->Pad[rtBus].Length!=0) && (ext->Pad[rtBus].Length > (*MaxBusFound)-ext->Res[rtBus].Base))
        {
            *MaxBusFound=OldMaxBus+(UINT8)ext->Pad[rtBus].Length;
        }
    }
    return EFI_SUCCESS;

}

/*
// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure:
//
// Description:
//
// Input:
//
// Output:
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS HpcUpdateLocationData(
                                PCI_HOST_BRG_DATA         *HostBrg, 
                                PCI_ROOT_BRG_DATA         *RootBrg, 
                                AMI_SDL_PCI_DEV_INFO    *HpBrgSdlData,
                                UINTN                    HpBrgIndex)
{
    EFI_STATUS                  Status;
    AMI_SDL_PCI_DEV_INFO        *parent;
    PCI_DEVICE_PATH             pcidp;
    EFI_DEVICE_PATH_PROTOCOL    *tmpdp;
    HPC_LOCATION_DATA           *locdata=NULL;
//-------------------------------------
    
    //If has not been allocated yet..
    if(HostBrg->HpcData==NULL){
        HostBrg->HpcData=MallocZ(sizeof(PCI_HPC_DATA));
        if(HostBrg->HpcData==NULL) {
            Status=EFI_OUT_OF_RESOURCES;
            ASSERT_EFI_ERROR(Status);
            return Status;
        }
    }
    
    //Initialize PCI Device Path variable we will not touch header...
    pcidp.Header.SubType=HW_PCI_DP;
    pcidp.Header.Type=HARDWARE_DEVICE_PATH;
    pcidp.Device=HpBrgSdlData->Device;
    pcidp.Function=HpBrgSdlData->Function;
    SET_NODE_LENGTH(&pcidp.Header,HW_PCI_DEVICE_PATH_LENGTH);
    
    //Get Parent of HP_BRG... 
    parent=&gSdlPciData->PciDevices[HpBrgSdlData->ParentIndex];
    
    //Generate the device path for the HPController\HPBus we have pci device path reserved there.
    //this will add END OF DP to the PCI_DP.
    tmpdp=DPAddNode(NULL, (EFI_DEVICE_PATH_PROTOCOL*)&pcidp);  
    if(tmpdp==NULL) {
        Status=EFI_OUT_OF_RESOURCES;
        ASSERT_EFI_ERROR(Status);
        return Status;
    }

    while (!parent->PciDevFlags.RootBridgeType){
        VOID                        *oldp=tmpdp;
        EFI_DEVICE_PATH_PROTOCOL    *ppcidp;
    //---------------------------------------
        //ppcidp=NULL;
        pcidp.Device=parent->Device;
        pcidp.Function=parent->Function;
        ppcidp=DPAddNode(NULL, (EFI_DEVICE_PATH_PROTOCOL*)&pcidp);
 
        if(ppcidp==NULL) {
            Status=EFI_OUT_OF_RESOURCES;
            ASSERT_EFI_ERROR(Status);
            return Status;
        }
                
        tmpdp=DPAdd(ppcidp, tmpdp);
        pBS->FreePool(ppcidp);

        if(tmpdp==NULL) {
            Status=EFI_OUT_OF_RESOURCES;
            ASSERT_EFI_ERROR(Status);
            return Status;
        }
        
        if(oldp!=NULL)pBS->FreePool(oldp);
        
        parent=&gSdlPciData->PciDevices[parent->ParentIndex];
        
        //should not come here but who knows.. if wrong porting...
        if(parent->ParentIndex==0) break;
    }

    //Here DevicePath where points "tmpdp" Is Ready but missing root bridge DP...
    //Get some Memory ....
    locdata=MallocZ(sizeof(HPC_LOCATION_DATA));
    if(locdata==NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        ASSERT_EFI_ERROR(Status);
        return Status;
    } else Status=EFI_SUCCESS;
                
    locdata->Owner=RootBrg;
    locdata->SdlHpBrgData=HpBrgSdlData;
    locdata->SdlHpBrgIndex=HpBrgIndex;
    
    //Now we have to generate PCI_DP down to parent Root Bridge of this HpBrgSdlData 
    //For SHPC and PCIExpress Hotplug HPC==HPB = tmpdp.
    //make a copy of generated DP into hpclocation buffer
    locdata->HpcLocation.HpcDevicePath=DPAdd(RootBrg->DevPath, tmpdp);            
    locdata->HpcLocation.HpbDevicePath=DPAdd(RootBrg->DevPath, tmpdp);            
    
    pBS->FreePool(tmpdp);

    if(locdata->HpcLocation.HpbDevicePath==NULL || locdata->HpcLocation.HpcDevicePath==NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        ASSERT_EFI_ERROR(Status);
        return Status;
    }

    Status=AppendItemLst((T_ITEM_LIST*)&HostBrg->HpcData->HpclInitCnt, locdata);
    ASSERT_EFI_ERROR(Status);
    
    //Update get Padding Setup Data.
    if(HpBrgSdlData->PciDevFlags.HasSetup){
//TODO         
//Add code to get custom setup data from Device Dedicated VAR_STORE
//TODO         
    } else {
        //Use SDL PCI DATA object for padding...
        locdata->HpSetup.ArrayField.BusPadd=HpBrgSdlData->ResourcePadding[ptBus];
        locdata->HpSetup.ArrayField.IoPadd=HpBrgSdlData->ResourcePadding[ptIo];
        locdata->HpSetup.ArrayField.Io32Padd=HpBrgSdlData->ResourcePadding[ptIo32];
        locdata->HpSetup.ArrayField.Mmio32Padd=HpBrgSdlData->ResourcePadding[ptMmio32];   
        locdata->HpSetup.ArrayField.Mmio32PfPadd=HpBrgSdlData->ResourcePadding[ptMmio32pf];
        locdata->HpSetup.ArrayField.Mmio64Padd=HpBrgSdlData->ResourcePadding[ptMmio64];
        locdata->HpSetup.ArrayField.Mmio64PfPadd=HpBrgSdlData->ResourcePadding[ptMmio64pf];
    }    
    
    return Status;
    
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:    HpcFindHpSlots
//
// Description:
//  This function find
//  information and initialize an instance of PCI Root Hotplug Controller
//  Initialization Protocol.
//
// Input:        PCI_HOST_BRG_DATA   *HostBrg - Pointer on Private Data
//              structure for which PCI Hot Plug Init Protocol going to
//              be initialized
//
// Output:        EFI_SUCCESS is OK
//              EFI_NOT_FOUND no Hotplug slots where found.
//
// Notes:        CHIPSET AND/OR BOARD PORTING NEEDED
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS HpcFindSlots(PCI_HOST_BRG_DATA *HostBrg, PCI_ROOT_BRG_DATA *RootBrg)
{
    EFI_STATUS              Status=EFI_NOT_FOUND;
    UINTN                   i,j, idx;
    AMI_SDL_PCI_DEV_INFO    *brgdata=NULL;
    AMI_SDL_PCI_DEV_INFO    *sdltmp=NULL;
    AMI_SDL_PCI_DEV_INFO    **sltdata;
    UINTN                   hpscnt, hpbcnt;
//-------------------------

    DEBUG((DEBUG_INFO, "---------------------------------------------------------------\n"));
    DEBUG((DEBUG_INFO, "PciHp: Looking for HP Bridges of RB '%a'; SdlIndex 0x%X(%d)...\n",
            RootBrg->RbAslName,RootBrg->RbSdlIndex,RootBrg->RbSdlIndex));
    
    for(i=0,hpbcnt=0; i<gSdlPciData->PciDevCount; i++){
        sdltmp=&gSdlPciData->PciDevices[i];
        sltdata=NULL;
        
        if( sdltmp->PciDevFlags.HotPlug && sdltmp->PciDevFlags.Pci2PciBridgeType){
            //This bridge produces HP Bus. Check if i has slots behind it...
            if(AmiSdlCheckIfParent(i, RootBrg->RbSdlIndex)){
                //So far... BRG with HP caps belongs to the right RB
                //Now check if it has slot s behind it and  how many
                //each of the slots will create an entry in HostBrg->HpcData. 
                brgdata=sdltmp;
                
                Status=AmiSdlPciGetChildsOfParentIndex(&sltdata,&hpscnt,i);
                if(EFI_ERROR(Status)){
                    if(Status==EFI_NOT_FOUND){
                        DEBUG((DEBUG_WARN, "\t!!!WARNING!!! HP Capable Bridge has no Children SdlIndex=0x%X(%d)!!!\n", i,i));
                        continue;
                    } else return Status;
                }
                
                for(j=0; j<hpscnt; j++){
                    //Try to fcheck if there are any slots behind the bridge..
                    sdltmp=sltdata[j];
                    AmiSdlFindRecordIndex(sdltmp,&idx);
                    if(sdltmp->PciDevFlags.OnBoard){
                        //It has to be slot!
                        DEBUG((DEBUG_WARN, "\t!!!WARNING Found OnBoard Device @HP Bus, SdlIndex 0x%X(%d)!!!\n", idx, idx));
                        continue;
                    } else {
                        //Found a SLOT behind HP cap bridge!
                        DEBUG((DEBUG_INFO, "\tFound Slot# 0x%X(%d), SdlIndex 0x%X(%d)...Updating HPData...",
                                sdltmp->Slot, sdltmp->Slot,idx,idx));
                        //Free memory allocated for the 
                        if(sltdata!=NULL) pBS->FreePool(sltdata);
                        //Now we have to generate PCI_DP down to this HP_BRG paren Root Bridge
                        Status=HpcUpdateLocationData(HostBrg,RootBrg,brgdata, i);
                        DEBUG((DEBUG_INFO, "%r.\n", Status));
                        ASSERT_EFI_ERROR(Status);
                        if(EFI_ERROR(Status))break;
                        hpbcnt++;
                        //One HPC_LOCATION per bridge.
                        break;
                    }                    
                }
                
            }//if AmiSdlCheckIfParent() 
        }
        if(EFI_ERROR(Status)&& Status!=EFI_NOT_FOUND )break;
    }
    
    //now get memory buffer to store HP
    DEBUG((DEBUG_INFO, "---------------------------------------------------------------\n"));
    DEBUG((DEBUG_INFO, "Total found %d P2P HP Bridges with HP Slots. Returning %r\n", hpbcnt, Status));

    return Status;

}
*/

/**
    GetRootHpcList() Function of the PCI Root Hotplug Controller
    Initialization Protocol. See PI 1.1 Spec or details

**/
EFI_STATUS HpcInitGetRootHpcList(
  IN EFI_PCI_HOT_PLUG_INIT_PROTOCOL      *This,
  OUT UINTN                              *HpcCount,
  OUT EFI_HPC_LOCATION                   **HpcList)
{
  *HpcCount = PCIE_NUM;
  *HpcList  = mPcieLocation;

  return EFI_SUCCESS;
}

/*
// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure:
//
// Description:
//
// Input:
//
// Output:
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS FindHpcLocData(  IN EFI_DEVICE_PATH_PROTOCOL *HpcDevicePath,
                            IN PCI_HPC_DATA             *HpcData,
                            OUT HPC_LOCATION_DATA       **HpcLocData )
{
    UINTN               i;
    HPC_LOCATION_DATA   *locdata;
//------------------

    for(i=0; i< HpcData->HpcLocCount; i++){
        locdata=HpcData->HpcLocData[i];
        if(DPCmp(HpcDevicePath, locdata->HpcLocation.HpcDevicePath) == 0 ){
            *HpcLocData=locdata;
            return EFI_SUCCESS;
        }
    }

    DEBUG((DEBUG_ERROR, "PciHp: Can't find matching HPC for DevicePath @ 0x%X \n",HpcDevicePath));

    return EFI_NOT_FOUND;

}
*/

/**
    InitializeRootHpc() Function of the PCI Root Hotplug Controller
    Initialization Protocol. See PI 1.1 Spec or details

    @note  
  This function call will be used to overwrite default resource Padding settings.
**/
EFI_STATUS HpcInitInitializeRootHpc(
  IN EFI_PCI_HOT_PLUG_INIT_PROTOCOL       *This,
  IN  EFI_DEVICE_PATH_PROTOCOL            *HpcDevicePath,
  IN  UINT64                              HpcPciAddress,
  IN  EFI_EVENT                             Event, OPTIONAL
  OUT EFI_HPC_STATE                        *HpcState)
{
  if (Event) {
    pBS->SignalEvent (Event);
  }

  *HpcState = EFI_HPC_STATE_INITIALIZED;

  return EFI_SUCCESS;
}

/**

    @param 

    @retval 

**/
EFI_STATUS HpcGetResourcePadding(
  IN  EFI_PCI_HOT_PLUG_INIT_PROTOCOL *This,
  IN  EFI_DEVICE_PATH_PROTOCOL       *HpcDevicePath,
  IN  UINT64                         HpcPciAddress,
  OUT EFI_HPC_STATE                  *HpcState,
  OUT VOID                           **Padding,
  OUT EFI_HPC_PADDING_ATTRIBUTES     *Attributes)
{
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *PaddingResource;
  EFI_STATUS                        Status;
  UINT8                             FindRP;
  UINT8                             RsvdExtraBusNum = 0;
  UINT16                            RsvdPcieMegaMem = 0;
  UINT8                             RsvdPcieKiloIo = 0;
  UINT16                            RsvdPcieMegaPFMem = 0;
  UINT8                             RsvdPcieMegaMemalig = 0;
  UINT8                             RsvdPcieMegaPFMemalig = 0;
  UINT64                            RpAddress;
  UINTN                             VariableSize;
  UINTN                             RpNumber;
#if defined(Thunderbolt_SUPPORT) && Thunderbolt_SUPPORT //AMI_OVERRIDE Code will build error after disabling "Thunderbolt_SUPPORT" - EIP262081
  //AMI_OVERRIDE_START Boot from Thunderbolt device fail - EIP260096
  UINTN                             TbtRpAddress;
  UINTN                             Dev;
  UINTN                             Fun;
  //AMI_OVERRIDE_END Boot from Thunderbolt device fail - EIP260096
#endif //AMI_OVERRIDE Code will build error after disabling "Thunderbolt_SUPPORT" - EIP262081
  UINTN                             RpDev;
  UINTN                             RpFun;
  SETUP_DATA                        SetupData;
  EFI_GUID                          SetupGuid = SETUP_GUID;

  PaddingResource = AllocatePool (PCIE_NUM * sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) + sizeof (EFI_ACPI_END_TAG_DESCRIPTOR));
  ASSERT (PaddingResource != NULL);

  *Padding = (VOID *) PaddingResource;

  VariableSize = sizeof(SETUP_DATA);
  Status = pRS->GetVariable(L"Setup", &SetupGuid, NULL,
                            &VariableSize, &SetupData );
  if(EFI_ERROR(Status)) return EFI_SUCCESS;

  //
  // Check if PCIe Root Hob Controller need to reserve bus for docking downstream P2P hotplug
  //
  for (FindRP = 0; FindRP < PCIE_NUM; FindRP ++) {
    if (FindRP < SB_PORT8_NUM) {
      // SB PCIE root port 1-8
      RpAddress = EFI_PCI_ADDRESS (0, PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1, FindRP, 0);
      RpDev = PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1;
      RpFun = FindRP;
    } else if(FindRP >= SB_PORT8_NUM && FindRP < SB_PORT16_NUM) {
      // SB PCIE root port 9-16
      RpAddress = EFI_PCI_ADDRESS (0, PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2, (FindRP - SB_PORT8_NUM), 0);
      RpDev = PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2;
      RpFun = FindRP - SB_PORT8_NUM;
    } else if(FindRP >= SB_PORT16_NUM && FindRP < SB_PORT24_NUM) {
      // SB PCIE root port 17-24
      RpAddress = EFI_PCI_ADDRESS (0, PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_3, (FindRP - SB_PORT16_NUM), 0);
      RpDev = PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_3;
      RpFun = FindRP - SB_PORT16_NUM;
    } else {
      // NB PCIE root port 1-3
      RpAddress = EFI_PCI_ADDRESS (0, SA_PEG_DEV_NUM, (FindRP - SB_PORT24_NUM), 0);
      RpDev = SA_PEG_DEV_NUM;
      RpFun = FindRP - SB_PORT24_NUM;
    }
    
    if (HpcPciAddress == RpAddress) {
      if (FindRP < SB_PORT24_NUM) {
        //
        // PCH Rootports Hotplug Parameters Passing.
        //
        DEBUG((DEBUG_INFO, "Hotplug root port is SB PCIE root port %d (0-based)\n", FindRP));
        
        // Get PCIE root port swap status
        Status = GetPchPcieRpNumber(RpDev, RpFun, &RpNumber);
        if (!EFI_ERROR(Status)) {
          if (SetupData.PcieRootPortHPE[RpNumber]) {
            RsvdExtraBusNum       = SetupData.PcieExtraBusRsvd[RpNumber];
            RsvdPcieMegaMem       = SetupData.PcieMemRsvd[RpNumber];
            RsvdPcieKiloIo        = SetupData.PcieIoRsvd[RpNumber];
#if defined(SB_SETUP_SUPPORT) && (SB_SETUP_SUPPORT == 1)
            RsvdPcieMegaPFMem     = SetupData.PciePFMemRsvd[RpNumber];
            RsvdPcieMegaMemalig   = SetupData.PcieMemRsvdalig[RpNumber];
            RsvdPcieMegaPFMemalig = SetupData.PciePFMemRsvdalig[RpNumber];
#else
            RsvdPcieMegaPFMem     = 0x0A;
            RsvdPcieMegaMemalig   = 1;
            RsvdPcieMegaPFMemalig = 1;
#endif
          }
        } // end if
      } else {
        //
        // PEG Rootports Hotplug Parameters Passing.
        //
        DEBUG((DEBUG_INFO, "Hotplug root port is PEG_%d:", (FindRP - SB_PORT24_NUM) ));
        
        // Pass PEG number by RpFun value.
        RpNumber = RpFun;
        if (SetupData.PegRootPortHPE[RpNumber]) {
          RsvdExtraBusNum = SetupData.PegExtraBusRsvd[RpNumber];
          RsvdPcieMegaMem = SetupData.PegMemRsvd[RpNumber];
          RsvdPcieMegaPFMem = SetupData.PegMemRsvd[RpNumber];                       // [ EIP440130 ]
          RsvdPcieKiloIo  = SetupData.PegIoRsvd[RpNumber];
          DEBUG((DEBUG_INFO, "\n"));
          DEBUG((DEBUG_INFO, "  RsvdExtraBusNum = 0x%08X\n", RsvdExtraBusNum));
          DEBUG((DEBUG_INFO, "  RsvdPcieMegaMem = 0x%08X\n", RsvdPcieMegaMem));
          DEBUG((DEBUG_INFO, "  RsvdPcieMegaPFMem = 0x%08X\n", RsvdPcieMegaPFMem)); // [ EIP440130 ]
          DEBUG((DEBUG_INFO, "  RsvdPcieKiloIo  = 0x%08X\n", RsvdPcieKiloIo));
        } else {
          DEBUG((DEBUG_INFO, " Unsupported.\n", RsvdExtraBusNum));
        }
      }
    } // end if  for PCIE/PEG root port

#if defined(Thunderbolt_SUPPORT) && Thunderbolt_SUPPORT
    //AMI_OVERRIDE_START Boot from Thunderbolt device fail - EIP260096
    if ((SetupData.TbtSupport == 1) && (HpcPciAddress == RpAddress)) {
      //Get the Root Port address of Thunderbolt
      Status = GetTbtRpDevFun ((SetupData.TbtSelector - 1), &Dev, &Fun);
      TbtRpAddress = EFI_PCI_ADDRESS (0, Dev, Fun, 0);

      if (HpcPciAddress == TbtRpAddress) {
        //AMI_OVERRIDE_END Boot from Thunderbolt device fail - EIP260096
        RsvdExtraBusNum    = SetupData.TbtPcieExtraBusRsvd;
        RsvdPcieMegaMem    = SetupData.TbtPcieMemRsvd;
        RsvdPcieMegaPFMem   = SetupData.TbtPciePMemRsvd;
        RsvdPcieKiloIo     = SetupData.TbtPcieIoRsvd;
        RsvdPcieMegaMemalig  = SetupData.TbtPcieMemAddrRngMax;
        RsvdPcieMegaPFMemalig = SetupData.TbtPciePMemAddrRngMax;

        DEBUG((DEBUG_INFO, "\nUpdate Resource for Thunderbolt used.\n"));
        DEBUG((DEBUG_INFO, "  RsvdExtraBusNum = 0x%08x\n", RsvdExtraBusNum));
        DEBUG((DEBUG_INFO, "  RsvdPcieMegaMem = 0x%08x\n", RsvdPcieMegaMem));
        DEBUG((DEBUG_INFO, "  RsvdPcieMegaPFMem = 0x%08x\n", RsvdPcieMegaPFMem));
        DEBUG((DEBUG_INFO, "  RsvdPcieKiloIo = 0x%08x\n", RsvdPcieKiloIo));
        DEBUG((DEBUG_INFO, "  RsvdPcieMegaMemalig = 0x%08x\n", RsvdPcieMegaMemalig));
        DEBUG((DEBUG_INFO, "  RsvdPcieMegaPFMemalig = 0x%08x\n", RsvdPcieMegaPFMemalig));
      }
    } //AMI_OVERRIDE Boot from Thunderbolt device fail - EIP260096
#endif
  } // end of for loop

  //
  // Padding for bus
  //
  ZeroMem (PaddingResource, PCIE_NUM * sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) + sizeof (EFI_ACPI_END_TAG_DESCRIPTOR));
  *Attributes                   = EfiPaddingPciBus;

  PaddingResource->Desc         = 0x8A;
  PaddingResource->Len          = 0x2B;
  PaddingResource->ResType      = ACPI_ADDRESS_SPACE_TYPE_BUS;
  PaddingResource->GenFlag      = 0x0;
  PaddingResource->SpecificFlag = 0;
  PaddingResource->AddrRangeMin = 0;
  PaddingResource->AddrRangeMax = 0;
  PaddingResource->AddrLen      = RsvdExtraBusNum;

  //
  // Padding for non-prefetchable memory
  //
  PaddingResource++;
  PaddingResource->Desc                 = 0x8A;
  PaddingResource->Len                  = 0x2B;
  PaddingResource->ResType              = ACPI_ADDRESS_SPACE_TYPE_MEM;
  PaddingResource->GenFlag              = 0x0;
  PaddingResource->AddrSpaceGranularity = 32;
  PaddingResource->SpecificFlag         = 0;
  //
  // Pad non-prefetchable
  //
  PaddingResource->AddrRangeMin = 0;
  PaddingResource->AddrLen      = RsvdPcieMegaMem * 0x100000;
  PaddingResource->AddrRangeMax = (1 << RsvdPcieMegaMemalig) - 1;  // 0x3FFFFFF

  //
  // Padding for prefetchable memory
  //
  PaddingResource++;
  PaddingResource->Desc                 = 0x8A;
  PaddingResource->Len                  = 0x2B;
  PaddingResource->ResType              = ACPI_ADDRESS_SPACE_TYPE_MEM;
  PaddingResource->GenFlag              = 0x0;
  if(SetupData.EnableAbove4GBMmio == 1)
      PaddingResource->AddrSpaceGranularity = 64;
  else
      PaddingResource->AddrSpaceGranularity = 32;
  PaddingResource->SpecificFlag         = 06;
  //
  // Padding for prefetchable memory
  //
  PaddingResource->AddrRangeMin = 0;
  PaddingResource->AddrLen      = RsvdPcieMegaPFMem * 0x100000;
  //
  // Pad 16 MB of MEM
  //
  PaddingResource->AddrRangeMax = (1 << RsvdPcieMegaPFMemalig) - 1; // 0xfffffff
  //
  // Alignment
  //
  // Padding for I/O
  //
  PaddingResource++;
  PaddingResource->Desc         = 0x8A;
  PaddingResource->Len          = 0x2B;
  PaddingResource->ResType      = ACPI_ADDRESS_SPACE_TYPE_IO;
  PaddingResource->GenFlag      = 0x0;
  PaddingResource->SpecificFlag = 0;
  PaddingResource->AddrRangeMin = 0;
  PaddingResource->AddrLen      = RsvdPcieKiloIo * 0x400;
  //
  // Pad 4K of IO
  //
  PaddingResource->AddrRangeMax = 1;
  //
  // Alignment
  //
  // Terminate the entries.
  //
  PaddingResource++;
  ((EFI_ACPI_END_TAG_DESCRIPTOR *) PaddingResource)->Desc     = ACPI_END_TAG_DESCRIPTOR;
  ((EFI_ACPI_END_TAG_DESCRIPTOR *) PaddingResource)->Checksum = 0x0;

  *HpcState = EFI_HPC_STATE_INITIALIZED | EFI_HPC_STATE_ENABLED;

  return EFI_SUCCESS;
}


/**
    This function will collect all information HP related
    information and initialize an instance of PCI Root Hotplug Controller
    Initialization Protocol.

    @param PCI_HOST_BRG_DATA   *HostBrg - Pointer on Private Data
        structure for which PCI Hot Plug Init Protocol going to
        be initialized

    @retval EFI_SUCCESS is OK
    @retval EFI_NOT_FOUND no Hotplug slots where found.

    @note  CHIPSET AND/OR BOARD PORTING NEEDED
**/
EFI_STATUS HpcInstallHpProtocol(PCI_HOST_BRG_DATA *HostBrg)
{
  EFI_STATUS            Status;
  PCI_HOT_PLUG_INSTANCE *PciHotPlug;

  PciHotPlug = AllocatePool (sizeof (PCI_HOT_PLUG_INSTANCE));
  ASSERT (PciHotPlug != NULL);

  //
  // Initialize driver private data.
  //
  ZeroMem (PciHotPlug, sizeof (PCI_HOT_PLUG_INSTANCE));

  PciHotPlug->HotPlugInitProtocol.GetRootHpcList      = HpcInitGetRootHpcList;
  PciHotPlug->HotPlugInitProtocol.InitializeRootHpc   = HpcInitInitializeRootHpc;
  PciHotPlug->HotPlugInitProtocol.GetResourcePadding  = HpcGetResourcePadding;

  Status = pBS->InstallProtocolInterface (
                  &PciHotPlug->Handle,
                  &gEfiPciHotPlugInitProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &PciHotPlug->HotPlugInitProtocol
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

