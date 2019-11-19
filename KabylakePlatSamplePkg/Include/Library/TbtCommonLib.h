/**@file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2018 Intel Corporation.

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
#ifndef _TBT_COMMON_LIB_H_
#define _TBT_COMMON_LIB_H_

// AMI_OVERRIDE_START - Read / Write Host Router Status in NVRAM.
#if 0
#define TBT_HOST_STATUS_VARIABLE  L"TbtHostStatusVariable"
#else
extern EFI_GUID gAmiTbtHrStatusGuid;
#define AMI_TBT_HR_STATUS_VARIABLE L"TbtHRStatusVar"
#endif
// AMI_OVERRIDE_END - Read / Write Host Router Status in NVRAM.

// AMI_OVERRIDE_START - Already defined by token.
#if 0
#ifndef SW_SMI_IO_ADDRESS
#define SW_SMI_IO_ADDRESS           PcdGet8 (PcdSwSmiIoAddress)
#endif
#endif
// AMI_OVERRIDE_END - Already defined by token.

#ifndef SW_SMI_TBT_ENUMERATE
#define SW_SMI_TBT_ENUMERATE        PcdGet8 (PcdSwSmiTbtEnumerate)
#endif

#define MmPCIeAddress(Bus, Device, Function, Register) \
    ( \
      (UINTN) PcdGet64 (PcdPciExpressBaseAddress) + (UINTN) (Bus << 20) + (UINTN) (Device << 15) + (UINTN) (Function << 12) + (UINTN) \
        (Register) \
    )

#define PCIE2TBT_R                        0x54C
#define TBT2PCIE_R                        0x548
#define PCIE2TBT_VLD_B                    BIT0
#define TBT2PCIE_DON_R                    BIT0

#define PCIE2TBT_GO2SX                    (0x02 << 1)
#define PCIE2TBT_GO2SX_NO_WAKE            (0x03 << 1)
#define PCIE2TBT_SX_EXIT_TBT_CONNECTED    (0x04 << 1)
#define PCIE2TBT_SX_EXIT_NO_TBT_CONNECTED (0x05 << 1)
#define PCIE2TBT_OS_UP                    (0x06 << 1)
#define PCIE2TBT_SET_SECURITY_LEVEL       (0x08 << 1)
#define PCIE2TBT_GET_SECURITY_LEVEL       (0x09 << 1)
#define PCIE2TBT_CM_AUTH_MODE_ENTER       (0x10 << 1)
#define PCIE2TBT_CM_AUTH_MODE_EXIT        (0x11 << 1)
#define PCIE2TBT_BOOT_ON                  (0x18 << 1)
#define PCIE2TBT_BOOT_OFF                 (0x19 << 1)
#define PCIE2TBT_USB_ON                   (0x19 << 1)
#define PCIE2TBT_GET_ENUMERATION_METHOD   (0x1A << 1)
#define PCIE2TBT_SET_ENUMERATION_METHOD   (0x1B << 1)
#define PCIE2TBT_POWER_CYCLE              (0x1C << 1)
#define PCIE2TBT_PREBOOTACL               (0x1E << 1)

#define RESET_HR_BIT                      BIT0
#define ENUMERATE_HR_BIT                  BIT1
#ifndef AUTO
#define AUTO                              0x0
#endif
//
//Thunder Bolt Device IDs
//
// Light Ridge HR device ID
//
#define LR_HR 0x1513
//
// Eagle Ridge HR device IDs
//
#define ER_SFF_HR 0x151A
#define ER_HR     0x151B
//
// Cactus Ridge HR device IDs
//
#define CR_HR_2C  0x1548
#define CR_HR_4C  0x1547
//
// Redwood Ridge HR device IDs
//
#define RR_HR_4C  0x1569
#define RR_HR_2C  0x1567
//
// Falcon Ridge HR device IDs
//
#define FR_HR_2C  0x156B
#define FR_HR_4C  0x156D
//
// Win Ridge HR device ID
//
#define WR_HR_2C  0x157E
//
// Alpine Ridge HR device IDs
//
#define AR_HR_2C  0x1576
#define AR_HR_4C  0x1578
#define AR_XHC    0x15B5
#define AR_XHC_4C 0x15B6
#define AR_HR_LP  0x15C0
//
// Alpine Ridge C0 HR device IDs
//
#define AR_HR_C0_2C  0x15DA
#define AR_HR_C0_4C  0x15D3
//
// Titan Ridge HR device IDs
//
#define TR_HR_2C  0x15E7
#define TR_HR_4C  0x15EA
//
//End of Thunderbolt(TM) Device IDs
//

#define CONVENTIONAL_MEMORY_TOP 0xA0000 ///< 640 KB
#define BIN_FILE_SIZE_MAX       0x10000

#define OPTION_ROM_SIGNATURE    0xAA55

//
// Alpine Ridge FW OS capability
//
#define NO_OS_NATIVE_SUPPORT    0
#define OS_NATIVE_SUPPORT_ONLY  1
#define OS_NATIVE_SUPPORT_RTD3  2
///
/// PEG Capability Equates
///
#define PEG_CAP_ID  0x10
#define PEG_CAP_VER 0x2

typedef struct _DEV_ID {
  UINT8 Bus;
  UINT8 Dev;
  UINT8 Fun;
} DEV_ID;

//@todo Seems to only be used by Platform/TBT/Smm/TbtSmm.inf
//@todo should refactor this to only be present in that driver
//@todo also definitions like this should never be in a .h file anyway
//@todo this is a quick hack to get things compiling for now
#ifdef __GNUC__
#pragma GCC diagnostic warning "-Wunused-variable"
#endif
static DEV_ID HR_Slots[] = {
  {
    0x00,
    0x1C,
    0x00
  },                                          // PCH slot 0
  {
    0x00,
    0x1C,
    0x01
  },                                          // PCH slot 1
  {
    0x00,
    0x1C,
    0x02
  },                                          // PCH slot 2
  {
    0x00,
    0x1C,
    0x03
  },                                          // PCH slot 3
  {
    0x00,
    0x1C,
    0x04
  },                                          // PCH slot 4
  {
    0x00,
    0x1C,
    0x05
  },                                          // PCH slot 5
  {
    0x00,
    0x1C,
    0x06
  },                                          // PCH slot 6
  {
    0x00,
    0x1C,
    0x07
  },                                          // PCH slot 7
  {
    0x00,
    0x1D,
    0x00
  },                                          // PCH slot 8
  {
    0x00,
    0x1D,
    0x01
  },                                          // PCH slot 9
  {
    0x00,
    0x1D,
    0x02
  },                                          // PCH slot 10
  {
    0x00,
    0x1D,
    0x03
  },                                          // PCH slot 11
  {
    0x00,
    0x1D,
    0x04
  },                                          // PCH slot 12
  {
    0x00,
    0x1D,
    0x05
  },                                          // PCH slot 13
  {
    0x00,
    0x1D,
    0x06
  },                                          // PCH slot 14
  {
    0x00,
    0x1D,
    0x07
  },                                          // PCH slot 15
  {
    0x00,
    0x1B,
    0x00
  },                                          // PCH slot 16
  {
    0x00,
    0x1B,
    0x01
  },                                          // PCH slot 17
  {
    0x00,
    0x1B,
    0x02
  },                                          // PCH slot 18
  {
    0x00,
    0x1B,
    0x03
  },                                          // PCH slot 19
// AMI_OVERRIDE_START - EIP279338 : The resource for PEG Thunderbolt host is incorrect.
  {
    0x00,
    0x1B,
    0x04
  },                                          // PCH slot 20
  {
    0x00,
    0x1B,
    0x05
  },                                          // PCH slot 21
  {
    0x00,
    0x1B,
    0x06
  },                                          // PCH slot 22
  {
    0x00,
    0x1B,
    0x07
  },                                          // PCH slot 23
// AMI_OVERRIDE_END - EIP279338 : The resource for PEG Thunderbolt host is incorrect.
  {
    0x00,
    0x01,
    0x00
  },                                          // PEG0 slot
  {
    0x00,
    0x01,
    0x01
  },                                          // PEG1 slot
  {
    0x00,
    0x01,
    0x02
  },                                          // PEG2 slot
};

// AMI_OVERRIDE_START - Read / Write Host Router Status in NVRAM.
typedef enum {
    Tbt_Fully_Disabled = 2,
    Tbt_Disabled = 0,
    Tbt_Enable
} TBT_FEATURE;

typedef enum {
    Redwood_Ridge = 0,
    Falcon_Ridge,
    BDW_TBT_LP,
    Alpine_Ridge
} TBT_HOST_SERIES;

#pragma pack(1)
typedef struct _AMI_TBT_HR_STATUS_DATA {
  UINT8                    TbtHrSeries;
  UINT8                    TbtHrStatus;
} AMI_TBT_HR_STATUS_DATA;

#pragma pack()
// AMI_OVERRIDE_END - Read / Write Host Router Status in NVRAM.

/**
  Check connected TBT controller is supported or not by DeviceID

  @param[in]  DeviceID              DeviceID of of TBT controller


  @retval     TRUE                  Valid DeviceID
  @retval     FALSE                 Invalid DeviceID
**/

BOOLEAN
IsTbtHostRouter (
  IN    UINT16  DeviceID
  );
/**
  Get Pch/Peg Pcie Root Port Device and Function Number for TBT by Root Port physical Number

  @param[in]  RpNumber              Root port physical number. (0-based)
  @param[out] RpDev                 Return corresponding root port device number.
  @param[out] RpFun                 Return corresponding root port function number.

  @retval     EFI_SUCCESS           Root port device and function is retrieved
**/
EFI_STATUS
EFIAPI
GetTbtRpDevFun(
  IN  UINTN   RpNumber,
  OUT UINTN   *RpDev,
  OUT UINTN   *RpFunc
  );


#endif