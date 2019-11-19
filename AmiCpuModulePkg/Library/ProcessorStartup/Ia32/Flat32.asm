;; @file
;  This is the code that goes from real-mode to protected mode.
;  it consumes the reset vector.
;
; @copyright
;  INTEL CONFIDENTIAL
;  Copyright 1999 - 2016 Intel Corporation.
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

  INCLUDE token.equ		;AMI_OVERRIDE
  INCLUDE Platform.inc
  INCLUDE Chipset.inc
  INCLUDE SecCore.inc
  INCLUDE SecCpuLib.inc		;AMI_OVERRIDE

.686p
.xmm
.model small, c

;AMI_OVERRIDE_START >>>
ifndef MKF_MPDTable_CREATED
MKF_MPDTable_CREATED EQU 0
endif

ifndef MKF_MICROCODE_SPLIT_BB_UPDATE
MKF_MICROCODE_SPLIT_BB_UPDATE EQU 0
endif

FV_LENGTH		EQU (16 + 16)
FV_SIGNATURE    EQU (FV_LENGTH + 8)
FV_HDR_LENGTH	EQU (FV_SIGNATURE + 4 + 4)
FV_EXT_HDR_OFFSET EQU (FV_HDR_LENGTH + 2 + 2)

FV_EXT_HDR_SIZE EQU 16

FFS_TYPE        EQU (16 + 2)
FFS_LENGTH      EQU (FFS_TYPE + 1 + 1)
FFS_HDR_LENGTH	EQU (FFS_LENGTH + 3 + 1)

uCODE_CPU_SIGNATURE	EQU 12
uCODE_CPU_FLAGS		EQU	24
uCODE_DATA_SIZE		EQU	28
uCODE_TOTAL_SIZE	EQU	32
;<<<AMI_OVERRIDE_END
IFNDEF FSP_FLAG
EXTRN   SecStartup:NEAR
ENDIF
EXTRN   SecSaInit:NEAR
;AMI_OVERRIDE_START >>>
EXTERN  SecPlatformLibEntry:NEAR
EXTERN  SECCoreAtPowerOn:NEAR
EXTERN  BeforeSECEntry:NEAR
PUBLIC  SecPlatformLibEntryEnd
PUBLIC  SECCoreAtPowerOnEnd
PUBLIC  BeforeSECEntryEnd
;<<<AMI_OVERRIDE_END

;
; TraceHubInitialize is a helper function to correctly configure the TraceHub controller
;
EXTRN   TraceHubInitialize:NEAR

;AMI_OVERRIDE_START >>>
IF 0
EXTRN   PcdGet32 (PcdTemporaryRamBase):DWORD
EXTRN   PcdGet32 (PcdTemporaryRamSize):DWORD
IFNDEF FSP_FLAG
EXTRN   PcdGet64 (PcdPciExpressBaseAddress):QWORD
ENDIF
EXTRN   PcdGet64 (PcdMchBaseAddress):QWORD
EXTRN   PcdGet32 (PcdFlashMicrocodeFvBase):DWORD
EXTRN   PcdGet32 (PcdFlashMicrocodeFvSize):DWORD



EXTERN   SearchAndLoadMicrocode:NEAR
ENDIF
;<<<AMI_OVERRIDE_END
EXTERN   SecCarInit:NEAR

_TEXT_REALMODE      SEGMENT PARA PUBLIC USE16 'CODE'
                    ASSUME  CS:_TEXT_REALMODE, DS:_TEXT_REALMODE

;------------------------------------------------------------------------------
;
;  SEC "Security" Code module.
;
;  Transition to non-paged flat-model protected mode from a
;  hard-coded GDT that provides exactly two descriptors.
;  This is a bare bones transition to protected mode only
;  used for while in PEI and possibly DXE.
;
;  IA32 specific cache as RAM modules
;
;  After enabling protected mode, a far jump is executed to
;  TransferToPEI using the newly loaded GDT.
;  This code also enables the Cache-as-RAM
;
;  RETURNS:    none
;
;  MMX Usage:
;              MM0 = BIST State
;              MM3 = Patch Revision
;              MM4 = Patch Pointer
;              MM5 = Save time-stamp counter value high32bit
;              MM6 = Save time-stamp counter value low32bit.
;              MM7 = Used in CALL_MMX & RET_ESI macro
;
;------------------------------------------------------------------------------

; Reset Boot Flow Start
IFNDEF FSP_FLAG
align 4
_ModuleEntryPoint PROC NEAR C PUBLIC
  ;
  ; Save BIST state in MM0
  ;
  fninit                                ; clear any pending Floating point exceptions
  movd    mm0, eax

  ;
  ; Save time-stamp counter value
  ; rdtsc load 64bit time-stamp counter to EDX:EAX
  ;
  rdtsc
  movd    mm5, edx
  movd    mm6, eax

;AMI_OVERRIDE_START >>> This code is for detecting cpu only reset, if happened, change to chipset reset.
;----------------------------------------------------------------------------------------
; "Merlin" support
;----------------------------------------------------------------------------------------
  xor     eax, eax
  mov     es, ax
  mov     ax, cs
  mov     ds, ax

;******************************************************************************
; BEGIN WARM-START CHANGE
;******************************************************************************
;
; PLATFORM-SPECIFIC EQUATES!
; These equates define an address which has the following requirements
; on the target platform:
; 1. After booting DOS, the memory is not used by other DOS applications
;    or drivers (thus very platform/configuration specific).
;    Minimum of roughly 8 bytes required.
; 2. The memory contents and address range are not affected by an INIT
; 3. By default, after booting DOS, the first 4 bytes at this address
;    contain either 0 (cleared memory) or 0xFFFFFFFF.
; 4. After booting DOS, the memory is writable
;
; It's expected that a manual inspection (using ITP) is performed to ensure
; that the requirements are met. If the manual inspection fails, then a
; different address must be identified, the below two equates must be
; changed accordingly, and the platform firmware must be rebuilt.
; Note that simply changing the platform hardware configuration could
; break this firmware because drivers may be loaded differently in
; memory, potentially using the address arbitrarily chosen here.
;
  ;
  ; Check if value in magic address contains non-zero/non-FF value.
  ; It should actually contain executable code, typically a jmp
  ; instruction.
  ;
  mov ax, MAGIC_SEG
  mov es, ax
  mov al, BYTE PTR es:[MAGIC_ADDRESS_IN_SEG]

  ;
  ; 0EAh is the FAR JMP opcode that Merlin inserts
  ;
  cmp al, 0EAh
  jz LegacyBiosWarmStart
; <<< AMI_OVERRIDE_END This code is for detecting cpu only reset, if happened, change to chipset reset.
  ;
  ; Check INIT# is asserted by port 0xCF9
  ;
  mov dx, 0CF9h
  in  al, dx
  cmp al, 04h
  jnz NotWarmStart

  ;
  ; @note Issue warm reset, since if CPU only reset is issued not all MSRs are restored to their defaults
  ;
  mov dx, 0CF9h
  mov al, 06h
  out dx, al
;AMI_OVERRIDE_START >>> This code is for detecting cpu only reset, if happened, change to chipset reset.
LegacyBiosWarmStart:
  ;
  ; Check 0ffff1h again to see is it warm reset
  ;
  mov eax, DWORD PTR es:[MAGIC_ADDRESS_IN_SEG + 1]  
  cmp eax,0f000e05bh
  jne NotWarmStart
  ;
  ; Check APIC_BASE_MSR.BIT8 to see if we're the BSP
  ;
  mov cx, MSR_APIC_BASE
  rdmsr
  test ah, 1
  jz TightLoop
  ;
  ; We're the BSP, so jump to the magic address.
  ;
  DB  0EAh
  DW  MAGIC_ADDRESS_IN_SEG
  DW  MAGIC_SEG

  ; Not reached
; <<< AMI_OVERRIDE_END This code is for detecting cpu only reset, if happened, change to chipset reset.
NotWarmStart:

  ;
  ; Enter Protected mode.
  ;
  STATUS_CODE (01h)                     ; BSP_PROTECTED_MODE_START
  mov     esi,  OFFSET GdtDesc
  DB      66h
  lgdt    fword ptr cs:[si]
  mov     eax, cr0                      ; Get control register 0
  or      eax, 00000003h                ; Set PE bit (bit #0) & MP bit (bit #1)
  mov     cr0, eax                      ; Activate protected mode
  mov     eax, cr4                      ; Get control register 4
  or      eax, 00000600h                ; Set OSFXSR bit (bit #9) & OSXMMEXCPT bit (bit #10)
  mov     cr4, eax

  ;
  ; Now we're in Protected16
  ; Set up the selectors for protected mode entry
  ;
  mov     ax, SYS_DATA_SEL
  mov     ds, ax
  mov     es, ax
  mov     fs, ax
  mov     gs, ax
  mov     ss, ax

  ;
  ; Go to Protected32
  ;
  mov     esi, offset NemInitLinearAddress
  jmp     fword ptr cs:[si]

TightLoop:
  cli
  hlt
  jmp     TightLoop

_ModuleEntryPoint ENDP
ENDIF
_TEXT_REALMODE      ENDS

_TEXT_PROTECTED_MODE      SEGMENT PARA PUBLIC USE32 'CODE'
                          ASSUME  CS:_TEXT_PROTECTED_MODE, DS:_TEXT_PROTECTED_MODE

IFDEF FSP_FLAG
LOAD_MMX_EXT MACRO   ReturnAddress, MmxRegister
  mov     esi, ReturnAddress
  movd    MmxRegister, esi              ; save ReturnAddress into MM7
ENDM

CALL_MMX_EXT MACRO   RoutineLabel, MmxRegister
  local   ReturnAddress
  mov     esi, offset ReturnAddress
  movd    MmxRegister, esi              ; save ReturnAddress into MM7
  jmp     RoutineLabel
ReturnAddress:
ENDM

RET_ESI_EXT  MACRO   MmxRegister
  movd    esi, MmxRegister              ; restore ESP from MM7
  jmp     esi
ENDM
ENDIF; FSP_FLAG

CALL_MMX MACRO   RoutineLabel

  local   ReturnAddress
  mov     esi, offset ReturnAddress
  movd    mm7, esi                      ; save ReturnAddress into MM7
  jmp     RoutineLabel
ReturnAddress:

ENDM

RET_ESI  MACRO

  movd    esi, mm7                      ; restore ESP from MM7
  jmp     esi

ENDM

CALL_EBP MACRO   RoutineLabel

  local   ReturnAddress
  mov     ebp, offset ReturnAddress
  jmp     RoutineLabel
ReturnAddress:

ENDM

RET_EBP  MACRO

  jmp     ebp                           ; restore ESP from EBP

ENDM

align 4
ProtectedModeSecStart PROC NEAR PUBLIC

;AMI_OVERRIDE_START >>>
; Invoke the SecCore Init hook for other components to hook functions
; This also executes referece code.
	jmp     SecPlatformLibEntry
SecPlatformLibEntryEnd::
	jmp     SECCoreAtPowerOn
SECCoreAtPowerOnEnd::
;<<<AMI_OVERRIDE_END
IFNDEF FSP_FLAG

  STATUS_CODE (02h)
;AMI_OVERRIDE_START >>> 
;  CALL_MMX  EarlyMicrocodeUpdate 
  CALL_MMX  VeryEarlyMicrocodeUpdate 
;<<<AMI_OVERRIDE_END
  STATUS_CODE (03h)
  CALL_MMX  SecPlatformInit

  STATUS_CODE (04h)
  CALL_MMX  SecCarInit

  STATUS_CODE (05h)
  CALL_MMX  EstablishStack

  STATUS_CODE (06h)
  jmp  CallPeiCoreEntryPoint
ENDIF

ProtectedModeSecStart ENDP

;
; Perform early platform initialization
;
SecPlatformInit    PROC    NEAR    PUBLIC

  CALL_EBP  SecSaInit

  ;
  ; Enable HPET decode in PCH.
  ;
IFDEF FSP_FLAG
  ;
  ; Read PCI Express base address
  ;
  mov     eax,  PCIEX_BAR_REG
  mov     dx,   0CF8h
  out     dx,   eax
  mov     dx,   0CFCh
  in      eax,  dx
  and     eax,  0xFC000000
  mov     esi,  eax
ELSE
  mov     esi, DWORD PTR PCIEXPRESS_BASE_ADDRESS	;AMI_OVERRIDE
ENDIF

  add     esi, PCH_HPET_REG
  mov     al,  PCH_HPET_AE
  mov     Byte Ptr [esi], al
  mov     al,  Byte Ptr [esi]                     ; read back
  xor     eax, eax
  mov     esi, HPET_COMP_1
  mov     Dword Ptr [esi], eax
  mov     esi, HPET_COMP_2
  mov     Dword ptr [esi], eax
  RET_ESI

SecPlatformInit    ENDP

;AMI_OVERRIDE_START >>>
;
; Load MCU path, if not already loaded from FIT table
;
VeryEarlyMicrocodeUpdate    PROC    NEAR    PRIVATE

IF EARLY_MICROCODE_SUPPORT
	xor     eax, eax
    xor     edx, edx
    mov     ecx, MSR_IA32_BIOS_SIGN_ID
    wrmsr
    mov     eax, 01h
    cpuid
    mov     ecx, MSR_IA32_BIOS_SIGN_ID
    rdmsr                               ; CPU PatchID -> EDX
    cmp     edx, 0                      ; If microcode has been updated
    jnz     uc_exit                     ; Skip if patch already loaded
	
IF  MKF_MICROCODE_SPLIT_BB_UPDATE
    mov     ebx, MKF_FV_MICROCODE_UPDATE_BASE
FindMicrocodeFv:
ELSE
    mov     ebx, MKF_FV_MICROCODE_BASE
ENDIF
	mov		edx, ebx
    cmp     dword ptr [edx + FV_SIGNATURE], 'HVF_'
    jne     Microcode_Not_Found             ;Corrupt FV?
	mov		eax, [edx + FV_HDR_LENGTH]
	and		eax, 0ffffh		;Get 16 bit FV header length
	add		ebx,  eax		;ebx = start of FFS
    jc      Microcode_Not_Found             ;Corrupt FV?

	movzx   eax, word ptr [edx + FV_EXT_HDR_OFFSET]	;Optional Header
    cmp ax, 0								;Optional exteneded header ?
	je  @f
	lea eax, [edx + eax]					;eax = extended header

	mov ecx, [eax + FV_EXT_HDR_SIZE]		;ecx = extended header size
	mov ebx, eax
	add ebx, ecx
	jc  Microcode_Not_Found                 ;Corrupt FV?
@@:

	add		ebx, 7							;Align to 8-bytes for FFs
	and		ebx, not 7

	add		edx, [edx + FV_LENGTH]			;edx = end of FV.
    jz      @f                              ;zf if Boot Block
    jc      Microcode_Not_Found             ;Corrupt FV?
@@:
    dec     edx                             ;edx = End of FFS

;---Find File ---
;ebx = Start of FFS
;edx = End of FFS
Get_Next_File:
    cmp     byte ptr [ebx + FFS_TYPE], 0f0h ;iqnore guid for pad type
    je  @f
	cmp		dword ptr [ebx], -1		;Is in end of files, but not firmware volume.
	je		File_not_found
@@:
	mov		ecx, 4
	mov		esi, ebx
	mov		edi, offset MICOCODE_FFS_GUID
	;compare file guid
	repe 	cmpsd
	je		Found_File
;---get next file--
	;get file length and add to ebx
	mov		eax, [ebx + FFS_LENGTH]
	and		eax, 0ffffffh			;eax = File length (24 bits)
	add		ebx, eax				;next file
	;align file
	add		ebx, 7
    jc      Microcode_Not_Found     ;Check for wrap-around if bad data.
	and		ebx, not 7

    mov     eax, ebx
    add     eax, FFS_HDR_LENGTH     ;eax is microcode data
    cmp     eax, edx                ;Is end of firmware volume?
	jb		Get_Next_File
File_not_found:
    jmp     Microcode_Not_Found
;---Found File---
Found_File:
;ebx = Start of Microcode FFS
;edx = End of FFS + 1

;---Search micocode for match.---
	mov		edi, ebx

	mov		eax, [ebx + FFS_LENGTH]
	and		eax, 0ffffffh			;eax = File length (24 bits)
	add		edi, eax				;edi = end of file
    jc      Microcode_Not_Found     ;Check for wrap-around if bad data.

IF MKF_MPDTable_CREATED
    movzx   eax, word ptr [edi-2]           ;eax = MPDT length
    sub     edi, eax                        ;edi = end of microcodes.
ENDIF

	add		ebx, FFS_HDR_LENGTH
    jc      Microcode_Not_Found     ;Check for wrap-around if bad data.

	mov		esi, ebx

    mov     eax, 1
    cpuid				; EAX = CPU signature.

	mov		ebx, eax	;ebx = CPU signature

	mov		ecx, 17h
	rdmsr				;read CPUS flags

	shr		edx, 18
	and		dl, 7		;dl = CPU FLAG
	mov		cl, dl
	mov		dl, 1
	shl		dl, cl		;dl = bit set for CPU flag

    xchg    ebx, esi    ;ebx = ptr, esi = signature

Next_Microcode:
	cmp		ebx, edi                ;Check to see if microcode at end of FFS File.
	jae		Microcode_Not_Found

	cmp		dword ptr [ebx], 1 ;Check if correct header revision
	jne		Microcode_Not_Found

	mov		ecx, 2048	;total size if data size in field = 0.
	cmp		dword ptr [ebx + uCODE_DATA_SIZE], 0
	jz		@f
	mov		ecx, [ebx + uCODE_TOTAL_SIZE]	;Get total size.
@@:

	cmp		esi, [ebx + uCODE_CPU_SIGNATURE]
	jne		Check_Ext_Sig

    mov     eax, ebx                        ;Store uC addr in eax, so if jnz succeeds.
	test	dl, [ebx + uCODE_CPU_FLAGS]     ;uC flags match?
	jnz		Exit_Find_Microcode

Check_Ext_Sig:
    mov     ebp, [ebx + uCODE_TOTAL_SIZE]	;Get total size.
    mov     eax, [ebx + uCODE_DATA_SIZE]    ;Get Data Size
    add     eax, 48
    cmp     ebp, eax
    jbe     Find_Next_Microcode             ;No extended signature.

    mov     ecx, [ebx + eax]                ;Get extended signature.
    cmp     ecx, 20
    jae     Microcode_Not_Found             ;Corrupt?
    lea     ebp, [ebx + eax + 20]           ;First Proc Signature.

@@:
    cmp     [ebp], esi                      ;Check Signature
    jne     Next_Ext_Sig

    mov     eax, ebx
    test    dl, [ebp + 4]                   ;Check processor flags
    jnz     Exit_Find_Microcode
Next_Ext_Sig:
    add     ebp, 12
    loop    @b
    mov     ecx, [ebx + uCODE_TOTAL_SIZE]	;Get total size.
Find_Next_Microcode:
    ;align to next block size
IF MKF_PACK_MICROCODE
    add     ecx, 15
    and     ecx, 0fffffff0h
ELSE
    add     ecx, MKF_MICROCODE_BLOCK_SIZE - 1
    and     ecx, NOT (MKF_MICROCODE_BLOCK_SIZE - 1)
ENDIF
	add		ebx, ecx
    jc      Microcode_Not_Found             ;Overflow - bad data.
	jmp		Next_Microcode

Microcode_Not_Found:
IF  MKF_MICROCODE_SPLIT_BB_UPDATE
    cmp     ebx, MKF_FV_MICROCODE_BASE
    jae     @F
    mov     ebx, MKF_FV_MICROCODE_BASE
    jmp     FindMicrocodeFv
@@:
ENDIF

	xor		eax, eax
Exit_Find_Microcode:
	or      eax, eax
    jz      uc_exit             ;No microcode found:
    
	;Update microcode
	mov		ecx, 79h
	xor		edx, edx
	add		eax, 48		;eax = Update data
	wrmsr				;Update microcode
uc_exit:

ENDIF
	RET_ESI
;    RET_EBP
VeryEarlyMicrocodeUpdate    ENDP
;<<<AMI_OVERRIDE_END

;
; Establish Stack
;
EstablishStack    PROC    NEAR    PRIVATE

  ;
  ; Enable STACK
  ;
  RET_ESI

EstablishStack    ENDP

;
; This is the entry point to PEI phase
;
CallPeiCoreEntryPoint   PROC    NEAR    PRIVATE
  ;
  ; Set stack top pointer
  ;
  mov     esp, TEMPORARY_RAM_BASE_ADDRESS       ;AMI_OVERRIDE
  add     esp, TEMPORARY_RAM_SIZE               ;AMI_OVERRIDE

  ;
  ; Push CPU count to stack first, then AP's (if there is one)
  ; BIST status, and then BSP's
  ;
  mov     ch, 01 ; for client we have only one BSP
  movzx   ecx, ch

  ; Save number of BSPs
  push  ecx

GetSbspBist:
  ; Save SBSP BIST
  movd  eax, mm0
  push  eax

  ; Save SBSP APIC ID
  movd  eax, mm1
  shr   eax, BspApicIdSaveStart               ; Resume APIC ID
  push  eax

  ; Save Time-Stamp Counter
  movd eax, mm5
  push eax

  movd eax, mm6
  push eax

TransferToSecStartup:

  ; Switch to "C" code
  STATUS_CODE (0Ch)

  ;
  ; Do platform specific TraceHub initialization prior to calling into core code.
  ; This is required for PCH if the TraceHub debugger is going to be used.
  ;
  ; Must preserve register values for call to SecStartup.
  ;
  pushad
  push 0  ; EnableMode = PchTraceHubModeDisabled
  call    TraceHubInitialize
  popad

  STATUS_CODE (0Dh)
;AMI_OVERRIDE_START >>>
  ;
  ; Pass entry point of the PEI core
  ;
  ;mov     edi, PEI_CORE_ENTRY_BASE      ; 0FFFFFFE0h
  ;push    DWORD PTR ds:[edi]
  
  ;
  ; Add for PI
  ;
  
  mov     eax, esp
  mov     ebx, 087655678h
  push    ebx
  push    eax
  mov     bx,  0
  push    bx
  lidt    fword  ptr [esp]        ;Load IDT register
  pop     bx
  pop     eax  
;<<<AMI_OVERRIDE_END

  ;
  ; Pass BFV into the PEI Core
  ;
  mov     edi, FV_MAIN_BASE             ; 0FFFFFFFCh
  push    DWORD PTR ds:[edi]

  ; @todo ECPoverride: SecStartup entry point needs 4 parameters
  push    TEMPORARY_RAM_BASE_ADDRESS    ;AMI_OVERRIDE

  ;
  ; Pass stack size into the PEI Core
  ;
  push    TEMPORARY_RAM_SIZE            ;AMI_OVERRIDE

  ;
  ; Pass Control into the PEI Core
  ;
;AMI_OVERRIDE_START >>>
  jmp     BeforeSECEntry
BeforeSECEntryEnd::
IFNDEF FSP_FLAG
  call SecStartup
ENDIF
CallPeiCoreEntryPoint   ENDP

;AMI_OVERRIDE_START >>>
IF 0
;<<<AMI_OVERRIDE_END
;
; Load MCU path, if not already loaded from FIT table
;
EarlyMicrocodeUpdate    PROC    NEAR    PUBLIC
    mov     edi, PcdGet32 (PcdFlashMicrocodeFvSize)
    or      edi, edi
    jz      @f

    add     edi, PcdGet32 (PcdFlashMicrocodeFvBase)  ; End addr of uCodes.

    mov     esi, PcdGet32 (PcdFlashMicrocodeFvBase)
    movzx   ebx, WORD PTR [esi + FVHEADER_LEN_OFF]
    add     esi, ebx
    add     esi, FFSHEADER_LEN ; add FFS header

    CALL_EBP  SearchAndLoadMicrocode
@@:
    RET_ESI
EarlyMicrocodeUpdate    ENDP
ENDIF
;<<<AMI_OVERRIDE_END

align 10h
PUBLIC  BootGdtTable

;
; GDT[0]: 0x00: Null entry, never used.
;
NULL_SEL        EQU $ - GDT_BASE        ; Selector [0]
GDT_BASE:
BootGdtTable        DD  0
                    DD  0
;
; Linear data segment descriptor
;
LINEAR_SEL      EQU $ - GDT_BASE        ; Selector [0x8]
    DW  0FFFFh                          ; limit 0xFFFFF
    DW  0                               ; base 0
    DB  0
    DB  092h                            ; present, ring 0, data, expand-up, writable
    DB  0CFh                            ; page-granular, 32-bit
    DB  0
;
; Linear code segment descriptor
;
LINEAR_CODE_SEL EQU $ - GDT_BASE        ; Selector [0x10]
    DW  0FFFFh                          ; limit 0xFFFFF
    DW  0                               ; base 0
    DB  0
    DB  09Bh                            ; present, ring 0, data, expand-up, not-writable
    DB  0CFh                            ; page-granular, 32-bit
    DB  0
;
; System data segment descriptor
;
SYS_DATA_SEL    EQU $ - GDT_BASE        ; Selector [0x18]
    DW  0FFFFh                          ; limit 0xFFFFF
    DW  0                               ; base 0
    DB  0
    DB  093h                            ; present, ring 0, data, expand-up, not-writable
    DB  0CFh                            ; page-granular, 32-bit
    DB  0

;
; System code segment descriptor
;
SYS_CODE_SEL    EQU $ - GDT_BASE        ; Selector [0x20]
    DW  0FFFFh                          ; limit 0xFFFFF
    DW  0                               ; base 0
    DB  0
    DB  09Ah                            ; present, ring 0, data, expand-up, writable
    DB  0CFh                            ; page-granular, 32-bit
    DB  0
;
; Spare segment descriptor
;
SYS16_CODE_SEL  EQU $ - GDT_BASE        ; Selector [0x28]
    DW  0FFFFh                          ; limit 0xFFFFF
    DW  0                               ; base 0
    DB  0Eh                             ; Changed from F000 to E000.
    DB  09Bh                            ; present, ring 0, code, expand-up, writable
    DB  00h                             ; byte-granular, 16-bit
    DB  0
;
; Spare segment descriptor
;
SYS16_DATA_SEL  EQU $ - GDT_BASE        ; Selector [0x30]
    DW  0FFFFh                          ; limit 0xFFFF
    DW  0                               ; base 0
    DB  0
    DB  093h                            ; present, ring 0, data, expand-up, not-writable
    DB  00h                             ; byte-granular, 16-bit
    DB  0

;
; Spare segment descriptor
;
SPARE5_SEL      EQU $ - GDT_BASE        ; Selector [0x38]
    DW  0                               ; limit 0
    DW  0                               ; base 0
    DB  0
    DB  0                               ; present, ring 0, data, expand-up, writable
    DB  0                               ; page-granular, 32-bit
    DB  0
GDT_SIZE        EQU $ - BootGdtTable    ; Size, in bytes

GdtDesc:                                ; GDT descriptor
OffsetGDTDesc   EQU $ - _ModuleEntryPoint
    DW  GDT_SIZE - 1                    ; GDT limit
    DD  OFFSET BootGdtTable             ; GDT base address

NemInitLinearAddress   LABEL   FWORD
NemInitLinearOffset    LABEL   DWORD
    DD  OFFSET ProtectedModeSecStart    ; Offset of our 32 bit code
    DW  LINEAR_CODE_SEL

;AMI_OVERRIDE_START >>>
MICOCODE_FFS_GUID label dword
	dd	17088572h
	dw	377Fh
	dw	44efh
	db	8Fh,4Eh,0B0h,9Fh,0FFh,46h,0A0h,70h
;<<< AMI_OVERRIDE_END
_TEXT_PROTECTED_MODE    ENDS
END
