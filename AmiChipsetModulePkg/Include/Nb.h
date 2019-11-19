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

/** @file Nb.h
    North Bridge header file, define all the North Bridge
    specific equates and structures in this file.

**/

#ifndef  _NB_H__   // To Avoid this header get compiled twice
#define  _NB_H__

#include <Token.h>

#define NB_PCI_CFG_ADDRESS(bus, dev, func, reg) \
            (UINT64) ((((UINT8)(bus) << 24) + ((UINT8)(dev) << 16) + \
            ((UINT8)(func) << 8) + ((UINT8)(reg))) & 0xffffffff)

#ifdef PCIEX_BASE_ADDRESS 
#define NB_PCIE_CFG_ADDRESS(bus, dev, func, reg) \
            ((UINTN)(PCIEX_BASE_ADDRESS + ((UINT8)(bus) << 20) + \
            ((UINT8)(dev) << 15) + ((UINT8)(func) << 12) + (reg)))
#endif

#ifndef PCIBUS_VERSION
#define PCIBUS_VERSION ( PCI_BUS_MAJOR_VER * 10000 + \
                         PCI_BUS_MINOR_VER * 100 + \
                         PCI_BUS_REVISION )
#endif 

//-----------------------------------------------------------------------
// CSM Details
//-----------------------------------------------------------------------
// DO NOT REMOVE THE DEFINITION OF THIS STRUCTURE. THIS IS USED BY CSM ALSO
typedef struct ROOT_BRIDGE_MAPPING_TABLE {
    UINT32  rbUID;
    UINT8   rbDevFunc;
} ROOT_BRIDGE_MAPPING_ENTRY;

// For systems with config mechanism 1
#define NB_PCICFG_SPACE_INDEX_REG       0xcf8
#define NB_PCICFG_SPACE_DATA_REG        0xcfc

typedef struct {
    UINT16  MmioReg;
    UINT32  bAndMask;
    UINT32  bOrMask;
} AMI_NB_MMIO_INIT_TABLE_STRUCT;

typedef struct  {
    UINT64      PciAddr;
    UINT32      Sid;
} NB_PCI_SUBID_STRUCT;

#define GET_PRIMARY_VGA_PCI_INFO_GUID \
    {0x2fcaa4e9, 0xdc14, 0x4644, 0xbe, 0x51, 0xd2, 0x33, 0xd8, 0x20, 0xed, 0x4}

#define GET_PRIMARY_VGA_PCI_INFO_VARIABLE L"GetPriVideoPciInfo"

#define NB_ASL_BUFFER_PTR_GUID \
    {0x30bc0b91, 0x8106, 0x4f6b, 0xb8, 0x5d, 0x22, 0x47, 0x35, 0xd, 0x95, 0x38 }

#define NB_ASL_BUFFER_PTR_VARIABLE L"NbAslBufferPtrVar"


//-----------------------------------------------------------------------
// NB : Platform Details
//-----------------------------------------------------------------------

typedef union {
    UINT32 Data;
    struct 
    {
        UINT32 CPUType:4;
        UINT32 ExtCPUType:4;
        UINT32 Reserved:24;
    } Fields;
} NB_PLATFORM_INFO;


typedef struct {
    UINT8   NbAslByte0;
    UINT8   NbAslByte1;
    UINT8   NbAslByte2;
    UINT8   NbAslByte3;
    UINT8   NbAslByte4;
    UINT8   NbAslByte5;
    UINT8   NbAslByte6;
    UINT8   NbAslByte7;
    UINT8   NbAslByte8;
    UINT8   NbAslByte9;
    UINT8   NbAslByte10;
    UINT8   NbAslByte11;
    UINT8   NbAslByte12;
    UINT8   NbAslByte13;
    UINT8   NbAslByte14;
    UINT8   NbAslByte15;
} NB_ASL_BUFFER;

//----------------------------------------------------------------------------
// INTEL Processor Host Bridge PCI Bus Number Equates
//----------------------------------------------------------------------------
#define NB_BUS                  0x00    // North Bridge Bus Number
#define PCIEBRN_BUS             0x00    // North Bridge PCI Express Bridge 
                                        // Bus Number
#define IGD_BUS                 0x00    // Integrated Graphics Device 
                                        // Device Number
#define SA_HDA_BUS              0x00    //
#define SA_THERMAL_BUS			0x00	// Thermal Device (B0:D4:F0)
										// Bus Number
#define IMGU_BUS                0x00	// Imaging Unit Bus Number
#define SA_CHAP_BUS				0x00	// CHAP Device (B0:D7:F0)
										// Bus Number
#define GMM_BUS                 0x00	// Gaussian Mixture Model Device 
										// Bus Number

//----------------------------------------------------------------------------
// INTEL Processor Host Bridge Device Number Equates
//----------------------------------------------------------------------------
#define NB_DEV                  0x00    // North Bridge Device Number
#define PCIEBRN_DEV             0x01    // North Bridge PCI Express Bridge
                                        // Device Number
#define IGD_DEV                 0x02    // Integrated Graphics Device 
                                        // Device Number
#define SA_THERMAL_DEV			0x04	// Thermal Device (B0:D4:F0)
										// Device Number
#define IMGU_DEV                0x05    // Imaging Unit Device Number
#define SA_CHAP_DEV				0x07	// CHAP Device (B0:D7:F0)
										// Device Number
#define GMM_DEV                 0x08    // Gaussian Mixture Model Device
										// Device Number
//----------------------------------------------------------------------------
// INTEL Processor Host Bridge Function Number Equates
//----------------------------------------------------------------------------
#define NB_FUN                  0x00    // North Bridge Function Number
#define PCIEBRN_FUN             0x00    // North Bridge PCI Express Bridge 
                                        // Function Number
#define PCIEBRN_FUN1            0x01    // North Bridge PCI Express Bridge  
                                        // Function Number
#define PCIEBRN_FUN2            0x02    // North Bridge PCI Express Bridge 
                                        // Function Number
#define IGD_FUN                 0x00    // Integrated Graphics Device 
#define SA_THERMAL_FUN			0x00	// Thermal Device (B0:D4:F0)
										// Function Number
#define IMGU_FUN                0x00    // Imaging Unit Function Number
#define SA_CHAP_FUN				0x00	// CHAP Device (B0:D7:F0)
										// Function Number
#define GMM_FUN                 0x00    // Gaussian Mixture Model Device
										// Function Number

//----------------------------------------------------------------------------
// INTEL Processor - PCI Bus/Device/Function/Register Number Macros
//----------------------------------------------------------------------------
#define NB_REG(Reg)             NB_PCI_CFG_ADDRESS(NB_BUS, NB_DEV, \
                                                   NB_FUN, Reg)
#define PCIEBRN_REG(Reg)        NB_PCI_CFG_ADDRESS(PCIEBRN_BUS, PCIEBRN_DEV, \
                                                   PCIEBRN_FUN, Reg)
#define PCIEBRNx16_REG(Reg)     NB_PCI_CFG_ADDRESS(PCIEBRN_BUS, PCIEBRN_DEV, \
                                                   PCIEBRN_FUN, Reg)
#define PCIEBRNx8_REG(Reg)      NB_PCI_CFG_ADDRESS(PCIEBRN_BUS, PCIEBRN_DEV, \
                                                   PCIEBRN_FUN1, Reg)
#define PCIEBRNx4_REG(Reg)      NB_PCI_CFG_ADDRESS(PCIEBRN_BUS, PCIEBRN_DEV, \
                                                   PCIEBRN_FUN2, Reg)
#define IGD_REG(Reg)            NB_PCI_CFG_ADDRESS(IGD_BUS, IGD_DEV, \
                                                   IGD_FUN, Reg)
#define SA_THERMAL_REG(Reg)     NB_PCI_CFG_ADDRESS(SA_THERMAL_BUS, SA_THERMAL_DEV, \
												   SA_THERMAL_FUN, Reg)
#define IMGU_REG(Reg)           NB_PCI_CFG_ADDRESS(IMGU_BUS, IMGU_DEV, \
												   IMGU_FUN, Reg)
#define SA_CHAP_REG(Reg)        NB_PCI_CFG_ADDRESS(SA_CHAP_BUS, SA_CHAP_DEV, \
												   SA_CHAP_FUN, Reg)
#define GMM_REG(Reg)            NB_PCI_CFG_ADDRESS(GMM_BUS, GMM_DEV, \
												   GMM_FUN, Reg)

//----------------------------------------------------------------------------
// INTEL Processor - PCI Bus/Device/Function Number Macros
//----------------------------------------------------------------------------
#define NB_BUS_DEV_FUN             NB_REG(0)
#define NB_IGD_BUS_DEV_FUN         IGD_REG(0)
#define NB_PCIEBRN_BUS_DEV_FUN     PCIEBRNx16_REG(0)
#define NB_PCIEBRNx16_BUS_DEV_FUN  PCIEBRNx16_REG(0)
#define NB_PCIEBRNx8_BUS_DEV_FUN   PCIEBRNx8_REG(0)
#define NB_PCIEBRNx4_BUS_DEV_FUN   PCIEBRNx4_REG(0)
#define NB_SA_THERMAL_BUS_DEV_FUN  SA_THERMAL_REG(0)
#define NB_IMGU_BUS_DEV_FUN   	   IMGU_REG(0)
#define NB_SA_CHAP_BUS_DEV_FUN     SA_CHAP_REG(0)
#define NB_GMM_BUS_DEV_FUN   	   GMM_REG(0)

//----------------------------------------------------------------------------
// INTEL Processor - PCI Device/Vendor IDs Equates
//----------------------------------------------------------------------------
#define NB_VENID                0x8086  // North Bridge Vendor ID
#define NB_DEVID                0x0002  // North Bridge Device ID
#define PCIEBRN_VENID           0x8086  // North Bridge PCI Express Bridge
                                        // Vendor ID
#define PCIEBRN_DEVID           0x0045  // North Bridge PCI Express Bridge
                                        // Device ID
#define IGD_VENID               0x8086  // Integrated Graphics Device
                                        // Vendor ID 
#define IGD_DT_DEVID            0x0102  // Integrated Graphics Device
                                        // Device ID 
#define IGD_MB_DEVID            0x0106  // Integrated Graphics Device
                                        // Device ID
#define SA_THERMAL_VENID		0x8086	// Thermal Device (B0:D4:F0)
										// Vendor ID
#define SA_THERMAL_DEVID		0x1903	// Thermal Device (B0:D4:F0)
										// Device ID
#define IGMU_VENID              0x8086  // Imaging Unit Vendor ID
#define IGMU_DEVID              0x1919  // Imaging Unit Device ID
#define SA_CHAP_VENID			0x8086	// CHAP Device (B0:D7:F0)
										// Vendor ID
#define SA_CHAP_DEVID			0x1907	// CHAP Device (B0:D7:F0)
										// Device ID
#define GMM_VENID               0x8086  // Gaussian Mixture Model Device 
										// Vendor ID
#define GMM_DEVID               0x1911  // Gaussian Mixture Model Device 
										// Device ID

//----------------------------------------------------------------------------
// INTEL Processor - DRAM Controller Register Equates (D0:F0)
//----------------------------------------------------------------------------
#define NB_REG_VID              0x00    // Vendor ID Reg.
#define NB_REG_DID              0x02    // Device ID Reg.
#define NB_REG_SVID             0x2c    // Subsystem Vendor ID Reg.
//#define NB_REG_DEVEN            0x54    // Device Enable Reg.
//#define NB_REG_TSEGMB           0xac    // TSEG Memory Base Reg.
//#define NB_REG_LAC              0x97    // Legacy Access Control Reg.
//#define NB_REG_CAPID0           0xe0    // Capability Identifier 0 Reg.
//#define NB_REG_CAPID1           0xe4    // Capability Identifier 1 Reg.

//----------------------------------------------------------------------------
// INTEL Processor - MCH BAR Memory Mapped IO Register Equates
//----------------------------------------------------------------------------
#define MCH_MMIO_C0REFRCTRL     0x0269  // Channel 0 DRAM Refersh Control
#define MCH_MMIO_C1REFRCTRL     0x0669  // Channel 1 DRAM Refersh Control

//----------------------------------------------------------------------------
// INTEL Processor -  PCI-Express Bridge Register Equates (D1:F0)
//----------------------------------------------------------------------------
#define PCIEBRN_REG_VID         0x00    // Vendor ID Reg.
#define PCIEBRN_REG_SBUSN       0x19    // Secondary Bus Number Reg. 
#define PCIEBRN_REG_SUBUSN      0x1a    // Subordinate Bus Number Reg.
#define PCIEBRN_REG_SVID        0x8c    // Vendor ID Reg.
/**
 Description of D1F2EN (1:1)
 - 0: Bus 0 Device 1 Function 2 is disabled and hidden.
 - 1: Bus 0 Device 1 Function 2 is enabled and visible.
 - This bit will remain 0 if PEG12 capability is disabled.
**/
#define N_SA_DEVEN_D1F2EN_OFFSET   (0x1)
#define S_SA_DEVEN_D1F2EN_WIDTH    (0x1)
#define B_SA_DEVEN_D1F2EN_MASK     (0x2)
#define V_SA_DEVEN_D1F2EN_DEFAULT  (0x2)
/**
 Description of D1F1EN (2:2)
 - 0: Bus 0 Device 1 Function 1 is disabled and hidden.
 - 1: Bus 0 Device 1 Function 1 is enabled and visible.
 - This bit will remain 0 if PEG11 capability is disabled.
**/
#define N_SA_DEVEN_D1F1EN_OFFSET   (0x2)
#define S_SA_DEVEN_D1F1EN_WIDTH    (0x1)
#define B_SA_DEVEN_D1F1EN_MASK     (0x4)
#define V_SA_DEVEN_D1F1EN_DEFAULT  (0x4)
/**
 Description of D1F0EN (3:3)
 - 0: Bus 0 Device 1 Function 0 is disabled and hidden.
 - 1: Bus 0 Device 1 Function 0 is enabled and visible.
 - This bit will remain 0 if PEG10 capability is disabled.
**/
#define N_SA_DEVEN_D1F0EN_OFFSET   (0x3)
#define S_SA_DEVEN_D1F0EN_WIDTH    (0x1)
#define B_SA_DEVEN_D1F0EN_MASK     (0x8)
#define V_SA_DEVEN_D1F0EN_DEFAULT  (0x8)

//----------------------------------------------------------------------------
// INTEL Processor - Integrated Graphics Device Register Equates (D2:F0)
//----------------------------------------------------------------------------
#define IGD_REG_VID             0x00    // Vendor ID Reg.
#define IGD_REG_SVID            0x2c    // Subsystem Vendor ID Reg.

//----------------------------------------------------------------------------
// INTEL Processor - System Address Decoder Register Equates (D0:F1)
//----------------------------------------------------------------------------

#define MC_REG_PAM0            0x80    // Programmable Attribute Map 0 Reg.
#define MC_REG_PAM1            0x81    // Programmable Attribute Map 1 Reg.
#define MC_REG_PAM2            0x82    // Programmable Attribute Map 2 Reg.
#define MC_REG_PAM3            0x83    // Programmable Attribute Map 3 Reg.
#define MC_REG_PAM4            0x84    // Programmable Attribute Map 4 Reg.
#define MC_REG_PAM5            0x85    // Programmable Attribute Map 5 Reg.
#define MC_REG_PAM6            0x86    // Programmable Attribute Map 6 Reg.
#define MC_REG_SMRAM           0x88

//----------------------------------------------------------------------------
// INTEL GTTMMADR
//----------------------------------------------------------------------------
#define GTT_MEM_ALIGN     22
#define GTTMMADR_SIZE_4MB 0x400000

//----------------------------------------------------------------------------
// For Runtime Error Logging
//----------------------------------------------------------------------------

#ifndef CAP_XCAP_PORT_TYPE
#define CAP_XCAP_PORT_TYPE  0X02        // Device Port Type of XCAP Register [7:4]
#endif
#ifndef CAP_DCTL
#define CAP_DCTL            0x08        // CapPtr + Device Control
#endif
#ifndef CAP_DSTS
#define CAP_DSTS            0x0A        // CapPtr + Device Status
#endif
#ifndef CAP_RCTL
#define CAP_RCTL            0x1C        // CapPtr + Root Control
#endif
#ifndef CAP_RSTS
#define CAP_RSTS            0x20        // CapPtr + Root Status
#endif
#ifndef PCI_STS_ERROR
#define PCI_STS_ERROR       0x07        // PCI Error Status for PCI Status Register
#endif
#ifndef PCI_SECSTATUS_ERROR
#define PCI_SECSTATUS_ERROR 0x1F        // PCI Error Status of Secondary Status Register
#endif

//-----------------------------------------------------------------------
// NB : PEG information HOB
//-----------------------------------------------------------------------
#ifndef __PI_HOB_H__
#ifndef _PEI_HOB_H_
#ifndef __HOB__H__
typedef struct _EFI_HOB_GENERIC_HEADER {
  UINT16  HobType;
  UINT16  HobLength;
  UINT32  Reserved;
} EFI_HOB_GENERIC_HEADER;

typedef struct _EFI_HOB_GUID_TYPE {
  EFI_HOB_GENERIC_HEADER  Header;
  EFI_GUID                Name;
} EFI_HOB_GUID_TYPE;
#endif
#endif
#endif

typedef struct {
  EFI_HOB_GUID_TYPE Header;
  UINT8             PegDeOverride[3];
} NB_PEG_INFO_HOB;

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
