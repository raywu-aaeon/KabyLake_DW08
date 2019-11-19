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
# $Header:  $
#
# $Revision: $
#
# $Date:  $
#**********************************************************************
#<AMI_FHDR_START>
#
# Name:	PciPort.mak
#
# Description:	Make file to build Pci Compatibility Porting Driver
#
#<AMI_FHDR_END>
#**********************************************************************
Prepare : $(BUILD_DIR)/PciPortElink.h

$(BUILD_DIR)/PciPortElink.h :  $(BUILD_DIR)/Token.mak
	$(ECHO) \
"#define OEM_PCI_PROGRAM_DEVICE_CALLBACK_LIST $(OEM_PROGRAM_PCI_DEVICE) $(EOL)\
#define OEM_PCI_SKIP_DEVICE_CALLBACK_LIST $(OEM_SKIP_PCI_DEVICE) $(EOL)\
#define OEM_PCI_DEVICE_SET_ASPM_CALLBACK_LIST $(OEM_PCI_DEVICE_SET_ASPM) $(EOL)\
#define OEM_PCI_DEVICE_SET_LNK_SPEED_CALLBACK_LIST $(OEM_PCI_DEVICE_SET_LNK_SPEED) $(EOL)\
#define OEM_PCI_DEVICE_GET_OPT_ROM_CALLBACK_LIST $(OEM_PCI_DEVICE_GET_OPT_ROM) $(EOL)\
#define OEM_PCI_DEVICE_OUT_OF_RESOURCES_CALLBACK_LIST $(OEM_PCI_DEVICE_OUT_OF_RESOURCES) $(EOL)\
#define OEM_PCI_DEVICE_ATTRIBUTE_CALLBACK_LIST $(OEM_PCI_ATTRIBUTES) $(EOL)\
#define OEM_PCI_CHECK_BAD_PCIE_DEVICE_CALLBACK_LIST $(OEM_PCI_CHECK_BAD_PCIE_DEVICE) $(EOL)\
#define OEM_PCI_QUERY_PCI_DEVICE_CALLBACK_LIST $(OEM_PCI_QUERY_PCI_DEVICE) $(EOL)"\
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