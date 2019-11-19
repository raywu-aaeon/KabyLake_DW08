;; @file
;  This file implements SEC system agent library
;
; @copyright
;  INTEL CONFIDENTIAL
;  Copyright 2014 - 2016 Intel Corporation.
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
;  This file contains an 'Intel Peripheral Driver' and is uniquely identified as
;  "Intel Reference Module" and is licensed for Intel CPUs and chipsets under
;  the terms of your license agreement with Intel or your vendor. This file may
;  be modified by the user, subject to additional terms of the license agreement.
;
;@par Specification Reference:
;;


INCLUDE SecSaLib.inc

.686p
.xmm
.model small, c

    .CODE

IFNDEF FSP_FLAG
; AMI_OVERRIDE_SA0023_START >>>
IFDEF AMI_PCIE_DYNAMIC_SUPPORT_FLAG
ELSE
EXTRN   PcdGet64 (PcdPciExpressBaseAddress):QWORD
EXTRN   PcdGet32 (PcdPciExpressRegionLength):DWORD
ENDIF
; AMI_OVERRIDE_SA0023_END <<<
ENDIF
EXTRN   PcdGet64 (PcdMchBaseAddress):QWORD
RET_EBP  MACRO

  jmp     ebp                           ; restore ESP from EBP

ENDM

IFDEF FSP_FLAG
FSPT_CFG_PCI_EXPRESS_BASE_OFF        EQU 048h ; FsptConfig.PcdPciExpressBaseAddress offset
FSPT_CFG_PCI_EXPRESS_LENGHTH_OFF     EQU 050h ; FsptConfig.PcdPciExpressRegionLength offset

EXTRN   AsmGetFspInfoHeaderNoStack:NEAR
CALL_EDI MACRO   RoutineLabel

  local   ReturnAddress
  mov     edi, offset ReturnAddress
  jmp     RoutineLabel
ReturnAddress:

ENDM
ENDIF

SecSaInit  PROC

IFDEF FSP_FLAG
;
; Get FSP-T Config region offset, to get the FSP-T UPD values
;
  CALL_EDI  AsmGetFspInfoHeaderNoStack
  mov   ecx,  DWORD PTR [eax + 01Ch]      ; Read FsptImageBaseAddress
  add   ecx,  DWORD PTR [eax + 024h]      ; Get Cfg Region base address = FsptImageBaseAddress + CfgRegionOffset
ENDIF
;
; Program PCI Express base address
;
  mov   eax,  PCIEX_BAR_REG
  mov   dx,   0CF8h
  out   dx,   eax
IFDEF FSP_FLAG
  mov   eax,  DWORD PTR [ecx + FSPT_CFG_PCI_EXPRESS_LENGHTH_OFF]    ; Read FsptConfig.PcdPciExpressRegionLength
ELSE
; AMI_OVERRIDE_SA0023_START >>>
IFDEF AMI_PCIE_DYNAMIC_SUPPORT_FLAG
  mov   eax,  DWORD PTR 010000000h
ELSE
  mov   eax,  DWORD PTR PcdGet32 (PcdPciExpressRegionLength)
ENDIF
; AMI_OVERRIDE_SA0023_END <<<
ENDIF
  mov   dx,   04h       ; Default is PCIEXBAR length is 64MB ([2:1] = 10b)
  cmp   eax,  10000000h ; Is it 256MB length?
  jne   Check128MB
  mov   dx,   00h       ; Set PCIEXBAR length as 256MB ([2:1] = 00b)
  jmp   ProgramLength
Check128MB:
  cmp   eax,  8000000h  ; Is it 128MB length?
  jne   ProgramLength
  mov   dx,   02h       ; Set PCIEXBAR length as 128MB ([2:1] = 01b)
ProgramLength:
IFDEF FSP_FLAG
  mov   eax,  DWORD PTR [ecx + FSPT_CFG_PCI_EXPRESS_BASE_OFF]      ; Read FsptConfig.PcdPciExpressBaseAddress
ELSE
; AMI_OVERRIDE_SA0023_START >>>
IFDEF AMI_PCIE_DYNAMIC_SUPPORT_FLAG
  mov   eax,  DWORD PTR 0E0000000h
ELSE
  mov   eax,  DWORD PTR PcdGet64 (PcdPciExpressBaseAddress)
ENDIF
; AMI_OVERRIDE_SA0023_END <<<
ENDIF
  or    ax,  dx
  or    eax,  (0 OR 1)  ;  PCIEXBAREN = 1
  mov   dx,   0CFCh
  out   dx,   eax

;
; Enable MCH Bar
;
IFDEF FSP_FLAG
  mov     esi, DWORD PTR [ecx + FSPT_CFG_PCI_EXPRESS_BASE_OFF]     ; Read FsptConfig.PcdPciExpressBaseAddress
ELSE
; AMI_OVERRIDE_SA0023_START >>>
IFDEF AMI_PCIE_DYNAMIC_SUPPORT_FLAG
  mov     esi, DWORD PTR 0E0000000h
ELSE
  mov     esi, DWORD PTR PcdGet64 (PcdPciExpressBaseAddress)
ENDIF
; AMI_OVERRIDE_SA0023_END <<<
ENDIF
  add     esi, MCHBAR_REG
  mov     eax, DWORD PTR PcdGet64 (PcdMchBaseAddress)
  or      eax, 1
  mov     Dword Ptr [esi], eax
;
; Clear upper 32bit for MCHBAR
;
  xor     eax, eax
  mov     Dword Ptr [esi + 4], eax
  RET_EBP
SecSaInit  ENDP

END
