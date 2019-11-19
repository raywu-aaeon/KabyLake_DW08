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

/** @file AmiHpetApicLib.c
    South Bridge HPET APIC Library implementation

*/

#include <Token.h>
#include <Library/AmiHpetApicLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>

#if defined HPET_PROTOCOL_SUPPORT && HPET_PROTOCOL_SUPPORT == 1
//----------------------------------------------------------------------------
// Generic IO APIC routine.
//----------------------------------------------------------------------------
UINT8 gBspLocalApicID = 0;

/**
    This routine returns a structure pointer to the I/O APIC.

    @param VOID

    @retval IO_APIC structure.
**/
struct IO_APIC* IoApicBase( VOID )
{
	static UINT32 IoApicAddr = 0;
	if (IoApicAddr == 0 || IoApicAddr == -1) {
        // This value may need to read from chipset registers.
		IoApicAddr = APCB;
	}

	return (struct IO_APIC *)(UINTN)IoApicAddr;
}

/**
    This function writes a 32bits data to the register of
    I/O APIC.

    @param Reg - The register offset to be written.
    @param Value - A 32bits data will be written to the register
                   of I/O APIC.

    @retval EFI_SUCCESS
**/
EFI_STATUS IoApicWrite (
    IN UINT8        Reg,
    IN UINT32       Value )
{
	struct IO_APIC *IoApicStruct = IoApicBase();

    (*(volatile UINT8*)(&IoApicStruct->Index)=(Reg));
	(*(volatile UINT32*)(&IoApicStruct->Data)=(Value));

	return EFI_SUCCESS;
}

/**
    This function reads a 32bits data from the register of
    I/O APIC.

    @param Reg - the register offset to be read.

    @return A 32bits data read from the register of I/O APIC.
**/
UINT32 IoApicRead (
    IN UINT8        Reg )
{
	struct IO_APIC *IoApicStruct = IoApicBase();

	(*(volatile UINT8*)(&IoApicStruct->Index)=(Reg));
	return (*(volatile UINT32*)(&IoApicStruct->Data));
}

/**
    This function enables the specific interrupt pin of I/O APIC.

    @param Irq - The pin number of I/O APIC
    @param LevelTriggered - Trigger mechanism (level or edge)
                            for this INT pin.
    @param Polarity - Specifies the polarity of the INT pin.
                      (Active High or Active Low)
    @retval EFI_SUCCESS
**/
EFI_STATUS IoApicEnableIrq (
    IN UINT8        Irq,
    IN BOOLEAN      LevelTriggered,
    IN BOOLEAN      Polarity )
{
	IO_APIC_ROUTE_ENTRY	ApicEntry;
	union ENTRY_UNION			Eu = {{0, 0}};

	ApicEntry.DestMode = 0; // 0: physical
	ApicEntry.Mask = 0;		 // 0 : enable
	ApicEntry.Dest = gBspLocalApicID; // suppose the BSP handle interrupt.
	ApicEntry.DeliveryMode = 0;      // 000: FIXED
	ApicEntry.Polarity = (Polarity) ? 1 : 0;
	ApicEntry.Trigger = (LevelTriggered) ? 1 : 0;
	ApicEntry.Vector = MASTER_INTERRUPT_BASE + Irq;

	Eu.Entry = ApicEntry;
	IoApicWrite(IO_APIC_REDIR_TABLE_HIGH + 2 * Irq, Eu.W2);
	IoApicWrite(IO_APIC_REDIR_TABLE_LOW + 2 * Irq, Eu.W1);
	return EFI_SUCCESS;
}

/**
    This function disables the specific interrupt pin of I/O APIC.

    @param Irq - The pin number of I/O APIC

    @retval EFI_SUCCESS
**/
EFI_STATUS IoApicDisableIrq (
    IN UINT8        Irq )
{
	union ENTRY_UNION Eu = {{0, 0}};

	Eu.W1 = IoApicRead(IO_APIC_REDIR_TABLE_LOW + 2 * Irq);
	Eu.W2 = IoApicRead(IO_APIC_REDIR_TABLE_HIGH + 2 * Irq);
	Eu.Entry.Mask = 1;
	IoApicWrite(IO_APIC_REDIR_TABLE_LOW + 2 * Irq, Eu.W1);
	IoApicWrite(IO_APIC_REDIR_TABLE_HIGH + 2 * Irq, Eu.W2);
	IoApicEoi(Irq);

	return EFI_SUCCESS;
}

/**
    This routine masks the specific interrupt pin of I/O APIC.

    @param Irq - The pin number of I/O APIC

    @retval EFI_SUCCESS
**/
EFI_STATUS IoApicMaskIrq (
    IN UINT8        Irq )
{
	union ENTRY_UNION Eu = {{0, 0}};

	Eu.W1 = IoApicRead(IO_APIC_REDIR_TABLE_LOW + 2 * Irq);
	Eu.Entry.Mask = 1;
	IoApicWrite(IO_APIC_REDIR_TABLE_LOW + 2 * Irq, Eu.W1);

	return EFI_SUCCESS;
}

/**
    This routine unmasks the specific interrupt pin of I/O APIC.

    @param Irq - The pin number of I/O APIC

    @retval EFI_SUCCESS
**/
EFI_STATUS IoApicUnmaskIrq (
    IN UINT8        Irq )
{
	union ENTRY_UNION Eu = {{0, 0}};

	Eu.W1 = IoApicRead(IO_APIC_REDIR_TABLE_LOW + 2 * Irq);
	Eu.Entry.Mask = 0;
	IoApicWrite(IO_APIC_REDIR_TABLE_LOW + 2 * Irq, Eu.W1);

	return EFI_SUCCESS;
}

/**
    This routine issues an EOI to the specific pin of I/O APIC.

    @param Irq - The pin number of I/O APIC

    @retval EFI_SUCCESS
**/
EFI_STATUS IoApicEoi (
    IN UINT8        Irq )
{
	EFI_STATUS 	Status = EFI_SUCCESS;
	UINT32		Vector = MASTER_INTERRUPT_BASE + Irq;
	struct IO_APIC *IoApicStruct = IoApicBase();
	UINT32		Isr = 0;
	union ENTRY_UNION Eu = {{0, 0}};

	do {
		(*(volatile UINT32*)(&IoApicStruct->Eoi)=(Vector));
		Eu.W1 = IoApicRead(IO_APIC_REDIR_TABLE_LOW + 2 * Irq);
	} while (Eu.Entry.Irr);

	do {
		(*(volatile UINT32*)(LOCAL_APIC_BASE + APIC_EOI_REGISTER)=(Vector));
		Isr = (*(volatile UINT32*)(UINTN)(LOCAL_APIC_BASE + ISR_REG (Vector)));
	} while (Isr & ISR_BIT(Vector));

	return EFI_SUCCESS;
}

/**
    This routine gets the pin number of I/O APIC for HPET.

    @param VOID

    @return Irq - The pin number of I/O APIC for HPET.
**/
UINT8 GetHpetApicPin (VOID)
{
    volatile HPET_TIMER_CONFIGURATION_REGISTER   TimerConfiguration;
    UINT8   Irq = 0;
    UINTN   TimerIndex;
      
    // Refer to HpetTimer.c
    for (TimerIndex = 0; TimerIndex <= 7; TimerIndex++) {
        //
        // Read the HPET Timer Capabilities and Configuration register
        //
        TimerConfiguration.Uint64 = (*(volatile UINT64*)(HPET_BASE_ADDRESS + HPET_TIMER_CONFIGURATION_OFFSET + TimerIndex * HPET_TIMER_STRIDE));
      
        //
        // Check to see if this HPET Timer supports I/O APIC interrupts
        //
        if (TimerConfiguration.Bits.InterruptRouteCapability != 0) {
            //
            // The index of the first HPET Timer that supports I/O APIC interrupts
            //
            break;
        }
    } 

    Irq = TimerConfiguration.Bits.InterruptRoute;
    
    return Irq;
}

/**
    This routine checks the pin of I/O APIC for HPET is enabled or
    not.

    @param VOID

    @retval TRUE The pin of I/O APIC for HPET is enabled 
    @retval FALSE The pin of I/O APIC for HPET is disabled 
**/
BOOLEAN IsHpetApicEnable (VOID)
{
    union ENTRY_UNION Eu = {{0, 0}};
    UINT8 Irq;
    
    if (FeaturePcdGet(PcdHpetMsiEnable)) {
        return FALSE;
    }
    
    Irq = GetHpetApicPin();
    
    Eu.W1 = IoApicRead(IO_APIC_REDIR_TABLE_LOW + 2 * Irq);
    
    return (Eu.Entry.Mask) ? FALSE : TRUE;
}
#endif

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
