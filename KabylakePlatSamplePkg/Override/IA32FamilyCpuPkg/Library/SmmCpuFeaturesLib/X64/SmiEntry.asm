;
; This file contains an 'Intel Peripheral Driver' and is
; licensed for Intel CPUs and chipsets under the terms of your
; license agreement with Intel or your vendor.  This file may
; be modified by the user, subject to additional terms of the
; license agreement
;
;------------------------------------------------------------------------------
;
; Copyright (c) 2009 - 2019, Intel Corporation. All rights reserved.<BR>
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

;
; Variables referrenced by C code
;
EXTERNDEF   SmiRendezvous:PROC
EXTERNDEF   SmmFeatureCpuSmmDebugEntry:PROC
EXTERNDEF   SmmFeatureCpuSmmDebugExit:PROC
EXTERNDEF   gcSmmFeatureSmiHandlerTemplate:BYTE
EXTERNDEF   gcSmmFeatureSmiHandlerSize:WORD
EXTERNDEF   gSmmFeatureSmiCr3:DWORD
EXTERNDEF   gSmmFeatureSmiStack:DWORD
EXTERNDEF   gSmmFeatureSmbase:DWORD
EXTERNDEF   gSmmFeatureXdSupported:BYTE
EXTERNDEF   gcSmiPMHandlerTemplate:BYTE
EXTERNDEF   gcSmiPMHandlerSize:WORD
EXTERNDEF   gProtModeSmbase:DWORD
EXTERNDEF   gSmmFeatureSmiHandlerIdtr:FWORD
EXTERNDEF   mSmmProtectedModeEnable:BYTE
EXTERNDEF   gPMStackDesc:FWORD
EXTERNDEF   gSmmUsableStackSize:DWORD

MSR_EFER      EQU     0c0000080h
MSR_EFER_XD   EQU     0800h

;
; Constants relating to TXT_PROCESSOR_SMM_DESCRIPTOR
;
DSC_OFFSET    EQU     0fb00h
DSC_GDTPTR    EQU     48h   ; RPPO-SKL-0018
DSC_GDTSIZ    EQU     50h   ; RPPO-SKL-0018
DSC_CS        EQU     14h   ; RPPO-SKL-0018
DSC_DS        EQU     16h   ; RPPO-SKL-0018
DSC_SS        EQU     18h   ; RPPO-SKL-0018
DSC_OTHERSEG  EQU     1ah   ; RPPO-SKL-0018

;
; Constants relating to CPU State Save Area
;
SSM_DR6         EQU     0ffd0h
SSM_DR7         EQU     0ffc8h

PROTECT_MODE_CS EQU     10h ; RPPO-SKL-0018
PROTECT_MODE_DS EQU     18h ; RPPO-SKL-0018
LONG_MODE_CS    EQU     38h
TSS_SEGMENT     EQU     40h
GDT_SIZE        EQU     50h

    .const

align 16
gSmmFeatureSmiHandlerIdtr LABEL   FWORD
    DW 0
    DQ 0
    
gPMStackDescOffset = offset gPMStackDesc - offset _SmiPMEntryPoint

    .code

gcSmmFeatureSmiHandlerTemplate    LABEL   BYTE

_SmiEntryPoint:
    ;
    ; The encoding of BX in 16-bit addressing mode is the same as of RDI in 64-
    ; bit addressing mode. And that coincidence has been used in the following
    ; "64-bit like" 16-bit code. Be aware that once RDI is referenced as a
    ; base address register, it is actually BX that is referenced.
    ;
    DB      0bbh                        ; mov bx, imm16
    DW      offset _GdtDesc - _SmiEntryPoint + 8000h  ; bx = GdtDesc offset
; fix GDT descriptor
    DB      2eh, 0a1h                   ; mov ax, cs:[offset16]
    DW      DSC_OFFSET + DSC_GDTSIZ
    DB      48h                         ; dec ax
    DB      2eh
    mov     [rdi], eax                  ; mov cs:[bx], ax
    DB      66h, 2eh, 0a1h              ; mov eax, cs:[offset16]
    DW      DSC_OFFSET + DSC_GDTPTR
    DB      2eh
    mov     [rdi + 2], ax               ; mov cs:[bx + 2], eax
    DB      66h, 2eh
    lgdt    fword ptr [rdi]             ; lgdt fword ptr cs:[bx]
; Patch ProtectedMode Segment
    DB      0b8h                        ; mov ax, imm16
    DW      PROTECT_MODE_CS             ; set AX for segment directly
    DB      2eh
    mov     [rdi - 2], eax              ; mov cs:[bx - 2], ax
; Patch ProtectedMode entry
    DB      66h, 0bfh                   ; mov edi, SMBASE
gSmmFeatureSmbase    DD    ?
    lea     ax, [edi + (@ProtectedMode - _SmiEntryPoint) + 8000h]
    DB      2eh
    mov     [rdi - 6], ax               ; mov cs:[bx - 6], eax
; Switch into @ProtectedMode
    mov     rbx, cr0
    DB      66h
    and     ebx, 9ffafff3h
    DB      66h
    or      ebx, 00000023h

    mov     cr0, rbx
    DB      66h, 0eah
    DD      ?
    DW      ?

_GdtDesc    FWORD   ?
@ProtectedMode:
    mov     ax, PROTECT_MODE_DS
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     ss, ax
    DB      0bch                   ; mov esp, imm32
gSmmFeatureSmiStack   DD      ?
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
    lss     esp, fword ptr [rax]

    ; reload CS:EIP
    mov     eax, edi
    add     eax, ProtFlatMode - _SmiPMEntryPoint + 8000h
    mov     dx, PROTECT_MODE_CS
    mov     [rax - 2], dx               ; mov cs:[bx - 6], eax
    mov     [rax - 6], eax              ; mov cs:[bx - 6], eax

    DB      0eah                        ; jmp @ProtFlatMode
    DD      ?
    DW      ?

ProtFlatMode:
    DB      0b8h                        ; mov eax, offset gSmmFeatureSmiCr3
gSmmFeatureSmiCr3     DD      ?
    mov     cr3, rax
    mov     eax, 668h                   ; as cr4.PGE is not set here, refresh cr3
    mov     cr4, rax                    ; in PreModifyMtrrs() to flush TLB.
; Load TSS
    sub     esp, 8                      ; reserve room in stack
    sgdt    fword ptr [rsp]
    mov     eax, [rsp + 2]              ; eax = GDT base
    add     esp, 8
    mov     dl, 89h
    mov     [rax + TSS_SEGMENT + 5], dl ; clear busy flag
    mov     eax, TSS_SEGMENT
    ltr     ax

; Switch into @LongMode
    push    LONG_MODE_CS                ; push cs hardcore here
    call    Base                       ; push reture address for retf later
Base:
    add     dword ptr [rsp], @LongMode - Base; offset for far retf, seg is the 1st arg

; enable NXE if supported
    DB      0b0h                        ; mov al, imm8
gSmmFeatureXdSupported     DB      0
    cmp     al, 0
    jz      @f
    mov     ecx, MSR_EFER
    rdmsr
    or      ax, MSR_EFER_XD            ; enable NXE
    wrmsr
@@:

    mov     ecx, MSR_EFER
    rdmsr
    or      ah, 1                      ; enable LME
    wrmsr
    mov     rbx, cr0
    or      ebx, 080000000h            ; enable paging + WP
    mov     cr0, rbx
    retf
@LongMode:                              ; long mode (64-bit code) starts here
    mov     rax, offset gSmmFeatureSmiHandlerIdtr
    lidt    fword ptr [rax]
    lea     ebx, [rdi + DSC_OFFSET]
    mov     ax, [rbx + DSC_DS]
    mov     ds, eax
    mov     ax, [rbx + DSC_OTHERSEG]
    mov     es, eax
    mov     fs, eax
    mov     gs, eax
    mov     ax, [rbx + DSC_SS]
    mov     ss, eax
;   jmp     _SmmFeatureSmiHandler                 ; instruction is not needed

_SmmFeatureSmiHandler:
    mov     rbx, [rsp]                  ; rcx <- CpuIndex

    ;
    ; Save FP registers
    ;
    sub     rsp, 208h
    DB      48h                         ; FXSAVE64
    fxsave  [rsp]

    add     rsp, -20h
    
    mov     rcx, rbx
    mov     rax, SmmFeatureCpuSmmDebugEntry
    call    rax

    mov     rcx, rbx
    mov     rax, SmiRendezvous          ; rax <- absolute addr of SmiRedezvous
    call    rax
    
    mov     rcx, rbx
    mov     rax, SmmFeatureCpuSmmDebugExit
    call    rax

    add     rsp, 20h

    ;
    ; Restore FP registers
    ;
    DB      48h                         ; FXRSTOR64
    fxrstor [rsp]

    add     rsp, 208h

    ;
    ; Clear stack
    ;
    xor     rax, rax
    lea     eax, offset gSmmUsableStackSize
    xor     rbx, rbx
    mov     ebx, [rax]
    lea     rax, [rsp]
    add     rax, 0x1000
    and     rax, 0xFFFFF000
    sub     rax, rbx
    lea     rcx, [rsp]
    sub     rcx, rax
    lea     rdi, [rsp - 1]
    xor     rax, rax
    std
    rep stosb
    cld

    rsm

align 16
gSmmUsableStackSize LABEL   DWORD
    DD 0

align 16
; used by LSS
gPMStackDesc LABEL   FWORD
    DD 0 ; to be fixed
    DD PROTECT_MODE_DS

gcSmmFeatureSmiHandlerSize    DW      $ - _SmiEntryPoint
gcSmiPMHandlerSize  DW      $ - _SmiPMEntryPoint

    END
