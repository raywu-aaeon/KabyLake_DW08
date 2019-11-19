#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************

#<AMI_FHDR_START>
#**********************************************************************
#
# Name: Misc.asm
#
# Description:	
#
#**********************************************************************
#<AMI_FHDR_END>
#

.text

ASM_GLOBAL ASM_PFX(ClearDirectionFlag)
ASM_PFX(ClearDirectionFlag):
    cld
    ret

ASM_GLOBAL ASM_PFX(MpMtrrSynchUpEntry)
ASM_PFX(MpMtrrSynchUpEntry):
    #
    #Enter no fill cache mode, CD=1(Bit30), NW=0 (Bit29)
    #
    movq %cr0, %rax
    btsl $30, %eax
    btrl $29, %eax
    movq %rax, %cr0
    #
    # Flush cache
    #
    wbinvd
    #
    # Clear PGE flag Bit 7
    #
    movq %cr4, %rax
    movq %rax, %rdx
    btrl $7, %eax
    movq %rax, %cr4
    #
    # Flush all TLBs
    #
    movq %cr3, %rax
    movq %rax, %cr3
    
    movq %rdx, %rax
    
    ret

ASM_GLOBAL ASM_PFX(MpMtrrSynchUpExit)
ASM_PFX(MpMtrrSynchUpExit):
    #
    #Flush all TLBs the second time
    #
    movq %cr3, %rax
    movq %rax, %cr3
    #
    # Enable Normal Mode caching CD=NW=0, CD(Bit30), NW(Bit29)
    #
    movq %cr0, %rax
    btrl $29, %eax
    btrl $30, %eax
    movq %rax, %cr0
    #
    # Set PGE Flag in CR4 if set
    #
    movq %rcx, %cr4
    ret

#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************
