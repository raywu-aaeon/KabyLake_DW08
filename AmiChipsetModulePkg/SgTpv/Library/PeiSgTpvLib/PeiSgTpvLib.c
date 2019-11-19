//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//**********************************************************************
#include <Library/PeiSgTpvLib.h>

VOID
PcieGpioWrite(
  IN       UINT8                        GpioSupport,
  IN       UINT8                        Expander,
  IN       UINT32                       Gpio,
  IN       BOOLEAN                      Active,
  IN       BOOLEAN                      Level
)
{
  EFI_STATUS  Status;

  if (Active == 0) {
    Level = (~Level) & 0x1;
  }

  if (GpioSupport == PchGpio) {
    Status = GpioSetOutputValue(Gpio, (UINT32)Level);
      if (Status != EFI_SUCCESS) {
        return;
      }
  }
    ///
    ///
}

BOOLEAN 
SgTpvGpuCheck (
  UINT8          BusNum
)
{
  UINT8            Count;
  UINT16           VendorID;
  for (Count = 0; Count < 50; Count ++){
    VendorID = READ_PCI16(BusNum, 0x00, 0x00, 0x00);
    if(VendorID == 0xFFFF){
      return TRUE;
    }                              
  }
  return FALSE;
}

VOID
SgTpvN17PWorkaround (
  VOID       
  )
{
  UINT8            EndpointBus;
  UINTN            Index;
  UINT8            RootPortDev;
  UINT8            RootPortFun;
  BOOLEAN          SgTpvGpioSupport;
  PCH_SERIES       PchSeries;
  UINT16           VendorID;

  DEBUG ((DEBUG_INFO,  "SgTpvN17PWorkaround Start.\n"));
  PchSeries = GetPchSeries();
  if (PchSeries == PchH) {
    RootPortDev = SG_H_PORT_DEV;
    RootPortFun = SG_H_PORT_FUNC;
    if (SG_PCIE0_GPIO_SUPPORT || SG_PCIE1_GPIO_SUPPORT || SG_PCIE2_GPIO_SUPPORT){
      SgTpvGpioSupport = 1;
    }
  }else if (PchSeries == PchLp){
    RootPortDev = SG_ULT_PORT_DEV;
    RootPortFun = SG_ULT_PORT_FUNC;
    SgTpvGpioSupport = SG_PCIE0_GPIO_SUPPORT;
  }
  DEBUG ((DEBUG_INFO, " SgTpvN17PWorkaround::dGPU Rootport info[B/D/F] : [0x00/0x%x/0x%x]\n", RootPortDev, RootPortFun));  
  DEBUG ((DEBUG_INFO, " SgTpvN17PWorkaround::Rootport vendor ID = %x.\n", READ_PCI16 (0,RootPortDev, RootPortFun, PCI_VENDOR_ID_OFFSET)));  
  ///
  /// Get External Gfx BUS number
  ///
  EndpointBus = READ_PCI8(0,RootPortDev,RootPortFun, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
  ///
  /// If bus hasn't be assigned, assign 0x10 as temp bus to read end point.
  ///  
  if(EndpointBus == 0x00) {
    DEBUG ((DEBUG_INFO,  "SgTpvN17PWorkaround:: Assign temp bus.\n"));
    ///
    /// (Bus Numbers)  Subordinate(1A)  Secondary(19)  Primary(18)
    /// --------------------------------------------------------------
    ///                            01             01           00
    ///     
    WRITE_PCI32 (0, RootPortDev, RootPortFun, PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET, 0x00010100);
    EndpointBus = READ_PCI8 (0, RootPortDev, RootPortFun, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
    ///
    /// A config write is required in order for the device to re-capture the Bus number,
    /// according to PCI Express Base Specification, 2.2.6.2
    /// Write to a read-only register VendorID to not cause any side effects.
    ///    
    WRITE_PCI16 (EndpointBus, 0, 0, PCI_VENDOR_ID_OFFSET, 0);
    MicroSecondDelay (STALL_ONE_MILLI_SECOND); 
  }
  DEBUG ((DEBUG_INFO,  "SgTpvN17PWorkaround:: EndpointBus = 0x%x.\n",EndpointBus));
  ///
  /// Delay 50ms
  /// 
  MicroSecondDelay (50 * STALL_ONE_MILLI_SECOND); 

  ///
  /// To do GPU reset when detect GPU Vender ID is invalid.
  ///
  for (Index = 0; SgTpvGpuCheck(EndpointBus) && (Index < 10); Index ++){
    ///
    ///  Drive DGPU HOLD RST Enable to make sure we hold reset
    ///      
    PcieGpioWrite (SgTpvGpioSupport,
                   EXPANDER_dGPU_HOLD_RST,
                   GPIO_dGPU_HOLD_RST,
                   ACTIVE_dGPU_HOLD_RST,
                   GP_ENABLE
                   );
    ///
    /// Delay 100us
    ///
    MicroSecondDelay (100);
    ///
    /// Drive DGPU HOLD RST Disabled to remove reset
    ///
    PcieGpioWrite (SgTpvGpioSupport,
                   EXPANDER_dGPU_HOLD_RST,
                   GPIO_dGPU_HOLD_RST,
                   ACTIVE_dGPU_HOLD_RST,
                   GP_DISABLE
                   );
    ///
    /// Delay 50ms 
    ///     
    MicroSecondDelay (50 * STALL_ONE_MILLI_SECOND);
  }
  VendorID = READ_PCI16(EndpointBus, 0x00, 0x00, 0x00);
  DEBUG ((DEBUG_INFO,  "SgTpvN17PWorkaround:: SgTpvGpuCheck Vendor ID = %x.\n",VendorID));
  ///
  /// Clear assigned root port bus number
  ///
  if (EndpointBus == 0x01)
  {
    DEBUG ((DEBUG_INFO,  "SgTpvN17PWorkaround:: Clear temp bus.\n"));        
    WRITE_PCI32 (0, RootPortDev, RootPortFun, PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET, 0x00000000);
  }
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
