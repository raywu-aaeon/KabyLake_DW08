#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1985-2011, American Megatrends, Inc.            **
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
# $Header: $
#
# $Revision: $
#
# $Date: $
#*************************************************************************
#<AMI_FHDR_START>
#
# Name:         Crb.mak
#
# Description:
#
#<AMI_FHDR_END>
#*************************************************************************
.PHONY : CopyRomFile

AMI_CSP_LIB_INCLUDE_FILES := $(CRB_DIR)/Include/Library/CrbLib.h  \
$(AMI_CSP_LIB_INCLUDE_FILES)

Prepare : DelROMFiles CreateDecFile

#EDK1 varaibles
ECP_SOURCE=$(CURDIR)\EdkCompatibilityPkg
EFI_SOURCE=$(CURDIR)
EDK_SOURCE=$(CURDIR)\EdkCompatibilityPkg
export ECP_SOURCE EFI_SOURCE EDK_SOURCE

ifeq ($(BUILD_OS), $(BUILD_OS_LINUX))
DelROMFiles:
    ifneq ("$(wildcard  $(PROJECT_TAG)*.cap)", "")
		$(RM)  $(PROJECT_TAG)*.cap
    endif
    ifneq ("$(wildcard  $(PROJECT_TAG).bin)", "")
		$(RM)  $(PROJECT_TAG).bin
    endif
    ifneq ("$(wildcard  $(PROJECT_TAG)*.rom)", "")
		$(RM)  $(PROJECT_TAG)*.rom
    endif        
    ifneq ("$(wildcard  AMIBIOS.rom)", "")
		$(RM)  AMIBIOS.rom
    endif
    ifneq ("$(wildcard  $(PLATFORM_ASL_COMPILER))", "")
	    chmod +x $(PLATFORM_ASL_COMPILER)
    endif
else
DelROMFiles:
    ifneq ("$(wildcard  $(PROJECT_TAG)*.cap)", "")
		@del /q $(PROJECT_TAG)*.cap
    endif
    ifneq ("$(wildcard  $(PROJECT_TAG).bin)", "")
		@del /q $(PROJECT_TAG).bin
    endif
    ifneq ("$(wildcard  $(PROJECT_TAG)*.rom)", "")
		@del /q $(PROJECT_TAG)*.rom
    endif        
    ifneq ("$(wildcard  AMIBIOS.rom)", "")
		@del /q AMIBIOS.rom
    endif
endif
    
CreateDecFile: 
	$(ECHO) \
	"{$(EOL)\
	gsub(/DEFINE REF_CODE_PATH = ./,\"DEFINE REF_CODE_PATH = ../$(PLATFORM_RC_PACKAGE)\",$(DOUBLEDOLLAR)0)$(EOL)\
	print $(DOUBLEDOLLAR)0$(EOL)\
	}"$(EOL)\
> $(BUILD_DIR)/CrbPatchfile.txt
	$(GAWK) -f $(BUILD_DIR)/CrbPatchfile.txt $(CHIPSET_REF_DEC_PATH) > $(BUILD_DIR)$(PATH_SLASH)$(CHIPSET_REF_DEC_NAME)
   
End : CopyRomFile

CopyRomFile:
ifeq ($(SecureMod_SUPPORT), 1)
    ifeq ($(wildcard $(FWpriv)), $(FWpriv))
		$(ECHO) "----- Copy Signed BIOS Image file to AMIBIOS.rom for RomImage."
		$(CP) $(FWCAPSULE_FILE_NAME) AMIBIOS.rom
    else
		@$(ECHO) "----- Copy Unsigned BIOS Image file to AMIBIOS.rom for RomImage."
		$(CP) $(ROM_FILE_NAME) $(AMI_ROM)
		$(CP) $(AMI_ROM) AMIBIOS.rom
    endif
else
	@$(ECHO) "- ----- Copy Unsigned BIOS Image file to AMIBIOS.rom for RomImage."
	$(CP) $(ROM_FILE_NAME) $(AMI_ROM)
	$(CP) $(AMI_ROM) AMIBIOS.rom	
endif
ifeq ($(ME_ROM_IMAGE_BUILD_SUPPORT), 1)    
	$(CP) AMIBIOS.rom $(BUILD_DIR)$(PATH_SLASH)AMIBIOS.rom
	@$(RM) AMIBIOS.rom
endif
	
ifeq ($(ME_ROM_IMAGE_BUILD_SUPPORT), 1)
End : BUILD_ME
endif

#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2011, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************
