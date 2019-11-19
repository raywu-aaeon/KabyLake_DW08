#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
#**                                                                     **
#**                       All Rights Reserved.                          **
#**                                                                     **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
#**                                                                     **
#**                       Phone: (770)-246-8600                         **
#**                                                                     **
#*************************************************************************
#*************************************************************************

.PHONY : RebaseAndPatchFspBinBaseAddress

Prepare : RebaseAndPatchFspBinBaseAddress

RebaseAndPatchFspBinBaseAddress:
	$(RM) $(BUILD_DIR)\Fsp_Rebased*.fd
	$(FSP_BIN_PKG_TOOLS_DIR)\RebaseAndPatchFspBinBaseAddress.exe $(TARGET) Build\Platform.fdf $(FSP_BIN_PKG_DIR) $(FSP_BINARY_NAME) $(ACTIVE_PLATFORM) 0x70"
	$(CP) /y /b $(BUILD_DIR)\Fsp_Rebased_M.fd+$(BUILD_DIR)\Fsp_Rebased_T.fd $(BUILD_DIR)\Fsp_Rebased_M_T.fd
	@REM prefix pad.bin file which adds 0xC bytes of data (Note: Section will add 4 bytes of SECTION Header). This is done to align the FSP Header to 16 bytes
	$(CP) /y /b $(FSP_BIN_PKG_TOOLS_DIR)\pad.bin+$(BUILD_DIR)\Fsp_Rebased_S.fd $(BUILD_DIR)\Fsp_Rebased_S_padded.fd

#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************