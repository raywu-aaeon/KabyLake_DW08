/** @file
  Register names for CPU registers

  <b>Conventions</b>
  - Definitions beginning with "MSR_" are MSRs
  - Definitions beginning with "R_" are registers
  - Definitions beginning with "B_" are bits within registers
  - Definitions beginning with "V_" are meaningful values of bits within the registers
  - Definitions beginning with "S_" are register sizes
  - Definitions beginning with "N_" are the bit position

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2004 - 2019 Intel Corporation.

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
#ifndef _CPU_REGS_H_
#define _CPU_REGS_H_

#ifndef MINTREE_FLAG
///
/// CPU ID Instruction defines not in Register/Cpuid.h from UefiCpuPkg
/// @{
#define CPUID_SGX_ENABLED                                             0x12
/// @}
#endif  // MINTREE_FLAG
///
/// MSR defines not in SDM
/// @{
#ifndef MINTREE_FLAG
#define MSR_CORE_THREAD_COUNT                                         0x00000035
#define N_CORE_COUNT_OFFSET                                           16
#define B_THREAD_COUNT_MASK                                           0xFFFF

#define PIC_THREAD_CONTROL                                            0x0000002E
#define B_PIC_THREAD_CONTROL_TPR_DIS                                  BIT10

#define MSR_IA32_SMM_SAVE_CONTROL                                     0x0000003E
#define B_MSR_IA32_SMM_SAVE_CONTROL_SFPPE                             BIT0
#define MSR_IA32_BIOS_ENBL_XU                                         0x0000007A

#define MSR_PRMRR_PHYS_BASE                                           0x000001F4
#define PRMRR_MAX_SIZE                                                0x08000000
#define MSR_PRMRR_PHYS_MASK                                           0x000001F5
#define V_MSR_PRMRR_MASK                                              0x0000007FFFFFFFFF
#define MSR_PRMRR_VALID_CONFIG                                        0x000001FB
#define MSR_UNCORE_PRMRR_PHYS_BASE                                    0x000002F4
#define MSR_UNCORE_PRMRR_PHYS_MASK                                    0x000002F5
#define B_MSR_PRMRR_PHYS_MASK_LOCK                                    BIT10
#define B_MSR_PRMRR_VALID_BIT                                         BIT11
#define MSR_TRACE_HUB_STH_ACPIBAR_BASE                                0x00000080
#define B_MSR_TRACE_HUB_STH_ACPIBAR_BASE_LOCK                         BIT0
#define V_MSR_TRACE_HUB_STH_ACPIBAR_BASE_MASK                         0x000000000003FFFF
#define MSR_PLAT_FRMW_PROT_CTRL                                       0x00000110
#define B_MSR_PLAT_FRMW_PROT_CTRL_LK                                  BIT0
#define B_MSR_PLAT_FRMW_PROT_CTRL_EN                                  BIT1
#define B_MSR_PLAT_FRMW_PROT_CTRL_S1                                  BIT2
#define MSR_PLAT_FRMW_PROT_HASH_0                                     0x00000111
#define MSR_PLAT_FRMW_PROT_HASH_1                                     0x00000112
#define MSR_PLAT_FRMW_PROT_HASH_2                                     0x00000113
#define MSR_PLAT_FRMW_PROT_HASH_3                                     0x00000114
#define MSR_PLAT_FRMW_PROT_TRIG_PARAM                                 0x00000115
#define N_MSR_PLAT_FRMW_PROT_TRIG_PARAM_STATUS_OFFSET                 0
#define V_MSR_PLAT_FRMW_PROT_TRIG_PARAM_STATUS_MASK                   0x000000000000FFFF
#define N_MSR_PLAT_FRMW_PROT_TRIG_PARAM_DATA_OFFSET                   16
#define V_MSR_PLAT_FRMW_PROT_TRIG_PARAM_DATA_MASK                     0x000000000000FFFF
#define N_MSR_PLAT_FRMW_PROT_TRIG_PARAM_TERMINAL_OFFSET               32
#define V_MSR_PLAT_FRMW_PROT_TRIG_PARAM_TERMINAL_MASK                 0x000000000000FFFF
#define B_MSR_PLAT_FRMW_PROT_TRIG_PARAM_SE                            BIT62
#define N_MSR_PLAT_FRMW_PROT_TRIG_PARAM_SE_OFFSET                     62
#define MSR_PLAT_FRMW_PROT_TRIGGER                                    0x00000116
#define MSR_PLAT_FRMW_PROT_PASSWD                                     0x00000117
#define MSR_PLAT_BIOS_INFO_FLAGS                                      0x0000011F
#define MSR_BIOS_UCODE_PM_TMR_EMULATION_CFG                           0x00000121           ///< MSR 0x121 PM_TMR Emulation Configuration
#define N_BIOS_UCODE_PM_TMR_EMULATION_CFG_CORRECTION_FACTOR_OFFSET    32
#define V_BIOS_UCODE_PM_TMR_EMULATION_CFG_CORRECTION_FACTOR           0x262E8B51           ///< ACPI Timer (PM_TMR) Emulation Correction Factor
#define B_BIOS_UCODE_PM_TMR_EMULATION_CFG_VALID                       BIT16                ///< PM_TMR
#define N_BIOS_UCODE_PM_TMR_EMULATION_CFG_DELAY_VALUE_OFFSET          20
#define V_BIOS_UCODE_PM_TMR_EMULATION_CFG_DELAY_VALUE                 0x13
#endif  // MINTREE_FLAG
#define MSR_SPCL_CHIPSET_USAGE_ADDR                                   0x000001FE
#ifndef MINTREE_FLAG
#define MSR_FLEX_RATIO                                                0x00000194
#define N_FLEX_RATIO                                                  8
#define B_FLEX_RATIO                                                  (0xFF << 8)
#define B_FLEX_EN                                                     BIT16
#define B_MAX_EXTRA_VOLTAGE                                           0xFF
#define N_OVERCLOCKING_BINS                                           17
#define B_OVERCLOCKING_BINS                                           (0x7 << 17)
#define B_OVERCLOCKING_LOCK                                           BIT20
#define RATIO_FLEX_CLEAR_MASK                                         0xFFFFFFFFFFFF00FFULL
#define NO_EVICT_MODE                                                 0x000002E0
#define B_NO_EVICT_MODE_SETUP                                         BIT0
#define B_NO_EVICT_MODE_RUN                                           BIT1
#define MSR_LT_UNLOCK_MEMORY                                          0x000002E6
#define MSR_LT_LOCK_MEMORY                                            0x000002E7
#define B_MSR_LT_LOCK_MEMORY_CONFIG_LOCK                              BIT0
#define B_MSR_LT_LOCK_MEMORY_MEM_LOCK_CPU                             BIT1
#define B_MSR_LT_LOCK_MEMORY_MEM_LOCK_DEV                             BIT2
#define MSR_PL4_CONTROL                                               0x00000601
#define V_POWER_LIMIT_4_MASK                                          (0x1FFF)
#define MSR_PL3_CONTROL                                               0x00000615
#define POWER_LIMIT_3_TIME_MASK                                       (0xFE0000)
#define POWER_LIMIT_3_DUTY_CYCLE_MASK                                 (0x7F000000)
#define MSR_RING_RATIO_LIMIT                                          0x00000620
#define V_MSR_RING_RATIO_LIMIT_MASK                                   0x7F
#define MSR_UCODE_CR_BIOS_SE_SVN                                      0x00000302
#endif  // MINTREE_FLAG
#define MSR_IA32_CR_PAT                                               0x00000277

//
// MSRs for SMM State Save Register
//
#define MSR_EFLAGS                                                    0x00000C02
#define MSR_RBX                                                       0x00000C11
#define MSR_RDX                                                       0x00000C12
#define MSR_RCX                                                       0x00000C13
#define MSR_RAX                                                       0x00000C14
#define MSR_SMBASE                                                    0x00000C20

#ifndef MINTREE_FLAG
#define MSR_BC_PBEC                                                   0x00000139
#define B_STOP_PBET                                                   BIT0
#define MSR_BOOT_GUARD_SACM_INFO                                      0x0000013A
#define B_BOOT_GUARD_SACM_INFO_NEM_ENABLED                            BIT0
#define V_TPM_PRESENT_MASK                                            0x06
#define V_TPM_PRESENT_NO_TPM                                          0
#define V_TPM_PRESENT_DTPM_12                                         1
#define V_TPM_PRESENT_DTPM_20                                         2
#define V_TPM_PRESENT_PTT                                             3
#define B_BOOT_GUARD_SACM_INFO_TPM_SUCCESS                            BIT3
#define B_BOOT_GUARD_SACM_INFO_MEASURED_BOOT                          BIT5
#define B_BOOT_GUARD_SACM_INFO_VERIFIED_BOOT                          BIT6
#define B_BOOT_GUARD_SACM_INFO_CAPABILITY                             BIT32
#endif
/// @}

///
/// Arch-specific MSR defines in SDM, but not defined for SKL
/// @{

#define MSR_PLATFORM_INFO                                             0x000000CE
#define N_PLATFORM_INFO_MIN_RATIO                                     40
#define B_PLATFORM_INFO_RATIO_MASK                                    0xFF
#define N_PLATFORM_INFO_MAX_RATIO                                     8
#define B_MSR_PLATFORM_INFO_BIOSGUARD_AVAIL                           BIT35
#define N_MSR_PLATFORM_INFO_CONFIG_TDP_NUM_LEVELS_OFFSET              33
#define V_CONFIG_TDP_NUM_LEVELS_MASK                                  (BIT34 | BIT33)
#define B_PLATFORM_INFO_TDC_TDP_LIMIT                                 BIT29
#define N_PLATFORM_INFO_RATIO_LIMIT                                   28
#define B_PLATFORM_INFO_RATIO_LIMIT                                   BIT28
#define B_PLATFORM_INFO_SAMPLE_PART                                   BIT27
#define B_PLATFORM_INFO_SMM_SAVE_CONTROL                              BIT16
#define N_PLATFORM_INFO_PROG_TCC_ACTIVATION_OFFSET                    30
#define B_PLATFORM_INFO_PROG_TCC_ACTIVATION_OFFSET                    BIT30
#define B_PLATFORM_INFO_TIMED_MWAIT_SUPPORTED                         BIT37
#define B_PLATFORM_INFO_EDRAM_EN                                      BIT57

//
// MSR_BROADWELL_PKG_CST_CONFIG_CONTROL: related defines
//
#define B_TIMED_MWAIT_ENABLE                                          BIT31 ///< @todo Remove when bitfield definition is available.
#define V_CSTATE_LIMIT_C1                                             0x01
#define V_CSTATE_LIMIT_C3                                             0x02
#define V_CSTATE_LIMIT_C6                                             0x03
#define V_CSTATE_LIMIT_C7                                             0x04
#define V_CSTATE_LIMIT_C7S                                            0x05
#define V_CSTATE_LIMIT_C8                                             0x06
#define V_CSTATE_LIMIT_C9                                             0x07
#define V_CSTATE_LIMIT_C10                                            0x08

#define MSR_PMG_IO_CAPTURE_BASE                                       0x000000E4
#define B_MSR_PMG_CST_RANGE                                           (BIT18 | BIT17 | BIT16)
#define V_IO_CAPT_LVL2                                                (0x0 << 16)   ///< C3
#define V_IO_CAPT_LVL3                                                (0x1 << 16)   ///< C6
#define V_IO_CAPT_LVL4                                                (0x2 << 16)   ///< C7
#define V_IO_CAPT_LVL5                                                (0x3 << 16)   ///< C8
#define V_IO_CAPT_LVL6                                                (0x4 << 16)   ///< C9
#define V_IO_CAPT_LVL7                                                (0x5 << 16)   ///< C10
#define V_IO_CAPT_LVL2_BASE_ADDR_MASK                                 0xFFFF

#define MSR_TEMPERATURE_TARGET                                        0x000001A2
#define B_MSR_TEMPERATURE_TARGET_TCC_OFFSET_LOCK                      BIT31
#define N_MSR_TEMPERATURE_TARGET_TCC_OFFSET_LIMIT                     24
#define V_MSR_TEMPERATURE_TARGET_TCC_ACTIVATION_OFFSET_MASK           0x3F
#define N_MSR_TEMPERATURE_TARGET_TCC_ACTIVATION_TEMPERATURE_OFFSET    (16)
#define B_MSR_TEMPERATURE_TARGET_TCC_ACTIVATION_TEMPERATURE_MASK      (0xFF << 16)
#define N_MSR_TEMPERATURE_TARGET_FAN_TEMP_TARGET_OFFSET               8
#define B_MSR_TEMPERATURE_TARGET_FAN_TEMP_TARGET_OFFSET               (0xFF << 8)
#define B_MSR_TEMPERATURE_TARGET_TCC_OFFSET_TIME_WINDOW               (0x7F)
#define B_MSR_TEMPERATURE_TARGET_TCC_OFFSET_MASK                      0xFF
#define B_MSR_TEMPERATURE_TARGET_TCC_OFFSET_CLAMP_BIT                 BIT7

#ifndef MINTREE_FLAG
#define MISC_FEATURE_CONTROL                                          0x000001A4
#define B_MISC_FEATURE_CONTROL_MLC_STRP                               BIT0
#define B_MISC_FEATURE_CONTROL_MLC_SPAP                               BIT1
#define B_MISC_FEATURE_CONTROL_DCU_STRP                               BIT2
#define B_MISC_FEATURE_CONTROL_DCU_IPP                                BIT3
#define B_MISC_FEATURE_CONTROL_3_STRIKE_CNT                           BIT11

#define MSR_MISC_PWR_MGMT                                             0x000001AA
#define B_MISC_PWR_MGMT_SINGLE_PCTL_EN                                BIT0
#define B_MISC_PWR_MGMT_ENABLE_HWP                                    BIT6
#define B_MISC_PWR_MGMT_ENABLE_HWP_INTERRUPT                          BIT7
#define N_MISC_PWR_MGMT_ENABLE_HWP_INTERRUPT                          7
#define B_MISC_PWR_MGMT_ENABLE_OUT_OF_BAND_AUTONOMOUS                 BIT8
#define B_MISC_PWR_MGMT_ENABLE_SDC_OOB                                BIT9
#define N_MISC_PWR_MGMT_ENABLE_SDC_OOB                                9
#define B_MISC_PWR_MGMT_ENABLE_SDC                                    BIT10
#define N_MISC_PWR_MGMT_ENABLE_SDC                                    10
#define B_MISC_PWR_MGMT_SDC_OOB_CAPABLE                               BIT11
#define B_MISC_PWR_MGMT_ENABLE_HWP_EPP                                BIT12
#define N_MISC_PWR_MGMT_ENABLE_HWP_EPP                                12
#define B_MISC_PWR_MGMT_LOCK_TERM_INT                                 BIT22
#endif  // MINTREE_FLAG

#define MSR_TURBO_RATIO_LIMIT                                         0x000001AD
#define N_MSR_TURBO_RATIO_LIMIT_1C                                    0
#define B_MSR_TURBO_RATIO_LIMIT_1C                                    (0xFFULL << 0)
#define N_MSR_TURBO_RATIO_LIMIT_2C                                    8
#define B_MSR_TURBO_RATIO_LIMIT_2C                                    (0xFFULL << 8)
#define N_MSR_TURBO_RATIO_LIMIT_3C                                    16
#define B_MSR_TURBO_RATIO_LIMIT_3C                                    (0xFFULL << 16)
#define N_MSR_TURBO_RATIO_LIMIT_4C                                    24
#define B_MSR_TURBO_RATIO_LIMIT_4C                                    (0xFFULL << 24)
#define N_MSR_TURBO_RATIO_LIMIT_5C                                    32
#define B_MSR_TURBO_RATIO_LIMIT_5C                                    (0xFFULL << 32)
#define N_MSR_TURBO_RATIO_LIMIT_6C                                    40
#define B_MSR_TURBO_RATIO_LIMIT_6C                                    (0xFFULL << 40)
#define N_MSR_TURBO_RATIO_LIMIT_7C                                    48
#define B_MSR_TURBO_RATIO_LIMIT_7C                                    (0xFFULL << 48)
#define N_MSR_TURBO_RATIO_LIMIT_8C                                    56
#define B_MSR_TURBO_RATIO_LIMIT_8C                                    (0xFFULL << 56)

#define MSR_IA32_FEATURE_CONFIG                                       0x0000013C
#define B_IA32_FEATURE_CONFIG_AES_DIS                                 BIT1
#define B_IA32_FEATURE_CONFIG_LOCK                                    BIT0

#ifndef MINTREE_FLAG
#define MSR_POWER_CTL                                                 0x000001FC
#define B_MSR_POWER_CTL_BI_PROCHOT                                    BIT0
#define B_MSR_POWER_CTL_C1E                                           BIT1
#define B_ENERGY_EFFICIENT_P_STATE_FEATURE_ENABLE                     BIT18
#define B_ENERGY_EFFICIENT_TURBO_FEATURE_DISABLE                      BIT19
#define B_MSR_POWER_CTL_RACE_TO_HALT_DISABLE                          BIT20
#define B_MSR_POWER_CTL_DISABLE_PROCHOT_OUT                           BIT21
#define B_MSR_POWER_CTL_PROCHOT_RESPONSE                              BIT22
#define B_MSR_POWER_CTL_PROCHOT_LOCK                                  BIT23
#define B_MSR_POWER_CTL_DISABLE_VR_THERMAL_ALERT                      BIT24
#define B_MSR_POWER_CTL_CSTATE_PRE_WAKE_DISABLE                       BIT30
#endif  // MINTREE_FLAG

//
// MSRs for SMM State Save Register
//
#define MSR_SMM_MCA_CAP                                               0x0000017D
#define B_TARGETED_SMI                                                BIT56
#define N_TARGETED_SMI                                                56
#define B_SMM_CPU_SVRSTR                                              BIT57
#define N_SMM_CPU_SVRSTR                                              57
#define B_SMM_CODE_ACCESS_CHK                                         BIT58
#define N_SMM_CODE_ACCESS_CHK                                         58
#define B_LONG_FLOW_INDICATION                                        BIT59
#define N_LONG_FLOW_INDICATION                                        59
#define MSR_SMM_FEATURE_CONTROL                                       0x000004E0
#define B_SMM_FEATURE_CONTROL_LOCK                                    BIT0
#define B_SMM_CPU_SAVE_EN                                             BIT1
#define B_SMM_CODE_CHK_EN                                             BIT2

#ifndef MINTREE_FLAG
#define MSR_PACKAGE_POWER_SKU_UNIT                                    0x00000606
#define PACKAGE_POWER_UNIT_MASK                                       0xF

///
/// Only some are defined in SDM
/// @{
#define MSR_C_STATE_LATENCY_CONTROL_0                                 0x0000060A
#define MSR_C_STATE_LATENCY_CONTROL_1                                 0x0000060B
#define MSR_C_STATE_LATENCY_CONTROL_2                                 0x0000060C
#define MSR_C_STATE_LATENCY_CONTROL_3                                 0x00000633
#define MSR_C_STATE_LATENCY_CONTROL_4                                 0x00000634
#define MSR_C_STATE_LATENCY_CONTROL_5                                 0x00000635
/// @}
#define B_PKG_IRTL_VALID                                              BIT15
#define B_INTERRUPT_RESPONSE_TIME_LIMIT_MASK                          0x3FF
#define B_TIME_UNIT_MASK                                              (0x7 << 10)
#define N_TIME_UNIT_OFFSET                                            10

#define MSR_PACKAGE_POWER_LIMIT                                       0x00000610
#define MSR_PACKAGE_POWER_SKU                                         0x00000614
#define B_POWER_LIMIT_ENABLE                                          BIT15
#define B_CRITICAL_POWER_CLAMP_ENABLE                                 BIT16
#define B_POWER_LIMIT_LOCK                                            BIT31
#define POWER_LIMIT_MASK                                              (0x7FFF)
#define POWER_LIMIT_1_TIME_MASK                                       (0xFE0000)
#define PACKAGE_TDP_POWER_MASK                                        (0x7FFF)
#define PACKAGE_MIN_POWER_MASK                                        (0x7FFF0000)
#define PACKAGE_MAX_POWER_MASK                                        (0x7FFF)
#define MSR_DDR_RAPL_LIMIT                                            0x00000618
#define B_RAPL_LIMIT_LOCK                                             BIT63

#define MSR_CONFIG_TDP_NOMINAL                                        0x00000648
#define CONFIG_TDP_NOMINAL_RATIO_MASK                                 0xFF
#define MSR_CONFIG_TDP_LVL1                                           0x00000649
#define CONFIG_TDP_LVL1_RATIO_OFFSET                                  16
#define CONFIG_TDP_LVL1_RATIO_MASK                                    (0xFF << 16)
#define CONFIG_TDP_LVL1_PKG_TDP_MASK                                  (0x7FFF)
#define MSR_CONFIG_TDP_LVL2                                           0x0000064A
#define CONFIG_TDP_LVL2_RATIO_OFFSET                                  16
#define CONFIG_TDP_LVL2_RATIO_MASK                                    (0xFF << 16)
#define CONFIG_TDP_LVL2_PKG_TDP_MASK                                  (0x7FFF)
#define MSR_CONFIG_TDP_CONTROL                                        0x0000064B
#define CONFIG_TDP_CONTROL_LOCK                                       (1 << 31)
#define CONFIG_TDP_CONTROL_LVL_MASK                                   0x3
#define CONFIG_TDP_NOMINAL                                            0
#define CONFIG_TDP_LEVEL1                                             1
#define CONFIG_TDP_LEVEL2                                             2
#define MSR_TURBO_ACTIVATION_RATIO                                    0x0000064C
#define MSR_TURBO_ACTIVATION_RATIO_LOCK                               (1 << 31)
#define MSR_TURBO_ACTIVATION_RATIO_MASK                               0xFF
#endif  // MINTREE_FLAG
/// @}

#ifndef MINTREE_FLAG
///
/// MSR defines in SkylakeMsr.h
/// @{
#define MSR_PLATFORM_POWER_LIMIT                                      0x0000065C
/// @}
#endif  // MINTREE_FLAG

///
/// Bit defines for MSRs defined in UefiCpuPkg/Include/Register/ArchitecturalMsr.h.
/// @{
#ifndef MINTREE_FLAG
//
// Bit defines for MSR_IA32_MTRRCAP
//
#define   B_IA32_MTRR_CAP_PRMRR_SUPPORT                                 BIT12

//
// Bit defines for MSR_IA32_PERF_STATUS
//
#define N_IA32_PERF_STSP_STATE_TARGET                                 8
#define B_IA32_PERF_STSP_STATE_MASK                                   0xFF
//
// Bit defines for MSR_IA32_PERF_CTL
//
#define N_IA32_PERF_CTRLP_STATE_TARGET                                8
#define B_IA32_PERF_CTRLP_STATE_TARGET                                (0x7F << 8)

//
// Bit defines for MSR_IA32_MISC_ENABLE
//
#define B_MSR_IA32_MISC_ENABLE_FSE                                    BIT0
#define B_MSR_IA32_MISC_ENABLE_TME                                    BIT3
#define N_MSR_IA32_MISC_ENABLE_EIST_OFFSET                            16
#define B_MSR_IA32_MISC_ENABLE_EIST                                   BIT16
#define B_MSR_IA32_MISC_ENABLE_MONITOR                                BIT18
#define B_MSR_IA32_MISC_ENABLE_CPUID_MAX                              BIT22
#define B_MSR_IA32_MISC_ENABLE_TPR_DIS                                BIT23
#define B_MSR_IA32_MISC_ENABLE_XD                                     BIT34
#define B_MSR_IA32_MISC_DISABLE_TURBO                                 BIT38
#endif  // MINTREE_FLAG

//
// Number of fixed MTRRs
//
#define V_FIXED_MTRR_NUMBER                                           11


//
// Number of variable MTRRs
//
#define V_MAXIMUM_VARIABLE_MTRR_NUMBER                                10

//
// Bit defines for MSR_IA32_MTRR_DEF_TYPE
//
#define B_CACHE_MTRR_VALID                                            BIT11
#define B_CACHE_FIXED_MTRR_VALID                                      BIT10

#ifndef MINTREE_FLAG
//
// Bit defines for MSR_IA32_RTIT_STATUS
//
#define B_RTIT_CTL_TRACE_ENABLE                                       BIT0

//
// Bit defines for MSR_IA32_PM_ENABLE
//
#define B_IA32_PM_ENABLE_HWP_ENABLE                                   0x00000001

//
// Bit defines for MSR_IA32_HWP_CAPABILITIES
//
#define V_HWP_GUARANTEED_PERFORMANCE_MASK                             0x000000000000FF00

//
// Bit defines for MSR_IA32_HWP_INTERRUPT
//
#define B_IA32_HWP_CHANGE_TO_GUARANTEED                               BIT0
#define B_IA32_HWP_EXCURSION_TO_MINIMUM                               BIT1

//
// Bit defines for MSR_IA32_HWP_STATUS
//
#define B_HWP_CHANGE_TO_GUARANTEED                                    BIT0
#define B_HWP_EXCURSION_TO_MINIMUM                                    BIT2

//
// Bit defines for MSR_IA32_X2APIC_LVT_THERMAL
//
#define MMIO_LOCAL_APIC_THERMAL_DEF                                   0xFEE00330
#define B_INTERRUPT_MASK                                              (1 << 16)
#define B_DELIVERY_MODE                                               (0x07 << 8)
#define V_MODE_SMI                                                    (0x02 << 8)
#define B_VECTOR                                                      (0xFF << 0)

//
// Bit defines for MSR_IA32_DEBUG_INTERFACE
//
#define B_DEBUG_INTERFACE_ENABLE                                      BIT0
#define B_DEBUG_INTERFACE_LOCK                                        BIT30
#define B_DEBUG_INTERFACE_DEBUG_STATUS                                BIT31

//
// Bit defines for MSR_IA32_PKG_HDC_CTL
//
#define B_HDC_PKG_CTL_SDC_PACKAGE_ENABLE                              BIT0
#define N_HDC_PKG_CTL_SDC_PACKAGE_ENABLE                              0
#define B_HDC_PKG_CTL_SDC_WAS_ONCE_ENABLED                            BIT1
#define N_HDC_PKG_CTL_SDC_WAS_ONCE_ENABLED                            1
#endif  // MINTREE_FLAG
/// @}

///
/// Other defines
///

//
// Local APIC defines
//
#define APIC_REGISTER_LOCAL_ID_OFFSET         0x00000020
#define APIC_REGISTER_APIC_VERSION_OFFSET     0x00000030
#define APIC_REGISTER_SPURIOUS_VECTOR_OFFSET  0x000000F0
#define APIC_REGISTER_ICR_LOW_OFFSET          0x00000300
#define APIC_REGISTER_ICR_HIGH_OFFSET         0x00000310
#define APIC_REGISTER_LINT0_VECTOR_OFFSET     0x00000350
#define APIC_REGISTER_LINT1_VECTOR_OFFSET     0x00000360

#define BROADCAST_MODE_SPECIFY_CPU            0x00
#define BROADCAST_MODE_ALL_INCLUDING_SELF     0x01
#define BROADCAST_MODE_ALL_EXCLUDING_SELF     0x02

#ifndef DELIVERY_MODE_FIXED
#define DELIVERY_MODE_FIXED           0x0
#endif
#ifndef DELIVERY_MODE_LOWEST_PRIORITY
#define DELIVERY_MODE_LOWEST_PRIORITY 0x1
#endif
#ifndef DELIVERY_MODE_SMI
#define DELIVERY_MODE_SMI             0x2
#endif
#ifndef DELIVERY_MODE_REMOTE_READ
#define DELIVERY_MODE_REMOTE_READ     0x3
#endif
#ifndef DELIVERY_MODE_NMI
#define DELIVERY_MODE_NMI             0x4
#endif
#ifndef DELIVERY_MODE_INIT
#define DELIVERY_MODE_INIT            0x5
#endif
#ifndef DELIVERY_MODE_SIPI
#define DELIVERY_MODE_SIPI            0x6
#endif
#ifndef DELIVERY_MODE_MAX
#define DELIVERY_MODE_MAX             0x7
#endif

#ifndef TRIGGER_MODE_EDGE
#define TRIGGER_MODE_EDGE             0x00
#endif
#ifndef TRIGGER_MODE_LEVEL
#define TRIGGER_MODE_LEVEL            0x01
#endif

#ifndef CPU_FEATURE_DISABLE
#define CPU_FEATURE_DISABLE  0
#endif
#ifndef CPU_FEATURE_ENABLE
#define CPU_FEATURE_ENABLE   1
#endif

#define CACHE_UNCACHEABLE               0
#define CACHE_WRITECOMBINING            1
#define CACHE_WRITETHROUGH              4
#define CACHE_WRITEPROTECTED            5
#define CACHE_WRITEBACK                 6

#ifndef MINTREE_FLAG
#define TXT_PUBLIC_BASE                                               0xFED30000
#define R_CPU_BOOT_GUARD_ERRORCODE                                    0x30
#define R_CPU_BOOT_GUARD_BOOTSTATUS                                   0xA0
#define R_CPU_BOOT_GUARD_ACM_STATUS                                   0x328
#define V_CPU_BOOT_GUARD_LOAD_ACM_SUCCESS                             0x8000000000000000
#define B_BOOT_GUARD_ACM_ERRORCODE_MASK                               0x00007FF0
#endif  // MINTREE_FLAG

//
// Processor Definitions
//
#define CPUID_FULL_STEPPING                      0x0000000F
#define CPUID_FULL_FAMILY_MODEL                  0x0FFF0FF0
#define CPUID_FULL_FAMILY_MODEL_STEPPING         0x0FFF0FFF
#define CPUID_FULL_FAMILY_MODEL_SKYLAKE_ULT_ULX  0x000406E0
#define CPUID_FULL_FAMILY_MODEL_SKYLAKE_DT_HALO  0x000506E0
#define CPUID_FULL_FAMILY_MODEL_KABYLAKE_ULT_ULX 0x000806E0
#define CPUID_FULL_FAMILY_MODEL_KABYLAKE_DT_HALO 0x000906E0

#ifndef STALL_ONE_MICRO_SECOND
#define STALL_ONE_MICRO_SECOND 1
#endif
#ifndef STALL_ONE_MILLI_SECOND
#define STALL_ONE_MILLI_SECOND 1000
#endif

#define BITS(x) (1 << (x))

/**
Notes :
  1.  Bit position always starts at 0.
  2.  Following macros are applicable only for Word aligned integers.
**/
#define BIT(Pos, Value)               (1 << (Pos) & (Value))
#define BITRANGE(From, Width, Value)  (((Value) >> (From)) & ((1 << (Width)) - 1))

///
/// Enums for CPU Family IDs
///
typedef enum {
  EnumCpuSklUltUlx    = CPUID_FULL_FAMILY_MODEL_SKYLAKE_ULT_ULX,
  EnumCpuSklDtHalo    = CPUID_FULL_FAMILY_MODEL_SKYLAKE_DT_HALO,
  EnumCpuKblUltUlxG0  = EnumCpuSklUltUlx,
  EnumCpuKblDtHaloA0  = EnumCpuSklDtHalo,
  EnumCpuKblUltUlx    = CPUID_FULL_FAMILY_MODEL_KABYLAKE_ULT_ULX,
  EnumCpuKblDtHalo    = CPUID_FULL_FAMILY_MODEL_KABYLAKE_DT_HALO,
  EnumCpuMax          = CPUID_FULL_FAMILY_MODEL
} CPU_FAMILY;

///
/// Enums for CPU Stepping IDs
///
typedef enum {
  ///
  /// Skylake ULX/ULT Steppings
  ///
  EnumSklB0         = 1,
  EnumSklJ0         = 2,
  EnumSklC0         = 2,
  EnumSklK0         = 3,
  EnumSklD0         = 3,
  EnumSklMaxUltUlxStep = EnumSklD0,

  ///
  /// Kabylake ULX/ULT Steppings
  ///
  EnumKblG0         = 8,
  EnumKblH0         = 9,
  EnumKblJ0         = 9,
  EnumKblY0         = 0xA,
  EnumAmlW0         = 0xB,
  EnumAmlV0         = 0xC,
  EnumKblMaxUltUlxStep = EnumAmlV0,

  ///
  /// Skylake DT/Halo Steppings
  ///
  EnumSklP0         = 0,
  EnumSklQ0         = 1,
  EnumSklM0         = 2,
  EnumSklR0         = 3,
  EnumSklS0         = 3,
  EnumSklN0         = 3,
  EnumSklMaxDtHaloStep = EnumSklN0,

  ///
  /// Kabylake DT/Halo Steppings
  ///
  EnumKblA0         = 8,
  EnumKblB0         = 9,
  EnumKblS0         = 9,
  EnumKblM0         = 9,
  EnumKblN0         = 0xA,
  EnumCflB0         = 0xB,
  EnumCflP0         = 0xC,
  EnumCflR0         = 0xD,
  EnumKblMaxDtHaloStep = EnumCflR0,

  ///
  /// Max Stepping
  ///
  EnumCpuSteppingMax  = CPUID_FULL_STEPPING
} CPU_STEPPING;

///
/// Enums for CPU SKU IDs
///
typedef enum {
  EnumCpuUlt        = 0,
  EnumCpuTrad,
  EnumCpuUlx,
  EnumCpuHalo,
  EnumCpuUnknown
} CPU_SKU;

///
/// Enums for CPU Generation
///
typedef enum {
  EnumSklCpu  = 0,
  EnumKblCpu,
  EnumCpuUnknownGeneration
} CPU_GENERATION;
#endif
