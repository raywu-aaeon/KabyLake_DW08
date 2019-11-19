;@file
;
; @copyright
;  INTEL CONFIDENTIAL
;  Copyright 2011 - 2014 Intel Corporation.
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
;;
  .686p
  .MODEL FLAT,C
  .CODE

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;  Procedure:   DoSwbpeTypeIoOut
;
;  Input:       PortAddress   -  Port Address for doing IO Write
;               PortData      -  Data value for doing IO Write
;               PortDataWidth -  Data Width (Byte=1/Word=2/Dword=4) for doing IO Write
;               Mailbox       -  Data structure contains Data Store informations
;
;  Output:      Debuggers can update structure pointed by EDI via this IO Write Break.
;
;  Registers:   All are preserved
;
;  Description: Function to do an IO Write and set EDI EDI pointer to Mailbox structure,
;               so Debuggers can recognize the GUID and update status in the structure.
;
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

DoSwbpeTypeIoOut    PROC C  USES ECX EDX EDI PortAddress:DWORD, PortData:DWORD, PortDataWidth:DWORD, Mailbox:DWORD
    ;
    mov     edi, Mailbox
    mov     edx, PortAddress
    mov     eax, PortData
    mov     ecx, PortDataWidth
    ;
    cmp     ecx, 1
    jne     TryIoOutWord
    out     dx, al
    jmp     IoOutReturn
    ;
TryIoOutWord:
    cmp     ecx, 2
    jne     TryIoOutDword
    out     dx, ax
    jmp     IoOutReturn
    ;
TryIoOutDword:
    cmp     ecx, 4
    jne     IoOutReturn
    out     dx, eax
    ;
IoOutReturn:
    ret
    ;
DoSwbpeTypeIoOut    ENDP

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;  Procedure:   DoSwbpeTypeIoIn
;
;  Input:       PortAddress   -  Port Address for doing IO Read
;               PortDataWidth -  Port data width for doing IO Read
;               Mailbox       -  Data structure contains Data Store informations
;
;  Output:      Debuggers can update structure pointed by EDI via this IO Read Break,
;               EAX contains IO Read byte/word/dword value depends on PortDataWidth.
;
;  Registers:   All are preserved
;
;  Description: Function to do an IO Read and set EDI pointer to Mailbox structure,
;               so Debuggers can recognize the GUID and update status in the structure.
;
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

DoSwbpeTypeIoIn    PROC C   USES ECX EDX EDI PortAddress:DWORD, PortDataWidth:DWORD, Mailbox:DWORD
    ;
    xor     eax, eax
    mov     edi, Mailbox
    mov     edx, PortAddress
    mov     ecx, PortDataWidth
    ;
    cmp     ecx, 1
    jne     TryIoInWord
    in      al, dx
    jmp     IoInReturn
    ;
TryIoInWord:
    cmp     ecx, 2
    jne     TryIoInDword
    in      ax, dx
    jmp     IoInReturn
    ;
TryIoInDword:
    cmp     ecx, 4
    jne     IoInReturn
    in      eax, dx
    ;
IoInReturn:
    ret
    ;
DoSwbpeTypeIoIn    ENDP


;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;  Procedure:   DoSwbpeTypeMemWrite
;
;  Input:       MemAddress   -  Memory Address for doing MEM Write
;               MemData      -  Data for doing Memory Write
;               MemDataWidth -  Write Data Width (Byte=1/Word=2/DWord=4)
;               Mailbox      -  Data structure contains Data Store info
;
;  Output:      Debuggers can update structure pointed by EDI via this MEM Write Break
;
;  Registers:   All are preserved
;
;  Description: Function to do Memory Write and set EDI pointer to Mailbox structure,
;               so Debuggers can recognize the GUID and update status in the structure.
;
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

DoSwbpeTypeMemWrite    PROC C  USES ECX EDX EDI MemAddress:DWORD, MemData:DWORD, MemDataWidth:DWORD, Mailbox:DWORD
    ;
    mov     edi, Mailbox
    mov     edx, MemAddress
    mov     eax, MemData
    mov     ecx, MemDataWidth
    ;
    cmp     ecx, 1
    jne     TryMemWriteWord
    mov     BYTE PTR [edx], al
    jmp     MemWriteReturn
    ;
TryMemWriteWord:
    cmp     ecx, 2
    jne     TryMemWriteDword
    mov     WORD PTR [edx], ax
    jmp     MemWriteReturn
    ;
TryMemWriteDword:
    cmp     ecx, 4
    jne     MemWriteReturn
    mov     DWORD PTR [edx], eax
    ;
MemWriteReturn:
    ret
    ;
DoSwbpeTypeMemWrite    ENDP

;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;  Procedure:   DoSwbpeTypeMemRead
;
;  Input:       MemAddress   -  Memory Address for doing MEM Write
;               MemDataWidth -  Write Data Width (Byte=1/Word=2/DWord=4)
;               Mailbox      -  Data structure contains Data Store info
;
;  Output:      Debuggers can update structure pointed by EDI via this MEM Read Break,
;               EAX contains MEM Read byte/word/dword value depends on PortDataWidth.
;
;  Registers:   All are preserved
;
;  Description: Function to do Memory Read and set EDI pointer to Mailbox structure,
;               so Debuggers can recognize the GUID and update status in the structure.
;
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

DoSwbpeTypeMemRead    PROC C   USES ECX EDX EDI MemAddress:DWORD, MemDataWidth:DWORD, Mailbox:DWORD
    ;
    xor     eax, eax
    mov     edi, Mailbox
    mov     edx, MemAddress
    mov     ecx, MemDataWidth
    ;
    cmp     ecx, 1
    jne     TryMemReadWord
    mov     al, BYTE PTR [edx]
    jmp     MemReadReturn
    ;
TryMemReadWord:
    cmp     ecx, 2
    jne     TryMemReadDword
    mov     ax, WORD PTR [edx]
    jmp     MemReadReturn
    ;
TryMemReadDword:
    cmp     ecx, 4
    jne     MemReadReturn
    mov     eax, DWORD PTR [edx]
    ;
MemReadReturn:
    ret
    ;
DoSwbpeTypeMemRead    ENDP


;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;  Procedure:   DoSwbpeTypeInt3
;
;  Input:       Mailbox -  Data structure contains Data Store informations
;
;  Output:      Debuggers can update structure pointed by EDI via this Interrupt Break
;
;  Registers:   All are preserved
;
;  Description: Function to do Interrupt 3 and set EDI pointer to Mailbox structure,
;               so Debuggers can recognize the GUID and update status in the structure.
;
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

DoSwbpeTypeInt3    PROC C   USES EDI    Mailbox:DWORD
    ;
    mov     edi, Mailbox
    int     3
    ret
    ;
DoSwbpeTypeInt3    ENDP


END
