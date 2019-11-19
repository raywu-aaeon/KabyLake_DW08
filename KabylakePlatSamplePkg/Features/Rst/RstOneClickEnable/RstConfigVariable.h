#ifndef __RST_CONFIG_VARIABLE__
#define __RST_CONFIG_VARIABLE__

extern EFI_GUID gRstConfigVariableGuid;

CHAR16 *RstConfigVariableName = L"RstOptaneConfig";

typedef struct {
  UINT8 RemapEnable : 1;
  UINT8 ModeSwitch : 1;
  UINT8 Reserved : 6;
} RST_CONFIG_VARIABLE;

#endif
