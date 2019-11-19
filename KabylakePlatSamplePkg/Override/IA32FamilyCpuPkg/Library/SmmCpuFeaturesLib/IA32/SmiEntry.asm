;
; This file contains an 'Intel Peripheral Driver' and is
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may
; be modified by the user, subject to additional terms of the
; license agreement
;
;------------------------------------------------------------------------------
;
; Copyright (c) 2009 - 2015, Intel Corporation. All rights reserved.<BR>
; This software and associated documentation (if any) is furnished
; under a license and may only be used or copied in accordance
; with the terms of the license. Except as permitted by such
; license, no part of this software or documentation may be
; reproduced, stored in a retrieval system, or transmitted in any
; form or by any means without the express written consent of
; Intel Corporation.
;
; Module Name:
;
;   SmiEntry.asm
;
; Abstract:
;
;   Code template of the SMI handler for a particular processor
;
;------------------------------------------------------------------------------

    .686p
    .model  flat,C
    .xmm

MSR_EFER      EQU     0c0000080h
MSR_EFER_XD   EQU     0800h

DSC_OFFSET    EQU     0fb00h
DSC_GDTPTR    EQU     30h
DSC_GDTSIZ    EQU     38h
DSC_CS        EQU     14
DSC_DS        EQU     16
DSC_SS        EQU     18
DSC_OTHERSEG  EQU     20

PROTECT_MODE_CS EQU   08h
PROTECT_MODE_DS EQU   20h
TSS_SEGMENT     EQU   40h

SmiRendezvous                PROTO   C
SmmFeatureCpuSmmDebugEntry   PROTO   C
SmmFeatureCpuSmmDebugExit    PROTO   C

EXTERNDEF   gcSmmFeatureSmiHandlerTemplate:BYTE
EXTERNDEF   gcSmmFeatureSmiHandlerSize:WORD
EXTERNDEF   gSmmFeatureSmiCr3:DWORD
EXTERNDEF   gSmmFeatureSmiStack:DWORD
EXTERNDEF   gSmmFeatureSmbase:DWORD
EXTERNDEF   gSmmFeatureXdSupported:BYTE
EXTERNDEF   FeaturePcdGet (PcdCpuSmmStackGuard):BYTE
EXTERNDEF   gcSmiPMHandlerTemplate:BYTE
EXTERNDEF   gcSmiPMHandlerSize:WORD
EXTERNDEF   gProtModeSmbase:DWORD
EXTERNDEF   gSmmFeatureSmiHandlerIdtr:FWORD
EXTERNDEF   gPMStackDesc:FWORD

    .const

align 16
gSmmFeatureSmiHandlerIdtr LABEL   FWORD
    DW 0
    DD 0
    
gPMStackDescOffset = offset gPMStackDesc - offset _SmiPMEntryPoint

    .code

gcSmmFeatureSmiHandlerTemplate    LABEL   BYTE

_SmiEntryPoint:
    DB      0bbh                        ; mov bx, imm16
    DW      offset _GdtDesc - _SmiEntryPoint + 8000h
    DB      2eh, 0a1h                   ; mov ax, cs:[offset16]
    DW      DSC_OFFSET + DSC_GDTSIZ
    dec     eax
    mov     cs:[edi], eax               ; mov cs:[bx], ax
    DB      66h, 2eh, 0a1h              ; mov eax, cs:[offset16]
    DW      DSC_OFFSET + DSC_GDTPTR
    mov     cs:[edi + 2], ax            ; mov cs:[bx + 2], eax
    mov     bp, ax                      ; ebp = GDT base
    DB      66h
    lgdt    fword ptr cs:[edi]          ; lgdt fword ptr cs:[bx]
; Patch ProtectedMode Segment
    DB      0b8h                        ; mov ax, imm16
    DW      PROTECT_MODE_CS             ; set AX for segment directly
    mov     cs:[edi - 2], eax           ; mov cs:[bx - 2], ax
; Patch ProtectedMode entry
    DB      66h, 0bfh                   ; mov edi, SMBASE
gSmmFeatureSmbase    DD    ?
    DB      67h
    lea     ax, [edi + (@32bit - _SmiEntryPoint) + 8000h]
    mov     cs:[edi - 6], ax            ; mov cs:[bx - 6], eax
    mov     ebx, cr0
    DB      66h
    and     ebx, 9ffafff3h
    DB      66h
    or      ebx, 23h
    mov     cr0, ebx
    DB      66h, 0eah
    DD      ?
    DW      ?
_GdtDesc    FWORD   ?

@32bit:
    mov     ax, PROTECT_MODE_DS
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     ss, ax
    DB      0bch                   ; mov esp, imm32
gSmmFeatureSmiStack   DD      ?
    mov     eax, offset gSmmFeatureSmiHandlerIdtr
    lidt    fword ptr [eax]
    jmp     ProtFlatMode

gcSmiPMHandlerTemplate    LABEL   BYTE
_SmiPMEntryPoint:
; if SMM PROT MODE feature is ok, processor will break here with 32bit protected mode
    DB      0bfh                        ; mov edi, SMBASE
gProtModeSmbase    DD    ?
    ; reload SS:ESP, it should be done in one instruction
    mov     eax, gPMStackDescOffset
    add     eax, edi
    add     eax, 08000h
    lss     esp, fword ptr [eax]

    ; reload CS:EIP
    mov     eax, edi
    add     eax, ProtFlatMode - _SmiPMEntryPoint + 8000h
    mov     dx, PROTECT_MODE_CS
    mov     [eax - 2], dx               ; mov cs:[bx - 6], eax
    mov     [eax - 6], eax              ; mov cs:[bx - 6], eax

    DB      0eah                        ; jmp @ProtFlatMode
    DD      ?
    DW      ?

ProtFlatMode:
    DB      0b8h                        ; mov eax, imm32
gSmmFeatureSmiCr3     DD      ?
    mov     cr3, eax
;
; Need to test for CR4 specific bit support
;
    mov     eax, 1
    cpuid                               ; use CPUID to determine if specific CR4 bits are supported
    xor     eax, eax                    ; Clear EAX
    test    edx, BIT2                   ; Check for DE capabilities
    jz      @f
    or      eax, BIT3
@@:
    test    edx, BIT6                   ; Check for PAE capabilities
    jz      @f
    or      eax, BIT5
@@:
    test    edx, BIT7                   ; Check for MCE capabilities
    jz      @f
    or      eax, BIT6
@@:
    test    edx, BIT24                  ; Check for FXSR capabilities
    jz      @f
    or      eax, BIT9
@@:
    test    edx, BIT25                  ; Check for SSE capabilities
    jz      @f
    or      eax, BIT10
@@:                                     ; as cr4.PGE is not set here, refresh cr3
    mov     cr4, eax                    ; in PreModifyMtrrs() to flush TLB.

; enable NXE if supported
    DB      0b0h                        ; mov al, imm8
gSmmFeatureXdSupported     DB      0
    cmp     al, 0
    jz      @f
    mov     ecx, MSR_EFER
    rdmsr
    or      ax, MSR_EFER_XD             ; enable NXE
    wrmsr
@@:

    mov     ebx, cr0
    or      ebx, 080000000h             ; enable paging + WP
    mov     cr0, ebx
    lea     ebx, [edi + DSC_OFFSET]
    mov     ax, [ebx + DSC_DS]
    mov     ds, eax
    mov     ax, [ebx + DSC_OTHERSEG]
    mov     es, eax
    mov     fs, eax
    mov     gs, eax
    mov     ax, [ebx + DSC_SS]
    mov     ss, eax

    cmp     FeaturePcdGet (PcdCpuSmmStackGuard), 0
    jz      @F

; Load TSS
    mov     byte ptr [ebp + TSS_SEGMENT + 5], 89h ; clear busy flag
    mov     eax, TSS_SEGMENT
    ltr     ax
@@:
;   jmp     _SmmFeatureSmiHandler                 ; instruction is not needed

_SmmFeatureSmiHandler PROC
    mov     ebx, [esp]                  ; CPU Index

    push    ebx
    mov     eax, SmmFeatureCpuSmmDebugEntry
    call    eax
    pop     ebx

    push    ebx
    mov     eax, SmiRendezvous
    call    eax
    pop     ebx
    
    push    ebx
    mov     eax, SmmFeatureCpuSmmDebugExit
    call    eax
    pop     ebx

    rsm
_SmmFeatureSmiHandler ENDP

align 16
; used by LSS
gPMStackDesc LABEL   FWORD
    DD 0 ; to be fixed
    DD PROTECT_MODE_DS

gcSmmFeatureSmiHandlerSize    DW      $ - _SmiEntryPoint
gcSmiPMHandlerSize  DW      $ - _SmiPMEntryPoint

    END
