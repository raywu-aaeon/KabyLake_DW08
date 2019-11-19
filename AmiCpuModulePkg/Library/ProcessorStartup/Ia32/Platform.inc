;; @file
;  Platform Specific Definitions
;
; @copyright
;  INTEL CONFIDENTIAL
;  Copyright 2008 - 2016 Intel Corporation.
;
;  The source code contained or described herein and all documents related to the
;  source code ("Material") are owned by Intel Corporation or its suppliers or
;  licensors. Title to the Material remains with Intel Corporation or its suppliers
;  and licensors. The Material may contain trade secrets and proprietary and
;  confidential information of Intel Corporation and its suppliers and licensors,
;  and is protected by worldwide copyright and trade secret laws and treaty
;  provisions. No part of the Material may be used, copied, reproduced, modified,
;  published, uploaded, posted, transmitted, distributed, or disclosed in any way
;  without Intel's prior express written permission.
;
;  No license under any patent, copyright, trade secret or other intellectual
;  property right is granted to or conferred upon you by disclosure or delivery
;  of the Materials, either expressly, by implication, inducement, estoppel or
;  otherwise. Any license under such intellectual property rights must be
;  express and approved by Intel in writing.
;
;  Unless otherwise agreed by Intel in writing, you may not remove or alter
;  this notice or any other notice embedded in Materials by Intel or
;  Intel's suppliers or licensors in any way.
;
;  This file contains a 'Sample Driver' and is licensed as such under the terms
;  of your license agreement with Intel or your vendor. This file may be modified
;  by the user, subject to the additional terms of the license agreement.
;
;@par Specification Reference:
;
;;
; AMI_OVERRIDE_START >>
EARLY_MICROCODE_SUPPORT       EQU 1

;----------------------------------------------------------------------------------------
; "Merlin" support used equates
;----------------------------------------------------------------------------------------
MAGIC_ADDRESS_IN_SEG          EQU  0FFF0h
MAGIC_SEG                     EQU  0F000h
; << AMI_OVERRIDE_END
PEI_CORE_ENTRY_BASE           EQU 0FFFFFFE0h
FV_MAIN_BASE                  EQU 0FFFFFFFCh
; AMI_OVERRIDE_START >>
MICROCODE_FV_BASE_ADDRESS     EQU MKF_FV_MICROCODE_BASE
CODE_CACHE_BASE_ADDRESS       EQU MKF_CODE_CACHE_BASE_ADDRESS      ; PcdGet32 (PcdNemCodeCacheBase)
FLASH_AREA_BASE_ADDRESS       EQU 0      ; PcdGet32 (PcdFlashAreaBaseAddress)
TEMPORARY_RAM_BASE_ADDRESS    EQU MKF_TemporaryRamBaseAddress      ; PcdGet32 (PcdTemporaryRamBase)
TEMPORARY_RAM_SIZE            EQU MKF_TotalTemporaryRamSize           ; PcdGet32 (PcdTemporaryRamSize)
; AMI_OVERRIDE_SA0023_START >>>
IFDEF AMI_PCIE_DYNAMIC_SUPPORT_FLAG
PCIEXPRESS_BASE_ADDRESS       EQU 0E0000000h                       ; For Dynamic PCIE_BASE_ADDRESS feature implementation
ELSE
PCIEXPRESS_BASE_ADDRESS       EQU MKF_PCIEX_BASE_ADDRESS
ENDIF
; AMI_OVERRIDE_SA0023_END <<<
; << AMI_OVERRIDE_END

BIT0                          EQU 01h
BIT1                          EQU 02h
BIT2                          EQU 04h
BIT3                          EQU 08h
BIT4                          EQU 10h
BIT5                          EQU 20h
BIT6                          EQU 40h
BIT7                          EQU 80h
BIT8                          EQU 100h
BIT9                          EQU 200h
BIT10                         EQU 400h
BIT11                         EQU 800h
BIT12                         EQU 1000h
BIT13                         EQU 2000h
BIT14                         EQU 4000h
BIT15                         EQU 8000h
BIT16                         EQU 10000h
BIT17                         EQU 20000h
BIT18                         EQU 40000h
BIT19                         EQU 80000h
BIT23                         EQU 0800000h
BIT31                         EQU 080000000h

BspApicIdSaveStart            EQU 24


