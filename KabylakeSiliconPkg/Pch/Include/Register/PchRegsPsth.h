/** @file
  Register definition for PSTH component

  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values within the bits
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position
  - In general, PCH registers are denoted by "_PCH_" in register names
  - Registers / bits that are different between PCH generations are denoted by
    "_PCH_[generation_name]_" in register/bit names.
  - Registers / bits that are specific to PCH-H denoted by "_H_" in register/bit names.
    Registers / bits that are specific to PCH-LP denoted by "_LP_" in register/bit names.
    e.g., "_PCH_H_", "_PCH_LP_"
    Registers / bits names without _H_ or _LP_ apply for both H and LP.
  - Registers / bits that are different between SKUs are denoted by "_[SKU_name]"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a PCH generation will be just named
    as "_PCH_" without [generation_name] inserted.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2014 - 2016 Intel Corporation.

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
#ifndef _PCH_REGS_PSTH_H_
#define _PCH_REGS_PSTH_H_

//
// Private chipset regsiter (Memory space) offset definition
// The PCR register defines is used for PCR MMIO programming and PCH SBI programming as well.
//

//
// PSTH and IO Trap PCRs (PID:PSTH)
//
#define R_PCH_PCR_PSTH_PSTHCTL          0x1D00                ///< PSTH control register
#define B_PCH_PCR_PSTH_PSTHIOSFPTCGE    BIT2                  ///< PSTH IOSF primary trunk clock gating enable
#define B_PCH_PCR_PSTH_PSTHIOSFSTCGE    BIT1                  ///< PSTH IOSF sideband trunk clock gating enable
#define B_PCH_PCR_PSTH_PSTHDCGE         BIT0                  ///< PSTH dynamic clock gating enable
#define R_PCH_PCR_PSTH_TRPST            0x1E00                ///< Trap status regsiter
#define B_PCH_PCR_PSTH_TRPST_CTSS       0x0000000F            ///< Cycle Trap SMI# Status mask
#define R_PCH_PCR_PSTH_TRPC             0x1E10                ///< Trapped cycle
#define B_PCH_PCR_PSTH_TRPC_RW          BIT24                 ///< Read/Write#: 1=Read, 0=Write
#define B_PCH_PCR_PSTH_TRPC_AHBE        0x00000000000F0000    ///< Active high byte enables
#define B_PCH_PCR_PSTH_TRPC_IOA         0x000000000000FFFC    ///< Trap cycle I/O address
#define R_PCH_PCR_PSTH_TRPD             0x1E18                ///< Trapped write data
#define B_PCH_PCR_PSTH_TRPD_IOD         0x00000000FFFFFFFF    ///< Trap cycle I/O data
#define R_PCH_PCR_PSTH_TRPREG0          0x1E80                ///< IO Tarp 0 register
#define R_PCH_PCR_PSTH_TRPREG1          0x1E88                ///< IO Tarp 1 register
#define R_PCH_PCR_PSTH_TRPREG2          0x1E90                ///< IO Tarp 2 register
#define R_PCH_PCR_PSTH_TRPREG3          0x1E98                ///< IO Tarp 3 register
#define B_PCH_PCR_PSTH_TRPREG_RWM       BIT17                 ///< 49 - 32 for 32 bit access, Read/Write mask
#define B_PCH_PCR_PSTH_TRPREG_RWIO      BIT16                 ///< 48 - 32 for 32 bit access, Read/Write#, 1=Read, 0=Write
#define N_PCH_PCR_PSTH_TRPREG_RWIO      16                    ///< 48 - 32 for 32 bit access, 16bit shift for Read/Write field
#define N_PCH_PCR_PSTH_TRPREG_BEM       (36 - 32)
#define B_PCH_PCR_PSTH_TRPREG_BEM       0x000000F000000000    ///< Byte enable mask
#define B_PCH_PCR_PSTH_TRPREG_BE        0x0000000F00000000    ///< Byte enable
#define B_PCH_PCR_PSTH_TRPREG_AM        0x0000000000FC0000    ///< IO Address mask
#define B_PCH_PCR_PSTH_TRPREG_AD        0x000000000000FFFC    ///< IO Address
#define B_PCH_PCR_PSTH_TRPREG_TSE       BIT0                  ///< Trap and SMI# Enable


#endif
