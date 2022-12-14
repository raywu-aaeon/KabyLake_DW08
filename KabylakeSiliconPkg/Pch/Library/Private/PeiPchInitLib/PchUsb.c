/** @file
  Initializes PCH USB Controllers.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2014 - 2019 Intel Corporation.

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Intel Corporation or its suppliers or
  licensors. Title to the Material remains with Intel Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary and
  confidential information of Intel Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Intel in writing.

  Unless otherwise agreed by Intel in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Intel or
  Intel's suppliers or licensors in any way.

  This file contains an 'Intel Peripheral Driver' and is uniquely identified as
  "Intel Reference Module" and is licensed for Intel CPUs and chipsets under
  the terms of your license agreement with Intel or your vendor. This file may
  be modified by the user, subject to additional terms of the license agreement.

@par Specification Reference:
**/
#include "PchUsb.h"
#include <Ppi/Spi.h>
#include <Library/PostCodeLib.h>

//AMI_OVERRIDE_PCH0014_START >>> EIP213804
#include <Ppi/Wdt.h>

VOID
ResetPdo (
  IN  CONST PCH_USB_CONFIG        *UsbConfig,
  IN  UINTN                       XhciMmioBase
  )
{
  EFI_STATUS      Status;
  BOOLEAN         ResetRequired = FALSE;
  UINT32          Index;
  UINT32          XhciUsb2Pdo;
  UINT32          XhciUsb3Pdo;
  PCH_SERIES      PchSeries;
  EFI_BOOT_MODE   BootMode;
  WDT_PPI         *gWdtPei;

  Status = PeiServicesGetBootMode (&BootMode);
  if (!EFI_ERROR (Status) && (BootMode == BOOT_ON_S3_RESUME || BootMode == BOOT_ON_FLASH_UPDATE || BootMode == BOOT_IN_RECOVERY_MODE ))
      return;
  
  PchSeries = GetPchSeries ();

  ///
  /// XHCI PDO for HS
  ///
  if (PchSeries == PchLp) {
    XhciUsb2Pdo = MmioRead32 (XhciMmioBase + R_PCH_XHCI_USB2PDO) & B_PCH_XHCI_LP_USB2PDO_MASK;
  } else {
    XhciUsb2Pdo = MmioRead32 (XhciMmioBase + R_PCH_XHCI_USB2PDO) & B_PCH_XHCI_H_USB2PDO_MASK;
  }
  for (Index = 0; Index < GetPchUsbMaxPhysicalPortNum (); Index++) {
    if (XhciUsb2Pdo & (UINT32) (B_PCH_XHCI_USB2PDO_DIS_PORT0 << Index)) //Port is Disabled
    {
        ResetRequired = TRUE;
        goto DoReset;
    }
  }
  ///
  /// XHCI PDO for SS
  ///
  if (PchSeries == PchLp) {
    XhciUsb3Pdo = MmioRead32 (XhciMmioBase + R_PCH_XHCI_USB3PDO) & B_PCH_XHCI_LP_USB3PDO_MASK;
  } else {
    XhciUsb3Pdo = MmioRead32 (XhciMmioBase + R_PCH_XHCI_USB3PDO) & B_PCH_XHCI_H_USB3PDO_MASK;
  }  
  for (Index = 0; Index < GetPchXhciMaxUsb3PortNum (); Index++){
    if (XhciUsb3Pdo & (UINT32) (B_PCH_XHCI_USB3PDO_DIS_PORT0 << Index)) //Port is Disabled
    {
        ResetRequired = TRUE;
        goto DoReset;
    }
  }

DoReset:
  if (ResetRequired) {
      Status = PeiServicesLocatePpi (
                 &gWdtPpiGuid,
                 0,
                 NULL,
                 (VOID **) &gWdtPei
                 );
      
      if (!EFI_ERROR(Status)) {      
          
          gWdtPei->AllowKnownReset();
      }
      
      IoWrite8(0xCF9,0x06);
  }
}
//AMI_OVERRIDE_PCH0014_END <<<
/**
  Configures PCH xHCI device

  @param[in]  SiPolicy            The SI Policy PPI instance
  @param[in]  TempMemBaseAddr     Temporary Memory Base Address for PCI
                                  devices to be used to initialize MMIO
                                  registers.

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver
**/
EFI_STATUS
EFIAPI
ConfigureXhci (
  IN  SI_POLICY_PPI            *SiPolicy,
  IN  UINT32                    TempMemBaseAddr
  )
{
  EFI_STATUS         Status;
  UINTN              XhciPciMmBase;
  PCH_SERIES         PchSeries;
  PCH_USB_CONFIG     *UsbConfig;
  UINT32             XhciMmioBase;
  UINT32             Usb2DisabledPorts;
  UINT32             Usb3DisabledPorts;
  BOOLEAN            XhciPdoRestartNeeded;
  UINT32             Index;
  EFI_BOOT_MODE      BootMode;

  DEBUG ((DEBUG_INFO, "ConfigureXhci() - Start\n"));

  Status = GetConfigBlock ((VOID *) SiPolicy, &gUsbConfigGuid, (VOID *) &UsbConfig);
  ASSERT_EFI_ERROR (Status);

  XhciMmioBase  = TempMemBaseAddr;
  PchSeries     = GetPchSeries ();

  ///
  /// Enable Mipi PLL
  ///
  if (PchSeries == PchH) {
    PchPcrAndThenOr32 (
      PID_MODPHY0,
      R_PCH_HSIO_CLANE0_CMN_ANA_DWORD2,
      (UINT32) ~(B_PCH_HSIO_CLANE0_CMN_ANA_DWORD2_O_DTPLL1_lC_PLLEN_H_OVRDEN | B_PCH_HSIO_CLANE0_CMN_ANA_DWORD2_O_DTPLL1_lC_FULLCALRESET_L_OVERDEN),
      (UINT32) 0);
  }

  XhciPciMmBase   = MmPciBase (
                      DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_XHCI,
                      PCI_FUNCTION_NUMBER_PCH_XHCI
                      );

  ///
  /// Assign memory resources
  ///
  XhciMemorySpaceOpen (
    UsbConfig,
    XhciMmioBase,
    XhciPciMmBase
    );

  ///
  /// Disable Compliance Mode
  /// It must be done before clock gating is configured.
  ///
  if (UsbConfig->DisableComplianceMode == TRUE) {
    MmioOr32 (
      XhciMmioBase + R_PCH_XHCI_HOST_CTRL_PORT_LINK_REG,
      (UINT32) (BIT0)
      );
  }

  // Need to check if by any chance current flow is resume from S3
  PeiServicesGetBootMode (&BootMode);

  ///
  /// Program Xhci Port Disable Override.
  /// Need to set values here in case of resume from DeepS3 due to PDO regs values are lost after
  ///
  if ((UsbConfig->DelayPdoProgramming == FALSE) || (BootMode == BOOT_ON_S3_RESUME)) {
    DEBUG ((DEBUG_INFO, "xHCI: PEI phase PDO programming\n"));

    Usb2DisabledPorts = 0;
    for (Index = 0; Index < GetPchUsbMaxPhysicalPortNum (); Index++) {
      if (UsbConfig->PortUsb20[Index].Enable == FALSE) {
        Usb2DisabledPorts |= (BIT0 << Index);
      }
    }
    Usb3DisabledPorts = 0;
    for (Index = 0; Index < GetPchXhciMaxUsb3PortNum (); Index++) {
      if (UsbConfig->PortUsb30[Index].Enable == FALSE) {
        Usb3DisabledPorts |= (BIT0 << Index);
      }
    }

    XhciPdoRestartNeeded = UsbPdoProgramming (
                             XhciMmioBase,
                             Usb2DisabledPorts,
                             Usb3DisabledPorts
                             );
    // If PDO register is locked, reset platform to unlock it
    if (XhciPdoRestartNeeded) {
      (*GetPeiServicesTablePointer ())->ResetSystem2 (EfiResetWarm, EFI_SUCCESS, 0, NULL);
    }
  }
//AMI_OVERRIDE_PCH0014_START >>> EIP213804
  else ResetPdo (UsbConfig, XhciMmioBase);
//AMI_OVERRIDE_PCH0014_END <<< EIP213804

  XhciHcInit (
    SiPolicy,
    XhciMmioBase,
    XhciPciMmBase
    );

  PostCode (0xB06);
  #ifndef MINTREE_FLAG
  XhciSsicInit (
    SiPolicy,
    XhciMmioBase,
    XhciPciMmBase
    );
  #endif //MINTREE_FLAG

  ///
  /// Setup USB Over-Current Mapping.
  ///
  XhciOverCurrentMapping (
    UsbConfig,
    XhciPciMmBase
    );

  ///
  /// Clear memory resources
  ///
  XhciMemorySpaceClose (
    UsbConfig,
    XhciMmioBase,
    XhciPciMmBase
    );

  ///
  /// Disable Mipi PLL
  ///
  if (PchSeries == PchH) {
    PchPcrAndThenOr32 (
      PID_MODPHY0,
      R_PCH_HSIO_CLANE0_CMN_ANA_DWORD2,
      (UINT32) ~0,
      (UINT32) (B_PCH_HSIO_CLANE0_CMN_ANA_DWORD2_O_DTPLL1_lC_PLLEN_H_OVRDEN | B_PCH_HSIO_CLANE0_CMN_ANA_DWORD2_O_DTPLL1_lC_FULLCALRESET_L_OVERDEN));
  }

  DEBUG ((DEBUG_INFO, "ConfigureXhci() - End\n"));

  return EFI_SUCCESS;
}

/**
  Performs basic configuration of PCH USB3 (xHCI) controller.

  @param[in] SiPolicy             The SI Policy PPI
  @param[in] XhciMmioBase         Memory base address of xHCI Controller
  @param[in] XhciPciMmBase        XHCI PCI Base Address

  @retval None
**/
VOID
XhciHcInit (
  IN  SI_POLICY_PPI               *SiPolicy,
  IN  UINT32                      XhciMmioBase,
  IN  UINTN                       XhciPciMmBase
  )
{
  EFI_STATUS      Status;
  UINT32          Data32;
  UINT32          Data32Or;
  UINT32          Data32And;
  PCH_SERIES      PchSeries;
  PCH_STEPPING    PchStep;
  PCH_GENERATION  PchGen;
  UINT32          HsPortCount;
  UINT32          SsPortCount;
  UINTN           PortSCxUsb3Base;
  EFI_BOOT_MODE   BootMode;
  BOOLEAN         DebugEnable;
  UINT32          DebugPort;
  UINT32          DebugPortSsIndex;
  UINT32          CapabilityPointer;
  UINT32          Capability;
  UINT32          UsbPort;
  UINT32          XhciUsb3Pdo;

  PchSeries   = GetPchSeries ();
  PchStep     = PchStepping ();
  PchGen      = GetPchGeneration ();
  HsPortCount = 0;
  SsPortCount = 0;

  DEBUG ((DEBUG_INFO, "Before XhciHcInit()\n"));
  DEBUG ((DEBUG_INFO, "Xhci Mmio Base = 0x%x\n", XhciMmioBase));
  DEBUG ((DEBUG_INFO, "Xhci Mmio Base + 0x0000 (Register 0x0000 to 0x001F) = \n"));
  PrintRegisters (XhciMmioBase + 0x0000, 0x20, 0);
  DEBUG ((DEBUG_INFO, "Xhci Mmio Base + 0x0080 (Register 0x0080 to 0x00BF) = \n"));
  PrintRegisters (XhciMmioBase + 0x0080, 0x40, 0x0080);
  DEBUG ((DEBUG_INFO, "Xhci Mmio Base + 0x0480 (Register 0x0480 to 0x05CF) = \n"));
  PrintRegisters (XhciMmioBase + 0x0480, 0x150,0x0480);
  DEBUG ((DEBUG_INFO, "Xhci Mmio Base + 0x8000 (Register 0x8000 to 0x833F) = \n"));
  PrintRegisters (XhciMmioBase + 0x8000, 0x340,0x8000);

  HsPortCount = (MmioRead32 (XhciMmioBase + R_PCH_XHCI_XECP_SUPP_USB2_2) & 0x0000FF00) >> 8;
  SsPortCount = (MmioRead32 (XhciMmioBase + R_PCH_XHCI_XECP_SUPP_USB3_2) & 0x0000FF00) >> 8;

  DEBUG ((DEBUG_INFO, "Max number of Super Speed Ports  = %d\n", SsPortCount));
  DEBUG ((DEBUG_INFO, "Max number of High Speed Ports   = %d\n", HsPortCount));

  //
  // Sx WA Start
  //

  //
  //  XHCC1 - XHC System Bus Configuration 1
  //  Address Offset: 40-43h
  //  Value: [21:19] 110b, [18] 1b, [8] 1b
  //  Writes to this registers needs to be performed per bytes to avoid touching bit 31
  //  Bit 31 is used to lock RW/L bits and can be writen once.
  //
  MmioOr8 (
    XhciPciMmBase + R_PCH_XHCI_XHCC1 + 1,
    (UINT32) (BIT0)
    );
  MmioOr8 (
    XhciPciMmBase + R_PCH_XHCI_XHCC1 + 2,
    (UINT32) (BIT5 | BIT4 | BIT2)
    );

  //
  //  XHCC2 - XHC System Bus Configuration 2
  //  Address Offset: 44-47h
  //  Value: [19:14] 111111b, [10] 1b, [9:8] 10b, [7:6] 10b, [5:3] 001b, [2:0] 000b
  //
  MmioOr32 (
    XhciPciMmBase + R_PCH_XHCI_XHCC2,
    (UINT32) (BIT19 | BIT18 | BIT17 | BIT16 | BIT15 | BIT14 | BIT10 | BIT9 | BIT7 | BIT3)
    );

  //
  //  XHCLKGTEN - Clock Gating
  //  Address Offset: 50-53h
  //  Value: 0x0FCE6E5F
  //
  Data32 = 0x0FCE6E5F;
  MmioWrite32 (
    XhciPciMmBase + R_PCH_XHCI_XHCLKGTEN,
    Data32
    );
  #ifndef MINTREE_FLAG
  if (PchSeries == PchLp) {
    //
    //  AUX_CTRL_REG - AUX Reset Control
    //  Address Offset: 80C0-80C3h
    //  Value: [10] 0b
    //
    MmioAnd32 (
      XhciMmioBase + R_PCH_XHCI_AUX_CTRL_REG,
      (UINT32) ~(BIT10)
      );
  }
  #endif //MINTREE_FLAG
  //
  // Put XHCI in D3
  //
  MmioOr16 (XhciPciMmBase + R_PCH_XHCI_PWR_CNTL_STS, (UINT16) V_PCH_XHCI_PWR_CNTL_STS_PWR_STS_D3);

  //
  // Delay of 20us (this is to allow xHCI power gating).
  //
  MicroSecondDelay (20);

  //
  // Put XHCI in D0
  //
  MmioAnd8 (XhciPciMmBase + R_PCH_XHCI_PWR_CNTL_STS, (UINT8) ~(V_PCH_XHCI_PWR_CNTL_STS_PWR_STS_D3));

  //
  // Sx WA End
  //

  //
  //  XHCC1 - XHC System Bus Configuration 1
  //  Address Offset: 40-43h
  //  Value: [21:19] 110b, [18] 1b, [8] 1b
  //  Writes to this registers needs to be performed per bytes to avoid touching bit 31
  //  Bit 31 is used to lock RW/L bits and can be writen once.
  //
  MmioOr8 (
    XhciPciMmBase + R_PCH_XHCI_XHCC1 + 1,
    (UINT32) (BIT0)
    );
  MmioOr8 (
    XhciPciMmBase + R_PCH_XHCI_XHCC1 + 2,
    (UINT32) (BIT5 | BIT4 | BIT2)
    );

  //
  //  XHCC2 - XHC System Bus Configuration 2
  //  Address Offset: 44-47h
  //  Value: [19:14] 111111b, [10] 1b, [9:8] 10b, [7:6] 10b, [5:3] 001b, [2:0] 000b
  //
  MmioOr32 (
    XhciPciMmBase + R_PCH_XHCI_XHCC2,
    (UINT32) (BIT19 | BIT18 | BIT17 | BIT16 | BIT15 | BIT14 | BIT10 | BIT9 | BIT7 | BIT3)
    );

  //
  //  XHCLKGTEN - Clock Gating
  //  Address Offset: 50-53h
  //  Value: 0x0FCE6E5B
  //
  Data32 = 0x0FCE6E5B;
  MmioWrite32 (
    XhciPciMmBase + R_PCH_XHCI_XHCLKGTEN,
    Data32
    );

  ///
  /// The value in this field reflects the highest numbered port
  /// in the controller,not the actual count of the number of ports.
  /// This allows for gaps in the port numbering,
  /// between USB2 and USB3 protocol capabilities.
  ///
  Data32Or = (((MmioRead32 (XhciMmioBase + R_PCH_XHCI_XECP_SUPP_USB3_2) & 0x000000FF) + SsPortCount) - 1) << 24;
  MmioAndThenOr32 (
    (XhciMmioBase + R_PCH_XHCI_HCSPARAMS1),
    (UINT32) 0x00FFFFFF,
    (UINT32) Data32Or
    );
  #ifndef MINTREE_FLAG
  //
  //  PCE - Power Control Enables
  //  Address Offset:   A2h - A3h
  //  Value: [3] 0b, [2] 0b,
  //
  MmioAnd16 (
    XhciPciMmBase + R_PCH_XHCI_PCE,
    (UINT16)~(BIT3 | BIT2)
    );
  #endif //MINTREE_FLAG
  //
  //  HSCFG1 - High Speed Configuration 1
  //  Address Offset: AC-AFh
  //  Value: [18] 1b
  //
  MmioOr32 (
    XhciPciMmBase + R_PCH_XHCI_HSCFG1,
    (UINT32) (BIT18)
    );

  //
  //  HSCFG2 - High Speed Configuration 2
  //  Address Offset: A4-A7h
  //  [13] 0b
  //  [12:11] 11b
  //
  MmioAndThenOr32 (
    XhciPciMmBase + R_PCH_XHCI_HSCFG2,
    (UINT32) ~(BIT13),
    (UINT32) (BIT12 | BIT11)
    );

  //
  //  SSCFG1 - SuperSpeed Configuration 1
  //  Address Offset: A8 - ABh
  //  Value: [17] 1b, [14] 1b
  //
  MmioOr32 (
    XhciPciMmBase + R_PCH_XHCI_SSCFG1,
    (UINT32) (BIT17 | BIT14)
    );

  //
  //  MMIO Space
  //

  //
  //  HCSPARAMS3 - Structural Parameters 3
  //  Address Offset:   0x0C - 0x0F
  //  Set xHCIBAR + 0Ch[7:0] = 0Ah and [31:16] = 200h
  //
  MmioAndThenOr32 (
    (XhciMmioBase + R_PCH_XHCI_HCSPARAMS3),
    (UINT32) 0x0000FF00,
    (UINT32) 0x0200000A
    );

  //
  //  XECP_SUPP_USB2_2 - USB2 Supported Protocol Capability 2
  //  Address Offset: 0x8008 - 0x800B
  //  Value: [19] 0b
  //
  MmioAnd32 (
    XhciMmioBase + R_PCH_XHCI_XECP_SUPP_USB2_2,
    (UINT32) ~(BIT19)
    );

  //
  //  PMCTRL - Power Management Control
  //  Address Offset: 80A4-80A7h
  //  Value: [30] 1b, [27] 1b [24] 1b, [23] 1b, [16] 0b, [7:4] 1001b, [3] 1b, [2] 1b
  //
  MmioAndThenOr32 (
    (XhciMmioBase + R_PCH_XHCI_PMCTRL),
    (UINT32) ~(BIT16),
    (UINT32) (BIT30 | BIT27 | BIT24 | BIT23 | BIT7 | BIT4 | BIT3 | BIT2)
    );

  //
  //  PGCBCTRL - PGCB Control
  //  Address Offset: 80A8-80ABh
  //  Value: [24] 1b
  //
  MmioOr32 (
    (XhciMmioBase + R_PCH_XHCI_PGCBCTRL),
    (UINT32) (BIT24)
    );

  //
  //  AUX_CTRL_REG1 - AUX Power Management Control
  //  Address Offset: 80E0-80E3h
  //  Value: [22] 0b, [9] 0b, [6] 1b
  //
  MmioAndThenOr32 (
    (XhciMmioBase + R_PCH_XHCI_AUX_CTRL_REG1),
    (UINT32)~(BIT22 | BIT9),
    (UINT32) (BIT6)
    );

  //
  //  USB2_LINK_MGR_CTRL_REG1 - USB2 Port Link Control 1, 2, 3, 4
  //  Address Offset: 0x80F0 - 0x80FF [121] 1b, [20] 0b
  //  [127:96] is mapped to DW4 at offset 80FCh-80FFh [25] 1b
  //  [31:0]   is mapped to DW1 at offset 80F0h-80F3h [20] 0b
  //
  MmioOr32 (
    XhciMmioBase + R_PCH_XHCI_USB2_LINK_MGR_CTRL_REG1_DW4,
    (UINT32) (BIT25)
    );

  MmioAnd32 (
    XhciMmioBase + R_PCH_XHCI_USB2_LINK_MGR_CTRL_REG1_DW1,
    (UINT32) ~(BIT20)
    );

  //
  //  AUX_CTRL_REG2 - Aux PM Control Register 2
  //  Address Offset: 8154-8157h
  //  Value: [21] 0b, [13] 1b
  //
  MmioAndThenOr32 (
    (XhciMmioBase + R_PCH_XHCI_AUX_CTRL_REG2),
    (UINT32) ~(BIT21),
    (UINT32) (BIT13)
    );

  //
  //  xHCI Aux Clock Control Register
  //  Address Offset: 0x816C - 0x816F
  //  [13] 0b, [12] 0b, [11] 0b, [10] 0b, [9] 0b, [8] 0b
  //  [19] 1b, [18] 1b, [17] 1b, [14] 1b, [4] 1b, [3] 1b, [2] 1b,
  //
  Data32And = (UINT32) ~(BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8);
  Data32Or  = (UINT32) (BIT19 | BIT18 | BIT17 | BIT14 | BIT4 | BIT3 | BIT2);

  MmioAndThenOr32 (
    (XhciMmioBase + R_PCH_XHCI_AUXCLKCTL),
    Data32And,
    Data32Or
    );

  //
  //  HOST_IF_PWR_CTRL_REG0 - Power Scheduler Control 0
  //  Address Offset: 8140-8143h
  //  Value: [31:24] 0xFF, [11:0] 0x03C
  //         [23:12] 0x080 for SPT-LP Bx and SPT-H Bx/Cx steppings
  //         [23:12] 0x00F for all others steppings
  //
  if ((PchGen == SklPch) &&
      (((PchSeries == PchLp) && (PchStep < PchLpC0)) ||
       ((PchSeries == PchH) && (PchStep < PchHD0)))) {
    Data32 = 0xFF08003C;
  } else {
    Data32 = 0xFF00F03C;
  }
  MmioWrite32 (
    XhciMmioBase + R_PCH_XHCI_HOST_IF_PWR_CTRL_REG0,
    Data32
    );

  //
  // WA for HSDES 1205636788:
  //   Disable USB2 PHY SUS Well Power Gating by clearing USB2PMCTRL[3:2]
  //
  MmioAnd32 (XhciMmioBase + R_PCH_XHCI_USB2PMCTRL, (UINT32) ~(BIT3 | BIT2));

  //
  //  USB2PMCTRL - USB2 Power Management Control
  //  Address Offset:   0x81C4 - 0x81C7
  //  Value: [11] 1b, [8] 1b, [3:2] 0b, [1:0] 11b
  //
  MmioAndThenOr32 (XhciMmioBase + R_PCH_XHCI_USB2PMCTRL,
    (UINT32) ~(BIT3 | BIT2),
    (UINT32) (BIT11 | BIT8 | BIT1 | BIT0)
    );

  //
  //  HOST_CTRL_SCH_REG - Host Control Scheduler
  //  Address Offset: 8094-8097h
  //  Value: [23] 1b, [22] 1b, [6] 1b
  //
  MmioOr32 (
    XhciMmioBase + R_PCH_XHCI_HOST_CTRL_SCH_REG,
    (UINT32) (BIT23 | BIT22 | BIT6)
    );

  //
  //  HOST_CTRL_IDMA_REG - Host Control IN DMA Register
  //  Address Offset: 809C-809Fh
  //  Value: [31] 0b
  //
  MmioAnd32 (
    XhciMmioBase + R_PCH_XHCI_HOST_CTRL_IDMA_REG,
    (UINT32) ~(BIT31)
    );

  //
  //  Host Controller Misc Reg
  //  Address Offset: 0x80B0 - 0x80B3
  //  Value: [24] 0b, [23] 1b, [18:16] 000b
  //
  MmioAndThenOr32 (
    XhciMmioBase + R_PCH_XHCI_HOST_CTRL_MISC_REG,
    (UINT32) ~(BIT24 | BIT18| BIT17 | BIT16),
    (UINT32) (BIT23)
    );

  //
  //  Host Controller Misc Reg 2
  //  Address Offset: 0x80B4 - 0x80B7
  //  Value: [2] 1b, [5] 1b for SPT-LP Bx step and SPT-H Bx/Cx steppings
  //         [2] 0b, [5] 0b for all others steppings
  //
  if ((PchGen == SklPch) &&
      (((PchSeries == PchLp) && (PchStep < PchLpC0)) ||
       ((PchSeries == PchH) && (PchStep < PchHD0)))) {
    MmioOr32 (XhciMmioBase + R_PCH_XHCI_HOST_CTRL_MISC_REG_2,
      (UINT32) (BIT5 | BIT2)
      );
  }

  //
  //  SSPE - Super Speed Port Enables
  //  Address Offset: 0x80B8 - 0x80BC
  //  Value: [30] 1b
  //
  MmioOr32 (XhciMmioBase + R_PCH_XHCI_SSPE,
    (UINT32) (BIT30)
    );

  //
  //  HOST_BW_OV_HS_REG - High Speed TT Bandwidth Overhead
  //  Address Offset: 0x80C8
  //  Value: [23:12] 0xA, [11:0] 0x0
  //
  MmioAndThenOr32 (
    XhciMmioBase + R_PCH_XHCI_HOST_BW_OV_HS_REG,
    (UINT32)~(B_PCH_XHCI_HOST_BW_OV_HS_REG_OVRH_HSBW | B_PCH_XHCI_HOST_BW_OV_HS_REG_OVRH_HSTTBW),
    (UINT32) (0xA << N_PCH_XHCI_HOST_BW_OV_HS_REG_OVRH_HSBW)
    );

  //
  //  HOST_CTRL_PORT_LINK_REG - SuperSpeed Port Link Control
  //  Address Offset: 0x80EC - 0x80EF
  //  Value: [19] 1b, [17] 1b
  //
  MmioOr32 (
    XhciMmioBase + R_PCH_XHCI_HOST_CTRL_PORT_LINK_REG,
    (UINT32) (BIT19 | BIT17)
    );

  //
  //  HOST_CTRL_TRM_REG2 - Host Controller Transfer Manager Control 2
  //  Address Offset: 8110-8113h
  //  Value: [2] 0b, [20] 1b, [11] 1b
  //
  MmioAndThenOr32 (
    (XhciMmioBase + R_PCH_XHCI_HOST_CTRL_TRM_REG2),
    (UINT32) ~(BIT2),
    (UINT32) (BIT20 | BIT11)
    );

  //
  //  HOST_CTRL_BW_MAX_REG - MAX BW Control Reg 4
  //  Address Offset: 0x8128
  //  Value: [23:12] 0x753
  //
  MmioAndThenOr32 (
    (XhciMmioBase + R_PCH_XHCI_HOST_CTRL_BW_MAX_REG),
    (UINT32)~(B_PCH_XHCI_HOST_CTRL_BW_MAX_REG_HS_MAX_BW),
    (UINT32) (0x753 << N_PCH_XHCI_HOST_CTRL_BW_MAX_REG_HS_MAX_BW)
    );

  //
  //  HOST_IF_PWR_CTRL_REG1 - Power Scheduler Control 1
  //  Address Offset: 8144-8147h
  //  Value: [8] 1b
  //
  MmioOr32 (XhciMmioBase + R_PCH_XHCI_HOST_IF_PWR_CTRL_REG1,
    (UINT32) (BIT8)
    );

  //
  //  USB2 PHY Power Management Control
  //  Address Offset: 8164-8167h
  //  Value: [7] 1b, [6] 1b, [5] 1b, [4] 1b, [3] 1b, [2] 1b,
  //
  MmioOr32 (XhciMmioBase + R_PCH_XHCI_USB2PHYPM,
    (UINT32) (BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2)
    );

  //
  //  xHC Latency Tolerance Parameters - LTV Control
  //  Address Offset: 0x8174 - 0x8177
  //  Value: [24] 1b, [11:0] 0xC0A
  //
  MmioAndThenOr32 (XhciMmioBase + R_PCH_XHCI_XHCLTVCTL2,
    (UINT32) ~0x00000FFF,
    0x01000C0A
    );

  //
  //  xHC Latency Tolerance Parameters - High Idle Time Control
  //  Address Offset: 0x817C - 0x817F
  //  Value: [28:16] 0x0332, [12:0] 0x00A3
  //
  MmioOr32 (XhciMmioBase + R_PCH_XHCI_LTVHIT,
    0x033200A3
    );

  //
  //  xHC Latency Tolerance Parameters - Medium Idle Time Control
  //  Address Offset: 0x8180 - 0x8183
  //  Value: [28:16] 0x00CB, [12:0] 0x0028
  //
  MmioOr32 (XhciMmioBase + R_PCH_XHCI_LTVMIT,
    0x00CB0028
    );

  //
  //  xHC Latency Tolerance Parameters - Low Idle Time Control
  //  Address Offset: 0x8184 - 0x8187
  //  Value: [28:16] 0x0064, [12:0] 0x001E
  //
  MmioOr32 (XhciMmioBase + R_PCH_XHCI_LTVLIT,
    0x0064001E
    );

  //
  //  XECP_CMDM_CTRL_REG1 - Command Manager Control 1
  //  Address Offset: 818C-818Fh
  //  Value: [8] 0b, [16] 1b
  //
  MmioAndThenOr32 (
    (XhciMmioBase + R_PCH_XHCI_XECP_CMDM_CTRL_REG1),
    (UINT32) ~(BIT8),
    (UINT32) (BIT16)
    );

  //
  //  XECP_CMDM_CTRL_REG3 - Command Manager Control 3
  //  Address Offset: 8194-8197h
  //  Value: [25] 1b
  //
  MmioOr32 (
    XhciMmioBase + R_PCH_XHCI_XECP_CMDM_CTRL_REG3,
    (UINT32) (BIT25)
    );

  //
  //  THROTT - XHCI Throttle Control
  //  Address Offset: 819C-819Fh
  //  Value: [20] 1b, [16] 1b, [14] 1b, [13] 1b, [12] 1b, [11:8] 0x3, [7:4] 0x7, [3:0] 0xD
  //
  MmioAndThenOr32 (
    XhciMmioBase + R_PCH_XHCI_THROTT,
    (UINT32) ~(BIT11 | BIT10 | BIT7 | BIT1),
    (UINT32) (BIT20 | BIT16 | BIT14 | BIT13 | BIT12 | BIT9 | BIT8 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT0)
    );

  //
  //  THROTT2 - XHCI Throttle Control2
  //  Address Offset: 81B4-81B7h
  //  Value: [9:0] 0x0
  //
  MmioAnd32 (
    XhciMmioBase + R_PCH_XHCI_THROTT2,
    (UINT32) ~(0x3FF)
    );

  //
  //  D0I2CTRL - D0I2 Control Register
  //  Address Offset: 81BC-81BFh
  //  Value: [31] 1b
  //
  MmioOr32 (
    XhciMmioBase + R_PCH_XHCI_D0I2CTRL,
    (UINT32) (BIT31)
    );

  //
  //  Force Disconnect upon DCE
  //  DBCCTL - DbC Control
  //  Address Offset: 0x8760 - 0x8763
  //  Value: [0] 1b
  //
  MmioOr32 (
    XhciMmioBase + R_PCH_XHCI_DBC_DBCCTL,
    (UINT32) (BIT0)
    );

  PortSCxUsb3Base = 0;

  switch (PchSeries) {
    case PchLp:
      PortSCxUsb3Base = R_PCH_LP_XHCI_PORTSC01USB3;
      break;

    case PchH:
      PortSCxUsb3Base = R_PCH_H_XHCI_PORTSC01USB3;
      break;

    default:
      ASSERT (FALSE);
      break;
  }

  //
  // Set 1 to enable Super Speed Ports terminations on enabled ports only (PDO = 0)
  // Required for Deep S3
  //
  if (PchSeries == PchLp) {
    XhciUsb3Pdo = MmioRead32 (XhciMmioBase + R_PCH_XHCI_USB3PDO) & B_PCH_XHCI_LP_USB3PDO_MASK;

    MmioAndThenOr32 (
      XhciMmioBase + R_PCH_XHCI_SSPE,
      (UINT32) (~B_PCH_XHCI_LP_SSPE_MASK),
      (UINT32) ((~XhciUsb3Pdo) & B_PCH_XHCI_LP_SSPE_MASK)
      );
  } else {
    XhciUsb3Pdo = MmioRead32 (XhciMmioBase + R_PCH_XHCI_USB3PDO) & B_PCH_XHCI_H_USB3PDO_MASK;

    MmioAndThenOr32 (
      XhciMmioBase + R_PCH_XHCI_SSPE,
      (UINT32) (~B_PCH_XHCI_H_SSPE_MASK),
      (UINT32) ((~XhciUsb3Pdo) & B_PCH_XHCI_H_SSPE_MASK)
      );
  }

  //
  // XHC Save/Restore function will be broken if reset during S3 resume.
  //
  Status = PeiServicesGetBootMode (&BootMode);
  if (!EFI_ERROR (Status)) {
    if (BootMode == BOOT_ON_S3_RESUME) {
      return;
    }
  }

  //
  // Perform WPR on USB3 port except for the port has DBC enabled.
  //
  //
  // Get debug enable status in order to skip some XHCI init which
  // may break XHCI debug
  //
  CapabilityPointer = (UINT32) (XhciMmioBase + (MmioRead32 (XhciMmioBase + R_PCH_XHCI_HCCPARAMS) >> 16) * 4);
  DebugEnable = FALSE;
  DebugPortSsIndex = 0xFFFFFFFF;
  Capability = MmioRead32 (CapabilityPointer);
  DEBUG ((DEBUG_ERROR, "PchUsbCommon XHCI Capability Pointer = 0x%x\n", CapabilityPointer));

  while (TRUE) {
    if ((Capability & 0xFF) == 0xA) {
      //
      // Check DCR bit in DCCTRL register (Debug Capability Base + 20h), if set, debug device is running
      //
      if ((MmioRead32 (CapabilityPointer + 0x20) & BIT0) != 0) {
        DebugEnable = TRUE;
        //
        // Get debug port number [24:31] in DCST register which starts from 1
        //
        DebugPort = (MmioRead32 (CapabilityPointer + 0x24) >> 24) - 1;
        for (UsbPort = 0; UsbPort < SsPortCount; UsbPort++) {
          if ((R_PCH_LP_XHCI_PORTSC01USB2 + DebugPort * 0x10) == (PortSCxUsb3Base + (UsbPort * 0x10))) {
            DebugPortSsIndex = UsbPort;
            DEBUG ((DEBUG_ERROR, "PchUsbCommon DebugPortSsIndex = ?%d\n", DebugPortSsIndex));
            break;
          }
        }
      }
      break;
    }
    if ((((Capability & 0xFF00) >> 8) & 0xFF) == 0) {
      //
      // Reach the end of list, quit
      //
      break;
    }
    CapabilityPointer += ((Capability & 0xFF00) >> 8) * 4;
    Capability = MmioRead32 (CapabilityPointer);
  }

  //
  // Initiate warm reset to all USB3 ports except for the USB3 port which has Dbc enabled
  //
  for (UsbPort = 0; UsbPort < SsPortCount; UsbPort++) {
    if ((DebugEnable) && (UsbPort == DebugPortSsIndex)) {
      continue;
    }
    MmioAndThenOr32 (
      XhciMmioBase + (PortSCxUsb3Base + (UsbPort * 0x10)),
      (UINT32) ~ (B_PCH_XHCI_PORTSCXUSB3_PED),
      B_PCH_XHCI_PORTSCXUSB3_WPR
      );
  }
}

/**
  Program and enable XHCI Memory Space

  @param[in] UsbConfig            The PCH Policy for USB configuration
  @param[in] XhciMmioBase         Memory base address of XHCI Controller
  @param[in] XhciPciMmBase        XHCI PCI Base Address

**/
VOID
XhciMemorySpaceOpen (
  IN  CONST PCH_USB_CONFIG        *UsbConfig,
  IN  UINT32                      XhciMmioBase,
  IN  UINTN                       XhciPciMmBase
  )
{
  ///
  /// Assign memory resources
  ///
  MmioWrite32 (XhciPciMmBase + R_PCH_XHCI_MEM_BASE, XhciMmioBase);

  MmioOr16 (
    XhciPciMmBase + PCI_COMMAND_OFFSET,
    (UINT16) (EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER)
    );

}

/**
  Clear and disable XHCI Memory Space

  @param[in] UsbConfig            The PCH Policy for USB configuration
  @param[in] XhciMmioBase         Memory base address of XHCI Controller
  @param[in] XhciPciMmBase        XHCI PCI Base Address

  @retval None
**/
VOID
XhciMemorySpaceClose (
  IN  CONST PCH_USB_CONFIG        *UsbConfig,
  IN  UINT32                      XhciMmioBase,
  IN  UINTN                       XhciPciMmBase
  )
{
  ///
  /// Clear memory resources
  ///
  MmioAnd16 (
    XhciPciMmBase + PCI_COMMAND_OFFSET,
    (UINT16) ~(EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER)
    );

  MmioWrite32 ((XhciPciMmBase + R_PCH_XHCI_MEM_BASE), 0);

}

/**
  Configure PCH xHCI after initialization

  @param[in] SiPolicy             The SI Policy PPI instance
  @param[in] TempMemBaseAddr      Temporary Memory Base Address for PCI
                                  devices to be used to initialize MMIO
                                  registers.

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver
**/
EFI_STATUS
EFIAPI
ConfigureXhciAfterInit (
  IN  SI_POLICY_PPI            *SiPolicy,
  IN  UINT32                    TempMemBaseAddr
  )
{
  UINTN           XhciPciMmBase;
  PCH_SERIES      PchSeries;
  PCH_USB_CONFIG  *UsbConfig;
  UINT32          XhciMmioBase;
  EFI_STATUS      Status;

  DEBUG ((DEBUG_INFO, "ConfigureXhciAfterInit () - Start\n"));

  Status = GetConfigBlock ((VOID *) SiPolicy, &gUsbConfigGuid, (VOID *) &UsbConfig);
  ASSERT_EFI_ERROR (Status);

  XhciMmioBase  = TempMemBaseAddr;
  PchSeries     = GetPchSeries ();

  XhciPciMmBase = MmPciBase (
                    0,
                    PCI_DEVICE_NUMBER_PCH_XHCI,
                    PCI_FUNCTION_NUMBER_PCH_XHCI
                    );

  //
  // Assign memory resources
  //
  MmioWrite32 (XhciPciMmBase + R_PCH_XHCI_MEM_BASE, XhciMmioBase);
  MmioOr16 (
    XhciPciMmBase + PCI_COMMAND_OFFSET,
    (UINT16) (EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER)
    );

  //
  //  AUX_CTRL_REG1 - AUX Power Management Control
  //  Address Offset: 80E0-80E3h
  //  Value: [16] 0b
  //  @note: This enables xHCI Port Clock Gating and is required to be done after HC initialization is done
  //
  MmioAnd32 (
    (XhciMmioBase + R_PCH_XHCI_AUX_CTRL_REG1),
    (UINT32)~(BIT16)
    );

  Usb3WarmPortReset (XhciMmioBase);

  //
  // Clear memory resources
  //
  MmioAnd16 (
    XhciPciMmBase + PCI_COMMAND_OFFSET,
    (UINT16) ~(EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER)
    );
  MmioWrite32 ((XhciPciMmBase + R_PCH_XHCI_MEM_BASE), 0);

  DEBUG ((DEBUG_INFO, "ConfigureXhciAfterInit () - End\n"));

  return EFI_SUCCESS;
}

/**
  Performs USB3 warm port reset.

  @param[in] XhciMmioBase        XHCI Mmio Base Address

  @retval None
**/
VOID
Usb3WarmPortReset (
  IN  UINT32    XhciMmioBase
  )
{
  PCH_SERIES      PchSeries;
  UINT32          SsPortCount;
  UINTN           PortSCxUsb3Base;
  UINT32          Data32;
  UINTN           Timeout;
  BOOLEAN         DebugEnable;
  UINT32          DebugPortSsIndex;
  UINT32          UsbPort;
  UINT32          DebugPort;
  UINT32          CapabilityPointer;
  UINT32          Capability;

  PortSCxUsb3Base = 0;
  PchSeries       = GetPchSeries ();
  SsPortCount     = 0;
  SsPortCount     = (MmioRead32 (XhciMmioBase + R_PCH_XHCI_XECP_SUPP_USB3_2) & 0x0000FF00) >> 8;

  switch (PchSeries) {
    case PchLp:
      PortSCxUsb3Base = R_PCH_LP_XHCI_PORTSC01USB3;
      break;

    case PchH:
      PortSCxUsb3Base = R_PCH_H_XHCI_PORTSC01USB3;
      break;

    default:
      ASSERT (FALSE);
      break;
  }

  //
  // Perform WPR on USB3 port except for the port has DBC enabled.
  //
  //
  // Get debug enable status in order to skip some XHCI init which
  // may break XHCI debug
  //
  CapabilityPointer = (UINT32) (XhciMmioBase + (MmioRead32 (XhciMmioBase + R_PCH_XHCI_HCCPARAMS) >> 16) * 4);
  DebugEnable       = FALSE;
  DebugPortSsIndex  = 0xFFFFFFFF;
  Capability        = MmioRead32 (CapabilityPointer);

  DEBUG ((DEBUG_ERROR, "PchUsbPei XHCI Capability Pointer = 0x%x\n", CapabilityPointer));

  while (TRUE) {
    if ((Capability & 0xFF) == 0xA) {
      //
      // Check DCR bit in DCCTRL register (Debug Capability Base + 20h), if set, debug device is running
      //
      if ((MmioRead32 (CapabilityPointer + 0x20) & BIT0) != 0) {
        DebugEnable = TRUE;
        //
        // Get debug port number [24:31] in DCST register which starts from 1
        //
        DebugPort = (MmioRead32 (CapabilityPointer + 0x24) >> 24) - 1;
        for (UsbPort = 0; UsbPort < SsPortCount; UsbPort++) {
          if ((R_PCH_LP_XHCI_PORTSC01USB2 + DebugPort * 0x10) == (PortSCxUsb3Base + (UsbPort * 0x10))) {
            DebugPortSsIndex = UsbPort;
            DEBUG ((DEBUG_ERROR, "PchUsbCommon DebugPortSsIndex = ?%d\n", DebugPortSsIndex));
            break;
          }
        }
      }
      break;
    }
    if ((((Capability & 0xFF00) >> 8) & 0xFF) == 0) {
      //
      // Reach the end of list, quit
      //
      break;
    }
    CapabilityPointer += ((Capability & 0xFF00) >> 8) * 4;
    Capability = MmioRead32 (CapabilityPointer);
  }

  //
  // Poll for warm reset bit at steps #a to be cleared or timeout at 120ms
  //
  Timeout = 0;
  while (TRUE) {
    Data32 = 0;
    for (UsbPort = 0; UsbPort < SsPortCount; UsbPort++) {
      if ((DebugEnable) && (UsbPort == DebugPortSsIndex)) {
        continue;
      }
      Data32 |= MmioRead32 (XhciMmioBase + (PortSCxUsb3Base + (UsbPort * 0x10)));
    }
    if (((Data32 & B_PCH_XHCI_PORTSCXUSB3_PR) == 0) || (Timeout > PORT_RESET_TIMEOUT)) {
      break;
    }
    MicroSecondDelay (10);
    Timeout++;
  }

  //
  // Clear all the port's status by program xHCIBAR + 570h [23:17] to 1111111b.
  //
  for (UsbPort = 0; UsbPort < SsPortCount; UsbPort++) {
    if ((DebugEnable) && (UsbPort == DebugPortSsIndex)) {
      continue;
    }
    MmioAndThenOr32 (
      XhciMmioBase + (PortSCxUsb3Base + (UsbPort * 0x10)),
      (UINT32) ~ (B_PCH_XHCI_PORTSCXUSB3_PED),
      B_PCH_XHCI_PORTSCXUSB3_CEC |
      B_PCH_XHCI_PORTSCXUSB3_PLC |
      B_PCH_XHCI_PORTSCXUSB3_PRC |
      B_PCH_XHCI_PORTSCXUSB3_OCC |
      B_PCH_XHCI_PORTSCXUSB3_WRC |
      B_PCH_XHCI_PORTSCXUSB3_PEC |
      B_PCH_XHCI_PORTSCXUSB3_CSC
      );
  }
}

/**
  Tune the USB 2.0 high-speed signals quality.

  @param[in]  UsbConfig                 The PCH Policy for USB configuration
  @param[in]  TempMemBaseAddr           The temporary memory base address for PMC device
**/
VOID
Usb2AfeProgramming (
  IN  CONST PCH_USB_CONFIG    *UsbConfig,
  IN  UINT32                  TempMemBaseAddr
  )
{
  UINT8           Response;
  UINT32          Data32;
  UINT8           PpLane;
  UINT64          Offset;
  UINT8           MaxUsbPhyPortNum;
  UINT32          PchPwrmBase;
  UINTN           PmcBaseAddress;

  DEBUG ((DEBUG_INFO, "xHCI: Usb2AfeProgramming Start\n"));

  PchPwrmBaseGet (&PchPwrmBase);

  PmcBaseAddress = MmPciBase (
                     DEFAULT_PCI_BUS_NUMBER_PCH,
                     PCI_DEVICE_NUMBER_PCH_PMC,
                     PCI_FUNCTION_NUMBER_PCH_PMC
                     );

  //
  // USB2 PCR GLB ADP VBUS REG 0x402B[22:22] = 0x1
  // This register must access through Sideband interface.
  //
  PchSbiExecution (
    PID_USB2,
    R_PCH_PCR_USB2_GLB_ADP_VBUS_REG,
    PrivateControlRead,
    FALSE,
    &Data32,
    &Response
    );

  Data32 |= (BIT22);

  PchSbiExecution (
    PID_USB2,
    R_PCH_PCR_USB2_GLB_ADP_VBUS_REG,
    PrivateControlWrite,
    TRUE,
    &Data32,
    &Response
    );
  PchPmcXramWrite32 ((PCH_SBI_PID) PID_USB2, (UINT16) R_PCH_PCR_USB2_GLB_ADP_VBUS_REG, (UINT32) Data32, TempMemBaseAddr, PmcBaseAddress);

  //
  // USB2 GLOBAL PORT 0x4001[25] = 0x1
  // This register must access through Sideband interface.
  //
  PchSbiExecution (
    PID_USB2,
    R_PCH_PCR_USB2_GLOBAL_PORT,
    PrivateControlRead,
    FALSE,
    &Data32,
    &Response
    );

  Data32 |= (BIT25);

  PchSbiExecution (
    PID_USB2,
    R_PCH_PCR_USB2_GLOBAL_PORT,
    PrivateControlWrite,
    TRUE,
    &Data32,
    &Response
    );
  PchPmcXramWrite32 ((PCH_SBI_PID) PID_USB2, (UINT16) R_PCH_PCR_USB2_GLOBAL_PORT, (UINT32) Data32, TempMemBaseAddr, PmcBaseAddress);

  //
  // USB2 PCR ADPLL 0x7034[31:0] = 0x50000000
  // This register must access through Sideband interface.
  //
  Data32 = 0x50000000;
  PchSbiExecution (
    PID_USB2,
    R_PCH_PCR_USB2_7034,
    PrivateControlWrite,
    TRUE,
    &Data32,
    &Response
    );
  PchPmcXramWrite32 ((PCH_SBI_PID) PID_USB2, (UINT16) R_PCH_PCR_USB2_7034, (UINT32) Data32, TempMemBaseAddr, PmcBaseAddress);

  //
  // USB2 PCR ADPLL 0x7038[31:0] = 0x0A70FCF6
  // This register must access through Sideband interface.
  //
  Data32 = 0x0A70FCF6;
  PchSbiExecution (
    PID_USB2,
    R_PCH_PCR_USB2_7038,
    PrivateControlWrite,
    TRUE,
    &Data32,
    &Response
    );
  PchPmcXramWrite32 ((PCH_SBI_PID) PID_USB2, (UINT16) R_PCH_PCR_USB2_7038, (UINT32) Data32, TempMemBaseAddr, PmcBaseAddress);

  //
  // USB2 PCR ADPLL 0x703C[31:0] = 0x1FBBC000
  // This register must access through Sideband interface.
  //
  Data32 = 0x1FBBC000;
  PchSbiExecution (
    PID_USB2,
    R_PCH_PCR_USB2_703C,
    PrivateControlWrite,
    TRUE,
    &Data32,
    &Response
    );
  PchPmcXramWrite32 ((PCH_SBI_PID) PID_USB2, (UINT16) R_PCH_PCR_USB2_703C, (UINT32) Data32, TempMemBaseAddr, PmcBaseAddress);

  //
  //  USB2 PCR PLL2 register
  //  Set bit 26 to force PLL cycle
  //
  PchSbiExecution (
    PID_USB2,
    R_PCH_PCR_USB2_PLL2,
    PrivateControlRead,
    FALSE,
    &Data32,
    &Response
    );

  if (!(Data32 & B_PCH_PCR_USB2_PLL2_USB_PLL_LOCK)) {
    Data32 |= B_PCH_PCR_USB2_PLL2_FORCE_PLL_CYCLE;

    PchSbiExecution (
      PID_USB2,
      R_PCH_PCR_USB2_PLL2,
      PrivateControlWrite,
      TRUE,
      &Data32,
      &Response
      );
    PchPmcXramWrite32 ((PCH_SBI_PID) PID_USB2, (UINT16) R_PCH_PCR_USB2_PLL2, (UINT32) Data32, TempMemBaseAddr, PmcBaseAddress);
  }

  //
  // Set USB2 COMPBG to 0x4600 (Default Value 0x600)
  // This register must access through Sideband interface.
  //
  PchSbiExecution (
    PID_USB2,
    R_PCH_PCR_USB2_CFG_COMPBG,
    PrivateControlRead,
    FALSE,
    &Data32,
    &Response
    );
  Data32 |= (BIT10 | BIT9);
  PchSbiExecution (
    PID_USB2,
    R_PCH_PCR_USB2_CFG_COMPBG,
    PrivateControlWrite,
    TRUE,
    &Data32,
    &Response
    );
  PchPmcXramWrite32 ((PCH_SBI_PID) PID_USB2, (UINT16) R_PCH_PCR_USB2_CFG_COMPBG, (UINT32) Data32, TempMemBaseAddr, PmcBaseAddress);

  MaxUsbPhyPortNum = GetPchUsbMaxPhysicalPortNum ();
  for (PpLane = 1; PpLane <= MaxUsbPhyPortNum; PpLane++) {
    //
    // USB2 PER PORT
    // Address Offset: 0x0
    // Bit[14:8] according to recommendation for current board design
    // Bit[31:26] = 111111b
    //

    Offset = R_PCH_PCR_USB2_PP_LANE_BASE_ADDR | V_PCH_PCR_USB2_PER_PORT | (PpLane << 8);

    PchSbiExecution (
      PID_USB2,
      Offset,
      PrivateControlRead,
      FALSE,
      &Data32,
      &Response
      );

    Data32 &= ~(BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8);
    Data32 |= (((UsbConfig->PortUsb20[PpLane - 1].Afe.Petxiset) & 0x7)  << 11) |
      (((UsbConfig->PortUsb20[PpLane - 1].Afe.Txiset) & 0x7)    << 8) |
      (((UsbConfig->PortUsb20[PpLane - 1].Afe.Pehalfbit) & 0x1) << 14);
    Data32 |= (BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26);

    PchSbiExecution (
      PID_USB2,
      Offset,
      PrivateControlWrite,
      TRUE,
      &Data32,
      &Response
      );
    PchPmcXramWrite32 ((PCH_SBI_PID) PID_USB2, (UINT16) Offset, (UINT32) Data32, TempMemBaseAddr, PmcBaseAddress);

    //
    // USB2 PER PORT 2
    // Address Offset: 0x26
    //
    Offset = R_PCH_PCR_USB2_PP_LANE_BASE_ADDR | V_PCH_PCR_USB2_PER_PORT_2 | (PpLane << 8);

    Data32 = 0;
    PchSbiExecution (
      PID_USB2,
      Offset,
      PrivateControlRead,
      FALSE,
      &Data32,
      &Response
      );

    Data32 &= ~(BIT24 | BIT23);
    Data32 |= (((UsbConfig->PortUsb20[PpLane - 1].Afe.Predeemp) & 0x3) << 23);

    PchSbiExecution (
      PID_USB2,
      Offset,
      PrivateControlWrite,
      TRUE,
      &Data32,
      &Response
      );
    PchPmcXramWrite32 ((PCH_SBI_PID) PID_USB2, (UINT16) Offset, (UINT32) Data32, TempMemBaseAddr, PmcBaseAddress);

    //
    // UTMI MISC REG PER PORT
    // Address Offset: 0x08 (for each lane)
    // [13:12] 01b, [11] 1b [10:9] 01b, [7] 1b
    //
    Offset = R_PCH_PCR_USB2_PP_LANE_BASE_ADDR | V_PCH_PCR_USB2_UTMI_MISC_PER_PORT | (PpLane << 8);
    PchSbiExecution (
      PID_USB2,
      Offset,
      PrivateControlRead,
      FALSE,
      &Data32,
      &Response
      );

    Data32 &= ~(BIT13 | BIT10);
    Data32 |= (BIT12 | BIT11 | BIT9 | BIT7);

    PchSbiExecution (
      PID_USB2,
      Offset,
      PrivateControlWrite,
      TRUE,
      &Data32,
      &Response
      );
    PchPmcXramWrite32 ((PCH_SBI_PID) PID_USB2, (UINT16) Offset, (UINT32) Data32, TempMemBaseAddr, PmcBaseAddress);
  }
// AMI_OVERRIDE_PCH0029_START >>> Move to PchInit.c AmiPchHsioProg()
#if 0
  PchPmcXramEndTableMarker (TempMemBaseAddr, PmcBaseAddress);
  PchPmcXramDataHdrProg (TempMemBaseAddr, PmcBaseAddress);

  ///
  /// Please note: below programming must be done after USB2 GLOBAL PORT 2 programming
  /// PM_CFG (0x018h[29]) = 1, Allow 24MHz Crystal Oscillator Shutdown (ALLOW_24_OSC_SD)
  ///
  MmioOr32 (PchPwrmBase + R_PCH_PWRM_CFG, B_PCH_PWRM_CFG_ALLOW_24_OSC_SD);
#endif
// AMI_OVERRIDE_PCH0029_END <<<
}

/**
  Setup XHCI Over-Current Mapping

  @param[in] UsbConfig            The PCH Policy for USB configuration
  @param[in] XhciPciMmBase        XHCI PCI Base Address

  @retval None
**/
VOID
XhciOverCurrentMapping (
  IN  CONST PCH_USB_CONFIG        *UsbConfig,
  IN  UINTN                       XhciPciMmBase
  )
{
  ///
  ///  BIOS responsibility on Overcurrent protection.
  ///  ----------------------------------------------
  ///
  UINT32                   Index;
  UINT32                   U2OCMBuf[V_PCH_XHCI_NUMBER_OF_OC_PINS];
  UINT32                   U3OCMBuf[V_PCH_XHCI_NUMBER_OF_OC_PINS];
  UINT32                   OCPin;

  ZeroMem (U2OCMBuf, sizeof (U2OCMBuf));
  ZeroMem (U3OCMBuf, sizeof (U3OCMBuf));

  for (Index = 0; Index < GetPchXhciMaxUsb3PortNum (); Index++) {
    if (UsbConfig->PortUsb30[Index].OverCurrentPin == PchUsbOverCurrentPinSkip) {
      ///
      /// No OC pin assigned, skip this port
      ///
    } else {
      OCPin = UsbConfig->PortUsb30[Index].OverCurrentPin;
      ASSERT (OCPin < V_PCH_XHCI_NUMBER_OF_OC_PINS);
      U3OCMBuf[OCPin] |= (UINT32) (BIT0 << Index);
    }
  }

  for (Index = 0; Index < V_PCH_XHCI_NUMBER_OF_OC_PINS; Index++) {
    MmioWrite32 (XhciPciMmBase + R_PCH_XHCI_U3OCM + (Index * 4), U3OCMBuf[Index]);
  }

  for (Index = 0; Index < GetPchXhciMaxUsb2PortNum (); Index++) {
    if (UsbConfig->PortUsb20[Index].OverCurrentPin == PchUsbOverCurrentPinSkip) {
      ///
      /// No OC pin assigned, skip this port
      ///
    } else {
      OCPin = UsbConfig->PortUsb20[Index].OverCurrentPin;
      ASSERT (OCPin < V_PCH_XHCI_NUMBER_OF_OC_PINS);
      U2OCMBuf[OCPin] |= (UINT32) (BIT0 << Index);
    }
  }

  for (Index = 0; Index < V_PCH_XHCI_NUMBER_OF_OC_PINS; Index++) {
    MmioWrite32 (XhciPciMmBase + R_PCH_XHCI_U2OCM + (Index * 4), U2OCMBuf[Index]);
  }

}

/**
  Write RRAP commands to registers.

  @param[in]      Cmd           RRAP command
  @param[in]      Port          SSIC port number
  @param[in]      XhciMmioBase  Memory base address of xHCI Controller
  @param[in, out] IndexP1       Command index for port 1
  @param[in, out] IndexP2       Command index for port 2

**/
EFI_STATUS
EFIAPI
XhciSsicRrapCmd (
  IN     UINT32  Cmd,
  IN     UINT32  Port,
  IN     UINT32  XhciMmioBase,
  IN OUT UINT32  *IndexP1,
  IN OUT UINT32  *IndexP2
  )
{

  DEBUG ((DEBUG_INFO, "xHCI: Rrap: Port: %x, Cmd: %x, Ix1: %x Ix2: %x\n", Port, Cmd, *IndexP1, *IndexP2));

  //
  // RRAP commands are writen to the register bank.
  // This bank of registers provides 64 Dwords per port to be used to store attributes that
  // need to be written into the local and remote phy.
  // Each port has its own buffer, 64 Dwords per port (SSIC Port N Register Access Control)
  // Port 1: 0Ch,10h, ... ,108h
  // Port 2: 11Ch, 120h, ... , 218h
  //
  // IndexP1 and IndexP2 need to be kept outside the function to remember the pointer.
  //
  switch (Port) {
    case 1:

      MmioWrite32 (
        XhciMmioBase + R_PCH_XHCI_SSIC_PROF_ATTR_P1_0C + (*IndexP1 * 4),
        Cmd
        );
      (*IndexP1) ++;
      break;

    case 2:
      MmioWrite32 (
        XhciMmioBase + R_PCH_XHCI_SSIC_PROF_ATTR_P2_0C + (*IndexP2 * 4),
        Cmd
        );
      (*IndexP2) ++;
      break;

    default:

      break;

  }

  return EFI_SUCCESS;
}

/**
  Check if boot on S5 resume.

  @retval TRUE   Boot on S5 resume.
  @retval FALSE  Not boot on S5 resume.

**/
BOOLEAN
XhciIsS5 (
  VOID
  )
{
  UINT32  SleepType;
  UINT32  Pm1Cnt;
  UINT16  Pm1Sts;
  UINT16  AcpiBaseAddr;

  PchAcpiBaseGet (&AcpiBaseAddr);
  Pm1Sts      = IoRead16 (AcpiBaseAddr + R_PCH_ACPI_PM1_STS);
  Pm1Cnt      = IoRead32 (AcpiBaseAddr + R_PCH_ACPI_PM1_CNT);

  if ((Pm1Sts & B_PCH_ACPI_PM1_STS_WAK) != 0) {
    if ((MmioRead16 (
           MmPciBase (
             DEFAULT_PCI_BUS_NUMBER_PCH,
             PCI_DEVICE_NUMBER_PCH_PMC,
             PCI_FUNCTION_NUMBER_PCH_PMC) + R_PCH_PMC_GEN_PMCON_B) & (B_PCH_PMC_GEN_PMCON_B_RTC_PWR_STS | B_PCH_PMC_GEN_PMCON_B_PWR_FLR)) == 0) {
      //
      // Wake event occurred and there is no power failure and reset
      //
      SleepType = Pm1Cnt & B_PCH_ACPI_PM1_CNT_SLP_TYP;
      return (SleepType == V_PCH_ACPI_PM1_CNT_S5);
    }
  }
  return FALSE;
}

#ifndef MINTREE_FLAG
/**
  Performs configuration of PCH xHCI SSIC.

  @param[in] SiPolicy             The SI Policy PPI
  @param[in] XhciMmioBase         Memory base address of xHCI Controller
  @param[in] XhciPciMmBase        XHCI PCI Base Address

**/
VOID
XhciSsicInit (
  IN  SI_POLICY_PPI               *SiPolicy,
  IN  UINT32                      XhciMmioBase,
  IN  UINTN                       XhciPciMmBase
  )
{
  UINT32            Data32;
  UINT32            DataAnd32;
  UINT32            Iteration;
  UINT8             Response;
  PCH_STEPPING      PchStep;
  EFI_STATUS        Status;
  PCH_SPI_PPI       *SpiPpi;
  UINT8             SoftStrapValue;
  UINT32            IndexP1;
  UINT32            IndexP2;
  PCH_USB_CONFIG    *UsbConfig;
  PCH_SERIES        PchSeries;

  DEBUG ((DEBUG_INFO, "xHCI: XhciSsicInit Start\n"));

  PchSeries = GetPchSeries ();

  // SSIC is not supported on KBL-H platforms
  if (PchSeries == PchH) {
    return;
  }

  Status = GetConfigBlock ((VOID *) SiPolicy, &gUsbConfigGuid, (VOID *) &UsbConfig);
  ASSERT_EFI_ERROR (Status);

  IndexP1 = 0;
  IndexP2 = 0;

  if ((MmioRead32 (XhciMmioBase + R_PCH_XHCI_STRAP2) & 0x6) == 0x0) {
    //
    // SSIC Configuration Register 2 Port 1
    // Address Offset: 880Ch
    // Value [30] 1h, [31] 1h if the port has no physical SSIC Device connected
    //
    Data32 = B_PCH_XHCI_SSIC_CONF_REG2_PORT_UNUSED | B_PCH_XHCI_SSIC_CONF_REG2_PROG_DONE;
    MmioOr32 (
      XhciMmioBase + R_PCH_XHCI_SSIC_CONF_REG2_PORT_1,
      Data32
      );
    //
    // SSIC Configuration Register 2 Port 2
    // Address Offset: 883Ch
    // Value [30] 1h, [31] 1h if the port has no physical SSIC Device connected
    //
    MmioOr32 (
      XhciMmioBase + R_PCH_XHCI_SSIC_CONF_REG2_PORT_2,
      Data32
      );
    DEBUG ((DEBUG_INFO, "xHCI: SSIC disabled by straps\n"));
    return;
  }

  Status = PeiServicesLocatePpi (
             &gPchSpiPpiGuid,
             0,
             NULL,
             (VOID **) &SpiPpi
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "xHCI: SPI PPI is not available\n"));
    return;
  }

  Status = SpiPpi->ReadPchSoftStrap (SpiPpi, R_PCH_SPI_STRP_MMP0, sizeof (SoftStrapValue), &SoftStrapValue);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "xHCI: Reading strap error\n"));
    return;
  }
  if ((SoftStrapValue & B_PCH_SPI_STRP_MMP0)) {
    DEBUG ((DEBUG_ERROR, "xHCI: SSIC: MMP0 disabled by strap\n"));
    return;
  }

  //
  // ModPHY  0xF00C[5:0] 0x33,
  // Access to this PCI register via SBI, PID MMP0 = 0xB0
  //
  PchSbiExecution (
    PID_MMP0,
    R_PCH_PCR_MMP0_CMNREG4,
    PciConfigRead,
    FALSE,
    &Data32,
    &Response
    );

  Data32 &= 0xFFFFFF80;
  Data32 |= 0x33;

  PchSbiExecution (
    PID_MMP0,
    R_PCH_PCR_MMP0_CMNREG4,
    PciConfigWrite,
    TRUE,
    &Data32,
    &Response
    );

  //
  // Set MMP0 CMNREG15 0xF038[0] 1b
  //
  PchSbiExecution (
    PID_MMP0,
    R_PCH_PCR_MMP0_CMNREG15,
    PciConfigRead,
    FALSE,
    &Data32,
    &Response
    );

  Data32 |= BIT0;

  PchSbiExecution (
    PID_MMP0,
    R_PCH_PCR_MMP0_CMNREG15,
    PciConfigWrite,
    TRUE,
    &Data32,
    &Response
    );

  //
  // Lane 0 settings
  //
  if (UsbConfig->SsicConfig.SsicPort[0].Enable == TRUE) {
    //
    // Set MMP0 IMPREG21 0x1050 [24] = 1b, [7:0] = 0x20
    //
    PchSbiExecution (
      PID_MMP0,
      R_PCH_PCR_MMP0_LANE_0_OFFSET + R_PCH_PCR_MMP0_IMPREG21,
      PciConfigRead,
      FALSE,
      &Data32,
      &Response
      );

    Data32 &= 0xFFFFFF00;
    Data32 |= (BIT24 | BIT5);

    PchSbiExecution (
      PID_MMP0,
      R_PCH_PCR_MMP0_LANE_0_OFFSET + R_PCH_PCR_MMP0_IMPREG21,
      PciConfigWrite,
      TRUE,
      &Data32,
      &Response
      );

    //
    // Set cnfgupd (BIT26) of IMPREG22 register to update effective configuration registers from
    // shadow registers. BIT26 - Update Private Registers
    //
    PchSbiExecution (
      PID_MMP0,
      R_PCH_PCR_MMP0_LANE_0_OFFSET + R_PCH_PCR_MMP0_IMPREG22,
      PciConfigRead,
      FALSE,
      &Data32,
      &Response
      );
    Data32 |= BIT26;
    PchSbiExecution (
      PID_MMP0,
      R_PCH_PCR_MMP0_LANE_0_OFFSET + R_PCH_PCR_MMP0_IMPREG22,
      PciConfigWrite,
      TRUE,
      &Data32,
      &Response
      );

    //
    // Set MMP0 IMPREG23 0x1058[16] 1b
    //
    PchSbiExecution (
      PID_MMP0,
      R_PCH_PCR_MMP0_LANE_0_OFFSET + R_PCH_PCR_MMP0_IMPREG23,
      PciConfigRead,
      FALSE,
      &Data32,
      &Response
      );

    Data32 |= BIT16;

    PchSbiExecution (
      PID_MMP0,
      R_PCH_PCR_MMP0_LANE_0_OFFSET + R_PCH_PCR_MMP0_IMPREG23,
      PciConfigWrite,
      TRUE,
      &Data32,
      &Response
      );

    //
    // Set cnfgupd (BIT26) of IMPREG22 register to update effective configuration registers from
    // shadow registers. BIT26 - Update Private Registers
    //
    PchSbiExecution (
      PID_MMP0,
      R_PCH_PCR_MMP0_LANE_0_OFFSET + R_PCH_PCR_MMP0_IMPREG22,
      PciConfigRead,
      FALSE,
      &Data32,
      &Response
      );
    Data32 |= BIT26;
    PchSbiExecution (
      PID_MMP0,
      R_PCH_PCR_MMP0_LANE_0_OFFSET + R_PCH_PCR_MMP0_IMPREG22,
      PciConfigWrite,
      TRUE,
      &Data32,
      &Response
      );
    //
    // Set MMP0 IMPREG25 0x1060[23:16] 0x0
    //
    PchSbiExecution (
      PID_MMP0,
      R_PCH_PCR_MMP0_LANE_0_OFFSET + R_PCH_PCR_MMP0_IMPREG25,
      PciConfigRead,
      FALSE,
      &Data32,
      &Response
      );

    Data32 &= 0XFF00FFFF;

    PchSbiExecution (
      PID_MMP0,
      R_PCH_PCR_MMP0_LANE_0_OFFSET + R_PCH_PCR_MMP0_IMPREG25,
      PciConfigWrite,
      TRUE,
      &Data32,
      &Response
      );

    //
    // Set cnfgupd (BIT26) of IMPREG22 register to update effective configuration registers from
    // shadow registers. BIT26 - Update Private Registers
    //
    PchSbiExecution (
      PID_MMP0,
      R_PCH_PCR_MMP0_LANE_0_OFFSET + R_PCH_PCR_MMP0_IMPREG22,
      PciConfigRead,
      FALSE,
      &Data32,
      &Response
      );
    Data32 |= BIT26;
    PchSbiExecution (
      PID_MMP0,
      R_PCH_PCR_MMP0_LANE_0_OFFSET + R_PCH_PCR_MMP0_IMPREG22,
      PciConfigWrite,
      TRUE,
      &Data32,
      &Response
      );
  }

  //
  // Lane 1 settings
  //
  if (UsbConfig->SsicConfig.SsicPort[1].Enable == TRUE) {
    //
    // Set MMP0 IMPREG21 0x3050 [24] = 1b, 0x3050[7:0] = 0x20
    //
    PchSbiExecution (
      PID_MMP0,
      R_PCH_PCR_MMP0_LANE_1_OFFSET + R_PCH_PCR_MMP0_IMPREG21,
      PciConfigRead,
      FALSE,
      &Data32,
      &Response
      );

    Data32 &= 0xFFFFFF00;
    Data32 |= (BIT24 | BIT5);

    PchSbiExecution (
      PID_MMP0,
      R_PCH_PCR_MMP0_LANE_1_OFFSET + R_PCH_PCR_MMP0_IMPREG21,
      PciConfigWrite,
      TRUE,
      &Data32,
      &Response
      );

    //
    // Set cnfgupd (BIT26) of IMPREG22 register to update effective configuration registers from
    // shadow registers. BIT26 - Update Private Registers
    //
    PchSbiExecution (
      PID_MMP0,
      R_PCH_PCR_MMP0_LANE_1_OFFSET + R_PCH_PCR_MMP0_IMPREG22,
      PciConfigRead,
      FALSE,
      &Data32,
      &Response
      );
    Data32 |= BIT26;;
    PchSbiExecution (
      PID_MMP0,
      R_PCH_PCR_MMP0_LANE_1_OFFSET + R_PCH_PCR_MMP0_IMPREG22,
      PciConfigWrite,
      TRUE,
      &Data32,
      &Response
      );

    //
    // Set MMP0 IMPREG23 0x3058[16] 1b
    //
    PchSbiExecution (
      PID_MMP0,
      R_PCH_PCR_MMP0_LANE_1_OFFSET + R_PCH_PCR_MMP0_IMPREG23,
      PciConfigRead,
      FALSE,
      &Data32,
      &Response
      );

    Data32 |= BIT16;

    PchSbiExecution (
      PID_MMP0,
      R_PCH_PCR_MMP0_LANE_1_OFFSET + R_PCH_PCR_MMP0_IMPREG23,
      PciConfigWrite,
      TRUE,
      &Data32,
      &Response
      );

    //
    // Set cnfgupd (BIT26) of IMPREG22 register to update effective configuration registers from
    // shadow registers. BIT26 - Update Private Registers
    //
    PchSbiExecution (
      PID_MMP0,
      R_PCH_PCR_MMP0_LANE_1_OFFSET + R_PCH_PCR_MMP0_IMPREG22,
      PciConfigRead,
      FALSE,
      &Data32,
      &Response
      );
    Data32 |= BIT26;
    PchSbiExecution (
      PID_MMP0,
      R_PCH_PCR_MMP0_LANE_1_OFFSET + R_PCH_PCR_MMP0_IMPREG22,
      PciConfigWrite,
      TRUE,
      &Data32,
      &Response
      );

    //
    // Set MMP0 IMPREG25 0x3060[23:16] 0x0
    //
    PchSbiExecution (
      PID_MMP0,
      R_PCH_PCR_MMP0_LANE_1_OFFSET + R_PCH_PCR_MMP0_IMPREG25,
      PciConfigRead,
      FALSE,
      &Data32,
      &Response
      );

    Data32 &= 0XFF00FFFF;

    PchSbiExecution (
      PID_MMP0,
      R_PCH_PCR_MMP0_LANE_1_OFFSET + R_PCH_PCR_MMP0_IMPREG25,
      PciConfigWrite,
      TRUE,
      &Data32,
      &Response
      );

    //
    // Set cnfgupd (BIT26) of IMPREG22 register to update effective configuration registers from
    // shadow registers. BIT26 - Update Private Registers
    //
    PchSbiExecution (
      PID_MMP0,
      R_PCH_PCR_MMP0_LANE_1_OFFSET + R_PCH_PCR_MMP0_IMPREG22,
      PciConfigRead,
      FALSE,
      &Data32,
      &Response
      );
    Data32 |= BIT26;
    PchSbiExecution (
      PID_MMP0,
      R_PCH_PCR_MMP0_LANE_1_OFFSET + R_PCH_PCR_MMP0_IMPREG22,
      PciConfigWrite,
      TRUE,
      &Data32,
      &Response
      );
  }

  //
  // ModPHY MODPHY_PWRGATE_EN 0xF03C[16] 1b, [8] 1b
  // Access to this PCI register via SBI, PID MMP0 = 0xB0
  // Set cfg_update (BIT0) of CMNREG16 register to copy the shadow register contents to the effective register.
  //
  PchSbiExecution (
    PID_MMP0,
    R_PCH_PCR_MMP0_CMNREG16,
    PciConfigRead,
    FALSE,
    &Data32,
    &Response
    );

  Data32 |= (BIT16 | BIT8 | BIT0);

  PchSbiExecution (
    PID_MMP0,
    R_PCH_PCR_MMP0_CMNREG16,
    PciConfigWrite,
    TRUE,
    &Data32,
    &Response
    );

  //
  // SSIC Global Configuration Control
  // Set 8804[3] to 0b
  // Set 8804[17:16] 01b
  //
  MmioAndThenOr32 (
    XhciMmioBase + R_PCH_XHCI_SSIC_GLOBAL_CONF_CTRL,
    (UINT32) ~(BIT17 | BIT3),
    (UINT32) (BIT16)
    );

  PchStep = PchStepping ();

  //
  // Profile Attributes: Port 1
  //
  if (UsbConfig->SsicConfig.SsicPort[0].Enable == TRUE) {
    //
    // If SPTLP B0 or SPTLP B1 then set xHCI Aux Clock Control Register (XHCI_AUX_CCR) (0x816C) [5](XHCI_AC_GE) 1b
    //
    if ((PchStep == PchLpB0) || (PchStep == PchLpB1)) {
      MmioOr32 (
        (XhciMmioBase + R_PCH_XHCI_AUXCLKCTL),
        (UINT32) BIT5
        );
    }

    //
    // SSIC Series: XHCI MMIO offset 0x8808[4] = 0b (Series B)
    // SSIC Gear: XHCI MMIO offset 0x8808[3:2] = 0b (Gear 1)
    //
    DataAnd32 = (UINT32) ~(BIT4 | BIT3 | BIT2);
    MmioAnd32 (
      XhciMmioBase + R_PCH_XHCI_SSIC_CONF_REG1_PORT_1,
      DataAnd32
      );

    //
    // Issue RRAP cmd to local PHY
    //
    XhciSsicRrapCmd (0x0022C002, 1, XhciMmioBase, &IndexP1, &IndexP2);
    XhciSsicRrapCmd (0x00A2C002, 1, XhciMmioBase, &IndexP1, &IndexP2);

    //
    // Issue RRAP cmd to remote PHY
    //
    XhciSsicRrapCmd (0x00228002, 1, XhciMmioBase, &IndexP1, &IndexP2);
    XhciSsicRrapCmd (0x00A28002, 1, XhciMmioBase, &IndexP1, &IndexP2);

    //
    // Issue RRAP cmd to remote PHY
    //
    XhciSsicRrapCmd (0x33C00A, 1, XhciMmioBase, &IndexP1, &IndexP2);

    //
    // SSIC Disable Data Scrambling
    // XHCI MMIO offset 0x880C[25] = 0x1
    //
    MmioOr32 (
      XhciMmioBase + R_PCH_XHCI_SSIC_CONF_REG2_PORT_1,
      (UINT32) (BIT25)
      );
    //
    // Issue RRAP cmd to remote PHY
    //
    XhciSsicRrapCmd (0x04038001, 1, XhciMmioBase, &IndexP1, &IndexP2);

    //
    // SSIC U0 Stall
    // XHCI MMIO offset 0x8810[2] = 0x1
    //
    MmioOr32 (
      XhciMmioBase + R_PCH_XHCI_SSIC_CONF_REG3_PORT_1,
      (UINT32) (BIT2)
      );
    //
    // Issue RRAP cmd to remote PHY
    //
    XhciSsicRrapCmd (0x04048001, 1, XhciMmioBase, &IndexP1, &IndexP2);

  }
  //
  // Profile Attributes: Port 2
  //

  if (UsbConfig->SsicConfig.SsicPort[1].Enable == TRUE) {
    //
    // If SPTLP B0 or SPTLP B1 then set xHCI Aux Clock Control Register (XHCI_AUX_CCR) (0x816C) [5](XHCI_AC_GE) 1b
    //
    if ((PchStep == PchLpB0) || (PchStep == PchLpB1)) {
      MmioOr32 (
        (XhciMmioBase + R_PCH_XHCI_AUXCLKCTL),
        (UINT32) BIT5
        );
    }

    //
    // SSIC Series: XHCI MMIO offset 0x8838[4] = 0b (Series B)
    // SSIC Gear: XHCI MMIO offset 0x8838[3:2] = 0b (Gear 1)
    //
    DataAnd32 = (UINT32) ~(BIT4 | BIT3 | BIT2);
    MmioAnd32 (
      XhciMmioBase + R_PCH_XHCI_SSIC_CONF_REG1_PORT_2,
      DataAnd32
      );

    //
    // Issue RRAP cmd to local PHY
    //
    XhciSsicRrapCmd (0x0022C002, 2, XhciMmioBase, &IndexP1, &IndexP2);
    XhciSsicRrapCmd (0x00A2C002, 2, XhciMmioBase, &IndexP1, &IndexP2);
    //
    // Issue RRAP cmd to remote PHY
    //
    XhciSsicRrapCmd (0x00228002, 2, XhciMmioBase, &IndexP1, &IndexP2);
    XhciSsicRrapCmd (0x00A28002, 2, XhciMmioBase, &IndexP1, &IndexP2);

    //
    // SSIC Disable Data Scrambling
    // XHCI MMIO offset 0x883C[25] = 0x1
    //
    MmioOr32 (
      XhciMmioBase + R_PCH_XHCI_SSIC_CONF_REG2_PORT_2,
      (UINT32) (BIT25)
      );
    //
    // Issue RRAP cmd to remote PHY
    //
    XhciSsicRrapCmd (0x04038001, 2, XhciMmioBase, &IndexP1, &IndexP2);

    //
    // SSIC U0 Stall
    // XHCI MMIO offset 0x8840[2] = 0x1
    //
    MmioOr32 (
      XhciMmioBase + R_PCH_XHCI_SSIC_CONF_REG3_PORT_2,
      (UINT32) (BIT2)
      );
    //
    // Issue RRAP cmd to remote PHY
    //
    XhciSsicRrapCmd (0x04048001, 2, XhciMmioBase, &IndexP1, &IndexP2);

  }

  if (UsbConfig->SsicConfig.SsicPort[0].Enable == TRUE) {
    //
    // SSIC Port N Register Access Control: Port 1
    // Address Offset: 8904h
    // Value [25] 1h, [23] 1h
    //
    MmioOr32 (
      XhciMmioBase + R_PCH_XHCI_SSIC_ACCESS_CONT_PORT_1,
      (UINT32) (BIT25 | BIT23)
      );
  }

  if (UsbConfig->SsicConfig.SsicPort[1].Enable == TRUE) {
    //
    // SSIC Port N Register Access Control: Port 2
    // Address Offset: 8A14h
    // Value [25] 1h, [23] 1h
    //
    MmioOr32 (
      XhciMmioBase + R_PCH_XHCI_SSIC_ACCESS_CONT_PORT_2,
      (UINT32) (BIT25 | BIT23)
      );
  }

  //
  // SSIC Configuration Register 2 Port 1
  // Address Offset: 880Ch
  // Value [30] 1h, [31] 1h if the port has no physical SSIC Device connected
  //
  if ((UsbConfig->SsicConfig.SsicPort[0].Enable == TRUE) && (MmioRead32 (XhciMmioBase + R_PCH_XHCI_STRAP2) & BIT1)) {
    Data32 = B_PCH_XHCI_SSIC_CONF_REG2_PROG_DONE;
  } else {
    Data32 = B_PCH_XHCI_SSIC_CONF_REG2_PORT_UNUSED | B_PCH_XHCI_SSIC_CONF_REG2_PROG_DONE;
  }

  MmioAnd32 (
    XhciMmioBase + R_PCH_XHCI_SSIC_CONF_REG2_PORT_1,
    (UINT32) ~(BIT24 + BIT23 + BIT22 + BIT21)
    );

  MmioOr32 (
    XhciMmioBase + R_PCH_XHCI_SSIC_CONF_REG2_PORT_1,
    Data32
    );

  //
  // SSIC Configuration Register 2 Port 2
  // Address Offset: 883Ch
  // Value [30] 1h, [31] 1h if the port has no physical SSIC Device connected
  //
  if ((UsbConfig->SsicConfig.SsicPort[1].Enable== TRUE) && (MmioRead32 (XhciMmioBase + R_PCH_XHCI_STRAP2) & BIT2)) {
    Data32 = B_PCH_XHCI_SSIC_CONF_REG2_PROG_DONE;
  } else {
    Data32 = B_PCH_XHCI_SSIC_CONF_REG2_PORT_UNUSED | B_PCH_XHCI_SSIC_CONF_REG2_PROG_DONE;
  }
  MmioOr32 (
    XhciMmioBase + R_PCH_XHCI_SSIC_CONF_REG2_PORT_2,
    Data32
    );

  if (XhciIsS5 () == TRUE &&
      (MmioRead32 (XhciMmioBase + R_PCH_XHCI_STRAP2) & (BIT2 | BIT1)) &&
      ((UsbConfig->SsicConfig.SsicPort[0].Enable == TRUE) ||
       (UsbConfig->SsicConfig.SsicPort[1].Enable == TRUE))) {

    MicroSecondDelay (100);  // 100us

    if (GetPchSeries () == PchLp) {
      //
      // Clear SSIC ports PP bits
      //
      MmioAnd32 (
        XhciMmioBase + R_PCH_LP_XHCI_PORTSC02USB3,
        (UINT32) ~(B_PCH_XHCI_PORTSCXUSB3_PP)
        );
      MmioAnd32 (
        XhciMmioBase + R_PCH_LP_XHCI_PORTSC03USB3,
        (UINT32) ~(B_PCH_XHCI_PORTSCXUSB3_PP)
        );
      //
      // Wait for 150ms for Host to complete the DSP Disconnect protocol
      //
      MicroSecondDelay (150 * 1000);

      MmioOr32 (
        XhciMmioBase + R_PCH_LP_XHCI_PORTSC02USB3,
        (UINT32) B_PCH_XHCI_PORTSCXUSB3_PP
        );

      MmioOr32 (
        XhciMmioBase + R_PCH_LP_XHCI_PORTSC03USB3,
        (UINT32) B_PCH_XHCI_PORTSCXUSB3_PP
        );
    } else {
      MmioAnd32 (
        XhciMmioBase + R_PCH_H_XHCI_PORTSC02USB3,
        (UINT32) ~(B_PCH_XHCI_PORTSCXUSB3_PP)
        );
      MmioAnd32 (
        XhciMmioBase + R_PCH_H_XHCI_PORTSC03USB3,
        (UINT32) ~(B_PCH_XHCI_PORTSCXUSB3_PP)
        );
      //
      // Wait for 150ms for Host to complete the DSP Disconnect protocol
      //
      MicroSecondDelay (150 * 1000);

      MmioOr32 (
        XhciMmioBase + R_PCH_H_XHCI_PORTSC02USB3,
        (UINT32) B_PCH_XHCI_PORTSCXUSB3_PP
        );
      MmioOr32 (
        XhciMmioBase + R_PCH_H_XHCI_PORTSC03USB3,
        (UINT32) B_PCH_XHCI_PORTSCXUSB3_PP
        );
    }

    //
    // Set HCRST after S5, HCRST is cleared by HW when reset process is complete
    //
    MmioOr32 (
      XhciMmioBase + R_PCH_XHCI_USBCMD,
      (UINT32) B_PCH_XHCI_USBCMD_RST
      );
    //
    // Waiting for reset complete
    // The controller requires that its MMIO space not be accessed for a minimum of 1 ms after
    // an HCRST is triggered which includes reading the HCRST bit
    //
    MicroSecondDelay (1000);  // 1ms
    for (Iteration = 0; Iteration < 8000; Iteration++) {
      if ((MmioRead32 (XhciMmioBase + R_PCH_XHCI_USBCMD) & B_PCH_XHCI_USBCMD_RST) == 0) {
        break;
      }
      MicroSecondDelay (100);  // 100us
    }
  }
}
#endif //MINTREE_FLAG
/**
  Tune the USB 3.0 signals quality.

  @param[in]  UsbConfig                 The PCH Policy for USB configuration
**/
VOID
XhciUsb3Tune (
  IN  CONST PCH_USB_CONFIG    *UsbConfig
  )
{
  HSIO_LANE   HsioLane;
  UINT16      Offset;
  UINT8       Index;
  UINT8       PortId;
  UINT8       LaneOwner;
  UINT8       LaneNum;
  EFI_STATUS  Status;

  Offset = 0;

  DEBUG ((DEBUG_INFO, "XhciUsb3Tune() Start\n"));

  for (Index = 0; Index < GetPchXhciMaxUsb3PortNum (); Index++) {

    if ((UsbConfig->PortUsb30[Index].HsioTxDeEmphEnable == FALSE) &&
        (UsbConfig->PortUsb30[Index].HsioTxDownscaleAmpEnable == FALSE)) {
      continue;
    }

    Status = PchGetUsb3LaneNum (Index, &LaneNum);
    if (EFI_ERROR (Status)) {
      continue;
    }
    PchGetLaneInfo (LaneNum, &PortId, &LaneOwner);

    if (LaneOwner == V_PCH_PCR_FIA_LANE_OWN_USB3) {
      HsioGetLane (LaneNum, &HsioLane);
      //
      //Step 1: Make changes to any of the TX (Transmit) ModPHY Register Bit Fields
      //
      //
      // USB 3.0 TX Output -3.5dB De-Emphasis Adjustment Setting (ow2tapgen2deemph3p5)
      // HSIO_TX_DWORD5[21:16]
      //
      if (UsbConfig->PortUsb30[Index].HsioTxDeEmphEnable == TRUE) {
        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_TX_DWORD5;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          (UINT32) ~B_PCH_HSIO_TX_DWORD5_OW2TAPGEN2DEEMPH3P5_5_0,
          (UINT32) (UsbConfig->PortUsb30[Index].HsioTxDeEmph << N_PCH_HSIO_TX_DWORD5_OW2TAPGEN2DEEMPH3P5_5_0));
      }

      //
      // USB 3.0 TX Output Downscale Amplitude Adjustment (orate01margin)
      // HSIO_TX_DWORD8[21:16]
      //
      if (UsbConfig->PortUsb30[Index].HsioTxDownscaleAmpEnable == TRUE) {
        Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_TX_DWORD8;
        PchPcrAndThenOr32 (
          (PCH_SBI_PID) PortId,
          (UINT16) Offset,
          (UINT32) ~B_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0,
          (UINT32) (UsbConfig->PortUsb30[Index].HsioTxDownscaleAmp << N_PCH_HSIO_TX_DWORD8_ORATE01MARGIN_5_0));
      }

      //
      // Step 2: Clear HSIO_TX_DWORD19 Bit[1] (o_calcinit bit) to 0b
      //
      Offset = HsioLane.Base + (UINT16) R_PCH_HSIO_TX_DWORD19;
      PchPcrAndThenOr32 (
        (PCH_SBI_PID) PortId,
        (UINT16) Offset,
        (UINT32) ~(BIT1),
        (UINT32) (0));

      //
      // Step 3: Set HSIO_TX_DWORD19 Bit[1] (o_calcinit) to 1b
      //
      PchPcrAndThenOr32 (
        (PCH_SBI_PID) PortId,
        (UINT16) Offset,
        (UINT32) ~(0),
        (UINT32) (BIT1));
    }
  }

  DEBUG ((DEBUG_INFO, "XhciUsb3Tune() End\n"));
}
