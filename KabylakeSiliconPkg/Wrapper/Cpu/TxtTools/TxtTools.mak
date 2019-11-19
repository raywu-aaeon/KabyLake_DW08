#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
#**                                                                     **
#**                       All Rights Reserved.                          **
#**                                                                     **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
#**                                                                     **
#**                       Phone: (770)-246-8600                         **
#**                                                                     **
#*************************************************************************
#*************************************************************************
End : Txtfixup
.PHONY : Txtfixup

Txtfixup:
	$(ECHO) Inside Txttools.mak
	$(INTEL_CLIENT_COMMON_PKG_TOOLS_DIR)\Stafixup\Stafixup.exe $(AP_GUID) $(BUILD_DIR)$(PATH_SLASH)$(PLATFORM_NAME)$(PATH_SLASH)$(TARGET)_$(TOOL_CHAIN_TAG)$(PATH_SLASH)FV$(PATH_SLASH)AMIROM.fd  $(AP_FIXUP_FILE)  $(AP_OFFSET)

#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
#**                                                                     **
#**                       All Rights Reserved.                          **
#**                                                                     **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
#**                                                                     **
#**                       Phone: (770)-246-8600                         **
#**                                                                     **
#*************************************************************************
#*************************************************************************	