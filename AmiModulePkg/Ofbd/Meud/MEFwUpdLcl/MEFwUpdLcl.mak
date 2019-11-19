#***********************************************************************
#*                                                                     *
#*   Copyright (c) 1985-2019, American Megatrends International LLC.   *
#*                                                                     *
#*      All rights reserved. Subject to AMI licensing agreement.       *
#*                                                                     *
#***********************************************************************

Prepare : $(BUILD_DIR)$(PATH_SLASH)MeFwUpdLcl.inf

$(BUILD_DIR)$(PATH_SLASH)MeFwUpdLcl.inf : $(BUILD_DIR)$(PATH_SLASH)Token.h $(MEFwUpdLcl_DIR)$(PATH_SLASH)MEFwUpdLcl.mak 
	$(ECHO)  \
"$(EOL)\
$(EOL)\
[Defines]$(EOL)\
  INF_VERSION                    = 0x00010005$(EOL)\
  BASE_NAME                      = MeFwUpdLcl$(EOL)\
  FILE_GUID                      = A11585B7-8FA2-4F1C-AA6F-DD6309469613$(EOL)\
  MODULE_TYPE                    = DXE_DRIVER$(EOL)\
  VERSION_STRING                 = 1.0$(EOL)\
  PI_SPECIFICATION_VERSION       = 0x0001000A$(EOL)\
  ENTRY_POINT                    = MeFwUpdLclEntry$(EOL)\
$(EOL)\
#$(EOL)\
# The following information is for reference only and not required by the build tools.$(EOL)\
#$(EOL)\
#  VALID_ARCHITECTURES           = IA32 X64 IPF EBC$(EOL)\
#$(EOL)\
$(EOL)\
[Sources]$(EOL)\
  ../$(MEFwUpdLcl_DIR)/MEFwUpdLcl.c$(EOL)\
$(EOL)\
[Packages] $(EOL)\
  MdePkg/MdePkg.dec$(EOL)\
  AmiCompatibilityPkg/AmiCompatibilityPkg.dec$(EOL)\
  AmiModulePkg/AmiModulePkg.dec$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(MEFwUpdLcLPackages))$(EOL)\
  $(EOL)\
[LibraryClasses]$(EOL)\
  AmiDxeLib$(EOL)\
  UefiDriverEntryPoint$(EOL)\
  MemoryAllocationLib$(EOL)\
  UefiLib$(EOL)\
  $(EOL)\
[Depex]$(EOL)\
	$(subst $(SPACE),$(EOL)$(SPACE),$(MEFwUpdLcLDepex))$(EOL)\
$(EOL)"\
> $(BUILD_DIR)$(PATH_SLASH)MeFwUpdLcl.inf