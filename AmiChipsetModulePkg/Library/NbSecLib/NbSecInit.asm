        TITLE NBSECInit.ASM -- North Bridge SEC initialization
;*************************************************************************
;*************************************************************************
;**                                                                     **
;**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
;**                                                                     **
;**                       All Rights Reserved.                          **
;**                                                                     **
;**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
;**                                                                     **
;**                       Phone: (770)-246-8600                         **
;**                                                                     **
;*************************************************************************
;*************************************************************************

;*************************************************************************
; $Header: /Alaska/BIN/Chipset/Intel/NorthBridge/SandyBridge_A/Board/NBSECInit.asm 1     2/24/10 10:38p Tonywu $
;
; $Revision: 1 $
;
; $Date: 2/24/10 10:38p $
;*************************************************************************
;<AMI_FHDR_START>
;
; Name:         NBSECInit.ASM
;
; Description:  Program any workaround or initialization needed before
;               enabling Cache as memory in the SEC stage
;
;<AMI_FHDR_END>
;*************************************************************************

;----------------------------------------------------------------------------
        INCLUDE token.equ
;----------------------------------------------------------------------------

.586P
.XMM
.MODEL SMALL, C
.code

; Externs
EXTERN  SECNB_EarlyInitEnd:NEAR32


; Define the equates here
NB_BUS                          EQU 0
NB_DEV_FUN                      EQU 0

MSR_IA32_PLATFORM_ID            EQU 0017h

;<AMI_PHDR_START>
;----------------------------------------------------------------------------
;
; Procedure:    SECNB_EarlyInit
;
; Description:  This routine initializes north bridge for PEI preparation
;
; Input:        ESP BIST Info
;               EBP Reset ID (EDX at reset)
;               Stack not available
;
; Output:       None
;
; Modified:     All, except EBP and ESP
;
; Notes:        None.
;
;----------------------------------------------------------------------------
;<AMI_PHDR_END>
SECNB_EarlyInit     PROC PUBLIC

        mov     al, 03h
        out     80h, al


        ; Program PCI Express base address
        ; SNB bug [2992042]
        ; Programming of PCIEXBAR has to be done in two steps:
        ;  1. Program the length first - bits [2:1]
        ;  2. Program the base and enable bit (read / modify / write).
;        mov   eax,  (1 shl 31t) + (NB_BUS shl 16t) + (NB_DEV_FUN shl 8) + 60h     ; 0:0:0:60                           ; 0:0:0:60
;        mov   dx,   0CF8h
;        out   dx,   eax
;        mov   dl,   0FCh
;;        mov   eax,  MKF_PCIEX_LENGTH_BIT_SETTING        ; Program the length first.
;        out   dx,   eax
;        in    eax,  dx
;        or    eax,  MKF_PCIEX_BASE_ADDRESS OR 1         ; Program the Base and Enable bit.
;        out   dx,   eax

        ; Program PXPEPBAR address
;        mov   eax,  (1 shl 31t) + (NB_BUS shl 16t) + (NB_DEV_FUN shl 8) + 40h     ; 0:0:0:40                           ; 0:0:0:60
;        mov   dx,   0CF8h
;        out   dx,   eax
;        mov   dl,   0FCh
;        mov   eax,  MKF_NB_EP_BASE_ADDRESS OR 1            ; Set the Enable bit.
;        out   dx,   eax

        ; Program MCHBAR address
;        mov   eax,  (1 shl 31t) + (NB_BUS shl 16t) + (NB_DEV_FUN shl 8) + 48h     ; 0:0:0:48                           ; 0:0:0:60
;        mov   dx,   0CF8h
;        out   dx,   eax
;        mov   dl,   0FCh
;        mov   eax,  MKF_NB_MCH_BASE_ADDRESS OR 1            ; Set the Enable bit.
;        out   dx,   eax

        ; Program DMIBAR address
;        mov   eax,  (1 shl 31t) + (NB_BUS shl 16t) + (NB_DEV_FUN shl 8) + 68h     ; 0:0:0:68                           ; 0:0:0:60
;        mov   dx,   0CF8h
;        out   dx,   eax
;        mov   dl,   0FCh
;        mov   eax,  MKF_NB_DMI_BASE_ADDRESS OR 1            ; Set the Enable bit.
;        out   dx,   eax

        jmp     SECNB_EarlyInitEnd
SECNB_EarlyInit     ENDP

END

;*************************************************************************
;*************************************************************************
;**                                                                     **
;**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
;**                                                                     **
;**                       All Rights Reserved.                          **
;**                                                                     **
;**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
;**                                                                     **
;**                       Phone: (770)-246-8600                         **
;**                                                                     **
;*************************************************************************
;*************************************************************************
