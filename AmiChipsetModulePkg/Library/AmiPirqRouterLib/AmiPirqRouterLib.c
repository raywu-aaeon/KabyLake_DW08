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

/** @file AmiPirqRouterLib.c
    South Bridge Pirq Router Library implementation

*/

#include <AmiDxeLib.h>
#include <Library/AmiPirqRouterLib.h>
#include <Sb.h>

#define MAX_PIRQS        8

UINT8   bMaxPIRQ        = MAX_PIRQS;                // For CSM
UINT8   bRouterBus      = SB_BUS;// PORTING REQUIRED (Use appropriate Equate)
UINT8   bRouterDevice   = SB_DEV;// PORTING REQUIRED (Use appropriate Equate)
UINT8   bRouterFunction = SB_FUN;// PORTING REQUIRED (Use appropriate Equate)
UINT8   RRegs[MAX_PIRQS]= {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8}; // Porting required

// Local variable
static EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL      *gPciRootBridgeIo;


/**
    This function is clears the routing registers to default values

    @param PciRBIo Root bridge IO protocol pointer

    @retval EFI_STATUS Returned from PCI read call

**/
EFI_STATUS SbGenInitializeRouterRegisters (
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRBIo
)
{
    UINT8       RegIdx;

    gPciRootBridgeIo = PciRBIo; // Save RB IO value for later use

    for (RegIdx = 0; RegIdx < MAX_PIRQS; RegIdx++) {
        MMIO_WRITE8(SB_PCR_ADDRESS(PID_ITSS, (R_SB_PCR_ITSS_PIRQA_ROUT + RegIdx)), 0x80);
    }

    return EFI_SUCCESS;
}

/**
    This function returns the 0 based PIRQ index (PIRQ0, 1 etc)
    based on the PIRQ register number specified in the routing table

    @param PIRQRegister - Register number of the PIR

    @retval PIRQIndex 0 - (MAX_PIRQ -1)
            0xFF if the register is invalid

    @note  Here is the control flow of this function:
              1. If Invalid PirqNumber, return EFI_INVALID_PARAMETER.
              2. Read into *PriqData from PIRQ register for Pirq requested.
              3. Return EFI_SUCCESS.
**/
UINT8 SBGen_GetPIRQIndex (
    UINT8 PIRQRegister
)
{
    UINT8   RrIndx = 0;

    while ((RrIndx < MAX_PIRQS) && (RRegs[RrIndx] != PIRQRegister)) {
        RrIndx++;
    }

    if (RrIndx == MAX_PIRQS) {
        return 0xff;
    }

    return RrIndx;
}

/**
    This function reads the IRQ associated with the PIRQ from
    the chipset register

    @param This Pointer to Legacy interrupt protocol
    @param PirqNumber PIRQ number to read
    @param PirqData IRQ programmed for this PIRQ (BIT7 will be
           set if the PIRQ is not programmed)

    @retval EFI_STATUS
            EFI_SUCCESS On successfully IRQ value return
            EFI_INVALID_PARAMETER   If PirqNumber is greater than max PIRQs

    @note  Here is the control flow of this function:
              1. If Invalid PirqNumber, return EFI_INVALID_PARAMETER.
              2. Read into *PriqData from PIRQ register for PIRQ requested.
              3. Return EFI_SUCCESS.
**/
EFI_STATUS SbGenReadPirq (
    IN EFI_LEGACY_INTERRUPT_PROTOCOL    *This,
    IN UINT8                            PirqNumber,
    OUT UINT8                           *PirqData
)
{
    if (PirqNumber > MAX_PIRQS) return EFI_INVALID_PARAMETER;

    // If Pirq is not routed, bit 7 is set, however specification does not
    // specify a return error for this condition.
    //*PirqData = READ_PCI8_SB(RRegs[PirqNumber]);
    *PirqData = MMIO_READ8(SB_PCR_ADDRESS(PID_ITSS, (R_SB_PCR_ITSS_PIRQA_ROUT + PirqNumber)));

    return EFI_SUCCESS;
}

/**
    This function writes an IRQ value allocated for the PIRQ by
    programming the chipset register

    @param This - Pointer to Legacy interrupt protocol
    @param PirqNumber - PIRQ number to read
    @param PirqData - IRQ to be programmed

    @retval EFI_STATUS
            EFI_SUCCESS On successfully IRQ value return
            EFI_INVALID_PARAMETER If PirqNumber is greater than
                                  max PIRQs or PirqData is greater
                                  than 15 (MAX IRQ)

    @note  Here is the control flow of this function:
              1. If Invalid PirqNumber, return EFI_INVALID_PARAMETER.
              2. Write PirqData to PIRQ register for PIRQ requested.
              3. Return EFI_SUCCESS.
**/
EFI_STATUS SbGenWritePirq (
    IN EFI_LEGACY_INTERRUPT_PROTOCOL    *This,
    IN UINT8                            PirqNumber,
    IN UINT8                            PirqData
)
{
    UINT8           PirqValue = PirqData & 0x0f;

    if (PirqNumber > MAX_PIRQS) return EFI_INVALID_PARAMETER;
    if (PirqData > 15) return EFI_INVALID_PARAMETER;

    //WRITE_PCI8_SB(RRegs[PirqNumber], PirqValue);
    MMIO_WRITE8(SB_PCR_ADDRESS(PID_ITSS, (R_SB_PCR_ITSS_PIRQA_ROUT + PirqNumber)), PirqValue);

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
