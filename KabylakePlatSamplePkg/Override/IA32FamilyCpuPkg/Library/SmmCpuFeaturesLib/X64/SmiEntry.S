//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
#------------------------------------------------------------------------------
#
# Copyright (c) 2009 - 2016, Intel Corporation. All rights reserved.<BR>
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
#   SmiEntry.S
#
# Abstract:
#
#   Code template of the SMI handler for a particular processor
#
#------------------------------------------------------------------------------

ASM_GLOBAL  ASM_PFX(gcSmmFeatureSmiHandlerTemplate)
ASM_GLOBAL  ASM_PFX(gcSmmFeatureSmiHandlerSize)
ASM_GLOBAL  ASM_PFX(gSmmFeatureSmiCr3)
ASM_GLOBAL  ASM_PFX(gSmmFeatureSmiStack)
ASM_GLOBAL  ASM_PFX(gSmmFeatureSmbase)
ASM_GLOBAL  ASM_PFX(gSmmFeatureXdSupported)
ASM_GLOBAL  ASM_PFX(gcSmiPMHandlerTemplate)
ASM_GLOBAL  ASM_PFX(gcSmiPMHandlerSize)
ASM_GLOBAL  ASM_PFX(gProtModeSmbase)
ASM_GLOBAL  ASM_PFX(gSmmFeatureSmiHandlerIdtr)
ASM_GLOBAL  ASM_PFX(mSmmProtectedModeEnable)
ASM_GLOBAL  ASM_PFX(gPMStackDesc)
# AMI_OVERRIDE_START >>> EIP494432 - Intel missing the .S part of code change, which will cause GCC build fail.
ASM_GLOBAL  ASM_PFX(gSmmUsableStackSize)
# AMI_OVERRIDE_END <<< EIP494432 - Intel missing the .S part of code change, which will cause GCC build fail.

.equ            MSR_EFER, 0xc0000080
.equ            MSR_EFER_XD, 0x800

#
# Constants relating to TXT_PROCESSOR_SMM_DESCRIPTOR
#
.equ            DSC_OFFSET, 0xfb00
.equ            DSC_GDTPTR, 0x48    # RPPO-SKL-0018
.equ            DSC_GDTSIZ, 0x50    # RPPO-SKL-0018
.equ            DSC_CS, 0x14        # RPPO-SKL-0018
.equ            DSC_DS, 0x16        # RPPO-SKL-0018
.equ            DSC_SS, 0x18        # RPPO-SKL-0018
.equ            DSC_OTHERSEG, 0x1A  # RPPO-SKL-0018

#
# Constants relating to CPU State Save Area
#
.equ            SSM_DR6,   0xffd0
.equ            SSM_DR7,   0xffc8

.equ            PROTECT_MODE_CS, 0x10   # RPPO-SKL-0018
.equ            PROTECT_MODE_DS, 0x18   # RPPO-SKL-0018
.equ            LONG_MODE_CS, 0x38
.equ            TSS_SEGMENT, 0x40
.equ            GDT_SIZE, 0x50

    .data

.align 16
gSmmFeatureSmiHandlerIdtr:
    .word 0
    .quad 0

.equ gPMStackDescOffset, gPMStackDesc - _SmiPMEntryPoint

    .text

ASM_PFX(gcSmmFeatureSmiHandlerTemplate):

_SmiEntryPoint:
    #
    # The encoding of BX in 16-bit addressing mode is the same as of RDI in 64-
    # bit addressing mode. And that coincidence has been used in the following
    # "64-bit like" 16-bit code. Be aware that once RDI is referenced as a
    # base address register, it is actually BX that is referenced.
    #
    .byte 0xbb                          # mov bx, imm16
    .word _GdtDesc - _SmiEntryPoint + 0x8000
    #
    # fix GDT descriptor
    #
    .byte 0x2e,0xa1                     # mov ax, cs:[offset16]
    .word      DSC_OFFSET + DSC_GDTSIZ
    .byte 0x48                          # dec ax
    .byte 0x2e
    movl    %eax, (%rdi)                # mov cs:[bx], ax
    .byte 0x66,0x2e,0xa1                # mov eax, cs:[offset16]
    .word      DSC_OFFSET + DSC_GDTPTR
    .byte 0x2e
    movw    %ax, 2(%rdi)
    .byte 0x66,0x2e
    lgdt    (%rdi)
    #
    # Patch ProtectedMode Segment
    #
    .byte 0xb8
    .word PROTECT_MODE_CS
    .byte 0x2e
    movl    %eax, -2(%rdi)
    #
    # Patch ProtectedMode entry
    #
    .byte 0x66, 0xbf                    # mov edi, SMBASE
ASM_PFX(gSmmFeatureSmbase): .space 4
    lea     ((ProtectedMode - _SmiEntryPoint) + 0x8000)(%edi), %ax
    .byte 0x2e
    movw    %ax, -6(%rdi)
    #
    # Switch into ProtectedMode
    #
    movq    %cr0, %rbx
    .byte 0x66
    andl    $0x9ffafff3, %ebx
    .byte 0x66
    orl     $0x00000023, %ebx

    movq    %rbx, %cr0
    .byte 0x66, 0xea
    .space 6

_GdtDesc:    .space  6

ProtectedMode:
    movw    $PROTECT_MODE_DS, %ax
    movl    %eax, %ds
    movl    %eax, %es
    movl    %eax, %fs
    movl    %eax, %gs
    movl    %eax, %ss
    .byte   0xbc                       # mov esp, imm32
ASM_PFX(gSmmFeatureSmiStack):   .space  4
    jmp     ProtFlatMode

ASM_PFX(gcSmiPMHandlerTemplate):
_SmiPMEntryPoint:
# when SMM PROT MODE feature is ok, processor will break here with 32bit protected mode
    .byte    0x0bf                     # mov edi, SMBASE
ASM_PFX(gProtModeSmbase):    .space    4
    # reload SS:ESP, it should be done in one instruction
    movl    $ASM_PFX(gPMStackDescOffset), %eax
    addl    %edi, %eax
    addl    $0x8000, %eax
    lss     (%rax), %esp

    movl    %edi, %eax
    addl    $(ProtFlatMode - _SmiPMEntryPoint + 0x8000), %eax
    movw    $PROTECT_MODE_CS, %dx
    movw    %dx, 2(%rax)                # mov cs:[bx - 6], eax
    movl    %eax, 6(%rax)               # mov cs:[bx - 6], eax

    .byte   0xea                        # jmp @ProtFlatMode
    .space  6

ProtFlatMode:
    .byte   0xb8
ASM_PFX(gSmmFeatureSmiCr3):    .space  4
    movq    %rax, %cr3
    movl    $0x668,%eax                 # as cr4.PGE is not set here, refresh cr3
    movq    %rax, %cr4                  # in PreModifyMtrrs() to flush TLB.
# Load TSS
    subl    $8, %esp                    # reserve room in stack
    sgdt    (%rsp)
    movl    2(%rsp), %eax               # eax = GDT base
    addl    $8, %esp
    movb    $0x89, %dl
    movb    %dl, (TSS_SEGMENT + 5)(%rax) # clear busy flag
    movl    $TSS_SEGMENT, %eax
    ltr     %ax

    #
    # Switch to LongMode
    #
    pushq    $LONG_MODE_CS                # push cs hardcore here
    call     Base                         # push return address for retf later
Base:
    addl    $(LongMode - Base), (%rsp)  # offset for far retf, seg is the 1st arg

# enable NXE if supported
    .byte   0xb0                        # mov al, imm8
ASM_PFX(gSmmFeatureXdSupported): .space 1
    cmpb    $0, %al
    jz      NxeDone
    movl    $MSR_EFER, %ecx
    rdmsr
    orw     $MSR_EFER_XD,%ax            # enable NXE
    wrmsr
NxeDone:

    movl    $MSR_EFER, %ecx
    rdmsr
    orb     $1,%ah                      # enable LME
    wrmsr
    movq    %cr0, %rbx
    orl     $0x080000000, %ebx          # enable paging + WP
    movq    %rbx, %cr0
    retf
LongMode:                               # long mode (64-bit code) starts here
    movabsq $ASM_PFX(gSmmFeatureSmiHandlerIdtr), %rax
    lidt    (%rax)
    lea     (DSC_OFFSET)(%rdi), %ebx
    movw    DSC_DS(%rbx), %ax
    movl    %eax,%ds
    movw    DSC_OTHERSEG(%rbx), %ax
    movl    %eax,%es
    movl    %eax,%fs
    movl    %eax,%gs
    movw    DSC_SS(%rbx), %ax
    movl    %eax,%ss
#   jmp     _SmmFeatureSmiHandler                 # instruction is not needed

_SmmFeatureSmiHandler:

    movq    (%rsp), %rbx
    # Save FP registers

    subq    $0x208, %rsp
    .byte   0x48                        # FXSAVE64
    fxsave  (%rsp)

    addq    $-0x20, %rsp

    movq    %rbx, %rcx
    movabsq $ASM_PFX(SmmFeatureCpuSmmDebugEntry), %rax
    call    *%rax

    movq    %rbx, %rcx
    movabsq $ASM_PFX(SmiRendezvous), %rax
    call    *%rax

    movq    %rbx, %rcx
    movabsq $ASM_PFX(SmmFeatureCpuSmmDebugExit), %rax
    call    *%rax

    addq    $0x20, %rsp

    #
    # Restore FP registers
    #
    .byte   0x48                        # FXRSTOR64
    fxrstor (%rsp)

# AMI_OVERRIDE_START >>> EIP494432 - Intel missing the .S part of code change, which will cause GCC build fail.
    addq    $0x208, %rsp

    #
    # Clear stack
    #
    xorq    %rax, %rax
    leal    (gSmmUsableStackSize), %eax    
    xorq    %rbx, %rbx
    movl    (%rax), %ebx
    leaq    (%rsp), %rax
    addq    $0x1000, %rax
#    andq    $0xFFFFF000, %rax
    shrq    $12, %rax
    andq    $0x000FFFFF, %rax
    shlq    $12, %rax
    subq    %rbx, %rax
    leaq    (%rsp), %rcx
    subq    %rax, %rcx
    leaq    -1(%rsp), %rdi
    xorq    %rax, %rax
    std
    rep stosb
    cld

    rsm

.align 16
ASM_PFX(gSmmUsableStackSize):
    .long 0

# AMI_OVERRIDE_END <<< EIP494432 - Intel missing the .S part of code change, which will cause GCC build fail.
.align 16
# used by LSS
ASM_PFX(gPMStackDesc):
    .long 0 # to be fixed
    .long PROTECT_MODE_DS

ASM_PFX(gcSmmFeatureSmiHandlerSize):    .word      . - _SmiEntryPoint
ASM_PFX(gcSmiPMHandlerSize):  .word      . - _SmiPMEntryPoint

