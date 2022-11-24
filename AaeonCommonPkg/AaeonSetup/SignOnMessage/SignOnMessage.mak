Prepare : $(BUILD_DIR)/SignOnMessage.uni

$(BUILD_DIR)/SignOnMessage.uni : $(BUILD_DIR)/Token.mak
	$(ECHO) \
"/=# $(EOL)\
//---------------------------------------------------------------------------$(EOL)\
// Auto Generated BIOS Sign-On Message Setup String. Do not modify$(EOL)\
//---------------------------------------------------------------------------$(EOL)\
#include \"VFR.uni\"$(EOL)\
$(EOL)\
#string STR_BIOS_VERSION_VALUE_AMIBCP         #language eng \"$(AAEON_MODEL_TAG)$(AAEON_VERSION)\"$(EOL)"\
> $(BUILD_DIR)\Temp.uni
	$(GENUNIFILE) $(BUILD_DIR)\Temp.uni $@


