## @file
#  This is the code that goes from real-mode to protected mode.
#  it consumes the reset vector.
#
# @copyright
#  INTEL CONFIDENTIAL
#  Copyright 1999 - 2016 Intel Corporation.
#
#  The source code contained or described herein and all documents related to the
#  source code ("Material") are owned by Intel Corporation or its suppliers or
#  licensors. Title to the Material remains with Intel Corporation or its suppliers
#  and licensors. The Material may contain trade secrets and proprietary and
#  confidential information of Intel Corporation and its suppliers and licensors,
#  and is protected by worldwide copyright and trade secret laws and treaty
#  provisions. No part of the Material may be used, copied, reproduced, modified,
#  published, uploaded, posted, transmitted, distributed, or disclosed in any way
#  without Intel's prior express written permission.
#
#  No license under any patent, copyright, trade secret or other intellectual
#  property right is granted to or conferred upon you by disclosure or delivery
#  of the Materials, either expressly, by implication, inducement, estoppel or
#  otherwise. Any license under such intellectual property rights must be
#  express and approved by Intel in writing.
#
#  Unless otherwise agreed by Intel in writing, you may not remove or alter
#  this notice or any other notice embedded in Materials by Intel or
#  Intel's suppliers or licensors in any way.
#
#  This file contains a 'Sample Driver' and is licensed as such under the terms
#  of your license agreement with Intel or your vendor. This file may be modified
#  by the user, subject to the additional terms of the license agreement.
#
#@par Specification Reference:
#
##
#include <Token.h>	#AMI_OVERRIDE
.INCLUDE "PlatformGcc.inc"
.INCLUDE "ChipsetGcc.inc"
.INCLUDE "SecCoreGcc.inc"

.ifndef FSP_FLAG
ASM_GLOBAL ASM_PFX(SecStartup)
.endif
#AMI_OVERRIDE_START >>>

.equ	FV_LENGTH, (16 + 16)
.equ	FV_SIGNATURE, (FV_LENGTH + 8)
.equ	FV_HDR_LENGTH, (FV_SIGNATURE + 4 + 4)
.equ	FV_EXT_HDR_OFFSET, (FV_HDR_LENGTH + 2 + 2)

.equ	FV_EXT_HDR_SIZE, 16

.equ	FFS_TYPE, (16 + 2)
.equ	FFS_LENGTH, (FFS_TYPE + 1 + 1)
.equ	FFS_HDR_LENGTH, (FFS_LENGTH + 3 + 1)

.equ	uCODE_CPU_SIGNATURE, 12
.equ	uCODE_CPU_FLAGS, 24
.equ	uCODE_DATA_SIZE, 28
.equ	uCODE_TOTAL_SIZE, 32
#<<<AMI_OVERRIDE_END

ASM_GLOBAL ASM_PFX(_TEXT_REALMODE)
ASM_PFX(_TEXT_REALMODE):

#------------------------------------------------------------------------------
#
#  SEC "Security" Code module.
#
#  Transition to non-paged flat-model protected mode from a
#  hard-coded GDT that provides exactly two descriptors.
#  This is a bare bones transition to protected mode only
#  used for while in PEI and possibly DXE.
#
#  IA32 specific cache as RAM modules
#
#  After enabling protected mode, a far jump is executed to
#  TransferToPEI using the newly loaded GDT.
#  This code also enables the Cache-as-RAM
#
#  RETURNS:    none
#
#  MMX Usage:
#              MM0 = BIST State
#              MM3 = Patch Revision
#              MM4 = Patch Pointer
#              MM5 = Save time-stamp counter value high32bit
#              MM6 = Save time-stamp counter value low32bit.
#              MM7 = Used in CALL_MMX & RET_ESI macro
#
#------------------------------------------------------------------------------
  #
  # Reset Boot Flow Start
  #
.ifndef FSP_FLAG
.code16						#AMI_OVERRIDE
.align 4

ASM_GLOBAL ASM_PFX(_ModuleEntryPoint)
ASM_PFX(_ModuleEntryPoint):
  #
  # Save BIST state in MM0
  #
  fninit                                     # clear any pending Floating point exceptions
  movd    %eax, %mm0

  #
  # Save time-stamp counter value
  # rdtsc load 64bit time-stamp counter to EDX:EAX
  #
  rdtsc
  movd    %edx ,%mm5
  movd    %eax, %mm6

#AMI_OVERRIDE_START >>> This code is for detecting cpu only reset, if happened, change to chipset reset.
#----------------------------------------------------------------------------------------
# "Merlin" support
#----------------------------------------------------------------------------------------
  xorl    %eax, %eax
  movw    %ax, %es
  movw    %cs, %ax
  movw    %ax, %ds

#******************************************************************************
# BEGIN WARM-START CHANGE
#******************************************************************************
#
# PLATFORM-SPECIFIC EQUATES!
# These equates define an address which has the following requirements
# on the target platform:
# 1. After booting DOS, the memory is not used by other DOS applications
#    or drivers (thus very platform/configuration specific).
#    Minimum of roughly 8 bytes required.
# 2. The memory contents and address range are not affected by an INIT
# 3. By default, after booting DOS, the first 4 bytes at this address
#    contain either 0 (cleared memory) or 0xFFFFFFFF.
# 4. After booting DOS, the memory is writable
#
# It's expected that a manual inspection (using ITP) is performed to ensure
# that the requirements are met. If the manual inspection fails, then a
# different address must be identified, the below two equates must be
# changed accordingly, and the platform firmware must be rebuilt.
# Note that simply changing the platform hardware configuration could
# break this firmware because drivers may be loaded differently in
# memory, potentially using the address arbitrarily chosen here.
#
  #
  # Check if value in magic address contains non-zero/non-FF value.
  # It should actually contain executable code, typically a jmp
  # instruction.
  #
  movw 	$MAGIC_SEG, %ax
  movw 	%ax, %es
  movl  $MAGIC_ADDRESS_IN_SEG, %ecx
  movb 	%es:(%ecx), %al 

  #
  # 0EAh is the FAR JMP opcode that Merlin inserts
  #
  cmpb 	$0xEA, %al
  jz LegacyBiosWarmStart
# <<< AMI_OVERRIDE_END This code is for detecting cpu only reset, if happened, change to chipset reset.

  #
  # Check INIT# is asserted by port 0xCF9
  #
  movw     $0x0CF9, %dx				#AMI_OVERRIDE
  in      %dx, %al
  cmpb     $0x04, %al				#AMI_OVERRIDE
  jnz     NotWarmStart

  #
  # @note Issue warm reset, since if CPU only reset is issued not all MSRs are restored to their defaults
  #
  movw    $0xCF9, %dx
  movb    $0x06, %al
  out     %al, %dx

#AMI_OVERRIDE_START >>> This code is for detecting cpu only reset, if happened, change to chipset reset.
LegacyBiosWarmStart:
  #
  # Check 0ffff1h again to see is it warm reset
  #
  movl $MAGIC_ADDRESS_IN_SEG, %ecx
  movl %es:1(%ecx), %eax
  cmpl $0xf000e05b, %eax
  jne NotWarmStart
  #
  # Check APIC_BASE_MSR.BIT8 to see if we're the BSP
  #
  movw $MSR_APIC_BASE, %cx
  rdmsr
  testb $1, %ah
  jz TightLoop
  #
  # We're the BSP, so jump to the magic address.
  #
  .byte	  0xEA
  .short  MAGIC_ADDRESS_IN_SEG
  .short  MAGIC_SEG

  # Not reached
# <<< AMI_OVERRIDE_END This code is for detecting cpu only reset, if happened, change to chipset reset.

NotWarmStart:
  #
  # Enter Protected mode.
  #
  STATUS_CODE $1				#AMI_OVERRIDE
  movl    $GdtDesc, %esi
  .byte   0x66
  lgdt    %cs:(%si)
  movl    %cr0 , %eax                        # Get control register 0
  orl     $00000003, %eax                    # Set PE bit (bit #0) & MP bit (bit #1)
  movl    %eax, %cr0                         # Activate protected mode
  movl    %cr4, %eax                         # Get control register 4
  orl     $0x00000600, %eax                  # Set OSFXSR bit (bit #9) & OSXMMEXCPT bit (bit #10)
  movl    %eax, %cr4

  #
  # Now we're in Protected16
  # Set up the selectors for protected mode entry
  #
  mov     $SYS_DATA_SEL, %ax
  mov     %ax, %ds
  mov     %ax, %es
  mov     %ax, %fs
  mov     %ax, %gs
  mov     %ax, %ss

  #
  # Go to Protected32
  #
  movl    $NemInitLinearAddress, %esi
  .byte 0x66				#AMI_OVERRIDE
  ljmp     *%cs:(%si)			#AMI_OVERRIDE

TightLoop:
  cli
  hlt
  jmp     TightLoop


.code32					#AMI_OVERRIDE
.endif

ASM_GLOBAL ASM_PFX(_TEXT_PROTECTED_MODE)
ASM_PFX(_TEXT_PROTECTED_MODE):

.ifdef FSP_FLAG
.macro LOAD_MMX_EXT   ReturnAddress, MmxRegister
  movl    $ReturnAddress, %esi
  movd    %esi, $MmxRegister                 # save ReturnAddress into MM7
.endm

.macro CALL_MMX_EXT  RoutineLabel, MmxRegister
  #.local   ReturnAddress
  movl    $ReturnAddress, %esi
  movd    %esi, $MmxRegister                 # save ReturnAddress into MM7
  jmp     RoutineLabel
  ReturnAddress:
.endm

.macro RET_ESI_EXT   MmxRegister
  movd    $MmxRegister, %esi                 # restore ESP from MM7
  jmp     *%esi
.endm

.endif# FSP_FLAG

.macro CALL_MMX   RoutineLabel

  movl    $ReturnAddress, %esi
  movd    %esi, %mm7                         # save ReturnAddress into MM7
  jmp     RoutineLabel
  ReturnAddress:
.endm

.macro RET_ESI
  movd    %mm7, %esi                         # restore ESP from MM7
  jmp     *%esi
.endm

.macro    CALL_EBP   RoutineLabel
  #local   ReturnAddress
  movl    $ReturnAddress, %ebp
  jmp     RoutineLabel

ReturnAddress:
.endm

.macro RET_EBP
  jmp     *%ebp                              # restore ESP from EBP
.endm


.align 4
ASM_GLOBAL ASM_PFX(ProtectedModeSecStart)
ASM_PFX(ProtectedModeSecStart):
#AMI_OVERRIDE_START >>> 
  movl    $SecPlatformLibEntryElink1, %esi     # CALL_MMX  SecPlatformInit
  movd    %esi, %mm7
  jmp     SECNB_EarlyInit
SecPlatformLibEntryElink1:

  movl    $SecPlatformLibEntryElink2, %esi     # CALL_MMX  SecPlatformInit
  movd    %esi, %mm7
  jmp     SECSB_EarlyInit
SecPlatformLibEntryElink2:

  movl    $SecPlatformLibEntryElink3, %esi     # CALL_MMX  SecPlatformInit
  movd    %esi, %mm7
  jmp     SECCRB_EarlyInit
SecPlatformLibEntryElink3:
#<<<AMI_OVERRIDE_END
.ifndef FSP_FLAG
  STATUS_CODE $0x02				#AMI_OVERRIDE
  movl    $ProtectedModeSecStartL0, %esi     # CALL_MMX  EarlyMicrocodeUpdate
  movd    %esi, %mm7
#AMI_OVERRIDE_START >>> 
#  jmp     EarlyMicrocodeUpdate
  jmp	  VeryEarlyMicrocodeUpdate
#<<<AMI_OVERRIDE_END
ProtectedModeSecStartL0:

.ifdef SV_HOOKS
  jmp PchSvSbSecEarlyInit
PchSvSbSecEarlyInitEndReal:
.endif # SV_HOOKS

  STATUS_CODE $0x03				#AMI_OVERRIDE
  movl    $ProtectedModeSecStartL1, %esi     # CALL_MMX  SecPlatformInit
  movd    %esi, %mm7
  jmp     SecPlatformInit
ProtectedModeSecStartL1:


  STATUS_CODE $0x04				#AMI_OVERRIDE
  movl    $ProtectedModeSecStartL2, %esi     # CALL_MMX  SecCarInit
  movd    %esi, %mm7
  jmp     SecCarInit
ProtectedModeSecStartL2:


  STATUS_CODE $0x05				#AMI_OVERRIDE
  movl    $ProtectedModeSecStartL3, %esi     # CALL_MMX  EstablishStack
  movd    %esi, %mm7
  jmp     EstablishStack
ProtectedModeSecStartL3:

  STATUS_CODE $0x06				#AMI_OVERRIDE
  jmp  CallPeiCoreEntryPoint

.endif #FSP_FLAG



.ifdef SV_HOOKS
ASM_GLOBAL ASM_PFX(PchSvSbSecEarlyInitEnd)
ASM_PFX(PchSvSbSecEarlyInitEnd):
  jmp PchSvSbSecEarlyInitEndReal

.endif #SV_HOOKS

#
# Perform early platform initialization
#
ASM_GLOBAL ASM_PFX(SecPlatformInit)
ASM_PFX(SecPlatformInit):

  movl    $SecPlatformInitL0, %ebp           # CALL_EBP  SecSaInit
  jmp     SecSaInit
SecPlatformInitL0:

  #
  # Enable HPET decode in PCH.
  #
.ifdef FSP_FLAG
  #
  # Read PCI Express base address
  #
  movl    $PCIEX_BAR_REG, %eax               # B0:D0:F0:R60
  mov     $0x0CF8, %dx
  out     %eax, %dx
  mov     $0x0CFC, %dx
  in      %dx, %eax
  and     $0xFC000000, %eax
  movl    %eax, %esi
.else
#  movl    PcdGet64 (PcdPciExpressBaseAddress), %esi	#AMI_OVERRIDE
.ifdef AMI_PCIE_DYNAMIC_SUPPORT_FLAG
  movl	  $0xE0000000, %esi
.else
  movl	  $PCIEX_BASE_ADDRESS, %esi		#AMI_OVERRIDE
.endif
.endif
  addl    $PCH_HPET_REG, %esi
  mov     $PCH_HPET_AE, %al
  mov     %al, (%esi)
  mov     (%esi), %al
  xorl    %eax, %eax
  movl    $HPET_COMP_1, %esi                 # read back
  movl    %eax, (%esi)
  movl    $HPET_COMP_2, %esi
  movl    %eax, (%esi)

  RET_ESI


  #
  # Establish Stack
  #
ASM_GLOBAL ASM_PFX(EstablishStack)
ASM_PFX(EstablishStack):
  #
  # Enable STACK
  #
  RET_ESI



  #
  # This is the entry point to PEI phase
  #
ASM_GLOBAL ASM_PFX(CallPeiCoreEntryPoint)
ASM_PFX(CallPeiCoreEntryPoint):
  #
  # Set stack top pointer
  #
#  movl    PcdGet32 (PcdTemporaryRamBase), %esp	#AMI_OVERRIDE
#  addl    PcdGet32 (PcdTemporaryRamSize), %esp	#AMI_OVERRIDE
  movl    $TemporaryRamBaseAddress, %esp	#AMI_OVERRIDE
  addl    $TotalTemporaryRamSize, %esp		#AMI_OVERRIDE

  #
  # Push CPU count to stack first, then AP's (if there is one)
  # BIST status, and then BSP's
  #
  mov     $0x01, %ch                         # for client we have only one BSP
  movzx   %ch, %ecx

  #
  # Save number of BSPs
  #
  pushl   %ecx

GetSbspBist:
  #
  # Save SBSP BIST
  #
  movd    %mm0, %eax
  pushl   %eax

  #
  # Save SBSP APIC ID
  #
  movd    %mm1, %eax
  shrl    $0x18, %eax                        # Resume APIC ID
  pushl   %eax

  #
  # Save Time-Stamp Counter
  #
  movd    %mm5, %eax
  pushl   %eax

  movd    %mm6, %eax
  pushl   %eax

TransferToSecStartup:

  #
  # Switch to "C" code
  #
  STATUS_CODE $0x0C				#AMI_OVERRIDE

  #
  # Do platform specific TraceHub initialization prior to calling into core code.
  # This is required for PCH if the TraceHub debugger is going to be used.
  #
  # Must preserve register values for call to SecStartup.
  #
  pushal					#AMI_OVERRIDE
  push    $0x00                                  # EnableMode = PchTraceHubModeDisabled
  call    ASM_PFX(TraceHubInitialize)
  popal						#AMI_OVERRIDE

  STATUS_CODE $0x0D				#AMI_OVERRIDE

  #
  # Pass entry point of the PEI core
  #
  movl    $PEI_CORE_ENTRY_BASE, %edi         # 0FFFFFFE0h
  pushl   %ds:(%edi)

  #
  # Pass BFV into the PEI Core
  #
  movl    $FV_MAIN_BASE_SEC, %edi                # 0FFFFFFFCh	#AMI_OVERRIDE
  pushl   %ds:(%edi)

  #
  # @todo ECPoverride: SecStartup entry point needs 4 parameters
  #
#  pushl   PcdGet32 (PcdTemporaryRamBase)	#AMI_OVERRIDE
  pushl   $TemporaryRamBaseAddress

  #
  # Pass stack size into the PEI Core
  #
#  pushl   PcdGet32 (PcdTemporaryRamSize)	#AMI_OVERRIDE
  pushl   $TotalTemporaryRamSize

  #
  # Pass Control into the PEI Core
  #
.ifndef FSP_FLAG  
  call    ASM_PFX(SecStartup)
.endif
#CallPeiCoreEntryPoint   ENDP

#
# Load MCU path, if not already loaded from FIT table
#

#-----------------------------------------------------------------------------
#
#  Section:     EarlyMicrocodeUpdate
#
#  Description:
#
#-----------------------------------------------------------------------------
#AMI_OVERRIDE_START >>>
#
# Load MCU path, if not already loaded from FIT table
#
ASM_GLOBAL ASM_PFX(VeryEarlyMicrocodeUpdate)
ASM_PFX(VeryEarlyMicrocodeUpdate):
  xorl	%eax, %eax
  xorl	%edx, %edx 
  movl  $MSR_IA32_BIOS_SIGN_ID, %ecx
  wrmsr
  movl  $0x01, %eax
  cpuid
  movl  $MSR_IA32_BIOS_SIGN_ID, %ecx
  rdmsr
  cmpl	$0, %edx					#If microcode has been updated
  jnz	uc_exit						#Skip if patch already loaded
.if MICROCODE_SPLIT_BB_UPDATE
  movl	$FV_MICROCODE_UPDATE_BASE, %ebx
.else
  movl	$FV_MICROCODE_BASE, %ebx
.endif
  movl	%ebx, %edx
  movl	$FV_SIGNATURE, %ecx
  xorl	%eax, %eax
  movb	$0x48, %ah				#'H'=0x48	
  movb	$0x56, %al				#'V'=0x56 
  shll   $0x10, %eax 
  movb  $0x46, %ah				#'F'=0x46
  movb  $0x5f, %al				#'_'=0x5f 
  cmpl	%eax, (%edx, %ecx)				#eax = 'HVF_'
  jne	Microcode_Not_Found			#Corrupt FV?
  movl	$FV_HDR_LENGTH, %ecx
  movl  (%edx, %ecx), %eax
  andl  $0xffff, %eax
  addl	%eax, %ebx
  jc	Microcode_Not_Found			#Corrupt FV?
  
  movl	$FV_EXT_HDR_OFFSET, %ecx		#Optional Header
  movzxw (%edx, %ecx), %eax
  cmpw  $0, %ax
  je	Extended_Header_Not_Found
  lea	(%eax, %edx), %eax
  
  movl	$FV_EXT_HDR_SIZE, %ecx
  movl	(%eax, %ecx), %ecx				#ecx = extended header size
  movl	%eax, %ebx
  addl	%ecx, %ebx
  jc	Microcode_Not_Found			#Corrupt FV?
Extended_Header_Not_Found:

  Movl  $0x7, %ecx
  addl	%ecx, %ebx
  not	%ecx
  andl	%ecx, %ebx
  
  movl	$FV_LENGTH, %ecx
  addl	(%edx, %ecx), %edx			#edx = end of FV.
#  jz
  jc	Microcode_Not_Found			#Corrupt FV?
  dec	%edx						#edx = End of FFS

#---Find File ---
#ebx = Start of FFS
#edx = End of FFS
Get_Next_File:
  movl 	$FFS_TYPE, %ecx
  cmpb	$0xf0, (%ebx, %ecx) 		#iqnore guid for pad type
  je	Pad_Type
  cmpl	$-1, (%ebx)
  je	File_not_found
Pad_Type:
  movl	$4, %ecx
  movl	%ebx, %esi
  lea 	MICOCODE_FFS_GUID, %edi
  #compare file guid
  repe	cmpsd
  je	Found_File
#---get next file--
  #get file length and add to ebx
  movl	$FFS_LENGTH, %ecx
  movl  (%ebx, %ecx), %eax
  andl  $0xffffff, %eax				#eax = File length (24 bits)
  addl  %eax, %ebx					#next file
  #align file
  movl	$7, %ecx
  addl	%ecx, %ebx
  jc	Microcode_Not_Found			#Check for wrap-around if bad data.
  not	%ecx
  andl	%ecx, %ebx
  
  mov	%ebx, %eax
  addl	$FFS_HDR_LENGTH, %eax		#eax is microcode data
  cmpl	%edx, %eax
  jb	Get_Next_File
File_not_found:
  jmp	Microcode_Not_Found
#---Found File---
Found_File:
#ebx = Start of Microcode FFS
#edx = End of FFS + 1

#---Search micocode for match.---
  movl	%ebx, %edi
  
  movl	$FFS_LENGTH, %ecx
  movl  (%ebx, %ecx), %eax
  andl  $0xffffff, %eax				#eax = File length (24 bits)
  addl	%eax, %edi					#edi = end of file
  jc	Microcode_Not_Found     	#Check for wrap-around if bad data.
  
.if	MPDTable_CREATED
  movzxw -2(%edi), %eax			#eax = MPDT length
  subl	%eax, %edi					#edi = end of microcodes.
.endif

  addl	$FFS_HDR_LENGTH, %ebx
  jc	Microcode_Not_Found     	#Check for wrap-around if bad data.	
  
  mov 	%ebx, %esi
  
  mov	$1, %eax
  cpuid								#EAX = CPU signature.
  
  movl	%eax, %ebx					#ebx = CPU signature
  
  movl	$0x17, %ecx
  rdmsr								#read CPUS flags
  
  shrl	$18, %edx
  andb	$7, %dl						#dl = CPU FLAG
  movb	%dl, %cl
  movb	$1, %dl
  shl	%cl, %dl					#dl = bit set for CPU flag
  
  xchgl	%esi, %ebx					#ebx = ptr, esi = signature
  
Next_Microcode:
  cmpl 	%edi, %ebx					#Check to see if microcode at end of FFS File.
  jae	Microcode_Not_Found
  
  cmpl	$1, (%ebx)					#Check if correct header revision
  jne	Microcode_Not_Found
  
  movl	$2048, %ecx					#total size if data size in field = 0.
  movl	$uCODE_DATA_SIZE, %eax
  cmpl	$0, (%ebx, %eax)
  jz	Microcode_Size_Zero
  movl	$uCODE_TOTAL_SIZE, %eax
  movl	(%ebx, %eax),	%ecx		#Get total size.
Microcode_Size_Zero:

  movl	$uCODE_CPU_SIGNATURE, %eax
  cmpl	(%ebx, %eax) , %esi
  jne	Check_Ext_Sig
  
  movl	%ebx, %eax					#Store uC addr in eax, so if jnz succeeds.
  movl	$uCODE_CPU_FLAGS, %ebp
  testb	(%ebx, %ebp), %dl			#uC flags match?
  jnz	Exit_Find_Microcode

Check_Ext_Sig:
  movl	$uCODE_TOTAL_SIZE, %eax
  movl	(%ebx, %eax), %ebp			#Get total size.
  movl	$uCODE_DATA_SIZE, %eax
  movl	(%ebx, %eax), %eax			#Get Data Size
  addl	$48, %eax
  cmpl	%eax, %ebp
  jbe	Find_Next_Microcode       	#No extended signature.
  
  movl	(%ebx, %eax), %ecx			#Get extended signature.
  cmpl	$20, %ecx
  jae	Microcode_Not_Found         #Corrupt?
  lea	20(%ebx, %eax), %ebp		#First Proc Signature.
  
Check_Ext_Sig_Loop:
  cmpl	%esi, (%ebp)				#Check Signature
  jne	Next_Ext_Sig
  
  movl	%ebx, %eax
  testb	4(%ebp), %dl
  jnz	Exit_Find_Microcode
Next_Ext_Sig:
  addl	$12, %ebp
  loop  Check_Ext_Sig_Loop
  movl	$uCODE_TOTAL_SIZE, %eax
  movl	(%ebx, %eax),	%ecx
Find_Next_Microcode:
  #align to next block size
.if PACK_MICROCODE
  addl	$15, %ecx
  and	$0xfffffff0, %ecx
.else
  movl	$MICROCODE_BLOCK_SIZE, %eax
  dec	%eax
  addl	%eax, %ecx
  not	%eax
  andl	%eax, %ecx
.endif
  addl	%ecx, %ebx
  jc	Microcode_Not_Found				#Overflow - bad data.
  jmp	Next_Microcode
Microcode_Not_Found:
.if MICROCODE_SPLIT_BB_UPDATE
  cmpl	$FV_MICROCODE_BASE, %ebx
  jae	No_More_Split_Microcode
  movl	$FV_MICROCODE_BASE, %ebx
  jmp	FindMicrocodeFv
No_More_Split_Microcode:
.endif

  xorl	%eax, %eax
Exit_Find_Microcode:
  orl	%eax, %eax
  jz	uc_exit             #No microcode found
  
  #Update microcode
  mov	$0x79, %ecx
  xorl	%edx, %edx
  add	$48, %eax
  wrmsr
uc_exit:
  RET_ESI

#ASM_GLOBAL ASM_PFX(EarlyMicrocodeUpdate)
#ASM_PFX(EarlyMicrocodeUpdate):
#  movl    ASM_PFX(_gPcd_FixedAtBuild_PcdFlashMicrocodeFvSize), %edi #AMI_OVERRIDE
#  or      %edi, %edi
#  jz      SearchAndLoadMicrocodeReturn

#  addl    PcdGet32 (PcdFlashMicrocodeFvBase), %edi # End addr of uCodes.

#  movl    ASM_PFX(_gPcd_FixedAtBuild_PcdFlashMicrocodeFvBase), %esi
#  movzxw  (FVHEADER_LEN_OFF), %ebx
#  addl    %ebx, %esi
#  addl    $FFSHEADER_LEN, %esi               #add FFS header

#  movl    $SearchAndLoadMicrocodeReturn, %ebp
#  jmp     SearchAndLoadMicrocode

#SearchAndLoadMicrocodeReturn:
#  RET_ESI
#<<<AMI_OVERRIDE_END
.align 0x10

  #
  # GDT[0]: 0x00: Null entry, never used.
  #
.equ NULL_SEL,          .-GDT_BASE           # Selector [0]
GDT_BASE:
BootGdtTable:
    .long  0
    .long  0

  #
  # Linear data segment descriptor
  #
.equ LINEAR_SEL,        .-GDT_BASE           # Selector [0x8]
    .word 0x0FFFF                            # limit0x0xFFFFF
    .word  0                                 # base 0
    .byte  0
    .byte 0x092                              # present, ring0x0, data, expand-up, writable
    .byte 0x0CF                              # page-granular, 32-bit
    .byte  0

  #
  # Linear code segment descriptor
  #
.equ LINEAR_CODE_SEL,   .-GDT_BASE           # Selector [0x10]
    .word 0x0FFFF                            # limit 0xFFFFF
    .word  0                                 # base 0
    .byte  0
    .byte 0x09B                              # present, ring 0, data, expand-up, not-writable
    .byte 0x0CF                              # page-granular, 32-bit
    .byte  0

  #
  # System data segment descriptor
  #
.equ SYS_DATA_SEL,     .-GDT_BASE            # Selector [0x18]
    .word 0x0FFFF                            # limit0x0xFFFFF
    .word  0                                 # base 0
    .byte  0
    .byte 0x093                              # present, ring0x0, data, expand-up, not-writable
    .byte 0x0CF                              # page-granular, 32-bit
    .byte  0

  #
  # System code segment descriptor
  #
.equ SYS_CODE_SEL,     .-GDT_BASE            # Selector [0x20]
    .word  0x0FFFF                           # limit 0xFFFFF
    .word  0                                 # base 0
    .byte  0
    .byte  0x09A                             # present, ring 0, data, expand-up, writable
    .byte  0x0CF                             # page-granular, 32-bit
    .byte  0

  #
  # Spare segment descriptor
  #
.equ SYS16_CODE_SEL,   .-GDT_BASE            # Selector [0x28]
    .word  0x0FFFF                           # limit 0x0xFFFFF
    .word  0                                 # base 0
    .byte  0x0E                              # Changed from F000 to E000.
    .byte  0x09B                             # present, ring 0x0, code, expand-up, writable
    .byte  0x00                              # byte-granular, 16-bit
    .byte  0

  #
  # Spare segment descriptor
  #
.equ SYS16_DATA_SEL,   .-GDT_BASE            # Selector [0x30]
    .word  0x0FFFF                           # limit 0xFFFF
    .word  0                                 # base 0
    .byte  0
    .byte  0x093                             # present, ring 0, data, expand-up, not-writable
    .byte  0x00                              # byte-granular, 16-bit
    .byte  0

  #
  # Spare segment descriptor
  #
.equ SPARE5_SEL,       .-GDT_BASE            # Selector [0x38]
    .word  0                                 # limit 0
    .word  0                                 # base 0
    .byte  0
    .byte  0                                 # present, ring 0, data, expand-up, writable
    .byte  0                                 # page-granular, 32-bit
    .byte  0
.equ GDT_SIZE,         .-BootGdtTable        # Size, in bytes

.ifndef FSP_FLAG
GdtDesc:                                     # GDT descriptor
.equ OffsetGDTDesc,  .-_ModuleEntryPoint
    .word  GDT_SIZE - 1                      # GDT limit
    .long  BootGdtTable                      # GDT base address
.endif
ASM_PFX(NemInitLinearAddress):
NemInitLinearOffset:
    .long  ProtectedModeSecStart             # Offset of our 32 bit code
    .word  LINEAR_CODE_SEL

#AMI_OVERRIDE_START >>>
MICOCODE_FFS_GUID:
	.long	0x17088572
	.word	0x377F
	.word	0x44ef
	.byte	0x8F,0x4E,0xB0,0x9F,0xFF,0x46,0xA0,0x70
#<<< AMI_OVERRIDE_END
