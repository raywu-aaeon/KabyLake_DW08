#include <Token.h>
#include <Setup.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmSxDispatch2.h>
#include <Library/PchInfoLib.h>
#include <Library/AmiSbMiscLib.h>
#include <Library/IoLib.h>
#include <Register\PchRegsPmc.h>
#include <Library/AmiGpioLib.h>

EFI_STATUS
EFIAPI
PchPmeWakeConfiguration (SETUP_DATA *SetupData)
{
	EFI_STATUS Status = EFI_SUCCESS;

        // Disable Wake Event
        SbResetGpe0En(TYPE_PME) ;
        // Clear Wake Status
        SbClearGpe0Sts(TYPE_PME) ;
        // Enable Wake Event
        SbSetGpe0En(TYPE_PME) ;
	return Status;
}

//ray_override / [XI-BringUp] Bring Up Porting / AC Power Loss from PCH : Always ON / Added >>
EFI_STATUS
EFIAPI
SmmSetTheStateToGoAfterG3 (SETUP_DATA *SetupData)
{
	EFI_STATUS Status = EFI_SUCCESS;

        SetAfterG3Bit( SetupData->StateAfterG3 == 0 ? 0 : 1  );

	return Status;
}
//ray_override / [XI-BringUp] Bring Up Porting / AC Power Loss from PCH : Always ON / Added <<

//ray_override / [XI-BringUp] Bring Up Porting / Support PCIe Wake / Added >>
EFI_STATUS
EFIAPI
PciExpressWakeConfiguration (SETUP_DATA *SetupData)
{
	EFI_STATUS Status = EFI_SUCCESS;
        UINT16  PmBase;

        PmBase = SbGetAcpiBaseAddress();
        // Disable Wake Event
        // GPIO[27] Enable (LANWAKE_EN)
        // PME_B0 Enable (PME_B0_EN)
        // PCI Express Enable (PCI_EXP_EN)
        // PCI Express Wake Disable (PCIEXP_WAKE_DIS)
        IoWrite32( PmBase + R_PCH_ACPI_GPE0_EN_127_96 ,
                IoRead32(PmBase + R_PCH_ACPI_GPE0_EN_127_96) & ~(B_PCH_ACPI_GPE0_EN_127_96_LAN_WAKE
                                                        | B_PCH_ACPI_GPE0_EN_127_96_PME_B0
                                                        | B_PCH_ACPI_GPE0_EN_127_96_PCI_EXP) ) ;
        IoWrite16( PmBase + R_PCH_ACPI_PM1_EN ,
                IoRead16(PmBase + R_PCH_ACPI_PM1_EN) | (B_PCH_ACPI_PM1_STS_PCIEXP_WAKE_STS) ) ;

        // Clear Wake Status
        IoWrite32( PmBase + R_PCH_ACPI_GPE0_STS_127_96 ,
                IoRead32(PmBase + R_PCH_ACPI_GPE0_STS_127_96) & ~(B_PCH_ACPI_GPE0_STS_127_96_LAN_WAKE
                                                        | B_PCH_ACPI_GPE0_STS_127_96_PME_B0
                                                        | B_PCH_ACPI_GPE0_STS_127_96_PCI_EXP) ) ;
        IoWrite16( PmBase + R_PCH_ACPI_PM1_STS ,
                IoRead16(PmBase + R_PCH_ACPI_PM1_STS) | (B_PCH_ACPI_PM1_STS_PCIEXP_WAKE_STS) ) ;
        // Enable Wake Event
        // GPIO[27] Enable (LANWAKE_EN)
        // PME_B0 Enable (PME_B0_EN)
        // PCI Express Enable (PCI_EXP_EN)
        // PCI Express Wake Disable (PCIEXP_WAKE_DIS)
        IoWrite32( PmBase + R_PCH_ACPI_GPE0_EN_127_96 ,
                IoRead32(PmBase + R_PCH_ACPI_GPE0_EN_127_96) | (B_PCH_ACPI_GPE0_EN_127_96_LAN_WAKE
                                                        | B_PCH_ACPI_GPE0_EN_127_96_PME_B0
                                                        | B_PCH_ACPI_GPE0_EN_127_96_PCI_EXP) ) ;
        IoWrite16( PmBase + R_PCH_ACPI_PM1_EN ,
                IoRead16(PmBase + R_PCH_ACPI_PM1_EN) & ~(B_PCH_ACPI_PM1_STS_PCIEXP_WAKE_STS) ) ;

	return Status;
}
//ray_override / [XI-BringUp] Bring Up Porting / Support PCIe Wake / Added <<