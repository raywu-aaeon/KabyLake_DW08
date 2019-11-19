//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file SbRunSmm.c
    This file contains code for SouthBridge runtime SMM
    protocol

**/

// Module specific Includes
#include "Efi.h"
#include "Token.h"
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <Library/AmiChipsetRuntimeServiceLib.h>
#include <Library/AmiSbMiscLib.h>
#include <Protocol/Reset.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Library/S3BootScriptLib.h>
#include <PchAccess.h>
#include <SetupVariable.h>

BOOT_SCRIPT_SB_PCI_REG_SAVE gSataRegistersSave[] = {
  R_SB_SATA_MAP            , EfiBootScriptWidthUint8,  0xffffffff, // 0x90
  R_SB_SATA_PI             , EfiBootScriptWidthUint8,  0xffffffff, // 0x09
  R_SB_SATA_INTR_LN        , EfiBootScriptWidthUint8,  0xffffffff, // 0x3c
  R_PCH_SATA_AHCI_INTLN    , EfiBootScriptWidthUint32, 0xffffffff, // 0x40
  R_SB_SATA_PMCS           , EfiBootScriptWidthUint16, 0xffffffff, // 0x74
  R_SB_LP_SATA_PCS         , EfiBootScriptWidthUint16, 0xffffffff, // 0x92
  R_SB_SATA_PCMD_BAR       , EfiBootScriptWidthUint32, 0xffffffff, // 0x10
  R_SB_SATA_PCNL_BAR       , EfiBootScriptWidthUint32, 0xffffffff, // 0x14
  R_SB_SATA_SCMD_BAR       , EfiBootScriptWidthUint32, 0xffffffff, // 0x18
  R_SB_SATA_SCNL_BAR       , EfiBootScriptWidthUint32, 0xffffffff, // 0x1c
  R_SB_SATA_BM_BASE        , EfiBootScriptWidthUint32, 0xffffffff, // 0x20
  R_SB_SATA_ABAR           , EfiBootScriptWidthUint32, 0xffffffff, // 0x24
  R_SB_SATA_PCICMD         , EfiBootScriptWidthUint8,  0xffffffff, // 0x04
};

EFI_STATUS SaveSbSataScriptTable(VOID)
{
    UINT32                          Data32;
    UINT16                          Data16;
    UINT8                           Data8;
    UINT8                           i;
    UINT64                          Address;

        for (i = 0; i < sizeof(gSataRegistersSave)/ sizeof(BOOT_SCRIPT_SB_PCI_REG_SAVE); i++) {

            Address = SB_SATA_MMIO_REG(gSataRegistersSave[i].Address);

            if (gSataRegistersSave[i].Width == EfiBootScriptWidthUint8) {
                Data8 = MmioRead8(Address);
                S3BootScriptSaveMemWrite(
                        gSataRegistersSave[i].Width,
                        Address,
                        1,
                        &Data8);
            } else if(gSataRegistersSave[i].Width == EfiBootScriptWidthUint16) {
                Data16 = MmioRead16(Address);
                S3BootScriptSaveMemWrite(
                        gSataRegistersSave[i].Width,
                        Address,
                        1,
                        &Data16);
            } else {
                Data32 = MmioRead32(Address);
                S3BootScriptSaveMemWrite(
                        gSataRegistersSave[i].Width,
                        Address,
                        1,
                        &Data32);
            }
        } // end for loop

    return EFI_SUCCESS;
}

#if defined (PCH_AN_ODD_DETECT_SUPPORT) && PCH_AN_ODD_DETECT_SUPPORT == 1
VOID SaveSbAnOddScriptTable(VOID)
{
    AMI_SB_SERIES                   Sb = AmiGetSbSeries();
    UINT8                           SaveIndex8;
    UINT32                          SaveData32;
    
    if (Sb == SbH) {
      MmioWrite8 (SB_SATA_MMIO_REG(R_SB_SATA_SIRI), R_PCH_SATA_SIR_90);
      SaveData32 = MmioRead32(SB_SATA_MMIO_REG(R_SB_SATA_STRD));
      SaveIndex8 = R_PCH_SATA_SIR_90;
      S3BootScriptSaveMemWrite(
                        EfiBootScriptWidthUint32,
                        SB_SATA_MMIO_REG(R_SB_SATA_SIRI),
                        1,
                        &SaveIndex8); 
      S3BootScriptSaveMemWrite(
                        EfiBootScriptWidthUint32,
                        SB_SATA_MMIO_REG(R_SB_SATA_STRD),
                        1,
                        &SaveData32);

      MmioWrite8 (SB_SATA_MMIO_REG(R_SB_SATA_SIRI), R_PCH_SATA_SIR_80);
      SaveData32 = MmioRead32(SB_SATA_MMIO_REG(R_SB_SATA_STRD));
      SaveIndex8 = R_PCH_SATA_SIR_80;
      S3BootScriptSaveMemWrite(
                        EfiBootScriptWidthUint32,
                        SB_SATA_MMIO_REG(R_SB_SATA_SIRI),
                        1,
                        &SaveIndex8); 
      S3BootScriptSaveMemWrite(
                        EfiBootScriptWidthUint32,
                        SB_SATA_MMIO_REG(R_SB_SATA_STRD),
                        1,
                        &SaveData32);
      
    } else if (Sb == SbLp) {
      MmioWrite8 (SB_SATA_MMIO_REG(R_SB_SATA_SIRI), R_PCH_SATA_SIR_50);
      SaveData32 = MmioRead32(SB_SATA_MMIO_REG(R_SB_SATA_STRD));
      SaveIndex8 = R_PCH_SATA_SIR_50;
      S3BootScriptSaveMemWrite(
                        EfiBootScriptWidthUint32,
                        SB_SATA_MMIO_REG(R_SB_SATA_SIRI),
                        1,
                        &SaveIndex8); 
      S3BootScriptSaveMemWrite(
                        EfiBootScriptWidthUint32,
                        SB_SATA_MMIO_REG(R_SB_SATA_STRD),
                        1,
                        &SaveData32);

      MmioWrite8 (SB_SATA_MMIO_REG(R_SB_SATA_SIRI), R_PCH_SATA_SIR_54);
      SaveData32 = MmioRead32(SB_SATA_MMIO_REG(R_SB_SATA_STRD));
      SaveIndex8 = R_PCH_SATA_SIR_54;
      S3BootScriptSaveMemWrite(
                        EfiBootScriptWidthUint32,
                        SB_SATA_MMIO_REG(R_SB_SATA_SIRI),
                        1,
                        &SaveIndex8); 
      S3BootScriptSaveMemWrite(
                        EfiBootScriptWidthUint32,
                        SB_SATA_MMIO_REG(R_SB_SATA_STRD),
                        1,
                        &SaveData32);
    }
}
#endif

/**
  This function save Sb script table in ReadToBoot event
  
  @param DispatchHandle  - SMI dispatcher handle
  @param *DispatchContext- Points to an optional S/W SMI context
  @param CommBuffer      - Points to the optional communication
      buffer
  @param CommBufferSize  - Points to the size of the optional
      communication buffer
  
  @retval EFI_STATUS if the new SMM PI is applied.

**/

EFI_STATUS 
EFIAPI
SaveSbScriptTable (
  IN EFI_HANDLE       DispatchHandle,
  IN CONST VOID       *DispatchContext OPTIONAL,
  IN OUT VOID         *CommBuffer OPTIONAL,
  IN OUT UINTN        *CommBufferSize OPTIONAL 
  )
{
  UINT8       Data8;
  UINT32      i, Data32;   
  UINT32      SpiRegister[] = { R_SB_SPI_SSFSTS,
                                R_SB_SPI_POOT,
                                R_SB_SPI_OPMENU0,
                                R_SB_SPI_OPMENU1,
                                R_SB_SPI_SFDP0_LVSCC,
                                R_SB_SPI_SFDP1_UVSCC
                              };
  UINT32      SmiEn;
  UINT16      Pm1Sts;
  UINTN       SpiBar0;
  AMI_SB_SERIES       Sb = AmiGetSbSeries();
  UINT16              DevPsfBase = 0;
  EFI_STATUS          Status;
  ME_SETUP            MeSetup;
  UINTN               Size;

  // Most of SATA registers are initialized in PchSata.c and write bootscript in ConfigurePchSataOnEndOfDxe().
  // So skip saving Sata and ABAR S3 reg.
  SaveSbSataScriptTable();

#if defined (PCH_AN_ODD_DETECT_SUPPORT) && PCH_AN_ODD_DETECT_SUPPORT == 1
  SaveSbAnOddScriptTable();
#endif
  
  //
  // Save SPI Registers for S3 resume usage
  //
  SpiBar0 = MMIO_READ32(SB_SPI_MMIO_REG(R_SB_SPI_BAR0)) & (~B_SB_SPI_BAR0_MASK);
  DEBUG ((DEBUG_INFO, "Prepare save SPI register for S3 resume usage... SPIBar:%x\n", SpiBar0));  
    
  for (i = 0; i < (sizeof (SpiRegister)/sizeof (UINT32)); i++) {
    Data32 = MMIO_READ32(SpiBar0 + SpiRegister[i]);
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      (UINTN) (SpiBar0 + SpiRegister[i]),
      1,
      &Data32
      );    
  }
  
  DEBUG ((DEBUG_INFO, "Prepare save ACPI IO base register for S3 resume usage...\n"));    
  // [EIP120623]>
  Data8 = IoRead8 (ACPI_BASE_ADDRESS + R_SB_ACPI_PM1_CNT);   //0x04
  Data8 |= B_SB_ACPI_PM1_CNT_SCI_EN;
  S3BootScriptSaveIoWrite (
    S3BootScriptWidthUint8,
    (UINTN) (ACPI_BASE_ADDRESS + R_SB_ACPI_PM1_CNT),
    1,
    &Data8
    );     
  // <[EIP120623]
  
  SmiEn = IoRead32 (ACPI_BASE_ADDRESS + R_SB_SMI_EN);   //0x30        
#if DIS_SW_SMI_TIMER_BEFORE_OS  
  SmiEn &= ~B_SB_SMI_EN_SWSMI_TMR;
#endif
#if defined EMUL6064_SUPPORT && EMUL6064_SUPPORT == 1
  SmiEn &= ~B_SB_SMI_EN_LEGACY_USB;
#endif
  S3BootScriptSaveIoWrite (
    S3BootScriptWidthUint32,
    (UINTN) (ACPI_BASE_ADDRESS + R_SB_SMI_EN),
    1,
    &SmiEn
    );     
  
  // Clear bus master status bit on S3 resume
  Pm1Sts = B_SB_ACPI_PM1_STS_BM; 
  S3BootScriptSaveIoWrite (
    S3BootScriptWidthUint16,
    (UINTN) (ACPI_BASE_ADDRESS + R_PCH_ACPI_PM1_STS),
    1,
    &Pm1Sts
    );

  Size = sizeof (ME_SETUP);
  Status = pRS->GetVariable(L"MeSetup", &gMeSetupVariableGuid, NULL, &Size, &MeSetup);
  DEBUG ((DEBUG_INFO, "Get MeSetup Status : %r\n", Status));    

  if ((MeSetup.Amt == 0) && (!EFI_ERROR (Status))) {
    if (Sb == SbH) {
      DevPsfBase = R_PCH_H_PCR_PSF1_T0_SHDW_KT_REG_BASE;
    } else if (Sb == SbLp) {
      DevPsfBase = R_PCH_LP_PCR_PSF1_T0_SHDW_KT_REG_BASE;
    }
  
    Data32 = MmioRead32 (PCH_PCR_ADDRESS(PID_PSF1, DevPsfBase + R_PCH_PCR_PSFX_T0_SHDW_PCIEN));
    S3BootScriptSaveMemWrite(
      EfiBootScriptWidthUint32,
      PCH_PCR_ADDRESS(PID_PSF1, DevPsfBase + R_PCH_PCR_PSFX_T0_SHDW_PCIEN),
      1,
      &Data32
      );
  }
  
#if SB_SWSMI_WRITE_TO_BOOTSCRIPT
  // Generate an ACPI Enable SMI when S3 resuming.
  Data8 = SW_SMI_ACPI_ENABLE;
  
  S3BootScriptSaveIoWrite (
    S3BootScriptWidthUint8,
    (UINTN) SW_SMI_IO_ADDRESS,
    1,
    &Data8
    );  
#endif

  return EFI_SUCCESS;
}

/**
  Submit runtime services both SMM and runtime.
  
  @param ImageHandle Image handle
  @param SystemTable Pointer to the system table
  
  @retval Return Status based on errors that occurred while waiting for
      time to expire.

**/
EFI_STATUS
SbRuntimeServiceInit (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS                      Status;
  EFI_SMM_SW_DISPATCH2_PROTOCOL   *SwDispatch = NULL;
  EFI_HANDLE                      Handle = NULL;
  EFI_SMM_SW_REGISTER_CONTEXT     SwContext = {SW_SMI_SCRIPT_TABLE_SAVE};    
  
  pRS->ResetSystem    = (EFI_RESET_SYSTEM) AmiChipsetResetSystem;
  pRS->GetTime        = AmiChipsetGetTime;
  pRS->SetTime        = AmiChipsetSetTime;
  pRS->GetWakeupTime  = AmiChipsetGetWakeupTime;
  pRS->SetWakeupTime  = AmiChipsetSetWakeupTime;
  
  Status = pSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, (VOID **)&SwDispatch);
  if (EFI_ERROR(Status))
  {
      ASSERT_EFI_ERROR(Status);
  }

  
  Status = SwDispatch->Register (SwDispatch, SaveSbScriptTable, &SwContext, &Handle);
  if (EFI_ERROR(Status))
  {
      ASSERT_EFI_ERROR(Status);
  }

  
  return EFI_SUCCESS;
}

/**
  This function is the entry point for this SMM. This function
  installs the runtime services related to SB in SMM.
  
  @param ImageHandle Image handle
  @param SystemTable Pointer to the system table
  
  @retval Return Status based on errors that occurred while waiting for
      time to expire.
  
**/
EFI_STATUS
EFIAPI
SbRuntimeSmmInitEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  InitAmiLib(ImageHandle, SystemTable);
  
  return InitSmmHandler(ImageHandle, SystemTable, SbRuntimeServiceInit, NULL);
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
