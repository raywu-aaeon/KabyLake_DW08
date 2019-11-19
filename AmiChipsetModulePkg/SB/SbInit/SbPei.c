//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file SbPei.c
    This file contains code for Template Southbridge initialization
    in the PEI stage

**/

// Module specific Includes
#include <Efi.h>
#include <Pei.h>
#include <Token.h>
#include <AmiPeiLib.h>
#include <Hob.h>
#include <Setup.h>
#include <AmiCspLib.h>
#include <Guid/Rtc.h>
#include <Guid/AmiCspGlobalNvs.h>
// Produced/used PPI interfaces
#include <Ppi/PciCfg2.h>
#include <Ppi/SbPpi.h>
#include <Ppi/CpuIo.h>
#include <Ppi/CspLibPpi.h>
#include <Ppi/SmmControl.h>
#include "Guid/Rtc.h"
#include <SbSetupData.h>
#include <NbSetupData.h>
#include <PchAccess.h>
#include <SbPeiLib.h>
//#include <Ppi\PchUsbPolicy\PchUsbPolicy.h>
//#include <Ppi\PchInit\PchInit.h>
#include <Guid/MemoryTypeInformation.h>
#include <Library/PchInfoLib.h>
#include <Library/DebugLib.h>
#include <Library/AmiSbMiscLib.h>
#include <Ppi/SiPolicy.h>
#include <Library/PchCycleDecodingLib.h>

#if defined   iME_SUPPORT && iME_SUPPORT
#include <MeBiosExtensionSetup.h>
#endif

#if SB_RESET_PPI_SUPPORT
#include <Ppi/Reset.h>
#endif

#if ATAPI_RECOVERY_SUPPORT
#include <Ppi/AtaController.h>
#endif

#if SB_STALL_PPI_SUPPORT
#include <Ppi/Stall.h>
#endif

#if WdtPei_SUPPORT
#include "Ppi/Wdt.h"
#endif
#include <Ppi/NbPpi.h>

#if Capsule_SUPPORT
#include <Ppi/Capsule.h>    //CAPSULE20
#include <Guid/CapsuleVendor.h>
#endif

#if defined AhciRecovery_SUPPORT && AhciRecovery_SUPPORT
#include    <Ppi/AhciControllerRecovery.h>
#endif

#if defined(POST_TIME_TUNING_SUPPORT) && POST_TIME_TUNING_SUPPORT && (PTT_VER > 3) && SUPPORT_RAID_DRIVER
#include <Pci.h>
#include <FastBoot2.h>
#endif

#if defined(LSIPCIEDEVICE_SUPPORT) && LSIPCIEDEVICE_SUPPORT == 1
#include <Pci.h>
#include <Library/PeiServicesLib.h>
#endif

#if defined(SgTpv_SUPPORT) && (SgTpv_SUPPORT == 1)
#define MmPciAddress(Base, Bus, Device, Function, Register) \
        ( (UINTN)(Base) + \
          (UINTN)(Bus << 20) + \
          (UINTN)(Device << 15) + \
          (UINTN)(Function << 12) + \
          (UINTN)(Register) )
#endif
//----------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//----------------------------------------------------------------------------
// Constant Definition(s)
#define SATA1_BDF (0x1f << 3 | 0x02)
#define SATA2_BDF (0x1f << 3 | 0x05)
#define TIMER_RESOLUTION  1
#define S3_SLP_TYP        0x05

#ifndef SMM_SUPPORT
  #define SMM_SUPPORT   0
#endif

// Macro Definition(s)
#if defined(LSIPCIEDEVICE_SUPPORT) && LSIPCIEDEVICE_SUPPORT == 1
#define SB_PCIE_CFG_ADDRESS(bus, dev, func, reg) \
            ((UINTN)(PCIEX_BASE_ADDRESS + ((UINT8)(bus) << 20) + \
            ((UINT8)(dev) << 15) + ((UINT8)(func) << 12) + (reg)))
#endif

// Type Definition(s)

// Function Prototype(s)

#if     SMM_SUPPORT
EFI_STATUS 
EFIAPI
PeiActivate (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN PEI_SMM_CONTROL_PPI      *This,
    IN OUT INT8                 *ArgumentBuffer OPTIONAL,
    IN OUT UINTN                *ArgumentBufferSize OPTIONAL,
    IN BOOLEAN                  Periodic OPTIONAL,
    IN UINTN                    ActivationInterval OPTIONAL
);
  
EFI_STATUS 
EFIAPI
PeiDeactivate (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN PEI_SMM_CONTROL_PPI      *This,
    IN BOOLEAN                  Periodic OPTIONAL
);
#endif

#if SB_RESET_PPI_SUPPORT
EFI_STATUS EFIAPI SbPeiResetSystem (
    IN EFI_PEI_SERVICES         **PeiServices
);
#endif

#if SB_STALL_PPI_SUPPORT
EFI_STATUS EFIAPI SbPeiStall (
    IN EFI_PEI_SERVICES         **PeiServices, 
    IN EFI_PEI_STALL_PPI        *This,
    IN UINTN                    Microseconds
);
#endif

#if ATAPI_RECOVERY_SUPPORT
EFI_STATUS EFIAPI EnableAtaChannel (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN PEI_ATA_CONTROLLER_PPI   *This,
    IN UINT8                    ChannelIndex
);
#endif

#if defined AhciRecovery_SUPPORT && AhciRecovery_SUPPORT
#if !defined(AHCIREC_DRIVER_VERSION) || (AHCIREC_DRIVER_VERSION < 4)
EFI_STATUS
EFIAPI
RecInitSataController (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN PEI_AHCI_CONTROLLER_PPI        *This
);
#else
EFI_STATUS
EFIAPI
GetSbSataLocation (
  IN  EFI_PEI_SERVICES             **PeiServices,
  IN  PEI_AHCI_CONTROLLER_2_PPI    *This,
  IN OUT UINT8                     *Bus,
  IN OUT UINT8                     *Device,
  IN OUT UINT8                     *Function
);

EFI_STATUS
EFIAPI
RecInitSbSataController (
  IN  EFI_PEI_SERVICES             **PeiServices,
  IN  PEI_AHCI_CONTROLLER_2_PPI    *This,
  IN  UINT32                       AhciMmioBaseAddress
);
#endif
#endif

VOID InitPCIe (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN EFI_PEI_CPU_IO_PPI       *CpuIo,
    IN EFI_PEI_PCI_CFG2_PPI     *PciCfg
);

VOID InitSMBus (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN EFI_PEI_CPU_IO_PPI       *CpuIo,
    IN EFI_PEI_PCI_CFG2_PPI     *PciCfg
);

VOID InitUsbMisc (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN EFI_PEI_CPU_IO_PPI       *CpuIo,
    IN EFI_PEI_PCI_CFG2_PPI     *PciCfg,
    IN EFI_BOOT_MODE            BootMode
);

EFI_STATUS EFIAPI ProgramSBRegAfterMemInstalled (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
    IN VOID                     *InvokePpi
);

EFI_STATUS EFIAPI ProgramSBRegBeforeEndofPei (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
    IN VOID                     *InvokePpi
);
  
EFI_STATUS EFIAPI ProgramSBRegBeforeMrc (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
    IN VOID                     *InvokePpi
);

EFI_STATUS EFIAPI ProgramSBRegAfterMrc (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
    IN VOID                     *InvokePpi
);

EFI_STATUS EFIAPI ProgramSBRegEndOfMrc (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
    IN VOID                     *InvokePpi
);

VOID InitPMRegs(
    IN EFI_PEI_SERVICES         **PeiServices,
    IN EFI_PEI_CPU_IO_PPI       *CpuIo,
    IN SB_SETUP_DATA            *SbSetupData
);

VOID PeiInitRtc(
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_CPU_IO_PPI           *CpuIo
);

VOID ProgramRCRBMmio (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN EFI_PEI_CPU_IO_PPI       *CpuIo 
);

EFI_STATUS
ProgramSBIoDecodeRegs (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN EFI_PEI_PCI_CFG2_PPI     *PciCfg
);

EFI_STATUS
ProgramPchDeviceBase (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN EFI_PEI_PCI_CFG2_PPI      *PciCfg
);

EFI_STATUS
GeneralPowerFailureHandler (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN EFI_PEI_CPU_IO_PPI       *CpuIo,
    IN EFI_PEI_PCI_CFG2_PPI     *PciCfg
);

EFI_STATUS
SetTheStateToGoAfterG3 (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN EFI_PEI_CPU_IO_PPI       *CpuIo,
    IN EFI_PEI_PCI_CFG2_PPI     *PciCfg,
    IN SB_SETUP_DATA            *SbSetupData
);

#if defined(LSIPCIEDEVICE_SUPPORT) && LSIPCIEDEVICE_SUPPORT ==1
EFI_STATUS
LSIDevOnEndOfPei (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDesc,
  IN VOID                               *Ppi
);
#endif

VOID SbWakeupTypeLib (
    VOID
);

VOID SbCheckPowerLoss (
    VOID
);

//----------------------------------------------------------------------------
// Variable and External Declaration(s)
//----------------------------------------------------------------------------
// Variable Declaration(s)

// GUID Definition(s)
EFI_GUID gAmiPEISBInitPolicyGuid     = AMI_PEI_SBINIT_POLICY_PPI_GUID;
//EFI_GUID gMasterBootModeGuid         = EFI_PEI_MASTER_BOOT_MODE_PEIM_PPI;
EFI_GUID gRecoveryBootModeGuid       = EFI_PEI_BOOT_IN_RECOVERY_MODE_PEIM_PPI;
EFI_GUID gEfiPeiPermMemInstalledGuid = EFI_PEI_PERMANENT_MEMORY_INSTALLED_PPI;

EFI_GUID gEfiPeiEndOfPeiPhasePpiGuid = EFI_PEI_END_OF_PEI_PHASE_PPI_GUID;
#if SB_STALL_PPI_SUPPORT
EFI_GUID gStallPpiGuid = EFI_PEI_STALL_PPI_GUID;
#endif

#if SB_RESET_PPI_SUPPORT
EFI_GUID gPeiResetPpiGuid = EFI_PEI_RESET_PPI_GUID;
#endif

#if WdtPei_SUPPORT
EFI_GUID gWdtPpiGuid = WDT_PPI_GUID;
#endif

#if defined AhciRecovery_SUPPORT && AhciRecovery_SUPPORT
#if !defined(AHCIREC_DRIVER_VERSION) || (AHCIREC_DRIVER_VERSION < 4)
EFI_GUID gPeiAhciControllerPpiGuid   = PEI_AHCI_CONTROLLER_PPI_GUID;
#else
EFI_GUID gPeiAhciController2PpiGuid   = PEI_AHCI_CONTROLLER_2_PPI_GUID;
#endif
#endif

#if defined(LSIPCIEDEVICE_SUPPORT) && LSIPCIEDEVICE_SUPPORT == 1
EFI_GUID gAmiLSIPathGuid = {0xC206DDF8, 0xB4D6, 0x4A5D, 0xB1, 0xE3, 0x74, 0x56, 0xEF, 0xB0, 0xB1, 0xF8};
#endif

EFI_GUID gAmiPeiSBCustomPpiGuid = AMI_PEI_SB_CUSTOM_PPI_GUID;
// PPI Definition(s)

static AMI_PEI_SBINIT_POLICY_PPI mAmiPeiSbInitPolicyPpi = {
    TRUE
};

#if ATAPI_RECOVERY_SUPPORT
static PEI_ATA_CONTROLLER_PPI mAtaControllerPpi = {
    EnableAtaChannel 
};
#endif

#if     SMM_SUPPORT
static PEI_SMM_CONTROL_PPI mSmmControlPpi = {
    PeiActivate,
    PeiDeactivate
};
#endif

#if SB_STALL_PPI_SUPPORT
static EFI_PEI_STALL_PPI mStallPpi = {
    (UINTN) TIMER_RESOLUTION,
    (EFI_PEI_STALL) SbPeiStall
};
#endif

#if SB_RESET_PPI_SUPPORT
static EFI_PEI_RESET_PPI mResetPpi = { 
    (EFI_PEI_RESET_SYSTEM) SbPeiResetSystem
};
#endif

#if defined AhciRecovery_SUPPORT && AhciRecovery_SUPPORT
#if !defined(AHCIREC_DRIVER_VERSION) || (AHCIREC_DRIVER_VERSION < 4)
static PEI_AHCI_CONTROLLER_PPI mAhciControllerPpi =
  {
	RecInitSataController
           
  };
#else
static PEI_AHCI_CONTROLLER_2_PPI mAhciController2Ppi =
  {
    GetSbSataLocation,
    RecInitSbSataController       
  };
#endif
#endif

// PPI that are installed

#if SB_STALL_PPI_SUPPORT
static EFI_PEI_PPI_DESCRIPTOR mBeforeBootModePpiList[] = {
    { EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
      &gStallPpiGuid, &mStallPpi },
};
#endif

static EFI_PEI_PPI_DESCRIPTOR mPpiList[] = {
#if ATAPI_RECOVERY_SUPPORT
    { EFI_PEI_PPI_DESCRIPTOR_PPI, \
      &gPeiAtaControllerPpiGuid, &mAtaControllerPpi },
#endif
#if defined AhciRecovery_SUPPORT && AhciRecovery_SUPPORT    
#if !defined(AHCIREC_DRIVER_VERSION) || (AHCIREC_DRIVER_VERSION < 4)
    { EFI_PEI_PPI_DESCRIPTOR_PPI, \
      &gPeiAhciControllerPpiGuid, &mAhciControllerPpi },
#else      
    { EFI_PEI_PPI_DESCRIPTOR_PPI, \
      &gPeiAhciController2PpiGuid, &mAhciController2Ppi },      
#endif      
#endif
#if SB_RESET_PPI_SUPPORT
    { EFI_PEI_PPI_DESCRIPTOR_PPI, &gPeiResetPpiGuid, &mResetPpi },
#endif
    { EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
      &gAmiPEISBInitPolicyGuid, &mAmiPeiSbInitPolicyPpi }
};

#if SMM_SUPPORT
static EFI_PEI_PPI_DESCRIPTOR mAmiPeiSmmControlPpiList[] = {
    { EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
      &gPeiSmmControlPpiGuid, &mSmmControlPpi }
};
#endif

// PPI that are notified

static EFI_PEI_NOTIFY_DESCRIPTOR mNotifyList[] = {
    { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | \
      EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
      &gEfiPeiEndOfPeiPhasePpiGuid, ProgramSBRegBeforeEndofPei },
};

static EFI_PEI_NOTIFY_DESCRIPTOR mMemoryReadyNotify[] = {
    { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | \
      EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
      &gEfiPeiPermMemInstalledGuid, ProgramSBRegAfterMemInstalled }
};

static EFI_PEI_NOTIFY_DESCRIPTOR BeforeMrcNotifyList[] = {
    { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | \
      EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
      &gAmiPeiBeforeMrcGuid, ProgramSBRegBeforeMrc },
};

static EFI_PEI_NOTIFY_DESCRIPTOR AfterMrcNotifyList[] = {
    { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | \
      EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
      &gAmiPeiAfterMrcGuid, ProgramSBRegAfterMrc },
};

static EFI_PEI_NOTIFY_DESCRIPTOR EndOfMrcNotifyList[] = {
    { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | \
      EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
     &gAmiPeiEndOfMemDetectGuid, ProgramSBRegEndOfMrc },
};

#if defined(LSIPCIEDEVICE_SUPPORT) && LSIPCIEDEVICE_SUPPORT == 1
GLOBAL_REMOVE_IF_UNREFERENCED EFI_PEI_NOTIFY_DESCRIPTOR  mLSIDevNotifyList[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiEndOfPeiSignalPpiGuid,
    LSIDevOnEndOfPei
  }
};
#endif

// External Declaration(s)
extern EFI_STATUS CountTime (
    IN UINTN            DelayTime,
    IN UINT16           BaseAddr
);

#if SB_RESET_PPI_SUPPORT
extern VOID SBLib_ResetSystem (
    IN EFI_RESET_TYPE   ResetType
);
#endif

#if defined(LSIPCIEDEVICE_SUPPORT) && LSIPCIEDEVICE_SUPPORT == 1
EFI_STATUS
LSIDevOnEndOfPei (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDesc,
  IN VOID                               *Ppi
  )
{
	EFI_PEI_READ_ONLY_VARIABLE2_PPI *ReadOnlyVariable;
	UINT32                   LSIDevPath;
	UINT32                   LSIDataSize = sizeof(UINT32);
	UINT32                   *LSIPointer = NULL;
	EFI_STATUS               Status;
	UINT32                   Count;
	UINT32                   BusNum,DevNum,FunNum;
	UINT32                   Length = 0;
	UINT32                   i;
	
	DEBUG ((-1, "LSIDevOnEndOfPei - Start\n"));

        Status = (*PeiServices)->LocatePpi( (CONST EFI_PEI_SERVICES**)PeiServices, \
        		&gEfiPeiReadOnlyVariable2PpiGuid, \
        		0, \
        		NULL, \
        		(VOID **) &ReadOnlyVariable );
        if (!EFI_ERROR(Status)) {
        	Status = ReadOnlyVariable->GetVariable( ReadOnlyVariable, \
        			L"LSIDevice", \
        			&gAmiLSIPathGuid, \
        			NULL, \
        			&LSIDataSize, \
        			&LSIDevPath );
        	if (!EFI_ERROR(Status)) {
			// PciPcieAddr Structure for one LSI Device :  (_1_) (_2_) ... (_N_) 
			// (1) Device Path Length
			// (2) LSI Device (Bus/Dev/Fun)
			//   :
			// (N) Parent Bridge (Bus/Dev/Fun)
        		LSIPointer = (UINT32 *) LSIDevPath;
        		while((*(LSIPointer + Length) != 0xFF)){
        			for(Count = (Length + *(LSIPointer + Length)-1); Count > (Length + 1); Count--){
        				BusNum = *(LSIPointer + Count) >> 16;
        				DevNum = (*(LSIPointer + Count) &0xFF00) >> 8;
        				FunNum = *(LSIPointer + Count) & 0xFF;
        				DEBUG((-1, "Device BusNum : %x, DevNum : %x, FunNum : %x\n",BusNum, DevNum, FunNum));
        				MmioWrite8(SB_PCIE_CFG_ADDRESS(BusNum,DevNum,FunNum,PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET), (UINT8) (*(LSIPointer + Count - 1) >> 16));
        				MmioWrite8(SB_PCIE_CFG_ADDRESS(BusNum,DevNum,FunNum,PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET), (UINT8) (*(LSIPointer + Count - 1) >> 16));
        			}
        			BusNum = *(LSIPointer + Count) >> 16;
        			DevNum = (*(LSIPointer + Count) &0xFF00) >> 8;
        			FunNum = *(LSIPointer + Count) & 0xFF;
        			DEBUG((-1, " Before PCI_CMD : %x\n",MmioRead16(SB_PCIE_CFG_ADDRESS(BusNum,DevNum,FunNum,PCI_CMD))));
        			MmioWrite16(SB_PCIE_CFG_ADDRESS(BusNum,DevNum,FunNum,PCI_CMD), BIT1 + BIT2); // Enable MSE and BME
        			DEBUG((-1, "Device BusNum : %x, DevNum : %x, FunNum : %x\n",BusNum, DevNum, FunNum));
        			DEBUG((-1, "After PCI_CMD : %x\n",MmioRead16(SB_PCIE_CFG_ADDRESS(BusNum,DevNum,FunNum,PCI_CMD))));
        			Length += *(LSIPointer + Length);
        		}
        		DEBUG((-1, "All LSI device Length : %x\n",Length));
        		for(i = 0x0; i < Length ; i++){
        			DEBUG((-1, "LSIDevice[%x] : %x\n",i , *(LSIPointer + i)));
        		}
        	}
        }
        DEBUG ((-1, "LSIDevOnEndOfPei - END\n"));
        
	return Status;
}
#endif

#if defined(POST_TIME_TUNING_SUPPORT) && POST_TIME_TUNING_SUPPORT && (PTT_VER > 3) && SUPPORT_RAID_DRIVER
/**
    This function provides SATA Port Information

    @param PeiServices - Pointer to the PEI services table

    @retval VOID
**/

VOID DetectSataPortInfo (
    IN EFI_PEI_SERVICES     **PeiServices)
{
    EFI_STATUS          Status;
    UINT16              HobSize = sizeof(SATA_PRESENT_HOB);
    EFI_GUID            SataPresentHobGuid = AMI_SATA_PRESENT_HOB_GUID;
    SATA_PRESENT_HOB    *SataPresentHob;
    UINT16              SataClassCode;
    UINT16              SataPortStatus;
    UINT16              SataPortEnable = 0;
    SB_SETUP_DATA       SbSetupData;
    UINT8               i;

    Status = (*PeiServices)->CreateHob (PeiServices,
                                        EFI_HOB_TYPE_GUID_EXTENSION,
                                        HobSize,
                                        &SataPresentHob);
    if(EFI_ERROR(Status)) return;

    SataPresentHob->EfiHobGuidType.Name = SataPresentHobGuid;

    for (i = 0; i < 4; i++) {
      SataPresentHob->SataInfo[i].ClassCode = 0;
      SataPresentHob->SataInfo[i].PresentPortBitMap = 0;
    }

    // The SATA Mode Select should be configured in PchInitPeim.
    SataClassCode = READ_PCI16_SATA(PCI_SCC);
    SataPresentHob->SataInfo[0].ClassCode = SataClassCode;
    
    if ((SataClassCode & 0xFF) == 0x01) // V_PCH_SATA_SUB_CLASS_CODE_IDE
        return;
    
    GetSbSetupData (PeiServices, &SbSetupData, TRUE);
    SataPortStatus = READ_PCI32_SATA(R_PCH_H_SATA_PCS) >> 16;
    for (i = 0; i < GetPchMaxSataPortNum(); i++) {
      // SataPort controll is done in DXE, so check Setup value here.
      SataPortEnable |= (SbSetupData.SataPort[i] << i);
    }
    SataPortStatus &= SataPortEnable;
    SataPresentHob->SataInfo[0].PresentPortBitMap = (SATA1_BDF << 16) | (SataPortStatus & 0xFF); // Port 0~7
    SataPresentHob->ControllerCount = 1;
}
#endif

/**
    This function converts data from DEC to BCD format

    @param Dec value to be converted

    @retval UINT8 result of conversion

**/
UINT8 DecToBCD(
    IN UINT8 Dec
)
{
    UINT8 FirstDigit = Dec % 10;
    UINT8 SecondDigit = Dec / 10;

    return (SecondDigit << 4) + FirstDigit;
}

/**
    This function converts data from BCD to DEC format

    @param IN UINT8 BCD - value to be converted

    @retval UINT8 result of conversion

**/
UINT8 BCDToDec(IN UINT8 BCD)
{
    UINT8 FirstDigit = BCD & 0xf;
    UINT8 SecondDigit = BCD >> 4;;

    return SecondDigit * 10  + FirstDigit;
}

VOID CheckTcoNewCenturyBit (BOOLEAN RtcPwrValid)
{
    UINT16  TcoSts1;
    UINT8   Century;
    UINT8   Value;
    UINT16  TcoSts2;
    
    TcoSts1 = IoRead16(TCO_BASE_ADDRESS + TCO_IOREG_STS1);
    
    if (TcoSts1 & 0x80) {
        TcoSts2 = IoRead16(TCO_BASE_ADDRESS + TCO_IOREG_STS2);
        
        // B_PCH_TCO1_STS_NEWCENTURY is set
        // If RTC Power failure, skip increase the century
        if (RtcPwrValid) {
			IoWrite8(CMOS_ADDR_PORT, ACPI_CENTURY_CMOS);
			Century = IoRead8(CMOS_DATA_PORT); // Read register.
						
			Value = BCDToDec(Century);
			Value++;
			Century = DecToBCD(Value);
						
			IoWrite8(CMOS_ADDR_PORT, ACPI_CENTURY_CMOS);
			IoWrite8(CMOS_DATA_PORT, Century);
        }
        
        // clear B_PCH_TCO1_STS_NEWCENTURY bit
        IoWrite8(TCO_BASE_ADDRESS + TCO_IOREG_STS1, 0x80);               
        if ((TcoSts1 == 0x80) && (TcoSts2 == 0)) {
            IoWrite32(ACPI_BASE_ADDRESS + R_PCH_SMI_STS, B_PCH_SMI_STS_TCO);
        }
    }    
}

/**
    This function is the entry point for this PEI. This function
    initializes the chipset SB

    @param FfsHeader Pointer to the FFS file header
    @param PeiServices Pointer to the PEI services table

    @retval Return Status based on errors that occurred while waiting for
        time to expire.

    @note  This function should initialize South Bridge for memory detection.
    Install AMI_PEI_SBINIT_POLICY_PPI to indicate that SB Init PEIM
    is installed
    Following things can be done at this point:
     - Enabling top of 4GB decode for full flash ROM
     - Programming South Bridge ports to enable access to South
      Bridge and other I/O bridge access

**/

EFI_STATUS
EFIAPI
SbPeiInit(
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
)
{
    EFI_STATUS                  Status;
    IN EFI_PEI_PCI_CFG2_PPI     *PciCfg;
    EFI_PEI_CPU_IO_PPI          *CpuIo;
    SB_SETUP_DATA               SbSetupData;
    BOOLEAN						RtcPwrValid;
#if defined CREATE_CMOS_BAD_HOB && CREATE_CMOS_BAD_HOB != 0
    UINT16                      HobSize = sizeof(CMOS_BAD_HOB);
    CMOS_BAD_HOB                *CmosBadHob;
#endif    
    
    DEBUG((DEBUG_INFO, "SbPeiInit() Start.\n"));
    
    // Get pointer to the PCI config PPI
    PciCfg = (*PeiServices)->PciCfg;
    CpuIo = (*PeiServices)->CpuIo;

    PEI_PROGRESS_CODE ( PeiServices, PEI_CAR_SB_INIT );
    
    SbCheckPowerLoss();
    
    SbWakeupTypeLib();
    
    WRITE_IO8(PORTB_IO_CNTL, 0x0c); // Disable IOCHK NMI #, PCI SERR#. (0x61)
////////////////
    // Disable Auto-Reset Function
//    SET_IO16_TCO(R_PCH_TCO1_CNT, B_PCH_TCO_CNT_TMR_HLT);
    // Clear status excpet Intrusion.
    WRITE_IO16_TCO(R_PCH_TCO2_STS, READ_IO16_TCO(R_PCH_TCO2_STS) & ~(B_PCH_TCO2_STS_INTRD_DET));
////////////////

    GetSbSetupData( PeiServices, &SbSetupData, TRUE );

#if WdtPei_SUPPORT
{
    WDT_PPI                     *WdtPpi;

    // Locate WDT PPI for access to Wdt->Disable()
    //
    Status = (*PeiServices)->LocatePpi (
                      (CONST EFI_PEI_SERVICES**)PeiServices,
                      &gWdtPpiGuid,
                      0,
                      NULL,
                      (VOID **)&WdtPpi
                      );
    if (!EFI_ERROR (Status)) {
        WdtPpi->Disable();
    }
}
#endif
  // Program Pch devices bar base
  ProgramPchDeviceBase( (EFI_PEI_SERVICES **)PeiServices, PciCfg );
  
#if SB_STALL_PPI_SUPPORT
    // Install the SB Stall PPI
    Status = (*PeiServices)->InstallPpi( PeiServices, \
                                         &mBeforeBootModePpiList[0] );
    ASSERT_EFI_ERROR (Status);
#endif

    // Program Pch RCBA base
    ProgramRCRBMmio( (EFI_PEI_SERVICES **)PeiServices, CpuIo );
    
#if defined DISABLE_DAYLIGHT_SAVINGS && DISABLE_DAYLIGHT_SAVINGS != 0
    MmioOr8(SB_PCR_ADDRESS(PID_RTC, R_PCH_PCR_RTC_BUC), BIT4);
#endif    
    
    RtcPwrValid = SbIsRtcPwrValid();
    
#if defined CREATE_CMOS_BAD_HOB && CREATE_CMOS_BAD_HOB != 0
    Status = (*PeiServices)->CreateHob( (CONST EFI_PEI_SERVICES**) PeiServices,
                                        EFI_HOB_TYPE_GUID_EXTENSION,
                                        HobSize,
                                        (VOID **)  &CmosBadHob);
    if(!EFI_ERROR(Status)) {
      CmosBadHob->Header.Name = gAmiCmosBadHobGuid;
      CmosBadHob->Health = RtcPwrValid; 
    }   
#endif            
    
    //DeCode LPC IO
    ProgramSBIoDecodeRegs( (EFI_PEI_SERVICES **)PeiServices, PciCfg );
    
    //Program PM Regs.
    InitPMRegs( (EFI_PEI_SERVICES **)PeiServices, CpuIo, &SbSetupData );

    // General power failure handling
    GeneralPowerFailureHandler( (EFI_PEI_SERVICES **)PeiServices, CpuIo, PciCfg );

    PeiInitRtc( (EFI_PEI_SERVICES **)PeiServices, CpuIo );
  
    // Set what state (S0/S5) to go to when power is re-applied after a power failure (G3 state)
//    SetTheStateToGoAfterG3(PeiServices, CpuIo, PciCfg, &SbSetupData);
 
    InitPCIe( (EFI_PEI_SERVICES **)PeiServices, CpuIo, PciCfg );

    InitSMBus( (EFI_PEI_SERVICES **)PeiServices, CpuIo, PciCfg );

    // Check TCO1_STS NewCentury bit 
    CheckTcoNewCenturyBit(RtcPwrValid);    
    
    // Install the SB Init Policy PPI
    Status = (*PeiServices)->InstallPpi( PeiServices, &mPpiList[0] );
    ASSERT_EFI_ERROR (Status);

    // Setup a SBPEI entry after PEI permantent memory be installed
    Status = (*PeiServices)->NotifyPpi( PeiServices, BeforeMrcNotifyList );
    
    Status = (*PeiServices)->NotifyPpi( PeiServices, AfterMrcNotifyList );

    Status = (*PeiServices)->NotifyPpi ( PeiServices, mMemoryReadyNotify );
    ASSERT_EFI_ERROR (Status);

    Status = AmiAdjustOemGpioConfig((EFI_PEI_SERVICES **)PeiServices);
    ASSERT_EFI_ERROR (Status);

    // Update LPC Generic I/O Range
    if ((SB_LPC_IO_DECODE_BASEADDR != 0) && (SB_LPC_IO_DECODE_SIZE != 0)) {
        Status = SbLibSetLpcGenericDecoding(NULL, SB_LPC_IO_DECODE_BASEADDR, SB_LPC_IO_DECODE_SIZE, TRUE);
        ASSERT_EFI_ERROR (Status);
    }
    
#if defined TRACE_HUB_STATUS_CODE_SUPPORT && TRACE_HUB_STATUS_CODE_SUPPORT == 1  
    WriteCmos(AMI_TRACE_HUB_ENABLE_REG, 1);
#endif    
    
    DEBUG((DEBUG_INFO, "SbPeiInit() End.\n"));
    
    return EFI_SUCCESS;
}

/**
    This function initializes SB PCI Express controller(s)

    @param PeiServices - Pointer to the PEI services table
    @param CpuIo       - Pointer to the CPU I/O PPI
    @param PciCfg      - Pointer to the PCI Configuration PPI

    @retval VOID
**/

VOID InitPCIe (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN EFI_PEI_CPU_IO_PPI   *CpuIo,
    IN EFI_PEI_PCI_CFG2_PPI     *PciCfg
)
{
    // TODO
}

/**
    This function initializes SB SMBUS controller(s)

    @param PeiServices - Pointer to the PEI services table
    @param CpuIo       - Pointer to the CPU I/O PPI
    @param PciCfg      - Pointer to the PCI Configuration PPI

    @retval VOID
**/

VOID InitSMBus (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN EFI_PEI_CPU_IO_PPI   *CpuIo,
    IN EFI_PEI_PCI_CFG2_PPI     *PciCfg
)
{
    // TODO
}

/**
    Miscellaneous USB initialization.

    @param PeiServices - Pointer to the PEI services table
    @param CpuIo       - Pointer to the CPU I/O PPI
    @param PciCfg      - Pointer to the PCI Configuration PPI
    @param BootMode    - Boot Mode

    @retval VOID
**/

VOID InitUsbMisc (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN EFI_PEI_CPU_IO_PPI   *CpuIo,
    IN EFI_PEI_PCI_CFG2_PPI *PciCfg,
    IN EFI_BOOT_MODE        BootMode
)
{
/*  
  UINT32      XhciMmioBase = PEI_XHCI_MMIOBASE;
  UINT8       XhciCapLength;
  UINT8       XhciMaxPorts;
  UINT32      XhciPort;
  EFI_PEI_STALL_PPI   *StallPpi;
  StallPpi = &mStallPpi;
 
if (BootMode == BOOT_IN_RECOVERY_MODE) {       
    WRITE_PCI32( DEFAULT_PCI_BUS_NUMBER_PCH, \
                 PCI_DEVICE_NUMBER_PCH_XHCI, \
                 PCI_FUNCTION_NUMBER_PCH_XHCI, \
                 0x10, \
                 XhciMmioBase);
    WRITE_PCI8( DEFAULT_PCI_BUS_NUMBER_PCH, \
                 PCI_DEVICE_NUMBER_PCH_XHCI, \
                 PCI_FUNCTION_NUMBER_PCH_XHCI, \
                 0x04, \
                 0x06);    
    WRITE_PCI8( DEFAULT_PCI_BUS_NUMBER_PCH, \
                 PCI_DEVICE_NUMBER_PCH_XHCI, \
                 PCI_FUNCTION_NUMBER_PCH_XHCI, \
                 R_PCH_XHCI_USB3PR, \
                 0x3F);
    
    //Add some delay to wait that device links are stable
    StallPpi->Stall(PeiServices, StallPpi, 500 * 1000); 
    
    XhciCapLength = READ_MEM8(XhciMmioBase);
    if (PchSeries == PchH) {
        switch ((READ_MEM8(XhciMmioBase + R_PCH_XHCI_FUS)) & B_PCH_XHCI_FUS_SSPRTCNT) {
            case V_PCH_XHCI_FUS_SSPRTCNT_11B:
                XhciMaxPorts = 0x0F;
                break;

            case V_PCH_XHCI_FUS_SSPRTCNT_10B:
                XhciMaxPorts = 0x11;
                break;

            case V_PCH_XHCI_FUS_SSPRTCNT_01B:
                XhciMaxPorts = 0x13;
                break;

            case V_PCH_XHCI_FUS_SSPRTCNT_00B:
            default:
                XhciMaxPorts = 0x15;
                break;
        }
    } else {
        XhciMaxPorts = READ_MEM8(XhciMmioBase + 0x7);
    }
    //Clear each xHCI port power
    for (XhciPort = 0; XhciPort < XhciMaxPorts; XhciPort++) {
      RESET_MEM32(XhciMmioBase + XhciCapLength + 0x400 + (0x10 * XhciPort), BIT9);
    }
    // Set xHCI D0h & D8h as power-on default value.
    WRITE_PCI16( DEFAULT_PCI_BUS_NUMBER_PCH, \
                 PCI_DEVICE_NUMBER_PCH_XHCI, \
                 PCI_FUNCTION_NUMBER_PCH_XHCI, \
                 R_PCH_XHCI_USB2PR, \
                 0 );
    WRITE_PCI8( DEFAULT_PCI_BUS_NUMBER_PCH, \
                 PCI_DEVICE_NUMBER_PCH_XHCI, \
                 PCI_FUNCTION_NUMBER_PCH_XHCI, \
                 R_PCH_XHCI_USB3PR, \
                 0 );
    WRITE_PCI8( DEFAULT_PCI_BUS_NUMBER_PCH, \
                 PCI_DEVICE_NUMBER_PCH_XHCI, \
                 PCI_FUNCTION_NUMBER_PCH_XHCI, \
                 0x04, \
                 0);
    WRITE_PCI32( DEFAULT_PCI_BUS_NUMBER_PCH, \
                 PCI_DEVICE_NUMBER_PCH_XHCI, \
                 PCI_FUNCTION_NUMBER_PCH_XHCI, \
                 0x10, \
                 0);
  }
*/

                                        // [EIP107424] [EIP123117]>
/*
  if (BootMode != BOOT_ON_S3_RESUME) { 
    //
    // Clear unexpected USB EHCI Legacy Support Extended status.
    // Set B0:D1A/1D:F0 Reg#6Ch[31:29] = '111b'.
    //
    WRITE_PCI32_EHCI  ( R_PCH_EHCI_LEGEXT_CS, \
                      B_PCH_EHCI_LEGEXT_CS_SMIBAR | \
                      B_PCH_EHCI_LEGEXT_CS_SMIPCI | \
                      B_PCH_EHCI_LEGEXT_CS_SMIOS);
    if (PchSeries == PchH) {
      WRITE_PCI32_EHCI ( R_PCH_EHCI_LEGEXT_CS, \
                        B_PCH_EHCI_LEGEXT_CS_SMIBAR | \
                        B_PCH_EHCI_LEGEXT_CS_SMIPCI | \
                        B_PCH_EHCI_LEGEXT_CS_SMIOS);
    }                                    
  }
*/
                                        // <[EIP107424] [EIP123117]    
										//[EIP115528]<<
}

/**
    This function can be used to program any SB regisater after
    PEI permantent memory be installed.

    @param FfsHeader        - Pointer to the desired FFS header.
    @param PeiServices      - Pointer to the PEI services table.

    @retval EFI_STATUS
**/

EFI_STATUS 
EFIAPI
ProgramSBRegAfterMemInstalled (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
    IN VOID                     *InvokePpi )
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    EFI_PEI_CPU_IO_PPI          *CpuIo;
    IN EFI_PEI_PCI_CFG2_PPI     *PciCfg;
    EFI_BOOT_MODE               BootMode;

    CpuIo = (*PeiServices)->CpuIo;
    PciCfg = (*PeiServices)->PciCfg;

    PEI_PROGRESS_CODE ( (CONST EFI_PEI_SERVICES **)PeiServices, PEI_MEM_SB_INIT );

    Status = (*PeiServices)->GetBootMode ( (CONST EFI_PEI_SERVICES **)PeiServices, &BootMode );

    InitUsbMisc( PeiServices, CpuIo, PciCfg, BootMode );

    // Clear Global Reset Bit
    RESET_PCI32(SB_PMC_BUS, SB_PMC_DEV, SB_PMC_FUN, SB_REG_LPC_PMIR, B_ICH_LPC_PMIR_CF9GR);

    // Set up necessary PPI notifications after PEI permantent memory
    // be installed
    Status = (*PeiServices)->NotifyPpi( (CONST EFI_PEI_SERVICES **)PeiServices, &mNotifyList[0] );
    if (EFI_ERROR(Status))
    {
        ASSERT_EFI_ERROR (Status);
    }

    return  EFI_SUCCESS;
}

/**
    This function can be used to program any SB regisater before
    end of PEI phase.

    @param PeiServices      - Pointer to the PEI services table
    @param NotifyDescriptor - Pointer to the descriptor for the
        notification event.
    @param InvokePpi        - Pointer to the PPI that was installed

    @retval EFI_STATUS
**/

EFI_STATUS 
EFIAPI
ProgramSBRegBeforeEndofPei (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi )
{
    EFI_STATUS                  Status;
    EFI_BOOT_MODE               BootMode;
#if defined AMI_CSP_IO_PORT_CHECK && AMI_CSP_IO_PORT_CHECK    
    UINT8                       Index;
    UINT8                       IoPort[] = {0x26, 0x27, 0x2A, 0x2B};
#endif
    EFI_PEI_READ_ONLY_VARIABLE2_PPI *ReadOnlyVariable;
    UINTN                       VarSize;
    UINT32                      CspGlobalNvsAreaPtr;
    AMI_CSP_NVS_AREA            *Area;

    Status = (*PeiServices)->GetBootMode ((CONST EFI_PEI_SERVICES **)PeiServices, &BootMode);
    if (EFI_ERROR(Status))
    {
        ASSERT_EFI_ERROR (Status);
    }

    if (BootMode == BOOT_ON_S3_RESUME) {
#if defined AMI_CSP_IO_PORT_CHECK && AMI_CSP_IO_PORT_CHECK
        for(Index = 0; Index < sizeof(IoPort)/sizeof(UINT8); Index++) {
            if (IoRead8 (IoPort[Index]) != 0xFF) {
                // Hang the system while the read value from IO Port 0x26,0x27,0x2A,0x2B is not 0xFF.
                // IO Port 0x26,0x27,0x2A,0x2B should not be occupied by any PCI controller.
                // It is for debug purpose and the system should not come to here.
                DEBUG ((DEBUG_ERROR, "Error!! IO Port 0x26,0x27,0x2A,0x2B is occupied by other PCI controller\n"));                
                IoWrite8 (0x80, 0xEE);
                while (1);
            }
        }
#endif        
                                        // [EIP87695]>
#if SYSTEM_REBOOT_NORMALLY_IF_S3_IS_FAILED
        WRITE_IO16_PM(ACPI_IOREG_PM1_CNTL, READ_IO16_PM(ACPI_IOREG_PM1_CNTL) & 0xe3ff | (S3_SLP_TYP << 10)); // Clear S3 for avoiding S3 resume twice
#endif
                                        // <[EIP87695]
#if defined TRACE_HUB_STATUS_CODE_SUPPORT && TRACE_HUB_STATUS_CODE_SUPPORT == 1  
        WriteCmos(AMI_TRACE_HUB_ENABLE_REG, 0);
#endif       
    
#if defined(LSIPCIEDEVICE_SUPPORT) && LSIPCIEDEVICE_SUPPORT == 1
        Status = PeiServicesNotifyPpi (mLSIDevNotifyList);
#endif

        Status = (*PeiServices)->LocatePpi(
                                    (CONST EFI_PEI_SERVICES**)PeiServices,
                                    &gEfiPeiReadOnlyVariable2PpiGuid,
                                    0,
                                    NULL,
                                    (VOID **) &ReadOnlyVariable
                                    );
        if (!EFI_ERROR(Status)) {
            Status = ReadOnlyVariable->GetVariable(
                                        ReadOnlyVariable,
                                        AMI_CSP_GLOBALNVS_PTR_VARIABLE,
                                        &gAmiCspGlobalNvsPtrGuid,
                                        NULL,
                                        &VarSize,
                                        &CspGlobalNvsAreaPtr
                                        );
            if (!EFI_ERROR(Status)) {
                Area = (AMI_CSP_NVS_AREA *) CspGlobalNvsAreaPtr;
                Area->NotifyPowerBtn = PcdGet8(AmiPcdCspNvsNtpb);
            }
        }
        // Porting if needed.
    } else {
        // Porting if needed.
    }

    // Setting 8254
    // program timer 1 as refresh timer
    IoWrite8(LEGACY_TIMER_CTRL,0x54);
    IoWrite8(LEGACY_TIMER_1_COUNT,0x12);

#if defined(POST_TIME_TUNING_SUPPORT) && POST_TIME_TUNING_SUPPORT && (PTT_VER > 3) && SUPPORT_RAID_DRIVER
    DetectSataPortInfo(PeiServices);
#endif
     
#ifdef FSP_WRAPPER_FLAG    
    //
    // Clear SMM_EISS (SPI PCI offset DCh[5])
    // Clear LPC/eSPI EISS (LPC/eSPI PCI offset DCh[5])
    // Since the HW default is 1, need to clear it when disabled in policy
    //
    MmioAnd8 (SB_SPI_MMIO_REG(R_PCH_SPI_BC), (UINT8) ~B_PCH_SPI_BC_EISS);
    MmioAnd8 (SB_LPC_MMIO_REG(R_PCH_LPC_BC), (UINT8) ~B_PCH_LPC_BC_EISS);
#endif    
    
    return  EFI_SUCCESS;
}


/**
    Check RTC Time Update In Progress.

    @param PeiServices - Pointer to the PEI services table

    @retval Boolean
**/
BOOLEAN IsRtcUipAlwaysSet(
  IN EFI_PEI_SERVICES       **PeiServices
  )
{

  EFI_PEI_STALL_PPI   *StallPpi;
  UINTN               Count;

  StallPpi = &mStallPpi;

  for (Count = 0; Count < 500; Count++) {   // Maximum waiting approximates to 1.5 seconds (= 3 msec * 500) 
    if ((READ_IO8_RTC(RTC_NMI_MASK | RTC_REG_A_INDEX) & BIT07) == 0) {
      return FALSE;
    }
    StallPpi->Stall ( (CONST EFI_PEI_SERVICES**)PeiServices, StallPpi, 3000); 
  }

  return TRUE;  
}

/**
    This function initializes SB RTC related registers

    @param PeiServices - Pointer to the PEI services table
    @param CpuIo       - Pointer to the CPU I/O PPI

    @retval VOID
**/
VOID PeiInitRtc(
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_CPU_IO_PPI           *CpuIo
)
{
#if !EXTERNAL_RTC_SUPPORT    
    UINT8        Buffer8;
    UINT16       Buffer16;
    BOOLEAN      RtcUipIsAlwaysSet;
    RTC_REG_A    RegA;
    RTC_REG_B    RegB;


    RegA.REG_A = ReadCmos(RTC_REG_A_INDEX);
    RegB.REG_B = ReadCmos(RTC_REG_B_INDEX);
	

    //
    // PCH BIOS Specification 0.6.0 - Section 19.2, "Power Failure Consideration"
    // 
    // When the RTC_PWR_STS bit is set, it indicates that the RTCRST# signal went low.
    // Software should clear this bit. For example, changing the RTC battery sets this bit.
    // System BIOS should reset CMOS to default values if the RTC_PWR_STS is set.
    //
    // The System BIOS should execute the sequence below if the RTC_PWR_STS bit is set before memory initialization.
    // This will ensure that the RTC state machine has been initialized.
    //  1.    If the RTC_PWR_STS bit is set, which indicates a new coin-cell batttery
    //        insertion or a battery failure, steps 2 through 5 should be executed.
    //  2.    Set RTC Register 0Ah[6:4] to '110b' or '111b'.
    //  3.    Set RTC Register 0Bh[7].
    //  4.    Set RTC Register 0Ah[6:4] to '010b'.
    //  5.    Clear RTC Register 0Bh[7].
    Buffer16 = READ_PCI16(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, R_PCH_PMC_GEN_PMCON_B); // 0xA4 

    RtcUipIsAlwaysSet = IsRtcUipAlwaysSet(PeiServices);
    if ((Buffer16 & B_PCH_PMC_GEN_PMCON_B_RTC_PWR_STS) || RtcUipIsAlwaysSet || (RegA.Divisor != 2) || (RegB.Set == 1)) { 

        //
        // Step 1.
        // BIOS clears this bit by writing a '0' to it.
        //
        if (Buffer16 & B_PCH_PMC_GEN_PMCON_B_RTC_PWR_STS) {
          //
          // The global reset is issued in PchEarlyInit() if RTC missing. If clear RTC_PWR_STS bit here, 
          // it will cause SbIsDefaultConfigMode fail. The RTC_PWR_STS bit will be cleared in PchConfigureRtc().
          //         
          WRITE_IO8_RTC((RTC_NMI_MASK | RTC_DAY_OF_MONTH_REG), 0xFF);
          WRITE_IO8_RTC((RTC_NMI_MASK | RTC_HOURS_REG), 0xFF);
        }

        //      
        // Step 2.
        // Set RTC Register 0Ah[6:4] to '110' or '111'.
        //
        WRITE_IO8_RTC((RTC_NMI_MASK | RTC_REG_A_INDEX), 0x66);

        //      
        // Step 3.
        // Set RTC Register 0Bh[7].
        //
        Buffer8 = (READ_IO8_RTC(RTC_NMI_MASK | RTC_REG_B_INDEX) | 0x80);
        WRITE_IO8_RTC((RTC_NMI_MASK | RTC_REG_B_INDEX), Buffer8);
  
        // Clear Flag
        READ_IO8_RTC(RTC_NMI_MASK | RTC_REG_C_INDEX);
        
        //      
        // Step 4.
        // Set RTC Register 0Ah[6:4] to '010'.
        //
        WRITE_IO8_RTC((RTC_NMI_MASK | RTC_REG_A_INDEX), 0x26);

        //      
        // Step 5.
        // Clear RTC Register 0Bh SET[7] & DSE[0].
        //
        Buffer8 = (READ_IO8_RTC(RTC_NMI_MASK | RTC_REG_B_INDEX) & ~0x81);
        WRITE_IO8_RTC((RTC_NMI_MASK | RTC_REG_B_INDEX), Buffer8);
    }
    
    //                                         
    // Clear Date Alarm bits in Register 0Dh[5:0]. 
    // 
    Buffer8 = (READ_IO8_RTC(RTC_NMI_MASK | RTC_REG_D_INDEX) & 0x40); 
    WRITE_IO8_RTC((RTC_NMI_MASK | RTC_REG_D_INDEX), Buffer8); 
#endif
}

/**
    This function initializes SB Power Management registers.

    @param PeiServices - Pointer to the PEI services table
    @param CpuIo       - Pointer to the CPU I/O PPI

    @retval VOID
**/

VOID InitPMRegs (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN EFI_PEI_CPU_IO_PPI   *CpuIo,
    IN SB_SETUP_DATA        *SbSetupData)
{
  EFI_BOOT_MODE  BootMode = 0;
  PCH_SERIES     PchSeries = GetPchSeries();

  
  (*PeiServices)->GetBootMode ((CONST EFI_PEI_SERVICES **)PeiServices, &BootMode);
//  if (BootMode != BOOT_ON_S3_RESUME) {
    WRITE_IO16_PM(R_PCH_ACPI_PM1_EN, 0);
//  }
  if (PchSeries == PchLp) {
    WRITE_IO32_PM(R_PCH_ACPI_GPE0_EN_127_96, 0);
  } else {

  }

}

/**
    This function initializes SB Root Complex registers

    @param PeiServices - Pointer to the PEI services table
    @param CpuIo       - Pointer to the CPU I/O PPI

    @retval VOID
**/

VOID ProgramRCRBMmio (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN EFI_PEI_CPU_IO_PPI   *CpuIo )
{

}

/**
    This function initializes SB IO Decide Registers.

    @param PeiServices - Pointer to the PEI services table
    @param PciCfg      - Pointer to the PCI Configuration PPI

    @retval EFI_STATUS
**/
EFI_STATUS
ProgramSBIoDecodeRegs (
  IN EFI_PEI_SERVICES       **PeiServices,
  IN EFI_PEI_PCI_CFG2_PPI   *PciCfg
)
{

#if defined EMUL6064_SUPPORT && EMUL6064_SUPPORT == 1
    // Force KBC_LPC_EN bit (B0:D31:F0 Reg 82h[10]) = 1 if EMUL6064_SUPPORT = 1.
    SbLibSetLpcDeviceDecoding(NULL, 0x60, 0, dsPS2K);
#endif
   return EFI_SUCCESS;
}

/**
    This function initializes SB Devices Base

    @param PeiServices - Pointer to the PEI services table
    @param PciCfg      - Pointer to the PCI Configuration PPI

    @retval EFI_STATUS
**/
EFI_STATUS
ProgramPchDeviceBase (
  IN EFI_PEI_SERVICES       **PeiServices,
  IN EFI_PEI_PCI_CFG2_PPI   *PciCfg
)
{

  // Write Heci Base Address and enable Heci device
//  WRITE_PCI32_HECI(ME_REG_HECI_MBAR, HECI_BASE_ADDRESS);
//  WRITE_PCI16_HECI(ME_REG_PCICMD, 0x06);

  // Write Heci Base Address and enable Heci device
//  WRITE_PCI32_HECI2(ME_REG_HECI_MBAR, HECI2_BASE_ADDRESS);
//  WRITE_PCI16_HECI2(ME_REG_PCICMD, 0x06);


  
  return EFI_SUCCESS;
}

/**
    When the PWR_FLR bit is set, it indicates the trickle power
    from the main battery or tricle supply failed while in suspend
    or since last boot. This bit us ub the RTC well and is cleared
    only by RTCRST#. Software writes a "1" to clear this bit.
    System BIUOS should follow cold boot path if PWR_FLR, GEN_RST_STS
    or PWRBTNOR_STS is set to 1 regardless of the value in the SLP_TYP
    field.

    @param PeiServices - Pointer to the PEI services table
    @param CpuIo       - Pointer to the CPU I/O PPI
    @param PciCfg      - Pointer to the PCI Configuration PPI

    @retval EFI_STATUS
**/
EFI_STATUS
GeneralPowerFailureHandler (
  IN EFI_PEI_SERVICES       **PeiServices,
  IN EFI_PEI_CPU_IO_PPI     *CpuIo,
  IN EFI_PEI_PCI_CFG2_PPI   *PciCfg
)
{

  UINT16       DataUint16;  
  UINT32        DataUint32;

  //
  // PCH BIOS Specification 0.6.0 - Section 19.2, "Power Failure Considerations"
  // 
  // When the PWR_FLR bit is set, it indicates the trickle power from the main
  // battery or tricle supply failed while in suspend or since last boot.
  // System BIOS should follow cold boot path if PWR_FLR, GEN_RST_STS or
  // PWRBTNOR_STS is set to 1 regardless of the value in the SLP_TYP field.
  //
  DataUint16 = READ_PCI16(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, R_PCH_PMC_GEN_PMCON_B);
  if ((DataUint16 & (B_PCH_PMC_GEN_PMCON_B_RTC_PWR_STS | B_PCH_PMC_GEN_PMCON_B_PWR_FLR)) || \
      (READ_IO16_PM(R_PCH_ACPI_PM1_STS) & B_PCH_ACPI_PM1_STS_PRBTNOR)) {
    //
    // BIOS clears these bits by writing a '1' to them.
    //
    WRITE_PCI16(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, R_PCH_PMC_GEN_PMCON_B, DataUint16);
    WRITE_IO16_PM(R_PCH_ACPI_PM1_STS, B_PCH_ACPI_PM1_STS_PRBTNOR);

    //
    // Clear Wake Status (WAK_STS) and Sleep Type (SLP_TYP)
    //
    WRITE_IO16_PM(R_PCH_ACPI_PM1_STS, B_PCH_ACPI_PM1_STS_WAK);
    DataUint16 = (READ_IO16_PM(R_PCH_ACPI_PM1_CNT) & ~B_PCH_ACPI_PM1_CNT_SLP_TYP);
    WRITE_IO16_PM(R_PCH_ACPI_PM1_CNT, DataUint16);
  }

  // 
  // When the CPUPWR_FLR bit is set, it indicates VRMPWRGD signal from
  // the CPU VRM went low. This bit is now set if VRMPWRGD goes low
  // during Intel (R) SpeedStep Technology transition.
  // Software must clear this bit if set.
  //
  DataUint32 = READ_PCI32(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, R_PCH_PMC_GEN_PMCON_B);

  // Clear SUSPWR_FLR bit 
  if (DataUint32 & B_PCH_PMC_GEN_PMCON_B_SUS_PWR_FLR) {
    //
    // BIOS clears this bit by writing a '1' to it.
    // Set D31:F0:A4h[1] = 0b,D31:F0:A4h[9] = 0b to avoid clearing write 1 clear bits.
    //  
    DataUint32 &= (UINT32) ~B_PCH_PMC_GEN_PMCON_B_PWR_FLR;
    DataUint32 &= (UINT32) ~B_PCH_PMC_GEN_PMCON_B_HOST_RST_STS;  
    DataUint32 |= (UINT32) B_PCH_PMC_GEN_PMCON_B_SUS_PWR_FLR;   	  
	  	  
    WRITE_PCI32(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, R_PCH_PMC_GEN_PMCON_B, DataUint32);
  }


  return EFI_SUCCESS;
}

/**
    Set what state (S0/S5) to go to when power is re-applied
    after a power failure (G3 state)

    @param PeiServices - Pointer to the PEI services table
    @param CpuIo       - Pointer to the CPU I/O PPI
    @param PciCfg      - Pointer to the PCI Configuration PPI
    @param SbSetupData - Pointer to the SbSetupData

    @retval EFI_STATUS
**/
EFI_STATUS
SetTheStateToGoAfterG3 (
  IN EFI_PEI_SERVICES       **PeiServices,
  IN EFI_PEI_CPU_IO_PPI     *CpuIo,
  IN EFI_PEI_PCI_CFG2_PPI   *PciCfg,
  IN SB_SETUP_DATA          *SbSetupData
)
{
  UINT8                     DataUint8;
  //
  // Make sure we have a setup data, if not, just return.
  //
  if (SbSetupData == NULL) {
    return EFI_UNSUPPORTED;
  }
 
  DataUint8 = READ_PCI8(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, R_PCH_PMC_GEN_PMCON_B);
  if(SbSetupData->StateAfterG3 == 1) {
    DataUint8 |= B_PCH_PMC_GEN_PMCON_B_AFTERG3_EN; // S5
  } else {
    DataUint8 &= ~B_PCH_PMC_GEN_PMCON_B_AFTERG3_EN; // S0
  }

// Done in PchMisc.c
//####  DataUint16 |= (SbSetupData->SlpS4AssW << 4);
  ///
  /// Set D31:F0:A4h[1] = 0b to avoid clearing write 1 clear bits.
  ///  
  DataUint8 &= ~B_PCH_PMC_GEN_PMCON_B_PWR_FLR;
 
  WRITE_PCI8(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, R_PCH_PMC_GEN_PMCON_B, DataUint8);

  return EFI_SUCCESS;
}

/**
    This function can be used to program any SB regisater before
    memory is detected.

    @param PeiServices      - Pointer to the PEI services table
    @param NotifyDescriptor - Pointer to the descriptor for the
        notification event.
    @param InvokePpi        - Pointer to the PPI that was installed

    @retval EFI_STATUS
**/

EFI_STATUS 
EFIAPI
ProgramSBRegBeforeMrc (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi )
{
    EFI_STATUS                      Status = EFI_SUCCESS;
#if Capsule_SUPPORT
    EFI_PEI_READ_ONLY_VARIABLE2_PPI *ReadOnlyVariable;
    EFI_GUID                        gEfiCapsuleVendorGuid = EFI_CAPSULE_VENDOR_GUID;
    UINTN                           VariableSize;
    EFI_PHYSICAL_ADDRESS            Data;
    SI_PREMEM_POLICY_PPI            *SiPreMemPolicyPpi;    
    MEMORY_CONFIG_NO_CRC            *MemConfigNoCrc;    
#endif
    
    DEBUG((DEBUG_INFO, "ProgramSBRegBeforeMrc(): Start.\n"));
    
    if ( AMI_OEM_SSID_SUPPORT ) {
        // Program SB Devices' Subsystem Vendor ID & Subsystem ID
        ProgramSbSubId( (EFI_PEI_SERVICES **)PeiServices, (*PeiServices)->PciCfg );
    }

#if Capsule_SUPPORT
    //
    // To prevent memory is cleaned when PTT is enabled and do Windows Firmware update with MEUD.
    //
    Status = (*PeiServices)->LocatePpi( (CONST EFI_PEI_SERVICES**)PeiServices, \
                                        &gEfiPeiReadOnlyVariable2PpiGuid, \
                                        0, \
                                        NULL, \
                                        (VOID **)&ReadOnlyVariable );

    DEBUG((DEBUG_INFO, "LocatePpi gEfiPeiReadOnlyVariable2PpiGuid Status :%x\n", Status ));
    if (!EFI_ERROR(Status)) {
      VariableSize = sizeof(EFI_PHYSICAL_ADDRESS);
      Status = ReadOnlyVariable->GetVariable( ReadOnlyVariable, \
                                              EFI_CAPSULE_VARIABLE_NAME, \
                                              &gEfiCapsuleVendorGuid, \
                                              NULL, \
                                              &VariableSize, \
                                              &Data );
      DEBUG((DEBUG_INFO, "Get Capsule Variable Status :%x\n", Status ));
      if (!EFI_ERROR(Status)) {
        Status = (*PeiServices)->LocatePpi( (CONST EFI_PEI_SERVICES**)PeiServices, \
                                        &gSiPreMemPolicyPpiGuid, \
                                        0, \
                                        NULL, \
                                        (VOID **)&SiPreMemPolicyPpi );
        DEBUG((DEBUG_INFO, "LocatePpi gSiPreMemPolicyPpiGuid Status :%x\n", Status ));
        Status = GetConfigBlock ((VOID *)SiPreMemPolicyPpi, &gMemoryConfigNoCrcGuid, (VOID *) &MemConfigNoCrc);
        DEBUG((DEBUG_INFO, "GetConfigBlock gMemoryConfigNoCrcGuid Status :%x\n", Status ));
        if (!EFI_ERROR(Status)) {
          DEBUG((DEBUG_INFO, "MemConfigNoCrc->CleanMemory :%x\n", MemConfigNoCrc->CleanMemory ));
          if (MemConfigNoCrc->CleanMemory != 0) {
            MemConfigNoCrc->CleanMemory = 0;
          }
        }
      }
    }
#endif
    
    DEBUG((DEBUG_INFO, "ProgramSBRegBeforeMrc(): End.\n"));

    return EFI_SUCCESS;
}

/**
    This function can be used to program any SB regisater after
    memory is detected.

    @param PeiServices      - Pointer to the PEI services table
    @param NotifyDescriptor - Pointer to the descriptor for the
        notification event.
    @param InvokePpi        - Pointer to the PPI that was installed

    @retval EFI_STATUS
**/

EFI_STATUS EFIAPI ProgramSBRegAfterMrc (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi )
{
    EFI_STATUS                      Status = EFI_SUCCESS;
    IN EFI_BOOT_MODE                BootMode;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI *ReadOnlyVariable;
    UINTN                           VariableSize;
    EFI_GUID                        WarmResetGuid = SB_WARM_RESET_GUID;
    CHAR16                          WarmResetVar[] = SB_WARM_RESET_VARIABLE;
    UINT32                          WarmResetFlag = 0;
#if Capsule_SUPPORT
    PEI_CAPSULE_PPI                 *Capsule;
#endif
    
    DEBUG((DEBUG_INFO, "ProgramSBRegAfterMrc(): Start.\n"));
    
    Status = (*PeiServices)->GetBootMode( (CONST EFI_PEI_SERVICES **)PeiServices, &BootMode );

    if (BootMode == BOOT_ON_S3_RESUME || SbIsWarmBoot()) {

        DEBUG((DEBUG_INFO, "ProgramSBRegAfterMrc(): S3 Resume.\n"));

        Status = (*PeiServices)->LocatePpi( (CONST EFI_PEI_SERVICES**)PeiServices, \
                                            &gEfiPeiReadOnlyVariable2PpiGuid, \
                                            0, \
                                            NULL, \
                                            (VOID **)&ReadOnlyVariable );
        if (!EFI_ERROR(Status)) {
            VariableSize = sizeof(WarmResetFlag);
            Status = ReadOnlyVariable->GetVariable( ReadOnlyVariable, \
                                                    WarmResetVar, \
                                                    &WarmResetGuid, \
                                                    NULL, \
                                                    &VariableSize, \
                                                    &WarmResetFlag );
            if (WarmResetFlag == SB_WARM_RESET_TAG) {
                DEBUG((DEBUG_INFO, "ProgramSBRegAfterMrc(): Update BootMode.\n"));
                BootMode = BOOT_WITH_FULL_CONFIGURATION;

#if Capsule_SUPPORT
                //
                // Update BootMode, if Capsule 2.0 PPI is available.
                //
                Status = (*PeiServices)->LocatePpi ( (CONST EFI_PEI_SERVICES**)PeiServices, \
                                                     &gPeiCapsulePpiGuid, \
                                                     0, \
                                                     NULL, \
                                                     (VOID **)&Capsule);

                if (!EFI_ERROR(Status)) {
                  BootMode = BOOT_ON_FLASH_UPDATE;

                } else {
                  // Clear ACPI Sleep Type 
                  RESET_IO32_PM(R_SB_ACPI_PM1_CNT, 0x1c00 ); // 0x04
                }

                (*PeiServices)->SetBootMode((CONST EFI_PEI_SERVICES **)PeiServices, BootMode);
#else
                (*PeiServices)->SetBootMode((CONST EFI_PEI_SERVICES **)PeiServices, BootMode);
                // Clear ACPI Sleep Type 
                RESET_IO32_PM(R_SB_ACPI_PM1_CNT, 0x1c00 ); // 0x04
#endif
            }
        }
    }

    DEBUG((DEBUG_INFO, "ProgramSBRegAfterMrc(): End.\n"));

    return EFI_SUCCESS;
}


/**
    This function can be used to program any SB regisater at
    end of MRC.

    @param PeiServices      - Pointer to the PEI services table
    @param NotifyDescriptor - Pointer to the descriptor for the
        notification event.
    @param InvokePpi        - Pointer to the PPI that was installed

    @retval EFI_STATUS
**/

EFI_STATUS EFIAPI ProgramSBRegEndOfMrc (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi )
{
    IN EFI_BOOT_MODE                BootMode;
    EFI_STATUS                      Status;
    VOID                            *RecoveryModePpi;

    DEBUG((DEBUG_INFO, "ProgramSBRegEndOfMrc(): Start.\n"));

    Status = (*PeiServices)->LocatePpi ( (CONST EFI_PEI_SERVICES**)PeiServices, \
                                         &gRecoveryBootModeGuid, \
                                         0, \
                                         NULL, \
                                         (VOID **)&RecoveryModePpi);

    if (EFI_ERROR(Status)) {
      // Update Bootmode
      (*PeiServices)->GetBootMode( (CONST EFI_PEI_SERVICES **)PeiServices, &BootMode );
      DEBUG((DEBUG_INFO, "Before change BootMode = %X\n", BootMode));
      BootMode = BOOT_WITH_FULL_CONFIGURATION;
      (*PeiServices)->SetBootMode((CONST EFI_PEI_SERVICES **)PeiServices, BootMode);
    }
    DEBUG((DEBUG_INFO, "ProgramSBRegEndOfMrc(): End.\n"));

    return EFI_SUCCESS;
}

#if SB_RESET_PPI_SUPPORT

/**
    This function is the reset call interface function published
    by the reset PPI

    @param PeiServices     Pointer to the PEI services table

    @retval SYSTEM RESET
**/

EFI_STATUS 
EFIAPI
SbPeiResetSystem (
    IN EFI_PEI_SERVICES         **PeiServices )
{
#if WdtPei_SUPPORT
    WDT_PPI      *Wdt;
    EFI_STATUS   Status;

    Status = (*PeiServices)->LocatePpi ( (CONST EFI_PEI_SERVICES**)PeiServices, \
                                         &gWdtPpiGuid, \
                                         0, \
                                         NULL, \
                                         (VOID **)&Wdt );
  
    ASSERT_EFI_ERROR (Status);
  
    Wdt->AllowKnownReset();
#endif

    SBLib_ResetSystem(EfiResetCold);

    // We should never get this far
    return EFI_SUCCESS;
}

#endif

#if SB_STALL_PPI_SUPPORT

/**
    This function provides a blocking stall at least number
    of microseconds by SB ACPI timer

    @param PeiServices  - Pointer to the PEI services table
    @param This         - Pointer to the Stall PPI
    @param MicroSeconds - Number of microseconds for which to stall

    @retval VOID
**/

EFI_STATUS
EFIAPI
SbPeiStall (
    IN EFI_PEI_SERVICES     **PeiServices, 
    IN EFI_PEI_STALL_PPI    *This, 
    IN UINTN                MicroSeconds )
{
    EFI_STATUS              Status;

    // Locate PciCfg PPI

    // At this time no manipulation needed.  The value passed in is in
    // MicroSeconds(us) and that is what the library function uses

    // Call Library function that is shared with Metronome
    // Architecture Protocol


    Status = CountTime(MicroSeconds, PM_BASE_ADDRESS);

    return Status;
}

#endif

#if ATAPI_RECOVERY_SUPPORT

EFI_GUID gIdeRecoveryNativeModePpiGuid = \
                                        PEI_IDE_RECOVERY_NATIVE_MODE_PPI_GUID;

PEI_IDE_RECOVERY_NATIVE_MODE_PPI IdeRecoveryNativeModePpi = {
                                                    SB_TEMP_IO_BASE+0x200,
                                                    SB_TEMP_IO_BASE+0x282,
                                                    SB_TEMP_IO_BASE+0x300,
                                                    SB_TEMP_IO_BASE+0x382
};

EFI_PEI_PPI_DESCRIPTOR IdeRecoveryNativeModePpiDescriptor = {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST), \
    &gIdeRecoveryNativeModePpiGuid, &IdeRecoveryNativeModePpi
};

/**
    This function initializes SATA controller registers
    for ATA/ATAPI recovery support.

    @param PeiServices - Pointer to the PEI services table
    @param PciCfg      - Pointer to the PCI Configuration PPI

    @retval VOID
**/

VOID InitSATARegs (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN EFI_PEI_PCI_CFG2_PPI     *PciCfg
)
{
    // TODO
}

/**
    This function enables the specific channel of ATA/SATA
    controller(s) depend on input ChannelMask

    @param PeiServices - Pointer to the PEI services table
    @param This        - Pointer to the ATA Controller PPI
    @param ChannelMask - Mask for the specific channel.

    @retval EFI_STATUS

    @note  Normally we have to enables all chennels on ATA/SATA
              controller(s) regardless of ChannelMask.
**/

EFI_STATUS EFIAPI EnableAtaChannel (
    IN EFI_PEI_SERVICES               **PeiServices,
    IN PEI_ATA_CONTROLLER_PPI         *This,
    IN UINT8                          ChannelMask )
{

  return EFI_UNSUPPORTED;
}
#endif // ATAPI_RECOVERY_SUPPORT

#if defined AhciRecovery_SUPPORT && AhciRecovery_SUPPORT
#if !defined(AHCIREC_DRIVER_VERSION) || (AHCIREC_DRIVER_VERSION < 4)
EFI_STATUS
RecInitSataController (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN PEI_AHCI_CONTROLLER_PPI        *This
)
{
    UINTN                        SataPciRegBase;
    
    DEBUG((DEBUG_INFO, "\nSbPei.c : RecInitSataController\n"));
    
    SataPciRegBase  = SB_PCIE_CFG_ADDRESS ( 0, PCI_DEVICE_NUMBER_PCH_SATA, PCI_FUNCTION_NUMBER_PCH_SATA, 0);    
    
    MmioWrite32(SataPciRegBase + R_PCH_SATA_AHCI_BAR, AHCI_BASE_ADDRESSES);
    ///
    /// Enable ABAR decode
    ///
    MmioOr16 ( SataPciRegBase + 0x04, BIT02 | BIT01);

    //MmioWrite32 (AHCI_BASE_ADDRESSES + R_PCH_SATA_AHCI_PI, (UINT32)(MmioRead8(SataPciRegBase + R_PCH_SATA_PCS + 1) & (0x03)));
    // Enable all sata ports
    MmioOr32 (AHCI_BASE_ADDRESSES + R_PCH_SATA_AHCI_PI, (UINT32) ~(0));

    //
    // Two reads required (C-spec ch. 25 - 1.9.14.1)
    //
    MmioRead32 (AHCI_BASE_ADDRESSES + R_PCH_SATA_AHCI_PI);
    MmioRead32 (AHCI_BASE_ADDRESSES + R_PCH_SATA_AHCI_PI);
        
    CountTime(4000, PM_BASE_ADDRESS);
    
    return EFI_SUCCESS;
}

#else
EFI_STATUS
EFIAPI
GetSbSataLocation (
  IN  EFI_PEI_SERVICES             **PeiServices,
  IN  PEI_AHCI_CONTROLLER_2_PPI    *This,
  IN OUT UINT8                     *Bus,
  IN OUT UINT8                     *Device,
  IN OUT UINT8                     *Function
)
{
    DEBUG((DEBUG_INFO, "\nSbPei.c : GetSbSataLocation\n"));
    *Bus = 0;
    *Device = PCI_DEVICE_NUMBER_PCH_SATA;
    *Function = PCI_FUNCTION_NUMBER_PCH_SATA;
    return EFI_SUCCESS; 
}

EFI_STATUS
EFIAPI
RecInitSbSataController (
  IN  EFI_PEI_SERVICES             **PeiServices,
  IN  PEI_AHCI_CONTROLLER_2_PPI    *This,
  IN  UINT32                       AhciMmioBaseAddress
)
{
    UINTN                        SataPciRegBase;
    
    DEBUG((DEBUG_INFO, "\nSbPei.c : RecInitSbSataController\n"));
    
    SataPciRegBase  = SB_PCIE_CFG_ADDRESS ( 0, PCI_DEVICE_NUMBER_PCH_SATA, PCI_FUNCTION_NUMBER_PCH_SATA, 0);    
    
    //MmioWrite32(SataPciRegBase + R_PCH_SATA_AHCI_BAR, AHCI_BASE_ADDRESSES);
    ///
    /// Enable ABAR decode
    ///
    MmioOr16 ( SataPciRegBase + 0x04, BIT02 | BIT01);

    //MmioWrite32 (AHCI_BASE_ADDRESSES + R_PCH_SATA_AHCI_PI, (UINT32)(MmioRead8(SataPciRegBase + R_PCH_SATA_PCS + 1) & (0x03)));
    // Enable all sata ports
    MmioOr32 (AhciMmioBaseAddress + R_PCH_SATA_AHCI_PI, (UINT32) ~(0));

    //
    // Two reads required (C-spec ch. 25 - 1.9.14.1)
    //
    MmioRead32 (AhciMmioBaseAddress + R_PCH_SATA_AHCI_PI);
    MmioRead32 (AhciMmioBaseAddress + R_PCH_SATA_AHCI_PI);
        
    CountTime(4000, PM_BASE_ADDRESS);
    
    return EFI_SUCCESS;
}
#endif
#endif //AhciRecovery_SUPPORT

#if SMM_SUPPORT
/**
    This function generates a software SMI by writing the provided
    byte value into the software SMI generation register

    @param PeiServices - Pointer to the PEI services table
    @param *This - Pointer to the SMM control PPI
    @param *ArgumentBuffer - Contains the value to be written to the
        S/W SMI port. Currently supports byte only
        and this is a optional pointer
    @param *ArgumentBufferSize - Optional. Valid value is 1
    @param Periodic - Boolean indicating the nature of generation
        TRUE - means periodic generation depending on
        timing value provided in the next variable
        CURRENTLY NOT SUPPORTED. EXPECTS FALSE
    @param ActivationInterval - Optional. NOT SUPPORTED

        
    @retval EFI_SUCCESS S/W SMI triggered successfully
    @retval EFI_INVALID_PARAMETER If Periodic is TRUE or when
        (ArgumentBuffer is not NULL and
        ArgumentBufferSize is not 1)

    @note  CHIPSET AND/OR BOARD PORTING NEEDED
              Here is the control flow of this function:
                1. If Periodic was TRUE, return EFI_INVALID_PARAMETER.
                2. If ArgumentBuffer == NULL, use 0xFF as data.
                3. If not NULL, if valid byte, use it as data. Otherwise 
                   return EFI_INVALID_PARAMETER.
                4. Enable SW SMI.
                5. Write the data to the SMI trigger port.
                6. Return EFI_SUCCESS.
**/

EFI_STATUS 
EFIAPI
PeiActivate (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN PEI_SMM_CONTROL_PPI  *This,
    IN OUT INT8             *ArgumentBuffer OPTIONAL,
    IN OUT UINTN            *ArgumentBufferSize OPTIONAL,
    IN BOOLEAN              Periodic OPTIONAL,
    IN UINTN                ActivationInterval OPTIONAL )
{
    UINT8   Data;

    if (Periodic) return EFI_INVALID_PARAMETER;

    if (ArgumentBuffer == NULL) {
        Data = 0xff;    // If no data given, use 0xff to trigger SMI.
    } else {
        if (ArgumentBufferSize == NULL || *ArgumentBufferSize != 1) 
            return EFI_INVALID_PARAMETER;   // Only able to send 1 byte.
        Data = *ArgumentBuffer;
    }

    // Porting Required.  Include code to generate S/W SMI
    // Clear the Power Button Override Status Bit
    WRITE_IO16_PM(R_PCH_ACPI_PM1_STS, BIT11); // 0x00
    // Clear the APM SMI Status Bit
    WRITE_IO32_PM(R_PCH_SMI_STS, BIT5); // 0x34
    // Set the EOS Bit
    RW_IO32_PM(R_PCH_SMI_EN, BIT1, 0); // 0x30
    // Enable Software SMIs
    SET_IO8_PM(R_PCH_SMI_EN, 0x21); // 0x30
    WRITE_IO8(SW_SMI_IO_ADDRESS, Data); // This triggers SMI
    READ_IO8(IO_DELAY_PORT);            // I/O Delay
    WRITE_IO8(IO_DELAY_PORT, Data);     // I/O Delay
    READ_IO8(IO_DELAY_PORT);            // I/O Delay
    // Porting End

    return EFI_SUCCESS;
}
  
/**
    This function clears software SMI status registers and
    generates End-of-SMI (EOS).

    @param PeiServices - Pointer to the PEI services table
    @param *This    - Pointer to the SMM control PPI
    @param Periodic - Boolean indicating the nature of clearing,
        TRUE means periodic SMI clearing.
        CURRENTLY NOT SUPPORTED. EXPECTS FALSE

        
    @retval EFI_SUCCESS SMI status successfully cleared
    @retval EFI_INVALID_PARAMETER If Periodic is TRUE

    @note  CHIPSET AND/OR BOARD PORTING NEEDED
              Here is the control flow of this function:
                1. If Periodic was TRUE, return EFI_INVALID_PARAMETER.
                2. Clear Software SMI Status
                3. Set EOS.
                4. Return EFI_SUCCESS.
**/

EFI_STATUS 
EFIAPI
PeiDeactivate (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN PEI_SMM_CONTROL_PPI  *This,
    IN BOOLEAN              Periodic OPTIONAL )
{
    if (Periodic) 
        return EFI_INVALID_PARAMETER;

    // Porting Required.  Include code to clear all SMI status
     // Clear the SW SMI Status Bit
    WRITE_IO8_PM(R_SB_SMI_STS, 0x20); // 0x34

    //Set EOS
    SET_IO8_PM(R_SB_SMI_EN, 0x02); // 0x30
    if ((READ_IO8_PM(R_SB_SMI_EN) & 0x02) == 0) {
        // Reset GBL_SMI_EN 
        RESET_IO8_PM(R_SB_SMI_EN, 0x01); // 0x30
        // Set EOS Again
        SET_IO8_PM(R_SB_SMI_EN, 0x02); // 0x30
        // Set GBL_SMI_EN 
        SET_IO8_PM(R_SB_SMI_EN, 0x01); // 0x30
    }
    // Porting end

    return EFI_SUCCESS;
}
#endif

/**
    Get the wake type and save it in PCD.

    @param  VOID

    @retval VOID

    @note
**/
VOID SbWakeupTypeLib (
    VOID
)
{
    SB_WAKEUP_TYPE  SbWakeType = DEFAULT_SYSTEM_WAKEUP_TYPE;

    UINT16          Pm1Status = IoRead16(PM_BASE_ADDRESS + SB_ACPI_PM1_STS);
    UINT32          Gpe0Status = IoRead32(PM_BASE_ADDRESS + R_SB_ACPI_GPE0_STS_127_96);
    
    if (Pm1Status & B_SB_ACPI_PM1_STS_WAK) {
        // Power button
//        if (Pm1Status & B_SB_ACPI_PM1_STS_PRBTNOR) {  // [ EIP401247 ]
        if (Pm1Status & B_SB_ACPI_PM1_STS_PWRBTN) {
            SbWakeType = SbWakeupTypePowerSwitch;
        } else if ((Pm1Status & B_SB_ACPI_PM1_STS_RTC) || (Gpe0Status & B_SB_ACPI_GPE0_STS_127_96_WADT)) {
            SbWakeType = SbWakeupTypeApmTimer;
        } else if (Gpe0Status & B_SB_ACPI_GPE0_STS_127_96_PME_B0) {
            SbWakeType = SbWakeupTypePciPme;
        }
    }

    PcdSet8S(PcdSbWakeType, SbWakeType);

    /**** PORTING REQUIRED ****/
    if (SbWakeType == SbWakeupTypeApmTimer) {
        PcdSet8S(AmiPcdCspNvsNtpb, 0);
    } else {
        PcdSet8S(AmiPcdCspNvsNtpb, 1);
    }
    /**** PORTING REQUIRED ****/
}

/**
    This function is PM Specific function to check and save CMOS
    Battery and Power Supply Power loss/failure in PcdSbPwrLossData.

    @param VOID

    @retval VOID

**/
VOID SbCheckPowerLoss (
    VOID
)
{
    SB_POWER_LOSS_DATA  SbPwrLossData;

    UINT32 				GenPmConA;
    UINT32 				GenPmConB;
    UINT32              PchPwrmBase;
    UINT32              GblRst0;             
    RTC_REG_D       	RegD;
   
    SbPwrLossData.PowerLossData = 0;

    PchPwrmBaseGet(&PchPwrmBase);
    
    GenPmConA = READ_PCI32(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, R_PCH_PMC_GEN_PMCON_A);
   
    GenPmConB = READ_PCI32(DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PMC, PCI_FUNCTION_NUMBER_PCH_PMC, R_PCH_PMC_GEN_PMCON_B);

    GblRst0 = MmioRead32(PchPwrmBase + R_PCH_PWRM_124);
    
    //Read RTC Power Register
    IoWrite8(CMOS_ADDR_PORT, RTC_REG_D_INDEX);
    RegD.REG_D = IoRead8(CMOS_DATA_PORT); // Read register.

    //TODO//TODO//TODO//TODO//TODO//TODO//TODO//TODO
    //Set the correct Code # here
    //Clear Power Failure/Status bits   
    //-------------------------------------------------------
    //this bit means it was a power loss during S0 or S1,
    //cleared by writing 1 to it
    if ((GblRst0 & BIT11) || (GblRst0 & BIT12)) SbPwrLossData.PwrOkFail = 1;

    //-------------------------------------------------------
    //this bit means it was a power loss on CPU VRM well 
    // during S0 or S1, cleared by writing 0 to it
    //if(GblRst0 & BIT12) SbPwrLossData.CpuPwrFail = 1;

    //-------------------------------------------------------
    //this bit means that S4 signal assertion was not long 
    //enough, cleared by writing 1 to it
    if(GenPmConA & B_PCH_PMC_GEN_PMCON_A_MS4V) SbPwrLossData.SlpS4Fail = 1;

    //-------------------------------------------------------
    //this bit means that a power was shut down because of 
    //CPU Power Trip, cleared by writing 1 to it
    if(GblRst0 & BIT5) SbPwrLossData.ThermTripSts = 1;
        
    //-------------------------------------------------------
    //this bit means that RESET# was active 
    //cleared by writing 1 to it
    //if(GenPmConA & BIT4) SbPwrLossData.ResetSts = 1;

    //-------------------------------------------------------
    //this bit means that Battery Bad or was removed
    //cleared by writing 1 to it
    if(GenPmConB & B_PCH_PMC_GEN_PMCON_B_PWR_FLR) SbPwrLossData.PwrFail = 1;

    //-------------------------------------------------------
    //this bit means that RTC Battery Bad or was removed
    //cleared by writing 0 to it
    if(GenPmConB & B_PCH_PMC_GEN_PMCON_B_RTC_PWR_STS) SbPwrLossData.RtcLostPower = 1;
    
    if(RegD.DataValid==0) SbPwrLossData.RtcLostPower = 1;
    
    PcdSet16S(PcdSbPwrLossData, SbPwrLossData.PowerLossData);
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
