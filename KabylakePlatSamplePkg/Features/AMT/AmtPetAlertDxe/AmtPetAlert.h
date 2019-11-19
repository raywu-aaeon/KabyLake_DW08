/**@file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2011 - 2017 Intel Corporation.

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

#ifndef __AMT_PET_ALERT_H__
#define __AMT_PET_ALERT_H__

#include <Guid/GlobalVariable.h>
#include <Guid/PlatformGlobalVariable.h>
#include <Library/DxeAmtLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/HeciProtocol.h>
#include <CoreBiosMsg.h>
#include <AlertStandardFormat.h>
#include <Library/UefiBootManagerLib.h>
#include <Protocol/IdeControllerInit.h>
#include <SetupVariable.h>
#include <Protocol/NvmExpressPassthru.h>

// BAE device type defines
#define BAE_NETWORK_DEVICE    0x1
#define BAE_HDD_DEVICE        0x2
#define BAE_REMOVABLE_DEVICE  0x3
#define BAE_EMPTY_QUEUE       0x7F

#define NVME_GENERIC_TIMEOUT            5000000   ///< us
#define NVME_ADMIN_IDENTIFY_CMD         0x06
#define NVME_CONTROLLER_ID              0
#define NVME_ALL_NAMESPACES             0xFFFFFFFF
#define NVME_IDENTIFY_CONTROLLER_STRUCT 1

typedef struct {
  UINT16 Mp;                /* Maximum Power */
  UINT8  Rsvd1;             /* Reserved as of Nvm Express 1.1 Spec */
  UINT8  Mps:1;             /* Max Power Scale */
  UINT8  Nops:1;            /* Non-Operational State */
  UINT8  Rsvd2:6;           /* Reserved as of Nvm Express 1.1 Spec */
  UINT32 Enlat;             /* Entry Latency */
  UINT32 Exlat;             /* Exit Latency */
  UINT8  Rrt:5;             /* Relative Read Throughput */
  UINT8  Rsvd3:3;           /* Reserved as of Nvm Express 1.1 Spec */
  UINT8  Rrl:5;             /* Relative Read Leatency */
  UINT8  Rsvd4:3;           /* Reserved as of Nvm Express 1.1 Spec */
  UINT8  Rwt:5;             /* Relative Write Throughput */
  UINT8  Rsvd5:3;           /* Reserved as of Nvm Express 1.1 Spec */
  UINT8  Rwl:5;             /* Relative Write Leatency */
  UINT8  Rsvd6:3;           /* Reserved as of Nvm Express 1.1 Spec */
  UINT8  Rsvd7[16];         /* Reserved as of Nvm Express 1.1 Spec */
} NVME_PSDESCRIPTOR;

//
//  Identify Controller Data
//
typedef struct {
  //
  // Controller Capabilities and Features 0-255
  //
  UINT16 Vid;                 /* PCI Vendor ID */
  UINT16 Ssvid;               /* PCI sub-system vendor ID */
  UINT8  Sn[20];              /* Product serial number */

  UINT8  Mn[40];              /* Proeduct model number */
  UINT8  Fr[8];               /* Firmware Revision */
  UINT8  Rab;                 /* Recommended Arbitration Burst */
  UINT8  Ieee_oui[3];         /* Organization Unique Identifier */
  UINT8  Cmic;                /* Multi-interface Capabilities */
  UINT8  Mdts;                /* Maximum Data Transfer Size */
  UINT8  Cntlid[2];           /* Controller ID */
  UINT8  Rsvd1[176];          /* Reserved as of Nvm Express 1.1 Spec */
  //
  // Admin Command Set Attributes
  //
  UINT16 Oacs;                /* Optional Admin Command Support */
    #define NAMESPACE_MANAGEMENT_SUPPORTED  BIT3
    #define FW_DOWNLOAD_ACTIVATE_SUPPORTED  BIT2
    #define FORMAT_NVM_SUPPORTED            BIT1
    #define SECURITY_SEND_RECEIVE_SUPPORTED BIT0
  UINT8  Acl;                 /* Abort Command Limit */
  UINT8  Aerl;                /* Async Event Request Limit */
  UINT8  Frmw;                /* Firmware updates */
  UINT8  Lpa;                 /* Log Page Attributes */
  UINT8  Elpe;                /* Error Log Page Entries */
  UINT8  Npss;                /* Number of Power States Support */
  UINT8  Avscc;               /* Admin Vendor Specific Command Configuration */
  UINT8  Apsta;               /* Autonomous Power State Transition Attributes */
  UINT8  Rsvd2[246];          /* Reserved as of Nvm Express 1.1 Spec */

  //
  // NVM Command Set Attributes
  //
  UINT8  Sqes;                /* Submission Queue Entry Size */
  UINT8  Cqes;                /* Completion Queue Entry Size */
  UINT16 Rsvd3;               /* Reserved as of Nvm Express 1.1 Spec */
  UINT32 Nn;                  /* Number of Namespaces */
  UINT16 Oncs;                /* Optional NVM Command Support */
  UINT16 Fuses;               /* Fused Operation Support */
  UINT8  Fna;                 /* Format NVM Attributes */
  UINT8  Vwc;                 /* Volatile Write Cache */
  UINT16 Awun;                /* Atomic Write Unit Normal */
  UINT16 Awupf;               /* Atomic Write Unit Power Fail */
  UINT8  Nvscc;               /* NVM Vendor Specific Command Configuration */
  UINT8  Rsvd4;               /* Reserved as of Nvm Express 1.1 Spec */
  UINT16 Acwu;                /* Atomic Compare & Write Unit */
  UINT16 Rsvd5;               /* Reserved as of Nvm Express 1.1 Spec */
  UINT32 Sgls;                /* SGL Support  */
  UINT8  Rsvd6[164];          /* Reserved as of Nvm Express 1.1 Spec */
  //
  // I/O Command set Attributes
  //
  UINT8 Rsvd7[1344];          /* Reserved as of Nvm Express 1.1 Spec */
  //
  // Power State Descriptors
  //
  NVME_PSDESCRIPTOR PsDescriptor[32];

  UINT8  VendorData[1024];    /* Vendor specific data */
} NVME_ADMIN_CONTROLLER_DATA;

#pragma pack(push,1)

// Example 1: 16 10 0F 6F 02 68 08 FF FF 00 00 40 13 XX XX XX
// Example 2: 15 10 0F 6F 02 68 10 FF FF 22 00 AA 13 03 03 02

typedef struct {
  UINT8 SubCommand;      // 0x00
  UINT8 VersionNumber;   // 0x01
  UINT8 EventSensorType; // 0x02
  UINT8 EventType;       // 0x03
  UINT8 EventOffset;     // 0x04
  UINT8 EventSourceType; // 0x05
  UINT8 EventSeverity;   // 0x06
  UINT8 SensorDevice;    // 0x07
  UINT8 Sensornumber;    // 0x08
  UINT8 Entity;          // 0x09
  UINT8 EntityInstance;  // 0x0A
  UINT8 EventData1;      // 0x0B
  UINT8 EventData2;      // 0x0C
  UINT8 EventData3;      // 0x0D
  UINT8 EventData4;      // 0x0E
  UINT8 EventData5;      // 0x0F
} BOOT_AUDIT_ENTRY;

typedef struct {
  UINT8            Command;
  UINT8            ByteCount;
  BOOT_AUDIT_ENTRY Data;
} BOOT_AUDIT_ENTRY_PACK;

typedef struct {
  UINT8 BootQueue[3];
  UINT8 BootHarddriveTag[64];
} PET_ALERT_CFG;

#pragma pack(pop)

VOID
EFIAPI
AmtPetAlertReadyToBoot (
  EFI_EVENT Event,
  VOID      *ParentImageHandle
);

EFI_STATUS
AmtPetAlertLegacyBoot (
  EFI_EVENT Event,
  VOID      *ParentImageHandle
);

#endif
