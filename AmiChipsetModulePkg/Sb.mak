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

#**********************************************************************
# $Header: $
#
# $Revision: $
#
# $Date: $
#**********************************************************************
#<AMI_FHDR_START>
#
# Name:		SB.mak
#
# Description:	
#
#<AMI_FHDR_END>
#**********************************************************************
Prepare : $(BUILD_DIR)/SbInc.h $(BUILD_DIR)/SbDxeInitElink.h $(BUILD_DIR)/SbPeiInitElink.h

AMI_CSP_LIB_INCLUDE_FILES := $(BUILD_DIR)/SbInc.h \
$(AMI_CSP_LIB_INCLUDE_FILES)

$(BUILD_DIR)/SbInc.h : $(SB_INCLUDE_DIR)/Sb.h $(SB_LIBRARY_INCLUDE_DIR)/SbCspLib.h $(AmiChipsetModulePkg_DIR)/Sb.mak
	$(ECHO) \
"// Don't delete this line$(EOL)\
#include <$(SB_INCLUDE_DIR)/Sb.h>$(EOL)\
#include <$(SB_LIBRARY_INCLUDE_DIR)/SbCspLib.h>$(EOL)\
#include <$(MDEPKG_DIR)/Include/Library/PciLib.h>$(EOL)\
// Don't delete this line$(EOL)"\
> $@
$(BUILD_DIR)/SbDxeInitElink.h :  $(BUILD_DIR)/Token.mak
	$(ECHO) \
"#define OEM_SPI_VSCC_TABLE $(OEM_SPI_VSCC_TABLE)$(EOL)\
#define SAVE_RESTORE_CALLBACK_LIST $(SbTimerSaveRestoreRegistersCallbacks)$(EOL)\
#define BUS_OVERRIDE_GUIDS_FOR_RAID $(BusOverrideGuidsforUefiRaid)$(EOL)"\
> $@

$(BUILD_DIR)/SbPeiInitElink.h : $(BUILD_DIR)/Token.h
	$(ECHO) \
"#define OEM_HDA_VERB_TABLE_CONTENT $(OEM_HDA_VERB_TABLE_CONTENT)$(EOL)\
#define OEM_HDA_VERB_TABLE $(OEM_HDA_VERB_TABLE)$(EOL)\
#define SB_OEM_ADJUST_GPIO_LIST $(SB_OEM_ADJUST_GPIO_LIST) $(EOL)"\
> $@

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
