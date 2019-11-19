/** @file
  Header file for PCH Pci Express helps library implementation.

@copyright
  INTEL CONFIDENTIAL
  Copyright 2018 Intel Corporation.

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
#ifndef _PCI_EXPRESS_HELPER_LIBRARY_H_
#define _PCI_EXPRESS_HELPER_LIBRARY_H_


#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <IndustryStandard/Pci.h>
#include <Library/PciSegmentLib.h>
#include <PcieRegs.h>


typedef struct {
  UINT32 Seg     : 8;
  UINT32 Bus     : 8;
  UINT32 Dev     : 5;
  UINT32 Func    : 3;
  UINT32 PcieCap : 8;
} SBDF;


typedef enum {
  DevTypePci,
  DevTypePcieEndpoint,
  DevTypePcieUpstream,
  DevTypePcieDownstream,
  DevTypeMax
} PCI_DEV_TYPE;

#define PCIE_DEVICE_PORT_TYPE_ROOT_PORT                        4
#define PCIE_DEVICE_PORT_TYPE_UPSTREAM_PORT                    5
#define PCIE_DEVICE_PORT_TYPE_DOWNSTREAM_PORT                  6

/**
  Converts device's segment:bus:device:function coordinates to flat address

  @param[in] Sbdf   device's segment:bus:device:function coordinates
  @retval    address of device's PCI cfg space
**/

UINT64
SbdfToBase (
  IN SBDF Sbdf
  );

/**
  Returns information about type of device.

  @param[out] Sbdf            device's segment:bus:device:function coordinates
  @retval     one of: not a PCIe device (legacy PCI), PCIe endpoint, PCIe upstream port or PCIe downstream port (including rootport)
**/

PCI_DEV_TYPE
GetDeviceType (
  OUT SBDF Sbdf
  );
/**
  Initializes Dev:Func numbers for use in FindNextPcieChild or FindNextLegalSbdf functions.

  @param[out] Sbdf            device's segment:bus:device:function coordinates
**/

VOID
InitChildFinder (
  OUT SBDF *Sbdf
  );


/**
  Finds next PCIe (not legacy PCI) device behind given device
  If device/function number == 0x1F/0x7, this function searches for children from scratch
  Any other device number means Dev:Func contain address of last found child device
  and this function should search for next one

  @param[in]     ParentDevType  type of bridge who's partent of this bus
  @param[in,out] Sbdf           On entry: location returned previously from this function
                                          Dev:Func value of 0x1F:0x07 means search should start from the beginning
                                On exit:  if PCIe device was found, its SBDF coordinates are returned
                                          otherwise, Dev:Func are initialized to 0x1F:0x07 for convenience
  @retval TRUE when next PCIe device was found; FALSE otherwise
**/

BOOLEAN
FindNextPcieChild (
  IN     PCI_DEV_TYPE ParentDevType,
  IN OUT SBDF   *Sbdf
  );

/**
  Checks the device is a bridge and has non-zero secondary bus number assigned.
  If so, it returns TRUE and initializes ChildSbdf with such values that
  allow searching for devices on the secondary bus.
  ChildSbdf will be mangled even if this function returns FALSE.

  Legal bus assignment is assumed. This function doesn't check subordinate bus numbers of
  the the device it was called on or any bridges between it and root complex

  @param[in]  Sbdf       device's segment:bus:device:function coordinates
  @param[out] ChildSbdf  SBDF initialized in such way that calling FindNextPcieChild( ) on it will find all children devices

  @retval TRUE if device is a bridge and has a bus behind it; FALSE otherwise
**/
BOOLEAN
HasChildBus (
  IN  SBDF   Sbdf,
  OUT SBDF   *ChildSbdf
  );

/**
  Returns TRUE and Dev:Func numbers where a PCIe device could legally be located, or FALSE if there
  no such coordinates left.

  Segment and Bus fields of SBDF structure are input only and determine which bus will be scanned.
  This function should be called in a while() loop. It replaces the less efficient method of
  using nested FOR loops that iterate over all device and function numbers. It is optimized for
  the amount of bus access. If function0 doesn't exist or doesn't have Multifunction bit set,
  then higher function numbers are skipped. If parent of this bus is a downstream port, then
  Device numbers 1-31 get skipped too (there can be only Dev0 behind downstream ports)
  If device/function number == 0x1F/0x7, this function returns first possible address, that is 0:0
  Any other device number means Dev:Func contain address of last found child device
  and this function should search for next one

  @param[in]     ParentDevType  type of bridge who's partent of this bus
  @param[in,out] Sbdf           On entry: location returned previously from this function
                                          Dev:Func value of 1F:07 means search should start from the beginning
                                On exit:  if legal Dev:Func combination was found, that Dev:Func is returned
                                          otherwise, Dev:Func are initialized to 1F:07 for convenience
  @retval TRUE when next legal Dev:Func address was found; FALSE otherwise
**/

BOOLEAN
FindNextLegalSbdf (
  IN PCI_DEV_TYPE     ParentDevType,
  IN OUT SBDF         *Sbdf
  );



#endif
