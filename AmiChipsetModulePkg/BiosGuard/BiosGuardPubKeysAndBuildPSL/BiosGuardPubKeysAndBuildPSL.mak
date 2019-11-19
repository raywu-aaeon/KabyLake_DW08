#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1985-2018, American Megatrends, Inc.            **
#**                                                                     **
#**                       All Rights Reserved.                          **
#**                                                                     **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
#**                                                                     **
#**                       Phone: (770)-246-8600                         **
#**                                                                     **
#*************************************************************************
#*************************************************************************
#<AMI_FHDR_START>
#
# Name: BiosGuardPubKey.mak
#
# Description: Make for BiosGuardPubKey
#
#<AMI_FHDR_END>
#*************************************************************************
ifeq ($(BUILD_OS),$(BUILD_OS_LINUX)) # Below is for developer environment in Linux OS
ifeq ($(wildcard $(BIOS_GUARD_PRIKEY)),$(BIOS_GUARD_PRIKEY))
Prepare: \
IMAGE_SIZE_CHECK\
BIOS_GUARD_GENERATE_KEYS \
$(BUILD_DIR)/BiosGuardPubKeyhash_FdfFileStatement.txt \
$(BUILD_DIR)/BiosMap.sh \
$(BUILD_DIR)/BuildPSL.sh \
$(BUILD_DIR)/BuildBiosGuardVariable.sh

IS_BIOS_FLASH_SIZE_ALIGN_1MB:
ifeq ($(BiosGuard_SUPPORT), 1)
ifeq ($(IS_BIOS_FLASH_SIZE_ALIGNMENT1MB), 0)
	$(error FLASH_SIZE must to align 1MB for Bios Guard, this is limitation of CPU RC in the BiosGuardService.c)
endif
endif

IMAGE_SIZE_CHECK:
ifeq ($(BIOS_GUARD_MEUD_SUPPORT), 1)
ifeq ($(BIOSGUARD_RECOVERY_IMAGE_SIZE), $(BIOSGUARD_MEUD_RECOVERY_IMAGE_SIZE))
	$(error BIOSGUARD_RECOVERY_IMAGE_SIZE cannot equal to BIOSGUARD_MEUD_RECOVERY_IMAGE_SIZE. Please change FV_MEFW_CAPSULE_SIZE.)
endif
endif

BIOS_GUARD_PRIVATE_KEY = $(BIOS_GUARD_PRIKEY)
BIOS_GUARD_PUBLIC_KEY = $(BIOS_GUARD_PUBKEY)
BIOS_GUARD_VKEY_EXP = $(BIOS_GUARD_PUB_PATH)/.BiosGuardVkeyExp
BIOS_GUARD_PUBLIC_KEY_HASH = $(BUILD_DIR)/BiosGuardPubKeyhash.bin

BIOS_GUARD_PUB_KEY_FFS_FILE_GUID = 8E295870-D377-4b75-BFDC-9AE2F6DBDE22

AMI_BIOS_GUARD_RELATED_FILE = $(wildcard AMI_BIOS_GUARD_*)

BIOS_GUARD_GENERATE_KEYS:
	$(RM) $(AMI_BIOS_GUARD_RELATED_FILE)
ifneq ("$(wildcard $(BIOS_GUARD_PUBLIC_KEY_HASH))","")  
	$(RM) $(BIOS_GUARD_PUBLIC_KEY_HASH)
endif
	wine $(BIOS_GUARD_CRYPTOCON) -z -w -k $(BIOS_GUARD_PRIVATE_KEY) -f $(BIOS_GUARD_PUB_PATH)/dummy.bin -s
	cat .out_key $(BIOS_GUARD_VKEY_EXP) > $(BIOS_GUARD_PUBLIC_KEY_HASH)
ifneq ("$(wildcard .out_key)","")  
	 $(RM) .out_key
endif
ifneq ("$(wildcard .out_PfatCer)","")  
	 $(RM) .out_PfatCer
endif
	wine $(BIOS_GUARD_CRYPTOCON) -h2 -f $(BIOS_GUARD_PUBLIC_KEY_HASH) -o $(BIOS_GUARD_PUBLIC_KEY_HASH)
	
$(BUILD_DIR)/BiosGuardPubKeyhash_FdfFileStatement.txt: $(BIOS_GUARD_PUBLIC_KEY_HASH)
	$(ECHO)  \
        FILE FREEFORM = $(BIOS_GUARD_PUB_KEY_FFS_FILE_GUID) {\$(EOL)\
        SECTION RAW = $< \$(EOL)}> $@
	
$(BUILD_DIR)/BiosMap.sh: $(BUILD_DIR)/Token.mak
	$(ECHO) "$(BUILD_BIOS_MAP_SET)" > $@
	chmod +x $@
	$@
	
$(BUILD_DIR)/BuildPSL.sh: $(BUILD_DIR)/Token.mak
	$(ECHO) "$(BUILD_PSL_SET)" > $@
	chmod +x $@
	$@
	
$(BUILD_DIR)/BuildBiosGuardVariable.sh: $(BUILD_DIR)/Token.mak
	$(ECHO) "$(BUILD_BIOS_GUARD_VARIABLE_SET)" > $@
	chmod +x $@
	$@

else # else - ifeq ($(wildcard $(BIOS_GUARD_PRIKEY)),$(BIOS_GUARD_PRIKEY))
Prepare: \
BIOS_GUARD_GENERATE_DUMMY_FILE\
$(BUILD_DIR)/BiosGuardPubKeyhash_FdfFileStatement.txt\

BIOS_GUARD_PRIVATE_KEY = $(BIOS_GUARD_PRIKEY)
BIOS_GUARD_PUBLIC_KEY = $(BIOS_GUARD_PUBKEY)
BIOS_GUARD_VKEY_EXP = $(BIOS_GUARD_PUB_PATH)/.BiosGuardVkeyExp
BIOS_GUARD_PUBLIC_KEY_HASH = $(BUILD_DIR)/BiosGuardPubKeyhash.bin

BIOS_GUARD_PUB_KEY_FFS_FILE_GUID = 8E295870-D377-4b75-BFDC-9AE2F6DBDE22

BIOS_GUARD_GENERATE_DUMMY_FILE:
ifneq ("$(wildcard $(BIOS_GUARD_PUBLIC_KEY_HASH))","")
	$(RM) $(BIOS_GUARD_PUBLIC_KEY_HASH)
endif
	$(CP) $(BIOS_GUARD_VKEY_EXP)  $(BIOS_GUARD_PUBLIC_KEY_HASH)
	@$(ECHO) "$(EOL)WARNING!!! Missing RSA private key '$(BIOS_GUARD_PRIKEY)' to sign BIOS image, so BIOS Guard binary don't be generated"

$(BUILD_DIR)/BiosGuardPubKeyhash_FdfFileStatement.txt: $(BIOS_GUARD_PUBLIC_KEY_HASH)
	$(ECHO)  \
        FILE FREEFORM = $(BIOS_GUARD_PUB_KEY_FFS_FILE_GUID) {\$(EOL)\
        SECTION RAW = $< \$(EOL)}> $@

endif # end - ifeq ($(wildcard $(BIOS_GUARD_PRIKEY)),$(BIOS_GUARD_PRIKEY))

else # Below is for developer environment in Windows OS
ifeq ($(wildcard $(BIOS_GUARD_PRIKEY)),$(BIOS_GUARD_PRIKEY))
Prepare: \
IMAGE_SIZE_CHECK\
BIOS_GUARD_GENERATE_KEYS\
$(BUILD_DIR)/BiosGuardPubKeyhash_FdfFileStatement.txt\
$(BUILD_DIR)/BiosMap.bat\
$(BUILD_DIR)/BuildPSL.bat\
$(BUILD_DIR)/BuildBiosGuardVariable.bat

IS_BIOS_FLASH_SIZE_ALIGN_1MB:
ifeq ($(BiosGuard_SUPPORT), 1)
ifeq ($(IS_BIOS_FLASH_SIZE_ALIGNMENT1MB), 0)
	$(error FLASH_SIZE must to align 1MB for Bios Guard, this is limitation of CPU RC in the BiosGuardService.c)
endif
endif

IMAGE_SIZE_CHECK:
ifeq ($(BIOS_GUARD_MEUD_SUPPORT), 1)
ifeq ($(BIOSGUARD_RECOVERY_IMAGE_SIZE), $(BIOSGUARD_MEUD_RECOVERY_IMAGE_SIZE))
	$(error BIOSGUARD_RECOVERY_IMAGE_SIZE cannot equal to BIOSGUARD_MEUD_RECOVERY_IMAGE_SIZE. Please change FV_MEFW_CAPSULE_SIZE.)
endif
endif

BIOS_GUARD_PRIVATE_KEY = $(BIOS_GUARD_PRIKEY)
BIOS_GUARD_PUBLIC_KEY = $(BIOS_GUARD_PUBKEY)
BIOS_GUARD_VKEY_EXP = $(BIOS_GUARD_PUB_PATH)\.BiosGuardVkeyExp
BIOS_GUARD_PUBLIC_KEY_HASH = $(BUILD_DIR)\BiosGuardPubKeyhash.bin

BIOS_GUARD_PUB_KEY_FFS_FILE_GUID = 8E295870-D377-4b75-BFDC-9AE2F6DBDE22

BIOS_GUARD_GENERATE_KEYS:
	if exist AMI_BIOS_GUARD_*.* $(RM) AMI_BIOS_GUARD_*.*
	if exist $(BIOS_GUARD_PUBLIC_KEY_HASH) del $(BIOS_GUARD_PUBLIC_KEY_HASH)
	$(BIOS_GUARD_CRYPTOCON) -z -w -k $(BIOS_GUARD_PRIVATE_KEY) -f $(BIOS_GUARD_PUB_PATH)/dummy.bin -s
	copy /b .out_key + $(BIOS_GUARD_VKEY_EXP) $(BIOS_GUARD_PUBLIC_KEY_HASH)
	if exist .out_key del .out_key
	if exist .out_PfatCer del .out_PfatCer
	$(BIOS_GUARD_CRYPTOCON) -h2 -f $(BIOS_GUARD_PUBLIC_KEY_HASH) -o $(BIOS_GUARD_PUBLIC_KEY_HASH)

$(BUILD_DIR)/BiosGuardPubKeyhash_FdfFileStatement.txt: $(BIOS_GUARD_PUBLIC_KEY_HASH)
	$(ECHO)  \
	FILE FREEFORM = $(BIOS_GUARD_PUB_KEY_FFS_FILE_GUID) {$(EOL)\
	SECTION RAW = $< $(EOL)}> $@

$(BUILD_DIR)/BiosMap.bat: $(BUILD_DIR)/Token.mak
	$(ECHO) "$(BUILD_BIOS_MAP_SET)" > $@
	call $@

$(BUILD_DIR)/BuildPSL.bat: $(BUILD_DIR)/Token.mak
	$(ECHO) "$(BUILD_PSL_SET)" > $@
	call $@
	
$(BUILD_DIR)/BuildBiosGuardVariable.bat: $(BUILD_DIR)/Token.mak
	$(ECHO) "$(BUILD_BIOS_GUARD_VARIABLE_SET)" > $@
	call $@	

else # else - ifeq ($(wildcard $(BIOS_GUARD_PRIKEY)),$(BIOS_GUARD_PRIKEY))
Prepare: \
BIOS_GUARD_GENERATE_DUMMY_FILE\
$(BUILD_DIR)/BiosGuardPubKeyhash_FdfFileStatement.txt\

BIOS_GUARD_PRIVATE_KEY = $(BIOS_GUARD_PRIKEY)
BIOS_GUARD_PUBLIC_KEY = $(BIOS_GUARD_PUBKEY)
BIOS_GUARD_VKEY_EXP = $(BIOS_GUARD_PUB_PATH)\.BiosGuardVkeyExp
BIOS_GUARD_PUBLIC_KEY_HASH = $(BUILD_DIR)\BiosGuardPubKeyhash.bin

BIOS_GUARD_PUB_KEY_FFS_FILE_GUID = 8E295870-D377-4b75-BFDC-9AE2F6DBDE22

BIOS_GUARD_GENERATE_DUMMY_FILE:
	if exist $(BIOS_GUARD_PUBLIC_KEY_HASH) del $(BIOS_GUARD_PUBLIC_KEY_HASH)
	copy $(BIOS_GUARD_VKEY_EXP) $(BIOS_GUARD_PUBLIC_KEY_HASH)
	@$(ECHO) "$(EOL)WARNING!!! Missing RSA private key '$(BIOS_GUARD_PRIKEY)' to sign BIOS image, so BIOS Guard binary don't be generated"

$(BUILD_DIR)/BiosGuardPubKeyhash_FdfFileStatement.txt: $(BIOS_GUARD_PUBLIC_KEY_HASH)
	$(ECHO)  \
	FILE FREEFORM = $(BIOS_GUARD_PUB_KEY_FFS_FILE_GUID) {$(EOL)\
	SECTION RAW = $< $(EOL)}> $@
endif # end - ifeq ($(wildcard $(BIOS_GUARD_PRIKEY)),$(BIOS_GUARD_PRIKEY))
	
endif

#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1985-2018, American Megatrends, Inc.            **
#**                                                                     **
#**                       All Rights Reserved.                          **
#**                                                                     **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
#**                                                                     **
#**                       Phone: (770)-246-8600                         **
#**                                                                     **
#*************************************************************************
#*************************************************************************
