/** @file*

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2017 Intel Corporation.

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

#ifndef PCH_HSIO_PTSSTABLES_H_
#define PCH_HSIO_PTSSTABLES_H_

#include <PchAccess.h>

///
/// SATA PTSS Topology Types
///
typedef enum {
  PchSataTopoUnknown = 0x00,
  PchSataTopoIsata,
  PchSataTopoDirectConnect,
  PchSataTopoFlex,
  PchSataTopoM2
} PCH_SATA_TOPOLOGY;

///
/// PCIe PTSS Topology Types
///
typedef enum {
  PchPcieTopoUnknown = 0x00,
  PchPcieTopox1,
  PchPcieTopox4,
  PchPcieTopoSataE,
  PchPcieTopoM2
} PCH_PCIE_TOPOLOGY;

///
/// The PCH_SBI_PTSS_HSIO_TABLE block describes HSIO PTSS settings for PCH.
///
typedef struct {
  UINT8       LaneNum;
  UINT8       PhyMode;
  UINT16      Offset;
  UINT32      Value;
  UINT32      BitMask;
} PCH_SBI_PTSS_HSIO_TABLE;

typedef struct {
  PCH_SBI_PTSS_HSIO_TABLE   PtssTable;
  UINT16                    Topology;
} HSIO_PTSS_TABLES;

#define HSIO_PTSS_TABLE_SIZE(A) A##_Size = sizeof (A) / sizeof (HSIO_PTSS_TABLES)

#endif // PCH_HSIO_PTSSTABLES_H_
