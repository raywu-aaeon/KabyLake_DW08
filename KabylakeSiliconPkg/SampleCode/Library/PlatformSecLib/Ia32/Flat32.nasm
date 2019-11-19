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


%include "PlatformNasm.inc"
%include "ChipsetNasm.inc"
%include "SecCoreNasm.inc"

extern   ASM_PFX(SecStartup)
extern   ASM_PFX(SecSaInit)

;
; TraceHubInitialize is a helper function to correctly configure the TraceHub controller
;
extern   ASM_PFX(TraceHubInitialize)

extern   ASM_PFX(PcdGet32 (PcdTemporaryRamBase))
extern   ASM_PFX(PcdGet32 (PcdTemporaryRamSize))
extern   ASM_PFX(PcdGet64(PcdPciExpressBaseAddress))
extern   ASM_PFX(PcdGet64 (PcdMchBaseAddress))
extern   ASM_PFX(PcdGet32 (PcdFlashMicrocodeFvBase))
extern   ASM_PFX(PcdGet32 (PcdFlashMicrocodeFvSize))

%ifdef FSP_FLAG
extern   ASM_PFX(TempRamInitApi)
extern   ASM_PFX(FspInitApi)
%endif

%ifdef SV_HOOKS
extern   ASM_PFX(PchSvSbSecEarlyInit)
%endif ; SV_HOOKS

extern   ASM_PFX(SearchAndLoadMicrocode)
extern   ASM_PFX(SecCarInit)

;_TEXT_REALMODE      SEGMENT PARA PUBLIC USE16 'CODE'
;                   ASSUME  CS:_TEXT_REALMODE, DS:_TEXT_REALMODE
global ASM_PFX(_TEXT_REALMODE)
ASM_PFX(_TEXT_REALMODE):
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

global ASM_PFX(_ModuleEntryPoint)
ASM_PFX(_ModuleEntryPoint):
BITS    16
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

NotWarmStart:

  ;
  ; Enter Protected mode.
  ;
  STATUS_CODE (01h)                     ; BSP_PROTECTED_MODE_START
  mov     esi,   GdtDesc
  DB      66h
  lgdt   [cs:si]     ;fword
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
  mov     esi, ASM_PFX(NemInitLinearAddress)
  jmp     far dword [cs:si]

TightLoop:
  cli
  hlt
  jmp     TightLoop


;_TEXT_PROTECTED_MODE      SEGMENT PARA PUBLIC USE32 'CODE'
;                          ASSUME  CS:_TEXT_PROTECTED_MODE, DS:_TEXT_PROTECTED_MODE
global ASM_PFX(_TEXT_PROTECTED_MODE)
ASM_PFX(_TEXT_PROTECTED_MODE):

;
;args 1:ReturnAddress 2: MmxRegister
;
%ifdef FSP_FLAG
%macro LOAD_MMX_EXT  2
  mov     esi, %1
  movd    %2, esi              ; save ReturnAddress into MM7
%endmacro


;
;args 1:RoutineLabel 2:MmxRegister
;
%macro CALL_MMX_EXT    2
  mov     esi,  %%ReturnAddress
  movd    %2, esi              ; save ReturnAddress into MM7
  jmp     %1
%%ReturnAddress:
%endmacro

;
;args 1:MmxRegister
;
%macro RET_ESI_EXT 1
  movd    esi, %1              ; restore ESP from MM7
  jmp     esi
%endmacro
%endif; FSP_FLAG


;
;args 1:RoutineLabel
;
%macro CALL_MMX 1

  mov     esi,  %%ReturnAddress
  movd    mm7, esi                      ; save ReturnAddress into MM7
  jmp     %1
%%ReturnAddress:

%endmacro


;
;args none
;
%macro RET_ESI 0

  movd    esi, mm7                      ; restore ESP from MM7
  jmp     esi

%endmacro


;
;args 1:RoutineLabel
;
%macro CALL_EBP  1

  mov     ebp,  %%ReturnAddress
  jmp     %1
%%ReturnAddress:

%endmacro


;
;args none
;
%macro RET_EBP  0

  jmp     ebp                           ; restore ESP from EBP

%endmacro

align 4

global ASM_PFX(ProtectedModeSecStart)
ASM_PFX(ProtectedModeSecStart):
BITS    32
%ifndef FSP_FLAG

  STATUS_CODE (02h)
  CALL_MMX  ASM_PFX(EarlyMicrocodeUpdate)
%ifdef SV_HOOKS
  jmp PchSvSbSecEarlyInit
PchSvSbSecEarlyInitEndReal::
%endif ; SV_HOOKS
  STATUS_CODE (03h)
  CALL_MMX  ASM_PFX(SecPlatformInit)

  STATUS_CODE (04h)
  CALL_MMX  ASM_PFX(SecCarInit)

  STATUS_CODE (05h)
  CALL_MMX  ASM_PFX(EstablishStack)

  STATUS_CODE (06h)
  jmp  ASM_PFX(CallPeiCoreEntryPoint)

%else

  mov  esp,  Continue
  jmp  ASM_PFX(TempRamInitApi)
Continue:
  sub  esp, 4
  push 09FFFCh ; Memory Stack Top
  push esp     ; PlatformData Parameter Pointer
  call ASM_PFX(FspInitApi)
  add  esp, 8

  ; Should never return
  jmp  $

%endif


%ifdef SV_HOOKS
global ASM_PFX(PchSvSbSecEarlyInitEnd)
ASM_PFX(PchSvSbSecEarlyInitEnd):
  jmp PchSvSbSecEarlyInitEndReal

%endif ; SV_HOOKS
;
; Perform early platform initialization
;

global ASM_PFX(SecPlatformInit)
ASM_PFX(SecPlatformInit):

  CALL_EBP  ASM_PFX(SecSaInit)

  ;
  ; Enable HPET decode in PCH.
  ;
  mov     esi, DWORD  [ASM_PFX(PcdGet64(PcdPciExpressBaseAddress))]
  add     esi, PCH_HPET_REG
  mov     al,  PCH_HPET_AE
  mov     Byte  [esi], al
  mov     al,  Byte [esi]                     ; read back
  xor     eax, eax
  mov     esi, HPET_COMP_1
  mov     Dword [esi], eax
  mov     esi, HPET_COMP_2
  mov     Dword  [esi], eax
  RET_ESI


;
; Establish Stack
;

global ASM_PFX(EstablishStack)
ASM_PFX(EstablishStack):
  ;
  ; Enable STACK
  ;
  RET_ESI


;
; This is the entry point to PEI phase
;

global ASM_PFX(CallPeiCoreEntryPoint)
ASM_PFX(CallPeiCoreEntryPoint):
  ;
  ; Set stack top pointer
  ;
  mov     esp, DWORD [ASM_PFX(PcdGet32 (PcdTemporaryRamBase))]
  add     esp, DWORD [ASM_PFX(PcdGet32 (PcdTemporaryRamSize))]

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
  call    ASM_PFX(TraceHubInitialize)
  popad

  STATUS_CODE (0Dh)

  ;
  ; Pass entry point of the PEI core
  ;
  mov     edi, PEI_CORE_ENTRY_BASE      ; 0FFFFFFE0h
  push    DWORD  [ds:edi]

  ;
  ; Pass BFV into the PEI Core
  ;
  mov     edi, FV_MAIN_BASE             ; 0FFFFFFFCh
  push    DWORD  [ds:edi]

  ; @todo ECPoverride: SecStartup entry point needs 4 parameters
  push    DWORD [ASM_PFX(PcdGet32 (PcdTemporaryRamBase))]

  ;
  ; Pass stack size into the PEI Core
  ;
  push    DWORD [ASM_PFX(PcdGet32 (PcdTemporaryRamSize))]

  ;
  ; Pass Control into the PEI Core
  ;
  call ASM_PFX(SecStartup)

;
; Load MCU path, if not already loaded from FIT table
;

global ASM_PFX(EarlyMicrocodeUpdate)
ASM_PFX(EarlyMicrocodeUpdate):
%ifdef SEC_LOAD_UCODE_FLAG
    mov     esi, DWORD [ASM_PFX(PcdGet32 (PcdFlashMicrocodeFvBase))]
    movzx   ebx, WORD  [esi + FVHEADER_LEN_OFF]
    add     esi, ebx
    add     esi,  FFSHEADER_LEN ; add FFS header

    mov     edi, DWORD [ASM_PFX(PcdGet32 (PcdFlashMicrocodeFvBase))]
    mov     ebx, DWORD [ASM_PFX(PcdGet32 (PcdFlashMicrocodeFvSize))]
    add     edi, ebx                    ; End addr of uCodes.

    CALL_EBP  ASM_PFX(SearchAndLoadMicrocode)
%endif ;SEC_LOAD_UCODE_FLAG

    RET_ESI

align 16

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
    DW  GDT_SIZE - 1                    ; GDT limit
    DD   BootGdtTable             ; GDT base address

ASM_PFX(NemInitLinearAddress):
NemInitLinearOffset:
    DD  ASM_PFX(ProtectedModeSecStart)  ; Offset of our 32 bit code
    DW  LINEAR_CODE_SEL
