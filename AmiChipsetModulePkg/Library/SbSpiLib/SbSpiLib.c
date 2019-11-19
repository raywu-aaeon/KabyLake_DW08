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

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/MmPciLib.h>
#include <Library/DebugLib.h>
#include <PchAccess.h>
#include <SaRegs.h>
#include <Library/SbSpiLib.h>

// GSPI Register define
#define GSPI_SSCR0  0x0
#define SSCR0_SSE   BIT7

#define GSPI_SSCR1  0x4
#define SSCR1_SPO   BIT3
#define SSCR1_SPH   BIT4

#define GSPI_SSSR   0x8
#define GSPI_SSDR   0x10

// Global Variable definition
UINTN       Bar = 0;
UINT32      SSCR0, SSCR1;

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
) 
{
    UINT32      ClkReg;
    
    if (GspiBarAddress == 0) return EFI_UNSUPPORTED;
    if (MmioRead32(GspiBarAddress) == 0xFFFFFFFF) return EFI_UNSUPPORTED;
    
    if ((DivM > 0x7FFF) || (DivN > 0x7FFF)) return EFI_INVALID_PARAMETER;
    
    Bar = GspiBarAddress;    
    DEBUG((DEBUG_INFO, "GSPI Bar Address = 0x%lx\n", Bar));    
    
    MmioWrite32 (Bar + R_PCH_SERIAL_IO_PPR_CLK,
     (B_PCH_SERIAL_IO_PPR_CLK_UPDATE | (DivN << 16) | (DivM << 1) | B_PCH_SERIAL_IO_PPR_CLK_EN )
    );    
    ClkReg = MmioRead32 (Bar + R_PCH_SERIAL_IO_PPR_CLK);
    DEBUG((DEBUG_INFO, "GSPI ClkReg = 0x%x\n", ClkReg));    
    
    MmioOr32(Bar + R_PCH_SERIAL_IO_SPI_CS_CONTROL, BIT0);
    
    MmioWrite32 (Bar + GSPI_SSCR0, 0);
    MmioWrite32 (Bar + GSPI_SSCR1, 0);
    
    SSCR0 = 0x7;
    SSCR1 = 0;
    
    // CPOL
    if (flags & BIT0) {
        SSCR1 |= SSCR1_SPO;
    }
    // CPHA
    if (flags & BIT1) {
        SSCR1 |= SSCR1_SPH;
    }   
    
    return EFI_SUCCESS;
}

/**    
    This function disable SSP operation and clock.

    @param 
**/
VOID
GspiStop(VOID)
{
    if (Bar == 0) return;
    
    MmioWrite32 (Bar + GSPI_SSCR0, 0);
    MmioWrite32 (Bar + GSPI_SSCR1, 0);
    MmioWrite32 (Bar + R_PCH_SERIAL_IO_PPR_CLK, 0); 
    
    Bar = 0;
    SSCR0 = 0;
    SSCR1 = 0;
}

/**    
    This function Set the Chip Select pin state.

    @param State - Should be CS_HIGH or CS_LOW

**/
VOID
SetSpiCsState(
  UINT8 State
)
{
    UINT32      CsCtrlReg;
    
    if (Bar == 0) return;
    
    CsCtrlReg = MmioRead32 (Bar + R_PCH_SERIAL_IO_SPI_CS_CONTROL);
    if (State == CS_HIGH) {
        CsCtrlReg |= BIT1;
    }
    if (State == CS_LOW) {
        CsCtrlReg &= ~BIT1;
    }
    MmioWrite32(Bar + R_PCH_SERIAL_IO_SPI_CS_CONTROL, CsCtrlReg);
}

/**    
    This function output 1 byte from MOSI

    @param Value - output data
**/
VOID SpiWrite8 (
  IN    UINT8  Value
  )
{
  UINT32    Data = 0;
  UINT32    SSSR;
  
  if (Bar == 0) return;
  
  Data |= Value;
  
  MmioWrite32 (Bar + GSPI_SSCR0, (SSCR0 | SSCR0_SSE));
  MmioWrite32 (Bar + GSPI_SSCR1, SSCR1);
  MmioWrite32 (Bar + GSPI_SSDR, Data);
  
  SSSR = MmioRead32(Bar + GSPI_SSSR);
  while((SSSR & (BIT2|BIT3)) != 0xC) {
      SSSR = MmioRead32(Bar + GSPI_SSSR);
  };

  MmioRead32 (Bar + GSPI_SSDR);
}

/**    
    This function read 1 byte from MISO

    @retval  
**/
UINT8 
SpiRead8 (VOID)
{
  UINT32    Data, SSSR;
  
  if (Bar == 0) return 0;
  
  MmioWrite32 (Bar + GSPI_SSCR0, (SSCR0 | BIT7));   // SET SSCR0
  MmioWrite32 (Bar + GSPI_SSCR1, SSCR1);            // SET SSCR1  Rx
  MmioWrite32 (Bar + GSPI_SSDR, 0);                 // SET SSDR
  
  SSSR = MmioRead32(Bar + GSPI_SSSR);
  while((SSSR & (BIT2|BIT3)) != 0xC) {
      SSSR = MmioRead32(Bar + GSPI_SSSR);
  };

  Data = MmioRead32 (Bar + GSPI_SSDR);
  return (Data & 0xFF);
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
