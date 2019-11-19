#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1987-2018, American Megatrends, Inc.            **
#**                                                                     **
#**                       All Rights Reserved.                          **
#**                                                                     **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30071           **
#**                                                                     **
#**                       Phone: (770)-246-8600                         **
#**                                                                     **
#*************************************************************************
#*************************************************************************

#**********************************************************************
#<AMI_FHDR_START>
#
# Name: FitFirstEnd.mak
#
# Description: This make file Updates the final ROM file
#
#<AMI_FHDR_END>
#**********************************************************************

RomPatch : FitFirstEnd

.PHONY : FitFirstEnd

FitFirstEnd:
	$(ECHO) Inside FitFirstEnd.mak
ifeq ($(BUILD_OS), $(BUILD_OS_LINUX))
	chmod +x $(FIT_DIR)/FITUtil
	$(FIT_DIR)/FITUtil $(FitFirstEndPatchBIOSRomPath) $(FITEntryPointToOtherFVBBRomAddress)
else
	$(FIT_DIR)/FITUtil.exe $(FitFirstEndPatchBIOSRomPath) $(FITEntryPointToOtherFVBBRomAddress)
endif

ifeq ($(wildcard $(BOOT_GUARD_BPM_KEY_FILENAME)),$(BOOT_GUARD_BPM_KEY_FILENAME))
ifeq ($(wildcard $(BOOT_GUARD_KM_KEY_FILENAME)),$(BOOT_GUARD_KM_KEY_FILENAME))
ifeq ($(INTEL_BOOT_GUARD_SUPPORT), 1)

ifeq ($(BUILD_OS), $(BUILD_OS_LINUX))
	wine $(FIT_DIR)/BpmKmGen.exe -SRDATA $(FitFirstEndPatchBIOSRomPath) $(BootGuardSaveTokenValue) $(BootGuardSignInterLockTokens)
else
	$(FIT_DIR)/BpmKmGen.exe -SRDATA $(FitFirstEndPatchBIOSRomPath) $(BootGuardSaveTokenValue) $(BootGuardSignInterLockTokens)
endif

ifeq ($(INTEL_BOOT_GUARD_CHAIN_OF_TRUST_SUPPORT), 1)
ifeq ($(BUILD_OS), $(BUILD_OS_LINUX))
	wine $(FIT_DIR)/BpmKmGen.exe -PFVMAIN $(FitFirstEndPatchBIOSRomPath) $(FIT_DIR) $(BIOS_STARTING_ADDRESS) $(DXE_SEGMENT_BASE) $(DXE_SEGMENT_SIZE) $(SUPPORT_BUILD_TIME_FV_BB_OFFSET)
else
	$(FIT_DIR)/BpmKmGen.exe -PFVMAIN $(FitFirstEndPatchBIOSRomPath) $(FIT_DIR) $(BIOS_STARTING_ADDRESS) $(DXE_SEGMENT_BASE) $(DXE_SEGMENT_SIZE) $(SUPPORT_BUILD_TIME_FV_BB_OFFSET)
endif
endif

ifeq ($(FV_BB_VERIFY_FV_AFTER_MEMORY), 1)
ifeq ($(FV_BB_VERIFY_FV_AFTER_MEMORY_SUPPORT_TOPSWAP), 1)
ifeq ($(INPUT_FV_AFTER_MEMORY_BACKUP_BINARY), 1)
ifeq ($(BUILD_OS), $(BUILD_OS_LINUX))
	wine $(FIT_DIR)/BpmKmGen.exe -PFVBBAMSWB $(FitFirstEndPatchBIOSRomPath) $(FIT_DIR) $(BOOT_GUARD_FV_AM_KEY_FILENAME) $(BIOS_STARTING_ADDRESS) $(FV_BB_AFTER_MEMORY_SEGMENT_BASE) $(FV_BB_AFTER_MEMORY_SEGMENT_SIZE) $(FV_AFTER_MEMORY_BACKUP_ADDRESS) $(INPUT_FV_AFTER_MEMORY_BACKUP_BINARY_LOCATION)
else
	$(FIT_DIR)/BpmKmGen.exe -PFVBBAMSWB $(FitFirstEndPatchBIOSRomPath) $(FIT_DIR) $(BOOT_GUARD_FV_AM_KEY_FILENAME) $(BIOS_STARTING_ADDRESS) $(FV_BB_AFTER_MEMORY_SEGMENT_BASE) $(FV_BB_AFTER_MEMORY_SEGMENT_SIZE) $(FV_AFTER_MEMORY_BACKUP_ADDRESS) $(INPUT_FV_AFTER_MEMORY_BACKUP_BINARY_LOCATION)
endif
else
ifeq ($(BUILD_OS), $(BUILD_OS_LINUX))
	wine $(FIT_DIR)/BpmKmGen.exe -PFVBBAMSW $(FitFirstEndPatchBIOSRomPath) $(FIT_DIR) $(BOOT_GUARD_FV_AM_KEY_FILENAME) $(BIOS_STARTING_ADDRESS) $(FV_BB_AFTER_MEMORY_SEGMENT_BASE) $(FV_BB_AFTER_MEMORY_SEGMENT_SIZE) $(FV_AFTER_MEMORY_BACKUP_ADDRESS) $(SUPPORT_BUILD_TIME_FV_BB_OFFSET)
else
	$(FIT_DIR)/BpmKmGen.exe -PFVBBAMSW $(FitFirstEndPatchBIOSRomPath) $(FIT_DIR) $(BOOT_GUARD_FV_AM_KEY_FILENAME) $(BIOS_STARTING_ADDRESS) $(FV_BB_AFTER_MEMORY_SEGMENT_BASE) $(FV_BB_AFTER_MEMORY_SEGMENT_SIZE) $(FV_AFTER_MEMORY_BACKUP_ADDRESS) $(SUPPORT_BUILD_TIME_FV_BB_OFFSET)
endif
endif	
else
ifeq ($(BUILD_OS), $(BUILD_OS_LINUX))
	wine $(FIT_DIR)/BpmKmGen.exe -PFVBBAM $(FitFirstEndPatchBIOSRomPath) $(FIT_DIR) $(BOOT_GUARD_FV_AM_KEY_FILENAME) $(BIOS_STARTING_ADDRESS) $(FV_BB_AFTER_MEMORY_SEGMENT_BASE) $(FV_BB_AFTER_MEMORY_SEGMENT_SIZE)
else
	$(FIT_DIR)/BpmKmGen.exe -PFVBBAM $(FitFirstEndPatchBIOSRomPath) $(FIT_DIR) $(BOOT_GUARD_FV_AM_KEY_FILENAME) $(BIOS_STARTING_ADDRESS) $(FV_BB_AFTER_MEMORY_SEGMENT_BASE) $(FV_BB_AFTER_MEMORY_SEGMENT_SIZE)
endif
endif
endif

endif
endif
endif
#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1987-2018, American Megatrends, Inc.            **
#**                                                                     **
#**                       All Rights Reserved.                          **
#**                                                                     **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30071           **
#**                                                                     **
#**                       Phone: (770)-246-8600                         **
#**                                                                     **
#*************************************************************************
#*************************************************************************
