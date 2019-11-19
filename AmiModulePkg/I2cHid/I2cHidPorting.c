//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file I2cHidPorting.c
    Porting I2C interrupt and reset pin.

**/

//---------------------------------------------------------------------------

#include <Efi.h>
#include <Token.h>
#include <Protocol/DriverBinding.h>
#include <I2cHid.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/IoLib.h>

//---------------------------------------------------------------------------

EFI_STATUS
Dev0HwReset (VOID);

BOOLEAN
Dev0IntSts (VOID);

EFI_STATUS
Dev1HwReset (VOID);

BOOLEAN
Dev1IntSts (VOID);

EFI_STATUS
Dev2HwReset (VOID);

BOOLEAN
Dev2IntSts (VOID);

EFI_STATUS
Dev3HwReset (VOID);

BOOLEAN
Dev3IntSts (VOID);

HID_DEV gHidDevTable[] = {
        {I2CHID_DEV_3_BUS_NUMBER, I2CHID_DEV_3_SLAVE_ADDRESS, I2CHID_DEV_3_HID_DESCRIPTOR_ADDRESS, Dev3IntSts, Dev3HwReset},
        {I2CHID_DEV_2_BUS_NUMBER, I2CHID_DEV_2_SLAVE_ADDRESS, I2CHID_DEV_2_HID_DESCRIPTOR_ADDRESS, Dev2IntSts, Dev2HwReset},
        {I2CHID_DEV_1_BUS_NUMBER, I2CHID_DEV_1_SLAVE_ADDRESS, I2CHID_DEV_1_HID_DESCRIPTOR_ADDRESS, Dev1IntSts, Dev1HwReset},
        {I2CHID_DEV_0_BUS_NUMBER, I2CHID_DEV_0_SLAVE_ADDRESS, I2CHID_DEV_0_HID_DESCRIPTOR_ADDRESS, Dev0IntSts, Dev0HwReset}
};

UINTN   gHidDevCount = sizeof(gHidDevTable)/sizeof(HID_DEV);

/**
    Device 0 hardware reset.

    @param VOID

    @retval EFI_SUCCESS - The function completed successfully

**/

EFI_STATUS
Dev0HwReset (VOID)
{
    if (I2CHID_DEV_0_HID_RESET_ADDRESS == 0xFFFFFFFF)
        return EFI_SUCCESS;
    //
    // Set RST Pin as low for HW reset
    //    
    MmioWrite32(I2CHID_DEV_0_HID_RESET_ADDRESS, LOW_VOLT);

    gBS->Stall(DELAY_10MS); // Wait 10ms

    MmioWrite32(I2CHID_DEV_0_HID_RESET_ADDRESS, HIGH_VOLT);

    return EFI_SUCCESS;
}

/**
    Device 1 hardware reset.

    @param VOID

    @retval EFI_SUCCESS - The function completed successfully

**/

EFI_STATUS
Dev1HwReset (VOID)
{
    if (I2CHID_DEV_1_HID_RESET_ADDRESS == 0xFFFFFFFF)
        return EFI_SUCCESS;
    //
    // Set RST Pin as low for HW reset
    //    
    MmioWrite32(I2CHID_DEV_1_HID_RESET_ADDRESS, LOW_VOLT);

    gBS->Stall(DELAY_10MS); // Wait 10ms

    MmioWrite32(I2CHID_DEV_1_HID_RESET_ADDRESS, HIGH_VOLT);

    return EFI_SUCCESS;
}

/**
    Device 2 hardware reset.

    @param VOID

    @retval EFI_SUCCESS - The function completed successfully

**/

EFI_STATUS
Dev2HwReset (VOID)
{
    if (I2CHID_DEV_2_HID_RESET_ADDRESS == 0xFFFFFFFF)
        return EFI_SUCCESS;
    //
    // Set RST Pin as low for HW reset
    //    
    MmioWrite32(I2CHID_DEV_2_HID_RESET_ADDRESS, LOW_VOLT);

    gBS->Stall(DELAY_10MS); // Wait 10ms

    MmioWrite32(I2CHID_DEV_2_HID_RESET_ADDRESS, HIGH_VOLT);

    return EFI_SUCCESS;
}

/**
    Device 3 hardware reset.

    @param VOID

    @retval EFI_SUCCESS - The function completed successfully

**/

EFI_STATUS
Dev3HwReset (VOID)
{
    if (I2CHID_DEV_3_HID_RESET_ADDRESS == 0xFFFFFFFF)
        return EFI_SUCCESS;
    //
    // Set RST Pin as low for HW reset
    //    
    MmioWrite32(I2CHID_DEV_3_HID_RESET_ADDRESS, LOW_VOLT);

    gBS->Stall(DELAY_10MS); // Wait 10ms

    MmioWrite32(I2CHID_DEV_3_HID_RESET_ADDRESS, HIGH_VOLT);

    return EFI_SUCCESS;
}

/**
    According HID over I2C specification, all device followed this specification.
    Must have INT pin. This function return INT status.

    @param VOID

    @retval FALSE INT pin is high, device haven't data
    @retval TRUE  INT pin is low, device have data

**/

BOOLEAN
Dev0IntSts (VOID)
{
#if I2C_INTERRUPT_GPIO_RX_STATE == 0x00
    if ((MmioRead32(I2CHID_DEV_0_HID_INTERRUPT_ADDRESS) & BIT0) == BIT0) {
        return FALSE;
    }
    else{
        return TRUE;
    }
#else
    if ((MmioRead32(I2CHID_DEV_0_HID_INTERRUPT_ADDRESS) & BIT1) == BIT1) {
        return FALSE;
    }
    else{
        return TRUE;
    }
#endif
}

/**
    According HID over I2C specification, all device followed this specification.
    Must have INT pin. This function return INT status.

    @param VOID

    @retval FALSE INT pin is high, device haven't data
    @retval TRUE  INT pin is low, device have data

**/

BOOLEAN
Dev1IntSts (VOID)
{
#if I2C_INTERRUPT_GPIO_RX_STATE == 0x00
    if ((MmioRead32(I2CHID_DEV_1_HID_INTERRUPT_ADDRESS) & BIT0) == BIT0) {
        return FALSE;
    }
    else{
        return TRUE;
    }
#else
    if ((MmioRead32(I2CHID_DEV_1_HID_INTERRUPT_ADDRESS) & BIT1) == BIT1) {
        return FALSE;
    }
    else{
        return TRUE;
    }
#endif
}

/**
    According HID over I2C specification, all device followed this specification.
    Must have INT pin. This function return INT status.

    @param VOID

    @retval FALSE INT pin is high, device haven't data
    @retval TRUE  INT pin is low, device have data

**/

BOOLEAN
Dev2IntSts (VOID)
{
#if I2C_INTERRUPT_GPIO_RX_STATE == 0x00
    if ((MmioRead32(I2CHID_DEV_2_HID_INTERRUPT_ADDRESS) & BIT0) == BIT0) {
        return FALSE;
    }
    else{
        return TRUE;
    }
#else
    if ((MmioRead32(I2CHID_DEV_2_HID_INTERRUPT_ADDRESS) & BIT1) == BIT1) {
        return FALSE;
    }
    else{
        return TRUE;
    }
#endif
}

/**
    According HID over I2C specification, all device followed this specification.
    Must have INT pin. This function return INT status.

    @param VOID

    @retval FALSE INT pin is high, device haven't data
    @retval TRUE  INT pin is low, device have data

**/

BOOLEAN
Dev3IntSts (VOID)
{
#if I2C_INTERRUPT_GPIO_RX_STATE == 0x00
    if ((MmioRead32(I2CHID_DEV_3_HID_INTERRUPT_ADDRESS) & BIT0) == BIT0) {
        return FALSE;
    }
    else{
        return TRUE;
    }
#else
    if ((MmioRead32(I2CHID_DEV_3_HID_INTERRUPT_ADDRESS) & BIT1) == BIT1) {
        return FALSE;
    }
    else{
        return TRUE;
    }
#endif
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
