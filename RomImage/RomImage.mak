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

#<AMI_FHDR_START>
#-----------------------------------------------------------------------
#
# Name:         RomImage.mak
#
# Description:
#
#-----------------------------------------------------------------------
#<AMI_FHDR_END>

.PHONY : BUILD_ME

Prepare : DelAmiCrbMeRoms

ifneq ($(wildcard $(BIOS_GUARD_PRIKEY)),$(BIOS_GUARD_PRIKEY)) 
        BiosGuard_SUPPORT := 0
endif

ifeq ($(BUILD_OS), $(BUILD_OS_WINDOWS))
        CALL := call
else
ifdef WINECONSOLE
        CALL := $(WINECONSOLE)
else
        CALL := wineconsole
endif #ifdef WINECONSOLE
endif #ifeq ($(BUILD_OS), $(BUILD_OS_WINDOWS))

DelAmiCrbMeRoms:
    ifeq ($(ME_ROM_IMAGE_BUILD_SUPPORT), 1)
        ifneq ("$(wildcard  $(UNSIGNED_BIOS_ROM))", "")
			$(RM)  $(UNSIGNED_BIOS_ROM)
        endif
    endif

BUILD_ME:
#
# Copy a BIOS rom to the build folder for Fit packaging.
#
ifeq ($(FWCAPSULE_FILE_FORMAT), 1)
	$(CP) $(UNSIGNED_BIOS_ROM)  $(BUILD_DIR)$(PATH_SLASH)FITc_BIOS.rom
else
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AMIBIOS.rom  $(BUILD_DIR)$(PATH_SLASH)FITc_BIOS.rom
endif #ifeq ($(FWCAPSULE_FILE_FORMAT), 1)

ifeq ($(BUILD_OS), $(BUILD_OS_LINUX))
	$(ECHO) "RomImage module: Build environment is Linux."
endif #ifeq ($(BUILD_OS), $(BUILD_OS_LINUX))

ifeq ($(SPS_ROM_IMAGE_BUILD_SUPPORT), 1)

#*************************************************************************
#**                                                                     **
#**    Intel Server Platform Services Manageability Engine Firmware     **
#**                                                                     **
#*************************************************************************

ifeq ($(ZUMBA_BEACH_SERVER_EV), 1)
	$(CALL) $(ROM_IMAGE_DIR)$(PATH_SLASH)SpsFlashImageTool$(PATH_SLASH)ZumbaBeachServerEV_sps.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)ZumbaBeachServerEV_sps.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)ZumbaBeachServerEV_sps.bin ZumbaBeachServerEV_sps.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
endif

ifeq ($(ZUMBA_BEACH_SERVER_EV), 1)
	$(CALL) $(ROM_IMAGE_DIR)$(PATH_SLASH)SpsFlashImageTool$(PATH_SLASH)ZumbaBeachServerEV_spsfull.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)ZumbaBeachServerEV_spsfull.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)ZumbaBeachServerEV_spsfull.bin ZumbaBeachServerEV_spsfull.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
endif

else #//else of "ifeq ($(SPS_ROM_IMAGE_BUILD_SUPPORT), 1)"

#*************************************************************************
#**                                                                     **
#**                Intel Management Engine Firmware 11                  **
#**                                                                     **
#*************************************************************************

ifeq ($(SKL_BUILD_ALL), 1)

#*********************
#**    Consumer     **
#*********************

	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP3_C_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP5_C_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP7_C_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP3_C_ME115.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP5_C_ME115.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP7_C_ME115.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP8_D_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP8_D_OC_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP9_D_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP10_D_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP10_D_OC_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP11_D_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_KBL_uDIMM_D_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_KBL_Refresh_C_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_KBL_Refresh_C.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP3_C.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP3_Y42_C.bat	
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP5_C.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP7_C.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP8_D.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP8_D_OC.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP9_D.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP10_D.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP10_D_OC.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP11_D.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_KBL_uDIMM_D.bat

ifeq ($(BUILD_NON_PRODUCTION), 1)
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP3_C_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP5_C_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP7_C_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP3_C_ME115.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP5_C_ME115.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP7_C_ME115.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP8_D_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP8_D_OC_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP9_D_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP10_D_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP10_D_OC_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP11_D_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_KBL_uDIMM_D_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_KBL_Refresh_C_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_KBL_Refresh_C.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP3_C.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP3_Y42_C.bat	
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP5_C.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP7_C.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP8_D.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP8_D_OC.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP9_D.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP10_D.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP10_D_OC.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP11_D.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_KBL_uDIMM_D.bat
endif #//else of "ifeq ($(BUILD_NON_PRODUCTION), 1)"

#**********************
#**    Corporate     **
#**********************

	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP3_C_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP5_C_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP7_C_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP3_C_ME115.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP5_C_ME115.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP7_C_ME115.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP8_C236_D_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP8_D_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP8_D_OC_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP9_D_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP10_D_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP10_D_OC_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP11_D_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_KBL_uDIMM_D_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_KBL_Refresh_C_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_KBL_Refresh_C.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP3_C.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP3_Y42_C.bat	
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP5_C.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP7_C.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP8_C236_D.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP8_D.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP8_D_OC.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP9_D.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP10_D.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP10_D_OC.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP11_D.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_KBL_uDIMM_D.bat

ifeq ($(BUILD_NON_PRODUCTION), 1)
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP3_C_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP5_C_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP7_C_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP3_C_ME115.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP5_C_ME115.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP7_C_ME115.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP8_C236_D_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP8_D_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP8_D_OC_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP9_D_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP10_D_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP10_D_OC_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP11_D_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_KBL_uDIMM_D_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_KBL_Refresh_C_ME116.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_KBL_Refresh_C.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP3_C.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP3_Y42_C.bat	
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP5_C.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP7_C.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP8_C236_D.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP8_D.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP8_D_OC.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP9_D.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP10_D.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP10_D_OC.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP11_D.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_KBL_uDIMM_D.bat
endif #//else of "ifeq ($(BUILD_NON_PRODUCTION), 1)"

else#//else of "ifeq ($(SKL_BUILD_ALL), 1)"

ifeq ($(Consumer_RVP3_C), 1)
#
# ME 11.5
#
#	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP3_C_ME115.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP3_C_Cons_Prod_ME115.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP3_C_Cons_Prod_ME115.bin RVP3_C_Cons_Prod_ME115.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP3_C_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP3_C_Cons_Prod_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP3_C_Cons_Prod_ME116.bin RVP3_C_Cons_Prod_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
ifeq ($(Consumer_RVP3_Y42_C), 1)
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP3_Y42_C.bat
else
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP3_C.bat
endif	
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP3_C_Cons_Prod.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP3_C_Cons_Prod.bin RVP3_C_Cons_Prod.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
ifeq ($(BUILD_NON_PRODUCTION), 1)
#
# ME 11.5
#
#	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP3_C_ME115.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP3_C_Cons_ME115.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP3_C_Cons_ME115.bin RVP3_C_Cons_ME115.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP3_C.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP3_C_Cons.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP3_C_Cons.bin RVP3_C_Cons.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
ifeq ($(Consumer_RVP3_Y42_C), 1)
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP3_Y42_C.bat
else
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP3_C.bat
endif		
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP3_C_Cons.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP3_C_Cons.bin RVP3_C_Cons.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
endif #//else of "ifeq ($(BUILD_NON_PRODUCTION), 1)"
endif #//else of "ifeq ($(Consumer_RVP3_C), 1)"

ifeq ($(Consumer_RVP5_C), 1)
#
# ME 11.5
#
#	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP5_C_ME115.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP5_C_Cons_Prod_ME115.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP5_C_Cons_Prod_ME115.bin RVP5_C_Cons_Prod_ME115.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP5_C_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP5_C_Cons_Prod_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP5_C_Cons_Prod_ME116.bin RVP5_C_Cons_Prod_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP5_C.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP5_C_Cons_Prod.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP5_C_Cons_Prod.bin RVP5_C_Cons_Prod.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
ifeq ($(BUILD_NON_PRODUCTION), 1)
#
# ME 11.5
#
#	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP5_C_ME115.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP5_C_Cons_ME115.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP5_C_Cons_ME115.bin RVP5_C_Cons_ME115.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP5_C_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP5_C_Cons_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP5_C_Cons_ME116.bin RVP5_C_Cons_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP5_C.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP5_C_Cons.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP5_C_Cons.bin RVP5_C_Cons.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
endif #//else of "ifeq ($(BUILD_NON_PRODUCTION), 1)"
endif #//else of "ifeq ($(Consumer_RVP5_C), 1)"

ifeq ($(Consumer_RVP7_C), 1)
#
# ME 11.5
#
#	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP7_C_ME115.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP7_C_Cons_Prod_ME115.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP7_C_Cons_Prod_ME115.bin RVP7_C_Cons_Prod_ME115.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP7_C_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP7_C_Cons_Prod_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP7_C_Cons_Prod_ME116.bin RVP7_C_Cons_Prod_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP7_C.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP7_C_Cons_Prod.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP7_C_Cons_Prod.bin RVP7_C_Cons_Prod.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
ifeq ($(BUILD_NON_PRODUCTION), 1)
#
# ME 11.5
#
#	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP7_C_ME115.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP7_C_Cons_ME115.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP7_C_Cons_ME115.bin RVP7_C_Cons_ME115.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP7_C_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP7_C_Cons_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP7_C_Cons_ME116.bin RVP7_C_Cons_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP7_C.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP7_C_Cons.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP7_C_Cons.bin RVP7_C_Cons.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
endif #//else of "ifeq ($(BUILD_NON_PRODUCTION), 1)"
endif #//else of "ifeq ($(Consumer_RVP7_C), 1)"

ifeq ($(Consumer_RVP8_D), 1)
#	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP8_D_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_D_Cons_Prod_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_D_Cons_Prod_ME116.bin RVP8_D_Cons_Prod_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP8_D.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_D_Cons_Prod.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_D_Cons_Prod.bin RVP8_D_Cons_Prod.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
ifeq ($(BUILD_NON_PRODUCTION), 1)
#	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP8_D_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_D_Cons_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_D_Cons_ME116.bin RVP8_D_Cons_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP8_D.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_D_Cons.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_D_Cons.bin RVP8_D_Cons.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
endif #//else of "ifeq ($(BUILD_NON_PRODUCTION), 1)"
endif #//else of "ifeq ($(Consumer_RVP8_D), 1)"

ifeq ($(Consumer_RVP8_D_OC), 1)
#	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP8_D_OC_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_D_OC_Cons_Prod_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_D_OC_Cons_Prod_ME116.bin RVP8_D_OC_Cons_Prod_ME116_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP8_D_OC.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_D_OC_Cons_Prod.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_D_OC_Cons_Prod.bin RVP8_D_OC_Cons_Prod.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
ifeq ($(BUILD_NON_PRODUCTION), 1)
#	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP8_D_OC.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_D_OC_Cons_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_D_OC_Cons_ME116.bin RVP8_D_OC_Cons_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP8_D_OC.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_D_OC_Cons.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_D_OC_Cons.bin RVP8_D_OC_Cons.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
endif #//else of "ifeq ($(BUILD_NON_PRODUCTION), 1)"
endif #//else of "ifeq ($(Consumer_RVP8_D_OC), 1)"

ifeq ($(Consumer_RVP9_D), 1)
#	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP9_D_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP9_D_Cons_Prod_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP9_D_Cons_Prod_ME116.bin RVP9_D_Cons_Prod_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP9_D.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP9_D_Cons_Prod.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP9_D_Cons_Prod.bin RVP9_D_Cons_Prod.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
ifeq ($(BUILD_NON_PRODUCTION), 1)
#	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP9_D_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP9_D_Cons_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP9_D_Cons_ME116.bin RVP9_D_Cons_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP9_D.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP9_D_Cons.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP9_D_Cons.bin RVP9_D_Cons.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
endif #//else of "ifeq ($(BUILD_NON_PRODUCTION), 1)"
endif #//else of "ifeq ($(Consumer_RVP9_D), 1)"

ifeq ($(Consumer_RVP10_D), 1)
#	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP10_D_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP10_D_Cons_Prod_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP10_D_Cons_Prod_ME116.bin RVP10_D_Cons_Prod_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP10_D.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP10_D_Cons_Prod.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP10_D_Cons_Prod.bin RVP10_D_Cons_Prod.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
ifeq ($(BUILD_NON_PRODUCTION), 1)
#	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP10_D_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP10_D_Cons_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP10_D_Cons_ME116.bin RVP10_D_Cons_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP10_D.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP10_D_Cons.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP10_D_Cons.bin RVP10_D_Cons.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
endif #//else of "ifeq ($(BUILD_NON_PRODUCTION), 1)"
endif #//else of "ifeq ($(Consumer_RVP10_D), 1)"

ifeq ($(Consumer_RVP10_D_OC), 1)
#	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP10_D_OC_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP10_D_OC_Cons_Prod_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP10_D_OC_Cons_Prod_ME116.bin RVP10_D_OC_Cons_Prod_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP10_D_OC.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP10_D_OC_Cons_Prod.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP10_D_OC_Cons_Prod.bin RVP10_D_OC_Cons_Prod.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
ifeq ($(BUILD_NON_PRODUCTION), 1)
#	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP10_D_OC_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP10_D_OC_Cons_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP10_D_OC_Cons_ME116.bin RVP10_D_OC_Cons_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP10_D_OC.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP10_D_OC_Cons.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP10_D_OC_Cons.bin RVP10_D_OC_Cons.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
endif #//else of "ifeq ($(BUILD_NON_PRODUCTION), 1)"
endif #//else of "ifeq ($(Consumer_RVP10_D_OC), 1)"

ifeq ($(Consumer_RVP11_D), 1)
#	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP11_D_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP11_D_Cons_Prod_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP11_D_Cons_Prod_ME116.bin RVP11_D_Cons_Prod_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP11_D.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP11_D_Cons_Prod.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP11_D_Cons_Prod.bin RVP11_D_Cons_Prod.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
ifeq ($(BUILD_NON_PRODUCTION), 1)
#	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP11_D_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP11_D_Cons_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP11_D_Cons_ME116.bin RVP11_D_Cons_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_RVP11_D.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP11_D_Cons.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP11_D_Cons.bin RVP11_D_Cons.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
endif #//else of "ifeq ($(BUILD_NON_PRODUCTION), 1)"
endif #//else of "ifeq ($(Consumer_RVP11_D), 1)"

ifeq ($(Consumer_KBL_uDIMM_D), 1)
#	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_KBL_uDIMM_D_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_uDIMM_D_Cons_Prod_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_uDIMM_D_Cons_Prod_ME116.bin KBL_uDIMM_D_Cons_Prod_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_KBL_uDIMM_D.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_KBL_Z370_D.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_uDIMM_D_Cons_Prod.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_uDIMM_D_Cons_Prod.bin KBL_uDIMM_D_Cons_Prod.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_Z370_D_Cons_Prod.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_Z370_D_Cons_Prod.bin KBL_Z370_D_Cons_Prod.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
ifeq ($(BUILD_NON_PRODUCTION), 1)
#	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_KBL_uDIMM_D_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_uDIMM_D_Cons_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_uDIMM_D_Cons_ME116.bin KBL_uDIMM_D_Cons_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.7
#
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_KBL_uDIMM_D.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_KBL_Z370_D.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_uDIMM_D_Cons.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_uDIMM_D_Cons.bin KBL_uDIMM_D_Cons.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_Z370_D_Cons.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_Z370_D_Cons.bin KBL_Z370_D_Cons.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
endif #//else of "ifeq ($(BUILD_NON_PRODUCTION), 1)"
endif #//else of "ifeq ($(Consumer_KBL_uDIMM_D), 1)"

ifeq ($(Consumer_KBL_Refresh_C), 1)

#	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_KBL_Refresh_C_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_Refresh_C_Cons_Prod_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_Refresh_C_Cons_Prod_ME116.bin KBL_Refresh_C_Cons_Prod_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#ifeq ($(BUILD_NON_PRODUCTION), 1)
#	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_KBL_Refresh_C_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_Refresh_C_Cons_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_Refresh_C_Cons_ME116.bin KBL_Refresh_C_Cons_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#endif #//else of "ifeq ($(BUILD_NON_PRODUCTION), 1)"

#
# ME 11.8 For U4+2 
#
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_KBL_Refresh_C.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_Refresh_C_Cons_Prod.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_Refresh_C_Cons_Prod.bin KBL_Refresh_C_Cons_Prod.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
ifeq ($(BUILD_NON_PRODUCTION), 1)
	$(CALL) $(ROM_IMAGE_BAT_DIR)Consumer_KBL_Refresh_C.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_Refresh_C_Cons.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_Refresh_C_Cons.bin KBL_Refresh_C_Cons.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
endif #//else of "ifeq ($(BUILD_NON_PRODUCTION), 1)"

endif #//else of "ifeq ($(Consumer_KBL_Refresh_C), 1)"


ifeq ($(Consumer_RVP17_D), 1)
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Consumer_RVP17_D.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP17_D_Cons_Prod.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP17_D_Cons_Prod.bin RVP17_D_Cons_Prod.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif

endif #//else of "ifeq ($(Consumer_RVP17_D), 1)"

ifeq ($(SKL_BUILD_CORPORATE), 1)
ifeq ($(Corporate_RVP3_C), 1)
#
# ME 11.5
#
#	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP3_C_ME115.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP3_C_Corp_Prod_ME115.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP3_C_Corp_Prod_ME115.bin RVP3_C_Corp_Prod_ME115.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP3_C_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP3_C_Corp_Prod_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP3_C_Corp_Prod_ME116.bin RVP3_C_Corp_Prod_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#	
ifeq ($(Corporate_RVP3_Y42_C), 1)
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP3_Y42_C.bat
else
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP3_C.bat
endif
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP3_C_Corp_Prod.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP3_C_Corp_Prod.bin RVP3_C_Corp_Prod.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
ifeq ($(BUILD_NON_PRODUCTION), 1)
#
# ME 11.5
#
#	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP3_C_ME115.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP3_C_Corp_ME115.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP3_C_Corp_ME115.bin RVP3_C_Corp_ME115.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP3_C_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP3_C_Corp_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP3_C_Corp_ME116.bin RVP3_C_Corp_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
ifeq ($(Corporate_RVP3_Y42_C), 1)
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP3_Y42_C.bat
else
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP3_C.bat	
endif	
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP3_C_Corp.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP3_C_Corp.bin RVP3_C_Corp.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
endif #//else of "ifeq ($(BUILD_NON_PRODUCTION), 1)"
endif #//else of "ifeq ($(Corporate_RVP3_C), 1)"

ifeq ($(Corporate_RVP5_C), 1)
#
# ME 11.5
#
#	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP5_C_ME115.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP5_C_Corp_Prod_ME115.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP5_C_Corp_Prod_ME115.bin RVP5_C_Corp_Prod_ME115.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP5_C_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP5_C_Corp_Prod_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP5_C_Corp_Prod_ME116.bin RVP5_C_Corp_Prod_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP5_C.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP5_C_Corp_Prod.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP5_C_Corp_Prod.bin RVP5_C_Corp_Prod.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
ifeq ($(BUILD_NON_PRODUCTION), 1)
#
# ME 11.5
#
#	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP5_C_ME115.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP5_C_Corp_ME115.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP5_C_Corp_ME115.bin RVP5_C_Corp_ME115.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP5_C_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP5_C_Corp_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP5_C_Corp_ME116.bin RVP5_C_Corp_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP5_C.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP5_C_Corp.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP5_C_Corp.bin RVP5_C_Corp.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
endif #//else of "ifeq ($(BUILD_NON_PRODUCTION), 1)"
endif #//else of "ifeq ($(Corporate_RVP5_C), 1)"

ifeq ($(Corporate_RVP7_C), 1)
#
# ME 11.5
#
#	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP7_C_ME115.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP7_C_Corp_Prod_ME115.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP7_C_Corp_Prod_ME115.bin RVP7_C_Corp_Prod_ME115.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP7_C_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP7_C_Corp_Prod_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP7_C_Corp_Prod_ME116.bin RVP7_C_Corp_Prod_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP7_C.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP7_C_Corp_Prod.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP7_C_Corp_Prod.bin RVP7_C_Corp_Prod.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
ifeq ($(BUILD_NON_PRODUCTION), 1)
#
# ME 11.5
#
#	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP7_C_ME115.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP7_C_Corp_ME115.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP7_C_Corp_ME115.bin RVP7_C_Corp_ME115.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP7_C.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP7_C_Corp_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP7_C_Corp_ME116.bin RVP7_C_Corp_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP7_C.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP7_C_Corp.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP7_C_Corp.bin RVP7_C_Corp.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
endif #//else of "ifeq ($(BUILD_NON_PRODUCTION), 1)"
endif #//else of "ifeq ($(Corporate_RVP7_C), 1)"

ifeq ($(Corporate_RVP8_C236_D), 1)
#	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP8_C236_D_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_C236_D_Corp_Prod_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_C236_D_Corp_Prod_ME116.bin RVP8_C236_D_Corp_Prod_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP8_C236_D.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_C236_D_Corp_Prod.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_C236_D_Corp_Prod.bin RVP8_C236_D_Corp_Prod.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
ifeq ($(BUILD_NON_PRODUCTION), 1)
#	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP8_C236_D_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_C236_D_Corp_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_C236_D_Corp_ME116.bin RVP8_C236_D_Corp_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP8_C236_D.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_C236_D_Corp.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_C236_D_Corp.bin RVP8_C236_D_Corp.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
endif #//else of "ifeq ($(BUILD_NON_PRODUCTION), 1)"
endif #//else of "ifeq ($(Corporate_RVP8_C236_D), 1)"

ifeq ($(Corporate_RVP8_D_OC), 1)
#	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP8_D_OC_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_D_OC_Corp_Prod_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_D_OC_Corp_Prod_ME116.bin RVP8_D_OC_Corp_Prod_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP8_D_OC.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_D_OC_Corp_Prod.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_D_OC_Corp_Prod.bin RVP8_D_OC_Corp_Prod.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
ifeq ($(BUILD_NON_PRODUCTION), 1)
#	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP8_D_OC_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_D_OC_Corp_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_D_OC_Corp_ME116.bin RVP8_D_OC_Corp_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP8_D_OC.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_D_OC_Corp.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_D_OC_Corp.bin RVP8_D_OC_Corp.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
endif #//else of "ifeq ($(BUILD_NON_PRODUCTION), 1)"
endif #//else of "ifeq ($(Corporate_RVP8_D_OC), 1)"

ifeq ($(Corporate_RVP8_D), 1)
#	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP8_D_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_D_Corp_Prod_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_D_Corp_Prod_ME116.bin RVP8_D_Corp_Prod_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP8_D.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_D_Corp_Prod.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_D_Corp_Prod.bin RVP8_D_Corp_Prod.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
ifeq ($(BUILD_NON_PRODUCTION), 1)
#	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP8_D_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_D_Corp_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_D_Corp_ME116.bin RVP8_D_Corp_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP8_D.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_D_Corp.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP8_D_Corp.bin RVP8_D_Corp.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
endif #//else of "ifeq ($(BUILD_NON_PRODUCTION), 1)"
endif #//else of "ifeq ($(Corporate_RVP8_D), 1)"

ifeq ($(Corporate_RVP9_D), 1)
#	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP9_D_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP9_D_Corp_Prod_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP9_D_Corp_Prod_ME116.bin RVP9_D_Corp_Prod_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP9_D.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP9_D_Corp_Prod.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP9_D_Corp_Prod.bin RVP9_D_Corp_Prod.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
ifeq ($(BUILD_NON_PRODUCTION), 1)
#	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP9_D_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP9_D_Corp_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP9_D_Corp_ME116.bin RVP9_D_Corp_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP9_D.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP9_D_Corp.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP9_D_Corp.bin RVP9_D_Corp.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
endif #//else of "ifeq ($(BUILD_NON_PRODUCTION), 1)"
endif #//else of "ifeq ($(Corporate_RVP9_D), 1)"

ifeq ($(Corporate_RVP10_D_OC), 1)
#	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP10_D_OC_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP10_D_OC_Corp_Prod_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP10_D_OC_Corp_Prod_ME116.bin RVP10_D_OC_Corp_Prod_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP10_D_OC.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP10_D_OC_Corp_Prod.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP10_D_OC_Corp_Prod.bin RVP10_D_OC_Corp_Prod.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
#ifeq ($(BUILD_NON_PRODUCTION), 1)
#	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP10_D_OC_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP10_D_OC_Corp_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP10_D_OC_Corp_ME116.bin RVP10_D_OC_Corp_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#endif #//else of "ifeq ($(BUILD_NON_PRODUCTION), 1)"
endif #//else of "ifeq ($(Corporate_RVP10_D_OC), 1)"

ifeq ($(Corporate_RVP10_D), 1)
#	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP10_D_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP10_D_Corp_Prod_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP10_D_Corp_Prod_ME116.bin RVP10_D_Corp_Prod_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP10_D.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP10_D_Corp_Prod.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP10_D_Corp_Prod.bin RVP10_D_Corp_Prod.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
ifeq ($(BUILD_NON_PRODUCTION), 1)
#	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP10_D_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP10_D_Corp_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP10_D_Corp_ME116.bin RVP10_D_Corp_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP10_D.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP10_D_Corp.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP10_D_Corp.bin RVP10_D_Corp.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
endif #//else of "ifeq ($(BUILD_NON_PRODUCTION), 1)"
endif #//else of "ifeq ($(Corporate_RVP10_D), 1)"

ifeq ($(Corporate_RVP11_D), 1)
#	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP11_D_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP11_D_Corp_Prod_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP11_D_Corp_Prod_ME116.bin RVP11_D_Corp_Prod_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP11_D.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP11_D_Corp_Prod.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP11_D_Corp_Prod.bin RVP11_D_Corp_Prod.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
ifeq ($(BUILD_NON_PRODUCTION), 1)
#	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP11_D_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP11_D_Corp_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP11_D_Corp_ME116.bin RVP11_D_Corp_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_RVP11_D.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP11_D_Corp.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP11_D_Corp.bin RVP11_D_Corp.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
endif #//else of "ifeq ($(BUILD_NON_PRODUCTION), 1)"
endif #//else of "ifeq ($(Corporate_RVP11_D), 1)"

ifeq ($(Corporate_KBL_uDIMM_D), 1)
#	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_KBL_uDIMM_D_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_uDIMM_D_Corp_Prod_ME116map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_uDIMM_D_Corp_Prod_ME116.bin KBL_uDIMM_D_Corp_Prod_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.8
#
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_KBL_uDIMM_D.bat
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_KBL_Z370_D.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_uDIMM_D_Corp_Prod.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_uDIMM_D_Corp_Prod.bin KBL_uDIMM_D_Corp_Prod.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_Z370_D_Corp_Prod.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_Z370_D_Corp_Prod.bin KBL_Z370_D_Corp_Prod.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
ifeq ($(BUILD_NON_PRODUCTION), 1)
#	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_KBL_uDIMM_D_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_uDIMM_D_Corp_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_uDIMM_D_Corp_ME116.bin KBL_uDIMM_D_Corp_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#
# ME 11.7
#
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_KBL_uDIMM_D.bat
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_KBL_Z370_D.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_uDIMM_D_Corp.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_uDIMM_D_Corp.bin KBL_uDIMM_D_Corp.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_Z370_D_Corp.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_Z370_D_Corp.bin KBL_Z370_D_Corp.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
endif #//else of "ifeq ($(BUILD_NON_PRODUCTION), 1)"
endif #//else of "ifeq ($(Corporate_KBL_uDIMM_D), 1)"

ifeq ($(Corporate_KBL_Refresh_C), 1)

#	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_KBL_Refresh_C_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_Refresh_C_Corp_Prod_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_Refresh_C_Corp_Prod_ME116.bin KBL_Refresh_C_Corp_Prod_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#ifeq ($(BUILD_NON_PRODUCTION), 1)
#	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_KBL_Refresh_C_ME116.bat
#ifeq ($(BiosGuard_SUPPORT), 1)
#	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_Refresh_C_Corp_ME116.map $(FITC_ME_MAP)
#	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_Refresh_C_Corp_ME116.bin KBL_Refresh_C_Corp_ME116.bin > $(BIOS_GUARD_NAME)
#	$(CALL) $(BUILD_BIOS_GUARD_PSL)
#	$(RM) $(FITC_ME_MAP)
#endif
#endif #//else of "ifeq ($(BUILD_NON_PRODUCTION), 1)"

#
# ME 11.8 For U4+2 
#
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_KBL_Refresh_C.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_Refresh_C_Corp_Prod.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_Refresh_C_Corp_Prod.bin KBL_Refresh_C_Corp_Prod.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
ifeq ($(BUILD_NON_PRODUCTION), 1)
	$(CALL) $(ROM_IMAGE_BAT_DIR)Corporate_KBL_Refresh_C.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_Refresh_C_Corp.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)KBL_Refresh_C_Corp.bin KBL_Refresh_C_Corp.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif
endif #//else of "ifeq ($(BUILD_NON_PRODUCTION), 1)"

endif #//else of "ifeq ($(Corporate_KBL_Refresh_C), 1)"

ifeq ($(Corporate_RVP17_D), 1)
	$(CALL) $(ROM_IMAGE_BAT_PROD_DIR)Corporate_RVP17_D.bat
ifeq ($(BiosGuard_SUPPORT), 1)
	$(CP) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP17_D_Corp_Prod.map $(FITC_ME_MAP)
	$(ECHO) $(BUILD_DIR)$(PATH_SLASH)AmiCrbMeRoms$(PATH_SLASH)RVP17_D_Corp_Prod.bin RVP17_D_Corp_Prod.bin > $(BIOS_GUARD_NAME)
	$(CALL) $(BUILD_BIOS_GUARD_PSL)
	$(RM) $(FITC_ME_MAP)
endif

endif #//else of "ifeq ($(Corporate_RVP17_D), 1)"


endif #//end of "ifeq ($(SKL_BUILD_CORPORATE), 1)"
endif #//end of "ifeq ($(SKL_BUILD_ALL), 1)"
endif #//else of "ifeq ($(SPS_ROM_IMAGE_BUILD_SUPPORT), 1)"

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