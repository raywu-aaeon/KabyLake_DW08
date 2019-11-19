//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
#------------------------------------------------------------------------------
#
# Copyright (c) 2009 - 2015, Intel Corporation. All rights reserved.<BR>
# This software and associated documentation (if any) is furnished
# under a license and may only be used or copied in accordance
# with the terms of the license. Except as permitted by such
# license, no part of this software or documentation may be
# reproduced, stored in a retrieval system, or transmitted in any
# form or by any means without the express written consent of
# Intel Corporation.
#
# Module Name:
#
#   SmiException.S
#
# Abstract:
#
#   Exception handlers used in SM mode
#
#------------------------------------------------------------------------------
ASM_GLOBAL  ASM_PFX(gSmiExceptionCr3)
ASM_GLOBAL  ASM_PFX(mMceHandlerLongMode)
ASM_GLOBAL  ASM_PFX(gSmmFeatureSmiHandlerIdtr)

#
# BUGBUG: Need to figure out how to get these values from GDT
# established in SMM module
#
.equ  CODE_SEL,    0x38
.equ  DATA_SEL,    0x20
.equ  CODE32_SEL,  0x08

    .text

ASM_GLOBAL ASM_PFX(PMIdtHandler)
ASM_PFX(PMIdtHandler):
    jmp    .
    retf

ASM_GLOBAL ASM_PFX(PMIdtHandlerMachineCheck)
ASM_PFX(PMIdtHandlerMachineCheck):
    subl    $8, %esp
    sidt    (%rsp)                     # save 32bit IDTR
    pushq   %rax
    pushq   %rdx
    pushq   %rcx

    .byte   0xb8                       # mov eax, offset gSmiExceptionCr3
ASM_PFX(gSmiExceptionCr3):   .space  4
    movq    %rax, %cr3

    movl    $0x668, %eax               # as cr4.PGE is not set here, refresh cr3
    movq    %rax, %cr4                 # in PreModifyMtrrs() to flush TLB.

# save old ss:esp
    movl    %ss, %eax
    pushq   %rax
    pushq   %rsp
    cmpw    $DATA_SEL, %ax             # check if ss:esp have been updated
    jz      Skip                       # if not, update ss:esp

# change ss:esp
    sgdt    -8(%rsp)
    movl    -6(%rsp), %ecx
    addl     %eax, %ecx

    xorl    %eax, %eax
    movw    2(%rcx), %ax
    xorl    %edx, %edx
    movb    4(%rcx), %dl
    movb    7(%rcx), %dh
    shl     $0x10, %edx
    orl     %edx, %eax
    addl    %eax, %esp
    movl    $DATA_SEL, %eax
    movw    %ax, %ss

Skip:
    movq    %cr4, %rax
    pushq   %rax                        # save cr4
    movl    $0xC0000080, %ecx
    rdmsr
    pushq   %rax                        # save MSR(0xc0000080)
    movq    %cr0, %rax
    pushq   %rax                        # save cr0

    movl    $CODE_SEL, %eax
    pushq   %rax                        # push cs hardcore
    call    Base                        # push reture address for retf later
Base:
    addl    $(LongMode - Base), (%rsp)  # offset for far retf, seg is the 1st arg
    movq    %cr4, %rax
    orb     $(1 << 5), %al              #??????
    movq    %rax, %cr4                  # enable PAE
    
#
# Need to test for XD support
#
    movl    $0x80000000, %eax
    cpuid
    cmp     $0x80000000, %eax
    jbe     L53
    movl    $0x80000001, %eax
    cpuid
    testl   $BIT20, %edx
    jz      L53
    movl    $0xc0000080, %ecx
    rdmsr
    orb     $8,%ah                      # enable NXE
    wrmsr
L53:
    
    movl    $0xc0000080, %ecx
    rdmsr
    orb     $1,%ah                      # enable LME
    wrmsr
    
    movq    %cr0, %rbx
    orl     $0x080010000, %ebx          # enable paging + WP
    movq    %rbx, %cr0                  # enable paging
    retf
LongMode:                               # long mode starts here
    movq    ASM_PFX(gSmmFeatureSmiHandlerIdtr)(%rip), %rax    # load long mode IDT table to handle the exception
    lidt    (%rax)                            # that maybe issued in 64bit MC exception handler
    movq    %rsp, %rcx
    andl    $0x0fffffff0, %esp
    xorq    %rax, %rax
    movw    %ss, %ax
    pushq   %rax
    pushq   %rcx
    addq    $32, %rcx
    movl    8(%rcx), %eax
    pushq   %rax                        # old eflags
    xorq    %rax, %rax
    movw    %cs, %ax                    # cs
    pushq   %rax
    movabsq $BackPmIdtHandler, %rax      # return EIP
    pushq   %rax
    movabsq $ASM_PFX(mMceHandlerLongMode), %rax
    pushq   (%rax)                      # long mode MCA handle
    movl     -8(%rcx), %eax
    movl    -12(%rcx), %edx
    movl    -16(%rcx), %ecx

    addq    $8, %rsp
    jmp     *-8(%rsp)                   # jmp to long mode MCA handle

BackPmIdtHandler:
    #
    # let rax save DS
    #
    movq    $DATA_SEL, %rax

    #
    # Change to Compatible Segment
    #
    movq    $CODE32_SEL, %rcx           # load compatible mode selector
    shlq    $32, %rcx
    movabsq $Compatible, %rdx            # assume address < 4G
    orq     %rdx, %rcx
    pushq   %rcx
    retf

Compatible:
    addl    $16, %esp
    lss     -4(%rsp), %esp              # restore ss:esp
    subl    $16, %esp

    popq    %rax
    movq    %rax, %cr0                  # restore cr0 to disable paging.

    movl    $0xC0000080, %ecx
    rdmsr
    popq    %rax
    wrmsr                               # restore MSR (0xc0000080) to clear EFER.LME

    popq    %rax
    movq    %rax, %cr4                  # restore cr4 to clear PAE

    addl    $8, %esp                    # skip old ss and esp
    popq    %rcx
    popq    %rdx
    popq    %rax
    lidt    (%rsp)                      # load saved 32bit IDTR
    addl    $(12 + 8), %esp
    #
    # retf maybe used for MCE
    #
    pushq   -8(%rsp)
    pushq   -8(%rsp)
    retf
