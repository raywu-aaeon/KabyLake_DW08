/** @file
  Header file for the SmbiosMisc Driver.

  @par Revision Reference:
  SMBIOS Specification version 2.7.1 from DMTF: http://www.dmtf.org/standards/smbios
  Intel Framework Specifications, all available at: http://www.intel.com/technology/framework/spec.htm
   - SMBUS Host Controller Protocol Specification
  Unified Extensible Firmware Interface (UEFI) Specifications: http://www.uefi.org/specs/

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2011 - 2016 Intel Corporation.

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

#ifndef _SMBIOS_MISC_DRIVER_H
#define _SMBIOS_MISC_DRIVER_H

#include "SmbiosMiscStrings.h"

#define T14_FVI_STRING          "Firmware Version Info"

//
// T1 related defines
//
#define SUPPORTED_INTEL_WLAN_DEVICES {\
  0x08B0, 0x08B1, 0x08B2, 0x08B3, 0x08B4, 0x08B5, 0x08B6\
  }

#define SMBIOS_T1_SYSTEM_UUID_VARIABLE         (L"SmbiosType1SystemUuid")

extern EFI_GUID       gEfiSmbiosVariableGuid;

//
// This is definition for SMBIOS Oem data type 0x88
//
#define EFI_SMBIOS_TYPE_OEM_DEFINED_0x88    0x88

typedef struct {
  SMBIOS_STRUCTURE              Hdr;
  UINT16                        RawData;
  UINT16                        Zero;    //terminator
} SMBIOS_TABLE_TYPE0x88;

//
// Data table entry update function.
//
typedef EFI_STATUS (EFIAPI EFI_MISC_SMBIOS_DATA_FUNCTION) (
  IN  VOID                 *RecordData,
  IN  EFI_SMBIOS_PROTOCOL  *Smbios
  );


//
// Data table entry definition.
//
typedef struct {
  //
  // intermediat input data for SMBIOS record
  //
  VOID                              *RecordData;
  EFI_MISC_SMBIOS_DATA_FUNCTION     *Function;
} EFI_MISC_SMBIOS_DATA_TABLE;


//
// Data Table extern definitions.
//
#define MISC_SMBIOS_TABLE_EXTERNS(NAME1, NAME2, NAME3) \
extern NAME1 NAME2 ## Data; \
extern EFI_MISC_SMBIOS_DATA_FUNCTION NAME3 ## Function


//
// Data Table entries
//

#define MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION(NAME1, NAME2) \
{ \
  & NAME1 ## Data, \
  & NAME2 ## Function \
}


//
// Global definition macros.
//
#define MISC_SMBIOS_TABLE_DATA(NAME1, NAME2) \
  NAME1 NAME2 ## Data

#define MISC_SMBIOS_TABLE_FUNCTION(NAME2) \
  EFI_STATUS EFIAPI NAME2 ## Function( \
  IN  VOID                  *RecordData, \
  IN  EFI_SMBIOS_PROTOCOL   *Smbios \
  )

//
// BCD to Decimal Conversion
//
#define BCD_TO_DECIMAL(A)           (((((A) >> 4) & 0xF) * 10) + ((A) & 0xF))

// Data Table Array
//
extern EFI_MISC_SMBIOS_DATA_TABLE   mSmbiosMiscDataTable[];

//
// Data Table Array Entries
//
extern UINTN                        mSmbiosMiscDataTableEntries;
extern EFI_HII_HANDLE               mHiiHandle;

/**
  Add an SMBIOS record.

  @param  Smbios                The EFI_SMBIOS_PROTOCOL instance.
  @param  SmbiosHandle          A unique handle will be assigned to the SMBIOS record.
  @param  Record                The data for the fixed portion of the SMBIOS record. The format of the record is
                                determined by EFI_SMBIOS_TABLE_HEADER.Type. The size of the formatted area is defined
                                by EFI_SMBIOS_TABLE_HEADER.Length and either followed by a double-null (0x0000) or
                                a set of null terminated strings and a null.

  @retval EFI_SUCCESS           Record was added.
  @retval EFI_OUT_OF_RESOURCES  Record was not added due to lack of system resources.

**/
EFI_STATUS
AddSmbiosRecord (
  IN EFI_SMBIOS_PROTOCOL        *Smbios,
  OUT EFI_SMBIOS_HANDLE         *SmbiosHandle,
  IN EFI_SMBIOS_TABLE_HEADER    *Record
  );

/**
  Initialize SMBIOS table strings.

  @param[out] **Destination            The pointer for the destination.
  @param[in]  *Source                  The pointer for the source date.
  @param[out] **StrBuffer              The pointer for string buffer.
  @param[out] *Count                   The accumulated number of strings.
  @param[out] *SizeOfStrings           The accumulated number of strings length.
  @param[in, out]  *StrBuffSize         Size of StrBuffer.

  @retval     EFI_SUCCESS              Successful.
  @retval     EFI_INVALID_PARAMETER    Distination pointer is not NULL.
  @retval     EFI_BUFFER_TOO_SMALL     StrBuffSize is zero.

**/
EFI_STATUS
SmbiosStrInit (
  OUT CHAR16        **Destination,
  IN  CHAR16  CONST *Source,
  OUT CHAR16        **StrBuffer,
  OUT UINTN          *Count,
  OUT UINTN          *SizeOfStrings,
  IN OUT UINTN      *StrBuffSize
  );

/**
  Update SMBIOS strings.

  @param[in]  **StringArray             The strings to be updated.
  @param[in]  *StarAddr                 The pointer of strat location.
  @param[in]  NumOfStrings              The number of strings.
**/
VOID
SmbiosStringsUpdate (
  CHAR16            **StringArray,
  UINT8             *StartAddr,
  UINTN             NumOfStrings,
  UINTN             BufferSize
  );

/**
  Create Dynamic SMBIOS Table and store data.

  @retval EFI_SUCCESS                Structure created sucessfully.
  @retval EFI_NOT_READY              Some of The SMBIOS records was not available yet.
  @retval EFI_OUT_OF_RESOURCES       No enough memory. sdg

**/
EFI_STATUS
EFIAPI
CreateDynamicSmbiosTable (
  VOID
  );

VOID
EFIAPI
DynamicTable(
  IN EFI_EVENT Event,
  IN VOID      *Context
  );

#define PortTypeSata  0x20


//
// BUGBUG: Following definitions are defined in Smbios spec and should be defined in IndustryStardard/Smbios.h
//

//
// Portable Battery - SMBIOS Type 22
//

typedef enum {
  EfiPortableBatteryDeviceChemistryOther = 1,
  EfiPortableBatteryDeviceChemistryUnknown = 2,
  EfiPortableBatteryDeviceChemistryLeadAcid = 3,
  EfiPortableBatteryDeviceChemistryNickelCadmium = 4,
  EfiPortableBatteryDeviceChemistryNickelMetalHydride = 5,
  EfiPortableBatteryDeviceChemistryLithiumIon = 6,
  EfiPortableBatteryDeviceChemistryZincAir = 7,
  EfiPortableBatteryDeviceChemistryLithiumPolymer = 8
} EFI_MISC_PORTABLE_BATTERY_DEVICE_CHEMISTRY;

//
// Misc. Hardware Security - SMBIOS Type 24
//

typedef enum {
  EfiHardwareSecurityStatusDisabled       = 0,
  EfiHardwareSecurityStatusEnabled        = 1,
  EfiHardwareSecurityStatusNotImplemented = 2,
  EfiHardwareSecurityStatusUnknown        = 3
} EFI_MISC_HARDWARE_SECURITY_STATUS;

#endif
