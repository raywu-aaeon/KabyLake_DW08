#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
#**                                                                     **
#**                       All Rights Reserved.                          **
#**                                                                     **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
#**                                                                     **
#**                       Phone: (770)-246-8600                         **
#**                                                                     **
#*************************************************************************
#*************************************************************************
#
# $Revision: 4 $
#
# $Date: 9/20/10 10:49p $
#
#**********************************************************************

#<AMI_FHDR_START>
#----------------------------------------------------------------------------
#
# Name:  SBSECInit.S
#
# Description: Program any workaround or initialization needed before
#              enabling Cache as memory in the SEC stage
#
# Notes:       None of the chipset initialization is done at this point.
#              System is exactly as if came out of RESET.
#
#----------------------------------------------------------------------------
#<AMI_FHDR_END>

#include <Token.h>

# Externs


#PUBLIC SECSB_EarlyInit

# Define the equates here
.equ BIT00,           0x00000001
.equ BIT01,           0x00000002
.equ BIT02,           0x00000004
.equ BIT03,           0x00000008
.equ BIT04,           0x00000010
.equ BIT05,           0x00000020
.equ BIT06,           0x00000040
.equ BIT07,           0x00000080
.equ BIT08,           0x00000100
.equ BIT09,           0x00000200

.equ PCI_LPC_BASE,    0x8000F800
.equ LPC_SS,               0x2c
.equ LPC_REG_RCBA,  0xf0
.equ ICH_RCRB_CIR1,  0x88
.equ ICH_REG_NMI_SC,  0x61

.equ PCI_P2SB_BASE,    0x8000F900
.equ PCI_P2SB_SBREG_BAR,    0x10
.equ PCI_COMMAND_OFFSET,    0x04
.equ PCH_PCR_BASE,    0xFD000000
.equ PCR_PID_SMB,             0xC60000
.equ PCR_SMBUS_GC,   0x0C

.equ LPC_IO_DEC,         0x80
.equ LPC_GEN1_DEC,    0x84
.equ LPC_GEN2_DEC,    0x88
.equ LPC_GEN3_DEC,    0x8c
.equ LPC_GEN4_DEC,    0x90
.equ IO_DEC_DEFAULT,  0x370f0010

.equ RCRB_RTC_CONF,  0x3400   # RTC Configuration register 

.macro    RET_ESI
  movd    %mm7, %esi                     # restore ESP from MM7
  jmp     *%esi
.endm

#<AMI_PHDR_START>
#----------------------------------------------------------------------------
#
# Procedure: SECSB_EarlyInit
#
# Description: This routine initializes South bridge for PEI preparation
#
# Input: ESP BIST Info
#  EBP Reset ID (EDX at reset)
#  Stack not available
#
# Output: None
#
# Modified: All, except EBP and ESP
#
#----------------------------------------------------------------------------
#<AMI_PHDR_END>
ASM_GLOBAL ASM_PFX(SECSB_EarlyInit)
ASM_PFX(SECSB_EarlyInit):

        movb   $0x04, %al
        out       %al, $0x80 

 # Program 8259 Interrupt Controller to disable all interrupts
        movb   $0xff, %al
        out       %al, $0x21  # Mask off all interrupts in master 8259
        out       %al, $0xa1  # Mask off all interrupts in master 8259
                
        push %eax
        push %edx
# Enable PCR base address in PCH
        mov $0x0CF8, %dx
        movl $(PCI_P2SB_BASE + PCI_P2SB_SBREG_BAR), %eax
        out %eax, %dx
        add $0x4, %dx
        movl $PCH_PCR_BASE, %eax
        out %eax, %dx

# Enable P2SB MSE
        mov $0x0CF8, %dx
        movl $(PCI_P2SB_BASE + PCI_COMMAND_OFFSET), %eax
        out %eax, %dx
        add $0x4, %dx
        movl $BIT01, %eax
        out %eax, %dx
        
# Disable the Watchdog timer expiration from causing a system reset
        movl $(PCH_PCR_BASE + PCR_PID_SMB + PCR_SMBUS_GC), %eax
        movl (%eax), %edx
        orl   $BIT01, %edx
        movl %edx, (%eax)
        
         pop %edx
         pop %eax

TXT_WA_EarlyInitEnd:
                                        # <(P092010G)

#        jmp SEC_SIO_EarlyInit
SEC_SIO_EarlyInitEnd:
 
RET_ESI

#<AMI_PHDR_START>
#----------------------------------------------------------------------------
#
# Procedure: SEC_SIO_EarlyInit
#
# Description: This routine initializes IT8728F Port-80 for PEI preparation
#
# Input: None
#
# Output: None
#
# Modified: EAX and EBX
#
# Notes: Super I/O Chip is IT8728F
#
#----------------------------------------------------------------------------
#<AMI_PHDR_END>
ASM_GLOBAL ASM_PFX(SEC_SIO_EarlyInit)
ASM_PFX(SEC_SIO_EarlyInit):

        push %eax
        push %ebx

        # GCS - General Control and Status Register
        # [2]:Reserved Page Route
        #  0:Writes will be forwarded to LPC, shadowed within the PCH, and reads will be returned
        #    from the internal shadow
        #  1:Writes will be forwarded to PCI, shadowed within the PCH, and reads will be returned
        #    from the internal shadow.
#        mov ebx, (MKF_SB_RCBA + 3410h)
#        mov al, byte ptr [ebx]
#        and al, (BIT02)
#        jnz EXIT
        jmp EXIT

.ifdef IT8728_CONFIG_INDEX
  .ifdef IT8728_CONFIG_DATA
        # Please make sure MKF_SB_IO_DECODE_IN_SEC token is set to 1.
        # It is to enable the decoding of the I/O locations 2Eh and 2Fh to the LPC interface.
        # This range is used for Super I/O devices.

        # Enter Configuration Mode.
        movb   $0x87, %al
        out       %al, IT8728_CONFIG_INDEX   
        movb   $0x01, %al
        out       %al, IT8728_CONFIG_INDEX   
        movb   $0x55, %al
        out       %al, IT8728_CONFIG_INDEX           

    .if IT8728_CONFIG_INDEX EQ 2Eh
        movb  $0x55, %al
    .else
        movb  $0xAA, %al
    .endif
        out      %al, IT8728_CONFIG_INDEX

        # IT8728 Chip ID Byte 1: 87h
        movb   $0x20, %al
        out       %al, IT8728_CONFIG_INDEX     
        in         IT8728_CONFIG_DATA, %al
        cmp    $87h, %al
        jnz EXIT

        # IT8728 Chip ID Byte 2: 28h
        movb     $0x21, %al
        out         %al, IT8728_CONFIG_INDEX     
        in           IT8728_CONFIG_DATA, %al
        cmp      $28h, %al
        jnz EXIT
                
        # Parallel Port Configuration Registers (LDN=03h)
        movb    IT8728_LDN_SEL_REGISTER, %al
        out        %al, IT8728_CONFIG_INDEX
        movb    $0x03, %al
        out       %al IT8728_CONFIG_DATA,

        # Parallel Port Activate (Index=30h, Default=00h)
        # [7:1]:Reserved.
        # [0]:Parallel Port Enable - 1:Enable/0:Disable.
        movb   $0x30, %al
        out       %al, IT8728_CONFIG_INDEX
        movb   $0x00, %al
        out       %al, IT8728_CONFIG_DATA

        # Parallel Port Special Configuration Register (Index=F0h, Default=0Bh)
        # [7:6]:Port-80 Output Slection - 00:Normal Port-80/01:EC Index 29hReading/10:EC Index 2Ah Reading/11:EC Index 2Bh Reading.
        # [5:4]:Reserved.
        # [3]:Port-80 Function - 1:Disable/0:Enable.
        # [2]:IRQ Type - 1:IRQ sharing/0:Normal.
        # [1:0]:Parallel Port Modes - 00:SPP mode/01:EPP mode/10:ECP mode/11:EPP mode & ECP mode.
        movb  $0xf0, %al
        out      %al, IT8728_CONFIG_INDEX
        movb  $0x00,  %al
        out      %al, IT8728_CONFIG_DATA

        # Exit Configuration Mode.
        movb    $0x02,  %al 
        out        %al, IT8728_CONFIG_INDEX
        movb    $0x02,  %al
        out        %al, IT8728_CONFIG_DATA
  .endif
.endif

EXIT:
        pop %ebx
        pop %eax

RET_ESI
#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
#**                                                                     **
#**                       All Rights Reserved.                          **
#**                                                                     **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
#**                                                                     **
#**                       Phone: (770)-246-8600                         **
#**                                                                     **
#*************************************************************************
#*************************************************************************#
