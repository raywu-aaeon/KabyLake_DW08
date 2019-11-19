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
/** @file AcpiModeEnable.c
    Provide functions to enable and disable ACPI mode

**/

//----------------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------------
#include <Token.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>

#include <Protocol/SmmBase2.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/DevicePath.h>
#include "AcpiModeEnable.h"
#include <Library/SmmServicesTableLib.h>
#include <PchAccess.h>
#include <Library/PchInfoLib.h>
#include <SbElinks.h>

#define AMI_SMM_SW_DISPATCH_PROTOCOL EFI_SMM_SW_DISPATCH2_PROTOCOL
#define AMI_SMM_SW_DISPATCH_CONTEXT  EFI_SMM_SW_REGISTER_CONTEXT
#define SMM_CHILD_DISPATCH_SUCCESS   EFI_SUCCESS

//----------------------------------------------------------------------------
// Variable and External Declaration(s)
//----------------------------------------------------------------------------
// Variable Declaration(s)

ACPI_DISPATCH_LINK  *gAcpiEnDispatchHead = 0, *gAcpiEnDispatchTail = 0;
ACPI_DISPATCH_LINK  *gAcpiDisDispatchHead = 0, *gAcpiDisDispatchTail = 0;

typedef VOID (ACPI_MODE_CALLBACK) (
    IN EFI_HANDLE                   DispatchHandle,
    IN AMI_SMM_SW_DISPATCH_CONTEXT  *DispatchContext
);

extern ACPI_MODE_CALLBACK ACPI_ENABLE_CALL_BACK_LIST EndOfAcpiCallBackList;
ACPI_MODE_CALLBACK* AcpiEnableCallbackList[] = {ACPI_ENABLE_CALL_BACK_LIST NULL};

extern ACPI_MODE_CALLBACK ACPI_DISABLE_CALL_BACK_LIST EndOfAcpiCallBackList;
ACPI_MODE_CALLBACK* AcpiDisableCallbackList[] = {ACPI_DISABLE_CALL_BACK_LIST NULL};

UINT8  gFirstInFlag = 0;
UINT16 wPM1_SaveState;
UINT32 dGPE_SaveState;

typedef enum {
  GPP_A = 0,
  GPP_B,
  GPP_C,
  GPP_D,
  GPP_E,
  GPP_F,
  GPP_G,
  GPP_H,
  GPP_I,
  GPD,
  Group_Max
} GPIO_GROUP_TYPE;

typedef struct _AMI_OEM_SCI_GPIO {
  GPIO_GROUP_TYPE       Group;
  UINT8                 PinNum;
} AMI_OEM_SCI_GPIO;

// GUID Definition(s)

//----------------------------------------------------------------------------

/**
    Create and add link to specified list.

    @param Size -
    @param Head -
    @param Tail -

    @retval VOID Pointer

    @note  Here is the control flow of this function:
              1. Allocate Link in Smm Pool.
              2. Add Link to end.
              3. Return Link address.

**/

VOID * AddLink(
    IN UINT32       Size,
    IN VOID         **Head,
    IN VOID         **Tail)
{
    VOID *Link;

    if (pSmst->SmmAllocatePool(EfiRuntimeServicesData, Size, (VOID **)&Link) != EFI_SUCCESS) return 0;

    ((GENERIC_LINK*)Link)->Link = 0;
    if (!*Head)
    {
        *Head = *Tail = Link;
    }
    else
    {
        ((GENERIC_LINK*)*Tail)->Link = Link;
        *Tail = Link;
    }

    return Link;
}

/**
    Remove link from specified list.

    @param Handle - EFI Handle
    @param Head -
    @param Tail -

    @retval BOOLEAN
        TRUE if link was removed. FALSE if link not in the list.

    @note  Here is the control flow of this function:
              1. Search link list for Link.
              2. Remove link from list.
              3. Free link.

**/

BOOLEAN RemoveLink(
    IN EFI_HANDLE   Handle,
    IN VOID         **Head,
    IN VOID         **Tail)
{
    GENERIC_LINK *PrevLink, *Link;

    PrevLink = *Head;

    // Is link first. Link address is the same as the Handle.
    if (((GENERIC_LINK*)*Head) == Handle)
    {
        if (PrevLink == *Tail) *Tail = 0; // If Tail = Head, then 0.
        *Head = PrevLink->Link;
        pSmst->SmmFreePool(PrevLink);
        return TRUE;
    }

    // Find Link.
    for (Link = PrevLink->Link; Link; PrevLink = Link, Link = Link->Link)
    {
        if (Link == Handle)     // Link address is the same as the Handle.
        {
            if (Link == *Tail) *Tail = PrevLink;
            PrevLink->Link = Link->Link;
            pSmst->SmmFreePool(Link);
            return TRUE;
        }
    }

    return FALSE;
}

/**
    Register a Link on ACPI enable SMI.

    @param This -
    @param Function -
        Context -


    @retval Handle -
    @retval EFI_STATUS

    @note  Here is the control flow of this function:
              1. Verify if Context if valid. If invalid,
                 return EFI_INVALID_PARAMETER.
              2. Allocate structure and add to link list.
              3. Fill link.
              4. Enable Smi Source.

**/

EFI_STATUS EfiAcpiEnRegister(
    IN EFI_ACPI_DISPATCH_PROTOCOL   *This,
    IN EFI_ACPI_DISPATCH            Function,
    OUT EFI_HANDLE                  *Handle)
{
    ACPI_DISPATCH_LINK *NewLink;

    NewLink = AddLink(sizeof(ACPI_DISPATCH_LINK), \
                      (VOID **)&gAcpiEnDispatchHead, \
                      (VOID **)&gAcpiEnDispatchTail);
    if (!NewLink) return EFI_OUT_OF_RESOURCES;

    NewLink->Function   = Function;
    *Handle = NewLink;

    return EFI_SUCCESS;
}

/**
    Unregister a Link on ACPI enable SMI.

    @param This -
    @param Handle -

    @retval EFI_STATUS

    @note  Here is the control flow of this function:
              1. Remove link. If no link, return EFI_INVALID_PARAMETER.
              2. Disable SMI Source if no other handlers using source.
              3. Return EFI_SUCCESS.

**/

EFI_STATUS 
EFIAPI
EfiAcpiEnUnregister(
    IN EFI_ACPI_DISPATCH_PROTOCOL   *This,
    IN EFI_HANDLE                   Handle)
{
    if (!RemoveLink(Handle, (VOID **)&gAcpiEnDispatchHead, (VOID **)&gAcpiEnDispatchTail))
        return EFI_INVALID_PARAMETER;
    return EFI_SUCCESS;
}

/**
    Register a Link on ACPI disable SMI.

    @param This -
    @param Function -
        *Context -


    @retval Handle EFI Handle
    @retval EFI_STATUS

    @note  Here is the control flow of this function:
              1. Verify if Context if valid. If invalid,
                 return EFI_INVALID_PARAMETER.
              2. Allocate structure and add to link list.
              3. Fill link.
              4. Enable Smi Source.

**/

EFI_STATUS 
EFIAPI
EfiAcpiDisRegister(
    IN EFI_ACPI_DISPATCH_PROTOCOL   *This,
    IN EFI_ACPI_DISPATCH            Function,
    OUT EFI_HANDLE                  *Handle)
{
    ACPI_DISPATCH_LINK *NewLink;

    NewLink = AddLink(sizeof(ACPI_DISPATCH_LINK), \
                      (VOID **)&gAcpiDisDispatchHead, \
                      (VOID **)&gAcpiDisDispatchTail);
    if (!NewLink) return EFI_OUT_OF_RESOURCES;

    NewLink->Function   = Function;
    *Handle = NewLink;

    return EFI_SUCCESS;
}

/**
    Unregister a Link on ACPI Disable SMI.

    @param This -
        Handle - EFI Handle

    @retval EFI_STATUS

    @note  Here is the control flow of this function:
              1. Remove link. If no link, return EFI_INVALID_PARAMETER.
              2. Disable SMI Source if no other handlers using source.
              3. Return EFI_SUCCESS.

**/

EFI_STATUS 
EFIAPI
EfiAcpiDisUnregister(
    IN EFI_ACPI_DISPATCH_PROTOCOL   *This,
    IN EFI_HANDLE                   Handle)
{
    if (!RemoveLink(Handle, (VOID **)&gAcpiDisDispatchHead, (VOID **)&gAcpiDisDispatchTail))
        return EFI_INVALID_PARAMETER;
    return EFI_SUCCESS;
}

EFI_ACPI_DISPATCH_PROTOCOL gEfiAcpiEnDispatchProtocol = \
        {EfiAcpiEnRegister, EfiAcpiEnUnregister};

EFI_ACPI_DISPATCH_PROTOCOL gEfiAcpiDisDispatchProtocol = \
        {EfiAcpiDisRegister, EfiAcpiDisUnregister};

/**
    Programming the corresponding GPIO pin to generate SCI#.

    @param VOID

    @retval VOID
**/
VOID SbGpioSciInit (VOID)
{
    AMI_OEM_SCI_GPIO    GpioTable[] = {SKL_GPIO_SCI_OVERRIDE};
    AMI_OEM_GPIO        GpioPin;
    UINT8               Size = sizeof(GpioTable) / sizeof(AMI_OEM_GPIO);
    PCH_SERIES          PchSeries = GetPchSeries();
    UINT8               Index;
    
    for (Index = 0; Index < Size; Index++) {
        if (GpioTable[Index].Group < Group_Max) {
            if (PchSeries == PchLp) {
                if (GpioTable[Index].Group == GPD) {
                    GpioTable[Index].Group -= 2;
                } else if(GpioTable[Index].Group > GPP_G) {
                    continue;
                }
                GpioPin.Group = GpioTable[Index].Group + Lp_Gpio_GPP_A;
            } else 
                GpioPin.Group = GpioTable[Index].Group + H_Gpio_GPP_A;
                        
            GpioPin.PinNum = GpioTable[Index].PinNum;
            
            SbProgramGpioGPE(GpioPin);
        }
    }    
}

/**
    This function enable ACPI mode by clearing all SMI and
    enabling SCI generation
    This routine is also called on a S3 resume for special ACPI
    programming.
    Status should not be cleared on S3 resume. Enables are
    already taken care of.

    PI 1.1, 1.2
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
EnableAcpiMode (
	IN EFI_HANDLE       DispatchHandle,
	IN CONST VOID       *DispatchContext OPTIONAL,
	IN OUT VOID         *CommBuffer OPTIONAL,
	IN OUT UINTN        *CommBufferSize OPTIONAL )
{
    ACPI_DISPATCH_LINK      *Link;
	UINT16                  wordValue;
	UINT32                  dwordValue;
	UINTN                   Index;

#if defined EMUL6064_SUPPORT && EMUL6064_SUPPORT == 1
#if defined USB_RUNTIME_DRIVER_IN_SMM && USB_RUNTIME_DRIVER_IN_SMM == 0     
  if (READ_PCI8_SB(R_PCH_LPC_ULKMC))
    WRITE_PCI8_SB(R_PCH_LPC_ULKMC, 0);
#endif  
#endif
  
  if ( gFirstInFlag == 0 ) {
    gFirstInFlag = 1;

    //  Check if WAK bit is set, if yes skip clearing status
    wordValue = READ_IO16_PM(R_PCH_ACPI_PM1_STS); // 0x00
    if (wordValue & B_PCH_ACPI_PM1_STS_WAK) {

    } else {
      //Disable SMI Sources
      dwordValue = READ_IO32_PM(R_PCH_SMI_EN);    //SMI_EN (SMI Control and Enable register.)

//####    dwordValue &= B_PCH_SMI_EN_LEGACY_USB2 | B_PCH_SMI_EN_APMC | B_PCH_SMI_EN_ON_SLP_EN | B_PCH_SMI_EN_GBL_SMI;  //Leave APMC_EN and SMI_ON_SLP_EN bits
#if DIS_SW_SMI_TIMER_BEFORE_OS      
      dwordValue &= ~(B_PCH_SMI_EN_ON_SLP_EN | B_PCH_SMI_EN_SWSMI_TMR);  // Clear SLP_SMI_EN and SWSMI_TMR bit.
#else
      dwordValue &= ~(B_PCH_SMI_EN_ON_SLP_EN);  // Clear SLP_SMI_EN and SWSMI_TMR bit.
#endif      

#if defined EMUL6064_SUPPORT && EMUL6064_SUPPORT == 1
#if defined USB_RUNTIME_DRIVER_IN_SMM && USB_RUNTIME_DRIVER_IN_SMM == 0      
      dwordValue &= ~(B_PCH_SMI_EN_LEGACY_USB);
#endif      
#endif
      WRITE_IO32_PM(R_PCH_SMI_EN, dwordValue);

    //Disable and Clear PM1 Sources except power button   
    wPM1_SaveState = READ_IO16_PM(R_PCH_ACPI_PM1_EN); //PM1_EN
    WRITE_IO16_PM(R_PCH_ACPI_PM1_EN, BIT08);
    WRITE_IO16_PM(R_PCH_ACPI_PM1_STS, 0xffff);

    //Set day of month alarm invalid - ACPI 1.0 section 4.7.2.4
    WRITE_IO8(CMOS_ADDR_PORT, 0x0d |0x80);         //RTC_REGD
    WRITE_IO8(CMOS_DATA_PORT, 0);
    }
  }

    //Enable Sci
    SET_IO8_PM(R_PCH_ACPI_PM1_CNT,  B_PCH_ACPI_PM1_CNT_SCI_EN);

    SbGpioSciInit();
    
    // Always enable B_PCH_ACPI_PM1_EN_PWRBTN 
    SET_IO16_PM(R_PCH_ACPI_PM1_EN, BIT08);
    
    for (Link = gAcpiEnDispatchHead; Link; Link = Link->Link)
    {
        Link->Function(Link);
    }

    for (Index = 0; AcpiEnableCallbackList[Index] != NULL; Index++) {
        AcpiEnableCallbackList[Index](DispatchHandle, (AMI_SMM_SW_DISPATCH_CONTEXT *)DispatchContext);
    }

    WRITE_IO8(0x80, SW_SMI_ACPI_ENABLE);

    return SMM_CHILD_DISPATCH_SUCCESS;
}

/**
    This function disables ACPI mode by enabling SMI generation

    PI 1.1, 1.2
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
DisableAcpiMode (
	IN EFI_HANDLE       DispatchHandle,
	IN CONST VOID       *DispatchContext OPTIONAL,
	IN OUT VOID         *CommBuffer OPTIONAL,
	IN OUT UINTN        *CommBufferSize OPTIONAL )
{
    ACPI_DISPATCH_LINK  *Link;
    UINTN               Index;               

    //Clear PM Sources and set Enables
    WRITE_IO16_PM(R_PCH_ACPI_PM1_STS, 0xffff);
    WRITE_IO16_PM(R_PCH_ACPI_PM1_EN, wPM1_SaveState);

    //Disable SCI
    RESET_IO8_PM(R_PCH_ACPI_PM1_CNT, BIT00);

    for (Link = gAcpiDisDispatchHead; Link; Link = Link->Link)
    {
        Link->Function(Link);
    }
    
    for (Index = 0; AcpiDisableCallbackList[Index] != NULL; Index++) {
        AcpiDisableCallbackList[Index](DispatchHandle, (AMI_SMM_SW_DISPATCH_CONTEXT *)DispatchContext);        
    }

    WRITE_IO8(0x80, SW_SMI_ACPI_DISABLE);

    return SMM_CHILD_DISPATCH_SUCCESS;
}

/**
    This function is part of the ACPI mode enable/disable
    driver and invoked during SMM initialization.  As the name
    suggests this function is called from SMM

    @param ImageHandle Handle for this FFS image
    @param SystemTable Pointer to the system table

    @retval EFI_STATUS
**/
EFI_STATUS AcpiModeEnableInSmmFunction(
    IN  EFI_HANDLE          ImageHandle, 
    IN  EFI_SYSTEM_TABLE    *SystemTable
)
{
    EFI_STATUS                   Status;
    EFI_HANDLE                   Handle = NULL;
    AMI_SMM_SW_DISPATCH_PROTOCOL *SwDispatch = NULL;
    EFI_HANDLE                   DummyHandle = NULL;
    AMI_SMM_SW_DISPATCH_CONTEXT  AcpiEnableContext = {SW_SMI_ACPI_ENABLE};
    AMI_SMM_SW_DISPATCH_CONTEXT  AcpiDisableContext = {SW_SMI_ACPI_DISABLE};

    Status = InitAmiSmmLib( ImageHandle, SystemTable );
    if (EFI_ERROR(Status)) return Status;

    Status = pSmst->SmmLocateProtocol( &gEfiSmmSwDispatch2ProtocolGuid , \
                                       NULL, \
                                       (VOID **)&SwDispatch );
    if (EFI_ERROR(Status)) return Status;

    Status = SwDispatch->Register( SwDispatch, \
                                   EnableAcpiMode, \
                                   &AcpiEnableContext, \
                                   &Handle );
    if (EFI_ERROR(Status)) return Status;   

    Status = pSmst->SmmInstallProtocolInterface (
                      &DummyHandle,
                      &gEfiAcpiEnDispatchProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      &gEfiAcpiEnDispatchProtocol
                      );
    if (EFI_ERROR(Status)) return Status;

    Status = SwDispatch->Register( SwDispatch, \
                                   DisableAcpiMode, \
                                   &AcpiDisableContext,\
                                   &Handle );
    if (EFI_ERROR(Status)) return Status;   

    Status = pSmst->SmmInstallProtocolInterface (
                      &DummyHandle,
                      &gEfiAcpiDisDispatchProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      &gEfiAcpiDisDispatchProtocol
                      );

    return Status;
}

/**
    This function is the entry point for the ACPI mode enable/disable
    driver.  This function is called twice: first time by the
    DXE dispatcher and the next time when it is loaded into the
    SMRAM

    @param ImageHandle Handle for this FFS image
    @param SystemTable Pointer to the system table

    @retval EFI_STATUS
**/

EFI_STATUS EFIAPI
AcpiModeEnableInit(
    IN  EFI_HANDLE              ImageHandle,
    IN  EFI_SYSTEM_TABLE        *SystemTable
)
{
    InitAmiLib(ImageHandle,SystemTable);

    return InitSmmHandler(ImageHandle, 
                          SystemTable, 
                          AcpiModeEnableInSmmFunction, 
                          NULL);
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
