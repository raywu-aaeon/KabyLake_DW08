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
;   SmiException.asm
;
; Abstract:
;
;   Exception handlers used in SM mode
;
;------------------------------------------------------------------------------
EXTERNDEF   gSmiExceptionCr3:DWORD
EXTERNDEF   mMceHandlerLongMode:QWORD
EXTERNDEF   gSmmFeatureSmiHandlerIdtr:FWORD

;
; BUGBUG: Need to figure out how to get these values from GDT
; established in SMM module
;
CODE_SEL          = 038h
DATA_SEL          = 020h
CODE32_SEL        = 08h

    .code

PMIdtHandler PROC
    jmp    $
    retf
PMIdtHandler ENDP

PMIdtHandlerMachineCheck PROC
    sub     esp, 8
    sidt    fword ptr [rsp]            ; save 32bit IDTR
    push    rax
    push    rdx
    push    rcx

    DB      0b8h                       ; mov eax, offset gSmiExceptionCr3
gSmiExceptionCr3     DD      ?
    mov     cr3, rax

    mov     eax, 668h                  ; as cr4.PGE is not set here, refresh cr3
    mov     cr4, rax                   ; in PreModifyMtrrs() to flush TLB.

; save old ss:esp
    mov     eax, ss
    push    rax
    push    rsp
    cmp     eax, DATA_SEL              ; check if ss:esp have been updated
    jz      Skip                       ; if not, update ss:esp

; change ss:esp
    sgdt    fword ptr [rsp - 8]
    mov     ecx, dword ptr [rsp - 6]
    add     ecx, eax

    xor     eax, eax
    mov     ax, word ptr [rcx + 2]
    xor     edx, edx
    mov     dl, byte ptr [rcx + 4]
    mov     dh, byte ptr [rcx + 7]
    shl     edx, 10h
    or      eax, edx
    add     esp, eax
    mov     eax, DATA_SEL
    mov     ss, ax

Skip:
    mov     rax, cr4
    push    rax                         ; save cr4
    mov     ecx, 0C0000080h
    rdmsr
    push    rax                         ; save MSR(0xc0000080)
    mov     rax, cr0
    push    rax                         ; save cr0

    mov     eax, CODE_SEL
    push    rax                         ; push cs hardcore
    call    Base                       ; push reture address for retf later
Base:
    add     dword ptr [rsp], LongMode - Base ; offset for far retf, seg is the 1st arg
    mov     rax, cr4
    or      al, (1 SHL 5)
    mov     cr4, rax                    ; enable PAE

;
; Need to test for XD support
;
    mov     eax, 080000000h
    cpuid
    cmp     eax, 080000000h
    jbe     @f
    mov     eax, 080000001h
    cpuid
    test    edx, BIT20
    jz      @f
    mov     ecx, 0c0000080h
    rdmsr
    or      ah, 8                      ; enable NXE
    wrmsr
@@:
    mov     ecx, 0c0000080h
    rdmsr
    or      ah, 1                      ; enable LME
    wrmsr

    mov     rbx, cr0
    or      ebx, 080010000h            ; enable paging + WP
    mov     cr0, rbx                    ; enable paging
    retf
LongMode:                               ; long mode starts here
    mov     rax, offset gSmmFeatureSmiHandlerIdtr ; load long mode IDT table to handle the exception
    lidt    fword ptr [rax]             ; that maybe issued in 64bit MC exception handler
    mov     rcx, rsp
    and     rsp, 0fffffff0h
    xor     rax, rax
    mov     ax,  ss
    push    rax
    push    rcx
    add     rcx, 32
    mov     eax, dword ptr [rcx + 8]
    push    rax                         ; old eflags
    xor     rax, rax
    mov     ax,  cs                     ; cs
    push    rax
    mov     rax, BackPmIdtHandler      ; return EIP
    push    rax
    mov     rax, offset mMceHandlerLongMode
    push    [rax]                       ; long mode MCA handle
    mov     eax, dword ptr [rcx - 8]
    mov     edx, dword ptr [rcx - 12]
    mov     ecx, dword ptr [rcx - 16]

    add     rsp, 8
    jmp     qword ptr [rsp - 8]         ; jmp to long mode MCA handle

BackPmIdtHandler:
    ;
    ; let rax save DS
    ;
    mov     rax, DATA_SEL

    ;
    ; Change to Compatible Segment
    ;
    mov     rcx, CODE32_SEL             ; load compatible mode selector
    shl     rcx, 32
    mov     rdx, OFFSET Compatible      ; assume address < 4G
    or      rcx, rdx
    push    rcx
    retf

Compatible:
    pop     rax
    mov     cr0, rax                    ; restore cr0 to disable paging.

    mov     ecx, 0C0000080h
    rdmsr
    pop     rax
    wrmsr                               ; restore MSR (0xc0000080) to clear EFER.LME

    pop     rax
    mov     cr4, rax                    ; restore cr4 to clear PAE

    lss     esp, fword ptr [rsp]        ; restore ss:esp

    add     esp, 4                      ; skip old ss
    pop     rcx
    pop     rdx
    pop     rax
    lidt    fword ptr [rsp]             ; load saved 32bit IDTR
    add     esp, 12 + 8
    ;
    ; retf maybe used for MCE
    ;
    push    [rsp - 8]
    push    [rsp - 8]
    retf
PMIdtHandlerMachineCheck ENDP

    END
