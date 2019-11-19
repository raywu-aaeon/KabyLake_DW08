//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************


/** @file Sb.h
    South Bridge header file, define all the South Bridge
    specific equates and structures in this file.

**/

#ifndef  _SB_H_                  // To Avoid this header get compiled twice
#define  _SB_H_

#include <Token.h>

#if SB_DEVICE_REGS_RC_DEFINITION_SUPPORT
#include SB_DEVICE_REGS_RC_DEFINITION_HEADER
#endif

#define SB_PCI_CFG_ADDRESS(bus, dev, func, reg) \
            (UINT64) ((((UINT8)(bus) << 24) + ((UINT8)(dev) << 16) + \
            ((UINT8)(func) << 8) + ((UINT8)(reg))) & 0xffffffff)

#ifdef PCIEX_BASE_ADDRESS 
#define SB_PCIE_CFG_ADDRESS(bus, dev, func, reg) \
            ((UINTN)(PCIEX_BASE_ADDRESS + ((UINT8)(bus) << 20) + \
            ((UINT8)(dev) << 15) + ((UINT8)(func) << 12) + (reg)))
#define SMBUS_PCIE_CFG_ADDRESS(Register) \
    ( \
      (UINTN) PCIEX_BASE_ADDRESS + (UINTN) (SB_SMBUS_BUS << 20) + (UINTN) (SB_SMBUS_DEV << 15) + (UINTN) \
        (SB_SMBUS_FUN << 12) + (Register) \
    )
#endif

#define PciD31F0RegBase           PCIEX_BASE_ADDRESS + (UINT32) (31 << 15)

#ifndef PCIBUS_VERSION
#define PCIBUS_VERSION ( PCI_BUS_MAJOR_VER * 10000 + \
                         PCI_BUS_MINOR_VER * 100 + \
                         PCI_BUS_REVISION )
#endif 

#define SB_PCR_ADDRESS(Pid, Offset)    (PCR_BASE_ADDRESS | ((UINT8)(Pid) << 16) | (UINT16)(Offset))

// 8259 Hardware definitions

#define LEGACY_MODE_BASE_VECTOR_MASTER                    0x08
#define LEGACY_MODE_BASE_VECTOR_SLAVE                     0x10
#define LEGACY_8259_CONTROL_REGISTER_MASTER               0x20
#define LEGACY_8259_MASK_REGISTER_MASTER                  0x21
#define LEGACY_8259_CONTROL_REGISTER_SLAVE                0xa0
#define LEGACY_8259_MASK_REGISTER_SLAVE                   0xa1
#define LEGACY_8259_EDGE_LEVEL_TRIGGERED_REGISTER_MASTER  0x4d0
#define LEGACY_8259_EDGE_LEVEL_TRIGGERED_REGISTER_SLAVE   0x4d1
#define LEGACY_8259_EOI                                   0x20

// 8254 Timer definitions
#define LEGACY_TIMER_0_COUNT    0x40
#define LEGACY_TIMER_1_COUNT    0x41
#define LEGACY_TIMER_CTRL       0x43

#define SLP_S5                  7       // Porting Required.

#define NUM_BITS_IN_ACPI_TIMER  24      // Programmed to 24 not 32
// This is the maximum possible bits in the timer.
// Currently this is 32 according to the spec.
#define MAX_ACPI_TIMER_BITS     32

typedef struct DMA_INIT_tag{
  UINT8  PortAddr;
  UINT8  Value;
} DMA_INIT;

#define SB_ASL_BUFFER_PTR_GUID { 0x1f33c25, 0x764d, 0x43ea, 0xae, 0xea, 0x6b, \
                             0x5a, 0x41, 0xf3, 0xf3, 0xe8 }

#define SB_ASL_BUFFER_PTR_VARIABLE L"SbAslBufferPtrVar"

typedef struct {
    UINT8   SbAslByte0;
    UINT8   SbAslByte1;
    UINT8   SbAslByte2;
    UINT8   SbAslByte3;
    UINT8   SbAslByte4;
    UINT8   SbAslByte5;
    UINT8   SbAslByte6;
    UINT8   SbAslByte7;
    UINT8   SbAslByte8;
    UINT8   SbAslByte9;
    UINT8   SbAslByte10;
    UINT8   SbAslByte11;
    UINT8   SbAslByte12;
    UINT8   SbAslByte13;
    UINT8   SbAslByte14;
    UINT8   SbAslByte15;
} SB_ASL_BUFFER;

#define SB_WARM_RESET_GUID {0xb8cafa84, 0x4593, 0x4aa9, 0xae, 0xf7, 0x8e, \
                            0x68, 0x6e, 0xb0, 0x73, 0x20}

#define SB_WARM_RESET_VARIABLE L"SbWarmResetVar"

#define SB_WARM_RESET_TAG 0x49735752    // 'IsWR'

#define RESET_PORT        0x0CF9
#define CLEAR_RESET_BITS  0x0F9
#define COLD_RESET        0x02 // Set bit 1 for cold reset
#define RST_CPU           0x04 // Setting this bit triggers a reset of the CPU

typedef enum _SB_RESET_TYPE
{
    HardReset = 0,
    SoftReset,
    ShutDown,
    FullReset = 0x80,
    GlobalReset
} SB_RESET_TYPE;

///
/// Definition for SBI PID
/// The PCH_SBI_PID defines the PID for PCR MMIO programming and PCH SBI programming as well.
///

//----------------------------------------------------------------------------
// INTEL SB GPIO Definition
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// INTEL PCH Device Bus Number Equates
//----------------------------------------------------------------------------
#define SB_BUS                  0               // South Bridge Bus Number
#define SB_LPC_BUS              SB_BUS          // Low Pin Count Interface
#define SB_ESPI_BUS             SB_BUS          // Enhanced SPI(eSPI) Interface
#define SB_ISH_BUS              SB_BUS          // Integrated Sensor Hub
#define SB_XHCI_BUS             SB_BUS          // xHCI Controller
#define SB_XDCI_BUS             SB_BUS          // xDCI (OTG) USB Device Controller
#define SB_THERMAL_BUS          SB_BUS          // Thermal Controller
#define SB_CIO2_BUS             SB_BUS          // Camera IO Host Controller
#define SB_SIO_I2C0_BUS         SB_BUS          // Serial IO I2C 0 Controller
#define SB_SIO_I2C1_BUS         SB_BUS          // Serial IO I2C 1 Controller
#define SB_SIO_I2C2_BUS         SB_BUS          // Serial IO I2C 2 Controller
#define SB_SIO_I2C3_BUS         SB_BUS          // Serial IO I2C 3 Controller
#define SB_HECI_BUS             SB_BUS          // ME HECI Controller
#define SB_HECI2_BUS            SB_BUS          // ME HECI2 Controller
#define SB_HECI3_BUS            SB_BUS          // ME HECI3 Controller
#define SB_IDER_BUS             SB_BUS          // ME IDER Controller
#define SB_KT_BUS               SB_BUS          // ME KT Controller
#define SB_SATA_BUS             SB_BUS          // Serial ATA Controller
#define SB_SIO_UART2_BUS        SB_BUS          // Serial IO UART 2 Interface
#define SB_SIO_I2C5_BUS         SB_BUS          // Serial IO I2C 5 Controller
#define SB_SIO_I2C4_BUS         SB_BUS          // Serial IO I2C 4 Controller
#define SB_PCIERP16_BUS         SB_BUS          // PCIE Root Port 16 
#define SB_PCIERP17_BUS         SB_BUS          // PCIE Root Port 17 
#define SB_PCIERP18_BUS         SB_BUS          // PCIE Root Port 18
#define SB_PCIERP19_BUS         SB_BUS          // PCIE Root Port 19
#define SB_PCIERP20_BUS         SB_BUS          // PCIE Root Port 20 
#define SB_PCIERP21_BUS         SB_BUS          // PCIE Root Port 21 
#define SB_PCIERP22_BUS         SB_BUS          // PCIE Root Port 22
#define SB_PCIERP23_BUS         SB_BUS          // PCIE Root Port 23
#define SB_PCIERP00_BUS         SB_BUS          // PCIE Root Port  0
#define SB_PCIERP01_BUS         SB_BUS          // PCIE Root Port  1 
#define SB_PCIERP02_BUS         SB_BUS          // PCIE Root Port  2
#define SB_PCIERP03_BUS         SB_BUS          // PCIE Root Port  3
#define SB_PCIERP04_BUS         SB_BUS          // PCIE Root Port  4
#define SB_PCIERP05_BUS         SB_BUS          // PCIE Root Port  5
#define SB_PCIERP06_BUS         SB_BUS          // PCIE Root Port  6
#define SB_PCIERP07_BUS         SB_BUS          // PCIE Root Port  7
#define SB_PCIERP08_BUS         SB_BUS          // PCIE Root Port  8
#define SB_PCIERP09_BUS         SB_BUS          // PCIE Root Port  9
#define SB_PCIERP10_BUS         SB_BUS          // PCIE Root Port 10
#define SB_PCIERP11_BUS         SB_BUS          // PCIE Root Port 11
#define SB_PCIERP12_BUS         SB_BUS          // PCIE Root Port 12
#define SB_PCIERP13_BUS         SB_BUS          // PCIE Root Port 13
#define SB_PCIERP14_BUS         SB_BUS          // PCIE Root Port 14
#define SB_PCIERP15_BUS         SB_BUS          // PCIE Root Port 15
#define SB_SIO_UART0_BUS        SB_BUS          // Serial IO UART 0 Interface
#define SB_SIO_UART1_BUS        SB_BUS          // Serial IO UART 1 Interface
#define SB_SIO_GSPI0_BUS        SB_BUS          // Serial IO Generic SPI 0
#define SB_SIO_GSPI1_BUS        SB_BUS          // Serial IO Generic SPI 1
#define SB_SCS_EMMC_BUS         SB_BUS          // SCS eMMC Controller 
#define SB_SCS_SDIO_BUS         SB_BUS          // SCS Secure Digital Input Output Controller
#define SB_SCS_SDCARD_BUS       SB_BUS          // SCS SDCARD Controller
#define SB_P2SB_BUS             SB_BUS          // Primary To Sideband Bridge Controller
#define SB_PMC_BUS              SB_BUS          // Power Management Controller
#define SB_HDA_BUS              SB_BUS          // High Definition Audio Controller Interface
#define SB_SMBUS_BUS            SB_BUS          // SMBus Controller Interface
#define SB_SPI_BUS              SB_BUS          // Generic Serial Peripheral Interface
#define SB_LAN_BUS              SB_BUS          // Gigabit LAN Controller
#define SB_TRACE_HUB_BUS        SB_BUS          // Trace Hub(North Peak) Controller

// AmiChipsetModulePkg Template definition
#define SB_PCIE_BUS             SB_BUS
#define LPC_BUS                 SB_LPC_BUS

//----------------------------------------------------------------------------
// INTEL PCH Device Device Number Equates
//----------------------------------------------------------------------------
#define SB_DEV                  0x1F            // South Bridge Device Number
#define SB_LPC_DEV              SB_DEV          // Low Pin Count Interface
#define SB_ESPI_DEV             SB_DEV          // Enhanced SPI(eSPI) Interface
#define SB_ISH_DEV              0x13            // Integrated Sensor Hub
#define SB_XHCI_DEV             0x14            // xHCI Controller
#define SB_XDCI_DEV             0x14            // xDCI (OTG) USB Device Controller
#define SB_THERMAL_DEV          0x14            // Thermal Controller
#define SB_CIO2_DEV             0x14            // Camera IO Host Controller
#define SB_SIO_I2C0_DEV         0x15            // Serial IO I2C 0 Controller
#define SB_SIO_I2C1_DEV         0x15            // Serial IO I2C 1 Controller
#define SB_SIO_I2C2_DEV         0x15            // Serial IO I2C 2 Controller
#define SB_SIO_I2C3_DEV         0x15            // Serial IO I2C 3 Controller
#define SB_HECI_DEV             0x16            // ME HECI Controller
#define SB_HECI2_DEV            0x16            // ME HECI2 Controller
#define SB_HECI3_DEV            0x16            // ME HECI3 Controller
#define SB_IDER_DEV             0x16            // ME IDER Controller
#define SB_KT_DEV               0x16            // ME KT Controller
#define SB_SATA_DEV             0x17            // Serial ATA Controller
#define SB_SIO_UART2_DEV        0x19            // Serial IO UART 2 Interface
#define SB_SIO_I2C5_DEV         0x19            // Serial IO I2C 5 Controller
#define SB_SIO_I2C4_DEV         0x19            // Serial IO I2C 4 Controller
#define SB_PCIERP16_DEV         0x1B            // PCIE Root Port 16 
#define SB_PCIERP17_DEV         0x1B            // PCIE Root Port 17 
#define SB_PCIERP18_DEV         0x1B            // PCIE Root Port 18
#define SB_PCIERP19_DEV         0x1B            // PCIE Root Port 19
#define SB_PCIERP20_DEV         0x1B            // PCIE Root Port 20 
#define SB_PCIERP21_DEV         0x1B            // PCIE Root Port 21 
#define SB_PCIERP22_DEV         0x1B            // PCIE Root Port 22
#define SB_PCIERP23_DEV         0x1B            // PCIE Root Port 23
#define SB_PCIERP00_DEV         0x1C            // PCIE Root Port  0
#define SB_PCIERP01_DEV         0x1C            // PCIE Root Port  1 
#define SB_PCIERP02_DEV         0x1C            // PCIE Root Port  2
#define SB_PCIERP03_DEV         0x1C            // PCIE Root Port  3
#define SB_PCIERP04_DEV         0x1C            // PCIE Root Port  4
#define SB_PCIERP05_DEV         0x1C            // PCIE Root Port  5
#define SB_PCIERP06_DEV         0x1C            // PCIE Root Port  6
#define SB_PCIERP07_DEV         0x1C            // PCIE Root Port  7
#define SB_PCIERP08_DEV         0x1D            // PCIE Root Port  8
#define SB_PCIERP09_DEV         0x1D            // PCIE Root Port  9
#define SB_PCIERP10_DEV         0x1D            // PCIE Root Port 10
#define SB_PCIERP11_DEV         0x1D            // PCIE Root Port 11
#define SB_PCIERP12_DEV         0x1D            // PCIE Root Port 12
#define SB_PCIERP13_DEV         0x1D            // PCIE Root Port 13
#define SB_PCIERP14_DEV         0x1D            // PCIE Root Port 14
#define SB_PCIERP15_DEV         0x1D            // PCIE Root Port 15
#define SB_SIO_UART0_DEV        0x1E            // Serial IO UART 0 Interface
#define SB_SIO_UART1_DEV        0x1E            // Serial IO UART 1 Interface
#define SB_SIO_GSPI0_DEV        0x1E            // Serial IO Generic SPI 0
#define SB_SIO_GSPI1_DEV        0x1E            // Serial IO Generic SPI 1
#define SB_SCS_EMMC_DEV         0x1E            // SCS eMMC Controller 
#define SB_SCS_SDIO_DEV         0x1E            // SCS Secure Digital Input Output Controller
#define SB_SCS_SDCARD_DEV       0x1E            // SCS SDCARD Controller
#define SB_P2SB_DEV             0x1F            // Primary To Sideband Bridge Controller
#define SB_PMC_DEV              0x1F            // Power Management Controller
#define SB_HDA_DEV              0x1F            // High Definition Audio Controller Interface
#define SB_SMBUS_DEV            0x1F            // SMBus Controller Interface
#define SB_SPI_DEV              0x1F            // Generic Serial Peripheral Interface
#define SB_LAN_DEV              0x1F            // Gigabit LAN Controller
#define SB_TRACE_HUB_DEV        0x1F            // Trace Hub(North Peak) Controller

// AmiChipsetModulePkg Template definition
#define LPC_DEVICE              SB_LPC_DEV

//----------------------------------------------------------------------------
// INTEL PCH Device Function Number Equates
//----------------------------------------------------------------------------
#define SB_FUN                  0x00            // South Bridge Function Number
#define SB_LPC_FUN              SB_FUN          // Low Pin Count Interface
#define SB_ESPI_FUN             SB_FUN          // Enhanced SPI(eSPI) Interface
#define SB_ISH_FUN              0x00            // Integrated Sensor Hub
#define SB_XHCI_FUN             0x00            // xHCI Controller
#define SB_XDCI_FUN             0x01            // xDCI (OTG) USB Device Controller
#define SB_THERMAL_FUN          0x02            // Thermal Controller
#define SB_CIO2_FUN             0x03            // Camera IO Host Controller
#define SB_SIO_I2C0_FUN         0x00            // Serial IO I2C 0 Controller
#define SB_SIO_I2C1_FUN         0x01            // Serial IO I2C 1 Controller
#define SB_SIO_I2C2_FUN         0x02            // Serial IO I2C 2 Controller
#define SB_SIO_I2C3_FUN         0x03            // Serial IO I2C 3 Controller
#define SB_HECI_FUN             0x00            // ME HECI Controller
#define SB_HECI2_FUN            0x01            // ME HECI2 Controller
#define SB_HECI3_FUN            0x04            // ME HECI3 Controller
#define SB_IDER_FUN             0x02            // ME IDER Controller
#define SB_KT_FUN               0x03            // ME KT Controller
#define SB_SATA_FUN             0x00            // Serial ATA Controller
#define SB_SIO_UART2_FUN        0x00            // Serial IO UART 2 Interface
#define SB_SIO_I2C5_FUN         0x01            // Serial IO I2C 5 Controller
#define SB_SIO_I2C4_FUN         0x02            // Serial IO I2C 4 Controller
#define SB_PCIERP16_FUN         0x00            // PCIE Root Port 16 
#define SB_PCIERP17_FUN         0x01            // PCIE Root Port 17 
#define SB_PCIERP18_FUN         0x02            // PCIE Root Port 18
#define SB_PCIERP19_FUN         0x03            // PCIE Root Port 19
#define SB_PCIERP20_FUN         0x04            // PCIE Root Port 20 
#define SB_PCIERP21_FUN         0x05            // PCIE Root Port 21 
#define SB_PCIERP22_FUN         0x06            // PCIE Root Port 22
#define SB_PCIERP23_FUN         0x07            // PCIE Root Port 23
#define SB_PCIERP00_FUN         0x00            // PCIE Root Port  0
#define SB_PCIERP01_FUN         0x01            // PCIE Root Port  1 
#define SB_PCIERP02_FUN         0x02            // PCIE Root Port  2
#define SB_PCIERP03_FUN         0x03            // PCIE Root Port  3
#define SB_PCIERP04_FUN         0x04            // PCIE Root Port  4
#define SB_PCIERP05_FUN         0x05            // PCIE Root Port  5
#define SB_PCIERP06_FUN         0x06            // PCIE Root Port  6
#define SB_PCIERP07_FUN         0x07            // PCIE Root Port  7
#define SB_PCIERP08_FUN         0x00            // PCIE Root Port  8
#define SB_PCIERP09_FUN         0x01            // PCIE Root Port  9
#define SB_PCIERP10_FUN         0x02            // PCIE Root Port 10
#define SB_PCIERP11_FUN         0x03            // PCIE Root Port 11
#define SB_PCIERP12_FUN         0x04            // PCIE Root Port 12
#define SB_PCIERP13_FUN         0x05            // PCIE Root Port 13
#define SB_PCIERP14_FUN         0x06            // PCIE Root Port 14
#define SB_PCIERP15_FUN         0x07            // PCIE Root Port 15
#define SB_SIO_UART0_FUN        0x00            // Serial IO UART 0 Interface
#define SB_SIO_UART1_FUN        0x01            // Serial IO UART 1 Interface
#define SB_SIO_GSPI0_FUN        0x02            // Serial IO Generic SPI 0
#define SB_SIO_GSPI1_FUN        0x03            // Serial IO Generic SPI 1
#define SB_SCS_EMMC_FUN         0x04            // SCS eMMC Controller 
#define SB_SCS_SDIO_FUN         0x05            // SCS Secure Digital Input Output Controller
#define SB_SCS_SDCARD_FUN       0x06            // SCS SDCARD Controller
#define SB_P2SB_FUN             0x01            // Primary To Sideband Bridge Controller
#define SB_PMC_FUN              0x02            // Power Management Controller
#define SB_HDA_FUN              0x03            // High Definition Audio Controller Interface
#define SB_SMBUS_FUN            0x04            // SMBus Controller Interface
#define SB_SPI_FUN              0x05            // Generic Serial Peripheral Interface
#define SB_LAN_FUN              0x06            // Gigabit LAN Controller
#define SB_TRACE_HUB_FUN        0x07            // Trace Hub(North Peak) Controller

// AmiChipsetModulePkg Template definition
#define LPC_FUNC                SB_LPC_FUN

//----------------------------------------------------------------------------
// INTEL PCH PCI Bus/Device/Function/Register Number Macros
//----------------------------------------------------------------------------
#define SB_LPC_REG(Reg)                 SB_PCI_CFG_ADDRESS(SB_LPC_BUS, SB_LPC_DEV, \
                                                           SB_LPC_FUN, Reg)
#define SB_ISH_REG(Reg)                 SB_PCI_CFG_ADDRESS(SB_ISH_BUS, SB_ISH_DEV, \
                                                           SB_ISH_FUN, Reg)
#define SB_HECI_REG(Reg)                SB_PCI_CFG_ADDRESS(SB_HECI_BUS, SB_HECI_DEV, \
                                                           SB_HECI_FUN, Reg)
#define SB_HECI2_REG(Reg)               SB_PCI_CFG_ADDRESS(SB_HECI2_BUS, SB_HECI2_DEV, \
                                                           SB_HECI2_FUN, Reg)
#define SB_HECI3_REG(Reg)               SB_PCI_CFG_ADDRESS(SB_HECI3_BUS, SB_HECI3_DEV, \
                                                           SB_HECI3_FUN, Reg)
#define SB_IDER_REG(Reg)                SB_PCI_CFG_ADDRESS(SB_IDER_BUS, SB_IDER_DEV, \
                                                           SB_IDER_FUN, Reg)
#define SB_KT_REG(Reg)                  SB_PCI_CFG_ADDRESS(SB_KT_BUS, SB_KT_DEV, \
                                                           SB_KT_FUN, Reg)
#define SB_XHCI_REG(Reg)                SB_PCI_CFG_ADDRESS(SB_XHCI_BUS, SB_XHCI_DEV, \
                                                           SB_XHCI_FUN, Reg)
#define SB_XDCI_REG(Reg)                SB_PCI_CFG_ADDRESS(SB_XDCI_BUS, SB_XDCI_DEV, \
                                                           SB_XDCI_FUN, Reg)
#define SB_CIO2_REG(Reg)                SB_PCI_CFG_ADDRESS(SB_CIO2_BUS, SB_CIO2_DEV, \
                                                           SB_CIO2_FUN, Reg)
#define SB_SATA_REG(Reg)                SB_PCI_CFG_ADDRESS(SB_SATA_BUS, SB_SATA_DEV, \
                                                           SB_SATA_FUN, Reg)
#define SB_SMBUS_REG(Reg)               SB_PCI_CFG_ADDRESS(SB_SMBUS_BUS, SB_SMBUS_DEV, \
                                                           SB_SMBUS_FUN, Reg)
#define SB_THERMAL_REG(Reg)             SB_PCI_CFG_ADDRESS(SB_THERMAL_BUS, SB_THERMAL_DEV, \
                                                           SB_THERMAL_FUN, Reg)
#define SB_HDA_REG(Reg)                 SB_PCI_CFG_ADDRESS(SB_HDA_BUS, SB_HDA_DEV, \
                                                           SB_HDA_FUN, Reg)
#define SB_LAN_REG(Reg)                 SB_PCI_CFG_ADDRESS(SB_LAN_BUS, SB_LAN_DEV, \
                                                           SB_LAN_FUN, Reg)
#define SB_PCIERP0_REG(Reg)             SB_PCI_CFG_ADDRESS(SB_PCIERP00_BUS, SB_PCIERP00_DEV, \
                                                           SB_PCIERP00_FUN, Reg)
#define SB_PCIERP1_REG(Reg)             SB_PCI_CFG_ADDRESS(SB_PCIERP01_BUS, SB_PCIERP01_DEV, \
                                                           SB_PCIERP01_FUN, Reg)
#define SB_PCIERP2_REG(Reg)             SB_PCI_CFG_ADDRESS(SB_PCIERP02_BUS, SB_PCIERP02_DEV, \
                                                           SB_PCIERP02_FUN, Reg)
#define SB_PCIERP3_REG(Reg)             SB_PCI_CFG_ADDRESS(SB_PCIERP03_BUS, SB_PCIERP03_DEV, \
                                                           SB_PCIERP03_FUN, Reg)
#define SB_PCIERP4_REG(Reg)             SB_PCI_CFG_ADDRESS(SB_PCIERP04_BUS, SB_PCIERP04_DEV, \
                                                           SB_PCIERP04_FUN, Reg)
#define SB_PCIERP5_REG(Reg)             SB_PCI_CFG_ADDRESS(SB_PCIERP05_BUS, SB_PCIERP05_DEV, \
                                                           SB_PCIERP05_FUN, Reg)
#define SB_PCIERP6_REG(Reg)             SB_PCI_CFG_ADDRESS(SB_PCIERP06_BUS, SB_PCIERP06_DEV, \
                                                           SB_PCIERP06_FUN, Reg)
#define SB_PCIERP7_REG(Reg)             SB_PCI_CFG_ADDRESS(SB_PCIERP07_BUS, SB_PCIERP07_DEV, \
                                                           SB_PCIERP07_FUN, Reg)
#define SB_PCIERP8_REG(Reg)             SB_PCI_CFG_ADDRESS(SB_PCIERP08_BUS, SB_PCIERP08_DEV, \
                                                           SB_PCIERP08_FUN, Reg)
#define SB_PCIERP9_REG(Reg)             SB_PCI_CFG_ADDRESS(SB_PCIERP09_BUS, SB_PCIERP09_DEV, \
                                                           SB_PCIERP09_FUN, Reg)
#define SB_PCIERP10_REG(Reg)            SB_PCI_CFG_ADDRESS(SB_PCIERP10_BUS, SB_PCIERP10_DEV, \
                                                           SB_PCIERP10_FUN, Reg)
#define SB_PCIERP11_REG(Reg)            SB_PCI_CFG_ADDRESS(SB_PCIERP11_BUS, SB_PCIERP11_DEV, \
                                                           SB_PCIERP11_FUN, Reg)
#define SB_PCIERP12_REG(Reg)            SB_PCI_CFG_ADDRESS(SB_PCIERP12_BUS, SB_PCIERP12_DEV, \
                                                           SB_PCIERP12_FUN, Reg)
#define SB_PCIERP13_REG(Reg)            SB_PCI_CFG_ADDRESS(SB_PCIERP13_BUS, SB_PCIERP13_DEV, \
                                                           SB_PCIERP13_FUN, Reg)
#define SB_PCIERP14_REG(Reg)            SB_PCI_CFG_ADDRESS(SB_PCIERP14_BUS, SB_PCIERP14_DEV, \
                                                           SB_PCIERP14_FUN, Reg)
#define SB_PCIERP15_REG(Reg)            SB_PCI_CFG_ADDRESS(SB_PCIERP15_BUS, SB_PCIERP15_DEV, \
                                                           SB_PCIERP15_FUN, Reg)
#define SB_PCIERP16_REG(Reg)            SB_PCI_CFG_ADDRESS(SB_PCIERP16_BUS, SB_PCIERP16_DEV, \
                                                           SB_PCIERP16_FUN, Reg)
#define SB_PCIERP17_REG(Reg)            SB_PCI_CFG_ADDRESS(SB_PCIERP17_BUS, SB_PCIERP17_DEV, \
                                                           SB_PCIERP17_FUN, Reg)
#define SB_PCIERP18_REG(Reg)            SB_PCI_CFG_ADDRESS(SB_PCIERP18_BUS, SB_PCIERP18_DEV, \
                                                           SB_PCIERP18_FUN, Reg)
#define SB_PCIERP19_REG(Reg)            SB_PCI_CFG_ADDRESS(SB_PCIERP19_BUS, SB_PCIERP19_DEV, \
                                                           SB_PCIERP19_FUN, Reg)
#define SB_PCIERP20_REG(Reg)            SB_PCI_CFG_ADDRESS(SB_PCIERP20_BUS, SB_PCIERP20_DEV, \
                                                           SB_PCIERP20_FUN, Reg)
#define SB_PCIERP21_REG(Reg)            SB_PCI_CFG_ADDRESS(SB_PCIERP21_BUS, SB_PCIERP21_DEV, \
                                                           SB_PCIERP21_FUN, Reg)
#define SB_PCIERP22_REG(Reg)            SB_PCI_CFG_ADDRESS(SB_PCIERP22_BUS, SB_PCIERP22_DEV, \
                                                           SB_PCIERP22_FUN, Reg)
#define SB_PCIERP23_REG(Reg)            SB_PCI_CFG_ADDRESS(SB_PCIERP23_BUS, SB_PCIERP23_DEV, \
                                                           SB_PCIERP23_FUN, Reg)

#define SB_SIO_UART0_REG(Reg)           SB_PCI_CFG_ADDRESS(SB_SIO_UART0_BUS, SB_SIO_UART0_DEV, \
                                                           SB_SIO_UART0_FUN, Reg)
#define SB_SIO_UART1_REG(Reg)           SB_PCI_CFG_ADDRESS(SB_SIO_UART1_BUS, SB_SIO_UART1_DEV, \
                                                           SB_SIO_UART1_FUN, Reg)
#define SB_SIO_UART2_REG(Reg)           SB_PCI_CFG_ADDRESS(SB_SIO_UART2_BUS, SB_SIO_UART2_DEV, \
                                                           SB_SIO_UART2_FUN, Reg)
#define SB_SIO_GSPI0_REG(Reg)           SB_PCI_CFG_ADDRESS(SB_SIO_GSPI0_BUS, SB_SIO_GSPI0_DEV, \
                                                           SB_SIO_GSPI0_FUN, Reg)
#define SB_SIO_GSPI1_REG(Reg)           SB_PCI_CFG_ADDRESS(SB_SIO_GSPI1_BUS, SB_SIO_GSPI1_DEV, \
                                                           SB_SIO_GSPI1_FUN, Reg)
#define SB_SCS_EMMC_REG(Reg)            SB_PCI_CFG_ADDRESS(SB_SCS_EMMC_BUS, SB_SCS_EMMC_DEV, \
                                                           SB_SCS_EMMC_FUN, Reg)
#define SB_SCS_SDIO_REG(Reg)            SB_PCI_CFG_ADDRESS(SB_SCS_SDIO_BUS, SB_SCS_SDIO_DEV, \
                                                           SB_SCS_SDIO_FUN, Reg)
#define SB_SCS_SDCARD_REG(Reg)          SB_PCI_CFG_ADDRESS(SB_SCS_SDCARD_BUS, SB_SCS_SDCARD_DEV, \
                                                           SB_SCS_SDCARD_FUN, Reg)
#define SB_P2SB_REG(Reg)                SB_PCI_CFG_ADDRESS(SB_P2SB_BUS, SB_P2SB_DEV, \
                                                           SB_P2SB_FUN, Reg)
#define SB_PMC_REG(Reg)                 SB_PCI_CFG_ADDRESS(SB_PMC_BUS, SB_PMC_DEV, \
                                                           SB_PMC_FUN, Reg)
#define SB_SPI_REG(Reg)                 SB_PCI_CFG_ADDRESS(SB_SPI_BUS, SB_SPI_DEV, \
                                                            SB_SPI_FUN, Reg)

#define SB_SIO_I2C0_REG(Reg)            SB_PCI_CFG_ADDRESS(SB_SIO_I2C0_BUS, SB_SIO_I2C0_DEV, \
                                                           SB_SIO_I2C0_FUN, Reg)
#define SB_SIO_I2C1_REG(Reg)            SB_PCI_CFG_ADDRESS(SB_SIO_I2C1_BUS, SB_SIO_I2C1_DEV, \
                                                           SB_SIO_I2C1_FUN, Reg)
#define SB_SIO_I2C2_REG(Reg)            SB_PCI_CFG_ADDRESS(SB_SIO_I2C2_BUS, SB_SIO_I2C2_DEV, \
                                                           SB_SIO_I2C2_FUN, Reg)
#define SB_SIO_I2C3_REG(Reg)            SB_PCI_CFG_ADDRESS(SB_SIO_I2C3_BUS, SB_SIO_I2C3_DEV, \
                                                           SB_SIO_I2C3_FUN, Reg)
#define SB_SIO_I2C4_REG(Reg)            SB_PCI_CFG_ADDRESS(SB_SIO_I2C4_BUS, SB_SIO_I2C4_DEV, \
                                                           SB_SIO_I2C4_FUN, Reg)
#define SB_SIO_I2C5_REG(Reg)            SB_PCI_CFG_ADDRESS(SB_SIO_I2C5_BUS, SB_SIO_I2C5_DEV, \
                                                           SB_SIO_I2C5_FUN, Reg)

#ifdef PCIEX_BASE_ADDRESS
#define SB_LPC_MMIO_REG(Reg)            SB_PCIE_CFG_ADDRESS(SB_LPC_BUS, SB_LPC_DEV, \
                                                            SB_LPC_FUN, Reg)
#define SB_ISH_MMIO_REG(Reg)            SB_PCIE_CFG_ADDRESS(SB_ISH_BUS, SB_ISH_DEV, \
                                                            SB_ISH_FUN, Reg)
#define SB_XHCI_MMIO_REG(Reg)           SB_PCIE_CFG_ADDRESS(SB_XHCI_BUS, SB_XHCI_DEV, \
                                                            SB_XHCI_FUN, Reg)
#define SB_XDCI_MMIO_REG(Reg)           SB_PCIE_CFG_ADDRESS(SB_XDCI_BUS, SB_XDCI_DEV, \
                                                            SB_XDCI_FUN, Reg)
#define SB_THERMAL_MMIO_REG(Reg)        SB_PCIE_CFG_ADDRESS(SB_THERMAL_BUS, SB_THERMAL_DEV, \
                                                            SB_THERMAL_FUN, Reg)
#define SB_CIO2_MMIO_REG(Reg)           SB_PCIE_CFG_ADDRESS(SB_CIO2_BUS, SB_CIO2_DEV, \
                                                            SB_CIO2_FUN, Reg)
#define SB_SIO_I2C0_MMIO_REG(Reg)       SB_PCIE_CFG_ADDRESS(SB_SIO_I2C0_BUS, SB_SIO_I2C0_DEV, \
                                                            SB_SIO_I2C0_FUN, Reg)
#define SB_SIO_I2C1_MMIO_REG(Reg)       SB_PCIE_CFG_ADDRESS(SB_SIO_I2C1_BUS, SB_SIO_I2C1_DEV, \
                                                            SB_SIO_I2C1_FUN, Reg)
#define SB_SIO_I2C2_MMIO_REG(Reg)       SB_PCIE_CFG_ADDRESS(SB_SIO_I2C2_BUS, SB_SIO_I2C2_DEV, \
                                                            SB_SIO_I2C2_FUN, Reg)
#define SB_SIO_I2C3_MMIO_REG(Reg)       SB_PCIE_CFG_ADDRESS(SB_SIO_I2C3_BUS, SB_SIO_I2C3_DEV, \
                                                            SB_SIO_I2C3_FUN, Reg)
#define SB_HECI_MMIO_REG(Reg)           SB_PCIE_CFG_ADDRESS(SB_HECI_BUS, SB_HECI_DEV, \
                                                            SB_HECI_FUN, Reg)
#define SB_HECI2_MMIO_REG(Reg)          SB_PCIE_CFG_ADDRESS(SB_HECI2_BUS, SB_HECI2_DEV, \
                                                            SB_HECI2_FUN, Reg)
#define SB_HECI3_MMIO_REG(Reg)          SB_PCIE_CFG_ADDRESS(SB_HECI3_BUS, SB_HECI3_DEV, \
                                                            SB_HECI3_FUN, Reg)
#define SB_IDER_MMIO_REG(Reg)           SB_PCIE_CFG_ADDRESS(SB_IDER_BUS, SB_IDER_DEV, \
                                                            SB_IDER_FUN, Reg)
#define SB_KT_MMIO_REG(Reg)             SB_PCIE_CFG_ADDRESS(SB_KT_BUS, SB_KT_DEV, \
                                                            SB_KT_FUN, Reg)
#define SB_SATA_MMIO_REG(Reg)           SB_PCIE_CFG_ADDRESS(SB_SATA_BUS, SB_SATA_DEV, \
                                                            SB_SATA_FUN, Reg)
#define SB_SIO_UART2_MMIO_REG(Reg)      SB_PCIE_CFG_ADDRESS(SB_SIO_UART2_BUS, SB_SIO_UART2_DEV, \
                                                            SB_SIO_UART2_FUN, Reg)
#define SB_SIO_I2C5_MMIO_REG(Reg)       SB_PCIE_CFG_ADDRESS(SB_SIO_I2C5_BUS, SB_SIO_I2C5_DEV, \
                                                            SB_SIO_I2C5_FUN, Reg)
#define SB_SIO_I2C4_MMIO_REG(Reg)       SB_PCIE_CFG_ADDRESS(SB_SIO_I2C4_BUS, SB_SIO_I2C4_DEV, \
                                                            SB_SIO_I2C4_FUN, Reg)
#define SB_PCIERP16_MMIO_REG(Reg)       SB_PCIE_CFG_ADDRESS(SB_PCIERP16_BUS, SB_PCIERP16_DEV, \
                                                            SB_PCIERP16_FUN, Reg)
#define SB_PCIERP17_MMIO_REG(Reg)       SB_PCIE_CFG_ADDRESS(SB_PCIERP17_BUS, SB_PCIERP17_BUS, \
                                                            SB_PCIERP17_FUN, Reg)
#define SB_PCIERP18_MMIO_REG(Reg)       SB_PCIE_CFG_ADDRESS(SB_PCIERP18_BUS, SB_PCIERP18_DEV, \
                                                            SB_PCIERP18_FUN, Reg)
#define SB_PCIERP19_MMIO_REG(Reg)       SB_PCIE_CFG_ADDRESS(SB_PCIERP19_BUS, SB_PCIERP19_DEV, \
                                                            SB_PCIERP19_FUN, Reg)
#define SB_PCIERP20_MMIO_REG(Reg)       SB_PCIE_CFG_ADDRESS(SB_PCIERP20_BUS, SB_PCIERP20_DEV, \
                                                            SB_PCIERP20_FUN, Reg)
#define SB_PCIERP21_MMIO_REG(Reg)       SB_PCIE_CFG_ADDRESS(SB_PCIERP21_BUS, SB_PCIERP21_DEV, \
                                                            SB_PCIERP21_FUN, Reg)
#define SB_PCIERP22_MMIO_REG(Reg)       SB_PCIE_CFG_ADDRESS(SB_PCIERP22_BUS, SB_PCIERP22_DEV, \
                                                            SB_PCIERP22_FUN, Reg)
#define SB_PCIERP23_MMIO_REG(Reg)       SB_PCIE_CFG_ADDRESS(SB_PCIERP23_BUS, SB_PCIERP23_DEV, \
                                                            SB_PCIERP23_FUN, Reg)
#define SB_PCIERP00_MMIO_REG(Reg)       SB_PCIE_CFG_ADDRESS(SB_PCIERP00_BUS, SB_PCIERP00_DEV, \
                                                            SB_PCIERP00_FUN, Reg)
#define SB_PCIERP01_MMIO_REG(Reg)       SB_PCIE_CFG_ADDRESS(SB_PCIERP01_BUS, SB_PCIERP01_DEV, \
                                                            SB_PCIERP01_FUN, Reg)
#define SB_PCIERP02_MMIO_REG(Reg)       SB_PCIE_CFG_ADDRESS(SB_PCIERP02_BUS, SB_PCIERP02_DEV, \
                                                            SB_PCIERP02_FUN, Reg)
#define SB_PCIERP03_MMIO_REG(Reg)       SB_PCIE_CFG_ADDRESS(SB_PCIERP03_BUS, SB_PCIERP03_DEV, \
                                                            SB_PCIERP03_FUN, Reg)
#define SB_PCIERP04_MMIO_REG(Reg)       SB_PCIE_CFG_ADDRESS(SB_PCIERP04_BUS, SB_PCIERP04_DEV, \
                                                            SB_PCIERP04_FUN, Reg)
#define SB_PCIERP05_MMIO_REG(Reg)       SB_PCIE_CFG_ADDRESS(SB_PCIERP05_BUS, SB_PCIERP05_DEV, \
                                                            SB_PCIERP05_FUN, Reg)
#define SB_PCIERP06_MMIO_REG(Reg)       SB_PCIE_CFG_ADDRESS(SB_PCIERP06_BUS, SB_PCIERP06_DEV, \
                                                            SB_PCIERP06_FUN, Reg)
#define SB_PCIERP07_MMIO_REG(Reg)       SB_PCIE_CFG_ADDRESS(SB_PCIERP07_BUS, SB_PCIERP07_DEV, \
                                                            SB_PCIERP07_FUN, Reg)
#define SB_PCIERP08_MMIO_REG(Reg)       SB_PCIE_CFG_ADDRESS(SB_PCIERP08_BUS, SB_PCIERP08_DEV, \
                                                            SB_PCIERP08_FUN, Reg)
#define SB_PCIERP09_MMIO_REG(Reg)       SB_PCIE_CFG_ADDRESS(SB_PCIERP09_BUS, SB_PCIERP09_DEV, \
                                                            SB_PCIERP09_FUN, Reg)
#define SB_PCIERP10_MMIO_REG(Reg)       SB_PCIE_CFG_ADDRESS(SB_PCIERP10_BUS, SB_PCIERP10_DEV, \
                                                            SB_PCIERP10_FUN, Reg)
#define SB_PCIERP11_MMIO_REG(Reg)       SB_PCIE_CFG_ADDRESS(SB_PCIERP11_BUS, SB_PCIERP11_DEV, \
                                                            SB_PCIERP11_FUN, Reg)
#define SB_PCIERP12_MMIO_REG(Reg)       SB_PCIE_CFG_ADDRESS(SB_PCIERP12_BUS, SB_PCIERP12_DEV, \
                                                            SB_PCIERP12_FUN, Reg)
#define SB_PCIERP13_MMIO_REG(Reg)       SB_PCIE_CFG_ADDRESS(SB_PCIERP13_BUS, SB_PCIERP13_DEV, \
                                                            SB_PCIERP13_FUN, Reg)
#define SB_PCIERP14_MMIO_REG(Reg)       SB_PCIE_CFG_ADDRESS(SB_PCIERP14_BUS, SB_PCIERP14_DEV, \
                                                            SB_PCIERP14_FUN, Reg)
#define SB_PCIERP15_MMIO_REG(Reg)       SB_PCIE_CFG_ADDRESS(SB_PCIERP15_BUS, SB_PCIERP15_DEV, \
                                                            SB_PCIERP15_FUN, Reg)
#define SB_SIO_UART0_MMIO_REG(Reg)      SB_PCIE_CFG_ADDRESS(SB_SIO_UART0_BUS, SB_SIO_UART0_DEV, \
                                                            SB_SIO_UART0_FUN, Reg)
#define SB_SIO_UART1_MMIO_REG(Reg)      SB_PCIE_CFG_ADDRESS(SB_SIO_UART1_BUS, SB_SIO_UART1_DEV, \
                                                            SB_SIO_UART1_FUN, Reg)
#define SB_SIO_GSPI0_MMIO_REG(Reg)      SB_PCIE_CFG_ADDRESS(SB_SIO_GSPI0_BUS, SB_SIO_GSPI0_DEV, \
                                                            SB_SIO_GSPI0_FUN, Reg)
#define SB_SIO_GSPI1_MMIO_REG(Reg)      SB_PCIE_CFG_ADDRESS(SB_SIO_GSPI1_BUS, SB_SIO_GSPI1_DEV, \
                                                            SB_SIO_GSPI1_FUN, Reg)
#define SB_SCS_EMMC_MMIO_REG(Reg)       SB_PCIE_CFG_ADDRESS(SB_SCS_EMMC_BUS, SB_SCS_EMMC_DEV, \
                                                            SB_SCS_EMMC_FUN, Reg)
#define SB_SCS_SDIO_MMIO_REG(Reg)       SB_PCIE_CFG_ADDRESS(SB_SCS_SDIO_BUS, SB_SCS_SDIO_DEV, \
                                                            SB_SCS_SDIO_FUN, Reg)
#define SB_SCS_SDCARD_MMIO_REG(Reg)     SB_PCIE_CFG_ADDRESS(SB_SCS_SDCARD_BUS, SB_SCS_SDCARD_DEV, \
                                                            SB_SCS_SDCARD_FUN, Reg)
#define SB_P2SB_MMIO_REG(Reg)           SB_PCIE_CFG_ADDRESS(SB_P2SB_BUS, SB_P2SB_DEV, \
                                                            SB_P2SB_FUN, Reg)
#define SB_PMC_MMIO_REG(Reg)            SB_PCIE_CFG_ADDRESS(SB_PMC_BUS, SB_PMC_DEV, \
                                                            SB_PMC_FUN, Reg)
#define SB_HDA_MMIO_REG(Reg)            SB_PCIE_CFG_ADDRESS(SB_HDA_BUS, SB_HDA_DEV, \
                                                            SB_HDA_FUN, Reg)
#define SB_SMBUS_MMIO_REG(Reg)          SB_PCIE_CFG_ADDRESS(SB_SMBUS_BUS, SB_SMBUS_DEV, \
                                                            SB_SMBUS_FUN, Reg)
#define SB_SPI_MMIO_REG(Reg)            SB_PCIE_CFG_ADDRESS(SB_SPI_BUS, SB_SPI_DEV, \
                                                            SB_SPI_FUN, Reg)
#define SB_LAN_MMIO_REG(Reg)            SB_PCIE_CFG_ADDRESS(SB_LAN_BUS, SB_LAN_DEV, \
                                                            SB_LAN_FUN, Reg)
#define SB_TRACE_HUB_MMIO_REG(Reg)      SB_PCIE_CFG_ADDRESS(SB_TRACE_HUB_BUS, SB_TRACE_HUB_DEV, \
                                                            SB_TRACE_HUB_FUN, Reg)

#endif // End of PCIEX_BASE_ADDRESS


//----------------------------------------------------------------------------
// INTEL PCH PCI Bus/Device/Function Number Macros
//----------------------------------------------------------------------------
#define SB_HECI_BUS_DEV_FUN        SB_HECI_REG(0)
#define SB_HECI2_BUS_DEV_FUN       SB_HECI2_REG(0)
#define SB_IDER_BUS_DEV_FUN        SB_IDER_REG(0)
#define SB_KT_BUS_DEV_FUN          SB_KT_REG(0)
#define SB_BUS_DEV_FUN             SB_LPC_REG(0)
#define SB_ISH_BUS_DEV_FUN         SB_ISH_REG(0)
#define SB_XHCI_BUS_DEV_FUN        SB_XHCI_REG(0)
#define SB_XDCI_BUS_DEV_FUN        SB_XDCI_REG(0)
#define SB_SATA_BUS_DEV_FUN        SB_SATA_REG(0)
#define SB_SMBUS_BUS_DEV_FUN       SB_SMBUS_REG(0)
#define SB_THERMAL_BUS_DEV_FUN     SB_THERMAL_REG(0)
#define SB_HDA_BUS_DEV_FUN         SB_HDA_REG(0)
#define SB_LAN_BUS_DEV_FUN         SB_LAN_REG(0)
#define SB_CIO2_BUS_DEV_FUN        SB_CIO2_REG(0)
#define SB_HECI3_BUS_DEV_FUN       SB_HECI3_REG(0)

#define SB_PCIERP00_BUS_DEV_FUN     SB_PCIERP0_REG(0)
#define SB_PCIERP01_BUS_DEV_FUN     SB_PCIERP1_REG(0)
#define SB_PCIERP02_BUS_DEV_FUN     SB_PCIERP2_REG(0)
#define SB_PCIERP03_BUS_DEV_FUN     SB_PCIERP3_REG(0)
#define SB_PCIERP04_BUS_DEV_FUN     SB_PCIERP4_REG(0)
#define SB_PCIERP05_BUS_DEV_FUN     SB_PCIERP5_REG(0)
#define SB_PCIERP06_BUS_DEV_FUN     SB_PCIERP6_REG(0)
#define SB_PCIERP07_BUS_DEV_FUN     SB_PCIERP7_REG(0)
#define SB_PCIERP08_BUS_DEV_FUN     SB_PCIERP8_REG(0)

#define SB_PCIERP09_BUS_DEV_FUN     SB_PCIERP9_REG(0)
#define SB_PCIERP10_BUS_DEV_FUN     SB_PCIERP10_REG(0)
#define SB_PCIERP11_BUS_DEV_FUN     SB_PCIERP11_REG(0)
#define SB_PCIERP12_BUS_DEV_FUN     SB_PCIERP12_REG(0)
#define SB_PCIERP13_BUS_DEV_FUN     SB_PCIERP13_REG(0)
#define SB_PCIERP14_BUS_DEV_FUN     SB_PCIERP14_REG(0)
#define SB_PCIERP15_BUS_DEV_FUN     SB_PCIERP15_REG(0)
#define SB_PCIERP16_BUS_DEV_FUN     SB_PCIERP16_REG(0)
#define SB_PCIERP17_BUS_DEV_FUN     SB_PCIERP17_REG(0)
#define SB_PCIERP18_BUS_DEV_FUN     SB_PCIERP18_REG(0)
#define SB_PCIERP19_BUS_DEV_FUN     SB_PCIERP19_REG(0)
#define SB_PCIERP20_BUS_DEV_FUN     SB_PCIERP20_REG(0)
#define SB_PCIERP21_BUS_DEV_FUN     SB_PCIERP21_REG(0)
#define SB_PCIERP22_BUS_DEV_FUN     SB_PCIERP22_REG(0)
#define SB_PCIERP23_BUS_DEV_FUN     SB_PCIERP23_REG(0)

#define SB_SIO_I2C0_BUS_DEV_FUN    SB_SIO_I2C0_REG(0)
#define SB_SIO_I2C1_BUS_DEV_FUN    SB_SIO_I2C1_REG(0)
#define SB_SIO_I2C2_BUS_DEV_FUN    SB_SIO_I2C2_REG(0)
#define SB_SIO_I2C3_BUS_DEV_FUN    SB_SIO_I2C3_REG(0)
#define SB_SIO_I2C4_BUS_DEV_FUN    SB_SIO_I2C4_REG(0)
#define SB_SIO_I2C5_BUS_DEV_FUN    SB_SIO_I2C5_REG(0)
#define SB_SIO_GSPI0_BUS_DEV_FUN   SB_SIO_GSPI0_REG(0)
#define SB_SIO_GSPI1_BUS_DEV_FUN   SB_SIO_GSPI1_REG(0)
#define SB_SIO_UART0_BUS_DEV_FUN   SB_SIO_UART0_REG(0)
#define SB_SIO_UART1_BUS_DEV_FUN   SB_SIO_UART1_REG(0)
#define SB_SIO_UART2_BUS_DEV_FUN   SB_SIO_UART2_REG(0)
#define SB_SCS_SDIO_BUS_DEV_FUN    SB_SCS_SDIO_REG(0)
#define SB_SCS_EMMC_BUS_DEV_FUN    SB_SCS_EMMC_REG(0)
#define SB_SCS_SDCARD_BUS_DEV_FUN  SB_SCS_SDCARD_REG(0)
#define SB_P2SB_BUS_DEV_FUN        SB_P2SB_REG(0)
#define SB_PMC_BUS_DEV_FUN         SB_PMC_REG(0)
#define SB_SPI_BUS_DEV_FUN         SB_SPI_REG(0)

#ifdef PCIEX_BASE_ADDRESS
#define SB_LPC_MMIO_BUS_DEV_FUN             SB_LPC_MMIO_REG(0)
#define SB_ISH_MMIO_BUS_DEV_FUN             SB_ISH_MMIO_REG(0)
#define SB_XHCI_MMIO_BUS_DEV_FUN            SB_XHCI_MMIO_REG(0)
#define SB_XDCI_MMIO_BUS_DEV_FUN            SB_XDCI_MMIO_REG(0)
#define SB_THERMAL_MMIO_BUS_DEV_FUN         SB_THERMAL_MMIO_REG(0)
#define SB_CIO2_MMIO_BUS_DEV_FUN            SB_CIO2_MMIO_REG(0)
#define SB_SIO_I2C0_MMIO_BUS_DEV_FUN        SB_SIO_I2C0_MMIO_REG(0)
#define SB_SIO_I2C1_MMIO_BUS_DEV_FUN        SB_SIO_I2C1_MMIO_REG(0)
#define SB_SIO_I2C2_MMIO_BUS_DEV_FUN        SB_SIO_I2C2_MMIO_REG(0)
#define SB_SIO_I2C3_MMIO_BUS_DEV_FUN        SB_SIO_I2C3_MMIO_REG(0)
#define SB_HECI_MMIO_BUS_DEV_FUN            SB_HECI_MMIO_REG(0)
#define SB_HECI2_MMIO_BUS_DEV_FUN           SB_HECI2_MMIO_REG(0)
#define SB_HECI3_MMIO_BUS_DEV_FUN           SB_HECI3_MMIO_REG(0)
#define SB_IDER_MMIO_BUS_DEV_FUN            SB_IDER_MMIO_REG(0)
#define SB_KT_MMIO_BUS_DEV_FUN              SB_KT_MMIO_REG(0)
#define SB_SATA_MMIO_BUS_DEV_FUN            SB_SATA_MMIO_REG(0)
#define SB_SIO_UART2_MMIO_BUS_DEV_FUN       SB_SIO_UART2_MMIO_REG(0)
#define SB_SIO_I2C5_MMIO_BUS_DEV_FUN        SB_SIO_I2C5_MMIO_REG(0)
#define SB_SIO_I2C4_MMIO_BUS_DEV_FUN        SB_SIO_I2C4_MMIO_REG(0)
#define SB_PCIERP16_MMIO_BUS_DEV_FUN        SB_PCIERP16_MMIO_REG(0)
#define SB_PCIERP17_MMIO_BUS_DEV_FUN        SB_PCIERP17_MMIO_REG(0)
#define SB_PCIERP18_MMIO_BUS_DEV_FUN        SB_PCIERP18_MMIO_REG(0)
#define SB_PCIERP19_MMIO_BUS_DEV_FUN        SB_PCIERP19_MMIO_REG(0)
#define SB_PCIERP20_MMIO_BUS_DEV_FUN        SB_PCIERP20_MMIO_REG(0)
#define SB_PCIERP21_MMIO_BUS_DEV_FUN        SB_PCIERP21_MMIO_REG(0)
#define SB_PCIERP22_MMIO_BUS_DEV_FUN        SB_PCIERP22_MMIO_REG(0)
#define SB_PCIERP23_MMIO_BUS_DEV_FUN        SB_PCIERP23_MMIO_REG(0)
#define SB_PCIERP00_MMIO_BUS_DEV_FUN        SB_PCIERP00_MMIO_REG(0)
#define SB_PCIERP01_MMIO_BUS_DEV_FUN        SB_PCIERP01_MMIO_REG(0)
#define SB_PCIERP02_MMIO_BUS_DEV_FUN        SB_PCIERP02_MMIO_REG(0)
#define SB_PCIERP03_MMIO_BUS_DEV_FUN        SB_PCIERP03_MMIO_REG(0)
#define SB_PCIERP04_MMIO_BUS_DEV_FUN        SB_PCIERP04_MMIO_REG(0)
#define SB_PCIERP05_MMIO_BUS_DEV_FUN        SB_PCIERP05_MMIO_REG(0)
#define SB_PCIERP06_MMIO_BUS_DEV_FUN        SB_PCIERP06_MMIO_REG(0)
#define SB_PCIERP07_MMIO_BUS_DEV_FUN        SB_PCIERP07_MMIO_REG(0)
#define SB_PCIERP08_MMIO_BUS_DEV_FUN        SB_PCIERP08_MMIO_REG(0)
#define SB_PCIERP09_MMIO_BUS_DEV_FUN        SB_PCIERP09_MMIO_REG(0)
#define SB_PCIERP10_MMIO_BUS_DEV_FUN        SB_PCIERP10_MMIO_REG(0)
#define SB_PCIERP11_MMIO_BUS_DEV_FUN        SB_PCIERP11_MMIO_REG(0)
#define SB_PCIERP12_MMIO_BUS_DEV_FUN        SB_PCIERP12_MMIO_REG(0)
#define SB_PCIERP13_MMIO_BUS_DEV_FUN        SB_PCIERP13_MMIO_REG(0)
#define SB_PCIERP14_MMIO_BUS_DEV_FUN        SB_PCIERP14_MMIO_REG(0)
#define SB_PCIERP15_MMIO_BUS_DEV_FUN        SB_PCIERP15_MMIO_REG(0)
#define SB_SIO_UART0_MMIO_BUS_DEV_FUN       SB_SIO_UART0_MMIO_REG(0)
#define SB_SIO_UART1_MMIO_BUS_DEV_FUN       SB_SIO_UART1_MMIO_REG(0)
#define SB_SIO_GSPI0_MMIO_BUS_DEV_FUN       SB_SIO_GSPI0_MMIO_REG(0)
#define SB_SIO_GSPI1_MMIO_BUS_DEV_FUN       SB_SIO_GSPI1_MMIO_REG(0)
#define SB_SCS_EMMC_MMIO_BUS_DEV_FUN        SB_SCS_EMMC_MMIO_REG(0)
#define SB_SCS_SDIO_MMIO_BUS_DEV_FUN        SB_SCS_SDIO_MMIO_REG(0)
#define SB_SCS_SDCARD_MMIO_BUS_DEV_FUN      SB_SCS_SDCARD_MMIO_REG(0)
#define SB_P2SB_MMIO_BUS_DEV_FUN            SB_P2SB_MMIO_REG(0)
#define SB_PMC_MMIO_BUS_DEV_FUN             SB_PMC_MMIO_REG(0)
#define SB_HDA_MMIO_BUS_DEV_FUN             SB_HDA_MMIO_REG(0)
#define SB_SMBUS_MMIO_BUS_DEV_FUN           SB_SMBUS_MMIO_REG(0)
#define SB_SPI_MMIO_BUS_DEV_FUN             SB_SPI_MMIO_REG(0)
#define SB_LAN_MMIO_BUS_DEV_FUN             SB_LAN_MMIO_REG(0)
#define SB_TRACE_HUB_MMIO_BUS_DEV_FUN       SB_TRACE_HUB_MMIO_REG(0)
#endif

/**
 Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values within the bits
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position
**/

//----------------------------------------------------------------------------
// INTEL PCH Common Equates             Chipset Porting Require !!!
//----------------------------------------------------------------------------
#define SKL_PCH_H_PCIE_MAX_ROOT_PORTS       20
#define KBL_PCH_H_PCIE_MAX_ROOT_PORTS       24
#define SB_H_PCIE_MAX_ROOT_PORTS            KBL_PCH_H_PCIE_MAX_ROOT_PORTS
#define SB_LP_PCIE_MAX_ROOT_PORTS           12
#if ULT_SUPPORT
#define SB_PCIE_MAX_PORTS                   SB_LP_PCIE_MAX_ROOT_PORTS
#else
#define SB_PCIE_MAX_PORTS                   SB_H_PCIE_MAX_ROOT_PORTS
#endif

#define SB_H_PCIE_MAX_CLK_REQ               16
#define SB_LP_PCIE_MAX_CLK_REQ              6

#define SB_MAX_RST_PCIE_STORAGE_CR          3

#define SB_H_XHCI_MAX_HS_PHYSICAL_PORTS     14
#define SB_LP_XHCI_MAX_HS_PHYSICAL_PORTS    10
#if ULT_SUPPORT
#define SB_USB_MAX_PHYSICAL_PORTS           SB_LP_XHCI_MAX_HS_PHYSICAL_PORTS
#else
#define SB_USB_MAX_PHYSICAL_PORTS           SB_H_XHCI_MAX_HS_PHYSICAL_PORTS
#endif

#define SB_H_XHCI_MAX_SS_PORTS              10
#define SB_LP_XHCI_MAX_SS_PORTS             6

#define SB_XHCI_MAX_SSIC_PORTS              2

#define SB_H_AHCI_MAX_PORTS                 8
#define SB_LP_AHCI_MAX_PORTS                3
#if ULT_SUPPORT
#define SB_SATA_MAX_PORTS                   SB_LP_AHCI_MAX_PORTS
#else
#define SB_SATA_MAX_PORTS                   SB_H_AHCI_MAX_PORTS
#endif
//----------------------------------------------------------------------------
// INTEL PCH Integrated Sensor Hub Controller (D19:F0) Equates
//----------------------------------------------------------------------------
#define R_SB_ISH_PCICMD				 0x04    // PCI Command Register
#define R_SB_ISH_BAR				 0x10    // Memory Base Address Register
#define R_SB_ISH_BAR_HIGH            0x14    // Memory Base Address Register High
#define R_SB_ISH_SVID                0x2C    // Subsystem Vendor IDs Reg.
#define R_SB_ISH_INTR_LN			 0x3C    // Interrupt Line Register
//----------------------------------------------------------------------------
// INTEL PCH USB 3.0 Controller (D20:F0) Equates             
//----------------------------------------------------------------------------
#define R_SB_XHCI_VID                0x00    // Vendor ID Reg.
#define R_SB_XHCI_DEVID              0x02    // Device ID Reg.
#define R_SB_XHCI_PCICMD             0x04    // PCI Command Register
#define R_SB_XHCI_RID                0x08    // Revision Identification Register
#define R_SB_XHCI_SVID               0x2c    // USB 3.0 System Vendor ID register
#define R_SB_XHCI_MEM_BASE           0x10
#define V_SB_XHCI_MEM_LENGTH         0x10000
#define N_SB_XHCI_MEM_ALIGN          16
#define B_SB_XHCI_MEM_ALIGN_MASK     0xFFFF

//----------------------------------------------------------------------------
// INTEL PCH xDCI (OTG) USB Device Controller (D20:F1) Equates             
//----------------------------------------------------------------------------
#define R_SB_XDCI_SVID               0x2C    // Subsystem Vendor IDs Reg.
//----------------------------------------------------------------------------
// INTEL PCH THERMAL Controller (D20:F2) Equates
//----------------------------------------------------------------------------
#define R_SB_THERMAL_PCICMD      0x04    // PCI Command Register
#define R_SB_THERMAL_TBAR        0x10    // Thermal Memory Base Address Register
#define R_SB_THERMAL_TBAR_HIGH   0x14    // Thermal Memory Base Address Register High
#define R_SB_THERMAL_SVID        0x2C    // Subsystem Vendor IDs Reg.
#define R_SB_THERMAL_INTR_LN     0x3c    // Interrupt Line Register

//----------------------------------------------------------------------------
// INTEL PCH Camera IO Host Controller (D20:F3) Equates
//----------------------------------------------------------------------------
#define R_SB_CIO2_SVID           0x2C    // Subsystem Vendor IDs Reg.
//----------------------------------------------------------------------------
// INTEL PCH ME HECIx Controller (D22:F0/F1/F2/F3) Equates
//----------------------------------------------------------------------------
#define R_SB_ME_VID              0x00    // PCI Vendor ID Register
#define ICH_REG_ME_VID          R_SB_ME_VID
#define R_SB_ME_PCICMD           0x04    // PCI Command Register
#define ICH_REG_ME_PCICMD       R_SB_ME_PCICMD
#define R_SB_ME_HECI_MBAR        0x10    // HECI MMIO Base Address Register
#define ICH_REG_ME_HECI0_BAR     R_SB_ME_HECI_MBAR
#define R_SB_ME_HECI_EXT_BAR     0x14
#define ICH_REG_ME_HECI0_EXT_BAR R_SB_ME_HECI_EXT_BAR
#define R_SB_ME_SVID             0x2c    // Sub-Vendor/SubSystem IDs Register
#define R_SB_ME_HIDM             0xA0
#define ICH_REG_ME_HIDM         R_SB_ME_HIDM

//----------------------------------------------------------------------------
// INTEL PCH Serial ATA Controller (D23:F0) Equates
//----------------------------------------------------------------------------
#define R_SB_SATA_DEVID          0x02    // Device ID Reg.
#define R_SB_SATA_PCICMD         0x04    // Command Register
#define R_SB_SATA_RID            0x08    // Revision ID Reg.
#define R_SB_SATA_PI             0x09    // Programming Interface Register
#define R_SB_SATA_PCMD_BAR       0x10    // Primary Command Block Base Address Register
#define R_SB_SATA_PCNL_BAR       0x14    // Primary Control Block Base Address Register
#define R_SB_SATA_SCMD_BAR       0x18    // Secondary Command Block Base Address Register
#define R_SB_SATA_SCNL_BAR       0x1C    // Secondary Control Block Base Address Register
#define R_SB_SATA_ABAR           0x24    // AHCI Base Address Register

#define R_SB_SATA_MLT            0x0d    // Primary Master Latnecy Timer Reg.

#define R_SB_SATA_BM_BASE        0x20    // Bus Master Base Address Register

#define R_SB_SATA_SVID           0x2C    // Sub-Vendor/SubSystem IDs register
#define R_SB_SATA_INTR_LN        0x3C    // Interrupt Line Register

#define R_PCH_SATA_AHCI_INTLN    0x40	 //

#define R_SB_SATA_PID            0x70    // PCI Power Management Capability ID register
#define R_SB_SATA_PC             0x72    // PCI Power Management Capability register
#define R_SB_SATA_PMCS           0x74    // PCI Power Management Control & Status register
#define R_SB_SATA_MSICI          0x80    // Message Signaled Interrupt Identifiers register
#define R_SB_SATA_MSIMC          0x82    // Message Signaled Interrupt Message Control register
#define R_SB_SATA_MSIMA          0x84    // Message Signaled Interrupt Message Address register
#define R_SB_SATA_MSIMD          0x88    // Message Signaled Interrupt Message Data register
#define R_SB_SATA_MAP            0x90    // Address Map register

#define B_SB_LP_SATA_MAP_SPD       (BIT10 | BIT9 | BIT8)
#define N_SB_LP_SATA_MAP_SPD       8
#define B_SB_LP_SATA_MAP_SPD2      BIT10
#define B_SB_LP_SATA_MAP_SPD1      BIT9
#define B_SB_LP_SATA_MAP_SPD0      BIT8
#define B_SB_LP_SATA_MAP_SMS_MASK  BIT6
#define N_SB_LP_SATA_MAP_SMS_MASK  6
#define V_SB_LP_SATA_MAP_SMS_AHCI  0x0
#define V_SB_LP_SATA_MAP_SMS_RAID  0x1

#define R_SB_LP_SATA_PCS         0x92    // Port Status & Control register
#define R_SB_H_SATA_PCS          0x94    // Port Status & Control register

#define R_SB_SATA_SATAGC         0x9C    // SATA General Configuration
#define B_SB_H_SATA_SATAGC_SMS_MASK   BIT16
#define N_SB_H_SATA_SATAGC_SMS_MASK   16
#define V_SB_H_SATA_SATAGC_SMS_AHCI   0x0
#define V_SB_H_SATA_SATAGC_SMS_RAID   0x1

#define R_SB_SATA_SIRI           0xA0    // S-ATA Register Index register
#define R_SB_SATA_STRD           0xA4    // S-ATA Register Data register

//
// AHCI BAR Area related Registers
//
#define R_SB_SATA_AHCI_CAP       0
#define R_SB_SATA_AHCI_PI        0x0C
#define R_SB_SATA_AHCI_P0CMD     0x118

//----------------------------------------------------------------------------
// INTEL PCH PCI Express Root Port (D28/29:F0/1/2/3/4/5/6/7, D27:F0/1/2/3) Equates
//----------------------------------------------------------------------------
#define R_SB_PCIE_SVID          0x94    // Subsystem Vendor IDs Reg.
#define ICH_REG_PCIE_BNUM       0x018
#define ICH_REG_PCIE_PCICMD     0x004
#define ICH_REG_PCIE_SLCAP      0x054

//----------------------------------------------------------------------------
// INTEL PCH Storage and Communication Subsystem Equates
//----------------------------------------------------------------------------
#define R_SB_SCS_SVID           0x2C    // Subsystem Vendor IDs Reg.

//----------------------------------------------------------------------------
// INTEL PCH LPC Bridge / eSPI (D31:F0) Equates
//----------------------------------------------------------------------------
#define ICH_REG_LPC_VID         0x00

#define R_SB_LPC_SVID           0x2C    // Subsystem Vendor IDs Reg.

#define R_SB_LPC_IOD            0x80    // I/O Decode Ranges Reg.
#define R_SB_LPC_IOE            0x82    // LPC Interface Enables Reg.

#define R_SB_LPC_GEN1_DEC       0x84    // LPC Interface Generic Decode 
                                        // Range 1 Reg.
#define R_SB_LPC_GEN2_DEC       0x88    // LPC Interface Generic Decode 
                                        // Range 2 Reg.
#define R_SB_LPC_GEN3_DEC       0x8c    // LPC Interface Generic Decode 
                                        // Range 3 Reg.
#define R_SB_LPC_GEN4_DEC       0x90    // LPC Interface Generic Decode 
                                        // Range 4 Reg.
#define B_SB_LPC_GENX_DEC_EN    0x00000001 ///**** PORTING REQUIRED ****
#define B_SB_LPC_GENX_DEC_BASE  0x0000FFFC ///**** PORTING REQUIRED ****
#define B_SB_LPC_GENX_DEC_MASK  0x00FC0000 ///**** PORTING REQUIRED ****
#define R_PCH_PCR_DMI_LPCLGIR1  0x2730     ///< LPC Generic I/O Range 1

#define SB_REG_LPC_PMIR         0xac    // 
#define ICH_REG_LPC_PMIR        SB_REG_LPC_PMIR
#define   B_ICH_LPC_PMIR_CF9GR     BIT20 // CF9h Global Reset
#define   B_ICH_LPC_PMIR_CF9LOCK   BIT31 // CF9h Lockdown
#define SB_REG_BIOS_CNTL        0xdc    // BIOS Control Reg.

// AmiChipsetModulePkg Template definition
#define SB_REG_GEN1_DEC         R_SB_LPC_GEN1_DEC
#define SB_REG_GEN2_DEC         R_SB_LPC_GEN2_DEC
#define SB_REG_GEN3_DEC         R_SB_LPC_GEN3_DEC
#define SB_REG_GEN4_DEC         R_SB_LPC_GEN4_DEC

//----------------------------------------------------------------------------
// INTEL PCH Primary To Sideband Bridge Controller(P2SB) (D31:F1) Equates
//----------------------------------------------------------------------------
#define R_SB_P2SB_SVID           0x2C    // Subsystem Vendor IDs Reg.

#define R_SB_PCR_ITSS_PIRQA_ROUT                0x3100
#define R_SB_PCR_ITSS_PIRQB_ROUT                0x3101
#define R_SB_PCR_ITSS_PIRQC_ROUT                0x3102
#define R_SB_PCR_ITSS_PIRQD_ROUT                0x3103
#define R_SB_PCR_ITSS_PIRQE_ROUT                0x3104
#define R_SB_PCR_ITSS_PIRQF_ROUT                0x3105
#define R_SB_PCR_ITSS_PIRQG_ROUT                0x3106
#define R_SB_PCR_ITSS_PIRQH_ROUT                0x3107

#define R_SB_PCR_ITSS_GIC                       0x31FC     ///< General Interrupt Control
#define B_SB_PCR_ITSS_GIC_AME                   BIT17      ///< Alternate Access Mode Enable
#define B_SB_PCR_ITSS_GIC_SPS                   BIT16      ///< Shutdown Policy Select

//----------------------------------------------------------------------------
// INTEL PCH Power Management Controller(PMC) (D31:F2) Equates
//----------------------------------------------------------------------------
#define R_SB_PMC_PCICMD                                    0x04    // PCI Command Register
#define R_SB_PMC_BAR                                       0x10    // Memory Base Address Register 
#define R_SB_PMC_SVID                                      0x2C    // Subsystem Vendor IDs Reg.
#define R_SB_PMC_ACPI_BASE                                 0x40
#define B_SB_PMC_ACPI_BASE_BAR                             0xFFFC
#define R_SB_PMC_GEN_PMCON_A                               R_PCH_PMC_GEN_PMCON_A
#define R_SB_PMC_GEN_PMCON_B                               R_PCH_PMC_GEN_PMCON_B
#define B_SB_PMC_GEN_PMCON_B_PWR_FLR                       BIT1
#define B_SB_PMC_GEN_PMCON_B_AFTERG3_EN                    BIT0

//
// ACPI and legacy I/O register offsets from ACPIBASE
//
#define R_SB_ACPI_PM1_STS                        0x00     ///> Power Management 1 Status Reg.
#define SB_ACPI_PM1_STS                          R_SB_ACPI_PM1_STS
#define B_SB_ACPI_PM1_STS_WAK                    0x8000
#define B_SB_ACPI_PM1_STS_PRBTNOR                0x0800
#define B_SB_ACPI_PM1_STS_RTC                    0x0400
#define B_SB_ACPI_PM1_STS_PWRBTN                 0x0100
#define B_SB_ACPI_PM1_STS_GBL                    0x0020
#define B_SB_ACPI_PM1_STS_BM                     0x0010
#define B_SB_ACPI_PM1_STS_TMROF                  0x0001
#define N_SB_ACPI_PM1_STS_WAK                    15
#define N_SB_ACPI_PM1_STS_PRBTNOR                11
#define N_SB_ACPI_PM1_STS_RTC                    10
#define N_SB_ACPI_PM1_STS_PWRBTN                 8
#define N_SB_ACPI_PM1_STS_GBL                    5
#define N_SB_ACPI_PM1_STS_BM                     4
#define N_SB_ACPI_PM1_STS_TMROF                  0

#define R_SB_ACPI_PM1_EN                         0x02      ///> Power Management 1 Enables Reg.
#define SB_ACPI_PM1_EN                           R_SB_ACPI_PM1_EN

#define R_SB_ACPI_PM1_CNT                        0x04       ///> Power Management 1 Control Reg.
#define SB_ACPI_PM1_CNTL                         R_SB_ACPI_PM1_CNT
#define S_SB_ACPI_PM1_CNT                        4
#define B_SB_ACPI_PM1_CNT_SLP_EN                 0x00002000
#define B_SB_ACPI_PM1_CNT_SLP_TYP                0x00001C00
#define V_SB_ACPI_PM1_CNT_S0                     0x00000000
#define V_SB_ACPI_PM1_CNT_S1                     0x00000400
#define V_SB_ACPI_PM1_CNT_S3                     0x00001400
#define V_SB_ACPI_PM1_CNT_S4                     0x00001800
#define V_SB_ACPI_PM1_CNT_S5                     0x00001C00
#define B_SB_ACPI_PM1_CNT_GBL_RLS                0x00000004
#define B_SB_ACPI_PM1_CNT_BM_RLD                 0x00000002
#define B_SB_ACPI_PM1_CNT_SCI_EN                 0x00000001

#define R_SB_ACPI_PM1_TMR                        0x08        ///> Power Management 1 Timer Reg.
#define V_SB_ACPI_TMR_FREQUENCY                  3579545
#define B_SB_ACPI_PM1_TMR_VAL                    0xFFFFFF
#define V_SB_ACPI_PM1_TMR_MAX_VAL                0x1000000       ///< The timer is 24 bit overflow

#define R_SB_SMI_EN                              0x30        ///> SMI Control and Enable Reg.
#define S_SB_SMI_EN                              4
#define B_SB_SMI_EN_LEGACY_USB3                  BIT31
#define B_SB_SMI_EN_GPIO_UNLOCK_SMI              BIT27
#define B_SB_SMI_EN_INTEL_USB2                   BIT18
#define B_SB_SMI_EN_LEGACY_USB2                  BIT17
#define B_SB_SMI_EN_PERIODIC                     BIT14
#define B_SB_SMI_EN_TCO                          BIT13
#define B_SB_SMI_EN_MCSMI                        BIT11
#define B_SB_SMI_EN_BIOS_RLS                     BIT7
#define B_SB_SMI_EN_SWSMI_TMR                    BIT6
#define B_SB_SMI_EN_APMC                         BIT5
#define B_SB_SMI_EN_ON_SLP_EN                    BIT4
#define B_SB_SMI_EN_LEGACY_USB                   BIT3
#define B_SB_SMI_EN_BIOS                         BIT2
#define B_SB_SMI_EN_EOS                          BIT1
#define B_SB_SMI_EN_GBL_SMI                      BIT0

#define R_SB_SMI_STS                             0x34         ///> SMI status Reg.
#define S_SB_SMI_STS                             4
#define B_SB_SMI_STS_LEGACY_USB3                 BIT31
#define B_SB_SMI_STS_SERIAL_IO                   BIT29
#define B_SB_SMI_STS_ESPI                        BIT28
#define B_SB_SMI_STS_GPIO_UNLOCK                 BIT27
#define B_SB_SMI_STS_SPI                         BIT26
#define B_SB_SMI_STS_MONITOR                     BIT21
#define B_SB_SMI_STS_PCI_EXP                     BIT20
#define B_SB_SMI_STS_PATCH                       BIT19
#define B_SB_SMI_STS_INTEL_USB2                  BIT18
#define B_SB_SMI_STS_LEGACY_USB2                 BIT17
#define B_SB_SMI_STS_SMBUS                       BIT16
#define B_SB_SMI_STS_SERIRQ                      BIT15
#define B_SB_SMI_STS_PERIODIC                    BIT14
#define B_SB_SMI_STS_TCO                         BIT13
#define B_SB_SMI_STS_DEVMON                      BIT12
#define B_SB_SMI_STS_MCSMI                       BIT11
#define B_SB_SMI_STS_GPIO_SMI                    BIT10
#define B_SB_SMI_STS_GPE0                        BIT9
#define B_SB_SMI_STS_PM1_STS_REG                 BIT8
#define B_SB_SMI_STS_SWSMI_TMR                   BIT6
#define B_SB_SMI_STS_APM                         BIT5
#define B_SB_SMI_STS_ON_SLP_EN                   BIT4
#define B_SB_SMI_STS_LEGACY_USB                  BIT3
#define B_SB_SMI_STS_BIOS                        BIT2

#define R_SB_ACPI_GPE_CNTL                       0x40        ///> General Purpose Event Control Reg.
#define B_SB_ACPI_GPE_CNTL_SWGPE_CTRL            BIT17

#define R_SB_DEVACT_STS                          0x44        ///> Device Activity Status Reg.
#define S_SB_DEVACT_STS                          2
#define B_SB_DEVACT_STS_MASK                     0x13E1
#define B_SB_DEVACT_STS_KBC                      0x1000
#define B_SB_DEVACT_STS_PIRQDH                   0x0200
#define B_SB_DEVACT_STS_PIRQCG                   0x0100
#define B_SB_DEVACT_STS_PIRQBF                   0x0080
#define B_SB_DEVACT_STS_PIRQAE                   0x0040
#define B_SB_DEVACT_STS_D0_TRP                   0x0001

#define R_SB_ACPI_PM2_CNT                        0x50        ///> Power Management 2 Control Reg.
#define B_SB_ACPI_PM2_CNT_ARB_DIS                0x01

#define R_SB_OC_WDT_CTL                          0x54

#define R_SB_ACPI_GPE0_STS_127_96                0x8C
#define B_SB_ACPI_GPE0_STS_127_96_WADT           BIT18
#define B_SB_ACPI_GPE0_STS_127_96_GP27           BIT16
#define B_SB_ACPI_GPE0_STS_127_96_GPIO_TIER2_SCI BIT15
#define B_SB_ACPI_GPE0_STS_127_96_PME_B0         BIT13
#define B_SB_ACPI_GPE0_STS_127_96_ME_SCI         BIT12
#define B_SB_ACPI_GPE0_STS_127_96_PME            BIT11
#define B_SB_ACPI_GPE0_STS_127_96_BATLOW         BIT10
#define B_SB_ACPI_GPE0_STS_127_96_PCI_EXP        BIT9
#define B_SB_ACPI_GPE0_STS_127_96_RI             BIT8
#define B_SB_ACPI_GPE0_STS_127_96_SMB_WAK        BIT7
#define B_SB_ACPI_GPE0_STS_127_96_TC0SCI         BIT6
#define B_SB_ACPI_GPE0_STS_127_96_SWGPE          BIT2
#define B_SB_ACPI_GPE0_STS_127_96_HOT_PLUG       BIT1

#define R_SB_ACPI_GPE0_EN_127_96                 0x9C
#define B_SB_ACPI_GPE0_EN_127_96_WADT            BIT18
#define B_SB_ACPI_GPE0_EN_127_96_GP27            BIT16
#define B_SB_ACPI_GPE0_EN_127_96_GPIO_TIER2_SCI  BIT15
#define B_SB_ACPI_GPE0_EN_127_96_PME_B0          BIT13
#define B_SB_ACPI_GPE0_EN_127_96_ME_SCI          BIT12
#define B_SB_ACPI_GPE0_EN_127_96_PME             BIT11
#define B_SB_ACPI_GPE0_EN_127_96_BATLOW          BIT10
#define B_SB_ACPI_GPE0_EN_127_96_PCI_EXP         BIT9
#define B_SB_ACPI_GPE0_EN_127_96_RI              BIT8
#define B_SB_ACPI_GPE0_EN_127_96_TC0SCI          BIT6
#define B_SB_ACPI_GPE0_EN_127_96_SWGPE           BIT2
#define B_SB_ACPI_GPE0_EN_127_96_HOT_PLUG        BIT1

#define R_SB_ACPI_GPE0a_STS                      0x20
#define R_SB_ACPI_GPE0b_STS                      0x24
#define R_SB_ACPI_GPE0a_EN                       0x28

//
// PWRM Registers
//
#define R_SB_PWRM_ST_PG_FDIS_PMC_1                         0x620 ///< Static PG Related Function Disable Register 1
#define B_SB_PWRM_ST_PG_FDIS_PMC_1_ST_FDIS_LK              BIT31 ///< Static Function Disable Lock (ST_FDIS_LK)
#define B_SB_PWRM_ST_PG_FDIS_PMC_1_CAM_FDIS_PMC            BIT6  ///< Camera Function Disable (PMC Version) (CAM_FDIS_PMC)
#define B_SB_PWRM_ST_PG_FDIS_PMC_1_ISH_FDIS_PMC            BIT5  ///< SH Function Disable (PMC Version) (ISH_FDIS_PMC)
#define B_SB_PWRM_ST_PG_FDIS_PMC_1_GBE_FDIS_PMC            BIT0  ///< GBE Function Disable (PMC Version) (GBE_FDIS_PMC)
#define R_SB_PWRM_ST_PG_FDIS_PMC_2                         0x624 ///< Static Function Disable Control Register 2
#define B_SB_PWRM_ST_PG_FDIS_PMC_2_SERIALIO_GSPI1_FDIS_PMC BIT10 ///< SerialIo Controller GSPI Device 1 Function Disable
#define B_SB_PWRM_ST_PG_FDIS_PMC_2_SERIALIO_GSPI0_FDIS_PMC BIT9  ///< SerialIo Controller GSPI Device 0 Function Disable
#define B_SB_PWRM_ST_PG_FDIS_PMC_2_SERIALIO_UART2_FDIS_PMC BIT8  ///< SerialIo Controller UART Device 2 Function Disable
#define B_SB_PWRM_ST_PG_FDIS_PMC_2_SERIALIO_UART1_FDIS_PMC BIT7  ///< SerialIo Controller UART Device 1 Function Disable
#define B_SB_PWRM_ST_PG_FDIS_PMC_2_SERIALIO_UART0_FDIS_PMC BIT6  ///< SerialIo Controller UART Device 0 Function Disable
#define B_SB_PWRM_ST_PG_FDIS_PMC_2_SERIALIO_I2C5_FDIS_PMC  BIT5  ///< SerialIo Controller I2C Device 5 Function Disable
#define B_SB_PWRM_ST_PG_FDIS_PMC_2_SERIALIO_I2C4_FDIS_PMC  BIT4  ///< SerialIo Controller I2C Device 4 Function Disable
#define B_SB_PWRM_ST_PG_FDIS_PMC_2_SERIALIO_I2C3_FDIS_PMC  BIT3  ///< SerialIo Controller I2C Device 3 Function Disable
#define B_SB_PWRM_ST_PG_FDIS_PMC_2_SERIALIO_I2C2_FDIS_PMC  BIT2  ///< SerialIo Controller I2C Device 2 Function Disable
#define B_SB_PWRM_ST_PG_FDIS_PMC_2_SERIALIO_I2C1_FDIS_PMC  BIT1  ///< SerialIo Controller I2C Device 1 Function Disable
#define B_SB_PWRM_ST_PG_FDIS_PMC_2_SERIALIO_I2C0_FDIS_PMC  BIT0  ///< SerialIo Controller I2C Device 0 Function Disable
#define R_SB_PWRM_NST_PG_FDIS_1                            0x628
#define B_SB_PWRM_NST_PG_FDIS_1_SMB_FDIS_PMC               BIT25 ///< SMB Function Disable
#define B_SB_PWRM_NST_PG_FDIS_1_LPC_FDIS_PMC               BIT24 ///< LPC Function Disable
#define B_SB_PWRM_NST_PG_FDIS_1_ADSP_FDIS_PMC              BIT23 ///< ADSP Function Disable
#define B_SB_PWRM_NST_PG_FDIS_1_SATA_FDIS_PMC              BIT22 ///< SATA Function Disable
#define B_SB_PWRM_NST_PG_FDIS_1_PCIE_C3_FDIS_PMC           BIT13 ///< PCIe Controller C Port 3 Function Disable
#define B_SB_PWRM_NST_PG_FDIS_1_PCIE_C2_FDIS_PMC           BIT12 ///< PCIe Controller C Port 2 Function Disable
#define B_SB_PWRM_NST_PG_FDIS_1_PCIE_C1_FDIS_PMC           BIT11 ///< PCIe Controller C Port 1 Function Disable
#define B_SB_PWRM_NST_PG_FDIS_1_PCIE_C0_FDIS_PMC           BIT10 ///< PCIe Controller C Port 0 Function Disable
#define B_SB_PWRM_NST_PG_FDIS_1_PCIE_B3_FDIS_PMC           BIT9  ///< PCIe Controller B Port 3 Function Disable
#define B_SB_PWRM_NST_PG_FDIS_1_PCIE_B2_FDIS_PMC           BIT8  ///< PCIe Controller B Port 2 Function Disable
#define B_SB_PWRM_NST_PG_FDIS_1_PCIE_B1_FDIS_PMC           BIT7  ///< PCIe Controller B Port 1 Function Disable
#define B_SB_PWRM_NST_PG_FDIS_1_PCIE_B0_FDIS_PMC           BIT6  ///< PCIe Controller B Port 0 Function Disable
#define B_SB_PWRM_NST_PG_FDIS_1_PCIE_A3_FDIS_PMC           BIT5  ///< PCIe Controller A Port 3 Function Disable
#define B_SB_PWRM_NST_PG_FDIS_1_PCIE_A2_FDIS_PMC           BIT4  ///< PCIe Controller A Port 2 Function Disable
#define B_SB_PWRM_NST_PG_FDIS_1_PCIE_A1_FDIS_PMC           BIT3  ///< PCIe Controller A Port 1 Function Disable
#define B_SB_PWRM_NST_PG_FDIS_1_PCIE_A0_FDIS_PMC           BIT2  ///< PCIe Controller A Port 0 Function Disable
#define B_SB_PWRM_NST_PG_FDIS_1_XHCI_FDIS_PMC              BIT0  ///< XHCI Function Disable

//----------------------------------------------------------------------------
// INTEL PCH Legacy Power Management I/O Registers
//----------------------------------------------------------------------------
#define ICH_IOREG_APMC          0xb2    // APM Control I/O Address
#define ICH_IOREG_APMS          0xb3    // APM Status I/O Address

//----------------------------------------------------------------------------
// INTEL PCH High Definition Audio Controller (D31:F3) Equates
//----------------------------------------------------------------------------
#define R_SB_HDA_COMMAND        0x04    // 
#define B_SB_HDA_COMMAND_BME    BIT02   //
#define B_SB_HDA_COMMAND_MSE    BIT01   // 
#define R_SB_HDA_HDBARL         0x10    // HD Audio Base Lower Address
#define R_SB_HDA_HDBARU         0x14    // HD Audio Base Upper Address
#define V_SB_HDA_HDBAR_SIZE     (1 << 14)
#define R_SB_HDA_SVID           0x2c    // Subsystem Vendor ID Reg.

//----------------------------------------------------------------------------
// INTEL PCH High Definition Audio Memory Mapped I/O Registers
//----------------------------------------------------------------------------
//
// Resides in 'HD Audio Global Registers' (0000h)
//
#define R_SB_HDA_STATESTS       0x0E     //
#define R_SB_HDA_STATESTS_SDIN3 BIT03
#define R_SB_HDA_STATESTS_SDIN2 BIT02
#define R_SB_HDA_STATESTS_SDIN1 BIT01
#define R_SB_HDA_STATESTS_SDIN0 BIT00
//
// Resides in 'HD Audio Controller Registers' (0030h)
//
#define R_SB_HDA_IC             0x60
#define R_SB_HDA_IR             0x64
#define R_SB_HDA_IRS            0x68
#define B_SB_HDA_IRS_IRV        BIT01
#define B_SB_HDA_IRS_ICB        BIT00

//----------------------------------------------------------------------------
// INTEL PCH SMBus Controller (D31:F4) Equates             
//----------------------------------------------------------------------------
#define R_SB_SMBUS_DEVID        0x02    // Device ID Reg.
#define R_SB_SMBUS_PCICMD        0x04    // PCI Command Register
#define R_SB_SMBUS_RID           0x08    // Revision Identification Register
#define R_SB_SMBUS_MBASE0_ADDR   0x10    // SMBus Memory base 0 address register 
#define R_SB_SMBUS_MBASE1_ADDR   0x14    // SMBus Memory base 1 address register 
#define R_SB_SMBUS_BASE_ADDR     0x20    // SMBus I/O base address register
#define R_SB_SMBUS_SVID          0x2c    // SMBus System Vendor ID register
#define R_SB_SMBUS_INTR_LN       0x3c    // Interrupt Line Register
#define R_SB_SMBUS_HOST_CONFIG   0x40    // SMBUS Host Configuration register
#define R_SB_SMBUS_TCOBASE                   0x50
#define B_SB_SMBUS_TCOBASE_BAR               0x0000FFE0
#define R_SB_SMBUS_TCOCTL                    0x54
#define B_SB_SMBUS_TCOCTL_TCO_BASE_EN        BIT8
#define B_SB_SMBUS_TCOCTL_TCO_BASE_LOCK      BIT0
#define R_SB_SMBUS_80                        0x80

//----------------------------------------------------------------------------
// INTEL PCH System Management TCO I/O Registers
//----------------------------------------------------------------------------
#define TCO_IOREG_RLD           0x00    // TCO Timer Reload and Current Value
#define TCO_IOREG_DAT_IN        0x02    // TCO Data In Reg.
#define TCO_IOREG_DAT_OUT       0x03    // TCO Data Out Reg.
#define TCO_IOREG_STS1          0x04    // TCO Status 1 Reg.
#define ICH_IOREG_TCO1_STS      TCO_IOREG_STS1
#define TCO_IOREG_STS2          0x06    // TCO Status 2 Reg.
#define TCO_IOREG_CNT1          0x08    // TCO Control 1 Reg.
#define TCO_IOREG_CNT2          0x0a    // TCO Control 2 Reg.
#define TCO_IOREG_MESSAGE1      0x0c    // TCO Message 1 Reg.
#define TCO_IOREG_MESSAGE2      0x0d    // TCO Message 2 Reg.
#define TCO_IOREG_WDCNT         0x0e    // TCO Watchdog Control Reg.
#define TCO_IOREG_SWIRQ_GEN     0x10    // Software IRQ Generation Reg.
#define TCO_IOREG_TMR           0x12    // TCO Timer Initial Value Reg

//----------------------------------------------------------------------------
// INTEL PCH SPI Controller (D31:F5) Equates                (Porting Required)
//----------------------------------------------------------------------------
#define R_SB_SPI_BAR0                      0x10
#define B_SB_SPI_BAR0_MASK                 0x0FFF
#define R_SB_SPI_SVID                      0x2C    // Subsystem Vendor IDs Reg.
#define R_SB_SPI_BDE                       0xD8      ///> BIOS Decode Enable
#define R_SB_SPI_BC                        0xDC      ///> BIOS Control
//
// BIOS Flash Program Registers (based on SPI_BAR0)
//
#define R_SB_SPI_HSFSC                     0x04      ///> Hardware Sequencing Flash Status and Control Register(32bits)
#define B_SB_SPI_HSFSC_FSMIE               BIT31   ///> Flash SPI SMI# Enable
#define B_SB_SPI_HSFSC_CYCLE_FGO           BIT16                         ///< Flash Cycle Go.
#define B_SB_SPI_HSFSC_FLOCKDN             BIT15                         ///< Flash Configuration Lock-Down
#define B_SB_SPI_HSFSC_FDV                 BIT14                         ///< Flash Descriptor Valid, once valid software can use hareware sequencing regs
#define B_SB_SPI_HSFSC_FDOPSS              BIT13                         ///< Flash Descriptor Override Pin-Strap Status
#define B_SB_SPI_HSFSC_PRR34_LOCKDN        BIT12                         ///< PRR3 PRR4 Lock-Down
#define B_SB_SPI_HSFSC_SAF_CE              BIT8                          ///< SAF ctype error
#define B_SB_SPI_HSFSC_SAF_MODE_ACTIVE     BIT7                          ///< Indicates flash is attached either directly to the PCH via the SPI bus or EC/BMC
#define B_SB_SPI_HSFSC_SAF_LE              BIT6                          ///< SAF link error
#define B_SB_SPI_HSFSC_SCIP                BIT5                          ///< SPI cycle in progress
#define B_SB_SPI_HSFSC_SAF_DLE             BIT4                          ///< SAF Data length error
#define B_SB_SPI_HSFSC_SAF_ERROR           BIT3                          ///< SAF Error
#define B_SB_SPI_HSFSC_AEL                 BIT2                          ///< Access Error Log
#define B_SB_SPI_HSFSC_FCERR               BIT1                          ///< Flash Cycle Error
#define B_SB_SPI_HSFSC_FDONE               BIT0                          ///< Flash Cycle Done

#define R_SB_SPI_FRAP                      0x50                          ///< SPI Flash Regions Access Permisions Register
#define R_SB_SPI_PR0                       0x84                          ///< Protected Region 0 Register
#define R_SB_SPI_PR1                       0x88                          ///< Protected Region 1 Register
#define R_SB_SPI_PR2                       0x8C                          ///< Protected Region 2 Register
#define R_SB_SPI_PR3                       0x90                          ///< Protected Region 3 Register
#define R_SB_SPI_PR4                       0x94                          ///< Protected Region 4 Register
#define S_SB_SPI_PRX                       4                             ///< Protected Region X Register size
#define B_SB_SPI_PRX_WPE                   BIT31                         ///< Write Protection Enable
#define B_SB_SPI_PRX_PRL_MASK              0x7FFF0000                    ///< Protected Range Limit Mask, [30:16] here represents upper limit of address [26:12]
#define N_SB_SPI_PRX_PRL                   16                            ///< Protected Range Limit bit position
#define B_SB_SPI_PRX_RPE                   BIT15                         ///< Read Protection Enable
#define B_SB_SPI_PRX_PRB_MASK              0x00007FFF                    ///< Protected Range Base Mask, [14:0] here represents base limit of address [26:12]
#define N_SB_SPI_PRX_PRB                   0                             ///< Protected Range Base bit position

#define R_SB_SPI_SSFSTS                    0xA0    ///> Software Sequencing Flash Status and Control

#define R_SB_SPI_POOT                      0xA4    ///> Prefix Opcode and Opcode Type Configuration
#define R_SB_SPI_OPMENU0                   0xA8    ///> Opcode Menu 0 Configuration
#define R_SB_SPI_OPMENU1                   0xAC    ///> Opcode Menu 1 Configuration

#define R_SB_SPI_FDOC                      0xB4    ///> Flash Descriptor Observability Control Register(32 bits)
#define R_SB_SPI_FDOD                      0xB8    ///> Flash Descriptor Observability Data Register(32 bits)
#define R_SB_SPI_SFDP0_LVSCC               0xC4    ///> Vendor Specific Component Capabilities Register(32 bits)
#define R_SB_SPI_SFDP1_UVSCC               0xC8    ///> Vendor Specific Component Capabilities Register(32 bits)
/*
#define R_RCRB_SPI_FADDR        0x08    // Flash Address Register
#define R_RCRB_SPI_FDATA0       0x10    // Flash Data 0 Register
#define R_RCRB_SPI_SSFSTS       0xA0    // Software Squencing Flash Status
#define R_RCRB_SPI_SSFCTL       0xA1    // Software Squencing Flash Control
#define R_RCRB_SPI_PREOP        0xA4    // Prefix Opcode Configuration
#define R_RCRB_SPI_OPTYPE       0xA6    // Opcode Type Configuration
#define R_RCRB_SPI_OPMENU       0xA8    // Opcode Menu Configuration
#define R_RCRB_SPI_LVSCC        0xC4    // Host Lower Vendor Specific Component Capabilities Register
#define R_RCRB_SPI_UVSCC        0xC8    // Host Upper Vendor Specific Component Capabilities Register

#define B_PCH_SPI_FDOC_FDSS_MASK      (BIT14 | BIT13 | BIT12)       // Flash Descritor Section Select
#define V_PCH_SPI_FDOC_FDSS_PCHS      0x4000                        // ICH soft straps
#define B_PCH_SPI_FDOC_FDSI_MASK      0x0FFC                        // Flash Descriptor Section Index
#define R_PCH_SPI_STRP15              0x3C
*/
//----------------------------------------------------------------------------
// INTEL PCH Gigabit LAN Controller (D31:F6) Equates
//----------------------------------------------------------------------------
#define R_SB_LAN_MBARA           0x10    // Memory Base Address Register A
#define R_SB_LAN_MBARB           0x14    // Memory Base Address Register B
#define R_SB_LAN_SVID            0x2C    // Subsystem Vendor ID register
#define R_SB_LAN_LANDISCTRL          0xA0
#define B_SB_LAN_LANDISCTRL_DISABLE  BIT0
#define R_SB_LAN_LOCKLANDIS          0xA4
#define B_SB_LAN_LOCKLANDIS_LOCK     BIT0
#define R_SB_LAN_PMCS            0xCC    // PCI Power Management Control and Status
#define B_SB_LAN_PMCS_PMES       BIT15
#define B_SB_LAN_PMCS_PMEE       BIT8

//----------------------------------------------------------------------------
// INTEL PCH Trace Hub(North Peak) Controller (D31:F7) Equates
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// INTEL Serial IO I2C Device Controller Equates
//----------------------------------------------------------------------------
#define R_SB_SIO_I2C_SVID           0x2C    // Subsystem Vendor IDs Reg.
//----------------------------------------------------------------------------
// INTEL Serial IO UART Device Controller Equates
//----------------------------------------------------------------------------
#define R_SB_SIO_UART_SVID          0x2C    // Subsystem Vendor IDs Reg.
//----------------------------------------------------------------------------
// INTEL Serial IO GSPI Device Controller Equates
//----------------------------------------------------------------------------
#define R_SB_SIO_GSPI_SVID          0x2C    // Subsystem Vendor IDs Reg.

//----------------------------------------------------------------------------
// INTEL PCH System Management Bus I/O Space Equates
//----------------------------------------------------------------------------
#define SMB_IOREG_HST_STS        0x00   // Host Status Reg.
#define     HST_STS_HOST_BUSY        0x01    // R/WC
#define     HST_STS_INTR             0x02    // R/WC
#define     HST_STS_DEV_ERR          0x04    // R/WC
#define     HST_STS_BUS_ERR          0x08    // R/WC
#define     HST_STS_FAILED           0x10    // R/WC
#define     HST_STS_SMBALERT_STS     0x20    // R/WC
#define     HST_STS_INUSE_STS        0x40    // R/WC
#define     HST_STS_BDS              0x80    // R/WC
#define     HST_STS_ALL              0xff    // R/WC
#define     HST_STS_ERROR            0x1c    // R/WC
#define SMB_IOREG_HST_CNT        0x02   // Host Control Reg.
#define     HST_CNT_INTREN           0x01    // RW
#define     HST_CNT_KILL             0x02    // RW
#define     HST_CNT_SMB_CMD          0x1C    // RW
#define         SMB_CMD_BYTE             0x04
#define         SMB_CMD_BYTE_DATA        0x08
#define         SMB_CMD_WORD_DATA        0x0c
#define         SMB_CMD_BLOCK            0x14
#define     HST_CNT_LAST_BYTE        0x20    // RW
#define     HST_CNT_START            0x40    // RW
#define     HST_CNT_PEC_EN           0x80    // RW
#define SMB_IOREG_HST_CMD        0x03   // Host Command Reg.
#define SMB_IOREG_XMIT_SLVA      0x04   // Transmit Slave Address Reg.
#define     XMIT_SLVA_RW             0x01    // RW
#define SMB_IOREG_HST_D0         0x05   // Host Data 0
#define SMB_IOREG_HST_D1         0x06   // Host Data 1
#define SMB_IOREG_HOST_BLOCK_DB  0x07   // Host Block Data Byte Reg.
#define SMB_IOREG_PEC            0x08   // Packet Error Check Reg.
#define SMB_IOREG_RCV_SLVA       0x09   // Receive Slave Address Reg.
#define SMB_IOREG_SLV_DATA0      0x0a   // Receive Slave Data 0 Reg.
#define SMB_IOREG_SLV_DATA1      0x0b   // Receive Slave Data 1 Reg.
#define SMB_IOREG_AUX_STS        0x0c   // Auxiliary Status Reg.
#define     AUX_STS_CRC_ERR          0x01    // R/WC
#define     AUX_STS_STCO             0x02    // RO
#define SMB_IOREG_AUX_CTL        0x0d   // Auxiliary Control Reg.
#define     AUX_CTL_AAC              0x01    // R/W
#define     AUX_CTL_E32B             0x02    // R/W
#define SMB_IOREG_SMLINK_PIN_CTL 0x0e   // SMLink Pin Control Reg.
#define SMB_IOREG_SMBUS_PIN_CTL  0x0f   // SMBus Pin Control Reg.
#define SMB_IOREG_SLV_STS        0x10   // Slave Status Reg.
#define SMB_IOREG_SLV_CMD        0x11   // Slave Command Reg.
#define SMB_IOREG_NOTIFY_DADDR   0x14   // Notify Device Address Reg.
#define SMB_IOREG_NOTIFY_DLOW    0x16   // Notify Data Low Reg.
#define SMB_IOREG_NOTIFY_DHIGH   0x17   // Notify Data High Reg.

#ifndef SMBUS_NUM_RESERVED
#define SMBUS_NUM_RESERVED      21      // Number of device addresses
                                        // that are reserved by the
                                        // SMBus spec.
#endif

#define SMBUS_DEVICE_DEFAULT_ADDRESS    0xc2 >> 1
#define PREPARE_TO_ARP                  0x01
#define GET_UDID_DIRECTED               0x01
#define RESET_DEVICE_GENERAL            0x02
#define GET_UDID_GENERAL                0x03
#define ASSIGN_ADDRESS                  0x04
#define GET_UDID_DATA_LENGTH            0x11    // 16 byte UDID + 1 byte address

//----------------------------------------------------------------------------
// PSFx PCRs definitions
//----------------------------------------------------------------------------

//
// PSFx PCRs definitions
//
#define R_SB_PCR_PSFX_T0_SHDW_PCIEN                    0x1C                    ///< PCI configuration space enable bits
//
// PSF1 PCRs
//
// PSF1 PCH-LP Specific Base Address
#define R_SB_LP_PCR_PSF1_T0_SHDW_GBE_REG_BASE          0x0200                  ///< D31F6 PSF base address (GBE)
#define R_SB_LP_PCR_PSF1_T0_SHDW_CAM_REG_BASE          0x0300                  ///< D20F3 PSF base address (CAM)
#define R_SB_LP_PCR_PSF1_T0_SHDW_CSE_WLAN_REG_BASE     0x0500                  ///< D22F7 PSF base address (CSME: WLAN)
#define R_SB_LP_PCR_PSF1_T0_SHDW_HECI3_REG_BASE        0x0700                  ///< D22F4 PSF base address (CSME: HECI3)
#define R_SB_LP_PCR_PSF1_T0_SHDW_HECI2_REG_BASE        0x0800                  ///< D22F1 PSF base address (CSME: HECI2)
#define R_SB_LP_PCR_PSF1_T0_SHDW_CSE_UMA_REG_BASE      0x0900                  ///< D18F3 PSF base address (CSME: CSE UMA)
#define R_SB_LP_PCR_PSF1_T0_SHDW_HECI1_REG_BASE        0x0A00                  ///< D22F0 PSF base address (CSME: HECI1)
#define R_SB_LP_PCR_PSF1_T0_SHDW_KT_REG_BASE           0x0B00                  ///< D22F3 PSF base address (CSME: KT)
#define R_SB_LP_PCR_PSF1_T0_SHDW_IDER_REG_BASE         0x0C00                  ///< D22F2 PSF base address (CSME: IDER)
#define R_SB_LP_PCR_PSF1_T0_SHDW_CLINK_REG_BASE        0x0D00                  ///< D18F1 PSF base address (CSME: CLINK)
#define R_SB_LP_PCR_PSF1_T0_SHDW_PMT_REG_BASE          0x0E00                  ///< D18F2 PSF base address (CSME: PMT)
#define R_SB_LP_PCR_PSF1_T0_SHDW_KVM_REG_BASE          0x0F00                  ///< D18F0 PSF base address (CSME: KVM)
#define R_SB_LP_PCR_PSF1_T0_SHDW_SATA_REG_BASE         0x1000                  ///< PCH-LP D23F0 PSF base address (SATA)
#define R_SB_LP_PCR_PSF1_T1_SHDW_PCIE12_REG_BASE       0x2000                  ///< PCH-LP D29F3 PSF base address (PCIE PORT 12)
#define R_SB_LP_PCR_PSF1_T1_SHDW_PCIE11_REG_BASE       0x2100                  ///< PCH-LP D29F2 PSF base address (PCIE PORT 11)
#define R_SB_LP_PCR_PSF1_T1_SHDW_PCIE10_REG_BASE       0x2200                  ///< PCH-LP D29F1 PSF base address (PCIE PORT 10)
#define R_SB_LP_PCR_PSF1_T1_SHDW_PCIE09_REG_BASE       0x2300                  ///< PCH-LP D29F0 PSF base address (PCIE PORT 09)
#define R_SB_LP_PCR_PSF1_T1_SHDW_PCIE08_REG_BASE       0x2400                  ///< PCH-LP D28F7 PSF base address (PCIE PORT 08)
#define R_SB_LP_PCR_PSF1_T1_SHDW_PCIE07_REG_BASE       0x2500                  ///< PCH-LP D28F6 PSF base address (PCIE PORT 07)
#define R_SB_LP_PCR_PSF1_T1_SHDW_PCIE06_REG_BASE       0x2600                  ///< PCH-LP D28F5 PSF base address (PCIE PORT 06)
#define R_SB_LP_PCR_PSF1_T1_SHDW_PCIE05_REG_BASE       0x2700                  ///< PCH-LP D28F4 PSF base address (PCIE PORT 05)
#define R_SB_LP_PCR_PSF1_T1_SHDW_PCIE04_REG_BASE       0x2800                  ///< PCH-LP D28F3 PSF base address (PCIE PORT 04)
#define R_SB_LP_PCR_PSF1_T1_SHDW_PCIE03_REG_BASE       0x2900                  ///< PCH-LP D28F2 PSF base address (PCIE PORT 03)
#define R_SB_LP_PCR_PSF1_T1_SHDW_PCIE02_REG_BASE       0x2A00                  ///< PCH-LP D28F1 PSF base address (PCIE PORT 02)
#define R_SB_LP_PCR_PSF1_T1_SHDW_PCIE01_REG_BASE       0x2B00                  ///< PCH-LP D28F0 PSF base address (PCIE PORT 01)
// PSF1 PCH-H Specific Base Address
#define R_SB_H_PCR_PSF1_T0_SHDW_CSE_WLAN_REG_BASE      0x0200                  ///< D22F7 PSF base address (CSME: WLAN)
#define R_SB_H_PCR_PSF1_T0_SHDW_HECI3_REG_BASE         0x0300                  ///< SPT-H D22F4 PSF base address (CSME: HECI3)
#define R_SB_H_PCR_PSF1_T0_SHDW_HECI2_REG_BASE         0x0400                  ///< SPT-H D22F1 PSF base address (CSME: HECI2)
#define R_SB_H_PCR_PSF1_T0_SHDW_CSE_UMA_REG_BASE       0x0500                  ///< D18F3 PSF base address (CSME: CSE UMA)
#define R_SB_H_PCR_PSF1_T0_SHDW_HECI1_REG_BASE         0x0600                  ///< SPT-H D22F0 PSF base address (CSME: HECI1)
#define R_SB_H_PCR_PSF1_T0_SHDW_KT_REG_BASE            0x0700                  ///< SPT-H D22F3 PSF base address (CSME: KT)
#define R_SB_H_PCR_PSF1_T0_SHDW_IDER_REG_BASE          0x0800                  ///< SPT-H D22F2 PSF base address (CSME: IDER)
#define R_SB_H_PCR_PSF1_T0_SHDW_CLINK_REG_BASE         0x0900                  ///< D18F1 PSF base address (CSME: CLINK)
#define R_SB_H_PCR_PSF1_T0_SHDW_PMT_REG_BASE           0x0A00                  ///< D18F2 PSF base address (CSME: PMT)
#define R_SB_H_PCR_PSF1_T0_SHDW_KVM_REG_BASE           0x0B00                  ///< D18F0 PSF base address (CSME: KVM)
#define R_SB_H_PCR_PSF1_T0_SHDW_SATA_REG_BASE          0x0C00                  ///< PCH-H D23F0 PSF base address (SATA)
#define R_SB_H_PCR_PSF1_T1_SHDW_PCIE20_REG_BASE        0x2000                  ///< PCH-H D27F3 PSF base address (PCIE PORT 20)
#define R_SB_H_PCR_PSF1_T1_SHDW_PCIE19_REG_BASE        0x2100                  ///< PCH-H D27F2 PSF base address (PCIE PORT 19)
#define R_SB_H_PCR_PSF1_T1_SHDW_PCIE18_REG_BASE        0x2200                  ///< PCH-H D27F1 PSF base address (PCIE PORT 18)
#define R_SB_H_PCR_PSF1_T1_SHDW_PCIE17_REG_BASE        0x2300                  ///< PCH-H D27F0 PSF base address (PCIE PORT 17)
#define R_SB_H_PCR_PSF1_T1_SHDW_PCIE16_REG_BASE        0x2400                  ///< PCH-H D29F7 PSF base address (PCIE PORT 16)
#define R_SB_H_PCR_PSF1_T1_SHDW_PCIE15_REG_BASE        0x2500                  ///< PCH-H D29F6 PSF base address (PCIE PORT 15)
#define R_SB_H_PCR_PSF1_T1_SHDW_PCIE14_REG_BASE        0x2600                  ///< PCH-H D29F5 PSF base address (PCIE PORT 14)
#define R_SB_H_PCR_PSF1_T1_SHDW_PCIE13_REG_BASE        0x2700                  ///< PCH-H D29F4 PSF base address (PCIE PORT 13)
#define R_SB_H_PCR_PSF1_T1_SHDW_PCIE12_REG_BASE        0x2800                  ///< PCH-H D29F3 PSF base address (PCIE PORT 10)
#define R_SB_H_PCR_PSF1_T1_SHDW_PCIE11_REG_BASE        0x2900                  ///< PCH-H D29F2 PSF base address (PCIE PORT 11)
#define R_SB_H_PCR_PSF1_T1_SHDW_PCIE10_REG_BASE        0x2A00                  ///< PCH-H D29F1 PSF base address (PCIE PORT 10)
#define R_SB_H_PCR_PSF1_T1_SHDW_PCIE09_REG_BASE        0x2B00                  ///< PCH-H D29F0 PSF base address (PCIE PORT 09)
#define R_SB_H_PCR_PSF1_T1_SHDW_PCIE08_REG_BASE        0x2C00                  ///< PCH-H D28F7 PSF base address (PCIE PORT 08)
#define R_SB_H_PCR_PSF1_T1_SHDW_PCIE07_REG_BASE        0x2D00                  ///< PCH-H D28F6 PSF base address (PCIE PORT 07)
#define R_SB_H_PCR_PSF1_T1_SHDW_PCIE06_REG_BASE        0x2E00                  ///< PCH-H D28F5 PSF base address (PCIE PORT 06)
#define R_SB_H_PCR_PSF1_T1_SHDW_PCIE05_REG_BASE        0x2F00                  ///< PCH-H D28F4 PSF base address (PCIE PORT 05)
#define R_SB_H_PCR_PSF1_T1_SHDW_PCIE04_REG_BASE        0x3000                  ///< PCH-H D28F3 PSF base address (PCIE PORT 04)
#define R_SB_H_PCR_PSF1_T1_SHDW_PCIE03_REG_BASE        0x3100                  ///< PCH-H D28F2 PSF base address (PCIE PORT 03)
#define R_SB_H_PCR_PSF1_T1_SHDW_PCIE02_REG_BASE        0x3200                  ///< PCH-H D28F1 PSF base address (PCIE PORT 02)
#define R_SB_H_PCR_PSF1_T1_SHDW_PCIE01_REG_BASE        0x3300                  ///< PCH-H D28F0 PSF base address (PCIE PORT 01)
//----------------------------------------------------------------------------
// INTEL PCH Misc. I/O Registers
//----------------------------------------------------------------------------
#define KBC_IO_DATA             0x60    // Keyboard Controller Data Port
#define PORTB_IO_CNTL           0x61    // Port B control Register
#define KBC_IO_STS              0x64    // Keyboard Controller Status Port
#define RTC_IO_INDEX            0x70    // CMOS I/O Index Port
#define CMOS_IO_EXT_INDEX       0x72    // CMOS I/O Extended Index Port
#define CMOS_IO_EXT_DATA        0x73    // CMOS I/O Extended Data Port
#define CMOS_IO_INDEX_BACKDOOR  0x74    // RTC I/O Index Port (Back Door)
#define CMOS_IO_DATA_BACKDOOR   0x75    // RTC I/O Data Port (Back Door)
#define RESET_PORT              0x0CF9
#define     COLD_RESET          0x02    // Set bit 1 for cold reset.
#define     RST_CPU             0x04    // Setting this bit triggers a
                                        // reset of the CPU.
#define     FULL_RESET          0x08    // Set bit 3 for full reset.
#define IO_DELAY_PORT           0xed    // Use for I/O delay

#define ONE_SECOND              1000000 // The stall PPI is defined in 
                                        // microseconds, this should be
                                        // one second in microseconds.
#endif 

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
