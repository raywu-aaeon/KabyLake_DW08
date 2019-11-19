/** @file
  Graphics header file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2016 Intel Corporation.

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
#ifndef _GRAPHICS_INIT_H_
#define _GRAPHICS_INIT_H_

#include <SaPolicyCommon.h>
#include <Ppi/SiPolicy.h>

typedef struct {
  UINT8   Bus;
  UINT8   Device;
  UINT8   Function;
  UINT16  DevenMask;
} PEG_DEVEN;

typedef enum {
  IGD               = 0,
  PEG,
  PCI,
  DISPLAY_DEVICE_MAX
} DISPLAY_DEVICE;

typedef enum {
  VBIOS_DEFAULT     = 0,
  CRT,
  LFP,
  CRT_LFP,
  TV,
  LFPSDVO,
  EFP,
  TVSDVO,
  CRT_LFPSDVO,
  CRT_EFP,
  IGD_BOOT_TYPE_MAX
} IGD_BOOT_TYPE;

typedef enum {
  DSM_0         = 0,
  DSM_32,
  DSM_64,
  DSM_4            = 240,
  DSM_8            = 241,
  DSM_12           = 242,
  DSM_16           = 243,
  DSM_20           = 244,
  DSM_24           = 245,
  DSM_28           = 246,
  DSM_36           = 248,
  DSM_40           = 249,
  DSM_44           = 250,
  DSM_48           = 251,
  DSM_52           = 252,
  DSM_56           = 253,
  DSM_60           = 254
} DATA_STOLEN_MEMORY;

typedef enum {
  GM_32M            = 1,
  GM_64M            = 2,
  GM_128M           = 4,
  GM_MAX
} STOLEN_MEMORY;

typedef enum {
  PAVP_DISABLED     = 0,
  PAVP_LITE,
  PAVP_HIGH
} PAVP_MODE;

#define GTTMMADR_SIZE_4MB           0x400000
#define GTT_SIZE_2MB                2
#define GT_WAIT_TIMEOUT             300000     ///< ~3 seconds in 10us interval
#define GT_CDCLK_TIMEOUT            3000     ///< ~3 ms
#define MAILBOX_WAITTIME            10       ///< ~10 us
#define MAILBOX_TIMEOUT             100      ///< ~100 us
///
/// PAVP Modes
///
#define PAVP_PCM_SIZE_1_MB  1



/**
  Check and Force Vdd On in End Of PEI

  @retval EFI_NOT_READY                   H/W Power Cycle sequence in progress, Vdd on not enabled or Policy not found
  @retval EFI_SUCCESS                     Vdd On already enabled or enabled successfully

**/
EFI_STATUS
EndOfPeiCheckAndForceVddOn (
  VOID
  );

/**
  Check and Force Vdd On in PostMem Phase

  @param[in] GRAPHICS_PEI_PREMEM_CONFIG      GtPreMemConfig
  @param[in] GRAPHICS_PEI_CONFIG             GtConfig

**/
VOID
PostMemCheckAndForceVddOn (
  IN   GRAPHICS_PEI_PREMEM_CONFIG      *GtPreMemConfig,
  IN   GRAPHICS_PEI_CONFIG             *GtConfig
  );

/**
  "Poll Status" for GT Readiness

  @param[in] Base            - Base address of MMIO
  @param[in] Offset          - MMIO Offset
  @param[in] Mask            - Mask
  @param[in] Result          - Value to wait for

**/
VOID
PollGtReady (
  IN       UINT64                  Base,
  IN       UINT32                  Offset,
  IN       UINT32                  Mask,
  IN       UINT32                  Result
  );

/**
  GraphicsInit: Initialize the IGD if no other external graphics is present

  @param[in] GtConfig  - GRAPHICS_PEI_PREMEM_CONFIG to access the GtConfig related information
  @param[in] PrimaryDisplay       - Primary Display - default is IGD
  @param[in, out] PegMmioLength   - Total IGFX MMIO length

**/
VOID
GraphicsInit (
  IN       GRAPHICS_PEI_PREMEM_CONFIG   *GtPreMemConfig,
  IN       DISPLAY_DEVICE               *PrimaryDisplay,
  IN       UINT32                       *IGfxMmioLength
  );

/**
Initialize GT Power management

  @param[in] GRAPHICS_PEI_PREMEM_CONFIG      GtPreMemConfig
  @param[in] GRAPHICS_PEI_CONFIG             GtConfig

  @retval EFI_SUCCESS          - GT Power management initialization complete
**/
EFI_STATUS
GraphicsPmInit (
  IN       GRAPHICS_PEI_PREMEM_CONFIG      *GtPreMemConfig,
  IN       GRAPHICS_PEI_CONFIG             *GtConfig
  );

/**
  Initialize PAVP feature of SystemAgent.

  @param[in] GtConfig      - Instance of GRAPHICS_PEI_CONFIG
  @param[in] MiscPeiConfig - Instance of SA_MISC_PEI_CONFIG


  @retval EFI_SUCCESS     - PAVP initialization complete
  @retval EFI_UNSUPPORTED - iGFX is not present so PAVP not supported
**/
EFI_STATUS
PavpInit (
  IN       GRAPHICS_PEI_CONFIG             *GtConfig,
  IN       SA_MISC_PEI_CONFIG              *MiscPeiConfig
  );

/**
  Program the max Cd Clock supported by the platform

  @param[in] GtConfig            - Instance of GRAPHICS_PEI_CONFIG
  @param[in] GttMmAdr            - Base Address of IGFX MMIO BAR

  @retval EFI_SUCCESS           - GT Power Management initialization complete
  @retval EFI_INVALID_PARAMETER - The input parameter is invalid
**/
EFI_STATUS
CdClkInit (
  IN       GRAPHICS_PEI_CONFIG          *GtConfig,
  IN       UINT32                       GttMmAdr
  );

/**
  ConfigureIDispAudioFrequency: Configures iDisplay Audio BCLK frequency and T-Mode

  @param[in] RequestedBclkFrequency     IDisplay Link clock frequency to be set
  @param[in] RequestedTmode             IDisplay Link T-Mode to be set

  @retval EFI_NOT_FOUND                 SA Policy PPI or GT config block not found, cannot initialize GttMmAdr
  @retval EFI_UNSUPPORTED               iDisp link unsupported frequency
  @retval EFI_SUCCESS                   The function completed successfully
**/
EFI_STATUS
ConfigureIDispAudioFrequency (
  IN       PCH_HDAUDIO_LINK_FREQUENCY  RequestedBclkFrequency,
  IN       PCH_HDAUDIO_IDISP_TMODE     RequestedTmode
  );

/**
  InitializeDisplayAudio: Initialize display engine for iDisplay Audio programming

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_ABORTED             S3 boot - display already initialized
  @retval EFI_UNSUPPORTED         iGfx disabled, iDisplay Audio not present
  @retval EFI_NOT_FOUND           SaPolicy or temporary GTT base address not found
**/
EFI_STATUS
InitializeDisplayAudio (
  VOID
  );
#endif
