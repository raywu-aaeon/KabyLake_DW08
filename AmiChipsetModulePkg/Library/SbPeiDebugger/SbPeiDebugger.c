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

/** @file SbPeiDebugger.c
    This file contains PEI stage debugger code for SB template

**/
// Files included from EDKII
#include <Token.h>
#include <Sb.h>
#include <AmiChipsetIoLib.h>
#include <AmiCspLib.h>
#include <PchAccess.h>
#include <PCI.h>
#include <AmiDebugPort.h>

// Definitions
#define PM_STATUS_CTRL      0x8000
#define DBG_PRT_CAP_ID      0xA
#define TIMER_CONTROL_PORT  0x43
#define TIMER0_COUNT_PORT   0x40

// Function Prototypes
VOID   Pei8259WriteMask (IN UINT16 Mask, IN UINT16 EdgeLevel);
VOID   Pei8259SetVectorBase (IN UINT8 MasterBase, IN UINT8 SlaveBase);
VOID   Program8254Timer0 (IN UINT16 Count);

EFI_STATUS SbPeiDbgInitUsb (
    IN OUT PEI_DBG_PORT_INFO    *DebugPort
);

#if USB_3_DEBUG_SUPPORT == 1
EFI_STATUS SbPeiDbgInitXhci (
    IN OUT PEI_DBG_PORT_INFO    *DebugPort
);
#endif

#ifdef  USB_DEBUGGER

/**
    This eLink function is used to initialize the EHCI controller
    debug port for USB PEI Debugging. It also fills the transport
    interface structure with appropriate information

    @param DebugPort Debug transport interface structure

    @retval EFI_STATUS

    @note  PORTING REQUIRED
**/
EFI_STATUS SbPeiDbgInitUsb (
    IN OUT PEI_DBG_PORT_INFO    *DebugPort
)
{
/*
  UINT8     PwrMgtCapReg;
  UINTN     PciAddress;
  UINT16    CmdReg;
  UINT8     DbgPrtCapReg;
  UINT16    DbgPrtBaseOffset;
  UINT32    EhciBaseAddress = 0;
  UINTN     PcieAddress;
#if defined(PEI_EHCI_MEM_BASE_ADDRESSES) && defined(PEI_EHCI_PCI_BDFS)
  UINT32    EhciBusDevFun[] = PEI_EHCI_PCI_BDFS;
  UINT32    EhciBaseAddr[] = PEI_EHCI_MEM_BASE_ADDRESSES;
  UINT32    i;
#endif

  if(DebugPort->UsbDebugPort.USBBaseAddr) {
    EhciBaseAddress = DebugPort->UsbDebugPort.USBBaseAddr;
  } else {
    EhciBaseAddress = EHCI_MMIO_BASE_ADDRESS1;
#if defined(PEI_EHCI_MEM_BASE_ADDRESSES) && defined(PEI_EHCI_PCI_BDFS)
    for (i = 0; i < (sizeof(EhciBusDevFun) / sizeof(UINT32)); i++) {
      if (EhciBusDevFun[i] == DBG_EHCI_PCI_ADDRESS) {
        EhciBaseAddress = EhciBaseAddr[i];
        break;
      }
    }
#endif
  }
  //
  // First disable the EHCI device
  //
  PciAddress = PCI_LIB_ADDRESS (
                  (DBG_EHCI_PCI_ADDRESS >> 24) & 0xFF,
                  (DBG_EHCI_PCI_ADDRESS >> 16) & 0x1F,
                  (DBG_EHCI_PCI_ADDRESS >>  8) & 0x07,
                  0
                  );
  PcieAddress = SB_PCIE_CFG_ADDRESS(
                  (DBG_EHCI_PCI_ADDRESS >> 24) & 0xFF,
                  (DBG_EHCI_PCI_ADDRESS >> 16) & 0x1F,
                  (DBG_EHCI_PCI_ADDRESS >>  8) & 0x07,
                  0
                  );
  
  CmdReg = PciRead16 (PciAddress + PCI_COMMAND_REGISTER_OFFSET);

  if(CmdReg & BIT1) {
    PciWrite16 (PciAddress + PCI_COMMAND_REGISTER_OFFSET, 0);
  }

  //
  // Assign base address register
  //
  PciWrite32 (PciAddress + 0x10, EhciBaseAddress);

  //
  // Set the Power state of the device to D0
  //
  PwrMgtCapReg = FindCapPtr (
		  PcieAddress,
                  PCI_CAP_ID_PMI
                  );

  PciWrite16 (PciAddress + PwrMgtCapReg + 4, PM_STATUS_CTRL);

  //
  // Enable PCH EHCI register & make it Bus master
  //
  CmdReg = CmdReg | 0x06;
  PciWrite16 (PciAddress + PCI_COMMAND_REGISTER_OFFSET, CmdReg);

  //
  // Locate the Debug port register Interface location
  //

  DbgPrtCapReg = FindCapPtr (
                  PcieAddress,
                  DBG_PRT_CAP_ID
                  );

  DbgPrtBaseOffset = PciRead16 (PciAddress + DbgPrtCapReg + 2) & 0x1fff;

  if (DebugPort->UsbDebugPort.USBBaseAddr == 0) {
    DebugPort->UsbDebugPort.USBBaseAddr = EhciBaseAddress;
    DebugPort->UsbDebugPort.MemoryMappedIoSpaceSize = EHCI_MMIO_SIZE;
  }
  DebugPort->UsbDebugPort.USBDebugPortStartAddr = EhciBaseAddress + DbgPrtBaseOffset;
  DebugPort->UsbDebugPort.PciBusNumber          = 0;
  DebugPort->UsbDebugPort.PciDeviceNumber       = DBG_EHCI_PCI_ADDRESS >> 16;
  DebugPort->UsbDebugPort.PciFunctionNumber     = 0;
  DebugPort->UsbDebugPort.PciBAROffset          = PCI_EHCI_BAR_OFFSET;
  DebugPort->UsbDebugPort.InitUSBEHCI           = SbPeiDbgInitUsb;
*/
  return EFI_SUCCESS;
}

#endif

#if USB_3_DEBUG_SUPPORT == 1
/**
    This eLink function is used to initialize the XHCI controller
    debug port for USB PEI Debugging. It also fills the transport
    interface structure with appropriate information

    @param DebugPort Debug transport interface structure

    @retval EFI_STATUS

    @note  PORTING REQUIRED
**/
EFI_STATUS SbPeiDbgInitXhci (
    IN OUT PEI_DBG_PORT_INFO    *DebugPort
)
{
    UINT8     PwrMgtCapReg;
    UINTN     PciAddress;
    UINT16    CmdReg;
    UINT32    XhciBaseAddress = 0;

    if(DebugPort->UsbDebugPort.USBBaseAddr) {
      XhciBaseAddress = DebugPort->UsbDebugPort.USBBaseAddr;
    } else {
      XhciBaseAddress = XHCI_MMIO_BASE_ADDRESS;
    }
    
    //
    // First disable the XHCI device
    //
    PciAddress = CSP_PCIE_CFG_ADDRESS (
                  DEBUG_XHCI_BUS_NUMBER,
                  DEBUG_XHCI_DEVICE_NUMBER,
                  DEBUG_XHCI_FUNCTION_NUMBER,
                  0
                  );

    CmdReg = READ_PCI16(DEBUG_XHCI_BUS_NUMBER, \
                        DEBUG_XHCI_DEVICE_NUMBER, \
                        DEBUG_XHCI_FUNCTION_NUMBER, \
                        PCI_COMMAND_REGISTER_OFFSET \
                        );

    if(CmdReg & 2) {
      WRITE_PCI16(DEBUG_XHCI_BUS_NUMBER, \
                  DEBUG_XHCI_DEVICE_NUMBER, \
                  DEBUG_XHCI_FUNCTION_NUMBER, \
                  PCI_COMMAND_REGISTER_OFFSET, \
                  0 \
                  );
    }

    //
    // Assign base address register
    //
    WRITE_PCI32(DEBUG_XHCI_BUS_NUMBER, \
                DEBUG_XHCI_DEVICE_NUMBER, \
                DEBUG_XHCI_FUNCTION_NUMBER, \
                0x10, \
                XhciBaseAddress \
                );

    //
    // Set the Power state of the device to D0
    //
    PwrMgtCapReg = FindCapPtr (
                    PciAddress,
                    PCI_CAP_ID_PMI
                    );

    WRITE_PCI16(DEBUG_XHCI_BUS_NUMBER, \
                DEBUG_XHCI_DEVICE_NUMBER, \
                DEBUG_XHCI_FUNCTION_NUMBER, \
                PwrMgtCapReg + 4, \
                PM_STATUS_CTRL \
                );

    //
    // Enable PCH XHCI register & make it Bus master
    //
    CmdReg = CmdReg | 0x06;
    WRITE_PCI16(DEBUG_XHCI_BUS_NUMBER, \
                DEBUG_XHCI_DEVICE_NUMBER, \
                DEBUG_XHCI_FUNCTION_NUMBER, \
                PCI_COMMAND_REGISTER_OFFSET, \
                CmdReg \
                );

    if (DebugPort->UsbDebugPort.USBBaseAddr == 0) {
      DebugPort->UsbDebugPort.USBBaseAddr = XhciBaseAddress;
    }
    DebugPort->UsbDebugPort.PciBusNumber          = DEBUG_XHCI_BUS_NUMBER;
    DebugPort->UsbDebugPort.PciDeviceNumber       = DEBUG_XHCI_DEVICE_NUMBER;
    DebugPort->UsbDebugPort.PciFunctionNumber     = DEBUG_XHCI_FUNCTION_NUMBER;
    DebugPort->UsbDebugPort.InitUSBEHCI           = SbPeiDbgInitXhci;

    return EFI_SUCCESS;
}
#endif

/**
    This function programs the SB register to enable the serial IRQ

    @param VOID

    @retval VOID

    @note  PORTING REQUIRED
**/
VOID EnableSerialIrq (
    VOID
)
{
  // Enable Serial IRQ, set it to continuous mode and set
  // Start Frame Pulse Width to 4 clocks
  WRITE_PCI8_SB (R_PCH_LPC_SERIRQ_CNT, \
		  B_PCH_LPC_SERIRQ_CNT_SIRQEN | \
		  B_PCH_LPC_SERIRQ_CNT_SIRQMD | \
		  V_PCH_LPC_SERIRQ_CNT_SFPW_4CLK
		  );
}

/**
    This function programs the SB register to enable the LPC IO
    Decoding ranges to enable the programming of SIO and Serial Port.

    @param VOID

    @retval VOID

    @note  PORTING REQUIRED
**/
VOID EnableLpcIoDecode (
    VOID
)
{
    UINT16  LpcComPortEn   = B_PCH_LPC_IOE_CAE;
    UINT16  LpcComPortAddr = 0;
    
    // Enable COMA decode
    switch (DEBUG_COM_PORT_ADDR)
    {
        case 0x2F8:
            LpcComPortAddr = V_PCH_LPC_IOD_COMA_2F8;
            break;
        case 0x220:
            LpcComPortAddr = V_PCH_LPC_IOD_COMA_220;
            break;
        case 0x228:
            LpcComPortAddr = V_PCH_LPC_IOD_COMA_228;
            break;
        case 0x238:
            LpcComPortAddr = V_PCH_LPC_IOD_COMA_238;
            break;
        case 0x2E8:
            LpcComPortAddr = V_PCH_LPC_IOD_COMA_2E8;
            break;
        case 0x338:
            LpcComPortAddr = V_PCH_LPC_IOD_COMA_338;
            break;
        case 0x3E8:
            LpcComPortAddr = V_PCH_LPC_IOD_COMA_3E8;
            break;
        default: // 0x3F8
            LpcComPortAddr = V_PCH_LPC_IOD_COMA_3F8;
    }
    
    RW_PCI16_SB(R_PCH_LPC_IOD, LpcComPortAddr, B_PCH_LPC_IOD_COMA); // 0x80
    //
    // program LPC I/O Decode Ranges, PCR[DMI] + 2770h[15:0] to the same value programmed in LPC/eSPI PCI offset 80h.
    //
    MmioAndThenOr16(SB_PCR_ADDRESS(PID_DMI, R_PCH_PCR_DMI_LPCIOD), ~B_PCH_LPC_IOD_COMA, LpcComPortAddr);
    //PchPcrWrite16 (PID_DMI, R_PCH_PCR_DMI_LPCIOD, LpcIoDecodeRanges);
    
    RW_PCI16_SB(R_PCH_LPC_IOE, LpcComPortEn, B_PCH_LPC_IOE_CAE); // 0x82
    //
    // program LPC I/O Decode Ranges, PCR[DMI] + 2774h[15:0] to the same value programmed in LPC/eSPI PCI offset 82h.
    //
    MmioAndThenOr16(SB_PCR_ADDRESS(PID_DMI, R_PCH_PCR_DMI_LPCIOE), ~B_PCH_LPC_IOE_CAE, LpcComPortEn);
    //PchPcrWrite16 (PID_DMI, R_PCH_PCR_DMI_LPCIOE, LpcIoEnableDecoding);
}

/**
    This function programs the SB register to disable
    the TCO timer. If this timer is not disabled the system
    will shutdown or reset as soon as the timer is expired

    @param VOID

    @retval VOID

    @note  PORTING REQUIRED
**/

VOID DisableTco (
    VOID
)
{
    UINT32        TcoBase;
    UINT16        RegVal;
    
    // Make sure TCO BASE has been programmed
    TcoBase = READ_PCI32(SB_SMBUS_BUS, SB_SMBUS_DEV, SB_SMBUS_FUN, R_SB_SMBUS_TCOBASE) & B_SB_SMBUS_TCOBASE_BAR;
    if(TcoBase == 0){
        /*
           Set PCH TCO base address.
           This cycle decoding is allowed to set when DMIC.SRL is 0.
           Programming steps:
           1. reset Smbus PCI offset 54h [8] to disable TCO base address.
           2. program Smbus PCI offset 50h [15:5] to TCO base address.
           3. set Smbus PCI offset 54h [8] to enable TCO base address.
           4. program "TCO Base Address" PCR[DMI] + 2778h[15:5, 1] to [Smbus PCI offset 50h[15:5], 1].
         */
        TcoBase = TCO_BASE_ADDRESS;
        
        // Disable TCO in SMBUS Device first before changing base address.
        RegVal = READ_PCI16(SB_SMBUS_BUS, SB_SMBUS_DEV, SB_SMBUS_FUN, R_SB_SMBUS_TCOCTL);
        
        if(RegVal & B_SB_SMBUS_TCOCTL_TCO_BASE_LOCK) return;
        else RegVal &= (UINT16)~B_SB_SMBUS_TCOCTL_TCO_BASE_EN;
        
        WRITE_PCI16(SB_SMBUS_BUS, SB_SMBUS_DEV, SB_SMBUS_FUN, R_SB_SMBUS_TCOCTL, RegVal);
        
        // Program TCO in SMBUS Device
        WRITE_PCI16(SB_SMBUS_BUS, SB_SMBUS_DEV, SB_SMBUS_FUN, R_SB_SMBUS_TCOBASE, TcoBase);
        
        // Enable TCO in SMBUS Device
        RegVal = READ_PCI16(SB_SMBUS_BUS, SB_SMBUS_DEV, SB_SMBUS_FUN, R_SB_SMBUS_TCOCTL);
        RegVal |= B_SB_SMBUS_TCOCTL_TCO_BASE_EN;
        WRITE_PCI16(SB_SMBUS_BUS, SB_SMBUS_DEV, SB_SMBUS_FUN, R_SB_SMBUS_TCOCTL, RegVal);
        
        // Program "TCO Base Address" PCR[DMI] + 2778h[15:5, 1] to [SMBUS PCI offset 50h[15:5], 1].
        MMIO_WRITE16(SB_PCR_ADDRESS(PID_DMI, R_PCH_PCR_DMI_TCOBASE), (TcoBase | BIT1));
        //PchPcrWrite16 (PID_DMI, R_PCH_PCR_DMI_TCOBASE, (TcoBase | BIT1));
    }
    
    RegVal = IoRead16 (TcoBase + R_PCH_TCO1_CNT);
    RegVal |= B_PCH_TCO_CNT_TMR_HLT;
    IoWrite16 (TcoBase + R_PCH_TCO1_CNT, RegVal);
    
    RegVal = IoRead16 (TcoBase + R_PCH_TCO2_STS);
    RegVal |= B_PCH_TCO2_STS_SECOND_TO;
    IoWrite16 (TcoBase + R_PCH_TCO2_STS, RegVal);
}

/**
    This function programs the onboard clock generator.

    @param VOID

    @retval VOID

    @note  PORTING REQUIRED if needed.
**/

VOID ProgramClockGenerator (VOID)
{

}



/**
    This eLink function is used to initialize the South Bridge
    for PEI Debugger support

    @param DebugPort Debug transport interface structure

    @retval EFI_STATUS

    @note  Normally no PORTING REQUIRED
**/

EFI_STATUS SbPeiDbgInitialize (
    IN OUT PEI_DBG_PORT_INFO    *DebugPort
)
{
    //
    // Program the TCO IO to avoid rebooting of the hardware
    //
    DisableTco();

    //
    // Programm Clock Generator
    //
    ProgramClockGenerator();

    //
    // Init 8259 Controller
    //
    Pei8259SetVectorBase (LEGACY_MODE_BASE_VECTOR_MASTER, LEGACY_MODE_BASE_VECTOR_SLAVE );

    //
    // Set all 8259 Interrupts to edge triggered and disabled
    //
    Pei8259WriteMask (0xffff , 0x0000);

    EnableLpcIoDecode ();

    EnableSerialIrq ();

    Program8254Timer0 (0);

    return EFI_SUCCESS;
}


/**
    This function searches the PCI address space for the PCI
    device specified for a particular capability ID and returns
    the offset in the PCI address space if one found

    @param Bus      PCI Bus number
    @param DevFunc  PCI Device and function number
    @param CapId    Capability ID to look for

    @retval Capability ID location if one found
        Otherwise returns 0
**/

UINT8
FindCapPtrDbg(
 IN UINT8 Bus,
 IN UINT8 DevFunc,
 IN UINT8 CapId
)
{
  UINTN     PcieAddress;
  UINT8     Dev;
  UINT8     Func;

  Dev = (DevFunc >> 3) & 0x1F;
  Func = DevFunc & 0x07;
  PcieAddress = CSP_PCIE_CFG_ADDRESS(Bus, Dev, Func, 0);
  return FindCapPtr (PcieAddress, CapId);
}


/**
    Writes PC 8259 interrupt Controller mask register

    @param Mask Mask to write
    @param EdgeLevel Edge/level trigger to be programmed

    @retval VOID

**/
VOID
Pei8259WriteMask (
    IN UINT16 Mask, 
    IN UINT16 EdgeLevel)
{
    IoWrite8 (LEGACY_8259_MASK_REGISTER_MASTER, (UINT8)Mask);
    IoWrite8 (LEGACY_8259_MASK_REGISTER_SLAVE,  (UINT8)(Mask >> 8));
    IoWrite8 (LEGACY_8259_EDGE_LEVEL_TRIGGERED_REGISTER_MASTER, (UINT8)EdgeLevel);
    IoWrite8 (LEGACY_8259_EDGE_LEVEL_TRIGGERED_REGISTER_SLAVE,  (UINT8)(EdgeLevel >> 8));
}


/**
    Set up the 8259 interrupt controller master and slave base values

    @param MasterBase Master base to be programmed
    @param SlaveBase Slave base to be programmed

    @retval VOID

**/
VOID
Pei8259SetVectorBase (
    IN UINT8 MasterBase, 
    IN UINT8 SlaveBase
)
{
    UINT8           Mask;
    
    // Initialize Slave interrupt controller.
    Mask = IoRead8 (LEGACY_8259_MASK_REGISTER_SLAVE);
    IoWrite8 (LEGACY_8259_CONTROL_REGISTER_SLAVE, 0x11);
    IoWrite8 (LEGACY_8259_MASK_REGISTER_SLAVE,    SlaveBase);
    IoWrite8 (LEGACY_8259_MASK_REGISTER_SLAVE,    0x02); 
    IoWrite8 (LEGACY_8259_MASK_REGISTER_SLAVE,    0x01);
    IoWrite8 (LEGACY_8259_MASK_REGISTER_SLAVE,    Mask);

    // Initialize Master interrupt controller.
    Mask = IoRead8 (LEGACY_8259_MASK_REGISTER_MASTER);
    IoWrite8 (LEGACY_8259_CONTROL_REGISTER_MASTER, 0x11);
    IoWrite8 (LEGACY_8259_MASK_REGISTER_MASTER,    MasterBase);
    IoWrite8 (LEGACY_8259_MASK_REGISTER_MASTER,    0x04);
    IoWrite8 (LEGACY_8259_MASK_REGISTER_MASTER,    0x01); 
    IoWrite8 (LEGACY_8259_MASK_REGISTER_MASTER,    Mask); 

    IoWrite8 (LEGACY_8259_CONTROL_REGISTER_SLAVE,  LEGACY_8259_EOI);
    IoWrite8 (LEGACY_8259_CONTROL_REGISTER_MASTER, LEGACY_8259_EOI);
}

/**
    Programs the 8254 Timer channel 0

    @param Count Timer tick count

    @retval VOID

**/
VOID Program8254Timer0(
    IN UINT16 Count
)
{
    UINT8   LCntbyte,HCntbyte;

    LCntbyte = (UINT8)Count;
    HCntbyte = (UINT8)(Count >> 8);

    //Write the timer control port to select timer 0 and set to mode 3
    IoWrite8(TIMER_CONTROL_PORT, 0x36);

    //Write the Counter 0 with initial count value
    IoWrite8(TIMER0_COUNT_PORT, LCntbyte);
    IoWrite8(TIMER0_COUNT_PORT, HCntbyte);

    //Enable the IRQ0 interrupt for this timer 0 in USB Xport module
    //......
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
