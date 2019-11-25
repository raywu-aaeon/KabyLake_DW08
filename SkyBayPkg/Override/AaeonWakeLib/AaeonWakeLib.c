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
