#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
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
# Name:         NB.mak
#
# Description:  This make file builds north bridge header files.
#
#<AMI_FHDR_END>
#**********************************************************************
Prepare : 

AMI_CSP_LIB_INCLUDE_FILES := \
 $(BUILD_DIR)/NbInc.h \
$(AMI_CSP_LIB_INCLUDE_FILES)


Prepare : $(BUILD_DIR)/NbInc.h

$(BUILD_DIR)/NbInc.h: $(NB_INCLUDE_DIR)/Nb.h $(NB_INCLUDE_DIR)/NbSetupData.h $(NB_LIBRARY_INCLUDE_DIR)/NbCspLib.h $(AmiChipsetModulePkg_DIR)/Nb.mak
	$(ECHO) \
"// Don't delete this line$(EOL)\
#include <$(NB_INCLUDE_DIR)/Nb.h>$(EOL)\
#include <$(NB_INCLUDE_DIR)/NbSetupData.h>$(EOL)\
#include <$(NB_LIBRARY_INCLUDE_DIR)/NbCspLib.h>$(EOL)\
// Don't delete this line$(EOL)"\
> $@

#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2013, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************
