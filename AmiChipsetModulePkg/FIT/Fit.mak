#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1987-2014, American Megatrends, Inc.            **
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
# Name: Fit.mak
#
# Description: This make file Updates the final ROM file
#
#<AMI_FHDR_END>
#**********************************************************************

Prepare: IS_FV_BB_SIZE_VALID

IS_FV_BB_SIZE_VALID:
ifeq ($(INTEL_BOOT_GUARD_SUPPORT), 1)
ifeq ($(BPM_LLC_SIZE_TOO_SMALL_FLAG), 1)
	$(error !ERROR Size of Hashed IBB(FV_BB_SIZE) > LLC -  0.5MB - sizeof (Boot Guard ACM) -  sizeof (DataStack) - sizeof (FIT) - sizeof (BPM))
endif
ifeq ($(BPM_NEM_DATA_STACK_SIZE), 0)
	$(error !ERROR BPM_NEM_DATA_STACK_SIZE is 0. Please check BPM_MIN_CPU_LLC_SIZE or enable FV_BB_VERIFY_FV_AFTER_MEMORY)
endif
endif

End : Fit

.PHONY : Fit

Fit:
	$(ECHO) Inside Fit.mak

#########
#########
######### Input BOOT_GUARD_BPM_KEY_FILENAME/BOOT_GUARD_KM_KEY_FILENAME
#########
#########
ifeq ($(wildcard $(BOOT_GUARD_BPM_KEY_FILENAME)),$(BOOT_GUARD_BPM_KEY_FILENAME))
ifeq ($(wildcard $(BOOT_GUARD_KM_KEY_FILENAME)),$(BOOT_GUARD_KM_KEY_FILENAME))
ifeq ($(INTEL_BOOT_GUARD_SUPPORT), 1)
######
###### Disable BOOT_GUARD_SIGNING_INTERLOCK_SUPPORT
######
ifeq ($(BOOT_GUARD_SIGNING_INTERLOCK_SUPPORT), 0)
###
### Enable INTEL_BOOT_GUARD_SAVE_DATA_ON_THE_ROM_SUPPORT
###
ifeq ($(INTEL_BOOT_GUARD_SAVE_DATA_ON_THE_ROM_SUPPORT), 1)
ifeq ($(BUILD_OS), $(BUILD_OS_LINUX))
	wine $(FIT_DIR)/BpmKmGen.exe -SBPMKM $(FitFirstEndPatchBIOSRomPath) $(FIT_DIR) $(BOOT_GUARD_BPM_KEY_FILENAME) $(BOOT_GUARD_KM_KEY_FILENAME)
else
	$(FIT_DIR)\BpmKmGen.exe -SBPMKM $(FitFirstEndPatchBIOSRomPath) $(FIT_DIR) $(BOOT_GUARD_BPM_KEY_FILENAME) $(BOOT_GUARD_KM_KEY_FILENAME)
endif
else
###
### Disable INTEL_BOOT_GUARD_SAVE_DATA_ON_THE_ROM_SUPPORT
### 
ifeq ($(BUILD_OS), $(BUILD_OS_LINUX))
	wine $(FIT_DIR)/BpmKmGen.exe -PBPMKM $(FitFirstEndPatchBIOSRomPath) $(FIT_DIR) $(BOOT_GUARD_BPM_KEY_FILENAME) $(BOOT_GUARD_KM_KEY_FILENAME) $(BootGuardTokens)
else
	$(FIT_DIR)\BpmKmGen.exe -PBPMKM $(FitFirstEndPatchBIOSRomPath) $(FIT_DIR) $(BOOT_GUARD_BPM_KEY_FILENAME) $(BOOT_GUARD_KM_KEY_FILENAME) $(BootGuardTokens)
endif
endif
else
######
###### Enable BOOT_GUARD_SIGNING_INTERLOCK_SUPPORT
######
ifeq ($(INTEL_BOOT_GUARD_SAVE_DATA_ON_THE_ROM_SUPPORT), 1)
###
### Enable INTEL_BOOT_GUARD_SAVE_DATA_ON_THE_ROM_SUPPORT
###
ifeq ($(BUILD_OS), $(BUILD_OS_LINUX))
	wine $(FIT_DIR)/BpmKmGen.exe -SBPMKMSIGN $(FitFirstEndPatchBIOSRomPath) $(FIT_DIR) $(BOOT_GUARD_BPM_KEY_FILENAME) $(BOOT_GUARD_KM_KEY_FILENAME) $(BootGuardSignInterLockTokens)
else
	$(FIT_DIR)\BpmKmGen.exe -SBPMKMSIGN $(FitFirstEndPatchBIOSRomPath) $(FIT_DIR) $(BOOT_GUARD_BPM_KEY_FILENAME) $(BOOT_GUARD_KM_KEY_FILENAME) $(BootGuardSignInterLockTokens)
endif
else
###
### Disable INTEL_BOOT_GUARD_SAVE_DATA_ON_THE_ROM_SUPPORT
###
ifeq ($(BUILD_OS), $(BUILD_OS_LINUX))
	wine $(FIT_DIR)/BpmKmGen.exe -PBPMKMSIGN $(FitFirstEndPatchBIOSRomPath) $(FIT_DIR) $(BOOT_GUARD_BPM_KEY_FILENAME) $(BOOT_GUARD_KM_KEY_FILENAME) $(BootGuardTokens) $(BootGuardSignInterLockTokens)
else
	$(FIT_DIR)\BpmKmGen.exe -PBPMKMSIGN $(FitFirstEndPatchBIOSRomPath) $(FIT_DIR) $(BOOT_GUARD_BPM_KEY_FILENAME) $(BOOT_GUARD_KM_KEY_FILENAME) $(BootGuardTokens) $(BootGuardSignInterLockTokens)
endif
endif
endif
endif
endif
endif
#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1987-2014, American Megatrends, Inc.            **
#**                                                                     **
#**                       All Rights Reserved.                          **
#**                                                                     **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30071           **
#**                                                                     **
#**                       Phone: (770)-246-8600                         **
#**                                                                     **
#*************************************************************************
#*************************************************************************