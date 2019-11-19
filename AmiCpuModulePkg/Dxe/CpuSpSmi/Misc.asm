;**********************************************************************
;**********************************************************************
;**                                                                  **
;**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
;**                                                                  **
;**                       All Rights Reserved.                       **
;**                                                                  **
;**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
;**                                                                  **
;**                       Phone: (770)-246-8600                      **
;**                                                                  **
;**********************************************************************
;**********************************************************************

;<AMI_FHDR_START>
;**********************************************************************
;
; Name: Misc.asm
;
; Description:	
;
;**********************************************************************
;<AMI_FHDR_END>
;

.code

ClearDirectionFlag Proc
    cld
    ret
ClearDirectionFlag endp

MpMtrrSynchUpEntry  PROC        PUBLIC
    ;
    ; Enter no fill cache mode, CD=1(Bit30), NW=0 (Bit29)
    ;
    mov rax, cr0
    and rax, 0DFFFFFFFh
    or  rax, 040000000h
    mov cr0, rax
    ;
    ; Flush cache
    ;
    wbinvd
    ;
    ; Clear PGE flag Bit 7
    ;
    mov rax, cr4
    mov rdx, rax
    and rax, 0FFFFFF7Fh
    mov cr4, rax
    ;
    ; Flush all TLBs
    ;
    mov rax, cr3
    mov cr3, rax
    
    mov rax, rdx
    
    ret
    
MpMtrrSynchUpEntry  ENDP
    
MpMtrrSynchUpExit  PROC        PUBLIC
    ;
    ; Flush all TLBs the second time
    ;
    mov rax, cr3
    mov cr3, rax
    ;
    ; Enable Normal Mode caching CD=NW=0, CD(Bit30), NW(Bit29)
    ;
    mov rax, cr0
    and rax, 09FFFFFFFh
    mov cr0, rax
    ;
    ; Set PGE Flag in CR4 if set
    ;
    mov cr4, rcx
    ret

MpMtrrSynchUpExit  ENDP

;------------------------------------------------------------------------------
; UINT64
; EFIAPI
; AsmWriteMsr64 (
;   IN UINT32  Index,
;   IN UINT64  Value
;   );
;------------------------------------------------------------------------------
AsmWriteMsr64   PROC
    mov     edx, [esp + 12]
    mov     eax, [esp + 8]
    mov     ecx, [esp + 4]
    wrmsr
	mov     eax, [esp + 16]
    ret
AsmWriteMsr64   ENDP

END

;**********************************************************************
;**********************************************************************
;**                                                                  **
;**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
;**                                                                  **
;**                       All Rights Reserved.                       **
;**                                                                  **
;**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
;**                                                                  **
;**                       Phone: (770)-246-8600                      **
;**                                                                  **
;**********************************************************************
;**********************************************************************