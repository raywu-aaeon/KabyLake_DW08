## @file
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
##
#*************************************************************************
#
# $Revision: 4 $
#
# $Date: 6/16/09 3:01a $
#*************************************************************************
# Revision History
# 
#*************************************************************************
#<AMI_FHDR_START>
#
# Name:         CrbSec.S
#
# Description:  Program any workaround or initialization needed before
#               enabling Cache as memory in the SEC stage for CRB.
#
#<AMI_FHDR_END>
#*************************************************************************

#----------------------------------------------------------------------------
       #include <Token.h>
#----------------------------------------------------------------------------

# Externs

# Define the equates here

.macro    RET_ESI
  movd    %mm7, %esi                     # restore ESP from MM7
  jmp     *%esi
.endm

#<AMI_PHDR_START>
#----------------------------------------------------------------------------
#
# Procedure:    SECCRB_EarlyInit
#
# Description:  This routine initializes CRB for PEI preparation
#
# Input:        ESP BIST Info
#               EBP Reset ID (EDX at reset)
#               Stack not available
#
# Output:       None
#
# Modified:     All, except EBP and ESP
#
# Notes:        NBSEC & SBSEC initialization is done at this point.
#----------------------------------------------------------------------------
#<AMI_PHDR_END>
ASM_GLOBAL ASM_PFX(SECCRB_EarlyInit)
ASM_PFX(SECCRB_EarlyInit):

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

