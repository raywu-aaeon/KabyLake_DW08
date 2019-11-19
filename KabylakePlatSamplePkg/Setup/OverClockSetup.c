/** @file
  This file implements a Setup callback funtion to update the
  Expected CPU frequency with respect to BusRatio and BusSpeed.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2010 - 2018 Intel Corporation.

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

#include <OverClockSetup.h>
#include <Protocol/SmbusHc.h>
#include <Library/HiiLib.h>
#include <Library/BaseLib.h>
#include <Library/CpuPlatformLib.h>

#define MAX_EFFICIENCY_RATIO_MASK           0xff
#define MAX_EFFICIENCY_RATIO_OFFSET         40
// AMI_OVERRIDE_START - For our Setup variable design.
#include "AmiSetupPrivate.h"
// AMI_OVERRIDE_END - For our Setup variable design.
// AMI_OVERRIDE_START - EIP315416:Overclocking callback for memory timing.
#include <Setup.h>
// AMI_OVERRIDE_END - EIP315416:Overclocking callback for memory timing.

GLOBAL_REMOVE_IF_UNREFERENCED CHAR16 Timing5cString[] = L"  % 5d   % 5d   % 5d   % 5d";
GLOBAL_REMOVE_IF_UNREFERENCED CHAR16 Timing7cString[] = L"% 7d % 7d % 7d % 7d";

// Ratio | OddRatio  | Ratio | OddRatio
// 133   | 133       | 100   | 100
GLOBAL_REMOVE_IF_UNREFERENCED const TRangeTable FreqTable[] = {                                                 //       |(PLL ratio)|       |(PLL ratio)
  { 0xFFFFFFFF,            fInvalid, MRC_FREQ_INVALID                                                        }, //------------------------------------
  { MRC_DDR_800_TCK_MIN,  f800,     MRC_FREQ_133                       | MRC_FREQ_100                       }, //   3   |           |   4   |
  { MRC_DDR_1000_TCK_MIN, f1000,                                         MRC_FREQ_100                       }, //       |           |   5   |
  { MRC_DDR_1067_TCK_MIN, f1067,    MRC_FREQ_133                                                            }, //   4   |           |       |
  { MRC_DDR_1100_TCK_MIN, f1100,                                                     MRC_FREQ_100_ODD_RATIO }, //       |           |       |  5 (11)
  { MRC_DDR_1200_TCK_MIN, f1200,                MRC_FREQ_133_ODD_RATIO | MRC_FREQ_100                       }, //       |   4 (9)   |   6   |
  { MRC_DDR_1300_TCK_MIN, f1300,                                                     MRC_FREQ_100_ODD_RATIO }, //       |           |       |  6 (13)
  { MRC_DDR_1333_TCK_MIN, f1333,    MRC_FREQ_133                                                            }, //   5   |           |       |
  { MRC_DDR_1400_TCK_MIN, f1400,                                         MRC_FREQ_100                       }, //       |           |   7   |
  { MRC_DDR_1467_TCK_MIN, f1467,                MRC_FREQ_133_ODD_RATIO                                      }, //       |   5 (11)  |       |
  { MRC_DDR_1500_TCK_MIN, f1500,                                                     MRC_FREQ_100_ODD_RATIO }, //       |           |       |  7 (15)
  { MRC_DDR_1600_TCK_MIN, f1600,    MRC_FREQ_133                       | MRC_FREQ_100                       }, //   6   |           |   8   |
  { MRC_DDR_1700_TCK_MIN, f1700,                                                     MRC_FREQ_100_ODD_RATIO }, //       |           |       |  8 (17)
  { MRC_DDR_1733_TCK_MIN, f1733,                MRC_FREQ_133_ODD_RATIO                                      }, //       |   6 (13)  |       |
  { MRC_DDR_1800_TCK_MIN, f1800,                                         MRC_FREQ_100                       }, //       |           |   9   |
  { MRC_DDR_1867_TCK_MIN, f1867,    MRC_FREQ_133                                                            }, //   7   |           |       |
  { MRC_DDR_1900_TCK_MIN, f1900,                                                     MRC_FREQ_100_ODD_RATIO }, //       |           |       |  9 (19)
  { MRC_DDR_2000_TCK_MIN, f2000,                MRC_FREQ_133_ODD_RATIO | MRC_FREQ_100                       }, //       |   7 (15)  |  10   |
  { MRC_DDR_2100_TCK_MIN, f2100,                                                     MRC_FREQ_100_ODD_RATIO }, //       |           |       | 10 (21)
  { MRC_DDR_2133_TCK_MIN, f2133,    MRC_FREQ_133                                                            }, //   8   |           |       |
  { MRC_DDR_2200_TCK_MIN, f2200,                                         MRC_FREQ_100                       }, //       |           |  11   |
  { MRC_DDR_2267_TCK_MIN, f2267,                MRC_FREQ_133_ODD_RATIO                                      }, //       |   8 (17)  |       |
  { MRC_DDR_2300_TCK_MIN, f2300,                                                     MRC_FREQ_100_ODD_RATIO }, //       |           |       | 11 (23)
  { MRC_DDR_2400_TCK_MIN, f2400,    MRC_FREQ_133                       | MRC_FREQ_100                       }, //   9   |           |  12   |
  { MRC_DDR_2500_TCK_MIN, f2500,                                                     MRC_FREQ_100_ODD_RATIO }, //       |           |       | 12 (25)
  { MRC_DDR_2533_TCK_MIN, f2533,                MRC_FREQ_133_ODD_RATIO                                      }, //       |   9 (19)  |       |
  { MRC_DDR_2600_TCK_MIN, f2600,                                         MRC_FREQ_100                       }, //       |           |  13   |
  { MRC_DDR_2667_TCK_MIN, f2667,    MRC_FREQ_133                                                            }, //  10   |           |       |
  { MRC_DDR_2700_TCK_MIN, f2700,                                                     MRC_FREQ_100_ODD_RATIO }, //       |           |       | 13 (27)
  { MRC_DDR_2800_TCK_MIN, f2800,                MRC_FREQ_133_ODD_RATIO | MRC_FREQ_100                       }, //       |  10 (21)  |  14   |
  { MRC_DDR_2900_TCK_MIN, f2900,                                                     MRC_FREQ_100_ODD_RATIO }, //       |           |       | 14 (29)
  { MRC_DDR_2933_TCK_MIN, f2933,    MRC_FREQ_133                                                            }, //  11   |           |       |
  { MRC_DDR_3000_TCK_MIN, f3000,                                         MRC_FREQ_100                       }, //       |           |  15   |
  { MRC_DDR_3067_TCK_MIN, f3067,                MRC_FREQ_133_ODD_RATIO                                      }, //       |  11 (23)  |       |
  { MRC_DDR_3100_TCK_MIN, f3100,                                                     MRC_FREQ_100_ODD_RATIO }, //       |           |       | 15 (31)
  { MRC_DDR_3200_TCK_MIN, f3200,    MRC_FREQ_133                       | MRC_FREQ_100                       }, //  12   |           |  16   |
  { MRC_DDR_3300_TCK_MIN, f3300,                                                     MRC_FREQ_100_ODD_RATIO }, //       |           |       | 16 (33)
  { MRC_DDR_3333_TCK_MIN, f3333,                MRC_FREQ_133_ODD_RATIO                                      }, //       |  12 (25)  |       |
  { MRC_DDR_3400_TCK_MIN, f3400,                                         MRC_FREQ_100                       }, //       |           |  17   |
  { MRC_DDR_3467_TCK_MIN, f3467,    MRC_FREQ_133                                                            }, //  13   |           |       |
  { MRC_DDR_3500_TCK_MIN, f3500,                                                     MRC_FREQ_100_ODD_RATIO }, //       |           |       | 17 (35)
  { MRC_DDR_3600_TCK_MIN, f3600,                MRC_FREQ_133_ODD_RATIO | MRC_FREQ_100                       }, //       |  13 (27)  |  18   |
  { MRC_DDR_3700_TCK_MIN, f3700,                                                     MRC_FREQ_100_ODD_RATIO }, //       |           |       | 18 (37)
  { MRC_DDR_3733_TCK_MIN, f3733,    MRC_FREQ_133                                                            }, //  14   |           |       |
  { MRC_DDR_3800_TCK_MIN, f3800,                                         MRC_FREQ_100                       }, //       |           |  19   |
  { MRC_DDR_3867_TCK_MIN, f3867,                MRC_FREQ_133_ODD_RATIO                                      }, //       |  14 (29)  |       |
  { MRC_DDR_3900_TCK_MIN, f3900,                                                     MRC_FREQ_100_ODD_RATIO }, //       |           |       | 19 (39)
  { MRC_DDR_4000_TCK_MIN, f4000,    MRC_FREQ_133                       | MRC_FREQ_100                       }, //  15   |           |  20   |
  { MRC_DDR_4100_TCK_MIN, f4100,                                                     MRC_FREQ_100_ODD_RATIO }, //       |           |       | 20 (41)
  { MRC_DDR_4133_TCK_MIN, f4133,                MRC_FREQ_133_ODD_RATIO                                      }, //       |  15 (31)  |       |
  { MRC_DDR_4200_TCK_MIN, f4200,                                         MRC_FREQ_100                       }, //       |           |  21   |
  { MRC_DDR_4267_TCK_MIN, f4267,    MRC_FREQ_133                                                            }, //  16   |           |       |
  { MRC_DDR_4300_TCK_MIN, f4300,                                                     MRC_FREQ_100_ODD_RATIO }, //       |           |       | 21 (43)
  { MRC_DDR_4400_TCK_MIN, f4400,                MRC_FREQ_133_ODD_RATIO | MRC_FREQ_100                       }, //       |  16 (33)  |  22   |
  { MRC_DDR_4500_TCK_MIN, f4500,                                                     MRC_FREQ_100_ODD_RATIO }, //       |           |       | 22 (45)
  { MRC_DDR_4533_TCK_MIN, f4533,    MRC_FREQ_133                                                            }, //  17   |           |       |
  { MRC_DDR_4600_TCK_MIN, f4600,                                         MRC_FREQ_100                       }, //       |           |  23   |
  { MRC_DDR_4667_TCK_MIN, f4667,                MRC_FREQ_133_ODD_RATIO                                      }, //       |  17 (35)  |       |
  { MRC_DDR_4700_TCK_MIN, f4700,                                                     MRC_FREQ_100_ODD_RATIO }, //       |           |       | 23 (47)
  { MRC_DDR_4800_TCK_MIN, f4800,    MRC_FREQ_133                       | MRC_FREQ_100                       }, //  18   |           |  24   |
  { MRC_DDR_4900_TCK_MIN, f4900,                                                     MRC_FREQ_100_ODD_RATIO }, //       |           |       | 24 (49)
  { MRC_DDR_4933_TCK_MIN, f4933,                MRC_FREQ_133_ODD_RATIO                                      }, //       |  18 (37)  |       |
  { MRC_DDR_5000_TCK_MIN, f5000,                                         MRC_FREQ_100                       }, //       |           |  25   |
  { MRC_DDR_5067_TCK_MIN, f5067,    MRC_FREQ_133                                                            }, //  19   |           |       |
  { MRC_DDR_5100_TCK_MIN, f5100,                                                     MRC_FREQ_100_ODD_RATIO }, //       |           |       | 25 (51)
  { MRC_DDR_5200_TCK_MIN, f5200,                MRC_FREQ_133_ODD_RATIO | MRC_FREQ_100                       }, //       |  19 (39)  |  26   |
  { MRC_DDR_5300_TCK_MIN, f5300,                                                     MRC_FREQ_100_ODD_RATIO }, //       |           |       | 26 (53)
  { MRC_DDR_5333_TCK_MIN, f5333,    MRC_FREQ_133                                                            }, //  20   |           |       |
  { MRC_DDR_5400_TCK_MIN, f5400,                                         MRC_FREQ_100                       }, //       |           |  27   |
  { MRC_DDR_5467_TCK_MIN, f5467,                MRC_FREQ_133_ODD_RATIO                                      }, //       |  20 (41)  |       |
  { MRC_DDR_5500_TCK_MIN, f5500,                                                     MRC_FREQ_100_ODD_RATIO }, //       |           |       | 27 (55)
  { MRC_DDR_5600_TCK_MIN, f5600,    MRC_FREQ_133                       | MRC_FREQ_100                       }, //  21   |           |  28   |
  { MRC_DDR_5700_TCK_MIN, f5700,                                                     MRC_FREQ_100_ODD_RATIO }, //       |           |       | 28 (57)
  { MRC_DDR_5733_TCK_MIN, f5733,                MRC_FREQ_133_ODD_RATIO                                      }, //       |  21 (43)  |       |
  { MRC_DDR_5800_TCK_MIN, f5800,                                         MRC_FREQ_100                       }, //       |           |  29   |
  { MRC_DDR_5867_TCK_MIN, f5867,    MRC_FREQ_133                                                            }, //  22   |           |       |
  { MRC_DDR_5900_TCK_MIN, f5900,                                                     MRC_FREQ_100_ODD_RATIO }, //       |           |       | 29 (59)
  { MRC_DDR_6000_TCK_MIN, f6000,                MRC_FREQ_133_ODD_RATIO | MRC_FREQ_100                       }, //       |  22 (45)  |  30   |
  { MRC_DDR_6100_TCK_MIN, f6100,                                                     MRC_FREQ_100_ODD_RATIO }, //       |           |       | 30 (61)
  { MRC_DDR_6133_TCK_MIN, f6133,    MRC_FREQ_133                                                            }, //  23   |           |       |
  { MRC_DDR_6200_TCK_MIN, f6200,                                         MRC_FREQ_100                       }, //       |           |  31   |
  { MRC_DDR_6267_TCK_MIN, f6267,                MRC_FREQ_133_ODD_RATIO                                      }, //       |  23 (47)  |       |
  { MRC_DDR_6300_TCK_MIN, f6300,                                                     MRC_FREQ_100_ODD_RATIO }, //       |           |       | 31 (63)
  { MRC_DDR_6400_TCK_MIN, f6400,    MRC_FREQ_133                                                            }, //  24   |           |       |
  { MRC_DDR_6533_TCK_MIN, f6533,                MRC_FREQ_133_ODD_RATIO                                      }, //       |  24 (49)  |       |
  { MRC_DDR_6667_TCK_MIN, f6667,    MRC_FREQ_133                                                            }, //  25   |           |       |
  { MRC_DDR_6800_TCK_MIN, f6800,                MRC_FREQ_133_ODD_RATIO                                      }, //       |  25 (51)  |       |
  { MRC_DDR_6933_TCK_MIN, f6933,    MRC_FREQ_133                                                            }, //  26   |           |       |
  { MRC_DDR_7067_TCK_MIN, f7067,                MRC_FREQ_133_ODD_RATIO                                      }, //       |  26 (53)  |       |
  { MRC_DDR_7200_TCK_MIN, f7200,    MRC_FREQ_133                                                            }, //  27   |           |       |
  { MRC_DDR_7333_TCK_MIN, f7333,                MRC_FREQ_133_ODD_RATIO                                      }, //       |  27 (55)  |       |
  { MRC_DDR_7467_TCK_MIN, f7467,    MRC_FREQ_133                                                            }, //  28   |           |       |
  { MRC_DDR_7600_TCK_MIN, f7600,                MRC_FREQ_133_ODD_RATIO                                      }, //       |  28 (57)  |       |
  { MRC_DDR_7733_TCK_MIN, f7733,    MRC_FREQ_133                                                            }, //  29   |           |       |
  { MRC_DDR_7867_TCK_MIN, f7867,                MRC_FREQ_133_ODD_RATIO                                      }, //       |  29 (59)  |       |
  { MRC_DDR_8000_TCK_MIN, f8000,    MRC_FREQ_133                                                            }, //  30   |           |       |
  { MRC_DDR_8133_TCK_MIN, f8133,                MRC_FREQ_133_ODD_RATIO                                      }, //       |  30 (61)  |       |
  { MRC_DDR_8267_TCK_MIN, f8267,    MRC_FREQ_133                                                            }, //  31   |           |       |
  { MRC_DDR_8400_TCK_MIN, f8400,                MRC_FREQ_133_ODD_RATIO                                      }, //       |  31 (63)  |       |
  { 0,                    fNoInit,  MRC_FREQ_INVALID                                                        }
};

// AMI_OVERRIDE_START - For our Callback function design.
EFI_STATUS
EFIAPI
OcFormCallBackFunction (
  AMI_HII_CALLBACK_PARAMETERS             *CallbackValues
);
// AMI_OVERRIDE_END - For our Callback function design.

//#ifdef OVERCLOCK_FLAG   // AMI_OVERRIDE_START - We don't use this feature flag.
/**
  This funciton converts the tCK value in femtoseconds to MHz.

  @param[in]      tCKvalue       - The tCK value to be converted to MHz.

  @retval tCkvalue in MHz
**/
UINT32
ConvertFs2Mhz (
  UINT32        tCKvalue
  )
{
  UINT32    tCKMhz;

  tCKMhz = (tCKvalue == 0) ? 0 : (2000000000 + (tCKvalue >> 1)) / tCKvalue;

  return tCKMhz;
}
//#endif   // AMI_OVERRIDE_END - We don't use this feature flag.

/**
  Installs OverClock Callback function.

  @param[in]  HiiHandle      Handle to HII database
  @param[in]  Class          Indicates the setup class
**/
VOID
InitOverClockStrings (
  EFI_HII_HANDLE        HiiHandle,
  UINT16                Class
  )
{
//#ifdef OVERCLOCK_FLAG  // AMI_OVERRIDE_START - We don't use this feature flag.
  EFI_STATUS            Status;
  UINT32                MailboxStatus;
  SA_SETUP              SaSetup;
  SETUP_DATA            SetupData;
  MEM_INFO_PROTOCOL     *MemInfoHobProtocol;
  UINTN                 VariableSize;
  MEMORY_INFO_DATA      *MemInfoData;
  MEMORY_TIMING         *MemoryTiming;
  UINT32                VariableAttr;
  UINT32                PcodeBclkFrequency;
  CPU_SKU               CpuSku;
  UINT32                tCKvalues[MEMORY_PROFILE_MAX];
  UINT8                 Index;

  if (Class != ADVANCED_FORM_SET_CLASS) {
    return;
  }

  DEBUG ((DEBUG_INFO, "<InitOverClockStrings>"));

  //
  // Get the pcode derived BCLK frequency in kHz. Message is only valid on DT/Halo skus.
  //
  CpuSku = GetCpuSku();
  if ((CpuSku == EnumCpuHalo) || (CpuSku == EnumCpuTrad)) {

    MailboxRead(MAILBOX_TYPE_OC, MAILBOX_BCLK_FREQUENCY_CMD, &PcodeBclkFrequency, &MailboxStatus);
    DEBUG ((DEBUG_INFO, "(OC) Pcode BCLK frequency = %X", PcodeBclkFrequency));

    //
    // Initialize BCLK Frequency String in MHz
    //
    InitString(HiiHandle, STRING_TOKEN(STR_PCODE_BCLK_FREQUENCY_VALUE), L"%d.%02d MHz",
      PcodeBclkFrequency/1000, PcodeBclkFrequency%1000);
  }

  //
  // Get the Memory Info HOB Protocol if it exists.
  //
  Status = gBS->LocateProtocol (&gMemInfoProtocolGuid, NULL, (VOID **) &MemInfoHobProtocol);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return;
  }

  MemInfoData  = &MemInfoHobProtocol->MemInfoData;
  MemoryTiming = MemInfoData->Timing;

  for (Index = 0; Index < MEMORY_PROFILE_MAX; Index++) {
    if (Index < XMP_PROFILE_1) {
      tCKvalues[Index] = ConvertFs2Mhz (MemoryTiming[Index].tCK);
    } else {
      tCKvalues[Index] = ConvertFs2Mhz (MemInfoData->DefaultXmptCK[Index - XMP_PROFILE_1]);
    }
  }

  InitString (
    HiiHandle,
    STRING_TOKEN (STR_MEMORY_TIMING_TCK_VALUE),
    Timing7cString,
    tCKvalues[DEFAULT_SPD_PROFILE],
    tCKvalues[CUSTOM_PROFILE],
    tCKvalues[XMP_PROFILE_1],
    tCKvalues[XMP_PROFILE_2]
    );

  InitString (
    HiiHandle,
    STRING_TOKEN (STR_MEMORY_TIMING_TCL_VALUE),
    Timing5cString,
    MemoryTiming[DEFAULT_SPD_PROFILE].tCL,
    MemoryTiming[CUSTOM_PROFILE].tCL,
    MemoryTiming[XMP_PROFILE_1].tCL,
    MemoryTiming[XMP_PROFILE_2].tCL
    );

  InitString (
    HiiHandle,
    STRING_TOKEN (STR_MEMORY_TIMING_TRCD_TRP_VALUE),
    Timing5cString,
    MemoryTiming[DEFAULT_SPD_PROFILE].tRCDtRP,
    MemoryTiming[CUSTOM_PROFILE].tRCDtRP,
    MemoryTiming[XMP_PROFILE_1].tRCDtRP,
    MemoryTiming[XMP_PROFILE_2].tRCDtRP
    );

  InitString (
    HiiHandle,
    STRING_TOKEN (STR_MEMORY_TIMING_TRAS_VALUE),
    Timing5cString,
    MemoryTiming[DEFAULT_SPD_PROFILE].tRAS,
    MemoryTiming[CUSTOM_PROFILE].tRAS,
    MemoryTiming[XMP_PROFILE_1].tRAS,
    MemoryTiming[XMP_PROFILE_2].tRAS
    );

  InitString (
    HiiHandle,
    STRING_TOKEN (STR_MEMORY_TIMING_TCWL_VALUE),
    Timing5cString,
    MemoryTiming[DEFAULT_SPD_PROFILE].tCWL,
    MemoryTiming[CUSTOM_PROFILE].tCWL,
    MemoryTiming[XMP_PROFILE_1].tCWL,
    MemoryTiming[XMP_PROFILE_2].tCWL
    );

  InitString (
    HiiHandle,
    STRING_TOKEN (STR_MEMORY_TIMING_TFAW_VALUE),
    Timing5cString,
    MemoryTiming[DEFAULT_SPD_PROFILE].tFAW,
    MemoryTiming[CUSTOM_PROFILE].tFAW,
    MemoryTiming[XMP_PROFILE_1].tFAW,
    MemoryTiming[XMP_PROFILE_2].tFAW
    );

  InitString (
    HiiHandle,
    STRING_TOKEN (STR_MEMORY_TIMING_TREFI_VALUE),
    Timing5cString,
    MemoryTiming[DEFAULT_SPD_PROFILE].tREFI,
    MemoryTiming[CUSTOM_PROFILE].tREFI,
    MemoryTiming[XMP_PROFILE_1].tREFI,
    MemoryTiming[XMP_PROFILE_2].tREFI
    );

  InitString (
    HiiHandle,
    STRING_TOKEN (STR_MEMORY_TIMING_TRFC_VALUE),
    Timing5cString,
    MemoryTiming[DEFAULT_SPD_PROFILE].tRFC,
    MemoryTiming[CUSTOM_PROFILE].tRFC,
    MemoryTiming[XMP_PROFILE_1].tRFC,
    MemoryTiming[XMP_PROFILE_2].tRFC
    );

  if (MemInfoData->DdrType == MRC_DDR_TYPE_DDR4) {
    InitString (
      HiiHandle,
      STRING_TOKEN (STR_MEMORY_TIMING_TRRD_VALUE),
      Timing5cString,
      MemInfoData->tRRD_L[DEFAULT_SPD_PROFILE],
      MemInfoData->tRRD_L[CUSTOM_PROFILE],
      MemInfoData->tRRD_L[XMP_PROFILE_1],
      MemInfoData->tRRD_L[XMP_PROFILE_2]
      );

    InitString (
      HiiHandle,
      STRING_TOKEN (STR_MEMORY_TIMING_TRRD_S_VALUE),
      Timing5cString,
      MemInfoData->tRRD_S[DEFAULT_SPD_PROFILE],
      MemInfoData->tRRD_S[CUSTOM_PROFILE],
      MemInfoData->tRRD_S[XMP_PROFILE_1],
      MemInfoData->tRRD_S[XMP_PROFILE_2]
      );

    InitString (
      HiiHandle,
      STRING_TOKEN (STR_MEMORY_TIMING_TWTR_VALUE),
      Timing5cString,
      MemInfoData->tWTR_L[DEFAULT_SPD_PROFILE],
      MemInfoData->tWTR_L[CUSTOM_PROFILE],
      MemInfoData->tWTR_L[XMP_PROFILE_1],
      MemInfoData->tWTR_L[XMP_PROFILE_2]
      );

    InitString (
      HiiHandle,
      STRING_TOKEN (STR_MEMORY_TIMING_TWTR_S_VALUE),
      Timing5cString,
      MemInfoData->tWTR_S[DEFAULT_SPD_PROFILE],
      MemInfoData->tWTR_S[CUSTOM_PROFILE],
      MemInfoData->tWTR_S[XMP_PROFILE_1],
      MemInfoData->tWTR_S[XMP_PROFILE_2]
      );
  } else {
    InitString (
      HiiHandle,
      STRING_TOKEN (STR_MEMORY_TIMING_TRRD_VALUE),
      Timing5cString,
      MemoryTiming[DEFAULT_SPD_PROFILE].tRRD,
      MemoryTiming[CUSTOM_PROFILE].tRRD,
      MemoryTiming[XMP_PROFILE_1].tRRD,
      MemoryTiming[XMP_PROFILE_2].tRRD
      );

    InitString (
      HiiHandle,
      STRING_TOKEN (STR_MEMORY_TIMING_TWTR_VALUE),
      Timing5cString,
      MemoryTiming[DEFAULT_SPD_PROFILE].tWTR,
      MemoryTiming[CUSTOM_PROFILE].tWTR,
      MemoryTiming[XMP_PROFILE_1].tWTR,
      MemoryTiming[XMP_PROFILE_2].tWTR
      );
  }

  InitString (
    HiiHandle,
    STRING_TOKEN (STR_MEMORY_TIMING_TRTP_VALUE),
    Timing5cString,
    MemoryTiming[DEFAULT_SPD_PROFILE].tRTP,
    MemoryTiming[CUSTOM_PROFILE].tRTP,
    MemoryTiming[XMP_PROFILE_1].tRTP,
    MemoryTiming[XMP_PROFILE_2].tRTP
    );

  InitString (
    HiiHandle,
    STRING_TOKEN (STR_MEMORY_TIMING_TWR_VALUE),
    Timing5cString,
    MemoryTiming[DEFAULT_SPD_PROFILE].tWR,
    MemoryTiming[CUSTOM_PROFILE].tWR,
    MemoryTiming[XMP_PROFILE_1].tWR,
    MemoryTiming[XMP_PROFILE_2].tWR
    );

  InitString (
    HiiHandle,
    STRING_TOKEN (STR_MEMORY_TIMING_NMODE_VALUE),
    Timing5cString,
    MemoryTiming[DEFAULT_SPD_PROFILE].NMode,
    MemoryTiming[CUSTOM_PROFILE].NMode,
    MemoryTiming[XMP_PROFILE_1].NMode,
    MemoryTiming[XMP_PROFILE_2].NMode
    );

  InitString (
    HiiHandle,
    STRING_TOKEN (STR_MEMORY_VOLTAGE_VDD_VALUE),
    Timing5cString,
    MemInfoData->VddVoltage[DEFAULT_SPD_PROFILE],
    MemInfoData->VddVoltage[CUSTOM_PROFILE],
    MemInfoData->VddVoltage[XMP_PROFILE_1],
    MemInfoData->VddVoltage[XMP_PROFILE_2]
    );
  if ((MemInfoData->DimmStatus[0] != DIMM_NOT_PRESENT) || (MemInfoData->DimmStatus[1] != DIMM_NOT_PRESENT)) {
    // Update Turnaround Timing string values with data from MemInfo protocol
    InitString (
      HiiHandle,
      STRING_TOKEN (STR_TURNAROUNDTIMING_RD2RD_CH0_VALUE),
      Timing5cString,
      MemInfoData->tRd2Rd.SG,
      MemInfoData->tRd2Rd.DG,
      MemInfoData->tRd2Rd.DR,
      MemInfoData->tRd2Rd.DD
      );

    InitString (
      HiiHandle,
      STRING_TOKEN (STR_TURNAROUNDTIMING_RD2WR_CH0_VALUE),
      Timing5cString,
      MemInfoData->tRd2Wr.SG,
      MemInfoData->tRd2Wr.DG,
      MemInfoData->tRd2Wr.DR,
      MemInfoData->tRd2Wr.DD
      );

    InitString (
      HiiHandle,
      STRING_TOKEN (STR_TURNAROUNDTIMING_WR2RD_CH0_VALUE),
      Timing5cString,
      MemInfoData->tWr2Rd.SG,
      MemInfoData->tWr2Rd.DG,
      MemInfoData->tWr2Rd.DR,
      MemInfoData->tWr2Rd.DD
      );

    InitString (
      HiiHandle,
      STRING_TOKEN (STR_TURNAROUNDTIMING_WR2WR_CH0_VALUE),
      Timing5cString,
      MemInfoData->tWr2Wr.SG,
      MemInfoData->tWr2Wr.DG,
      MemInfoData->tWr2Wr.DR,
      MemInfoData->tWr2Wr.DD
      );
  }
  if ((MemInfoData->DimmStatus[2] != DIMM_NOT_PRESENT) || (MemInfoData->DimmStatus[3] != DIMM_NOT_PRESENT)) {
    InitString (
      HiiHandle,
      STRING_TOKEN (STR_TURNAROUNDTIMING_RD2RD_CH1_VALUE),
      Timing5cString,
      MemInfoData->tRd2Rd_Ch1.SG,
      MemInfoData->tRd2Rd_Ch1.DG,
      MemInfoData->tRd2Rd_Ch1.DR,
      MemInfoData->tRd2Rd_Ch1.DD
      );

    InitString (
      HiiHandle,
      STRING_TOKEN (STR_TURNAROUNDTIMING_RD2WR_CH1_VALUE),
      Timing5cString,
      MemInfoData->tRd2Wr_Ch1.SG,
      MemInfoData->tRd2Wr_Ch1.DG,
      MemInfoData->tRd2Wr_Ch1.DR,
      MemInfoData->tRd2Wr_Ch1.DD
      );

    InitString (
      HiiHandle,
      STRING_TOKEN (STR_TURNAROUNDTIMING_WR2RD_CH1_VALUE),
      Timing5cString,
      MemInfoData->tWr2Rd_Ch1.SG,
      MemInfoData->tWr2Rd_Ch1.DG,
      MemInfoData->tWr2Rd_Ch1.DR,
      MemInfoData->tWr2Rd_Ch1.DD
      );

    InitString (
      HiiHandle,
      STRING_TOKEN (STR_TURNAROUNDTIMING_WR2WR_CH1_VALUE),
      Timing5cString,
      MemInfoData->tWr2Wr_Ch1.SG,
      MemInfoData->tWr2Wr_Ch1.DG,
      MemInfoData->tWr2Wr_Ch1.DR,
      MemInfoData->tWr2Wr_Ch1.DD
      );
  }


  VariableSize = sizeof (SETUP_DATA);
  Status = gRT->GetVariable (
                  L"Setup",
                  &gSetupVariableGuid,
                  &VariableAttr,
                  &VariableSize,
                  &SetupData
                  );
  ASSERT_EFI_ERROR (Status);

  VariableSize = sizeof (SA_SETUP);
  Status = gRT->GetVariable (
                  L"SaSetup",
                  &gSaSetupVariableGuid,
                  &VariableAttr,
                  &VariableSize,
                  &SaSetup
                  );
  ASSERT_EFI_ERROR (Status);
  //
  // Store the Factory configured values if IsOcDefaultsInitalized == 0.
  // IsOcDefaultsInitalized will be 0 :
  //    1. During first boot
  //    2. If User goes into BIOS setup and selects: Restore Defaults
  //
  if (!EFI_ERROR (Status)) {
    if (SetupData.IsOcDefaultsInitalized == 0) {
      //
      // Loading the default values from the MemInfoHobProtocol, using the currently running profile
      //
      MemoryTiming         = &MemInfoData->Timing[MemInfoData->Profile];
      SaSetup.tCL          = (UINT8) MemoryTiming->tCL;
      SaSetup.tCWL         = (UINT8) MemoryTiming->tCWL;
      SaSetup.tFAW         = MemoryTiming->tFAW;
      SaSetup.tRAS         = MemoryTiming->tRAS;
      SaSetup.tRCDtRP      = (UINT8) MemoryTiming->tRCDtRP;
      SaSetup.tREFI        = MemoryTiming->tREFI;
      SaSetup.tRFC         = MemoryTiming->tRFC;
      SaSetup.tRRD         = (UINT8) MemoryTiming->tRRD;
      SaSetup.tRTP         = (UINT8) MemoryTiming->tRTP;
      SaSetup.tWR          = (UINT8) MemoryTiming->tWR;
      SaSetup.tWTR         = (UINT8) MemoryTiming->tWTR;
      SaSetup.tWTR_L       = (UINT8) MemInfoData->tWTR_L[MemInfoData->Profile];
      SaSetup.tWTR_S       = (UINT8) MemInfoData->tWTR_S[MemInfoData->Profile];
      SaSetup.tRRD_L       = (UINT8) MemInfoData->tRRD_L[MemInfoData->Profile];
      SaSetup.tRRD_S       = (UINT8) MemInfoData->tRRD_S[MemInfoData->Profile];
      SaSetup.DdrRatio     = MemInfoData->Ratio;
      SaSetup.DdrRefClk    = MemInfoData->RefClk;

      SaSetup.MemoryVoltage = MemInfoData->VddVoltage[MemInfoData->Profile];

      SaSetup.tCLDefault      = SaSetup.tCL;
      SaSetup.tCWLDefault     = SaSetup.tCWL;
      SaSetup.tFAWDefault     = SaSetup.tFAW;
      SaSetup.tRASDefault     = SaSetup.tRAS;
      SaSetup.tRCDtRPDefault  = SaSetup.tRCDtRP;
      SaSetup.tREFIDefault    = SaSetup.tREFI;
      SaSetup.tRFCDefault     = SaSetup.tRFC;
      SaSetup.tRRDDefault     = SaSetup.tRRD;
      SaSetup.tRRDLDefault    = SaSetup.tRRD_L;
      SaSetup.tRRDSDefault    = SaSetup.tRRD_S;
      SaSetup.tRTPDefault     = SaSetup.tRTP;
      SaSetup.tWRDefault      = SaSetup.tWR;
      SaSetup.tWTRDefault     = SaSetup.tWTR;
      SaSetup.tWTRLDefault    = SaSetup.tWTR_L;
      SaSetup.tWTRSDefault    = SaSetup.tWTR_S;
      SaSetup.NModeDefault    = 0;
    } else {
      SaSetup.SpdProfileSelected = MemInfoData->Profile;
    }
    Status = gRT->SetVariable (
                  L"SaSetup",
                  &gSaSetupVariableGuid,
                  VariableAttr,
                  sizeof (SA_SETUP),
                  &SaSetup
                  );
    ASSERT_EFI_ERROR (Status);
  }
//#endif  // AMI_OVERRIDE_END - We don't use this feature flag.
// AMI_OVERRIDE_START - For AMI TSE callback design
  AmiSetupRegisterCallbackHandler (KEY_CORE_MAX_OC_RATIO_LIMIT, OcFormCallBackFunction);
  AmiSetupRegisterCallbackHandler (KEY_RING_MIN_OC_RATIO_LIMIT, OcFormCallBackFunction);
  AmiSetupRegisterCallbackHandler (KEY_RING_MAX_OC_RATIO_LIMIT, OcFormCallBackFunction);
  AmiSetupRegisterCallbackHandler (KEY_RatioLimit1, OcFormCallBackFunction);
  AmiSetupRegisterCallbackHandler (KEY_CPU_OVER_CLOCKING, OcFormCallBackFunction);
  //AmiSetupRegisterCallbackHandler (KEY_TPV_GFX_OVER_CLOCKING, OcFormCallBackFunction); Not use yet.
// AMI_OVERRIDE_END - For AMI TSE callback design
}

/**
  This funciton uses the tCK value to choose a reference clock to a clock ratio.

  @param[in]      tCKvalue              - The tCK value to be converted to frequency ratio.
  @param[in]      BclkFrequency         - The current Bclk Frequency.
  @param[in]      ExtendedDdrOverclock  - Indicates what range of possible Memory Ratios are possible.
  @param[in,out]  RefClk                - Used as return value for chosen Reference Clock.
  @param[in,out]  Ratio                 - Used as return value for chosen Ratio.
  @param[in,out]  OddRatioMode          - Used as return value for Odd Ratio Mode if applicable

  @retval Nothing
**/
VOID
ConvertClock2FreqRatio (
  IN     UINT32               tCKvalue,
  IN     UINT32               BclkFrequency,
  IN     BOOLEAN              ExtendedDdrOverclock,
  IN OUT UINT8                *RefClk,
  IN OUT UINT8                *Ratio,
  IN OUT UINT8                *OddRatioMode
  )
{
  MrcFrequency  Frequency;
  UINT32        Index;
  UINT8         FreqFlag;
  UINT64        RatioValue;
  UINT64        FreqValue;
  UINT32        RefClkValue;
  UINT32        BClkValue;
  UINT32        TableSize;
  UINT8         MemoryRatioMax;

  MemoryRatioMax = ExtendedDdrOverclock ? MEMORY_EXTENDED_RATIO_MAX : MEMORY_RATIO_MAX;

  // Set RefClk and Ratio to 0 (Auto) and OddRatioMode to 0 (Disabled)
  *RefClk       = 0;
  *Ratio        = 0;
  *OddRatioMode = 0;

  // Convert tCK value to the nearest frequency value.
  // Then find slowest valid frequency for the given reference clock.
  Frequency = fNoInit;
  TableSize = ExtendedDdrOverclock ? (ARRAY_COUNT (FreqTable)) : FREQ_TABLE_INDEX_MEMORY_RATIO_MAX;
  for (Index = 0; Index < (TableSize - 1); Index++) {
    if ((tCKvalue <= FreqTable[Index].tCK) && (tCKvalue > FreqTable[Index + 1].tCK)) {
      Frequency = FreqTable[Index].Frequency;
      break;
    }
  }
  FreqFlag = FreqTable[Index].FreqFlag;

  // If the Frequency is not found in the table, return without changing RefClk, Ratio and OddRatioMode values.
  if (FreqFlag == MRC_FREQ_INVALID) {
    return;
  }

  for (Index = 0; Index < MRC_REF_CLOCK_MAXIMUM; Index++) {
    if ((FreqFlag & (MRC_BIT0 << Index)) != 0) {
      *RefClk = (Index == MRC_REF_CLOCK_133) ? MRC_FREQ_133 : MRC_FREQ_100;
      *OddRatioMode = 0;
      break; // We can have this frequency at the given refclk
    }
    if ((FreqFlag & (MRC_BIT2 << Index)) != 0) {
      *RefClk = (Index == MRC_REF_CLOCK_133) ? MRC_FREQ_133 : MRC_FREQ_100;
      *OddRatioMode = 1;
      break; // We can have this frequency at the given refclk using Odd Ratio mode
    }
  }

  BClkValue   = (BclkFrequency == 0) ? (BCLK_DEFAULT / 100000) : (BclkFrequency / 100000);
  RefClkValue = (*RefClk == MRC_FREQ_100) ? 200000 : 266667;
  FreqValue   = MultU64x32 (Frequency, 1000000000ULL);
  RatioValue  = DivU64x64Remainder (FreqValue, (RefClkValue * BClkValue), NULL);
  RatioValue  = ((UINT32) RatioValue + 500) / 1000;

  if ((MEMORY_RATIO_MIN <= RatioValue) && (MemoryRatioMax >= RatioValue)) {
    *Ratio = (MrcClockRatio) RatioValue;
  }
  return;
}

/**
  This function displays the Expected CPU Frequency based on Setup
  BusRatio and BusSpeed.

  @param This            Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Action          Specifies the type of action taken by the browser.
  @param QuestionId      A unique value which is sent to the original exporting driver
                         so that it can identify the type of data to expect.
  @param Type            The type of value for the question.
  @param Value           A pointer to the data being sent to the original exporting driver.
  @param ActionRequest   On return, points to the action requested by the callback function.

  @retval EFI_SUCCESS       The function completed successfully.
**/
EFI_STATUS
EFIAPI
OcFormCallBackFunction (
#if 0 // AMI_OVERRIDE_START - For TSE Callback function design.
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_BROWSER_ACTION                   Action,
  IN EFI_QUESTION_ID                      KeyValue,
  IN UINT8                                Type,
  IN EFI_IFR_TYPE_VALUE                   *Value,
  OUT EFI_BROWSER_ACTION_REQUEST          *ActionRequest
#else  
  AMI_HII_CALLBACK_PARAMETERS 		      *CallbackValues
#endif // AMI_OVERRIDE_END - For TSE Callback function design.
  )
{
  CPU_SETUP               *CpuSetup;
  UINT8                   LfmRatio;
  MSR_REGISTER            MsrPlatformInfo;
  UINTN                   VarSize;
#if 0 // AMI_OVERRIDE_START - No use  
  UINTN                   VariableSize;
#endif // AMI_OVERRIDE_END - No use    
  EFI_STATUS              Status;
  EFI_STRING              RequestString = NULL;
  EFI_STRING              SetupRequestString = NULL;
  // AMI_OVERRIDE_START - For AMI setup design.
  //SA_SETUP                *SaSetup;
  //SETUP_DATA              *SetupData;
  // AMI_OVERRIDE_END - For AMI setup design.
// AMI_OVERRIDE_START - For TSE Callback function design.
  EFI_BROWSER_ACTION      Action;
  		    
  Action = CallbackValues->Action;
// AMI_OVERRIDE_END - For TSE Callback function design.

  if (Action != EFI_BROWSER_ACTION_CHANGING && Action != EFI_BROWSER_ACTION_CHANGED) {
    return EFI_UNSUPPORTED;
  }
#if 0 // AMI_OVERRIDE_START - For AMI setup design.
  VarSize  = sizeof (CPU_SETUP);
#else
  VarSize  = sizeof (SETUP_DATA);
#endif // AMI_OVERRIDE_END - For AMI setup design.
  CpuSetup = AllocatePool(VarSize);
  ASSERT (CpuSetup != NULL);
  if (CpuSetup == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  // GetBrowserData by VarStore Name (Setup)
#if 0 // AMI_OVERRIDE_START - For AMI setup design.
  Status = HiiGetBrowserData(&gCpuSetupVariableGuid, L"CpuSetup", VarSize, (UINT8 *) CpuSetup);
  ASSERT_EFI_ERROR(Status);
  
  VarSize  = sizeof (SETUP_DATA);
  SetupData = AllocatePool(VarSize);
  ASSERT (SetupData != NULL);
  if (SetupData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  
  Status = HiiGetBrowserData(&gSetupVariableGuid, L"Setup", VarSize, (UINT8 *) SetupData);
  ASSERT_EFI_ERROR(Status);
  
  VarSize  = sizeof (SA_SETUP);
  SaSetup = AllocatePool(VarSize);
  ASSERT (SaSetup != NULL);
  if (SaSetup == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  
  Status = HiiGetBrowserData(&gSaSetupVariableGuid, L"SaSetup", VarSize, (UINT8 *) SaSetup);
#else
  Status = HiiGetBrowserData(&gSetupVariableGuid, L"Setup", VarSize, (UINT8 *) CpuSetup);
#endif // AMI_OVERRIDE_END - For AMI setup design.
  ASSERT_EFI_ERROR(Status);

  MsrPlatformInfo.Qword    = AsmReadMsr64 (MSR_PLATFORM_INFO);
  LfmRatio                 = (UINT8) (RShiftU64 (MsrPlatformInfo.Qword, MAX_EFFICIENCY_RATIO_OFFSET)) & MAX_EFFICIENCY_RATIO_MASK;

#if 0 // AMI_OVERRIDE_START - No use  
  VariableSize = sizeof (CPU_SETUP);
  Status = gRT->GetVariable (
                  L"CpuSetup",
                  &gCpuSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &mCpuSetup
                  );
  ASSERT_EFI_ERROR (Status);
#endif // AMI_OVERRIDE_END - No use      
#if 0 // AMI_OVERRIDE_START - For AMI setup design.
  switch (KeyValue) {
#else
  switch (CallbackValues->QuestionId) {
#endif // AMI_OVERRIDE_END - For AMI setup design.
    case KEY_CORE_MAX_OC_RATIO_LIMIT:
      //
      // Ensure that max oc ratio limit does not go below the LFM ratio
      //
      if ((CpuSetup->CoreMaxOcRatio < LfmRatio) && (CpuSetup->CoreMaxOcRatio != 0)) {
        CpuSetup->CoreMaxOcRatio = LfmRatio;
      }
// AMI_OVERRIDE_START - CoreMaxOcRatio sync with RatioLimit
      CpuSetup->RatioLimit1 = CpuSetup->CoreMaxOcRatio;
      CpuSetup->RatioLimit2 = CpuSetup->CoreMaxOcRatio;
      CpuSetup->RatioLimit3 = CpuSetup->CoreMaxOcRatio;
      CpuSetup->RatioLimit4 = CpuSetup->CoreMaxOcRatio;
      CpuSetup->RatioLimit5 = CpuSetup->CoreMaxOcRatio;
      CpuSetup->RatioLimit6 = CpuSetup->CoreMaxOcRatio;
      CpuSetup->RatioLimit7 = CpuSetup->CoreMaxOcRatio;
      CpuSetup->RatioLimit8 = CpuSetup->CoreMaxOcRatio;
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, CoreMaxOcRatio), sizeof (CpuSetup->CoreMaxOcRatio));
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, RatioLimit1), sizeof (CpuSetup->RatioLimit1));
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, RatioLimit2), sizeof (CpuSetup->RatioLimit2));
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, RatioLimit3), sizeof (CpuSetup->RatioLimit3));
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, RatioLimit4), sizeof (CpuSetup->RatioLimit4));
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, RatioLimit5), sizeof (CpuSetup->RatioLimit5));
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, RatioLimit6), sizeof (CpuSetup->RatioLimit6));
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, RatioLimit7), sizeof (CpuSetup->RatioLimit7));
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, RatioLimit8), sizeof (CpuSetup->RatioLimit8));
      break;
// AMI_OVERRIDE_END - CoreMaxOcRatio sync with RatioLimit

    case KEY_RING_MIN_OC_RATIO_LIMIT:
      //
      // Ensure that min ring ratio limit does not go below the LFM ratio
      //
      if ((CpuSetup->RingMinOcRatio < LfmRatio) && (CpuSetup->RingMinOcRatio != 0)) {
        CpuSetup->RingMinOcRatio = LfmRatio;
      }
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, RingMinOcRatio), sizeof (CpuSetup->RingMinOcRatio));
      break;

    case KEY_RING_MAX_OC_RATIO_LIMIT:
      //
      // Ensure that max ring ratio limit does not go below the LFM ratio
      //
      if ((CpuSetup->RingMaxOcRatio < LfmRatio) && (CpuSetup->RingMaxOcRatio != 0)) {
        CpuSetup->RingMaxOcRatio = LfmRatio;
      }
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, RingMaxOcRatio), sizeof (CpuSetup->RingMaxOcRatio));
      break;
	  
// AMI_OVERRIDE_START - RatioLimit1 sync with CoreMaxOcRatio
    case KEY_RatioLimit1:
      CpuSetup->CoreMaxOcRatio = CpuSetup->RatioLimit1;
      RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (CPU_SETUP, CoreMaxOcRatio), sizeof (CpuSetup->CoreMaxOcRatio));
      break;
// AMI_OVERRIDE_START - RatioLimit1 sync with CoreMaxOcRatio

    case KEY_CPU_OVER_CLOCKING:
//
// KblGBegin
//
#ifndef MINTREE_FLAG
      if (IsKblGKSku()) {
        if (CpuSetup->OverclockingSupport == 0x1) {
          CpuSetup->EnableDptf = 0x0;
        } else {
          CpuSetup->EnableDptf = 0x1;
        }

        SetupRequestString = HiiConstructRequestString (SetupRequestString, OFFSET_OF (SETUP_DATA, EnableDptf), sizeof (CpuSetup->EnableDptf));
      }
#endif  // MINTREE_FLAG
//
// KblGEnd
//
      break;
    //Not use yet.
    //case KEY_TPV_GFX_OVER_CLOCKING:
    //  break;
    default:
      ASSERT(FALSE);
  }

  if (RequestString != NULL) {
#if 0 // AMI_OVERRIDE_START - For AMI TSE callback design
    VarSize = sizeof(CPU_SETUP);
    Status = HiiSetBrowserData(&gCpuSetupVariableGuid, L"CpuSetup", VarSize, (UINT8 *) CpuSetup, RequestString);
#else
    VarSize = sizeof(SETUP_DATA);
    Status = HiiSetBrowserData(&gSetupVariableGuid, L"Setup", VarSize, (UINT8 *) CpuSetup, NULL);
#endif // AMI_OVERRIDE_START - For AMI TSE callback design
	ASSERT_EFI_ERROR(Status);
    FreePool (RequestString);
  }
  if (SetupRequestString != NULL) {
    VarSize = sizeof(SETUP_DATA);
    // AMI_OVERRIDE_START - For AMI setup design.
    Status = HiiSetBrowserData(&gSetupVariableGuid, L"Setup", VarSize, (UINT8 *) CpuSetup, SetupRequestString);
    // AMI_OVERRIDE_END - For AMI setup design.
    ASSERT_EFI_ERROR(Status);
    FreePool (SetupRequestString);
  }

  FreePool(CpuSetup);
  // AMI_OVERRIDE_START - For AMI setup design.
  //FreePool(SetupData);
  //FreePool(SaSetup);
  // AMI_OVERRIDE_END - For AMI setup design.

  return EFI_SUCCESS;
}

/**
  This function displays the Memory Timing values in the OC Setup Menu when Memory Profile is changed.

  @param This            Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Action          Specifies the type of action taken by the browser.
  @param QuestionId      A unique value which is sent to the original exporting driver
                         so that it can identify the type of data to expect.
  @param Type            The type of value for the question.
  @param Value           A pointer to the data being sent to the original exporting driver.
  @param ActionRequest   On return, points to the action requested by the callback function.

  @retval EFI_SUCCESS - The function completed successfully
  @retval EFI_UNSUPPORTED - The Setup Menu was not updated
  @retval EFI_NOT_FOUND - There is an error in Locating protocols or retrieving browser data
**/
EFI_STATUS
EFIAPI
OcFormMemoryTimingCallBackFunction (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_BROWSER_ACTION                   Action,
  IN EFI_QUESTION_ID                      KeyValue,
  IN UINT8                                Type,
  IN EFI_IFR_TYPE_VALUE                   *Value,
  OUT EFI_BROWSER_ACTION_REQUEST          *ActionRequest
  )
{
  EFI_STATUS                      Status;
  SA_SETUP                        SaSetup;
  UINTN                           VariableSize;
  EFI_STRING                      RequestString = NULL;
  MEMORY_TIMING                   *MemoryTiming;
  UINT8                           MemoryProfile;
  MEM_INFO_PROTOCOL               *MemInfoHobProtocol;
  MEMORY_INFO_DATA                *MemInfoData;
  UINT8                           RefClk;
  UINT8                           OddRatioMode;
  UINT8                           Ratio;
  BOOLEAN                         ExtendedDdrOverclock;
  CPU_STEPPING                    CpuStepping;
  CPU_FAMILY                      CpuFamily;
  SETUP_VOLATILE_DATA             SetupVolatileData;


  if (Action != EFI_BROWSER_ACTION_CHANGED) {
    return EFI_UNSUPPORTED;
  }

  // Get the Memory Info HOB Protocol if it exists.
  Status = gBS->LocateProtocol (&gMemInfoProtocolGuid, NULL, (VOID **) &MemInfoHobProtocol);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  VariableSize = sizeof (SA_SETUP);
  Status = HiiGetBrowserData (&gSaSetupVariableGuid, L"SaSetup", VariableSize, (UINT8 *) &SaSetup);
  if (Status != TRUE) {
    return EFI_NOT_FOUND;
  }

  CpuFamily = GetCpuFamily();
  CpuStepping = GetCpuStepping();

  ExtendedDdrOverclock = FALSE;
  VariableSize = sizeof (SETUP_VOLATILE_DATA);
  Status = gRT->GetVariable (
                  L"SetupVolatileData",
                  &gSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &SetupVolatileData
                  );
  if (!EFI_ERROR (Status)) {
    if ((CpuFamily == EnumCpuKblDtHalo) &&
        (CpuStepping >= EnumKblN0) &&
        (SetupVolatileData.DdrOverclockSupported == 1)) {
      ExtendedDdrOverclock = TRUE;
    }
  }

  MemoryProfile = SaSetup.SpdProfileSelected;

  // If the MemoryProfile is changed to Custom, do display updated timing values
  if (MemoryProfile == CUSTOM_PROFILE) {
    return EFI_UNSUPPORTED;
  }

  MemInfoData  = &MemInfoHobProtocol->MemInfoData;
  MemoryTiming = &MemInfoData->Timing[MemoryProfile];

  SaSetup.tCL           = (UINT8) MemoryTiming->tCL;
  SaSetup.tCWL          = (UINT8) MemoryTiming->tCWL;
  SaSetup.tFAW          = MemoryTiming->tFAW;
  SaSetup.tRAS          = MemoryTiming->tRAS;
  SaSetup.tRCDtRP       = (UINT8) MemoryTiming->tRCDtRP;
  SaSetup.tREFI         = MemoryTiming->tREFI;
  SaSetup.tRFC          = MemoryTiming->tRFC;
  SaSetup.tRRD          = (UINT8) MemoryTiming->tRRD;
  SaSetup.tRTP          = (UINT8) MemoryTiming->tRTP;
  SaSetup.tWR           = (UINT8) MemoryTiming->tWR;
  SaSetup.tWTR          = (UINT8) MemoryTiming->tWTR;
  SaSetup.tWTR_L        = (UINT8) MemInfoData->tWTR_L[MemoryProfile];
  SaSetup.tWTR_S        = (UINT8) MemInfoData->tWTR_S[MemoryProfile];
  SaSetup.tRRD_L        = (UINT8) MemInfoData->tRRD_L[MemoryProfile];
  SaSetup.tRRD_S        = (UINT8) MemInfoData->tRRD_S[MemoryProfile];
  SaSetup.MemoryVoltage = MemInfoData->VddVoltage[MemoryProfile];

  ConvertClock2FreqRatio (
    MemoryTiming->tCK,
    (SaSetup.BclkOverride == 0) ? 0 : SaSetup.BclkFrequency,
    ExtendedDdrOverclock,
    &RefClk,
    &Ratio,
    &OddRatioMode
    );

  SaSetup.DdrRefClk     = RefClk;
  SaSetup.DdrRatio      = Ratio;
  SaSetup.DdrOddRatioMode = OddRatioMode;

  RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SA_SETUP, tCL), sizeof (SaSetup.tCL));
  RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SA_SETUP, tCWL), sizeof (SaSetup.tCWL));
  RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SA_SETUP, tFAW), sizeof (SaSetup.tFAW));
  RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SA_SETUP, tRAS), sizeof (SaSetup.tRAS));
  RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SA_SETUP, tRCDtRP), sizeof (SaSetup.tRCDtRP));
  RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SA_SETUP, tREFI), sizeof (SaSetup.tREFI));
  RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SA_SETUP, tRFC), sizeof (SaSetup.tRFC));
  RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SA_SETUP, tRTP), sizeof (SaSetup.tRTP));
  RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SA_SETUP, tWR), sizeof (SaSetup.tWR));
  RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SA_SETUP, NModeSupport), sizeof (SaSetup.NModeSupport));
  RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SA_SETUP, MemoryVoltage), sizeof (SaSetup.MemoryVoltage));
  RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SA_SETUP, DdrRefClk), sizeof (SaSetup.DdrRefClk));
  RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SA_SETUP, DdrRatio), sizeof (SaSetup.DdrRatio));
  RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SA_SETUP, DdrOddRatioMode), sizeof (SaSetup.DdrOddRatioMode));

  if (MemInfoData->DdrType == MRC_DDR_TYPE_DDR4) {
    RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SA_SETUP, tWTR_L), sizeof (SaSetup.tWTR_L));
    RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SA_SETUP, tWTR_S), sizeof (SaSetup.tWTR_S));
    RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SA_SETUP, tRRD_L), sizeof (SaSetup.tRRD_L));
    RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SA_SETUP, tRRD_S), sizeof (SaSetup.tRRD_S));
  } else {
    RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SA_SETUP, tWTR), sizeof (SaSetup.tWTR));
    RequestString = HiiConstructRequestString (RequestString, OFFSET_OF (SA_SETUP, tRRD), sizeof (SaSetup.tRRD));
  }

  if (RequestString != NULL) {
    Status = HiiSetBrowserData(&gSaSetupVariableGuid, L"SaSetup", VariableSize, (UINT8 *) &SaSetup, RequestString);
    if (Status != TRUE) {
      return EFI_NOT_FOUND;
    }
    FreePool (RequestString);
  }

  return EFI_SUCCESS;
}

// AMI_OVERRIDE_START - EIP315416:Overclocking callback for memory timing.
EFI_STATUS
EFIAPI
OverClockSetupCallback(
  IN EFI_HII_HANDLE HiiHandle, 
  IN UINT16         Class, 
  IN UINT16         SubClass, 
  IN UINT16         Key
)
{
    CALLBACK_PARAMETERS 	*pCallbackData = NULL;
    SETUP_DATA            SetupData;
    UINTN                 VariableSize;
    UINT32                VariableAttr;
    EFI_STATUS            Status;

    pCallbackData = GetCallbackParameters();
  
    if (pCallbackData == NULL)
        return EFI_UNSUPPORTED;
		
    if (pCallbackData->Action == EFI_BROWSER_ACTION_CHANGING || pCallbackData->Action == EFI_BROWSER_ACTION_CHANGED)
        return EFI_UNSUPPORTED;

    if (pCallbackData->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD){
	    VariableSize = sizeof (SETUP_DATA);
	    Status = gRT->GetVariable (
                L"Setup",
                &gSetupVariableGuid,
                &VariableAttr,
                &VariableSize,
                &SetupData
                );
	    ASSERT_EFI_ERROR (Status);
  
	    switch (pCallbackData->KeyValue) {
	        case KEY_MEMINFO:
	            pCallbackData->Value->u16 = SetupData.tCLDefault;
	            break;
	        case KEY_MEMINFO1:
	            pCallbackData->Value->u16 = SetupData.tRCDtRPDefault;
	            break;
	        case KEY_MEMINFO3:
	            pCallbackData->Value->u16 = SetupData.tRASDefault;
	            break;
	        case KEY_MEMINFO11:
	            pCallbackData->Value->u16 = SetupData.tCWLDefault;
	            break;
	        case KEY_MEMINFO10:
	            pCallbackData->Value->u16 = SetupData.tFAWDefault;
	            break;
	        case KEY_MEMINFO13:
	            pCallbackData->Value->u16 = SetupData.tREFIDefault;
	            break;
	        case KEY_MEMINFO5:
	            pCallbackData->Value->u16 = SetupData.tRFCDefault;
	            break;
	        case KEY_MEMINFO6:
	            pCallbackData->Value->u16 = SetupData.tRRDDefault;
	            break;
	        case KEY_MEMINFO8:
	            pCallbackData->Value->u16 = SetupData.tRTPDefault;
	            break;
	        case KEY_MEMINFO7:
	            pCallbackData->Value->u16 = SetupData.tWTRDefault;
	            break;
	        case KEY_MEMINFO4:
	            pCallbackData->Value->u16 = SetupData.NModeDefault;
	            break;
    	}
    }
  
    return EFI_SUCCESS;
}
// AMI_OVERRIDE_END - EIP315416:Overclocking callback for memory timing.
