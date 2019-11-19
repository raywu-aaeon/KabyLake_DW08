;*************************************************************************
;*************************************************************************
;**                                                                     **
;**        (C)Copyright 1985-2010, American Megatrends, Inc.            **
;**                                                                     **
;**                       All Rights Reserved.                          **
;**                                                                     **
;**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
;**                                                                     **
;**                       Phone: (770)-246-8600                         **
;**                                                                     **
;*************************************************************************
;*************************************************************************
;
; $Header: /Alaska/BIN/Chipset/Intel/SouthBridge/CougarPoint_A/Board/SBSECInit.asm 4     9/20/10 10:49p Yurenlai $
;
; $Revision: 4 $
;
; $Date: 9/20/10 10:49p $
;
;**********************************************************************

;<AMI_FHDR_START>
;----------------------------------------------------------------------------
;
; Name:  SBSECInit.ASM
;
; Description: Program any workaround or initialization needed before
;              enabling Cache as memory in the SEC stage
;
; Notes:       None of the chipset initialization is done at this point.
;              System is exactly as if came out of RESET.
;
;----------------------------------------------------------------------------
;<AMI_FHDR_END>

 INCLUDE token.equ

.586P
.XMM
.MODEL SMALL,C
.code

; Externs
EXTERN SECSB_EarlyInitEnd:NEAR32

;PUBLIC SECSB_EarlyInit

; Define the equates here
BIT00          equ 00000001h
BIT01          equ 00000002h
BIT02          equ 00000004h
BIT03          equ 00000008h
BIT04          equ 00000010h
BIT05          equ 00000020h
BIT06          equ 00000040h
BIT07          equ 00000080h
BIT08          equ 00000100h
BIT09          equ 00000200h

PCI_LPC_BASE   equ 8000F800h
LPC_SS         equ 2Ch
LPC_REG_RCBA   equ 0F0h
ICH_RCRB_CIR1  equ 088h
ICH_REG_NMI_SC equ 061h

PCI_P2SB_BASE  EQU 8000F900h
PCI_P2SB_SBREG_BAR  EQU 10h
PCI_COMMAND_OFFSET  EQU 04h
PCH_PCR_BASE  EQU 0FD000000h
PCR_PID_SMB  EQU 0C60000h
PCR_SMBUS_GC  EQU 0Ch

LPC_IO_DEC     equ 80h
LPC_GEN1_DEC   equ 84h
LPC_GEN2_DEC   equ 88h
LPC_GEN3_DEC   equ 8Ch
LPC_GEN4_DEC   equ 90h
IO_DEC_DEFAULT equ 370F0010h

RCRB_RTC_CONF                   EQU     3400h   ; RTC Configuration register 

;<AMI_PHDR_START>
;----------------------------------------------------------------------------
;
; Procedure: SECSB_EarlyInit
;
; Description: This routine initializes South bridge for PEI preparation
;
; Input: ESP BIST Info
;  EBP Reset ID (EDX at reset)
;  Stack not available
;
; Output: None
;
; Modified: All, except EBP and ESP
;
;----------------------------------------------------------------------------
;<AMI_PHDR_END>
SECSB_EarlyInit  PROC NEAR PUBLIC

        mov al, 04h
        out 80h, al

 ;; Program 8259 Interrupt Controller to disable all interrupts
        mov al, 0FFh
        out 21h, al    ;; Mask off all interrupts in master 8259
        out 0a1h, al   ;; Mask off all interrupts in slave 8259

        push eax
        push edx
;; Enable PCR base address in PCH
       mov dx, 0CF8h
       mov eax, (PCI_P2SB_BASE + PCI_P2SB_SBREG_BAR)
       out dx, eax
       add dx, 04h
       mov eax, PCH_PCR_BASE
       out dx, eax
       
;; Enable P2SB MSE       
       mov dx, 0CF8h
       mov eax, (PCI_P2SB_BASE + PCI_COMMAND_OFFSET)
       out dx, eax
       add dx, 04h
       mov eax, BIT01
       out dx, eax
       
;; Disable the Watchdog timer expiration from causing a system reset
        mov eax, (PCH_PCR_BASE + PCR_PID_SMB + PCR_SMBUS_GC) 
        mov edx, dword PTR [eax]
        or   edx, BIT01  
        mov dword PTR [eax], edx

        pop edx
        pop eax

                                        ; (P092010G)>
; Remove at PCH BIOS Spec Rev 0.9.0, Section 7.1.5.
; For TXT enable platform W/A, system BIOS is required to implement the following steps
; prior to load microcode patch.
;-        jmp TXT_WA_EarlyInit
TXT_WA_EarlyInitEnd::
                                        ; <(P092010G)

;        jmp SEC_SIO_EarlyInit
SEC_SIO_EarlyInitEnd::
        jmp SECSB_EarlyInitEnd
SECSB_EarlyInit  ENDP



;<AMI_PHDR_START>
;----------------------------------------------------------------------------
;
; Procedure: TXT_WA_EarlyInit
;
; Description: Intel PCH Spec. v1.0, 7.1.5.2 With *dale Processor
;              For TXT enable platform W/A, system BIOS is required to
;              disable NMI and implement the following steps prior to
;              load microcode patch.
;              a. If RCBA + Offset 84h[20] = '1b'
;                i.  Set "PCI_SERR_EN" bit (Port 61h[2]) and cleared it later
;                    to '0b' to clear SERR status bit.
;                ii. Write '1' to clear RCBA + Offset 84h[20]
;              b. Set RCBA + Offset 88h[20] ='1b'.
;
; Input: None
;
; Output: None
;
; Modified: 
;
;----------------------------------------------------------------------------
;<AMI_PHDR_END>

;TXT_WA_EarlyInit PROC PUBLIC
        ;Enable Maping RCBA
;        mov     dx, 0CF8h
;        mov     eax, (PCI_LPC_BASE + LPC_REG_RCBA)
;        out     dx, eax
;        mov     eax, (MKF_SB_RCBA + 01h)
;        add     dx, 04h
;        out     dx, eax

        ; 3.1 If RCBA + Offset 2084h[20] = 1b
;        mov     edi , MKF_SB_RCBA
;        test    dword ptr[edi + 2084h], 00100000h
;        jz      wa_exit

        ; 3.1.1 Set "PCI_SERR_EN" bit (Port 61h[2]) and cleared it later to `0b' to clear SERR status bit
;        in      al, ICH_REG_NMI_SC
;        or      al, 04h
;        out     ICH_REG_NMI_SC, al

        ; 3.1.2 Write 1 to clear RCBA + Offset 2084h [20]
;        or      dword ptr[edi + 2084h], 00100000h
    
;wa_exit:
        ; 3.2 Set RCBA + Offset 2088h[20] = 1b
;        or      dword ptr[edi + ICH_RCRB_CIR1], 00100000h

        ; 3.3 Clear "PCI_SERR_EN" bit (Port 61h[2])
;        in      al, ICH_REG_NMI_SC
;        and     al, 0FBh
;        out     ICH_REG_NMI_SC, al

;        jmp TXT_WA_EarlyInitEnd
;TXT_WA_EarlyInit  ENDP

;<AMI_PHDR_START>
;----------------------------------------------------------------------------
;
; Procedure: SEC_SIO_EarlyInit
;
; Description: This routine initializes IT8728F Port-80 for PEI preparation
;
; Input: None
;
; Output: None
;
; Modified: EAX and EBX
;
; Notes: Super I/O Chip is IT8728F
;
;----------------------------------------------------------------------------
;<AMI_PHDR_END>
SEC_SIO_EarlyInit PROC PUBLIC

        push eax
        push ebx

        ; GCS - General Control and Status Register
        ; [2]:Reserved Page Route
        ;  0:Writes will be forwarded to LPC, shadowed within the PCH, and reads will be returned
        ;    from the internal shadow
        ;  1:Writes will be forwarded to PCI, shadowed within the PCH, and reads will be returned
        ;    from the internal shadow.
;        mov ebx, (MKF_SB_RCBA + 3410h)
;        mov al, byte ptr [ebx]
;        and al, (BIT02)
;        jnz EXIT
        jmp EXIT

IFDEF MKF_IT8728_CONFIG_INDEX
  IFDEF MKF_IT8728_CONFIG_DATA
        ; Please make sure MKF_SB_IO_DECODE_IN_SEC token is set to 1.
        ; It is to enable the decoding of the I/O locations 2Eh and 2Fh to the LPC interface.
        ; This range is used for Super I/O devices.

        ; Enter Configuration Mode.
        mov al, 87h
        out MKF_IT8728_CONFIG_INDEX, al
        mov al, 01h
        out MKF_IT8728_CONFIG_INDEX, al
        mov al, 55h
        out MKF_IT8728_CONFIG_INDEX, al
    IF (MKF_IT8728_CONFIG_INDEX EQ 2Eh)
        mov al, 55h
    ELSE
        mov al, 0AAh
    ENDIF
        out MKF_IT8728_CONFIG_INDEX, al

        ; IT8728 Chip ID Byte 1: 87h
        mov al, 20h
        out MKF_IT8728_CONFIG_INDEX, al
        in  al,MKF_IT8728_CONFIG_DATA
        cmp al, 87h
        jnz EXIT

        ; IT8728 Chip ID Byte 2: 28h
        mov al, 21h
        out MKF_IT8728_CONFIG_INDEX, al
        in  al,MKF_IT8728_CONFIG_DATA
        cmp al, 28h
        jnz EXIT

        ; Parallel Port Configuration Registers (LDN=03h)
        mov al, MKF_IT8728_LDN_SEL_REGISTER
        out MKF_IT8728_CONFIG_INDEX, al
        mov al, 03h
        out MKF_IT8728_CONFIG_DATA, al

        ; Parallel Port Activate (Index=30h, Default=00h)
        ; [7:1]:Reserved.
        ; [0]:Parallel Port Enable - 1:Enable/0:Disable.
        mov al, 30h
        out MKF_IT8728_CONFIG_INDEX, al
        mov al, 00h
        out MKF_IT8728_CONFIG_DATA, al

        ; Parallel Port Special Configuration Register (Index=F0h, Default=0Bh)
        ; [7:6]:Port-80 Output Slection - 00:Normal Port-80/01:EC Index 29hReading/10:EC Index 2Ah Reading/11:EC Index 2Bh Reading.
        ; [5:4]:Reserved.
        ; [3]:Port-80 Function - 1:Disable/0:Enable.
        ; [2]:IRQ Type - 1:IRQ sharing/0:Normal.
        ; [1:0]:Parallel Port Modes - 00:SPP mode/01:EPP mode/10:ECP mode/11:EPP mode & ECP mode.
        mov al, 0F0h
        out MKF_IT8728_CONFIG_INDEX, al
        mov al, 00h
        out MKF_IT8728_CONFIG_DATA, al

        ; Exit Configuration Mode.
        mov al, 02h
        out MKF_IT8728_CONFIG_INDEX, al
        mov al, 02h
        out MKF_IT8728_CONFIG_DATA, al
  ENDIF
ENDIF

EXIT:
        pop ebx
        pop eax

        jmp SEC_SIO_EarlyInitEnd

SEC_SIO_EarlyInit  ENDP

END
;*************************************************************************
;*************************************************************************
;**                                                                     **
;**        (C)Copyright 1985-2010, American Megatrends, Inc.            **
;**                                                                     **
;**                       All Rights Reserved.                          **
;**                                                                     **
;**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
;**                                                                     **
;**                       Phone: (770)-246-8600                         **
;**                                                                     **
;*************************************************************************
;*************************************************************************;
