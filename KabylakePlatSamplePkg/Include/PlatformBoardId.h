/** @file
Defines Platform BoardIds

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2011 - 2019 Intel Corporation.

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

#ifndef _PLATFORM_BOARD_ID_H_
#define _PLATFORM_BOARD_ID_H_

#define PLATFORM_INFO_HOB_REVISION          0x1

#define FlavorUnknown                       0x0
#define FlavorMobile                        0x1
#define FlavorDesktop                       0x2
#define FlavorWorkstation                   0x3
#define FlavorUpServer                      0x4
#define FlavorEmbedded                      0x5
#define FlavorPlatformMax                   0x6

#define TypeUnknown                         0x0
#define TypeTrad                            0x1
#define TypeUltUlx                          0x2

#define GenLpt                              0x1
#define GenWpt                              0x2
#define GenUnknown                          0x3

//
// Mobile Board Id 0x00 - 0xFF
//
// PUT SKL CRB BoardIds here
#ifndef MINTREE_FLAG
#define BoardIdSkylakeA0Lpddr3Crb           0x1
#define BoardIdSkylakeA0Ddr4Rvp2            0x2
#define BoardIdKabylakeKcDdr3               0x3
#endif //MINTREE_FLAG
#define BoardIdSkylakeA0Rvp3                0x4
#ifndef MINTREE_FLAG
#define BoardIdSkylakeYRvp13                0x5
#define BoardIdHaloDdr4uSFF                 0x6
#define BoardIdKabylakeRDdr4                0x7
#define BoardIdSkylakeLpdimmDoe             0x8
#define BoardIdSkylakeStarbrook             0x9
#define BoardIdSkylakeULpddr3Rvp5           0xA
#define BoardIdSkylakeURvp7                 0xB
#define BoardIdSkylakeURvp15                0xC
#define BoardIdSkylakeUDoe                  0xD
#define BoardIdCannonlakeULpddr3Rvp5        0xE
//#define BoardIdSkylakeURsvd               0xE
//#define BoardIdSkylakeURsvd               0xF

#define BoardIdSkylakeX0Lpddr3PpvErb        0x10  //Not used - LPT Only
#define BoardIdSkylakeX0Lpddr3Ppv           0x11  //Not used - LPT Only
#define BoardIdSkylakeX0Ddr4Ppv             0x12  //Not used - LPT Only
#define BoardIdKabylakeRLpddr3              0x13
#define BoardIdSkylakeA0Rvp3Ppv             0x14
#define BoardIdSkylakeYRsvd2                0x15
#define BoardIdSkylakeYRsvd3                0x16
#define BoardIdSkylakeYRsvd4                0x17
#define BoardIdSkylakeYRsvd5                0x18
#define BoardIdSkylakeYRsvd6                0x19
#define BoardIdSkylakeULpddr3Rvp5Ppv        0x1A
#define BoardIdSkylakeURvp7Ppv              0x1B
#define BoardIdSkylakeDdr4Ppv               0x1C
#define BoardIdSkylakeURsvd2                0x1D
#define BoardIdSkylakeURsvd3                0x1E
#define BoardIdSkylakeURsvd4                0x1F
#define BoardIdSkylakeAioRvp10Erb           0x40
#define BoardIdSkylakeAioRvp9Crb            0x41
#define BoardIdSkylakeDtRvp8Crb             0x42
#define BoardIdSkylakeAioRvp10Crb           0x43
#define BoardIdSkylakeHaloDdr4Rvp11         0x44
#define BoardIdSkylakeHaloDdr4Rvp11Ppv      0x45
#define BoardIdSkylakeAioRvp10CrbPpv        0x46
#define BoardIdSkylakeHaloLpddr3Rvp16       0x48
#define BoardIdSkylakeSlpfPpv               0x49
#define BoardIdKabylakeDdr4Rvp17            0x4A  // KblGContent
#define BoardIdSkylakeSdlBrk                0x50
#define BoardIdSkylakeAioRvp10Evp           0x51
#define BoardIdSkylakeSds                   0x20
#define BoardIdSkylakePantherMtn            0x21
#define BoardIdSkylakePhabletPoc            0x23

//
// Virtual BoardId for OS Profile: 0x70 to 0x77
// This is only for distinguishing setup defaults based on OS profile selection.
// The actual BoardId won't change.
// They are to be used only in default statements in .sd or.vfr that get parsed by FCE tool during post build.
// These virtual IDs won't work in suppressif/grayoutif either.
//
#define BoardIdSkylakeSdsAndroid            0x70
#define BoardIdSkylakeSdsWinCs              0x71
#define BoardIdSkylakeSdsWinNonCs           0x72

#define BoardIdZumbaBeachServerEv           0x73
#define BoardIdZumbaBeachServerCrb          0x74

//
// End Virtual IDs
//

//    CRB build
// ----------------------------+------------------
//    SKL A0 / LPT_LP (in SKL_LPT stream only)
//    SKL A0 / SKL PCH LP (in SKL_SPT stream)
//
#define BoardIdKarkomX0                     0x54
#define BoardIdKarkomA0                     0x55
#define BoardIdKarkomUlt                    0x56
#define BoardIdKarkomMiniUlt                0x57
#define BoardIdPineValley                   0x58
//60-6F reserved for KBL RVPs
#endif //MINTREE_FLAG
#define BoardIdKabyLakeYLpddr3Rvp3          0x60
#ifndef MINTREE_FLAG
#define BoardIdKabylakeGrizzlyMtn           0x61
#define BoardIdKabylakeCoyoteMtn            0x62
#define BoardIdKabylakeULpddr3Doe1          0x63
#define BoardIdKabylakeUDdr3lRvp7           0x64
#define BoardIdKabylakeULpddr3Doe           0x65
#define BoardIdKabyLakeSDdr4UdimmEvErb      0x66
#define BoardIdKabyLakeSDdr4UdimmEvCrb      0x67
#define BoardIdKabyLakeOc                   0x68
#define BoardIdKabyLakeSDdr4UdimmCrb        0x69
#define BoardIdKabylakeSUdimmCpv            0x6A
#define BoardIdBasinFallsEv                 0x6B
#define BoardIdBasinFallsCrb                0x6C
#define BoardIdBasinFallsPpv                0x6D
#define BoardIdBasinFallsRsvd               0x6E
#define BoardIdHaloDdr4uSffCpv              0x70
#define BoardIdGlacierFallsXErb             0x78
#define BoardIdGlacierFallsXEv              0x79
#define BoardIdGlacierFallsXCrb             0x7A
#define BoardIdGlacierFallsXPpv             0x7B
#define BoardIdKabylakeHawkMountainIdv      0x80
#endif //MINTREE_FLAG
#define BoardIdAmberLakeY42Lpddr3Rvp3       0x81
#define BoardIdAmberLakeLpddr3Rvp3          0x82
#define BoardIdAmberLakeY42Lpddr3Cff        0x84
#define BoardIdAmberLakeY42Lpddr3CffCc      0x85
#ifndef MINTREE_FLAG
#define BoardIdAmberLakeLpddr3Rvp13         0x83

#define BoardIdMobileMax                    0x3f
#define BoardIdDesktopMax                   0x4f
#define BoardIdTradMobileMax                0x1f
#endif //MINTREE_FLAG
#define BoardIdUnknown1                     0xffff
#define BoardIdPlatformMax                  0xff
#define BOARD_ID_MASK_5BIT                  0x1f
#define BOARD_ID_MASK_8BIT                  0xff
#define FAB_ID_MASK                         0x7
#define BOM_ID_MASK                         0x7
#define GENERATION_MASK                     0x100
#ifndef MINTREE_FLAG
#define BoardRevPearValleyERB               0x07
#endif //MINTREE_FLAG

#endif
