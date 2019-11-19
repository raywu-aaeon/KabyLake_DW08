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
//*************************************************************************
/** @file NbPei.c
    This file contains code for North Bridge initialization
    in the PEI stage

**/
//*************************************************************************

//----------------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------------
#include <Efi.h>
#include <Pei.h>
#include <Token.h>
#include <AmiPeiLib.h>
#include <AmiCspLib.h>
#include <Guid/AcpiVariableCompatibility.h>
#include <Ppi/PciCfg2.h>
#include <Ppi/CpuIo.h>
#include <Ppi/NbPpi.h>
#include <Ppi/NbCpuOnlyResetPpi.h>
#include <Ppi/MemoryErrorReportPpi.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/MemoryDiscovered.h>
#include <Guid/NbChipsetGuid.h>
#include <Setup.h>
#include <CpuHobs.h>
#include <Protocol/NBMemInfo.h>
#include <Ppi/BaseMemoryTest.h>

#include <Protocol/SaPolicy.h>
#include <Ppi/Wdt.h>
#include <Guid/MemoryTypeInformation.h>
#include <MrcDebugHook.h>
#include <McAddress.h>
#include <Library/SmbusLib.h>
#include <Library/PchInfoLib.h>
#include <Library/HobLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/AmiSbMiscLib.h>
#include <IndustryStandard/Pci30.h>
#include <Ppi/SiPolicy.h>
#include <Library/PcdLib.h>

#ifdef SSA_FLAG
#include "SsaCallbackPeim.h"
#endif

#ifndef INTEL_MMA_SUPPORT
#define INTEL_MMA_SUPPORT 0
#endif

#define PEI_MR_GRAPHICS_MEMORY_2M_NOCACHE   0x00000004
#define PEI_MR_GRAPHICS_MEMORY_2M_CACHE     0x80000004
//----------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//----------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)
VOID
EFIAPI
NBResetCpuOnly (               // Do CPU Only Reset
    IN EFI_PEI_SERVICES         **PeiServices 
);

VOID 
ProgramNBSubId (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN EFI_PEI_PCI_CFG2_PPI     *PciCfg
);

EFI_STATUS
EFIAPI
BaseMemoryTest (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN struct _PEI_BASE_MEMORY_TEST_PPI *This,
    IN EFI_PHYSICAL_ADDRESS     BeginAddress,
    IN UINT64                   MemoryLength,
    IN PEI_MEMORY_TEST_OP       Operation,
    OUT EFI_PHYSICAL_ADDRESS    *ErrorAddress
);

EFI_STATUS
EFIAPI
NBPeiSiPreMemPolicyPpiNotify (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi
);

EFI_STATUS
EFIAPI
NBPeiBeforeMrcPei (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi
);

EFI_STATUS 
EFIAPI
NBPeiAfterMrcPei (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi
);

EFI_STATUS 
EFIAPI
NBPeiEndOfMrcPei (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi
);

EFI_STATUS 
EFIAPI
NbPeiInitAfterMemInstalled (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi
);

EFI_STATUS 
EFIAPI
ProgramNBRegBeforeEndofPei (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi
);

EFI_STATUS 
CreateCPUHob(
  IN EFI_PEI_SERVICES               **PeiServices
);

#ifdef SSA_FLAG
MrcStatus
AmiSsaCallbackPpi (
  struct _SSA_BIOS_CALLBACKS_PPI   *SsaBiosCallBacksPpi,
  MRC_OEM_STATUS_COMMAND           StatusCommand,
  VOID                             *CheckpointData
);
#endif // SSA_FLAG

EFI_STATUS
EFIAPI
MemoryErrorRead (
  IN      EFI_PEI_SERVICES              **PeiServices,
  IN      PEI_MEMORY_ERROR_REPORT_PPI   *This,
  IN OUT  UINT32                        MemErrData
);

EFI_STATUS
PlatformInfoPeiInit (
  IN EFI_PEI_SERVICES       **PeiServices
  );

VOID 
NbPegDetectNonComplaint (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN  NB_SETUP_DATA               *NbSetupData,
    IN OUT UINT32                   *MaxLinkSpeed
);

//----------------------------------------------------------------------------
// Variable and External Declaration(s)
//----------------------------------------------------------------------------
// Variable Declaration(s)

// GUID Definition(s)
EFI_GUID gEfiPeiEndOfPeiPhasePpiGuid     = EFI_PEI_END_OF_PEI_PHASE_PPI_GUID;
EFI_GUID gSetupGuid                      = SETUP_GUID;

// PPI Definition(s)

// PPI that are installed
static AMI_PEI_NBINIT_POLICY_PPI gAMIPEINBInitPolicyPpi = {
    TRUE
};

static PEI_BASE_MEMORY_TEST_PPI gBaseMemoryTest = {
    BaseMemoryTest
};

static EFI_PEI_NB_CPU_ONLY_RESET_PPI gNBCpuOnlyResetPpi = { 
    NBResetCpuOnly
};

#ifdef SSA_FLAG
static SSA_BIOS_CALLBACKS_PPI gAmiSsaCallbackPpi = {
    0,
    AmiSsaCallbackPpi,
    NULL
};
#endif // SSA_FLAG

static PEI_MEMORY_ERROR_REPORT_PPI gPeiMemoryErrorReportPpi = { 
    MemoryErrorRead
};

//  Include any additional PPI needed for memory detection in this
//  list and define the functions in this file

static EFI_PEI_PPI_DESCRIPTOR mPpiList[] = {
#ifdef SSA_FLAG
#if defined INTEL_MMA_SUPPORT && INTEL_MMA_SUPPORT == 0
    { EFI_PEI_PPI_DESCRIPTOR_PPI,
      &gSsaBiosCallBacksPpiGuid,
      &gAmiSsaCallbackPpi},
#endif      
#endif // SSA_FLAG
    { EFI_PEI_PPI_DESCRIPTOR_PPI, 
      &gAmiMemoryErrorReportPpiGuid,
      &gPeiMemoryErrorReportPpi },
    { EFI_PEI_PPI_DESCRIPTOR_PPI,
      &gPeiBaseMemoryTestPpiGuid,
      &gBaseMemoryTest },
    { EFI_PEI_PPI_DESCRIPTOR_PPI,
      &gAmiPeiNbCpuOnlyResetPpiGuid,
      &gNBCpuOnlyResetPpi },
    { (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
      &gAmiPeiNbInitPolicyGuid,
      &gAMIPEINBInitPolicyPpi }
};

#if defined DIMM_VOLTAGE_CHECK_SUPPORT && DIMM_VOLTAGE_CHECK_SUPPORT == 1
static EFI_PEI_PPI_DESCRIPTOR mAmiCheckDimmVoltageFail[] = {
    { (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
      &gAmiCheckDimmVoltageFailPpiGuid,
      NULL }
};
#endif

// PPI that are notified

static EFI_PEI_NOTIFY_DESCRIPTOR mNotifyList[] = {
    { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK, \
      &gSiPreMemPolicyPpiGuid, NBPeiSiPreMemPolicyPpiNotify },
    { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK, \
      &gAmiPeiBeforeMrcGuid, NBPeiBeforeMrcPei },
    { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK, \
      &gAmiPeiAfterMrcGuid, NBPeiAfterMrcPei },      
    { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK, \
      &gAmiPeiEndOfMemDetectGuid, NBPeiEndOfMrcPei },
    { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK, \
      &gEfiPeiMemoryDiscoveredPpiGuid, NbPeiInitAfterMemInstalled },
    { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | \
      EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
      &gEfiPeiEndOfPeiPhasePpiGuid, ProgramNBRegBeforeEndofPei },
};

// External Declaration(s)

// Function Definition(s)
#if 0
UINT32 NBPcieBridge[] = 
{
  {(UINT32)NB_PCIE_CFG_ADDRESS(PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN, PCI_VENDOR_ID_OFFSET)},
  {(UINT32)NB_PCIE_CFG_ADDRESS(PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN1, PCI_VENDOR_ID_OFFSET)},
  {(UINT32)NB_PCIE_CFG_ADDRESS(PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN2, PCI_VENDOR_ID_OFFSET)},
  {0xFFFFFFFF}
};
#endif
typedef struct {
    UINT8 Bus;
    UINT8 Dev;
    UINT8 Fun;
    UINT8 Reg;
} DEVICES_PCI_LIST;

DEVICES_PCI_LIST gNBPciBridge[] = {
    { PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN,  PCI_VENDOR_ID_OFFSET },
    { PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN1, PCI_VENDOR_ID_OFFSET },
    { PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN2, PCI_VENDOR_ID_OFFSET }
};

UINTN gNBPciBridgeCount = sizeof(gNBPciBridge) / sizeof(DEVICES_PCI_LIST);

//(EIP96114)
// Type Definition(s)
typedef struct {
    UINT8  Bus;
    UINT8  Dev;
    UINT8  Fun;
    UINT16 PegBitOffset;
} DEVICES_AFTER_PCIIO;

DEVICES_AFTER_PCIIO gDevicesTable[] = {
    { PCIEBRN_BUS, PCIEBRN_DEV,  PCIEBRN_FUN2, UNCORE_CR_DEVEN_0_0_0_PCI_D1F2EN_MSK },
    { PCIEBRN_BUS, PCIEBRN_DEV,  PCIEBRN_FUN1, UNCORE_CR_DEVEN_0_0_0_PCI_D1F1EN_MSK },
    { PCIEBRN_BUS, PCIEBRN_DEV,  PCIEBRN_FUN,  UNCORE_CR_DEVEN_0_0_0_PCI_D1F0EN_MSK }
};

UINTN gDevicesTableCount = sizeof(gDevicesTable) / sizeof(DEVICES_AFTER_PCIIO);
//(EIP96114)

//----------------------------------------------------------------------------

/**
    This function is the entry point for this PEI.
    it must be ported to do NB specific programming needed
    at power-on, both in wakeup path as well as power-on path.

    @param FfsHeader   Pointer to the FFS file header
        PeiServices Pointer to the PEI services table

    @retval Return Status based on errors that occurred while waiting for
        time to expire.

    @note  This function should initialize North Bridge before memory
              detection.
              Install AMI_PEI_NBINIT_POLICY_PPI to indicate that NB Init
              PEIM is installed
**/

EFI_STATUS
EFIAPI
NbPeiInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices)
{
    EFI_STATUS                  Status;


    PEI_PROGRESS_CODE (PeiServices, PEI_CAR_NB_INIT);

    //Create CPU HOB
    CreateCPUHob((EFI_PEI_SERVICES **) PeiServices);

    // For PCIE Length dymanic adjustment (If this feature supported)
#ifdef AMI_PCIE_DYNAMIC_SUPPORT_FLAG
{
     NB_SETUP_DATA               NbSetupData = {0};

    GetNbSetupData (PeiServices, &NbSetupData, TRUE);

    if(NbSetupData.PciExpressLength != 0) {

       UINT32 PcieValue32 = READ_PCI32(NB_BUS, NB_DEV, NB_FUN, R_SA_PCIEXBAR) & (~BIT0); // Save value 32

       UINT8 PcieLenBits = 0;

       WRITE_PCI32(NB_BUS, NB_DEV, NB_FUN, R_SA_PCIEXBAR, 0); // 0x60

       switch (NbSetupData.PciExpressLength) {
         case 1: // 64MB
             PcieValue32 = 0xF8000004;
             break;

         case 2: // 128MB
             PcieValue32 = 0xF0000002;
             break;

         case 3: // 256MB
             PcieValue32 = 0xE0000000;
             break;

         default: // 256MB
            break;
       }

       // Set pcie base and Pcds values
       WRITE_PCI32(NB_BUS, NB_DEV, NB_FUN, R_SA_PCIEXBAR, PcieValue32); // 0x60 
       SET_PCI32(NB_BUS, NB_DEV, NB_FUN, R_SA_PCIEXBAR, BIT0); // Enable PCIE base address.
       PcdSet64S (PcdPciExpressBaseAddress, PCIEX_BASE_ADDRESS);
       PcieLenBits = ((UINT8)PcieValue32) & (BIT1 | BIT2);

       if (PcieLenBits == BIT1) { // 128 MB
          PcdSet32S (PcdPciExpressRegionLength, 0x8000000);
       } else if (PcieLenBits == BIT2) { // 64 MB
          PcdSet32S (PcdPciExpressRegionLength, 0x4000000);
       } else {
          PcdSet32S (PcdPciExpressRegionLength, 0x10000000); // 256MB
       }

    }
}
#endif

    // Install the NB Init Policy PPI
    Status = (*PeiServices)->InstallPpi(PeiServices, &mPpiList[0]);
    if (EFI_ERROR (Status))
	{
        ASSERT_EFI_ERROR (Status);
	}

    // Set up necessary PPI notifications
    Status = (*PeiServices)->NotifyPpi( PeiServices, &mNotifyList[0] );
    if (EFI_ERROR (Status))
	{
        ASSERT_EFI_ERROR (Status);
	}

    // Program SSID
#ifndef INSTALL_INTEL_SSID
    ProgramNBSubId( (EFI_PEI_SERVICES **)(PeiServices), (*PeiServices)->PciCfg );
#endif    

    return EFI_SUCCESS;
}


/**
    This function issues a CPU only reset.

    @param PeiServices Pointer to the PEI services table

    @retval VOID
**/

VOID EFIAPI NBResetCpuOnly (               // Do CPU Only Reset
    IN EFI_PEI_SERVICES     **PeiServices )
{

}

/**
    This function programs NB PCI devices sub-vendor ID and
    sub-system ID.

    @param PeiServices Pointer to the PEI services table
    @param PciCfg Pointer to the PCI Configuration PPI

    @retval VOID

    @note  1. This routine only programs the PCI device in NB, hence, we
                 have to check the bus/device/function numbers whether they
                 are a NB PCI device or not.
              2. This routine is invoked by PEI phase.(After PEI permantent
                 memory be installed)
**/
VOID ProgramNBSubId (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN EFI_PEI_PCI_CFG2_PPI      *PciCfg )
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    AMI_PEI_NB_CUSTOM_PPI       *NBPeiOemPpi;
    UINTN                       i = 0;
    UINT8                       SsidReg = 0;
    UINT32                      PciSid = 0xffffffff;
    AMI_NB_PCI_SSID_TABLE_STRUCT DefaultSIdTbl[] = {NB_PCI_DEVICES_SSID_TABLE};
    AMI_NB_PCI_SSID_TABLE_STRUCT *SsidTblPtr = DefaultSIdTbl;

    Status = (*PeiServices)->LocatePpi( (CONST EFI_PEI_SERVICES**)PeiServices, \
                                        &gAmiPeiNbCustomPpiGuid, \
                                        0, \
                                        NULL, \
                                        (VOID **)&NBPeiOemPpi );

    if (Status == EFI_SUCCESS) {
        if (NBPeiOemPpi->SsidTable != NULL) 
            SsidTblPtr = NBPeiOemPpi->SsidTable;
    }

    while (SsidTblPtr[i].PciAddr != 0xffffffffffffffff) {
        if ((SsidTblPtr[i].PciAddr == NB_BUS_DEV_FUN) || \
            (SsidTblPtr[i].PciAddr == NB_IGD_BUS_DEV_FUN) || \
            (SsidTblPtr[i].PciAddr == NB_PCIEBRNx16_BUS_DEV_FUN) || \
            (SsidTblPtr[i].PciAddr == NB_PCIEBRNx8_BUS_DEV_FUN) || \
            (SsidTblPtr[i].PciAddr == NB_PCIEBRNx4_BUS_DEV_FUN) || \
            (SsidTblPtr[i].PciAddr == NB_IMGU_BUS_DEV_FUN) || \
            (SsidTblPtr[i].PciAddr == NB_SA_THERMAL_BUS_DEV_FUN) || \
            (SsidTblPtr[i].PciAddr == NB_SA_CHAP_BUS_DEV_FUN) || \
            (SsidTblPtr[i].PciAddr == NB_GMM_BUS_DEV_FUN)) \
        {
            if (SsidTblPtr[i].Sid == 0xffffffff) {
                Status = PciCfg->Read( (CONST EFI_PEI_SERVICES**)PeiServices,
                                       PciCfg,
                                       EfiPeiPciCfgWidthUint32,
                                       SsidTblPtr[i].PciAddr,
                                       &PciSid);
            } else {
                PciSid = SsidTblPtr[i].Sid;
            }

            SsidReg = PCI_SVID_OFFSET;

            if((SsidTblPtr[i].PciAddr == NB_PCIEBRNx16_BUS_DEV_FUN) || \
               (SsidTblPtr[i].PciAddr == NB_PCIEBRNx8_BUS_DEV_FUN) || \
               (SsidTblPtr[i].PciAddr == NB_PCIEBRNx4_BUS_DEV_FUN))
            {
              SsidReg = R_SA_PEG_SS_OFFSET;
            }

            Status = PciCfg->Write( (CONST EFI_PEI_SERVICES**)PeiServices,
                                    PciCfg,
                                    EfiPeiPciCfgWidthUint32,
                                    SsidTblPtr[i].PciAddr | SsidReg,
                                    &PciSid);

        }

        i++;
    }


}

#if defined DIMM_VOLTAGE_CHECK_SUPPORT && DIMM_VOLTAGE_CHECK_SUPPORT == 1
/**
    Check Dimm Voltage that is matched with CPU Support or not.

    @param PeiServices Pointer to the PEI services table

    @retval VOID
**/
VOID CheckDimmVoltage (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN SA_MISC_PEI_PREMEM_CONFIG    *MiscConfig
)
{
    EFI_STATUS                  Status;

    UINT8                       i;
    UINT8                       SpdAddress;    
    UINTN                       SmBusAddress;
    UINT8                       SpdVoltageData = 0; 
    PCH_SERIES                  PchSeries = GetPchSeries();
 
    for (i = 0; i < 4; i++) {
      SpdAddress = MiscConfig->SpdAddressTable[i];
      if (SpdAddress != 0) {
          SmBusAddress = (UINTN)SpdAddress | 0x600;  // Read Spd Byte6
          SpdVoltageData = SmBusReadDataByte (SmBusAddress, &Status);
          if (EFI_ERROR(Status)) continue;
          
          if (PchSeries == PchLp) {
              // Bit2: 1.2X V, Bit1: 1.35 V
              if (!(SpdVoltageData & 0x06)) {
                  DEBUG((DEBUG_INFO, "SpdAddress = %x\n", SpdAddress));
                  (*PeiServices)->InstallPpi(PeiServices, mAmiCheckDimmVoltageFail);
                  break;
              }
          }
      }
    }
    
    return;
}
#endif

/**
    This function can be Call NB PEI before Mrc.

    @param PeiServices Pointer to the PEI services table
    @param NotifyDescriptor Pointer to the descriptor for the
        notification event.
    @param InvokePpi Pointer to the PPI that was installed

    @retval EFI_STATUS
**/

EFI_STATUS EFIAPI NBPeiBeforeMrcPei (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi )
{
    EFI_STATUS                      Status;
    EFI_BOOT_MODE                   BootMode;
    SI_PREMEM_POLICY_PPI            *SiPreMemPolicyPpi;
    SA_MISC_PEI_PREMEM_CONFIG       *MiscPeiPreMemConfig;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI *ReadOnlyVariable;
    UINTN                           VariableSize;
    VOID                            *MemorySavedData;
   
    DEBUG((DEBUG_INFO, "NBPeiBeforeMrc Start.\n"));

    Status = (*PeiServices)->GetBootMode ( (CONST EFI_PEI_SERVICES**)PeiServices, &BootMode);
    ASSERT_EFI_ERROR (Status);
        
    Status = (*PeiServices)->LocatePpi (
                               (CONST EFI_PEI_SERVICES**)PeiServices,
                               &gSiPreMemPolicyPpiGuid,
                               0,
                               NULL,
                               (VOID **) &SiPreMemPolicyPpi
                               );
    ASSERT_EFI_ERROR (Status);
    
    Status = GetConfigBlock((VOID *)SiPreMemPolicyPpi, &gSaMiscPeiPreMemConfigGuid, (VOID *)&MiscPeiPreMemConfig);
    if (EFI_ERROR(Status)) return Status;
	
    Status = (*PeiServices)->LocatePpi (
                               (CONST EFI_PEI_SERVICES**)PeiServices,
                               &gEfiPeiReadOnlyVariable2PpiGuid,
                               0,
                               NULL,
                               (VOID **)&ReadOnlyVariable
                               );
    ASSERT_EFI_ERROR(Status);
        
    VariableSize = 0;
    MemorySavedData = NULL;
    Status = ReadOnlyVariable->GetVariable (
                                 ReadOnlyVariable,
                                 L"MemoryConfig",
                                 &gMemoryConfigVariableGuid,
                                 NULL,
                                 &VariableSize,
                                 MemorySavedData);
    if (Status != EFI_BUFFER_TOO_SMALL) {
        //
        // If can't get L"MemoryConfig", it indicates MiscPeiPreMemConfig->S3DataPtr is NULL.
        // Clear DRAM Initialization Scratchpad Bit (DISB).
        //
        DEBUG((DEBUG_INFO, "NbPei: Clear DISB.\n"));
        WRITE_MMIO8((UINTN)NB_PCIE_CFG_ADDRESS (0, 0x1f, 2, 0xA2), 0x00);
    }
    
    if (BootMode == BOOT_ON_FLASH_UPDATE) {
        //
        // Set MCH BAR SSKP.
        //
        SET_MMIO8(MiscPeiPreMemConfig->MchBar + 0x5D10, 1);
    }

#if defined DIMM_VOLTAGE_CHECK_SUPPORT && DIMM_VOLTAGE_CHECK_SUPPORT == 1
    CheckDimmVoltage (PeiServices, MiscPeiPreMemConfig);
#endif

    DEBUG((DEBUG_INFO, "NBPeiBeforeMrc end.\n"));

    return  Status;
}

/**
    This function can be Call NB PEI after Mrc.

    @param PeiServices Pointer to the PEI services table
    @param NotifyDescriptor Pointer to the descriptor for the
        notification event.
    @param InvokePpi Pointer to the PPI that was installed

    @retval EFI_STATUS
**/

EFI_STATUS EFIAPI NBPeiAfterMrcPei (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi )
{
    EFI_STATUS                  Status;
    EFI_BOOT_MODE               BootMode;
    EFI_PHYSICAL_ADDRESS        LegacyMemClear = 0xE0000;   
    UINTN                       VariableSize = sizeof(UINT32);
    EFI_PEI_READ_ONLY_VARIABLE2_PPI *ReadOnlyVariable;
    UINT64                      AcpiVariableSet64;
    ACPI_VARIABLE_SET_COMPATIBILITY *pAcpiVariableSet = NULL;
    SI_PREMEM_POLICY_PPI        *SiPreMemPolicyPpi;    
    SA_MISC_PEI_PREMEM_CONFIG   *MiscPeiPreMemConfig;    
    
    DEBUG((DEBUG_INFO, "NBPeiAfterMrc Start.\n"));
    
    Status = (*PeiServices)->GetBootMode ( (CONST EFI_PEI_SERVICES**) PeiServices, &BootMode);
    if (EFI_ERROR(Status))
    {    
        ASSERT_EFI_ERROR (Status);
    }
    
    if (BootMode != BOOT_ON_S3_RESUME) {
      DEBUG((DEBUG_INFO, "Clear uninitialized memory E0000 and F0000.\n"));
      NbRuntimeShadowRamWrite(TRUE);      
      (*PeiServices)->SetMem ((VOID *)(UINTN)LegacyMemClear, (UINTN)0x20000, (UINT8)0);
      NbRuntimeShadowRamWrite(FALSE);
      
    } else {
       Status = (*PeiServices)->LocatePpi( (CONST EFI_PEI_SERVICES**) PeiServices, \
                                           &gEfiPeiReadOnlyVariable2PpiGuid, \
                                           0, \
                                           NULL, \
                                           (VOID **)&ReadOnlyVariable );
       if (EFI_ERROR(Status))
       {    
           ASSERT_EFI_ERROR (Status);
       }
    
       VariableSize = sizeof (AcpiVariableSet64);  // Refer to EIP216501.
       Status = ReadOnlyVariable->GetVariable( ReadOnlyVariable, \
                                               ACPI_GLOBAL_VARIABLE, \
                                               &gEfiAcpiVariableCompatiblityGuid, \
                                               NULL, \
                                               &VariableSize, \
                                               &AcpiVariableSet64 );
       if (EFI_ERROR(Status))
       {    
           ASSERT_EFI_ERROR (Status);
       }
       pAcpiVariableSet = (ACPI_VARIABLE_SET_COMPATIBILITY *) (UINTN) AcpiVariableSet64;
    
       Status = (*PeiServices)->LocatePpi (
                                  (CONST EFI_PEI_SERVICES**)PeiServices,
                                  &gSiPreMemPolicyPpiGuid,
                                  0,
                                  NULL,
                                  (VOID **) &SiPreMemPolicyPpi
                                  );
       if (EFI_ERROR(Status))
       {    
           ASSERT_EFI_ERROR (Status);
       }

       Status = GetConfigBlock((VOID *)SiPreMemPolicyPpi, &gSaMiscPeiPreMemConfigGuid, (VOID *)&MiscPeiPreMemConfig);        
       if (EFI_ERROR(Status))
       {    
           ASSERT_EFI_ERROR (Status);
       }
    
       MiscPeiPreMemConfig->AcpiReservedMemoryBase = pAcpiVariableSet->AcpiReservedMemoryBase;
       MiscPeiPreMemConfig->AcpiReservedMemorySize = pAcpiVariableSet->AcpiReservedMemorySize;
       MiscPeiPreMemConfig->SystemMemoryLength = pAcpiVariableSet->SystemMemoryLength;
       
       DEBUG((DEBUG_INFO, "AcpiMemoryBase is 0x%8x\n", pAcpiVariableSet->AcpiReservedMemoryBase));
       DEBUG((DEBUG_INFO, "AcpiMemorySize is 0x%8x\n", pAcpiVariableSet->AcpiReservedMemorySize));
       DEBUG((DEBUG_INFO, "SystemMemoryLength is 0x%8x\n", pAcpiVariableSet->SystemMemoryLength));
    }
    
    DEBUG((DEBUG_INFO, "NBPeiAfterMrc end.\n"));

    return EFI_SUCCESS;
}

/**
    This function can be Call NB PEI after SaPolicy Ppi Installed.

    @param PeiServices Pointer to the PEI services table
    @param NotifyDescriptor Pointer to the descriptor for the
        notification event.
    @param InvokePpi Pointer to the PPI that was installed

    @retval EFI_STATUS
**/

EFI_STATUS 
EFIAPI
NBPeiSiPreMemPolicyPpiNotify (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi )
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    SI_PREMEM_POLICY_PPI        *SiPreMemPolicyPpi;      
    PCIE_PEI_PREMEM_CONFIG      *PcieConfig;
    NB_SETUP_DATA               *NbSetupData = NULL;  
    PCH_SERIES                  PchSeries = GetPchSeries();

    Status = (*PeiServices)->LocatePpi (
                               (CONST EFI_PEI_SERVICES**)PeiServices,
                               &gSiPreMemPolicyPpiGuid,
                               0,
                               NULL,
                               (VOID **) &SiPreMemPolicyPpi
                               );
    ASSERT_EFI_ERROR (Status);

    Status = GetConfigBlock ((VOID *)SiPreMemPolicyPpi, &gSaPciePeiPreMemConfigGuid, (VOID *)&PcieConfig);    
    ASSERT_EFI_ERROR (Status);
    //
    // Allocate buffer for setup data variable.
    //
    Status = (*PeiServices)->AllocatePool ( (CONST EFI_PEI_SERVICES**)PeiServices, sizeof (NB_SETUP_DATA), (VOID **)&NbSetupData);
    ASSERT_EFI_ERROR (Status);
    (*PeiServices)->SetMem ((VOID*) NbSetupData, sizeof (NB_SETUP_DATA), 0);

    GetNbSetupData( PeiServices, NbSetupData, TRUE );

    DEBUG((DEBUG_INFO, "NBPeiSiPreMemPolicyPpiNotify - START!\n"));

#if defined (NB_SETUP_SUPPORT) && NB_SETUP_SUPPORT == 1
    DEBUG((DEBUG_INFO, " PrimaryPeg : 0x%X\n", NbSetupData->PrimaryPeg));
    DEBUG((DEBUG_INFO, " PrimaryPcie : 0x%X\n", NbSetupData->PrimaryPcie));
    DEBUG((DEBUG_INFO, " DetectNonComplaint : 0x%X\n", NbSetupData->DetectNonComplaint));

#if (defined RC_PEG_0) && RC_PEG_0 == 1
    if (NbSetupData->DetectNonComplaint == 1 && PchSeries != PchLp) {
        ///
        /// Set PEG MaxLinkSpeed to Gen1, ensure that the PciExpressInit is successful.
        ///
        if (NbSetupData->Peg0MaxLinkSpeed == 0) {
            DEBUG((DEBUG_INFO, "PEG0 Enable\\"));
            PcieConfig->Peg0Enable = 1;
            PcieConfig->Peg0MaxLinkSpeed = 0x1;
        }
        if (NbSetupData->Peg1MaxLinkSpeed == 0) {
            DEBUG((DEBUG_INFO, "PEG1 Enable\\"));
            PcieConfig->Peg1Enable = 1;
            PcieConfig->Peg1MaxLinkSpeed = 0x1;
        }
        if (NbSetupData->Peg2MaxLinkSpeed == 0) {
            DEBUG((DEBUG_INFO, "PEG2 Enable\\"));
            PcieConfig->Peg2Enable = 1;
            PcieConfig->Peg2MaxLinkSpeed = 0x1;
        }
        DEBUG((DEBUG_INFO, "\n"));
    }
#endif
#endif

    DEBUG((DEBUG_INFO, "NBPeiSiPreMemPolicyPpiNotify - END!\n"));

    return  Status;
}

/**
    This function is Adjust Peg Device Link Speed, 
    If the command is 0, just execute device retrain.

    @param 
    @retval VOID

**/
VOID AdjustPegDeviceLinkSpeed (
    IN UINT8  PegBus,
    IN UINT8  PegDev,
    IN UINT8  PegFun,
    IN UINT8  CapId,
    IN UINT16 Command
) {

    ///
    /// Link Disable
    ///
    WRITE_PCI16(PegBus, PegDev, PegFun, CapId + R_PCIE_LCTL_OFFSET, B_PCIE_LCTL_LD);
    CountTime(50 * 4000, PM_BASE_ADDRESS); // delay 200ms

    ///
    /// Adjust Link Speed (TLS), If the command is 0 then do nothing.
    ///
    if(Command) {
        RW_PCI16(PegBus, PegDev, PegFun, CapId + R_PCIE_LCTL2_OFFSET, (UINT16)(Command), B_PCIE_LCTL2_TLS);
        CountTime(50 * 4000, PM_BASE_ADDRESS); // delay 200ms
    }

    ///
    /// Retrain Link
    ///
    WRITE_PCI16(PegBus, PegDev, PegFun, CapId + R_PCIE_LCTL_OFFSET, B_PCIE_LCTL_RL);
    CountTime(50 * 8000, PM_BASE_ADDRESS); // delay 400ms

}

/**
    This function is Detect peg NonComplaint devices.

    @param PeiServices Pointer to the PEI services table
        NB_SETUP_DATA    - NbSetupData - NB Setup data stored in NVRAM

    @retval VOID

**/

VOID NbPegDetectNonComplaint (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN  NB_SETUP_DATA               *NbSetupData,
    IN OUT UINT32                   *MaxLinkSpeed
)
{
    UINT16  i;
    UINT8   PegBus;
    UINT8   PegDev;
    UINT8   PegFun;
    UINT8   CapId = 0;
    UINT16  Command;
    UINT32  PegAddress;
    EFI_STATUS  Status = EFI_SUCCESS;
    NB_PEG_INFO_HOB *NbPegInfoHob;
    UINT16  PegXMaxLinkSpeed[] = {
      NbSetupData->Peg2MaxLinkSpeed,
      NbSetupData->Peg1MaxLinkSpeed,
      NbSetupData->Peg0MaxLinkSpeed
    };
    UINT8                   PegDisableMask;

    DEBUG((DEBUG_INFO, "NbPegDetectNonComplaint - START!\n"));

    Status = (*PeiServices)->CreateHob ( (CONST EFI_PEI_SERVICES**) PeiServices, \
                                         EFI_HOB_TYPE_GUID_EXTENSION, \
                                         sizeof (NB_PEG_INFO_HOB), \
                                         (VOID **) &NbPegInfoHob );
    if (EFI_ERROR (Status)) return ;

    NbPegInfoHob->Header.Name = gAmiNbPegInfoGuid;

    //
    // Bit 0 : Indicate B0/D1/F0 shall be disabled if Bit0 is set
    // Bit 1 : Indicate B0/D1/F1 shall be disabled if Bit1 is set
    // Bit 2 : Indicate B0/D1/F2 shall be disabled if Bit2 is set
    //
	PegDisableMask = 0;

    for ( i = 0; i < gDevicesTableCount; i++) {

        PegBus = gDevicesTable[i].Bus;
        PegDev = gDevicesTable[i].Dev;
        PegFun = gDevicesTable[i].Fun;
        NbPegInfoHob->PegDeOverride[PegFun] = NbSetupData->PegDeEmphasis[PegFun];

        if (PegXMaxLinkSpeed[i] != 0x0 ) continue;

        if (READ_PCI32(PegBus, PegDev, PegFun, 0) == 0xffffffff) continue;


        PegAddress = NB_PCIE_CFG_ADDRESS(PegBus, PegDev, PegFun, 0);

        ///
        ///
        /// Set PEG PortBus = 1 to Read Endpoint (Assign temp bus)
        /// (Bus Numbers)  Primary(18)    Secondary(19)   Subordinate(1A)
        /// --------------------------------------------------------------
        ///                        00               01                01
        ///
        WRITE_PCI32 (PegBus, PegDev, PegFun, PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET, 0x00010100);

        ///
        /// A config write is required in order for the device to re-capture the Bus number,
        /// according to PCI Express Base Specification, 2.2.6.2
        /// Write to a read-only register VendorID to not cause any side effects.
        ///
        WRITE_PCI16 (1, 0, 0, PCI_VENDOR_ID_OFFSET, 0x0);
        CountTime(50 * 8000, PM_BASE_ADDRESS); // delay 400ms

        ///
        /// Find the PCIe Capability ID
        ///
        CapId = NbFindCapPtr(PegAddress, EFI_PCI_CAPABILITY_ID_PCIEXP);
        // DEBUG((DEBUG_INFO, "PCIe Capability ID: 0x%X.\n", CapId));

        ///
        /// Try to speed to Gen3 (TLS to Gen 3)
        ///
        DEBUG((DEBUG_INFO, "Set Speed to Gen3 frist.\n"));
        Command = V_PCIE_LCTL2_TLS_GEN3;
        AdjustPegDeviceLinkSpeed (PegBus, PegDev, PegFun, CapId, Command);

        if (READ_PCI16(1, 0, 0, PCI_VENDOR_ID_OFFSET) == 0xFFFF) {

            DEBUG((DEBUG_INFO, " Can't find Device... Retrain device.\n"));

            ///
            /// Retrain device
            ///
            Command = 0;
            AdjustPegDeviceLinkSpeed (PegBus, PegDev, PegFun, CapId, Command);


            if (READ_PCI16(1, 0, 0, PCI_VENDOR_ID_OFFSET) == 0xFFFF) {

                DEBUG((DEBUG_INFO, "  Can't find Device in Gen3 Speed...\n  Set Speed to Gen2 and delay 1000 ms.\n"));

                ///
                /// Set Speed to Gen2 (TLS to Gen 2)
                ///
                Command = V_PCIE_LCTL2_TLS_GEN2;
                AdjustPegDeviceLinkSpeed (PegBus, PegDev, PegFun, CapId, Command);

                if (READ_PCI16(1, 0, 0, PCI_VENDOR_ID_OFFSET) == 0xFFFF) {

                    DEBUG((DEBUG_INFO, "   Still can't find Device in Gen2 Speed...\n   Set Speed to Gen1 and delay 1000 ms.\n"));

                    ///
                    /// Set Speed to Gen1 (TLS to Gen 1)
                    ///
                    Command = V_PCIE_LCTL2_TLS_GEN1;
                    AdjustPegDeviceLinkSpeed (PegBus, PegDev, PegFun, CapId, Command);

                    if (READ_PCI16(1, 0, 0, PCI_VENDOR_ID_OFFSET) == 0xFFFF) {

                        DEBUG((DEBUG_INFO, "    Still can't find Device in Gen1 Speed...\n    Retrain device again !!!\n"));

                        ///
                        /// Retrain device again
                        ///
                        Command = 0;
                        AdjustPegDeviceLinkSpeed (PegBus, PegDev, PegFun, CapId, Command);

                        if (READ_PCI16(1, 0, 0, PCI_VENDOR_ID_OFFSET) == 0xFFFF) {

                            ///
                            /// PDS - Presence Detect State: Slot Empty
                            ///
                            if ((READ_PCI8 (PegBus, PegDev, PegFun, R_SA_PEG_SLOTSTS_OFFSET) & BIT06) == 0) {
                                DEBUG((DEBUG_INFO, "Presence Detect State: 0x%X.\n", (READ_PCI8 (PegBus, PegDev, PegFun, R_SA_PEG_SLOTSTS_OFFSET) & BIT06) ));

							    if (PegDev == PCIEBRN_DEV) {
									PegDisableMask |= (BIT0 << PegFun);
									//
    								// Here is to prevent PEG10(root port) from disabling when PEG11 or PEG12 is existed.
    								//
									if (((PegDisableMask >> PCIEBRN_FUN2) & 0x01) == 0 || ((PegDisableMask >> PCIEBRN_FUN1) & 0x01) == 0) {
										PegDisableMask &= ~BIT0;
									}
                                }

								if ((PegDev == PCIEBRN_DEV) && ((PegDisableMask >> PegFun) & 0x01) == 1) {
                                ///
                                /// Link Disable
                                ///
                                WRITE_PCI16(PegBus, PegDev, PegFun, CapId + R_PCIE_LCTL_OFFSET, B_PCIE_LCTL_LD);

                                ///
                                /// Clear D0.F0.R 054h (DEVEN) enable bit.
                                ///
                                RESET_PCI8_NB (R_SA_DEVEN, gDevicesTable[i].PegBitOffset);

                            }

                            MaxLinkSpeed[i] = 0x1;
                            }
                        } else { // Always set device to Gen1

                            DEBUG((DEBUG_INFO, "VID on PEG: 0x%X.\n", READ_PCI16 (1, 0, 0, PCI_VENDOR_ID_OFFSET)));
                            DEBUG((DEBUG_INFO, "Force device to Gen1.\n"));
                            MaxLinkSpeed[i] = 0x1;
                        }

                    } else { // Find the device in Gen1

                        DEBUG((DEBUG_INFO, "VID on PEG: 0x%X.\n", READ_PCI16 (1, 0, 0, PCI_VENDOR_ID_OFFSET)));
                        MaxLinkSpeed[i] = 0x1;

                    }

                } else { // Find the device in Gen2

                    DEBUG((DEBUG_INFO, "VID on PEG: 0x%X.\n", READ_PCI16 (1, 0, 0, PCI_VENDOR_ID_OFFSET)));
                    MaxLinkSpeed[i] = 0x2;

                }

            } else { // Find the device in Gen3

                DEBUG((DEBUG_INFO, "VID on PEG: 0x%X.\n", READ_PCI16 (1, 0, 0, PCI_VENDOR_ID_OFFSET)));
                MaxLinkSpeed[i] = 0x3;

            }
        }

        WRITE_PCI32 (PegBus, PegDev, PegFun, PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET, 0x00000000);

    }

    DEBUG((DEBUG_INFO, "NbPegDetectNonComplaint - END!\n"));

}

/**
    This function is Clear PEG 0x3E bit3 for 3D card.

    @param PeiServices Pointer to the PEI services table
        NB_SETUP_DATA    - NbSetupData - NB Setup data stored in NVRAM

    @retval VOID

**/
VOID NbClearPegCtrlRegVgaEnable (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN  NB_SETUP_DATA               *NbSetupData
)
{
    UINT16  i;
    UINT8   PegBus;
    UINT8   PegDev;
    UINT8   PegFun;

    for ( i = 0; i < gDevicesTableCount; i++) 
    {
        PegBus = gDevicesTable[i].Bus;
        PegDev = gDevicesTable[i].Dev;
        PegFun = gDevicesTable[i].Fun;
        if (READ_PCI32(PegBus, PegDev, PegFun, 0) == 0xffffffff) continue;
        if ((READ_PCI8 (PegBus, PegDev, PegFun, R_SA_PEG_SLOTSTS_OFFSET) & BIT06) == 0) continue; // 0xBA

        WRITE_PCI32 (PegBus, PegDev, PegFun, PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET, 0x00010100);
        WRITE_PCI16 (1, 0, 0, PCI_VENDOR_ID_OFFSET, 0);

        // Clear Peg VgaEnable
        RESET_PCI8(PegBus, PegDev, PegFun, PCI_BRIDGE_CONTROL_REGISTER_OFFSET, BIT03);
        WRITE_PCI32 (PegBus, PegDev, PegFun, PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET, 0x00000000);
    }
}
//(EIP73801)
/**
    This function can be Call NB PEI after Mrc.

    @param PeiServices Pointer to the PEI services table
    @param NotifyDescriptor Pointer to the descriptor for the
        notification event.
    @param InvokePpi Pointer to the PPI that was installed

    @retval EFI_STATUS
**/

EFI_STATUS EFIAPI NBPeiEndOfMrcPei (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi )
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    NB_SETUP_DATA               *NbSetupData = NULL;
    WDT_PPI                     *WdtPpi = NULL;
    PCH_SERIES                  PchSeries = GetPchSeries();
    UINT32                      MaxLinkSpeed[3] = { 0 };
    EFI_BOOT_MODE               BootMode;

    DEBUG((DEBUG_INFO, "NBPeiEndOfMrc Start.\n"));

    //
    // Allocate buffer for setup data variable.
    //
    Status = (*PeiServices)->AllocatePool ( (CONST EFI_PEI_SERVICES**) PeiServices, sizeof (NB_SETUP_DATA), (VOID **)&NbSetupData);
    ASSERT_EFI_ERROR (Status);
    (*PeiServices)->SetMem ((VOID*) NbSetupData, sizeof (NB_SETUP_DATA), 0);

    GetNbSetupData( PeiServices, NbSetupData, TRUE );

    Status = (*PeiServices)->GetBootMode ( (CONST EFI_PEI_SERVICES**) PeiServices, &BootMode);  
    if ((!EFI_ERROR (Status)) && (BootMode == BOOT_ON_S3_RESUME)) {    
      // Set DRAM Initialization Bit.
      if ((READ_MMIO8(SB_PMC_MMIO_REG(0xA2)) & BIT07) == 0) {
        SET_MMIO8(SB_PMC_MMIO_REG(0xA2), BIT07);
      }
    }

    //
    // Locate WDT PPI for access to Wdt->Disable()
    //
    Status = (*PeiServices)->LocatePpi (
                      (CONST EFI_PEI_SERVICES**)PeiServices,
                      &gWdtPpiGuid,
                      0,
                      NULL,
                      (VOID **) &WdtPpi
                      );
    if (!EFI_ERROR (Status)) {
        WdtPpi->Disable();
    }

#if (defined RC_PEG_0) && RC_PEG_0 == 1
    if (NbSetupData->DetectNonComplaint == 1 && PchSeries != PchLp) {
        // Peg Detect NonComplaint devices.
        NbPegDetectNonComplaint(PeiServices, NbSetupData, MaxLinkSpeed);
    }
#endif

    // if SG and have pGPU
    if (READ_PCI32(0, 2, 0, 0) != 0xffffffff && NbSetupData->PrimaryDisplay == 4) {
       // Clear PEG 0x3E bit3 for SG mode.
       NbClearPegCtrlRegVgaEnable(PeiServices, NbSetupData);
    }

    DEBUG((DEBUG_INFO, "NBPeiEndOfMrc end.\n"));

    return  Status;
}

/**
    This function can be used to program any NB regisater after
    PEI permantent memory be installed.

    @param PeiServices Pointer to the PEI services table
    @param NotifyDescriptor Pointer to the descriptor for the
        notification event.
    @param InvokePpi Pointer to the PPI that was installed

    @retval EFI_STATUS
**/

EFI_STATUS EFIAPI NbPeiInitAfterMemInstalled (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi)
{
    return EFI_SUCCESS;
}

/**
    This function can be used to program any NB regisater before
    end of PEI phase.

    @param PeiServices Pointer to the PEI services table
    @param NotifyDescriptor Pointer to the descriptor for the
        notification event.
    @param InvokePpi Pointer to the PPI that was installed

    @retval EFI_STATUS
**/

EFI_STATUS EFIAPI ProgramNBRegBeforeEndofPei (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi )
{
    EFI_STATUS                  Status;
    EFI_BOOT_MODE               BootMode;
    UINTN                       PegAddress;
    UINT8                       CapPtr = 0;
    UINT8                       i = 0;
    UINT8                       PrimaryBus, SecondaryBus;
    UINT8                       PegBus, PegDev, PegFun, PegReg;
    
    Status = (*PeiServices)->GetBootMode ( (CONST EFI_PEI_SERVICES**) PeiServices, &BootMode);

    if (EFI_ERROR (Status))
    {
        return  EFI_SUCCESS;
    }
    
    if (BootMode == BOOT_ON_S3_RESUME) {
        // Porting if needed.

        for (i = 0; i < gNBPciBridgeCount; i++)
        {
            PegBus = gNBPciBridge[i].Bus;
            PegDev = gNBPciBridge[i].Dev;
            PegFun = gNBPciBridge[i].Fun;
            PegReg = gNBPciBridge[i].Reg;

            if (READ_PCI32(PegBus, PegDev, PegFun, PegReg) == 0xFFFFFFFF) continue;
            if (!(READ_PCI8(PegBus, PegDev, PegFun, R_SA_PEG_SLOTSTS_OFFSET) & BIT06)) continue;

            PrimaryBus = READ_PCI8(PegBus, PegDev, PegFun, PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET + 1);
            SecondaryBus = READ_PCI8(PegBus, PegDev, PegFun, PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET + 2);

            for (; PrimaryBus <= SecondaryBus; PrimaryBus++) { 
              PegAddress = NB_PCIE_CFG_ADDRESS(PrimaryBus, 0, 0, 0);

              // Network Card.
              if (READ_MEM8(PegAddress + 0x0B) == 0x02) { 
                 // Find Cap ID 0x01
                 CapPtr = NbFindCapPtr(PegAddress, 0x01); 
                 if(CapPtr != 0) {
                    // Clear PME status 
                    SET_MEM16((PegAddress + CapPtr + 0x04), BIT15); 
                 }
              }
            }

            // Clear root port PME status
            SET_PCI8(PegBus, PegDev, PegFun, 0xC2, BIT01);
        }

    } else {
	
#if (defined ENABLE_ABOVE_4G_MEM_DURING_BIOS_POST && (ENABLE_ABOVE_4G_MEM_DURING_BIOS_POST == 0))      	
       EFI_PEI_HOB_POINTERS    Hob;

       //
       // Get the HOB list for processing
       //
       Hob.Raw = GetHobList ();

       //
       // Collect memory ranges
       //
        while (!END_OF_HOB_LIST (Hob)) {
          if (Hob.Header->HobType == EFI_HOB_TYPE_RESOURCE_DESCRIPTOR) {
             if ((Hob.ResourceDescriptor->PhysicalStart >= BASE_4GB) &&
               (Hob.ResourceDescriptor->ResourceType == EFI_RESOURCE_SYSTEM_MEMORY)) {
               Hob.Header->HobType = EFI_HOB_TYPE_UNUSED;
               DEBUG((DEBUG_INFO, "NBPEI: Remove memory above 4G!.\n"));
             }
          }
          Hob.Raw = GET_NEXT_HOB (Hob);
        }
#endif
        
    }
    return  EFI_SUCCESS;
}

//----------------------------------------------------------------------------
// BaseMemoryTest
//----------------------------------------------------------------------------

/**
    This function performs the base memory test. 

    @param PeiServices Pointer to the PEI services table
    @param This Pointer to the Base Memory Test PPI
    @param BeginAddress The begin address for test.
    @param MemoryLength The length in byte for test.
    @param Operation The memort test operated policy.
    @param ErrorAddress The error address when test is failed.

        EFI_STATUS
    @retval EFI_DEVICE_ERROR Tge base memory test is failure.
**/

EFI_STATUS 
EFIAPI
BaseMemoryTest (
    IN  EFI_PEI_SERVICES            **PeiServices,
    IN  PEI_BASE_MEMORY_TEST_PPI    *This, 
    IN  EFI_PHYSICAL_ADDRESS        BeginAddress,
    IN  UINT64                      MemoryLength,
    IN  PEI_MEMORY_TEST_OP          Operation,
    OUT EFI_PHYSICAL_ADDRESS        *ErrorAddress )
{

#if defined PEI_MRC_BASE_MEMORY_TEST_ENABLE && PEI_MRC_BASE_MEMORY_TEST_ENABLE == 1
  UINT32                                 TestPattern;
  UINT32                                 TestMask;
  UINT32                                 SpanSize;
  EFI_PHYSICAL_ADDRESS                   TempAddress;

#if defined PeiRamBootSupport && PeiRamBootSupport == 1
  if(!CheckPeiFvCopyToRam(PeiServices)) return EFI_SUCCESS;
#endif

  (*PeiServices)->ReportStatusCode (
                                   PeiServices,
                                   EFI_PROGRESS_CODE,
                                   EFI_COMPUTING_UNIT_MEMORY + EFI_CU_MEMORY_PC_TEST,   
                                   0,
                                   NULL,
                                   NULL
                                   );

  TestPattern = 0x5A5A5A5A;
  TestMask = 0;
  SpanSize = 0;

  //
  // Make sure we don't try and test anything above the max physical address range
  //
  ASSERT_EFI_ERROR ((BeginAddress + MemoryLength < 0xFFFFFFFFFFFFFFFF));

  switch (Operation) {
    case Extensive:
      SpanSize = 0x4;
      break;
    case Sparse:
    case Quick:
      SpanSize = 0x40000;
      break;
    case Ignore:
      return EFI_SUCCESS;
  }

  //
  // Write the test pattern into memory range
  //
  TempAddress = BeginAddress;
  while (TempAddress < BeginAddress + MemoryLength) {
    (*(UINT32*)(UINTN)TempAddress) = TestPattern;
    TempAddress += SpanSize;
  }

  //
  // Read pattern from memory and compare it
  //
  TempAddress = BeginAddress;
  while (TempAddress < BeginAddress + MemoryLength) {
    if ((*(UINT32*)(UINTN)TempAddress) != TestPattern) {
      *ErrorAddress = TempAddress;
      (*PeiServices)->ReportStatusCode (
                                       PeiServices,
                                       EFI_ERROR_CODE + EFI_ERROR_UNRECOVERED,
                                       EFI_COMPUTING_UNIT_MEMORY + EFI_CU_MEMORY_EC_UNCORRECTABLE,
                                       0,
                                       NULL,
                                       NULL
                                       );
      return EFI_DEVICE_ERROR;
    } else {
      (*(UINT32*)(UINTN)TempAddress)  = 0;
    }
    TempAddress += SpanSize;
  }
#endif

  return EFI_SUCCESS;
}

/**
    Create Cpu Hob and initialize it.   

        
    @param PeiServices 

         
    @retval EFI_STATUS EFI_SUCCESS
                     
**/

EFI_STATUS CreateCPUHob(
  IN EFI_PEI_SERVICES **PeiServices
)
{
    EFI_HOB_CPU                 *pCpu = NULL;
    EFI_STATUS                  Status;
    UINT32                      RegEax;

    //
    // Refer Skylake Processor EDS - 2.3 System Address Map
    // The processor supports 512 GB (39 bits) of addressable memory space and 64 KB+3 of addressable I/O space.
    //
    
    Status = (*PeiServices)->CreateHob( (CONST EFI_PEI_SERVICES**)PeiServices, \
                                        EFI_HOB_TYPE_CPU, \
                                        sizeof(EFI_HOB_CPU), \
                                        (VOID **)&pCpu );
    if (Status == EFI_SUCCESS) {
      AsmCpuid (0x80000000, &RegEax, NULL, NULL, NULL);
      if (RegEax >= 0x80000008) {
        AsmCpuid (0x80000008, &RegEax, NULL, NULL, NULL);
        pCpu->SizeOfMemorySpace = (UINT8) RegEax;
      } else {
        pCpu->SizeOfMemorySpace = 36;
      }
      pCpu->SizeOfIoSpace = 16;
      MemSet(pCpu->Reserved, 6, 0);
    }

    return EFI_SUCCESS;
}

#ifdef SSA_FLAG
MrcStatus
AmiSsaCallbackPpi (
  struct _SSA_BIOS_CALLBACKS_PPI   *SsaBiosCallBacksPpi,
  MRC_OEM_STATUS_COMMAND           StatusCommand,
  VOID                             *CheckpointData)
{
    EFI_STATUS	                    Status;
    PEI_MEMORY_ERROR_REPORT_PPI     *MemoryErrorPpi;
    CONST EFI_PEI_SERVICES **PeiServices;
    
    PeiServices = GetPeiServicesTablePointer ();
    // if OEM have reinstall gAmiMemoryErrorReportPpi, it can Locate AmiMemoryError for OEM.
    Status = (*PeiServices)->LocatePpi(
                               (CONST EFI_PEI_SERVICES**)PeiServices, 
                               &gAmiMemoryErrorReportPpiGuid, 
                               0, 
                               NULL,
                               (VOID **)&MemoryErrorPpi);

    ASSERT_EFI_ERROR(Status);

    if (Status == EFI_SUCCESS)
        MemoryErrorPpi->AmiMemoryErrorRead (PeiServices, MemoryErrorPpi, (UINT32)StatusCommand);

    return StatusCommand;
}
#endif // #ifdef SSA_FLAG

/**
    This function reports the error status after memory training.    

             
    @param PeiServices PEI Services table pointer
    @param This Pointer to the PPI structure
    @param MemErrData Pointer to error data buffer

    @retval 
        EFI_STATUS
        EFI_SUCCESS     The function completed successfully.
**/
EFI_STATUS
EFIAPI
MemoryErrorRead (
  IN      EFI_PEI_SERVICES              **PeiServices,
  IN      PEI_MEMORY_ERROR_REPORT_PPI   *This,
  IN OUT  UINT32                        MemErrData
  )
{
    EFI_STATUS	                    Status = EFI_SUCCESS;
    
    switch  (MemErrData) {
        case MRC_MC_CONFIG_ERROR :           
        case MRC_MC_MEMORY_MAP_ERROR :
               PEI_ERROR_CODE(PeiServices, PEI_MEMORY_CONFIGURING, EFI_ERROR_MAJOR);
               break;
        case MRC_DIMM_RON_ERROR :
        case MRC_DIMM_ODT_ERROR :
        case MRC_WRITE_DS_ERROR :
        case MRC_WRITE_SR_ERROR :
        case MRC_WRITE_EQ_ERROR :
        case MRC_READ_ODT_ERROR :
        case MRC_READ_EQ_ERROR :
        case MRC_READ_AMP_POWER_ERROR :
        case MRC_CMP_OPT_ERROR :
        case MRC_PWR_MTR_ERROR :
        case MRC_SPD_PROCESSING_ERROR :
               PEI_ERROR_CODE(PeiServices, PEI_MEMORY_SPD_FAIL, EFI_ERROR_MAJOR);
               break;
        case MRC_RESET_ERROR :
               PEI_ERROR_CODE(PeiServices, PEI_MEMORY_INVALID_TYPE, EFI_ERROR_MAJOR);
               break;
        case MRC_PRE_TRAINING_ERROR :
        case MRC_EARLY_COMMAND_ERROR :
        case MRC_JEDEC_INIT_LPDDR3_ERROR :
        case MRC_SENSE_AMP_OFFSET_ERROR :
        case MRC_RECEIVE_ENABLE_ERROR :
        case MRC_JEDEC_WRITE_LEVELING_ERROR :
        case MRC_WRITE_TIMING_1D_ERROR :
        case MRC_WRITE_TIMING_2D_ERROR :
        case MRC_READ_TIMING_1D_ERROR :
        case MRC_READ_TIMING_2D_ERROR :
               PEI_ERROR_CODE(PeiServices, PEI_MEMORY_INVALID_SPEED, EFI_ERROR_MAJOR);
               break;
        case MRC_ECC_CLEAN_ERROR :
               PEI_ERROR_CODE(PeiServices, PEI_MEMORY_ERROR, EFI_ERROR_MAJOR);
               break;
        case MRC_NO_MEMORY_DETECTED :
               PEI_ERROR_CODE(PeiServices, PEI_MEMORY_NOT_DETECTED, EFI_ERROR_MAJOR);
               break;
        case MRC_MEM_INIT_DONE_WITH_ERRORS :
               PEI_ERROR_CODE(PeiServices, PEI_MEMORY_TEST, EFI_ERROR_MAJOR);
               break;
        case MRC_WRITE_VREF_2D_ERROR :
        case MRC_READ_VREF_2D_ERROR :
        case MRC_LATE_COMMAND_ERROR :
        case MRC_ROUND_TRIP_LAT_ERROR :
               PEI_ERROR_CODE(PeiServices, PEI_MEMORY_NONE_USEFUL, EFI_ERROR_MAJOR);
               break;
        case MRC_TURN_AROUND_ERROR :
        case MRC_SAVE_MC_VALUES_ERROR :
        case MRC_RMT_TOOL_ERROR :
        case MRC_CPGC_MEMORY_TEST_ERROR :
        case MRC_RESTORE_TRAINING_ERROR :
               PEI_ERROR_CODE(PeiServices, PEI_MEMORY_MISMATCH, EFI_ERROR_MAJOR);
               break;
        case MRC_SELF_REFRESH_EXIT_ERROR :
        case MRC_MRC_NORMAL_MODE_ERROR :
        case MRC_ALIAS_CHECK_ERROR :
        case MRC_POST_TRAINING_ERROR :
        case MRC_MC_ACTIVATE_ERROR :
        case MRC_DONE_WITH_ERROR :
// [ EIP411414 - Update for KBL RC v3.3.0 ] +>>>
#ifdef  MRC_FILL_RMT_STRUCTURE
        case MRC_FILL_RMT_STRUCTURE_ERROR :
               PEI_ERROR_CODE(PeiServices, PEI_MEMORY_NOT_INSTALLED, EFI_ERROR_MAJOR);
               break;
#endif  // #ifdef   MRC_FILL_RMT_STRUCTURE
#ifdef  MRC_FILL_BDAT_STRUCTURE_ERROR
        case MRC_FILL_BDAT_STRUCTURE_ERROR :
               PEI_ERROR_CODE(PeiServices, PEI_MEMORY_NOT_INSTALLED, EFI_ERROR_MAJOR);
               break;
#endif  // #ifdef   MRC_FILL_BDAT_STRUCTURE_ERROR
// [ EIP411414 - Update for KBL RC v3.3.0 ] +<<<
        default : 
               break;
    }

    return Status;
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
