## @file
#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
#**                                                                     **
#**                       All Rights Reserved.                          **
#**                                                                     **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
#**                                                                     **
#**                       Phone: (770)-246-8600                         **
#**                                                                     **
#*************************************************************************
#*************************************************************************

#*************************************************************************
# $Header: /Alaska/BIN/Chipset/Intel/NorthBridge/SandyBridge_A/Board/NBSECInit.asm 1     2/24/10 10:38p Tonywu $
#
# $Revision: 1 $
#
# $Date: 2/24/10 10:38p $
#*************************************************************************
#<AMI_FHDR_START>
#
# Name:         NBSECInit.ASM
#
# Description:  Program any workaround or initialization needed before
#               enabling Cache as memory in the SEC stage
#
#<AMI_FHDR_END>
#*************************************************************************
##
#----------------------------------------------------------------------------
        #include <Token.h>
#----------------------------------------------------------------------------

# Externs

# Define the equates here
.equ     NB_BUS,                                0x00
.equ     NB_DEV_FUN,                       0x00

.equ     MSR_IA32_PLATFORM_ID,    0x0017

.macro    RET_ESI
  movd    %mm7, %esi                     # restore ESP from MM7
  jmp     *%esi
.endm

#<AMI_PHDR_START>
#----------------------------------------------------------------------------
#
# Procedure:    SECNB_EarlyInit
#
# Description:  This routine initializes north bridge for PEI preparation
#
# Input:        ESP BIST Info
#               EBP Reset ID (EDX at reset)
#               Stack not available
#
# Output:       None
#
# Modified:     All, except EBP and ESP
#
# Notes:        None.
#
#----------------------------------------------------------------------------
#<AMI_PHDR_END>
ASM_GLOBAL ASM_PFX(SECNB_EarlyInit)
ASM_PFX(SECNB_EarlyInit):

        movb   $0x03, %al
        out       %al, $0x80 

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
#*************************************************************************
