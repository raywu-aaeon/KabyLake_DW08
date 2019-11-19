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

Prepare : $(BUILD_DIR)/CpuPeiPolicyInitElink.h $(BUILD_DIR)/CpuPeiPreMemPolicyInitElink.h

$(BUILD_DIR)/CpuPeiPreMemPolicyInitElink.h : $(BUILD_DIR)$(PATH_SLASH)Token.mak
	$(ECHO) \
"#define PEI_PRE_MEM_CPU_POLICY_INIT_LIST $(PeiPreMemCpuPolicyInitList)$(EOL)"\
> $@

$(BUILD_DIR)/CpuPeiPolicyInitElink.h : $(BUILD_DIR)$(PATH_SLASH)Token.mak
	$(ECHO) \
"#define PEI_CPU_POLICY_INIT_LIST $(PeiCpuPolicyInitList)$(EOL)"\
> $@

#$(BUILD_DIR)/CpuDxePolicyInitElink.h :  $(BUILD_DIR)$(PATH_SLASH)token.mak
#	$(ECHO) \
#"#define DXE_CPU_POLICY_INIT_LIST $(DxeCpuPolicyInitList)$(EOL)"\
#> $@

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
