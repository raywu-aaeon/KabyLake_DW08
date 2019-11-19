/** @file
  Source code for the others board configuration init function in DXE init phase.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2016 - 2019 Intel Corporation.

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

  This file contains a 'Sample Driver' and is licensed as such under the terms
  of your license agreement with Intel or your vendor. This file may be modified
  by the user, subject to the additional terms of the license agreement.

@par Specification Reference:
**/

#include "OthersBoardInitLib.h"

#include <Library/HobLib.h>
#include <MemInfoHob.h>

#include <Library/PchSerialIoLib.h>
#include <PlatformBoardConfig.h>
#include <GpioPinsSklLp.h>
#include <GpioPinsSklH.h>

#include <Protocol/GlobalNvsArea.h>
#include <OemSetup.h>

#include <Features/Smbios/SystemSlotInfoDxe/SystemSlotInfoDxe.h>

//
// Others Board Init Table
//
BOARD_INIT_TABLE mOthersBoardInit [] = {
  {COMMON_BOARD_ID, OthersBoardInit},
  { 0, 0 }
};

extern EFI_GUID gSysFwSklSdsGuid;
extern EFI_GUID gSysFwRvp8Guid;
extern EFI_GUID gSysFwZumbaBeachGuid;

//SKL PCH LP GPIO Expander Number
#define IO_EXPANDER_0            0
#define IO_EXPANDER_1            1

//SKL PCH LP GPIO Pin Mapping
#define IO_EXPANDER_GPIO_5        5   // P05
#define IO_EXPANDER_GPIO_11       11  // P13

CHAR16 *BoardID_Index[] = {
  L"Skylake X0 LPDDR3 RVP1 ERB",             // 0x00
  L"Skylake X0 LPDDR3 RVP1",                 // 0x01
  L"Skylake X0 DDR4 RVP2",                   // 0x02
  L"Kabylake KC LPDDR3 RVP",                 // 0x03
  L"Skylake Y LPDDR3 RVP3",                  // 0x04
  L"Skylake Y LPDDR3 RVP13 SIP",             // 0x05
  L"Skylake Halo RVP11 uSFF",                // 0x06
  L"Kabylake R DDR4 RVP",                    // 0x07
  L"Skylake U DDR4 LPDIMM DOE",              // 0x08
  L"Skylake U LPDDR3 Starbrook",             // 0x09
  L"Skylake U LPDDR3 RVP5",                  // 0x0A
  L"Skylake U DDR3L RVP7",                   // 0x0B
  L"Skylake U DDR4 RVP15 SIP",               // 0x0C
  L"Skylake U LPDDR3 Doe",                   // 0x0D
  L"Cannonlake X0 LPDDR3 RVP5",              // 0x0E
  L"TBD",                                    // 0x0F
  L"TBD",                                    // 0x10
  L"TBD",                                    // 0x11
  L"TBD",                                    // 0x12
  L"Skylake U LPDDR3 RVP3 PPV",              // 0x13
  L"Skylake Y LPDDR3 RVP3 PPV",              // 0x14
  L"TBD",                                    // 0x15
  L"TBD",                                    // 0x16
  L"TBD",                                    // 0x17
  L"TBD",                                    // 0x18
  L"TBD",                                    // 0x19
  L"TBD",                                    // 0x1A
  L"Skylake U DDR3L RVP7 PPV",               // 0x1B
  L"TBD",                                    // 0x1C
  L"TBD",                                    // 0x1D
  L"TBD",                                    // 0x1E
  L"TBD",                                    // 0x1F
  L"Skylake SDS",                            // 0x20
  L"Skylake Panther Mtn",                    // 0x21
  L"TBD",                                    // 0x22
  L"Skylake Phablet Poc",                    // 0x23
  L"TBD",                                    // 0x24
  L"TBD",                                    // 0x25
  L"TBD",                                    // 0x26
  L"TBD",                                    // 0x27
  L"TBD",                                    // 0x28
  L"TBD",                                    // 0x29
  L"TBD",                                    // 0x2A
  L"TBD",                                    // 0x2B
  L"TBD",                                    // 0x2C
  L"TBD",                                    // 0x2D
  L"TBD",                                    // 0x2E
  L"TBD",                                    // 0x2F
  L"TBD",                                    // 0x30
  L"TBD",                                    // 0x31
  L"TBD",                                    // 0x32
  L"TBD",                                    // 0x33
  L"TBD",                                    // 0x34
  L"TBD",                                    // 0x35
  L"TBD",                                    // 0x36
  L"TBD",                                    // 0x37
  L"TBD",                                    // 0x38
  L"TBD",                                    // 0x39
  L"TBD",                                    // 0x3A
  L"TBD",                                    // 0x3B
  L"TBD",                                    // 0x3C
  L"TBD",                                    // 0x3D
  L"TBD",                                    // 0x3E
  L"TBD",                                    // 0x3F
  L"TBD",                                    // 0x40
  L"Skylake AIO DDR4 RVP9",                  // 0x41
  L"Skylake DT DDR4 RVP8",                   // 0x42
  L"Skylake AIO DDR3L RVP10",                // 0x43
  L"Skylake Halo DDR4 RVP11",                // 0x44
  L"Skylake Halo DDR4 RVP11 PPV",            // 0x45
  L"Skylake AIO DDR3L RVP10 PPV",            // 0x46
  L"TBD",                                    // 0x47
  L"Skylake Halo LPDDR3 RVP16",              // 0x48
  L"Skylake SLPF",                           // 0x49
  L"TBD",                                    // 0x4A
  L"TBD",                                    // 0x4B
  L"TBD",                                    // 0x4C
  L"TBD",                                    // 0x4D
  L"TBD",                                    // 0x4E
  L"TBD",                                    // 0x4F
  L"Kabylake Embedded SaddleBrook",          // 0x50
  L"TBD",                                    // 0x51
  L"TBD",                                    // 0x52
  L"TBD",                                    // 0x53
  L"TBD",                                    // 0x54
  L"TBD",                                    // 0x55
  L"TBD",                                    // 0x56
  L"TBD",                                    // 0x57
  L"TBD",                                    // 0x58
  L"TBD",                                    // 0x59
  L"TBD",                                    // 0x5A
  L"TBD",                                    // 0x5B
  L"TBD",                                    // 0x5C
  L"TBD",                                    // 0x5D
  L"TBD",                                    // 0x5E
  L"TBD",                                    // 0x5F
  L"Kabylake Y LPDDR3 RVP3",                 // 0x60
  L"Kabylake Grizzly Mtn",                   // 0x61
  L"Coyote Mountain IDV",                    // 0x62
  L"Kabylake U LPDDR3 DOE1",                 // 0x63
  L"Kabylake U DDR3L RVP7",                  // 0x64
  L"Kabylake U LPDDR3 DOE",                  // 0x65
  L"KBL S DDR4 UDIMM EV ERB",                // 0x66
  L"KBL S DDR4 UDIMM EV CRB",                // 0x67
  L"KBL S UDIMM OC RVP",                     // 0x68
  L"KBL S DDR4 UDIMM CRB",                   // 0x69
  L"Kabylake S UDIMM CPV",                   // 0x6A
  L"BasinFalls EV",                          // 0x6B
  L"BasinFalls CRB",                         // 0x6C
  L"BasinFalls PPV",                         // 0x6D
  L"BasinFalls Rsvd",                        // 0x6E
  L"TBD",                                    // 0x6F
  L"TBD",                                    // 0x70
  L"TBD",                                    // 0x71
  L"TBD",                                    // 0x72
  L"Zumba Beach Server EV",                  // 0x73
  L"Zumba Beach Server CRB",                 // 0x74
  L"TBD",                                    // 0x75
  L"TBD",                                    // 0x76
  L"TBD",                                    // 0x77
  L"TBD",                                    // 0x78
  L"TBD",                                    // 0x79
  L"TBD",                                    // 0x7A
  L"TBD",                                    // 0x7B
  L"TBD",                                    // 0x7C
  L"TBD",                                    // 0x7D
  L"TBD",                                    // 0x7E
  L"TBD",                                    // 0x7F
  L"TBD",                                    // 0x80
  L"AmberLake Y 42 LPDDR3 RVP3",             // 0x81
  L"AmberLake Y LPDDR3 RVP3",                // 0x82
  L"AmberLake Y LPDDR3 RVP13",               // 0x83
  L"AmberLake Y 42 LPDDR3 CFF",              // 0x84
  L"AmberLake Y 42 LPDDR3 CFF CC",           // 0x85
  L"TBD"                                     // 0x86
};

//UINT16 mSlotEntryTableIndex [] = {BoardIdKabylakeUDdr3lRvp7, BoardIdKabyLakeYLpddr3Rvp3};

//
// Slot Entry Table for all the Supported boards
//
SYSTEM_SLOT_ENTRY mKabylakeUDdr3lRvp7SlotEntryTable =
    //Board 1
    { TRUE,
      0x08, // Number of Entries
      {
            // Slot Desig         SlotType       BusWidth               CurrentUsage SlotLength       SlotID   Char1  Char 2                          Grpnu Bus  Dev   Fun   Isvalid
        { L"PCI-Express 0", SlotTypePciExpress, SlotDataBusWidth4X, SlotUsageAvailable, SlotLengthLong, 1, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, 0, PCIE_BUS, PCIE_DEV_1, PCIE_FUN_1, TRUE },
        { L"PCI-Express 4", SlotTypePciExpress, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 2, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, 0, PCIE_BUS, PCIE_DEV_1, PCIE_FUN_5, TRUE },
        { L"PCI-Express 5", SlotTypePciExpress, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 3, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, 0, PCIE_BUS, PCIE_DEV_1, PCIE_FUN_6, TRUE },
        { L"PCI-Express 8", SlotTypePciExpress, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 4, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, 0, PCIE_BUS, PCIE_DEV_2, PCIE_FUN_1, TRUE },
        { L"M.2 SSD      ", SlotTypeM2Socket3   , SlotDataBusWidth4X, SlotUsageAvailable, SlotLengthLong, 0, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, TRUE },  // Bus Numbers are not fixed (changes from BOM to BOM)
        { L"M.2 PCIe WIFI", SlotTypeM2Socket1_SD, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 0, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, TRUE }, // Programmed as 0xFF , Instead of Populating Incoorect data
        { L"M.2 WIGIG    ", SlotTypeM2Socket1_DP, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 0, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, TRUE },
        { L"M.2 WLAN     ", SlotTypeM2Socket2   , SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 0, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, TRUE },
      },
    };

SYSTEM_SLOT_ENTRY mKabyLakeYLpddr3Rvp3SlotEntryTable =
    //Board 2
    { TRUE,
      0x0A, // Number of Entries
      {
            // Slot Desig         SlotType       BusWidth               CurrentUsage SlotLength       SlotID   Char1  Char 2                          Grpnu Bus  Dev   Fun   Isvalid
        { L"PCI-Express 0", SlotTypePciExpress, SlotDataBusWidth4X, SlotUsageAvailable, SlotLengthLong, 1, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, 0, PCIE_BUS, PCIE_DEV_1, PCIE_FUN_1, TRUE },
        { L"PCI-Express 5", SlotTypePciExpress, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 2, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, 0, PCIE_BUS, PCIE_DEV_1, PCIE_FUN_6, TRUE },
        { L"PCI-Express 8", SlotTypePciExpress, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 3, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, 0, PCIE_BUS, PCIE_DEV_2, PCIE_FUN_1, TRUE },
        { L"PCI-Express 9", SlotTypePciExpress, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 4, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, 0, PCIE_BUS, PCIE_DEV_2, PCIE_FUN_2, TRUE },
        { L"PCI-Express A", SlotTypePciExpress, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 5, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, 0, PCIE_BUS, PCIE_DEV_2, PCIE_FUN_3, TRUE },
        { L"PCI-Express B", SlotTypePciExpress, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 6, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, 0, PCIE_BUS, PCIE_DEV_2, PCIE_FUN_4, TRUE },
        { L"M.2 SSD      ", SlotTypeM2Socket3, SlotDataBusWidth4X, SlotUsageAvailable, SlotLengthLong, 0, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, TRUE },   // Bus Numbers are not fixed (changes from BOM to BOM)
        { L"M.2 PCIe WIFI", SlotTypeM2Socket1_SD, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 0, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, TRUE }, // Programmed as 0xFF , Instead of Populating Incoorect data
        { L"M.2 WIGIG    ", SlotTypeM2Socket1_DP, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 0, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, TRUE },
        { L"M.2 WLAN     ", SlotTypeM2Socket2, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 0, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, TRUE },

      },
    };

SYSTEM_SLOT_ENTRY mKabyLakeSDdr4UdimmEvCrbSlotEntryTable =
    //Board 3
    { TRUE,
      0x07, // Number of Entries
      {
            // Slot Desig          SlotType       BusWidth               CurrentUsage SlotLength       SlotID   Char1  Char 2                          Grpnu Bus  Dev   Fun   Isvalid
        { L"PCI-Express 6 ", SlotTypePciExpress, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 1, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, 0, PCIE_BUS, PCIE_DEV_1, PCIE_FUN_7, TRUE },
        { L"PCI-Express 7 ", SlotTypePciExpress, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 2, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, 0, PCIE_BUS, PCIE_DEV_1, PCIE_FUN_8, TRUE },
        { L"PCI-Express 8 ", SlotTypePciExpress, SlotDataBusWidth4X, SlotUsageAvailable, SlotLengthLong, 3, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, 0, PCIE_BUS, PCIE_DEV_2, PCIE_FUN_1, TRUE },
        { L"PCI-Express 14", SlotTypePciExpress, SlotDataBusWidth4X, SlotUsageAvailable, SlotLengthLong, 4, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, 0, PCIE_BUS, PCIE_DEV_3, PCIE_FUN_5, TRUE },
        { L"M.2 SSD       ", SlotTypeM2Socket3, SlotDataBusWidth4X, SlotUsageAvailable, SlotLengthLong, 0, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, TRUE },   // Bus Numbers are not fixed (changes from BOM to BOM)
        { L"M.2 PCIe WIFI ", SlotTypeM2Socket1_DP, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 0, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, TRUE }, // Programmed as 0xFF , Instead of Populating Incoorect data
        { L"M.2 WIGIG     ", SlotTypeM2Socket1_SD, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 0, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, TRUE },
      },
    };

SYSTEM_SLOT_ENTRY mKabyLakeSDdr4UdimmCrbSlotEntryTable =
    //Board 4
    { TRUE,
      0x06, // Number of Entries
      {
            // Slot Desig          SlotType       BusWidth               CurrentUsage SlotLength       SlotID   Char1  Char 2                          Grpnu Bus  Dev   Fun   Isvalid
        { L"PCI-Express 6 ", SlotTypePciExpress, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 1, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, 0, PCIE_BUS, PCIE_DEV_1, PCIE_FUN_7, TRUE },
        { L"PCI-Express 7 ", SlotTypePciExpress, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 2, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, 0, PCIE_BUS, PCIE_DEV_1, PCIE_FUN_8, TRUE },
        { L"PCI-Express 8 ", SlotTypePciExpress, SlotDataBusWidth4X, SlotUsageAvailable, SlotLengthLong, 3, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, 0, PCIE_BUS, PCIE_DEV_2, PCIE_FUN_1, TRUE },
        { L"PCI-Express 14", SlotTypePciExpress, SlotDataBusWidth4X, SlotUsageAvailable, SlotLengthLong, 4, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, 0, PCIE_BUS, PCIE_DEV_3, PCIE_FUN_5, TRUE },
        { L"M.2 SSD       ", SlotTypeM2Socket3, SlotDataBusWidth4X, SlotUsageAvailable, SlotLengthLong, 0, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, TRUE },   // Bus Numbers are not fixed (changes from BOM to BOM)
        { L"M.2 PCIe WIFI ", SlotTypeM2Socket1_DP, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 0, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, TRUE }, // Programmed as 0xFF , Instead of Populating Incoorect data
      },
    };

SYSTEM_SLOT_ENTRY mKabylakeRSlotEntryTable =
    //Board 5
    { TRUE,
      0x06, // Number of Entries
      {
            // Slot Desig          SlotType       BusWidth               CurrentUsage SlotLength       SlotID   Char1  Char 2                          Grpnu Bus  Dev   Fun   Isvalid
        { L"PCI-Express 0 ", SlotTypePciExpress, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 1, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, 0, PCIE_BUS, PCIE_DEV_1, PCIE_FUN_1, TRUE },
        { L"PCI-Express 8 ", SlotTypePciExpress, SlotDataBusWidth4X, SlotUsageAvailable, SlotLengthLong, 2, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, 0, PCIE_BUS, PCIE_DEV_2, PCIE_FUN_1, TRUE },
        { L"M.2 SSD ..... ", SlotTypeM2Socket3, SlotDataBusWidth4X, SlotUsageAvailable, SlotLengthLong, 0, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, TRUE },    // Bus Numbers are not fixed (changes from BOM to BOM)
        { L"M.2 PCIe WIFI ", SlotTypeM2Socket1_SD, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 0, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, TRUE }, // Programmed as 0xFF , Instead of Populating Incoorect data
        { L"M.2 WIGIG     ", SlotTypeM2Socket1_DP, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 0, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, TRUE },
        { L"M.2 WWAN      ", SlotTypeM2Socket2, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 0, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, TRUE },
      },
    };

SYSTEM_SLOT_ENTRY mSkylakeHaloDdr4Rvp11SlotEntryTable =
    //Board 6
    { TRUE,
      0x07, // Number of Entries
      {
            // Slot Desig         SlotType       BusWidth               CurrentUsage SlotLength       SlotID   Char1  Char 2                          Grpnu Bus  Dev   Fun   Isvalid
        { L"PCI-Express 0", SlotTypePciExpress, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 1, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, 0, PCIE_BUS, PCIE_DEV_1, PCIE_FUN_1, TRUE },
        { L"PCI-Express 8", SlotTypePciExpress, SlotDataBusWidth4X, SlotUsageAvailable, SlotLengthLong, 2, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, 0, PCIE_BUS, PCIE_DEV_2, PCIE_FUN_1, TRUE },
        { L"M.2 SSD 1    ", SlotTypeM2Socket3   , SlotDataBusWidth4X, SlotUsageAvailable, SlotLengthLong, 0, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, TRUE },  // Bus Numbers are not fixed (changes from BOM to BOM)
        { L"M.2 SSD 2    ", SlotTypeM2Socket3   , SlotDataBusWidth4X, SlotUsageAvailable, SlotLengthLong, 0, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, TRUE },  // Programmed as 0xFF , Instead of Populating Incoorect data
        { L"M.2 PCIe WLAN", SlotTypeM2Socket1_DP, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 0, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, TRUE },
        { L"M.2 WIGIG    ", SlotTypeM2Socket1_SD, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 0, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, TRUE },
        { L"M.2 WWAN     ", SlotTypeM2Socket2   , SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 0, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, PCIE_DEFAULT, TRUE },
      },
    };

//
// TODO: Added for Skylake Backward Compatibility, Need to remove in Cannon Lake
//
SYSTEM_SLOT_ENTRY mDefaultSlotEntryTable =
    { FALSE,
      0x07, // Number of Entries
      {
        // Slot Desig         SlotType               BusWidth              CurrentUsage       SlotLength     SlotID   Char1                  Char 2      SegGrpnu Bus Device      Fun          Isvalid
        { L"PCI-Express 0", SlotTypePciExpressX16, SlotDataBusWidth16X, SlotUsageAvailable, SlotLengthLong, 0, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, 0, PCIE_BUS, PCIE_DEV_1, PCIE_FUN_1, TRUE },
        { L"PCI-Express 1", SlotTypePciExpress, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 1, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, 0, PCIE_BUS, PCIE_DEV_1, PCIE_FUN_2, TRUE },
        { L"PCI-Express 2", SlotTypePciExpress, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 2, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, 0, PCIE_BUS, PCIE_DEV_1, PCIE_FUN_3, TRUE },
        { L"PCI-Express 3", SlotTypePciExpress, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 3, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, 0, PCIE_BUS, PCIE_DEV_1, PCIE_FUN_4, TRUE },
        { L"PCI-Express 4", SlotTypePciExpress, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 4, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, 0, PCIE_BUS, PCIE_DEV_1, PCIE_FUN_5, TRUE },
        { L"PCI-Express 5", SlotTypePciExpress, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 5, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, 0, PCIE_BUS, PCIE_DEV_1, PCIE_FUN_6, TRUE },
        { L"PCI-Express 6", SlotTypePciExpress, SlotDataBusWidth1X, SlotUsageAvailable, SlotLengthLong, 6, { 0, 0, 1, 0, 0, 0, 0, 0 }, { 1, 1, 1, 0 }, 0, PCIE_BUS, PCIE_DEV_1, PCIE_FUN_7, TRUE },
      }
    };

EFI_STATUS
OthersBoardSmbiosInit (
  IN PLATFORM_INFO *PlatformInfo,
  IN UINT16 BoardId,
  IN UINT16 BoardIdOrgValue
  )
{
  EFI_STATUS Status;

  Status = EFI_SUCCESS;

  if (BoardId == BoardIdUnknown1) {
    PcdSet64S (PcdSmbiosFabBoardName, 0);
  } else {
    PcdSet64S (PcdSmbiosFabBoardName, (UINTN) BoardID_Index [BoardIdOrgValue]);
  }

  switch (BoardIdOrgValue) {
    case BoardIdKabylakeUDdr3lRvp7:
      PcdSet64S (PcdSmbiosMainSlotEntry, (UINTN) &mKabylakeUDdr3lRvp7SlotEntryTable);
      break;
    case BoardIdKabyLakeYLpddr3Rvp3:
    case BoardIdAmberLakeLpddr3Rvp3:
    case BoardIdAmberLakeY42Lpddr3Rvp3:
      PcdSet64S (PcdSmbiosMainSlotEntry, (UINTN) &mKabyLakeYLpddr3Rvp3SlotEntryTable);
      break;
    case BoardIdAmberLakeY42Lpddr3Cff:
    case BoardIdAmberLakeY42Lpddr3CffCc:
      PcdSet64S (PcdSmbiosMainSlotEntry, (UINTN) &mKabyLakeYLpddr3Rvp3SlotEntryTable);
      break;
    case BoardIdKabyLakeSDdr4UdimmEvCrb:
      PcdSet64S (PcdSmbiosMainSlotEntry, (UINTN) &mKabyLakeSDdr4UdimmEvCrbSlotEntryTable);
      break;
    case BoardIdKabyLakeSDdr4UdimmCrb:
      PcdSet64S (PcdSmbiosMainSlotEntry, (UINTN) &mKabyLakeSDdr4UdimmCrbSlotEntryTable);
      break;
    case BoardIdKabylakeRDdr4:
    case BoardIdKabylakeRLpddr3:
      PcdSet64S (PcdSmbiosMainSlotEntry, (UINTN) &mKabylakeRSlotEntryTable);
      break;
    case BoardIdSkylakeHaloDdr4Rvp11:
      PcdSet64S (PcdSmbiosMainSlotEntry, (UINTN) &mSkylakeHaloDdr4Rvp11SlotEntryTable);
      break;
    default:
      PcdSet64S (PcdSmbiosMainSlotEntry, (UINTN) &mDefaultSlotEntryTable);
      break;
  }

  return Status;
}



EFI_STATUS
BoardHookPlatformSetup (
  VOID * Content
  )
{
  EFI_STATUS Status;
  UINT16 BoardId;
  UINT16 BoardIdOrgValue;
  SETUP_VOLATILE_DATA     *SetupVolatileData;

  SetupVolatileData = (SETUP_VOLATILE_DATA *) Content;

  Status = GetBoardId (&BoardId, &BoardIdOrgValue);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    SetupVolatileData->PlatId = BoardIdUnknown1;
  } else {
    SetupVolatileData->PlatId = BoardId;
    SetupVolatileData->OrgPlatId = BoardIdOrgValue;
  }

  return Status;
}


/**
  Updates DIMM slots status for Desktop, Server, Workstation.

**/
VOID
UpdateDimmPopulationConfig(
  PLATFORM_INFO                *PlatformInfo
  )
{
  MEMORY_INFO_DATA_HOB    *MemInfo;
  UINT8                   Slot0;
  UINT8                   Slot1;
  UINT8                   Slot2;
  UINT8                   Slot3;
  CONTROLLER_INFO         *ControllerInfo;
  EFI_HOB_GUID_TYPE       *GuidHob;

  GuidHob = NULL;
  MemInfo = NULL;

  GuidHob = GetFirstGuidHob (&gSiMemoryInfoDataGuid);
  ASSERT (GuidHob != NULL);
  if (GuidHob != NULL) {
    MemInfo = (MEMORY_INFO_DATA_HOB *) GET_GUID_HOB_DATA (GuidHob);
  }
  if (MemInfo) {
    if ( PlatformInfo->PlatformFlavor == FlavorDesktop ||
         PlatformInfo->PlatformFlavor == FlavorUpServer ||
         PlatformInfo->PlatformFlavor == FlavorWorkstation
        ) {
      ControllerInfo = &MemInfo->Controller[0];
      Slot0 = ControllerInfo->ChannelInfo[0].DimmInfo[0].Status;
      Slot1 = ControllerInfo->ChannelInfo[0].DimmInfo[1].Status;
      Slot2 = ControllerInfo->ChannelInfo[1].DimmInfo[0].Status;
      Slot3 = ControllerInfo->ChannelInfo[1].DimmInfo[1].Status;

      //
      // Channel 0          Channel 1
      // Slot0      Slot1      Slot0      Slot1      - Population     - AIO board
      // 0          0          0          0          - Invalid        - Invalid
      // 0          0          0          1          - Valid          - Invalid
      // 0          0          1          0          - Invalid        - Valid
      // 0          0          1          1          - Valid          - Valid
      // 0          1          0          0          - Valid          - Invalid
      // 0          1          0          1          - Valid          - Invalid
      // 0          1          1          0          - Invalid        - Invalid
      // 0          1          1          1          - Valid          - Invalid
      // 1          0          0          0          - Invalid        - Valid
      // 1          0          0          1          - Invalid        - Invalid
      // 1          0          1          0          - Invalid        - Valid
      // 1          0          1          1          - Invalid        - Valid
      // 1          1          0          0          - Valid          - Valid
      // 1          1          0          1          - Valid          - Invalid
      // 1          1          1          0          - Invalid        - Valid
      // 1          1          1          1          - Valid          - Valid
      //

      if ((Slot0 && (Slot1 == 0)) || (Slot2 && (Slot3 == 0))) {
        PcdSetBoolS (PcdDimmPopulationError, TRUE);
      }
    }
  }
}

/**
  Init Misc Platform Board Config Block.

  @param[in]  PlatformInfo      a pointer to Platform Info.

  @retval     EFI_SUCCESS       The function completed successfully.
**/
EFI_STATUS
OthersBoardMiscInit (
  IN PLATFORM_INFO *PlatformInfo,
  IN UINT16 BoardId,
  IN UINT16 BoardIdOrgValue
)
{
  PcdSet64S (PcdFuncBoardHookPlatformSetupOverride, (UINT64) (UINTN) BoardHookPlatformSetup);

  //
  // Set USB Type C support PCD
  //
  if((BoardIdOrgValue == BoardIdKabylakeRDdr4) ||
     (BoardIdOrgValue == BoardIdKabylakeRLpddr3) ||
     (BoardIdOrgValue == BoardIdKabyLakeSDdr4UdimmEvErb) ||
     (BoardIdOrgValue == BoardIdKabyLakeSDdr4UdimmEvCrb) ||
     (BoardIdOrgValue == BoardIdKabyLakeSDdr4UdimmCrb) ||
     (BoardIdOrgValue == BoardIdKabyLakeYLpddr3Rvp3) ||
     (BoardIdOrgValue == BoardIdAmberLakeLpddr3Rvp3) ||
     (BoardIdOrgValue == BoardIdAmberLakeLpddr3Rvp13) ||
     (BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Rvp3) ||
     (BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Cff) ||
     (BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3CffCc) ||
     (BoardId == BoardIdSkylakeHaloLpddr3Rvp16)) {
    PcdSetBoolS (PcdUsbTypeCSupport, TRUE);
  }

  if((BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Rvp3)) {
    PcdSet64S (PcdUsbTypeCAcpiTableSignature, SIGNATURE_64('U','s','b','C','R','V','P','3'));
    } else {
    PcdSet64S (PcdUsbTypeCAcpiTableSignature, SIGNATURE_64('U','s','b','C','T','a','b','l'));
    }

  //
  // Updating USB Type C Multi Port Setting Only For Kabylake R DDR4 board.
  //
  if((BoardIdOrgValue == BoardIdKabylakeRDdr4) ||
    (BoardIdOrgValue == BoardIdKabylakeRLpddr3)) {
    PcdSetBoolS (PcdUsbTypeCMultiPortSupport, TRUE);
  }

  //
  // Updating Channel Slot Map For SMBIOS Table Type 16 only for Kabylake S U Dimm board.
  //
  if (BoardIdOrgValue == BoardIdKabyLakeSDdr4UdimmCrb) {
    PcdSet8S(PcdChannelASlotMap, 0x03);
    PcdSet8S(PcdChannelBSlotMap, 0x03);
  }

  UpdateDimmPopulationConfig (PlatformInfo);

//
// AdvancedFeaturesBegin
//

  //
  // S3 Wake Capability LID Switch GPE0 Number. On KBL RVP board LAN wake and
  // LID Switch wake are muxed together. So for LID Switch wake to work,
  // R_PCH_ACPI_GPE0_EN_127_96 register BIT16 (LAN Wake) should be enabled.
  // Which translate to 0x70 GPE number. So passing 0x70 in PcdLidSwitchGpe0Number.
  //
  PcdSet8S(PcdLidSwitchGpe0Number, 0x70);
//
// AdvancedFeaturesEnd
//

  return EFI_SUCCESS;
}

/**
  Init Platform Board Config Block for ACPI platform.

  @param[in]  PlatformInfo      a pointer to Platform Info.

  @retval     EFI_SUCCESS       The function completed successfully.
**/
EFI_STATUS
InitAcpiPlatformConfigBlock (
  IN PLATFORM_INFO *PlatformInfo,
  IN UINT16        BoardId,
  IN UINT16        BoardIdOrgValue
)
{
  PCD64_BLOB Pcd64;

  //
  // Update OEM table ID
  //
  switch (BoardId) {
    case BoardIdSkylakeDtRvp8Crb:
      if ((BoardIdOrgValue == BoardIdKabyLakeSDdr4UdimmEvErb) ||
          (BoardIdOrgValue == BoardIdKabyLakeSDdr4UdimmEvCrb)) {
        DEBUG ((DEBUG_INFO, "Acpi for KBL S DDR4 uDIMM EV Crb or EV Erb Usb Ports\n"));
        PcdSet64S (PcdXhciAcpiTableSignature, SIGNATURE_64 ('x', 'h', '_', 'k', 'b', 'l', 's', 'e'));
      } else if (BoardIdOrgValue == BoardIdKabyLakeSDdr4UdimmCrb) {
        DEBUG ((DEBUG_INFO, "Acpi for KBL S DDR4 uDIMM Crb Usb Ports\n"));
        PcdSet64S (PcdXhciAcpiTableSignature, SIGNATURE_64 ('x', 'h', '_', 'k', 'b', 'l', '_', 's'));
      } else {
        DEBUG ((DEBUG_INFO, "Acpi for Rvp8 Usb Ports\n"));
        PcdSet64S (PcdXhciAcpiTableSignature, SIGNATURE_64 ('x', 'h', '_', 'r', 'v', 'p', '0', '8'));
      }
      break;

    case BoardIdSkylakeAioRvp10Crb:
    case BoardIdSkylakeAioRvp10CrbPpv:
    case BoardIdSkylakeAioRvp10Evp:
    case BoardIdSkylakeSlpfPpv:
      DEBUG ((DEBUG_INFO, "Acpi for Rvp10 Usb Ports\n"));
      PcdSet64S (PcdXhciAcpiTableSignature, SIGNATURE_64 ('x', 'h', '_', 'r', 'v', 'p', '1', '0'));
      break;

    case BoardIdSkylakeSds:
    case BoardIdSkylakePhabletPoc:
      DEBUG ((DEBUG_INFO, "Acpi for SDS Usb Ports\n"));
      PcdSet64S (PcdXhciAcpiTableSignature, SIGNATURE_64 ('x', 'h', '_', 's', 'd', 's', '0', '0'));
      break;

    default:
#if 0  // AMI_OVERRIDE_START - Report Acpi for other board case
      PcdSet64S (PcdXhciAcpiTableSignature, 0x0);
#else
      DEBUG ((DEBUG_INFO, "Acpi for OEM Usb Ports\n"));
      PcdSet64S (PcdXhciAcpiTableSignature, SIGNATURE_64 ('x', 'h', '_', 'O', 'E', 'M', 'B', 'D'));
#endif // AMI_OVERRIDE_END - Report Acpi for other board case
      break;
  }

  //
  // Modify Preferred_PM_Profile field based on Board SKU's. Default is set to Mobile
  //
  if((BoardId == BoardIdSkylakeDtRvp8Crb) ||
     (BoardId == BoardIdSkylakeAioRvp9Crb) ||
     (BoardId == BoardIdSkylakeAioRvp10Erb) ||
     (BoardId == BoardIdSkylakeAioRvp10Crb) ||
     (BoardId == BoardIdSkylakeAioRvp10Evp) ||
     (BoardId == BoardIdSkylakeAioRvp10CrbPpv)) {
    PcdSet8S (PcdPreferredPmProfile, EFI_ACPI_2_0_PM_PROFILE_DESKTOP);
  } else {
    PcdSet8S (PcdPreferredPmProfile, EFI_ACPI_2_0_PM_PROFILE_MOBILE);
  }
  if (PlatformInfo->PlatformFlavor == FlavorUpServer) {
    PcdSet8S (PcdPreferredPmProfile, EFI_ACPI_2_0_PM_PROFILE_ENTERPRISE_SERVER);
  }

  //
  // TherSds or TherRvp table enable.
  //
  if ((BoardId == BoardIdSkylakeSds)         ||
      (BoardId == BoardIdSkylakePhabletPoc)  ||
      (BoardId == BoardIdSkylakePantherMtn)  ||
      (BoardId == BoardIdSkylakeStarbrook)   ||
      (BoardId == BoardIdKabylakeGrizzlyMtn) ||
      (BoardId == BoardIdKabylakeCoyoteMtn)) {
    PcdSetBoolS (PcdTherSdsTableEnable, TRUE);
  } else {
    PcdSetBoolS (PcdTherSdsTableEnable, FALSE);
  }

  if ((BoardId == BoardIdSkylakeULpddr3Rvp5)    ||
      (BoardId == BoardIdSkylakeULpddr3Rvp5Ppv) ||
      (BoardId == BoardIdSkylakeLpdimmDoe)) {
    PcdSet8S (PcdSpecificIoExpanderBus, PchSerialIoIndexI2C4);
  } else {
    PcdSet8S (PcdSpecificIoExpanderBus, IO_EXPANDER_DISABLED);
  }

  //
  // Assign FingerPrint, Gnss, BLuetooth & TouchPanel relative GPIO.
  //
  switch(BoardId) {
    case BoardIdSkylakeAioRvp10Crb:
    case BoardIdSkylakeAioRvp10CrbPpv:
    case BoardIdSkylakeAioRvp10Evp:
    case BoardIdSkylakeAioRvp10Erb:
    case BoardIdSkylakeSdlBrk:
    case BoardIdSkylakeSlpfPpv:
      PcdSet32S (PcdFingerPrintSleepGpio, GPIO_SKL_H_GPP_A22);
      PcdSet32S (PcdFingerPrintIrqGpio  , GPIO_SKL_H_GPP_A23);
      PcdSet32S (PcdGnssResetGpio       , GPIO_SKL_H_GPP_A0);//not connected
      PcdSet32S (PcdBluetoothWakeGpio   , GPIO_SKL_H_GPP_A0);//not connected
      PcdSet32S (PcdBluetoothRfKillGpio , GPIO_SKL_H_GPP_B3);
      PcdSet32S (PcdBluetoothIrqGpio    , GPIO_SKL_H_GPP_B5);
      PcdSet32S (PcdTouchpadIrqGpio     , GPIO_SKL_H_GPP_F9);
      PcdSet32S (PcdTouchpanelIrqGpio   , GPIO_SKL_H_GPP_E7);
      break;

    case BoardIdSkylakeSds:
    case BoardIdSkylakePantherMtn: // @todo: Confirm Panther Mountain GPIO configuration.
    case BoardIdSkylakeStarbrook:
      PcdSet32S (PcdFingerPrintSleepGpio, GPIO_SKL_LP_GPP_A22);//not connected
      PcdSet32S (PcdFingerPrintIrqGpio  , GPIO_SKL_LP_GPP_A23);
      PcdSet32S (PcdGnssResetGpio       , GPIO_SKL_LP_GPP_A12);
      PcdSet32S (PcdBluetoothWakeGpio   , GPIO_SKL_LP_GPP_C2);//not sure if connected?
      PcdSet32S (PcdBluetoothRfKillGpio , GPIO_SKL_LP_GPP_B4);
      PcdSet32S (PcdBluetoothIrqGpio    , GPIO_SKL_LP_GPP_B5);
      PcdSet32S (PcdTouchpadIrqGpio     , GPIO_SKL_LP_GPP_B3);//not connected
      PcdSet32S (PcdTouchpanelIrqGpio   , GPIO_SKL_LP_GPP_E7);
      break;

    case BoardIdSkylakePhabletPoc: //GPIO assignements for PhabletPoc
      PcdSet32S (PcdFingerPrintSleepGpio, GPIO_SKL_LP_GPP_A22);//not connected in Phablet
      PcdSet32S (PcdFingerPrintIrqGpio  , GPIO_SKL_LP_GPP_A23);//not connected in Phablet
      PcdSet32S (PcdGnssResetGpio       , GPIO_SKL_LP_GPP_G0);//GNSS_RESET_N
      PcdSet32S (PcdBluetoothWakeGpio   , GPIO_SKL_LP_GPP_C2);//not connected in Phablet
      PcdSet32S (PcdBluetoothRfKillGpio , GPIO_SKL_LP_GPP_B4);//not connected in Phablet
      PcdSet32S (PcdBluetoothIrqGpio    , GPIO_SKL_LP_GPP_B5);//not connected in Phablet
      PcdSet32S (PcdTouchpadIrqGpio     , GPIO_SKL_LP_GPP_B3);//not connected
      PcdSet32S (PcdTouchpanelIrqGpio   , GPIO_SKL_LP_GPP_E7);//TOUCH_PANEL_INTR_N
      break;

    case BoardIdKabylakeGrizzlyMtn: //GPIO assignements for GrizzlyMtn
      PcdSet32S (PcdFingerPrintSleepGpio, GPIO_SKL_LP_GPP_A22);//connected to MODEM_DISABLE_RADIO_N in GM
      PcdSet32S (PcdFingerPrintIrqGpio  , GPIO_SKL_LP_GPP_A23);//connected to SSD_PWREN in GM
      PcdSet32S (PcdGnssResetGpio       , GPIO_SKL_LP_GPP_G0);//GNSS_RESET_N
      PcdSet32S (PcdBluetoothWakeGpio   , GPIO_SKL_LP_GPP_C2);//connected to TLS_CONF_PU in GM
      PcdSet32S (PcdBluetoothRfKillGpio , GPIO_SKL_LP_GPP_B4);//BT_RF_KILL_N
      PcdSet32S (PcdBluetoothIrqGpio    , GPIO_SKL_LP_GPP_B5);//not connected in GM
      PcdSet32S (PcdTouchpadIrqGpio     , GPIO_SKL_LP_GPP_B3);//connected to WLAN_PERST_N in GM
      PcdSet32S (PcdTouchpanelIrqGpio   , GPIO_SKL_LP_GPP_E7);//TOUCH_PNL_INT_N
      break;

     case BoardIdKabylakeCoyoteMtn: //GPIO assignements for CoyoteMountain
      PcdSet32S (PcdGnssResetGpio       , GPIO_SKL_LP_GPP_G0);//GNSS_RESET_N
      PcdSet32S (PcdBluetoothWakeGpio   , GPIO_SKL_LP_GPP_G1);//connected to TLS_CONF_PU in GM
      PcdSet32S (PcdTouchpanelIrqGpio   , GPIO_SKL_LP_GPP_D2);//TOUCH_PNL_INT_N
      break;

    default:
      PcdSet32S (PcdFingerPrintSleepGpio, GPIO_SKL_LP_GPP_A22);
      PcdSet32S (PcdFingerPrintIrqGpio  , GPIO_SKL_LP_GPP_A23);
      PcdSet32S (PcdGnssResetGpio       , GPIO_SKL_LP_GPP_B23);
      PcdSet32S (PcdBluetoothWakeGpio   , GPIO_SKL_LP_GPP_C2);//not sure if connected?
      PcdSet32S (PcdBluetoothRfKillGpio , GPIO_SKL_LP_GPP_B4);
      PcdSet32S (PcdBluetoothIrqGpio    , GPIO_SKL_LP_GPP_B5);
      PcdSet32S (PcdTouchpadIrqGpio     , GPIO_SKL_LP_GPP_B3);
      PcdSet32S (PcdTouchpanelIrqGpio   , GPIO_SKL_LP_GPP_E7);
      break;
  }

  PcdSetBoolS (PcdEInkDfuDevice, FALSE);

  //
  // Board Specific Init
  //
  switch (BoardId) {
    case BoardIdSkylakeAioRvp10Crb:
    case BoardIdSkylakeAioRvp10Evp:
    case BoardIdSkylakeAioRvp10CrbPpv:
      PcdSetBoolS (PcdRvp10Rtd3TableEnable, TRUE);
      break;
    case BoardIdSkylakeSdlBrk:
      PcdSetBoolS (PcdSdlBrkRtd3TableEnable, TRUE);
      break;
    case BoardIdSkylakeSds:
    case BoardIdSkylakePhabletPoc:
    case BoardIdSkylakePantherMtn:
    case BoardIdSkylakeStarbrook:
    case BoardIdKabylakeGrizzlyMtn:
      PcdSetBoolS (PcdSdsRtd3TableEnable, TRUE);
      PcdSetBoolS (PcdSdsDptfTablePresent, TRUE);
      break;
    default:
      break;
  }

  //
  // 3D Camera Specific Init
  //
  if (BoardId == BoardIdSkylakeSds) {
    Pcd64.BoardGpioConfig.Type = 1;
    Pcd64.BoardGpioConfig.u.Pin = GPIO_SKL_LP_GPP_E0;
    PcdSet64S (PcdIvcamDfuGpio, Pcd64.Blob);

    Pcd64.BoardGpioConfig.Type = 1;
    Pcd64.BoardGpioConfig.u.Pin = GPIO_SKL_LP_GPP_B2;
    PcdSet64S (PcdIvcamPwrGpio, Pcd64.Blob);
  }

  if ((BoardId == BoardIdSkylakeURvp7) || (BoardId == BoardIdSkylakeURvp7Ppv) || (BoardId == BoardIdKabylakeRDdr4) || (BoardId == BoardIdKabylakeRLpddr3) || (BoardId == BoardIdKabylakeKcDdr3)) {
    Pcd64.BoardGpioConfig.Type = 2;
    Pcd64.BoardGpioConfig.u.Expander.Section = 0;
    Pcd64.BoardGpioConfig.u.Expander.Pin = 18;
    PcdSet64S (PcdIvcamDfuGpio, Pcd64.Blob);

    Pcd64.BoardGpioConfig.Type = 2;
    Pcd64.BoardGpioConfig.u.Expander.Section = 0;
    Pcd64.BoardGpioConfig.u.Expander.Pin = 17;
    PcdSet64S (PcdIvcamPwrGpio, Pcd64.Blob);
  }

  if ((BoardId == BoardIdSkylakeA0Rvp3) || (BoardId == BoardIdSkylakeA0Rvp3)) {
    if ((BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3Cff) || (BoardIdOrgValue == BoardIdAmberLakeY42Lpddr3CffCc)){
      Pcd64.BoardGpioConfig.Type = 1;
      Pcd64.BoardGpioConfig.u.Pin = GPIO_SKL_LP_GPP_C9;
    } else {
      Pcd64.BoardGpioConfig.Type = 2;
      Pcd64.BoardGpioConfig.u.Expander.Section = 0;
      Pcd64.BoardGpioConfig.u.Expander.Pin = 17;
    }
    PcdSet64S (PcdDsPwrGpio, Pcd64.Blob);
  }

  PcdSet8S (PcdIvcamPresent, 0x0);
  PcdSet8S (PcdIvcamEpAddress, 0x0);
  if (BoardId == BoardIdSkylakeSds) {
    PcdSet8S (PcdIvcamPresent, 0xF);
    PcdSet8S (PcdIvcamEpAddress, 0x10);
  }
  if ((BoardId == BoardIdSkylakeURvp7) ||
      (BoardId == BoardIdSkylakeURvp7Ppv) ||
      (BoardId == BoardIdKabylakeUDdr3lRvp7) ||
      (BoardId == BoardIdKabylakeRDdr4) ||
      (BoardId == BoardIdKabylakeRLpddr3) ||
      (BoardId == BoardIdKabylakeKcDdr3)) {
    PcdSet8S (PcdIvcamPresent, 0xF);
    PcdSet8S (PcdIvcamEpAddress, 0x12);
  }

  PcdSet8S (PcdDsPresent, 0x0);
  PcdSet8S (PcdDsEpAddress, 0x0);

  if ((BoardId == BoardIdSkylakeA0Rvp3) || (BoardId == BoardIdSkylakeA0Rvp3)) {
    PcdSet8S (PcdDsPresent, 0xF);
    PcdSet8S (PcdDsEpAddress, 0xF);
  }

  return EFI_SUCCESS;
}

/**
  Init Common Platform Board Config Block.

  @param[in]  PlatformInfo      a pointer to Platform Info.

  @retval     EFI_SUCCESS       The function completed successfully.
**/
EFI_STATUS
InitCommonPlatformConfigBlock (
  IN PLATFORM_INFO *PlatformInfo,
  IN UINT16 BoardId,
  IN UINT16 BoardIdOrgValue
)
{
  UINTN Size;
  PCD64_BLOB Data64;

  //
  // Enable EC SMI# for SMI
  //
  switch (BoardId) {
    case BoardIdSkylakeA0Lpddr3Crb:
    case BoardIdSkylakeA0Ddr4Rvp2:
      // DxeConfig->EcSmiGpio = GPIO_SKL_LP_GPP_A0; // Platform specific @ TBD
      break;
    case BoardIdSkylakeULpddr3Rvp5:
    case BoardIdSkylakeULpddr3Rvp5Ppv:
    case BoardIdSkylakeLpdimmDoe:
    case BoardIdSkylakeSds:
    case BoardIdSkylakePantherMtn:
    case BoardIdKabylakeGrizzlyMtn:
    case BoardIdKabylakeCoyoteMtn:
    case BoardIdSkylakeStarbrook:
      //
      // SPT-LP, Group E, Pad 15
      //
      PcdSet32S (PcdEcSmiGpio, GPIO_SKL_LP_GPP_E15);
      PcdSet32S (PcdEcLowPowerExitGpio, GPIO_SKL_LP_GPP_A7);
      break;
    case BoardIdSkylakeAioRvp10Crb:
    case BoardIdSkylakeAioRvp10Evp:
    case BoardIdSkylakeAioRvp10CrbPpv:
    case BoardIdSkylakeSlpfPpv:
      //
      // SPT-H, Group I, Pad 3
      //
      PcdSet32S (PcdEcSmiGpio, GPIO_SKL_H_GPP_I3);
      break;
  };

  //
  // ESRT Firmware ID.
  //
  Size = sizeof (EFI_GUID);
  switch (BoardId) {
    case BoardIdSkylakeSds:
      PcdSetPtrS(PcdEsrtFirmwareId, &Size, &gSysFwSklSdsGuid);
      break;
    case BoardIdSkylakeDtRvp8Crb:
      PcdSetPtrS(PcdEsrtFirmwareId, &Size, &gSysFwRvp8Guid);
      break;
    case BoardIdZumbaBeachServerEv:
    case BoardIdZumbaBeachServerCrb:
      PcdSetPtrS(PcdEsrtFirmwareId, &Size, &gSysFwZumbaBeachGuid);
      break;
    default:
      //
      // BoardID is not on the list. Pcd default value is zero.
      //
      break;
  }

  //
  // HID I2C Interrupt GPIO.
  //
  switch (BoardId) {
    default:
      // on all supported boards interrupt input is on same GPIO pad. How convenient.
      PcdSet32S (PcdHidI2cIntPad, GPIO_SKL_LP_GPP_E7);
  }

  //
  // PS2 KB Specific Init for Sds Serial platform.
  // @todo : include BoardIdKabylakeGrizzlyMtn ir not?
  //
  if ((BoardId == BoardIdSkylakeSds)        ||
      (BoardId == BoardIdSkylakePantherMtn) ||
      (BoardId == BoardIdKabylakeGrizzlyMtn) ||
      (BoardId == BoardIdSkylakeStarbrook)) {
    PcdSetBoolS (PcdDetectPs2KbOnCmdAck, TRUE);
  } else {
    PcdSetBoolS (PcdDetectPs2KbOnCmdAck,  FALSE);
  }

  switch (BoardId) {
    case BoardIdSkylakeA0Lpddr3Crb:
      PcdSetBoolS (PcdSpdAddressOverride, TRUE);
      break;
    default:
      PcdSetBoolS (PcdSpdAddressOverride, FALSE);
      break;
  }

  //
  // Sds Specific Init
  //
  if (BoardId == BoardIdSkylakeSds) {
    PcdSetBoolS (PcdPchFpsSupport, TRUE);
    PcdSetBoolS (PcdEInkDfuSpecialPowerCycleEnable, TRUE);
    PcdSetBoolS (PcdUsbIvCamSupport, TRUE);
    PcdSetBoolS (PcdSdsPepBccdSmmCallbackSupport, TRUE);
    PcdSetBoolS (PcdSdsSetupOptionOverride, TRUE);
  } else {
    PcdSetBoolS (PcdPchFpsSupport, FALSE);
    PcdSetBoolS (PcdEInkDfuSpecialPowerCycleEnable, FALSE);
    PcdSetBoolS (PcdUsbIvCamSupport, FALSE);
    PcdSetBoolS (PcdSdsPepBccdSmmCallbackSupport, FALSE);
    PcdSetBoolS (PcdSdsSetupOptionOverride, FALSE);
  }

  //
  // HaloMd2 Specific Init.
  //
  if (BoardId == BoardIdSkylakeSdlBrk) {
    PcdSetBoolS (PcdHaloMd2Present, TRUE);
  } else {
    PcdSetBoolS (PcdHaloMd2Present, FALSE);
  }

  //
  // DDISelection
  //
  switch (BoardId) {
    case BoardIdSkylakeA0Lpddr3Crb:
    case BoardIdSkylakeA0Ddr4Rvp2:
    case BoardIdSkylakeULpddr3Rvp5:
    case BoardIdSkylakeULpddr3Rvp5Ppv:
    case BoardIdSkylakeLpdimmDoe:
    case BoardIdSkylakeSds:
    case BoardIdSkylakePantherMtn:
    case BoardIdKabylakeGrizzlyMtn:
    case BoardIdKabylakeCoyoteMtn:
      PcdSet8S (PcdDDISelection, 1);
      break;
    default:
      PcdSet8S (PcdDDISelection, 0);
      break;
  }

  //
  // Mipi Display Init
  //
  if (BoardId == BoardIdKabylakeGrizzlyMtn || BoardId == BoardIdKabylakeCoyoteMtn) {
    PcdSetBoolS (PcdMipiDisplayPresent, TRUE);
  } else {
    PcdSetBoolS (PcdMipiDisplayPresent, FALSE);
  }

  //
  // Sensor Hub
  //
  switch (BoardId) {
    case BoardIdSkylakeA0Lpddr3Crb:
    case BoardIdSkylakeA0Ddr4Rvp2:
    case BoardIdSkylakeULpddr3Rvp5:
    case BoardIdSkylakeULpddr3Rvp5Ppv:
    case BoardIdSkylakeLpdimmDoe:
      PcdSetBoolS (PcdSensorHubSupport, TRUE);
      break;
    default:
      PcdSetBoolS (PcdSensorHubSupport, FALSE);
      break;
  }

  //
  // GFX Detect
  //
  switch (BoardId) {
    case BoardIdSkylakeDtRvp8Crb:
    case BoardIdSkylakeAioRvp9Crb:
    case BoardIdSkylakeAioRvp10Crb:
    case BoardIdSkylakeAioRvp10CrbPpv:
    case BoardIdSkylakeAioRvp10Evp:
    case BoardIdSkylakeSdlBrk:
      //RVP8,9,10,11,16, SaddleBrook
      //GPIO_SKL_H_GPP_F13 - SATA_SDATAOUT0/GFX_CRB_DETECT
      Data64.BoardGpioConfig.Type = BoardGpioTypePch;
      Data64.BoardGpioConfig.u.Pin = GPIO_SKL_H_GPP_F13;
      //DxeConfig->GfxCrbDetectGpio.Type = BoardGpioTypePch;
      //DxeConfig->GfxCrbDetectGpio.u.Pin = GPIO_SKL_H_GPP_F13;
      break;

    case BoardIdSkylakeULpddr3Rvp5:
    case BoardIdSkylakeULpddr3Rvp5Ppv:
      //RVP 3,5,7,13,15
      //IO_EXPANDER_GPIO_5 in IO_EXPANDER_0
      Data64.BoardGpioConfig.Type = BoardGpioTypeExpander;
      Data64.BoardGpioConfig.u.Expander.Section = IO_EXPANDER_0;
      Data64.BoardGpioConfig.u.Expander.Pin = IO_EXPANDER_GPIO_5;
      break;

    default:
      // Not all the boards support GFX_CRB_DET. This is not an error.
      Data64.BoardGpioConfig.Type = BoardGpioTypeNotSupported;
      break;
  }

  PcdSet64S (PcdGfxCrbDetectGpio, Data64.Blob);

  //
  // Battery Present
  //
  switch (BoardId) {
    // SDS/Tablet group. Only supports Real battery.
    case BoardIdSkylakeSds:
    case BoardIdSkylakePantherMtn:
    case BoardIdSkylakeStarbrook:
    case BoardIdKabylakeGrizzlyMtn:
    case BoardIdKabylakeCoyoteMtn:
      PcdSet8S (PcdBatteryPresent, BOARD_REAL_BATTERY_SUPPORTED);
      break;
    // Desktop group. No battery support.
    case BoardIdSkylakeDtRvp8Crb:
    case BoardIdSkylakeAioRvp9Crb:
    case BoardIdSkylakeAioRvp10Crb:
    case BoardIdSkylakeAioRvp10Evp:
    case BoardIdSkylakeAioRvp10CrbPpv:
    case BoardIdSkylakeSlpfPpv:
      PcdSet8S (PcdBatteryPresent, BOARD_NO_BATTERY_SUPPORT);
      break;
    // Default: supports both real and virtual batteries.
    default:
      PcdSet8S (PcdBatteryPresent, BOARD_REAL_BATTERY_SUPPORTED | BOARD_VIRTUAL_BATTERY_SUPPORTED);
      break;
  }

  //
  // TS-on-DIMM temperature
  //
  switch (BoardIdOrgValue) {
    case BoardIdSkylakeSds:
    case BoardIdSkylakePantherMtn:
    case BoardIdKabylakeGrizzlyMtn:
    case BoardIdKabylakeCoyoteMtn:
    case BoardIdSkylakeStarbrook:
      PcdSetBoolS (PcdTsOnDimmTemperature, TRUE);
      break;
    default:
      PcdSetBoolS (PcdTsOnDimmTemperature, FALSE);
      break;
  }

  //
  // Ds End Point Camera
  //
  switch (BoardIdOrgValue) {
    case BoardIdKabyLakeYLpddr3Rvp3:
    case BoardIdAmberLakeY42Lpddr3Rvp3:
    case BoardIdAmberLakeLpddr3Rvp3:
    case BoardIdAmberLakeLpddr3Rvp13:
      PcdSetBoolS (PcdDsEndpointCamera, TRUE);
      break;
    case BoardIdAmberLakeY42Lpddr3Cff:
    case BoardIdAmberLakeY42Lpddr3CffCc:
      PcdSetBoolS (PcdDsEndpointCamera, TRUE);
      break;
    default:
      PcdSetBoolS (PcdDsEndpointCamera, FALSE);
      break;
  }

  //
  // Percep Ds 4 Camera
  //
  PcdSetBoolS (PcdPercepDs4Camera, FALSE);

  //
  // Disable IVCAM before going into Sx state
  //
  switch (BoardIdOrgValue) {
    case BoardIdSkylakeSds:
      PcdSetBoolS (PcdIvCamera, TRUE);
      break;
    default:
      PcdSetBoolS (PcdIvCamera, FALSE);
      break;
  }

  //
  // Real Battery 1 Control & Real Battery 2 Control
  //
  switch (BoardId) {
    case BoardIdSkylakeSds:
      PcdSet8S (PcdRealBattery1Control, 2);
      PcdSet8S (PcdRealBattery2Control, 1);
      break;
    default:
      PcdSet8S (PcdRealBattery1Control, 1);
      PcdSet8S (PcdRealBattery2Control, 2);
      break;
  }

  //
  // Sky Camera Sensor
  //
  switch (BoardId) {
    case BoardIdKabylakeGrizzlyMtn:
    case BoardIdKabylakeCoyoteMtn:
      PcdSetBoolS (PcdSkyCamSensor, TRUE);
      break;
    default:
      PcdSetBoolS (PcdSkyCamSensor, FALSE);
      break;
  }

  //
  // HD Audio I2S
  //
  switch (BoardId) {
    case BoardIdSkylakeSds:
      PcdSetBoolS (PcdHDAudioI2S, TRUE);
      break;
    default:
      PcdSetBoolS (PcdHDAudioI2S, FALSE);
      break;
  }

  //
  // H8S2113 SIO
  //
  switch (BoardId) {
    case BoardIdSkylakeAioRvp10Crb:
    case BoardIdSkylakeAioRvp10CrbPpv:
    case BoardIdSkylakeAioRvp10Evp:
    case BoardIdSkylakeSlpfPpv:
    case BoardIdSkylakeDtRvp8Crb:
      PcdSetBoolS (PcdH8S2113SIO, TRUE);
      if (BoardIdOrgValue == BoardIdKabyLakeOc) {
        PcdSetBoolS (PcdH8S2113SIO, FALSE);
      }
      break;
    default:
      PcdSetBoolS (PcdH8S2113SIO, FALSE);
      break;
  }

  //
  // NCT6776F COM, SIO & HWMON
  //
  switch (BoardId) {
    case BoardIdSkylakeSdlBrk:
      PcdSetBoolS (PcdNCT6776FCOM, TRUE);
      PcdSetBoolS (PcdNCT6776FSIO, TRUE);
      PcdSetBoolS (PcdNCT6776FHWMON, TRUE);
      break;
    default:
      PcdSetBoolS (PcdNCT6776FCOM, FALSE);
      PcdSetBoolS (PcdNCT6776FSIO, FALSE);
      PcdSetBoolS (PcdNCT6776FHWMON, FALSE);
      break;
  }

  //
  // Power Participant
  //
  switch (BoardId) {
    case BoardIdSkylakeSds:
      PcdSetBoolS (PcdPowerParticipant, TRUE);
      break;
    default:
      PcdSetBoolS (PcdPowerParticipant, FALSE);
      break;
  }

  //
  // Sensor Hub Application
  //
  switch (BoardId) {
    case BoardIdSkylakeSds:
      PcdSetBoolS (PcdSensorHubApp, TRUE);
      break;
    default:
      PcdSetBoolS (PcdSensorHubApp, FALSE);
      break;
  }

  //
  // EInk Application
  //
  switch (BoardId) {
    default:
      PcdSetBoolS (PcdEInkApp, FALSE);
      break;
  }

  //
  // ZPODD
  //
  switch (BoardId) {
    case BoardIdSkylakeAioRvp10Crb:
    case BoardIdSkylakeAioRvp10Evp:
      PcdSet8S (PcdZPODD, 2);
      break;
    case BoardIdSkylakeDtRvp8Crb:
      if((BoardIdOrgValue == BoardIdKabyLakeSDdr4UdimmEvErb) ||
         (BoardIdOrgValue == BoardIdKabyLakeSDdr4UdimmEvCrb) ||
         (BoardIdOrgValue == BoardIdKabyLakeSDdr4UdimmCrb)) {
        PcdSet8S (PcdZPODD, 2);
      } else {
        PcdSet8S (PcdZPODD, 0);
      }
      break;
    default:
      PcdSet8S (PcdZPODD, 0);
      break;
  }

  //
  //  DPTF: VSCT table check for Virtual Participants.
  //
  PcdSetBoolS (PcdVsctTableForSens, FALSE);  // load SEN taget in VSCT table for KBL-R.

  //
  //  DPTF: VSCT table check for Virtual Participants, use SEN targets for KBL-R
  //
  switch (BoardIdOrgValue) {
    case BoardIdKabylakeRDdr4:
    case BoardIdKabylakeRLpddr3:
      PcdSetBoolS (PcdVsctTableForSens, TRUE);   // load SEN target in VSCT table
      break;
    default:
      PcdSetBoolS (PcdVsctTableForSens, FALSE);  // load GEN target in VSCT table
      break;
  }

  //
  // SMC Runtime Sci Pin
  //
  switch (BoardId) {
    case BoardIdSkylakeULpddr3Rvp5:
    case BoardIdSkylakeULpddr3Rvp5Ppv:
    case BoardIdSkylakeLpdimmDoe:
    case BoardIdSkylakeSds:
    case BoardIdSkylakePhabletPoc:
    case BoardIdSkylakePantherMtn:
    case BoardIdKabylakeGrizzlyMtn:
    case BoardIdKabylakeCoyoteMtn:
    case BoardIdSkylakeStarbrook:
      PcdSet32S (PcdSmcRuntimeSciPin, (UINT32) GPIO_SKL_LP_GPP_E16);
      break;
    case BoardIdSkylakeDtRvp8Crb:
    case BoardIdSkylakeAioRvp9Crb:
    case BoardIdSkylakeAioRvp10Crb:
    case BoardIdSkylakeAioRvp10CrbPpv:
    case BoardIdSkylakeAioRvp10Evp:
    case BoardIdSkylakeSlpfPpv:
      PcdSet32S (PcdSmcRuntimeSciPin, (UINT32) GPIO_SKL_H_GPP_G3);
      break;
    default:
      PcdSet32S (PcdSmcRuntimeSciPin, 0x00);
      break;
  }

  //
  // Convertable Dock Support
  //
  switch (BoardId) {
    case BoardIdSkylakeSds:
      PcdSetBoolS (PcdConvertableDockSupport, TRUE);
      break;
    default:
      PcdSetBoolS (PcdConvertableDockSupport, FALSE);
      break;
  }

  //
  // Ec Hotkey F3, F4, F5, F6, F7 and F8 Support
  //
  switch (BoardId) {
    case BoardIdSkylakeULpddr3Rvp5:
    case BoardIdSkylakeULpddr3Rvp5Ppv:
    case BoardIdSkylakeLpdimmDoe:
    case BoardIdSkylakeSds:
    case BoardIdSkylakePhabletPoc:
    case BoardIdSkylakePantherMtn:
    case BoardIdKabylakeGrizzlyMtn:
    case BoardIdSkylakeStarbrook:
      PcdSet8S (PcdEcHotKeyF3Support, 1);
      PcdSet8S (PcdEcHotKeyF4Support, 1);
      PcdSet8S (PcdEcHotKeyF5Support, 1);
      PcdSet8S (PcdEcHotKeyF6Support, 1);
      PcdSet8S (PcdEcHotKeyF7Support, 1);
      PcdSet8S (PcdEcHotKeyF8Support, 1);
      break;
    default:
      PcdSet8S (PcdEcHotKeyF3Support, 0);
      PcdSet8S (PcdEcHotKeyF4Support, 0);
      PcdSet8S (PcdEcHotKeyF5Support, 0);
      PcdSet8S (PcdEcHotKeyF6Support, 0);
      PcdSet8S (PcdEcHotKeyF7Support, 0);
      PcdSet8S (PcdEcHotKeyF8Support, 0);
      break;
  }

  //
  // Virtual Button Volume Up & Done Support
  // Virtual Button Home Button Support
  // Virtual Button Rotation Lock Support
  //
  switch (BoardId) {
    case BoardIdSkylakeSds:
    case BoardIdSkylakePhabletPoc:
    case BoardIdSkylakePantherMtn:
    case BoardIdKabylakeGrizzlyMtn:
    case BoardIdKabylakeCoyoteMtn:
    case BoardIdSkylakeStarbrook:
      PcdSetBoolS (PcdVirtualButtonVolumeUpSupport, TRUE);
      PcdSetBoolS (PcdVirtualButtonVolumeDownSupport, TRUE);
      PcdSetBoolS (PcdVirtualButtonHomeButtonSupport, TRUE);
      PcdSetBoolS (PcdVirtualButtonRotationLockSupport, TRUE);
      break;
    default:
      PcdSetBoolS (PcdVirtualButtonVolumeUpSupport, FALSE);
      PcdSetBoolS (PcdVirtualButtonVolumeDownSupport, FALSE);
      PcdSetBoolS (PcdVirtualButtonHomeButtonSupport, FALSE);
      PcdSetBoolS (PcdVirtualButtonRotationLockSupport, FALSE);
      break;
  }

  //
  // Slate Mode Switch Support
  //
  switch (BoardId) {
    case BoardIdSkylakeSds:
    case BoardIdSkylakePhabletPoc:
    case BoardIdSkylakePantherMtn:
    case BoardIdKabylakeGrizzlyMtn:
    case BoardIdKabylakeCoyoteMtn:
    case BoardIdSkylakeStarbrook:
      PcdSetBoolS (PcdSlateModeSwitchSupport, TRUE);
      break;
    default:
      PcdSetBoolS (PcdSlateModeSwitchSupport, FALSE);
      break;
  }

  //
  // Ac Dc Auto Switch Support
  //
  switch (BoardId) {
    case BoardIdSkylakeDtRvp8Crb:
    case BoardIdSkylakeAioRvp9Crb:
    case BoardIdSkylakeAioRvp10Crb:
    case BoardIdSkylakeAioRvp10Evp:
      PcdSetBoolS (PcdAcDcAutoSwitchSupport, FALSE);
      break;
    default:
      PcdSetBoolS (PcdAcDcAutoSwitchSupport, TRUE);
      break;
  }

  //
  // Pm Power Button Gpio Pin
  //
  switch (BoardId) {
    case BoardIdSkylakeULpddr3Rvp5:
    case BoardIdSkylakeULpddr3Rvp5Ppv:
    case BoardIdSkylakeLpdimmDoe:
    case BoardIdSkylakeSds:
    case BoardIdSkylakePhabletPoc:
    case BoardIdSkylakePantherMtn:
    case BoardIdKabylakeGrizzlyMtn:
    case BoardIdKabylakeCoyoteMtn:
    case BoardIdSkylakeStarbrook:
      PcdSet32S (PcdPmPowerButtonGpioPin, (UINT32) GPIO_SKL_LP_GPD3);
      break;
    default:
      PcdSet32S (PcdPmPowerButtonGpioPin, 0x00);
      break;
  }

  //
  // Acpi Enable All Button Support
  //
  switch (BoardId) {
    case BoardIdSkylakeSds:
    case BoardIdKabylakeGrizzlyMtn:
    case BoardIdKabylakeCoyoteMtn:
      PcdSetBoolS (PcdAcpiEnableAllButtonSupport, TRUE);

      break;
    default:
      PcdSetBoolS (PcdAcpiEnableAllButtonSupport, FALSE);
      break;
  }

  //
  // Acpi Hid Driver Button Support
  //
  switch (BoardId) {
    case BoardIdSkylakeSds:
      PcdSetBoolS (PcdAcpiHidDriverButtonSupport, TRUE);
      break;
    default:
      PcdSetBoolS (PcdAcpiHidDriverButtonSupport, FALSE);
      break;
  }

  //
  // Ps2 Support
  //
  switch (BoardIdOrgValue) {
    case BoardIdKabyLakeSDdr4UdimmCrb:
      PcdSetBoolS (PcdPs2SupportDisable, TRUE);
      break;
    default:
      PcdSetBoolS (PcdPs2SupportDisable, FALSE);
      break;
  }

  return EFI_SUCCESS;
}


/**
  SkylaeA0Rvp3 board configuration init function for DXE phase.

  @param  Content  pointer to the buffer contain init information for board init.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   The parameter is NULL.
**/
EFI_STATUS
EFIAPI
OthersBoardInit (
  IN VOID *Content
  )
{
  EFI_STATUS Status;
  PLATFORM_INFO *PlatformInfo;
  UINT16 BoardId;
  UINT16 BoardIdOrgValue;

  if (Content == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  PlatformInfo = (PLATFORM_INFO *) Content;

  Status = GetBoardId (&BoardId, &BoardIdOrgValue);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    BoardId = COMMON_BOARD_ID;
  }
  DEBUG ((DEBUG_INFO, "OthersBoardInitLib.c function OthersBoardInit\n"));
  Status = InitAcpiPlatformConfigBlock (PlatformInfo, BoardId, BoardIdOrgValue);
  ASSERT_EFI_ERROR(Status);

  Status = InitCommonPlatformConfigBlock (PlatformInfo, BoardId, BoardIdOrgValue);
  DEBUG ((DEBUG_INFO, "OthersBoardInitLib.c InitCommonPlatformConfigBlock BoardId = 0x%x and Status = %r\n", BoardId, Status));
  ASSERT_EFI_ERROR(Status);

  Status = OthersBoardMiscInit (PlatformInfo, BoardId, BoardIdOrgValue);
  ASSERT_EFI_ERROR(Status);

  Status = OthersBoardSmbiosInit (PlatformInfo, BoardId, BoardIdOrgValue);
  ASSERT_EFI_ERROR(Status);

  return EFI_SUCCESS;
}

/**
  The library constructuor.

  The function does the necessary initialization work for this library
  instance.

  @param[in]  ImageHandle       The firmware allocated handle for the UEFI image.
  @param[in]  SystemTable       A pointer to the EFI system table.

  @retval     EFI_SUCCESS       The function always return EFI_SUCCESS for now.
                                It will ASSERT on error for debug version.
  @retval     EFI_ERROR         Please reference LocateProtocol for error code details.
**/
EFI_STATUS
EFIAPI
DxeOthersBoardInitLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  return CommonInitInternalLibConstructor (mOthersBoardInit, DEFAULT_INIT_SLOT_DXE);
}

