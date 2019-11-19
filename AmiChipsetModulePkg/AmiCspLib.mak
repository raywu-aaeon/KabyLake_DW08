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

#/** @file AmiCspLib.mak
#    Make file for the CSP Library.
#
#    CSP library contains all library 
#    functions related to chipset and CPU. The main idea for this library 
#    is to consolidate porting hooks related to various components 
#    (like CSM, SMM etc) in respective chipset files say NB, SB, CPU etc.  
#    This library can be built us a LIB file or as various FFS files.  
#    The MACROS and library init functions mask the actual implementation
#    which is controlled by the SDL token
#
#*/

Prepare :   $(BUILD_DIR)/AmiCspLibInc.h $(BUILD_DIR)/AmiCspLibPei.inf $(BUILD_DIR)/AmiCspLibDxe.inf $(BUILD_DIR)/Platform.dec
AMI_CSP_LIB_INCLUDE_FILES_CLEAN = $(subst +,$(SPACE),$(AMI_CSP_LIB_INCLUDE_FILES))
#---------------------------------------------------------------------------
#       Generate CSP Library
#---------------------------------------------------------------------------
$(BUILD_DIR)/AmiCspLibInc.h : $(AMI_CSP_LIB_INCLUDE_FILES_CLEAN)
	$(ECHO) \
"#include \"Token.h\"$(EOL)\
#include \"Pei.h\"$(EOL)\
#include \"AmiCompatibilityPkg/Include/Setup.h\"$(EOL)\
$(if $(AMI_CSP_LIB_INCLUDE_FILES_CLEAN) ,\
$(foreach S_DEF, $(AMI_CSP_LIB_INCLUDE_FILES_CLEAN),#include<$(S_DEF)>$(EOL)))"\
> $(BUILD_DIR)/AmiCspLibInc.h

$(BUILD_DIR)/AmiCspLibPei.inf : $(BUILD_DIR)/Token.h $(AmiCspLib_DIR)/AmiCspLib.mak
	$(ECHO)  \
"[Defines]$(EOL)\
  INF_VERSION                    = 0x00010005$(EOL)\
  BASE_NAME                      = AmiCspLibPei$(EOL)\
  FILE_GUID                      = 4B8F8866-D803-4b13-9A55-44BAEA414905$(EOL)\
  MODULE_TYPE                    = PEIM$(EOL)\
  VERSION_STRING                 = 1.0$(EOL)\
  LIBRARY_CLASS                  = AmiCspLib|PEIM PEI_CORE SEC $(EOL)\
$(EOL)\
[Sources]$(EOL)\
  $(patsubst %,../%,$(subst $(SPACE),$(EOL)$(SPACE),$(AMI_CSP_PEI_SOURCES)))$(EOL)\
  $(EOL)\
[Packages] $(EOL)\
  MdePkg/MdePkg.dec$(EOL)\
  IntelFrameworkPkg/IntelFrameworkPkg.dec$(EOL)\
  AmiCompatibilityPkg/AmiCompatibilityPkg.dec$(EOL)\
  AmiModulePkg/AmiModulePkg.dec$(EOL)\
  AmiChipsetModulePkg/AmiChipsetModulePkg.dec$(EOL)\
  AmiChipsetPkg/AmiChipsetPkg.dec$(EOL)\
  Build/Platform.dec$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AMI_CSP_PACKAGES))$(EOL)\
  $(EOL)\
[Pcd.common]$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AMI_CSP_PCD))$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AMI_CSP_PEI_PCD))$(EOL)\
  $(EOL)\
[Ppis]$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AMI_CSP_PEI_PPIS))$(EOL)\
  $(EOL)\
[Guids]$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AMI_CSP_PEI_GUIDS))$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AMI_CSP_GUIDS))$(EOL)\
  $(EOL)\
[LibraryClasses]$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AMI_CSP_LIBRARIES))$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AMI_CSP_PEI_LIBRARIES))$(EOL)"\
> $(BUILD_DIR)/AmiCspLibPei.inf

$(BUILD_DIR)/AmiCspLibDxe.inf : $(BUILD_DIR)/Token.h $(AmiCspLib_DIR)/AmiCspLib.mak
	$(ECHO)  \
"[Defines]$(EOL)\
  INF_VERSION                    = 0x00010005$(EOL)\
  BASE_NAME                      = AmiCspLibDxe$(EOL)\
  FILE_GUID                      = 94F399A7-C224-40AD-B695-FDF7EFDE4805$(EOL)\
  MODULE_TYPE                    = DXE_DRIVER$(EOL)\
  VERSION_STRING                 = 1.0$(EOL)\
  LIBRARY_CLASS                  = AmiCspLib|DXE_DRIVER DXE_CORE DXE_RUNTIME_DRIVER UEFI_DRIVER SMM_CORE DXE_SMM_DRIVER UEFI_APPLICATION$(EOL)\
$(EOL)\
[Sources]$(EOL)\
  $(patsubst %,../%,$(subst $(SPACE),$(EOL)$(SPACE),$(AMI_CSP_DXE_SOURCES)))$(EOL)\
  $(EOL)\
[Pcd.common]$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AMI_CSP_PCD))$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AMI_CSP_DXE_PCD))$(EOL)\
$(EOL)\
[Packages] $(EOL)\
  MdePkg/MdePkg.dec$(EOL)\
  IntelFrameworkPkg/IntelFrameworkPkg.dec$(EOL)\
  AmiCompatibilityPkg/AmiCompatibilityPkg.dec$(EOL)\
  AmiModulePkg/AmiModulePkg.dec$(EOL)\
  Build/Platform.dec$(EOL)\
  AmiChipsetModulePkg/AmiChipsetModulePkg.dec$(EOL)\
  AmiChipsetPkg/AmiChipsetPkg.dec$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AMI_CSP_PACKAGES))$(EOL)\
  $(EOL)\
[Protocols]$(EOL)\
  gEfiAcpiTableProtocolGuid$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AMI_CSP_DXE_PROTOCOLS))$(EOL)\
  $(EOL)\
[Guids]$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AMI_CSP_DXE_GUIDS))$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AMI_CSP_GUIDS))$(EOL)\
  $(EOL)\
[LibraryClasses]$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AMI_CSP_LIBRARIES))$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AMI_CSP_DXE_LIBRARIES))$(EOL)\
[BuildOptions]$(EOL)\
  MSFT:*_*_*_CC_FLAGS = /D AMI_CSP_DXE_SUPPORT$(EOL)\
  GCC:*_*_*_CC_FLAGS = -DAMI_CSP_DXE_SUPPORT$(EOL)"\
> $(BUILD_DIR)/AmiCspLibDxe.inf

$(BUILD_DIR)/Platform.dec : $(BUILD_DIR)/Token.h $(AmiChipsetModulePkg_DIR)/AmiCspLib.mak
	$(ECHO)  \
"#**********************************************************************$(EOL)\
#**********************************************************************$(EOL)\
#**                                                                  **$(EOL)\
#**        (C)Copyright 1985-2017, American Megatrends, Inc.         **$(EOL)\
#**                                                                  **$(EOL)\
#**                       All Rights Reserved.                       **$(EOL)\
#**                                                                  **$(EOL)\
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **$(EOL)\
#**                                                                  **$(EOL)\
#**                       Phone: (770)-246-8600                      **$(EOL)\
#**                                                                  **$(EOL)\
#**********************************************************************$(EOL)\
#**********************************************************************$(EOL)\
## @file$(EOL)\
$(EOL)\
[Defines]$(EOL)\
  DEC_SPECIFICATION              = 0x00010005$(EOL)\
  PACKAGE_NAME                   = AmiCspLib$(EOL)\
  PACKAGE_GUID                   = 072a622d-0a5f-4d69-ab23-81222cc85c12$(EOL)\
  PACKAGE_VERSION                = 0.1$(EOL)\
$(EOL)\
[Includes]$(EOL)\
  $(subst $(SPACE), $(EOL)../, $(PLATFORM_DSC_INCLUDE_PATHS))$(EOL)"\
 >$@

#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************