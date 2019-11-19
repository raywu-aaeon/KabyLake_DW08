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

/** @file SbHpetTimerLib.h
  This file contains HPET timer enabled and disabled library functions.
*/

#ifndef __SB_SPI_LIB_H__
#define __SB_SPI_LIB_H__

#define CS_HIGH     1
#define CS_LOW      0

#ifdef __cplusplus
extern "C" {
#endif

/**    
    This function initial GSPI.

    @param GspiBarAddress - Bar address of the GSPI Controller
    @param DivM - m value for the m over n divider. Refer CLOCKS register for more detail.
    @param DivN - n value for the m over n divider. Refer CLOCKS register for more detail.
    @param flags - BIT0 : CPOL. 0 = The inactive ot idle state of the SSPSCLK is low.
                                1 = The inactive ot idle state of the SSPSCLK is high.
                   BIT1 : CPHA. 0 = SSPSCLK is inactive one cycle at the start of a frame and cycle 
                                    at the end of a frame.
                                1 = SSPSCLK is inactive for one half cycle at the start of a frame 
                                    and one cycle at the end of a frame.

    @retval  EFI_SUCCESS - Initial is done.
**/
EFI_STATUS 
GspiInit(
  UINTN     GspiBarAddress,
  UINT16    DivM,
  UINT16    DivN,
  UINT32    flags
) ;

/**    
    This function disable SSP operation and clock.

    @param 
**/
VOID
GspiStop(
  VOID
);

/**    
    This function Set the Chip Select pin state.

    @param State - Should be CS_HIGH or CS_LOW

**/
VOID
SetSpiCsState(
  UINT8 State
);

/**    
    This function read 1 byte from MISO

    @retval  
**/
UINT8 
SpiRead8 (
 VOID
);

/**    
    This function output 1 byte from MOSI

    @param Value - output data
**/
VOID 
SpiWrite8 (
  IN    UINT8  Value
);

#ifdef __cplusplus
}
#endif
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
