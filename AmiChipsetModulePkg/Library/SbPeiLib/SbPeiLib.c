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
/** @file SbPeiLib.c
    This file contains code for Southbridge initialization library function
    in the PEI stage

**/

//-------------------------------------------------------------------------
// Include(s)
//-------------------------------------------------------------------------
#include <Efi.h>
#include <Pei.h>
#include <Hob.h>
#include <Token.h>
#include <Ppi/PciCfg2.h>
#include <Ppi/CpuIo.h>
#include <Ppi/SbPpi.h>
#include <Guid/Rtc.h>
#include <AmiCspLib.h>
#include <AmiPeiLib.h>
#include <SbSetupData.h>
#include <Library/GpioLib.h>
//#include <AmiSbGpio.h>
#include <PlatformBoardId.h>
#include <Library/PeiServicesLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/PchCycleDecodingLib.h>
#include <Library/AmiSbMiscLib.h>
#include <PchAccess.h>
#include <Ppi/SbChipsetPpi.h>
#if defined PROGRAM_GPIO_STYLE && PROGRAM_GPIO_STYLE == 0
#include <AmiGetBoardInfoLib.h>
#endif

#if PROGRAM_GPIO_STYLE == 1
#include <AmiGpioConfig.h>
#include <AmiGpioLib.h>
#include <AmiGpioPinsSklLp.h>
#include <AmiGpioPinsSklH.h>
#else
#include <AmiSbGpio.h>
#endif

//----------------------------------------------------------------------------
// GUID Definition(s)
//----------------------------------------------------------------------------

static EFI_GUID gRecoveryBootModeGuid       = EFI_PEI_BOOT_IN_RECOVERY_MODE_PEIM_PPI;

//----------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//----------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)

BOOLEAN IsRecovery (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_PCI_CFG2_PPI         *PciCfg,
    IN EFI_PEI_CPU_IO_PPI           *CpuIo
);

EFI_STATUS SbGetOemGpioTable(
    IN EFI_PEI_SERVICES             **PeiServices,
    OUT GPIO_INIT_CONFIG             **GpioTable,
    OUT UINTN                       *TableSize
);

EFI_STATUS 
EFIAPI
BoardInitAfterSbCustomPpiReady(
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi 
);

//----------------------------------------------------------------------------
// Variable and External Declaration(s)
//----------------------------------------------------------------------------
// Variable Declaration(s)
GPIO_INIT_CONFIG             *pTable = NULL;
static GPIO_INIT_CONFIG      AmiOemAdjustGpioList[] = {SB_OEM_ADJUST_GPIO_LIST {0xFFFFFFFF, {0, 0, 0, 0, 0, 0}}};
static AMI_SB_PCI_SSID_TABLE_STRUCT DefaultSIdTbl[] = {SB_PCI_DEVICES_SSID_TABLE};
static GPIO_GROUP       GpioGpeDwx[2][3] = {GPE_DWx_LP_LIST, GPE_DWx_H_LIST};

// Protocol Definition(s)

// External Declaration(s)

//----------------------------------------------------------------------------
// Installed PPI Definition(s)
//----------------------------------------------------------------------------

static EFI_PEI_PPI_DESCRIPTOR mBootModePpi[] = {
    { EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
      &gEfiPeiMasterBootModePpiGuid, NULL },
};

static EFI_PEI_PPI_DESCRIPTOR mRecoveryModePpi[] = {
    { EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
      &gRecoveryBootModeGuid, NULL },
};

//----------------------------------------------------------------------------
// Notified PPI Definition(s)
//----------------------------------------------------------------------------

static EFI_PEI_NOTIFY_DESCRIPTOR mGpioInitNotifyList[] = {
        { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | \
          EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
          &gAmiPeiSbCustomPpiGuid, BoardInitAfterSbCustomPpiReady },
};

/**
    This function determines if the system is resuming from an S3
    sleep state.

    @param PeiServices - Pointer to the Pei Services function and data
        structure.

    @retval TRUE It is an S3 Resume
    @retval FALSE It is not an S3 Resume
**/

BOOLEAN IsS3 (
  IN EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_CPU_IO_PPI   *CpuIo )
{
    UINT16        PmcBase;
    
    // PCH BWG section 19.12.1 S3 (Suspend to RAM) Steps
    // Check PWR_FLR Bit (D31:F2, Offset A4h, bit1)
    if ((READ_PCI8(SB_PMC_BUS, SB_PMC_DEV, SB_PMC_FUN, R_SB_PMC_GEN_PMCON_B) & B_SB_PMC_GEN_PMCON_B_PWR_FLR) == 0)
    {
        // Check PWROK_FLR bit (D31:F2, Offset A2h, bit0)
        //if(READ_PCI8(SB_PMC_BUS, SB_PMC_DEV, SB_PMC_FUN, R_SB_PMC_GEN_PMCON_B) & B_SB_PMC_GEN_PMCON_B_PWR_FLR) == 0)
        //{
            // Get PMC base first
            PchAcpiBaseGet(&PmcBase);
            
            // Check Power Button Override(PWRBTNOR_STS) bit (PMC ACPI IO Base Offset 0h, bit 11)
            if ((READ_IO16(PmcBase + R_SB_ACPI_PM1_STS) & B_SB_ACPI_PM1_STS_PRBTNOR) == 0)
            {
                // Check WAK_STS bit (PMC ACPI IO Base Offset 0h, bit 15)
                if ((READ_IO16(PmcBase + R_SB_ACPI_PM1_STS) & B_SB_ACPI_PM1_STS_WAK))
                {
                    // Check the sleep type
                    if ((READ_IO16(PmcBase + R_SB_ACPI_PM1_CNT) & B_SB_ACPI_PM1_CNT_SLP_TYP) == V_SB_ACPI_PM1_CNT_S3)
                        return TRUE;
                } // WAK_STS bit
            } // PWRBTNOR_STS bit
        //} // PWROK_FLR bit
    } // PWR_FLR Bit
    
    return FALSE;
}

/**
    This function determines if the system is resuming from an S4
    sleep state.

    @param PeiServices - Pointer to the Pei Services function and data
        structure.

    @retval TRUE It is an S4 Resume
    @retval FALSE It is not an S4 Resume
**/

BOOLEAN IsS4 (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN EFI_PEI_CPU_IO_PPI   *CpuIo )
{
    UINT16        PmcBase;
    
    // Get PMC base first
    PchAcpiBaseGet(&PmcBase);
    
    if((READ_IO16(PmcBase + R_SB_ACPI_PM1_CNT) & B_SB_ACPI_PM1_CNT_SLP_TYP) == V_SB_ACPI_PM1_CNT_S4)
        return TRUE;
    return FALSE;
}

/**
    This function determines CMOS data is available.

    @param PeiServices - Pointer to the Pei Services function and data
        structure.

    @retval TRUE CMOS data is bad
    @retval FALSE CMOS DATA is available
**/

BOOLEAN IsCmosBad (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN EFI_PEI_CPU_IO_PPI   *CpuIo )
{  
    UINT8               Nmi;

    ///
    /// Preserve NMI bet setting
    ///
    SET_MEM32(SB_PCR_ADDRESS(PID_ITSS, R_SB_PCR_ITSS_GIC), B_SB_PCR_ITSS_GIC_AME);    
    Nmi    = IoRead8 ((UINT64) CMOS_ADDR_PORT) & 0x80;
    RESET_MEM32(SB_PCR_ADDRESS(PID_ITSS, R_SB_PCR_ITSS_GIC), B_SB_PCR_ITSS_GIC_AME);

    IoWrite8 ((UINT64) CMOS_ADDR_PORT, CMOS_BAD_REG | Nmi);
    if (IoRead8 ((UINT64) CMOS_DATA_PORT) & (BIT6 + BIT7)) {
      return TRUE;
    } else {
      return FALSE;
    }
}

#if KBC_SUPPORT && Recovery_SUPPORT
/**
    This function resets Keyboard controller for Ctrl-Home
    recovery function.     

    @param PeiServices - Pointer to the Pei Services function and
        data structure
    @param CpuIo       - Pointer to the CPU I/O PPI
    @param PciCfg      - Pointer to the PCI Configuration PPI

    @retval VOID

    @note  No porting required.
**/

VOID ResetKbc (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN EFI_PEI_CPU_IO_PPI   *CpuIo,
    IN EFI_PEI_PCI_CFG2_PPI     *PciCfg
)
{
    volatile UINT8      KbcSts = 0;
    UINT32              TimeOut = 0x100;

    // Reset KBC
    if (CpuIo->IoRead8( (CONST EFI_PEI_SERVICES **)PeiServices, CpuIo, KBC_IO_STS ) != 0xff) {
        // Clear KBC buffer
        do {
            CpuIo->IoRead8( (CONST EFI_PEI_SERVICES **)PeiServices, (CONST EFI_PEI_CPU_IO_PPI *)CpuIo, (UINT64)KBC_IO_DATA );
            KbcSts = CpuIo->IoRead8( (CONST EFI_PEI_SERVICES **)PeiServices, (CONST EFI_PEI_CPU_IO_PPI *)CpuIo, (UINT64)KBC_IO_STS ); // 0x64
            TimeOut--;
        } while ((KbcSts & 3) && (TimeOut != 0));


        // Send BAT command 
        CpuIo->IoWrite8( (CONST EFI_PEI_SERVICES **)PeiServices, (CONST EFI_PEI_CPU_IO_PPI *)CpuIo, (UINT64)KBC_IO_STS, (UINT8)0xaa ); // 0x64

        // IBFree
        for (TimeOut = 0; TimeOut < 0x1000; TimeOut++) {
            CpuIo->IoWrite8( (CONST EFI_PEI_SERVICES **)PeiServices, (CONST EFI_PEI_CPU_IO_PPI *)CpuIo, (UINT64)IO_DELAY_PORT, (UINT8)KbcSts );
            KbcSts = CpuIo->IoRead8( (CONST EFI_PEI_SERVICES **)PeiServices, (CONST EFI_PEI_CPU_IO_PPI *)CpuIo, (UINT64)KBC_IO_STS ); // 0x64
            if ((KbcSts & 2) == 0) break;
        }

        // OBFree
        for (TimeOut = 0; TimeOut < 0x500; TimeOut++) {
            CpuIo->IoWrite8( (CONST EFI_PEI_SERVICES **)PeiServices, (CONST EFI_PEI_CPU_IO_PPI *)CpuIo, (UINT64)IO_DELAY_PORT, (UINT8)KbcSts );
            KbcSts = CpuIo->IoRead8( (CONST EFI_PEI_SERVICES **)PeiServices, (CONST EFI_PEI_CPU_IO_PPI *)CpuIo, (UINT64)KBC_IO_STS ); // 0x64
            if (KbcSts & 1) break;
        }

        // Get result if needed
        if (KbcSts & 1)
            CpuIo->IoRead8( (CONST EFI_PEI_SERVICES **)PeiServices, (CONST EFI_PEI_CPU_IO_PPI *)CpuIo, (UINT64)KBC_IO_DATA );
    }

    // Clear KBC status buffer.
    KbcSts = CpuIo->IoRead8 ( (CONST EFI_PEI_SERVICES **)PeiServices, (CONST EFI_PEI_CPU_IO_PPI *)CpuIo, (UINT64)KBC_IO_STS ); // 0x64
}
#endif

/**
    This function program the gpio setting before update the bootmode

    @param PeiServices - Pointer to the Pei Services function and
        data structure

    @retval VOID
**/
EFI_STATUS AmiAdjustOemGpioConfig (
    IN EFI_PEI_SERVICES     **PeiServices
)
{
    GPIO_INIT_CONFIG     *GpioData;
    AMI_SB_SERIES       SbSeries;
    BOOLEAN             Error;
    UINTN               AdjustGpioItems;
    UINTN               Index;
    UINT32              Group;
    
    DEBUG((DEBUG_INFO, "AmiSbPeiLib: AmiAdjustOemGpioConfig Entry\n"));
    SbSeries = AmiGetSbSeries();
    
    AdjustGpioItems = sizeof(AmiOemAdjustGpioList) / sizeof(GPIO_INIT_CONFIG) - 1;
    DEBUG((DEBUG_INFO, "AmiSbPeiLib: AdjustGpioItems: %x\n", AdjustGpioItems));
    
    for (Index = 0; Index < AdjustGpioItems; Index++){
        GpioData = &AmiOemAdjustGpioList[Index];
        Error = FALSE;

        Group = GpioData->GpioPad >> 16;  
        
        if (SbSeries == SbH){
            if ((Group & 0xFF00) != (H_Gpio_GPP_A & 0xFF00)) Error = TRUE;
            if ((Group & 0x00FF) > 9) Error = TRUE;
        } else {
            if ((Group & 0xFF00) != (Lp_Gpio_GPP_A & 0xFF00)) Error = TRUE;
            if ((Group & 0x00FF) > 7) Error = TRUE;
        }
        
        if (!Error){
            GpioSetPadConfig(GpioData->GpioPad, (GPIO_CONFIG *)&GpioData->GpioConfig);            
        } // end if
    } // end for loop
    
    DEBUG((DEBUG_INFO, "AmiSbPeiLib: AmiAdjustOemGpioConfig Exit\n"));
    
    return EFI_SUCCESS;
}

/**
    This function determines the boot mode of the system.
    After the correct boot mode has been determined, the PEI 
    Service function SetBootMode is called and then
    the MasterBootModePpi is installed

    @param PeiServices - Pointer to the Pei Services function and
        data structure
        CpuIo       - Pointer to the CPU I/O PPI
        PciCfg      - Pointer to the PCI Configuration PPI

    @retval Always returns EFI_SUCCESS
        Also defines the boot mode for the system
**/

EFI_STATUS AmiSbUpdateBootMode (
    IN EFI_PEI_SERVICES     **PeiServices
//    IN EFI_PEI_CPU_IO_PPI   *CpuIo,
//    IN EFI_PEI_PCI_CFG2_PPI     *PciCfg
)
{
    EFI_STATUS              Status;
    EFI_BOOT_MODE           BootMode;
    EFI_PEI_PCI_CFG2_PPI    *PciCfg;
    EFI_PEI_CPU_IO_PPI      *CpuIo;
    
    DEBUG((DEBUG_INFO, "AmiSbPeiLib: AmiSbUpdateBootMode Entry\n"));    
    
    PciCfg = (*PeiServices)->PciCfg;
    CpuIo  = (*PeiServices)->CpuIo;
    
    // Check for changes in the possible boot modes.  This should be made in
    // prioritized order.  At the end of this function the boot mode is
    // determined.  The EFI_BOOT_MODE is defined in the PEI Spec

    Status = (*PeiServices)->GetBootMode ( (CONST EFI_PEI_SERVICES **)PeiServices, &BootMode);
    if (EFI_ERROR(Status) || (BootMode != BOOT_IN_RECOVERY_MODE))
      BootMode = BOOT_WITH_FULL_CONFIGURATION;

    // Returns 0 if no S3 resume detected returns -1 if this is an S3 boot
    if (IsS3(PeiServices, CpuIo)) {
        BootMode = BOOT_ON_S3_RESUME;
        DEBUG((DEBUG_INFO, "Boot mode = BOOT_ON_S3_RESUME\n"));
    } else {
        // Check for S4 resume
        if (IsS4(PeiServices, CpuIo)) {
            BootMode = BOOT_ON_S4_RESUME;
            DEBUG((DEBUG_INFO, "Boot mode = BOOT_ON_S4_RESUME\n"));
        }

#if KBC_SUPPORT && FORCED_6064_DECODE_ENABLE
        Status = SbLibSetLpcDeviceDecoding(NULL,0x60, 0, dsPS2K);         // Decode SB Port 0x60,0x64
#endif

        // Check for recovery mode        
#if KBC_SUPPORT && Recovery_SUPPORT && PERFORM_KBC_RESET
            ResetKbc(PeiServices,  CpuIo, PciCfg);
#endif

        if (IsRecovery(PeiServices, PciCfg, CpuIo))
          BootMode = BOOT_IN_RECOVERY_MODE;
    }

    if (IsCmosBad(PeiServices, CpuIo)) {
        if (BootMode != BOOT_IN_RECOVERY_MODE) {
            BootMode = BOOT_WITH_DEFAULT_SETTINGS;
            DEBUG((DEBUG_INFO, "Boot mode = BOOT_WITH_DEFAULT_SETTING\n"));
        }

#if FORCE_USER_TO_SETUP_IF_CMOS_BAD
{
        EFI_STATUS          Status;
        UINT16              HobSize = sizeof(CMOS_BAD_HOB);
        EFI_GUID            CmosBadHobGuid = CMOS_BAD_HOB_GUID;
        CMOS_BAD_HOB        *CmosBadHob;

        Status = (*PeiServices)->CreateHob( (CONST EFI_PEI_SERVICES **)PeiServices,
                                            EFI_HOB_TYPE_GUID_EXTENSION,
                                            HobSize,
                                            (void **)&CmosBadHob);
        if(!EFI_ERROR(Status)) {
            CmosBadHob->Header.Name = CmosBadHobGuid;    
        }
}
#endif
    }
    // Set the system BootMode
    (*PeiServices)->SetBootMode( (CONST EFI_PEI_SERVICES **)PeiServices, BootMode);

    // Let everyone know that boot mode has been determined by installing the
    // MasterBootMode PPI
    (*PeiServices)->InstallPpi( (CONST EFI_PEI_SERVICES **)PeiServices, mBootModePpi );

    (*PeiServices)->GetBootMode ( (CONST EFI_PEI_SERVICES **)PeiServices, &BootMode); 

    if (BootMode == BOOT_IN_RECOVERY_MODE) { // Recovery Boot Mode PPI
            Status = (*PeiServices)->LocatePpi( (CONST EFI_PEI_SERVICES **)PeiServices, \
                                                &gRecoveryBootModeGuid, \
                                                0, \
                                                NULL, \
                                                NULL );
            if( EFI_ERROR(Status) ) {
                    (*PeiServices)->InstallPpi( (CONST EFI_PEI_SERVICES **)PeiServices, mRecoveryModePpi );
            }
    }

#if SYSTEM_REBOOT_NORMALLY_IF_S3_IS_FAILED
    if (BootMode == BOOT_ON_S3_RESUME) //S3 Boot Mode PPI
        WRITE_IO16_PM(R_PCH_ACPI_PM1_CNT, READ_IO16_PM(R_PCH_ACPI_PM1_CNT) & 0xe3ff ); // Clear S3 for avoiding S3 resume twice
#endif

    DEBUG((DEBUG_INFO, "AmiSbPeiLib: AmiSbUpdateBootMode Exit\n"));
    
    return EFI_SUCCESS;
}

/**
    This function detimines SB PCI devices

    @param UINT64 PciAddress
    @param UINT8  *PciSidReg

    @retval EFI_STATUS

**/
EFI_STATUS IsSBDevice(
    IN UINT64    PciAddress,
    IN OUT UINT8 *PciSidReg
)
{
    UINT8  i;
    AMI_SB_PCI_DEVICES_TABLE_STRUCT PchDeviceTable[] = {SB_LPC_MMIO_BUS_DEV_FUN,        R_SB_LPC_SVID,
                                                        SB_ISH_MMIO_BUS_DEV_FUN,        R_SB_ISH_SVID,
                                                        SB_XHCI_MMIO_BUS_DEV_FUN,       R_SB_XHCI_SVID,
                                                        SB_XDCI_MMIO_BUS_DEV_FUN,       R_SB_XDCI_SVID,
                                                        SB_THERMAL_MMIO_BUS_DEV_FUN,    R_SB_THERMAL_SVID,
                                                        SB_CIO2_MMIO_BUS_DEV_FUN,       R_SB_CIO2_SVID,
                                                        SB_SIO_I2C0_MMIO_BUS_DEV_FUN,   R_SB_SIO_I2C_SVID,
                                                        SB_SIO_I2C1_MMIO_BUS_DEV_FUN,   R_SB_SIO_I2C_SVID,
                                                        SB_SIO_I2C2_MMIO_BUS_DEV_FUN,   R_SB_SIO_I2C_SVID,
                                                        SB_SIO_I2C3_MMIO_BUS_DEV_FUN,   R_SB_SIO_I2C_SVID,
                                                        SB_SATA_MMIO_BUS_DEV_FUN,       R_SB_SATA_SVID,
                                                        SB_SIO_UART2_MMIO_BUS_DEV_FUN,  R_SB_SIO_UART_SVID,
                                                        SB_SIO_I2C5_MMIO_BUS_DEV_FUN,   R_SB_SIO_I2C_SVID,
                                                        SB_SIO_I2C4_MMIO_BUS_DEV_FUN,   R_SB_SIO_I2C_SVID,
                                                        SB_PCIERP16_MMIO_BUS_DEV_FUN,   R_SB_PCIE_SVID,
                                                        SB_PCIERP17_MMIO_BUS_DEV_FUN,   R_SB_PCIE_SVID,
                                                        SB_PCIERP18_MMIO_BUS_DEV_FUN,   R_SB_PCIE_SVID,
                                                        SB_PCIERP19_MMIO_BUS_DEV_FUN,   R_SB_PCIE_SVID,
                                                        SB_PCIERP20_MMIO_BUS_DEV_FUN,   R_SB_PCIE_SVID,
                                                        SB_PCIERP21_MMIO_BUS_DEV_FUN,   R_SB_PCIE_SVID,
                                                        SB_PCIERP22_MMIO_BUS_DEV_FUN,   R_SB_PCIE_SVID,
                                                        SB_PCIERP23_MMIO_BUS_DEV_FUN,   R_SB_PCIE_SVID,
                                                        SB_PCIERP00_MMIO_BUS_DEV_FUN,   R_SB_PCIE_SVID,
                                                        SB_PCIERP01_MMIO_BUS_DEV_FUN,   R_SB_PCIE_SVID,
                                                        SB_PCIERP02_MMIO_BUS_DEV_FUN,   R_SB_PCIE_SVID,
                                                        SB_PCIERP03_MMIO_BUS_DEV_FUN,   R_SB_PCIE_SVID,
                                                        SB_PCIERP04_MMIO_BUS_DEV_FUN,   R_SB_PCIE_SVID,
                                                        SB_PCIERP05_MMIO_BUS_DEV_FUN,   R_SB_PCIE_SVID,
                                                        SB_PCIERP06_MMIO_BUS_DEV_FUN,   R_SB_PCIE_SVID,
                                                        SB_PCIERP07_MMIO_BUS_DEV_FUN,   R_SB_PCIE_SVID,
                                                        SB_PCIERP08_MMIO_BUS_DEV_FUN,   R_SB_PCIE_SVID,
                                                        SB_PCIERP09_MMIO_BUS_DEV_FUN,   R_SB_PCIE_SVID,
                                                        SB_PCIERP10_MMIO_BUS_DEV_FUN,   R_SB_PCIE_SVID,
                                                        SB_PCIERP11_MMIO_BUS_DEV_FUN,   R_SB_PCIE_SVID,
                                                        SB_PCIERP12_MMIO_BUS_DEV_FUN,   R_SB_PCIE_SVID,
                                                        SB_PCIERP13_MMIO_BUS_DEV_FUN,   R_SB_PCIE_SVID,
                                                        SB_PCIERP14_MMIO_BUS_DEV_FUN,   R_SB_PCIE_SVID,
                                                        SB_PCIERP15_MMIO_BUS_DEV_FUN,   R_SB_PCIE_SVID,
                                                        SB_SIO_UART0_MMIO_BUS_DEV_FUN,  R_SB_SIO_UART_SVID,
                                                        SB_SIO_UART1_MMIO_BUS_DEV_FUN,  R_SB_SIO_UART_SVID,
                                                        SB_SIO_GSPI0_MMIO_BUS_DEV_FUN,  R_SB_SIO_GSPI_SVID,
                                                        SB_SIO_GSPI1_MMIO_BUS_DEV_FUN,  R_SB_SIO_GSPI_SVID,
                                                        SB_SCS_EMMC_MMIO_BUS_DEV_FUN,   R_SB_SCS_SVID,
                                                        SB_SCS_SDIO_MMIO_BUS_DEV_FUN,   R_SB_SCS_SVID,
                                                        SB_SCS_SDCARD_MMIO_BUS_DEV_FUN, R_SB_SCS_SVID,
                                                        SB_P2SB_MMIO_BUS_DEV_FUN,       R_SB_P2SB_SVID,
                                                        SB_PMC_MMIO_BUS_DEV_FUN,        R_SB_PMC_SVID,
                                                        SB_HDA_MMIO_BUS_DEV_FUN,        R_SB_HDA_SVID,
                                                        SB_SMBUS_MMIO_BUS_DEV_FUN,      R_SB_SMBUS_SVID,
                                                        SB_SPI_MMIO_BUS_DEV_FUN,        R_SB_SPI_SVID,
                                                        SB_LAN_MMIO_BUS_DEV_FUN,        R_SB_LAN_SVID
                                                       };
    UINT32 TableSize = sizeof(PchDeviceTable) / sizeof(AMI_SB_PCI_DEVICES_TABLE_STRUCT);

    for (i = 0; i < TableSize; i++) {

      if (PciAddress != PchDeviceTable[i].PciAddr) {
        continue;
      } else {
        if (MmioRead32((UINTN)PciAddress) == 0xffffffff) 
           return EFI_UNSUPPORTED;

        *PciSidReg = PchDeviceTable[i].PciSidReg;
        return EFI_SUCCESS;
      }
    }
    return EFI_UNSUPPORTED;
}

/**
    This function programs SB PCI devices sub-vendor ID and
    sub-system ID.

    @param PeiServices - Pointer to the PEI services table
        PciCfg      - Pointer to the PCI Configuration PPI

    @retval VOID

    @note  1. This routine only programs the PCI device in SB, hence, we
                 have to check the bus/device/function numbers whether they
                 are a SB PCI device or not.
              2. This routine is invoked by PEI phase.(After PEI permantent
                 memory be installed)
**/

VOID ProgramSbSubId (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN EFI_PEI_PCI_CFG2_PPI     *PciCfg
)
{
    EFI_STATUS                   Status = EFI_SUCCESS;
    UINTN                        i = 0;
    UINT32                       PciSid = 0xffffffff;
    UINT8                        PciSidReg = 0xff;
    AMI_SB_PCI_SSID_TABLE_STRUCT *SsidTblPtr = DefaultSIdTbl;
    
#if defined PROGRAM_GPIO_STYLE && PROGRAM_GPIO_STYLE == 2
    AMI_PEI_SB_CUSTOM_PPI        *SBPeiOemPpi = NULL;

    Status = (*PeiServices)->LocatePpi( (CONST EFI_PEI_SERVICES **)PeiServices, \
                                        &gAmiPeiSbCustomPpiGuid, \
                                        0, \
                                        NULL, \
                                        &SBPeiOemPpi );
    
    if ((SBPeiOemPpi != NULL) && (SBPeiOemPpi->SsidTable != NULL)){
        SsidTblPtr = SBPeiOemPpi->SsidTable;
    }
#endif
    DEBUG((DEBUG_INFO, "AMI: Prepare program SSID\n"));
    while (SsidTblPtr[i].PciAddr != 0xffffffffffffffff) {
        if (IsSBDevice(SsidTblPtr[i].PciAddr, &PciSidReg) == EFI_SUCCESS) {
            if (SsidTblPtr[i].Sid == 0xffffffff) {
                PciSid = MmioRead32((UINTN)(SsidTblPtr[i].PciAddr));
            } else {
                PciSid = SsidTblPtr[i].Sid;
            }
            
            MmioWrite32((UINTN)(SsidTblPtr[i].PciAddr + PciSidReg), PciSid);
        }
        i++;
    } // end while loop
}

/**
    This function detimines ME PCI devices

    @param UINT64 PciAddress
    @param UINT8  *PciSidReg

    @retval EFI_STATUS

**/
EFI_STATUS IsMEDevice(
    IN EFI_PEI_SERVICES  **PeiServices,
    IN UINT64            PciAddress,
    IN OUT UINT8         *PciSidReg
)
{
    UINT8  i;
#ifdef AMI_PCIE_DYNAMIC_SUPPORT_FLAG
    UINT32                      PciSid = 0xffffffff;
    EFI_STATUS                   Status = EFI_SUCCESS;
    EFI_PEI_PCI_CFG2_PPI        *PciCfg;
    AMI_SB_PCI_DEVICES_TABLE_STRUCT PchDeviceTable[] = {SB_HECI_BUS_DEV_FUN,       R_SB_ME_SVID,
                                                        SB_HECI2_BUS_DEV_FUN,      R_SB_ME_SVID,
                                                        SB_HECI3_BUS_DEV_FUN,      R_SB_ME_SVID,
                                                        SB_IDER_BUS_DEV_FUN,       R_SB_ME_SVID,
                                                        SB_KT_BUS_DEV_FUN,         R_SB_ME_SVID,};

    UINT32 TableSize = sizeof(PchDeviceTable) / sizeof(AMI_SB_PCI_DEVICES_TABLE_STRUCT);

    PciCfg = (*PeiServices)->PciCfg;

    for (i = 0; i < TableSize; i++) {

      if (PciAddress != PchDeviceTable[i].PciAddr) {
        continue;
      } else {
        Status = PciCfg->Read( (CONST EFI_PEI_SERVICES **)PeiServices,
                               PciCfg,
                               EfiPeiPciCfgWidthUint32,
                               PchDeviceTable[i].PciAddr,
                               &PciSid);

        if (PciSid == 0xffffffff) 
           return EFI_UNSUPPORTED;

        *PciSidReg = PchDeviceTable[i].PciSidReg;
        return EFI_SUCCESS;
      }
    }
    
#else
    AMI_SB_PCI_DEVICES_TABLE_STRUCT PchDeviceTable[] = {SB_HECI_MMIO_BUS_DEV_FUN,       R_SB_ME_SVID,
                                                        SB_HECI2_MMIO_BUS_DEV_FUN,      R_SB_ME_SVID,
                                                        SB_HECI3_MMIO_BUS_DEV_FUN,      R_SB_ME_SVID,
                                                        SB_IDER_MMIO_BUS_DEV_FUN,       R_SB_ME_SVID,
                                                        SB_KT_MMIO_BUS_DEV_FUN,         R_SB_ME_SVID,};
    
    UINT32 TableSize = sizeof(PchDeviceTable) / sizeof(AMI_SB_PCI_DEVICES_TABLE_STRUCT);

    for (i = 0; i < TableSize; i++) {

      if (PciAddress != PchDeviceTable[i].PciAddr) {
        continue;
      } else {
        if (MmioRead32((UINTN)PciAddress) == 0xffffffff) 
           return EFI_UNSUPPORTED;

        *PciSidReg = PchDeviceTable[i].PciSidReg;
        return EFI_SUCCESS;
      }
    }
#endif
    return EFI_UNSUPPORTED;
}

/**
    This function programs ME PCI devices sub-vendor ID and
    sub-system ID.

    @param PeiServices - Pointer to the PEI services table
        PciCfg      - Pointer to the PCI Configuration PPI

    @retval VOID

    @note  1. This routine only programs the PCI device in SB, hence, we
                 have to check the bus/device/function numbers whether they
                 are a SB PCI device or not.
              2. This routine is invoked by PEI phase.(After PEI permantent
                 memory be installed)
**/
VOID ProgramMeSubId (
  IN EFI_PEI_SERVICES         **PeiServices,
  IN EFI_PEI_PCI_CFG2_PPI     *PciCfg        
  )
{
    EFI_STATUS                   Status = EFI_SUCCESS;
    UINTN                        i = 0;
    UINT32                       PciSid = 0xffffffff;
    UINT8                        PciSidReg = 0xff;
    AMI_SB_PCI_SSID_TABLE_STRUCT *SsidTblPtr = DefaultSIdTbl; 

#if defined PROGRAM_GPIO_STYLE && PROGRAM_GPIO_STYLE == 2
    AMI_PEI_SB_CUSTOM_PPI        *SBPeiOemPpi = NULL;

    Status = (*PeiServices)->LocatePpi( (CONST EFI_PEI_SERVICES **)PeiServices, \
                                        &gAmiPeiSbCustomPpiGuid, \
                                        0, \
                                        NULL, \
                                        &SBPeiOemPpi );
    
    if ((SBPeiOemPpi != NULL) && (SBPeiOemPpi->SsidTable != NULL)){
        SsidTblPtr = SBPeiOemPpi->SsidTable;
    }
#endif    
    DEBUG((DEBUG_INFO, "AMI: Prepare program ME SSID\n"));
    while (SsidTblPtr[i].PciAddr != 0xffffffffffffffff) {
        if (IsMEDevice(PeiServices, SsidTblPtr[i].PciAddr, &PciSidReg) == EFI_SUCCESS) {
#ifdef AMI_PCIE_DYNAMIC_SUPPORT_FLAG
            if (SsidTblPtr[i].Sid == 0xffffffff) {
                Status = PciCfg->Read( (CONST EFI_PEI_SERVICES **)PeiServices,
                                       PciCfg,
                                       EfiPeiPciCfgWidthUint32,
                                       SsidTblPtr[i].PciAddr,
                                       &PciSid);

            } else {
                PciSid = SsidTblPtr[i].Sid;
            }

            Status = PciCfg->Write( (CONST EFI_PEI_SERVICES **)PeiServices,
                                    PciCfg,
                                    EfiPeiPciCfgWidthUint32,
                                    SsidTblPtr[i].PciAddr | PciSidReg,
                                    &PciSid);
#else
            if (SsidTblPtr[i].Sid == 0xffffffff) {
                PciSid = MmioRead32((UINTN)(SsidTblPtr[i].PciAddr));
            } else {
                PciSid = SsidTblPtr[i].Sid;
            }
            
            MmioWrite32((UINTN)(SsidTblPtr[i].PciAddr + PciSidReg), PciSid);
#endif
        }
        i++;
    } // end while loop
}

/**
  Configure GPIO group GPE tier.

  @param EFI_PEI_SERVICES             **PeiServices

  @retval     none.
**/
VOID AmiGpioGroupTierInit(
    IN EFI_PEI_SERVICES             **PeiServices)
{
    AMI_SB_SERIES               SbSeries = AmiGetSbSeries();
    
    DEBUG((DEBUG_INFO, "AmiSbPeiLib: AmiGpioGroupTierInit Entry\n"));
    
    if(SbSeries == SbLp){
        GpioSetGroupToGpeDwX(GpioGpeDwx[0][0], GpioGpeDwx[0][1], GpioGpeDwx[0][2]);
    } else {
        GpioSetGroupToGpeDwX(GpioGpeDwx[1][0], GpioGpeDwx[1][1], GpioGpeDwx[1][2]);
    }
    
    DEBUG((DEBUG_INFO, "AmiSbPeiLib: AmiGpioGroupTierInit Exit\n"));
}

/**
    This function initializes SB GPIOs via Intel GPIO Library

    @param PeiServices - Pointer to the PEI services table
    @param CpuIo       - Pointer to the CPU I/O PPI

    @retval VOID
**/

VOID ProgramGPIO (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN GPIO_INIT_CONFIG             *pTable,
    IN UINTN                        Tablesize
)
{
#if !defined(MDEPKG_NDEBUG)
    EFI_STATUS                  Status;
#endif
    UINT16                      GpioTableCount;
    UINT32               		GroupIndex;
    
    GpioTableCount = (UINT16)(Tablesize / sizeof(GPIO_INIT_CONFIG));
    DEBUG((DEBUG_INFO, "Gpio Table Point is 0x%x Size is 0x%x\n", pTable, Tablesize));
    
    // Only check the last GPIO
    GroupIndex = ((((GPIO_INIT_CONFIG *)pTable)[GpioTableCount - 1].GpioPad >> 16) & 0xFF);    
    if (GroupIndex >= V_PCH_GPIO_GROUP_MAX) {
        DEBUG((DEBUG_INFO, "Skip last GPIO ib GpioTable\n"));
        GpioTableCount--;
    }
    
    
    // Using Intel GPIO Library to configure GPIO pin
#if defined(MDEPKG_NDEBUG)
    GpioConfigurePads (GpioTableCount, (GPIO_INIT_CONFIG *)pTable);
#elif !defined(MDEPKG_NDEBUG)
    Status = GpioConfigurePads (GpioTableCount, (GPIO_INIT_CONFIG *)pTable);
    DEBUG((DEBUG_INFO, "AmiSbPeiLib: ProgramGPIO %r\n", Status));
#endif
}

/**
    Wait for SbCustomPpi ready then doing programming GPIO

    @param EFI_PEI_SERVICES             **PeiServices
    @param EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor
    @param VOID                         *InvokePpi

    @retval EFI_STATUS                   
**/
EFI_STATUS 
EFIAPI
BoardInitAfterSbCustomPpiReady(
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi 
)
{
    AMI_PEI_SB_CUSTOM_PPI       *SbPeiOemPpi;
    GPIO_INIT_CONFIG            *pTable       = NULL;
    UINTN                       GpioTableSize = 0;
    
    SbPeiOemPpi = (AMI_PEI_SB_CUSTOM_PPI *) InvokePpi;
    
    DEBUG((DEBUG_INFO, "AmiSbPeiLib: BoardInitAfterSbCustomPpiReady Entry\n"));
    
    if (SbPeiOemPpi->GpioInit != NULL) {        
        pTable = SbPeiOemPpi->GpioInit->GpioTable;
        GpioTableSize = SbPeiOemPpi->GpioInit->TableSize;
        
        // Start programming GPIO for OEM
        ProgramGPIO( PeiServices, pTable, GpioTableSize);
    } else DEBUG((DEBUG_WARN, "Warning !!! Cannot find OEM GPIO table !!!\n"));
    
    AmiGpioGroupTierInit(PeiServices);
    
    return EFI_SUCCESS;
}
/**
    This function initializes SB GPIOs

    @param PeiServices - Pointer to the PEI services table

    @retval VOID
**/

EFI_STATUS AmiBoardInit (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN UINT16               BoardId)
{
    EFI_STATUS                  Status;
    GPIO_INIT_CONFIG            *pTable = NULL;
    GPIO_INIT_CONFIG            *pTable2 = NULL;
    UINTN                       GpioTableSize = 0;
    UINTN                       GpioTable2Size = 0;
    UINT8                       ProgramFlag = 0;
    UINT16                      BoardIdValue = 0;
    UINT16                      BoardIdOrgValue = 0;
#if defined PROGRAM_GPIO_STYLE && PROGRAM_GPIO_STYLE == 2
    AMI_PEI_SB_CUSTOM_PPI       *SBPeiOemPpi = NULL;
#endif
    
    DEBUG((DEBUG_INFO, "AmiSbPeiLib: AmiBoardInit Entry\n"));
            
    // Get GPIO table and table size
#if defined PROGRAM_GPIO_STYLE && PROGRAM_GPIO_STYLE == 0
    DEBUG((DEBUG_INFO, "AmiSbPeiLib: Program Gpio by using AmiSbGpio.h directly...\n"));
    DEBUG((DEBUG_INFO, "AmiSbPeiLib: BoardId = 0x%x\n", BoardId));

    Status = AmiGetBoardInfo (&BoardIdValue, &BoardIdOrgValue);
    if(EFI_ERROR(Status))
    {
        ProgramFlag = 0;
        ASSERT_EFI_ERROR (Status);
    }
    else
    {
        ProgramFlag = 1;
    }
    switch (BoardId) {
#if CRB_BOARD == 0 // RVP3
        case BoardIdSkylakeA0Rvp3: // CRB_BOARD = 0
            pTable = mGpioTableLpDdr3Rvp3;
            GpioTableSize = sizeof(mGpioTableLpDdr3Rvp3);
            break;
#elif CRB_BOARD == 1 // RVP5
        case BoardIdSkylakeULpddr3Rvp5: // CRB_BOARD = 1
            pTable = mGpioTableLpddr3Rvp5;
            GpioTableSize = sizeof(mGpioTableLpddr3Rvp5);
            break;
#elif CRB_BOARD == 2 || CRB_BOARD == 8 || CRB_BOARD == 11 // RVP7  // RVP15 // Kaby Lake Refresh U
        case BoardIdSkylakeURvp7: // CRB_BOARD = 2
        case BoardIdSkylakeURvp15: // CRB_BOARD = 8
            pTable = mGpioTableDdr3LrRvp7Common;
            GpioTableSize = sizeof(mGpioTableDdr3LrRvp7Common);
            pTable2 = mGpioTableDdr3LrRvp7;
            GpioTable2Size = sizeof(mGpioTableDdr3LrRvp7);
            break;
        case BoardIdKabylakeRDdr4:
            pTable = mGpioTableDdr3LrRvp7Common;
            GpioTableSize = sizeof(mGpioTableDdr3LrRvp7Common);
            pTable2 = mGpioTableBoardIdKabylakeRDdr4;
            GpioTable2Size = sizeof(mGpioTableBoardIdKabylakeRDdr4);
            break;
#elif CRB_BOARD == 3 // RVP8 
        case BoardIdSkylakeDtRvp8Crb: // CRB_BOARD = 3
            pTable = mGpioTableDtRvp8Crb;
            GpioTableSize = sizeof(mGpioTableDtRvp8Crb);
            break;
#elif CRB_BOARD == 9 // Kaby Lake S uDIMM RVP
        case BoardIdSkylakeDtRvp8Crb: // Because KabyLake S CRB Board ID override by Skylake AIO Rvp8 Board ID
            if (BoardIdOrgValue == BoardIdKabyLakeSDdr4UdimmCrb) {
                pTable = mGpioTableAioRvp9Crb;
                GpioTableSize = sizeof(mGpioTableAioRvp9Crb);
            }
            break;
#elif CRB_BOARD == 4 // RVP9
        case BoardIdSkylakeAioRvp9Crb: // CRB_BOARD = 4
            pTable = mGpioTableAioRvp9Crb;
            GpioTableSize = sizeof(mGpioTableAioRvp9Crb);
            break;
#elif CRB_BOARD == 5 // RVP10
        case BoardIdSkylakeAioRvp10Crb: // CRB_BOARD = 5
            pTable = mGpioTableAioRvp10Crb;
            GpioTableSize = sizeof(mGpioTableAioRvp10Crb);
            break;
#elif CRB_BOARD == 6 // ZumbaBeach
        case BoardIdZumbaBeachServerEv: // CRB_BOARD = 6
            pTable = upZumbaBeachGpioDefinitionTable;
            GpioTableSize = sizeof(upZumbaBeachGpioDefinitionTable);
            break;
#elif CRB_BOARD == 7 // RVP11
        case BoardIdSkylakeHaloDdr4Rvp11: // CRB_BOARD = 7
            pTable = mGpioTableHaloRvp11;
            GpioTableSize = sizeof(mGpioTableHaloRvp11);
            break;
#elif CRB_BOARD == 10 // SaddleBrooke        		
        case BoardIdSkylakeHaloDdr4Rvp11: // CRB_BOARD = 10
            pTable = mGpioTableHaloSdlBrk;
            GpioTableSize = sizeof(mGpioTableHaloSdlBrk);
            break;
#elif CRB_BOARD == 12 // KBL-G CRB               
        case BoardIdKabylakeDdr4Rvp17: // CRB_BOARD = 12
            pTable = mGpioTableKblRvp17;
            GpioTableSize = sizeof(mGpioTableKblRvp17);
            break;            
#endif
        default:
            ProgramFlag = 0;
            DEBUG((DEBUG_INFO, "AmiSbPeiLib: Please fill in the existing value of CRB_BOARD in SDL file.\n"));
            ASSERT_EFI_ERROR (EFI_INVALID_PARAMETER);
    }

#elif defined PROGRAM_GPIO_STYLE && PROGRAM_GPIO_STYLE == 1
    DEBUG((DEBUG_INFO, "AmiSbPeiLib: Program Gpio by using OEM Library function...\n"));
    //for (i=0; AmiGetOemGpioTableList[i] != NULL; i++)
    //{
        Status = SbGetOemGpioTable(PeiServices, &pTable, &GpioTableSize);
    //    if(Status == EFI_SUCCESS) break;
    //}
    
    if (EFI_ERROR(Status)){
        DEBUG((DEBUG_ERROR, "AmiSbPeiLib: Skip Programming Gpio because cannot get GPIO table from OEM Elink...\n"));
    } else if (GpioTableSize == 0){
        DEBUG((DEBUG_INFO, "AmiSbPeiLib: Skip Programming Gpio because Table size is zero...\n"));
    } else {
        DEBUG((DEBUG_INFO, "AmiSbPeiLib: Table Point:%x, Table Size:0x%x\n", pTable, GpioTableSize));
        ProgramFlag = 1;
    }
#elif defined PROGRAM_GPIO_STYLE && PROGRAM_GPIO_STYLE == 2
    DEBUG((DEBUG_INFO, "AmiSbPeiLib: Program Gpio by using SbCustomPpi...\n"));
    // Program the OEM GPIO Setting for board.
    Status = (*PeiServices)->LocatePpi( (CONST EFI_PEI_SERVICES **)PeiServices, \
                                        &gAmiPeiSbCustomPpiGuid, \
                                        0, \
                                        NULL, \
                                        &SBPeiOemPpi );
    
    if(EFI_ERROR(Status)){
        // Can not get sb customer ppi, create notify event for OEM programming GPIO
        Status = (*PeiServices)->NotifyPpi( (CONST EFI_PEI_SERVICES **)PeiServices, mGpioInitNotifyList );
        DEBUG((DEBUG_ERROR, "AmiSbPeiLib: create notify event to program GPIO for OEM %r\n", Status));
    } else {
        if (SBPeiOemPpi->GpioInit != NULL) {
            pTable = SBPeiOemPpi->GpioInit->GpioTable;
            GpioTableSize = SBPeiOemPpi->GpioInit->TableSize;
            ProgramFlag = 1;            
            AmiGpioGroupTierInit(PeiServices);
        } else DEBUG((DEBUG_WARN, "Warning !!! Cannot find OEM GPIO table !!!\n"));
    }
#endif
    
    if (ProgramFlag == 1){
        ProgramGPIO( PeiServices, pTable, GpioTableSize);
        if (pTable2 != NULL) {
            ProgramGPIO( PeiServices, pTable2, GpioTable2Size);
        }
    }
    
#if defined PROGRAM_GPIO_STYLE && PROGRAM_GPIO_STYLE != 2
    AmiGpioGroupTierInit(PeiServices);
#endif
    
    DEBUG((DEBUG_INFO, "AmiSbPeiLib: AmiBoardInit Exit\n"));
    
    return EFI_SUCCESS;
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
