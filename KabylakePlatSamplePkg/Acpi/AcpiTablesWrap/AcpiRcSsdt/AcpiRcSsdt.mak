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

#**********************************************************************
# $Header: /Alaska/Tools/template.mak 6     1/13/10 2:13p Felixp $
#
# $Revision: 6 $
#
# $Date: 1/13/10 2:13p $
#**********************************************************************
#<AMI_FHDR_START>
#
# Name:	<ComponentName>.mak
#
# Description:	
#
#<AMI_FHDR_END>
#**********************************************************************
Prepare : CreateINFFile


CreateINFFile: 	
	$(ECHO) \
	"{$(EOL)\
	gsub(/RVP3RTD3_SSDT_PATH/,\"../$(ACPI_RC_RVP3RTD3_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/RVP3RTD3_TBTAIC_SSDT_PATH/,\"../$(ACPI_RC_RVP3RTD3_TBTAIC_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/AMLRTD3_TBTAIC_SSDT_PATH/,\"../$(ACPI_RC_AMLRTD3_TBTAIC_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/AMLY42RTD3_SSDT_PATH/,\"../$(ACPI_RC_AMLY42RTD3_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/AMLY42RTD3_TBTAIC_SSDT_PATH/,\"../$(ACPI_RC_AMLY42RTD3_TBTAIC_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/AMLY42AEPRTD3_TBT_SSDT_PATH/,\"../$(ACPI_RC_AMLY42AEPRTD3_TBT_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/RVP3RTD3_WWAN_SSDT_PATH/,\"../$(ACPI_RC_RVP3RTD3_WWAN_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/RVP7RTD3_SSDT_PATH/,\"../$(ACPI_RC_RVP7RTD3_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/RVPRRTD3_SSDT_PATH/,\"../$(ACPI_RC_RVPRRTD3_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/RVP10RTD3_SSDT_PATH/,\"../$(ACPI_RC_RVP10RTD3_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/RVP11RTD3_SSDT_PATH/,\"../$(ACPI_RC_RVP11RTD3_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/RVPKCRTD3_SSDT_PATH/,\"../$(ACPI_RC_RVPKCRTD3_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/AMI_DEFAULT_RTD3_SSDT_PATH/,\"../$(ACPI_AMI_DEFAULT_RTD3_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/SDSRTD3_SSDT_PATH/,\"../$(ACPI_RC_SDSRTD3_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/RTD3HSBSDS_SSDT_PATH/,\"../$(ACPI_RC_RTD3HSBSDS_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/THERMAL_SSDT_PATH/,\"../$(ACPI_RC_THERMAL_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/THERMALSDS_SSDT_PATH/,\"../$(ACPI_RC_THERMALSDS_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/TBT_SSDT_PATH/,\"../$(ACPI_RC_TBT_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/USBPORTXHCI_RVP3_SSDT_PATH/,\"../$(ACPI_RC_USBPORTXHCI_RVP3_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/USBPORTXHCI_RVP3_AML_SSDT_PATH/,\"../$(ACPI_RC_USBPORTXHCI_RVP3_AML_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/USBPORTXHCI_AML_Y42_SSDT_PATH/,\"../$(ACPI_RC_USBPORTXHCI_AML_Y42_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/USBPORTXHCI_RVP3_SKL_SSDT_PATH/,\"../$(ACPI_RC_USBPORTXHCI_RVP3_SKL_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/USBPORTXHCI_RVP7_SSDT_PATH/,\"../$(ACPI_RC_USBPORTXHCI_RVP7_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/USBPORTXHCI_RVP10_SSDT_PATH/,\"../$(ACPI_RC_USBPORTXHCI_RVP10_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/USBPORTXHCI_RVP16_SSDT_PATH/,\"../$(ACPI_RC_USBPORTXHCI_RVP16_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/USBPORTXHCI_RVP8_SSDT_PATH/,\"../$(ACPI_RC_USBPORTXHCI_RVP8_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/USBPORTXHCI_KBLS_DDR4_UDIMM_SSDT_PATH/,\"../$(ACPI_AMI_USBPORTXHCI_KBLS_DDR4_UDIMM_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/USBPORTXHCI_KBLS_EV_DDR4_UDIMM_SSDT_PATH/,\"../$(ACPI_AMI_USBPORTXHCI_KBLS_EV_DDR4_UDIMM_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/USBPORTXHCI_KBLR_U_SSDT_PATH/,\"../$(ACPI_AMI_USBPORTXHCI_KBLR_U_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/USBPORTXHCI_AML_AEP_SSDT_PATH/,\"../$(ACPI_AMI_USBPORTXHCI_AML_AEP_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/USBPORTXHCI_ZUMBA_SSDT_PATH/,\"../$(ACPI_RC_USBPORTXHCI_ZUMBA_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/AMIUSBPORTXHCI_RVP11_SSDT_PATH/,\"../$(ACPI_AMI_USBPORTXHCI_RVP11_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/AMIUSBPORTXHCI_DEFAULT_SSDT_PATH/,\"../$(ACPI_AMI_USBPORTXHCI_DEFAULT_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	gsub(/HPET_SSDT_PATH/,\"../$(ACPI_RC_HPET_SSDT_PATH)\",$(DOUBLEDOLLAR)0)$(EOL)\
	print $(DOUBLEDOLLAR)0$(EOL)\
	}"$(EOL)\
> $(BUILD_DIR)$(PATH_SLASH)AcpiRcSsdtPatchfile.txt
	$(GAWK) -f $(BUILD_DIR)$(PATH_SLASH)AcpiRcSsdtPatchfile.txt $(ACPI_RC_SSDT_INF_PATH) > $(BUILD_DIR)$(PATH_SLASH)AcpiRcSsdt.inf
	

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