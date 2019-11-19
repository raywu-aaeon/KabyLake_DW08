/**@file

@copyright
  INTEL CONFIDENTIAL
  Copyright 2010 - 2018 Intel Corporation.

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Intel Corporation or its suppliers or
  licensors. Title to the Material remains with Intel Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary and
  confidential information of Intel Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Intel in writing.

  Unless otherwise agreed by Intel in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Intel or
  Intel's suppliers or licensors in any way.

  This file contains a 'Sample Driver' and is licensed as such under the terms
  of your license agreement with Intel or your vendor. This file may be modified
  by the user, subject to the additional terms of the license agreement.

@par Specification Reference:
**/

#include <Protocol/HiiDatabase.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/HiiConfigRouting.h>
#include <Protocol/HiiString.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/HiiLib.h>
#include <Guid/GlobalVariable.h>
// AMI_OVERRIDE_START - Locate PrintLib for debugging.
#include <Library/PrintLib.h>
// AMI_OVERRIDE_END - Locate PrintLib for debugging.
#if 0 // AMI_OVERRIDE_START - For our Setup menu design.
#include "SetupPrivate.h"
#include <InitStringList.h>
#include <Library/DebugPrintErrorLevelLib.h>
#include <Library/SerialPortParameterLib.h>
#include <PlatformBoardId.h>
#include <Library/BoardConfigLib.h>

#include <CpuSmm.h>
#include <SaSetup.h>
#include "MeSetup.h"
#include "PlatformBoardId.h"
#ifdef TPM_FLAG
#include "TcgSetup.h"
#include "Tcg2ConfigNvData.h" // TCG2 implementation
#endif // TPM_FLAG
#ifdef SIO_NAT87393
#include "SioNat87393VSetup.h"
#endif //SIO_NAT87393
#ifdef SIO_WPCN381U
#include "SioWPCN381USetup.h"
#endif //SIO_WPCN381U
#include <IccSetupData.h>
#include <BootTime.h>
#include "SioSmsc1007Setup.h"

extern CALLBACK_INFO SetupCallBack[];
extern BOOLEAN  IsFirstBoot;
extern EFI_HII_CONFIG_ROUTING_PROTOCOL *mConfigRouting;

typedef struct {
  EFI_GUID    *VarGuid;
  CHAR16      *VarName;
  UINTN       VarSize;
} SETUP_HII_VARIABLE_INFO;

#define MAX_STRING_LENGTH          1024

//
// When you add new varstore, please add it to below struct
//
STATIC SETUP_HII_VARIABLE_INFO  mMainFormVar[] = {
  {&gSetupVariableGuid,         L"Setup",             sizeof (SETUP_DATA)},
  {&gSetupVariableGuid,         L"SetupVolatileData", sizeof (SETUP_VOLATILE_DATA)},
  {&gSystemAccessGuid,          L"SystemAccess",      sizeof (SYSTEM_ACCESS)},
};

STATIC SETUP_HII_VARIABLE_INFO  mAdvancedFormVar[] = {
  {&gSaSetupVariableGuid,             L"SaSetup",                 sizeof (SA_SETUP)},
  {&gMeSetupVariableGuid,             L"MeSetup",                 sizeof (ME_SETUP)},
  {&gCpuSetupVariableGuid,            L"CpuSetup",                sizeof (CPU_SETUP)},
  {&gPchSetupVariableGuid,            L"PchSetup",                sizeof (PCH_SETUP)},
  {&gSetupVariableGuid,               L"Setup",                   sizeof (SETUP_DATA)},
  {&gSystemAccessGuid,                L"SystemAccess",            sizeof (SYSTEM_ACCESS)},
#ifdef DEBUG_INTERFACE_FORM_ENABLE
  {&gDebugConfigVariableGuid,         L"DebugConfigData",         sizeof (DEBUG_CONFIG_DATA)},
#endif
#ifdef WHEA_SUPPORT_FLAG
  {&gSetupVariableGuid,                L"PciBusSetup",             sizeof (PCI_BUS_SETUP)},
  {&gSetupVariableGuid,                L"WheaSetup",               sizeof (WHEA_SETUP)},
#endif
  {&gOsProfileGuid,                    L"OsProfile",               sizeof (OS_PROFILE)},
  {&gSetupVariableGuid,                L"SetupCpuFeatures",        sizeof (SETUP_CPU_FEATURES)},
  {&gCpuSetupVariableGuid,             L"CpuSetupVolatileData",    sizeof (CPU_SETUP_VOLATILE_DATA)},
  {&gCpuSmmGuid,                       L"CpuSmm",                  sizeof (CPU_SMM)},
  {&gMeSetupVariableGuid,              L"MeSetupStorage",          sizeof (ME_SETUP_STORAGE)},
#ifdef TPM_FLAG
  {&gSetupVariableGuid,                L"TcgSetup",                sizeof (TCG_SETUP)},
  {&gTcg2ConfigFormSetGuid,            L"TCG2_CONFIGURATION",      sizeof (TCG2_CONFIGURATION)},
#endif
  // FLOPPY
  {&gEfiSioVariableGuid,              L"PNP0604_1_VV",               sizeof (FDC_V_DATA)},
  {&gEfiSioVariableGuid,              L"PNP0604_1_NV",               sizeof (FDC_NV_DATA)},
  // COM A
  {&gEfiSioVariableGuid,              L"PNP0501_0_VV",               sizeof (COMA_V_DATA)},
  {&gEfiSioVariableGuid,              L"PNP0501_0_NV",               sizeof (COMA_NV_DATA)},
  // COM B
  {&gEfiSioVariableGuid,              L"PNP0510_1_VV",               sizeof (COMB_V_DATA)},
  {&gEfiSioVariableGuid,              L"PNP0510_1_NV",               sizeof (COMB_NV_DATA)},
  // PARALLEL
  {&gEfiSioVariableGuid,              L"PNP0400_0_VV",               sizeof (LPT_V_DATA)},
  {&gEfiSioVariableGuid,              L"PNP0400_0_NV",               sizeof (LPT_NV_DATA)},
#ifdef SIO_NAT87393
  {&gEfiSioVariableGuid,              L"PNP0604_0_VV",               sizeof (NAT_FDC_V_DATA)},
  {&gEfiSioVariableGuid,              L"PNP0604_0_NV",               sizeof (NAT_FDC_NV_DATA)},
  {&gEfiSioVariableGuid,              L"PNP0501_2_VV",               sizeof (NAT_COMA_V_DATA)},
  {&gEfiSioVariableGuid,              L"PNP0501_2_NV",               sizeof (NAT_COMA_NV_DATA)},
  {&gEfiSioVariableGuid,              L"PNP0400_1_VV",               sizeof (NAT_LPT_V_DATA)},
  {&gEfiSioVariableGuid,              L"PNP0400_1_NV",               sizeof (NAT_LPT_NV_DATA)},
#endif
#ifdef SIO_WPCN381U
  {&gEfiSioVariableGuid,              L"PNP0604_2_VV",               sizeof (WPCN381U_FDC_V_DATA)},
  {&gEfiSioVariableGuid,              L"PNP0604_2_NV",               sizeof (WPCN381U_FDC_NV_DATA)},
  {&gEfiSioVariableGuid,              L"PNP0501_3_VV",               sizeof (WPCN381U_COMA_V_DATA)},
  {&gEfiSioVariableGuid,              L"PNP0501_3_NV",               sizeof (WPCN381U_COMA_NV_DATA)},
  {&gEfiSioVariableGuid,              L"PNP0501_4_VV",               sizeof (WPCN381U_COMB_V_DATA)},
  {&gEfiSioVariableGuid,              L"PNP0501_4_NV",               sizeof (WPCN381U_COMB_NV_DATA)},
  {&gEfiSioVariableGuid,              L"PNP0400_2_VV",               sizeof (WPCN381U_LPT_V_DATA)},
  {&gEfiSioVariableGuid,              L"PNP0400_2_NV",               sizeof (WPCN381U_LPT_NV_DATA)},
#endif
  {&gIccGuid,            L"IccAdvancedSetupDataVar", sizeof (ICC_SETUP_DATA)},
};

STATIC SETUP_HII_VARIABLE_INFO  mBootFormVar[] = {
  {&gSetupVariableGuid,           L"Setup",             sizeof (SETUP_DATA)},
  {&gSetupVariableGuid,           L"BootTime",          sizeof (BOOTTIME_SETUP)},
  {&gEfiGlobalVariableGuid,       L"SecureBoot",        sizeof (SECURE_BOOT_VARIABLE)},
  {&gSetupVariableGuid,           L"SetupVolatileData", sizeof (SETUP_VOLATILE_DATA)},
};

typedef struct {
  EFI_GUID                 FormGuid;
} SETUP_FORM_LIST;


STATIC SETUP_FORM_LIST    mSetupFormList[] = {
  {MAIN_FORM_SET_GUID},
  {ADVANCED_FORM_SET_GUID},
  {BOOT_FORM_SET_GUID},
  {{0}},                          // fake guid
};


VOID IccExtractConfig (
  VOID
  );
VOID MeExtractConfig (
  VOID
  );
VOID IccRouteConfig (
  VOID
  );
VOID MeRouteConfig (
  VOID
  );

extern EFI_HII_DATABASE_PROTOCOL *gIfrLibHiiDatabase;
extern EFI_HII_STRING_PROTOCOL   *gIfrLibHiiString;

#define EfiStrStr StrStr
#define GetSupportedLanguages HiiGetSupportedLanguages

//
// Limited buffer size recommended by RFC4646 (4.3.  Length Considerations)
// (42 characters plus a NULL terminator)
//
#define RFC_3066_ENTRY_SIZE             (42 + 1)

//
//  CONTAINING_RECORD - returns a pointer to the structure
//      from one of it's elements.
//
#define _CR(Record, TYPE, Field)  ((TYPE *) ((CHAR8 *) (Record) - (CHAR8 *) &(((TYPE *) 0)->Field)))
/**
  Append a string to a multi-string format.

  This is a internal function.

  @param  MultiString            String in <MultiConfigRequest>,
                                 <MultiConfigAltResp>, or <MultiConfigResp>. On
                                 input, the buffer length of  this string is
                                 MAX_STRING_LENGTH. On output, the  buffer length
                                 might be updated.
  @param  AppendString           NULL-terminated Unicode string.

  @retval EFI_INVALID_PARAMETER  Any incoming parameter is invalid.
  @retval EFI_SUCCESS            AppendString is append to the end of MultiString

**/
EFI_STATUS
AppendToMultiString (
  IN OUT EFI_STRING                *MultiString,
  IN EFI_STRING                    AppendString
  )
{
  UINTN AppendStringSize;
  UINTN MultiStringSize;
  UINTN MaxLen;

  if (MultiString == NULL || *MultiString == NULL || AppendString == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  AppendStringSize = StrSize (AppendString);
  MultiStringSize  = StrSize (*MultiString);
  MaxLen = MAX_STRING_LENGTH / sizeof (CHAR16);

  //
  // Enlarge the buffer each time when length exceeds MAX_STRING_LENGTH.
  //
  if (MultiStringSize + AppendStringSize > MAX_STRING_LENGTH ||
      MultiStringSize > MAX_STRING_LENGTH) {
    *MultiString = (EFI_STRING) ReallocatePool (
                                  MultiStringSize,
                                  MultiStringSize + AppendStringSize,
                                  (VOID *) (*MultiString)
                                  );
    MaxLen = (MultiStringSize + AppendStringSize) / sizeof (CHAR16);
    ASSERT (*MultiString != NULL);
  }
  //
  // Append the incoming string
  //
  StrCatS (*MultiString, MaxLen, AppendString);

  return EFI_SUCCESS;
}

/**
  Find current Form and return table size

  @param[in]   FormSetGuid     - Guid from This
  @param[out]  FormHiiVarInfo  - Point of Setup form variable table

  @return - Current Variable table size
          - 0 is not match any Setup Form Guid

**/
UINTN
SwitchSetupForm (
  IN   EFI_GUID                  FormSetGuid,
  OUT  SETUP_HII_VARIABLE_INFO   **FormHiiVarInfo
  )
{
  UINT8          Index;
  UINT8          FormSetTableSize;
  UINTN          VarTableInfoSize;

  FormSetTableSize = sizeof (mSetupFormList) / sizeof (EFI_GUID);

  for (Index = 0;Index < FormSetTableSize;Index++) {
    if (CompareGuid (&FormSetGuid, &mSetupFormList[Index].FormGuid)) {
      break;
    }
  }
  switch (Index) {
    case 0:
      *FormHiiVarInfo = mMainFormVar;
      VarTableInfoSize = sizeof (mMainFormVar) / sizeof (SETUP_HII_VARIABLE_INFO);
      break;

    case 1:
      *FormHiiVarInfo = mAdvancedFormVar;
      VarTableInfoSize = sizeof (mAdvancedFormVar) / sizeof (SETUP_HII_VARIABLE_INFO);
      break;

    case 2:
      *FormHiiVarInfo = mBootFormVar;
      VarTableInfoSize = sizeof (mBootFormVar) / sizeof (SETUP_HII_VARIABLE_INFO);
      break;

    default:
      FormHiiVarInfo = NULL;
      VarTableInfoSize = 0;
      break;
  }

  return VarTableInfoSize;
}

/**
  Determines if a Unicode character is a hexadecimal digit.
  The test is case insensitive.

  @param[out] Digit - Pointer to byte that receives the value of the hex character.
  @param[in]  Char  - Unicode character to test.

  @return TRUE  - If the character is a hexadecimal digit.
  @return FALSE - Otherwise.

**/
BOOLEAN
IsHexDigit (
  OUT UINT8      *Digit,
  IN  CHAR16      Char
  )
{
  if ((Char >= L'0') && (Char <= L'9')) {
    *Digit = (UINT8) (Char - L'0');
    return TRUE;
  }

  if ((Char >= L'A') && (Char <= L'F')) {
    *Digit = (UINT8) (Char - L'A' + 0x0A);
    return TRUE;
  }

  if ((Char >= L'a') && (Char <= L'f')) {
    *Digit = (UINT8) (Char - L'a' + 0x0A);
    return TRUE;
  }

  return FALSE;
}

/**
  Converts Unicode string to binary buffer.
  The conversion may be partial.
  The first character in the string that is not hex digit stops the conversion.
  At a minimum, any blob of data could be represented as a hex string.

  @param[in, out] Buf         - Pointer to buffer that receives the data.
  @param[in, out] Len         - Length in bytes of the buffer to hold converted data.
                                If routine return with EFI_SUCCESS, containing length of converted data.
                                If routine return with EFI_BUFFER_TOO_SMALL, containing length of buffer desired.
  @param[in] Str              - String to be converted from.
  @param[out] ConvertedStrLen - Length of the Hex String consumed.

  @return EFI_SUCCESS: Routine Success.
  @return EFI_BUFFER_TOO_SMALL: The buffer is too small to hold converted data.

**/
EFI_STATUS
HexStringToBuf (
  IN OUT UINT8                     *Buf,
  IN OUT UINTN                    *Len,
  IN     CHAR16                    *Str,
  OUT    UINTN                     *ConvertedStrLen  OPTIONAL
  )
{
  UINTN       HexCnt;
  UINTN       Idx;
  UINTN       BufferLength;
  UINT8       Digit;
  UINT8       Byte;

  //
  // Find out how many hex characters the string has.
  //
  for (Idx = 0, HexCnt = 0; IsHexDigit (&Digit, Str[Idx]); Idx++, HexCnt++);

  if (HexCnt == 0) {
    *Len = 0;
    return EFI_SUCCESS;
  }
  //
  // Two Unicode characters make up 1 buffer byte. Round up.
  //
  BufferLength = (HexCnt + 1) / 2;

  //
  // Test if  buffer is passed enough.
  //
  if (BufferLength > (*Len)) {
    *Len = BufferLength;
    return EFI_BUFFER_TOO_SMALL;
  }

  *Len = BufferLength;

  for (Idx = 0; Idx < HexCnt; Idx++) {

    IsHexDigit (&Digit, Str[HexCnt - 1 - Idx]);

    //
    // For odd charaters, write the lower nibble for each buffer byte,
    // and for even characters, the upper nibble.
    //
    if ((Idx & 1) == 0) {
      Byte = Digit;
    } else {
      Byte = Buf[Idx / 2];
      Byte &= 0x0F;
      Byte |= Digit << 4;
    }

    Buf[Idx / 2] = Byte;
  }

  if (ConvertedStrLen != NULL) {
    *ConvertedStrLen = HexCnt;
  }

  return EFI_SUCCESS;
}

/**
  Swap bytes in the buffer.

  @param[in, out] Buffer     -  Binary buffer.
  @param[in] BufferSize      -  Size of the buffer in bytes.

**/
STATIC
VOID
SwapBuffer (
  IN OUT UINT8     *Buffer,
  IN UINTN         BufferSize
  )
{
  UINTN  Index;
  UINT8  Temp;
  UINTN  SwapCount;

  SwapCount = BufferSize / 2;
  for (Index = 0; Index < SwapCount; Index++) {
    Temp = Buffer[Index];
    Buffer[Index] = Buffer[BufferSize - 1 - Index];
    Buffer[BufferSize - 1 - Index] = Temp;
  }
}

/**
  Converts Hex String to binary buffer in reversed byte order from HexStringToBuf().

  @param[in, out] Buffer     - Pointer to buffer that receives the data.
  @param[in, out] BufferSize - Length in bytes of the buffer to hold converted data.
                               If routine return with EFI_SUCCESS, containing length of converted data.
                               If routine return with EFI_BUFFER_TOO_SMALL, containing length of buffer desired.
  @param[in] Str             - String to be converted from.

  @return EFI_SUCCESS        - The function completed successfully.

**/
EFI_STATUS
HexStringToBuffer (
  IN OUT UINT8         *Buffer,
  IN OUT UINTN         *BufferSize,
  IN CHAR16            *Str
  )
{
  EFI_STATUS  Status;
  UINTN       ConvertedStrLen;

  ConvertedStrLen = 0;
  Status = HexStringToBuf (Buffer, BufferSize, Str, &ConvertedStrLen);
  if (!EFI_ERROR (Status)) {
    SwapBuffer (Buffer, (ConvertedStrLen + 1) / 2);
  }

  return Status;
}

/**
  Convert binary representation Config string (e.g. "0041004200430044") to the
  original string (e.g. "ABCD"). Config string appears in <ConfigHdr> (i.e.
  "&NAME=<string>"), or Name/Value pair in <ConfigBody> (i.e. "label=<string>").

  @param[in, out] UnicodeString - Original Unicode string.
  @param[in, out] StrBufferLen  - On input: Length in bytes of buffer to hold the Unicode string.
                                  Includes tailing '\0' character.
                                  On output:
                                    If return EFI_SUCCESS, containing length of Unicode string buffer.
                                    If return EFI_BUFFER_TOO_SMALL, containing length of string buffer desired.
  @param[in] ConfigString       - Binary representation of Unicode String, <string> := (<HexCh>4)+

  @return EFI_SUCCESS          - Routine success.
  @return EFI_BUFFER_TOO_SMALL - The string buffer is too small.

**/
EFI_STATUS
ConfigStringToUnicode (
  IN OUT CHAR16                *UnicodeString,
  IN OUT UINTN                 *StrBufferLen,
  IN CHAR16                    *ConfigString
  )
{
  UINTN       Index;
  UINTN       Len;
  UINTN       BufferSize;
  CHAR16      BackupChar;

  Len = StrLen (ConfigString) / 4;
  BufferSize = (Len + 1) * sizeof (CHAR16);

  if (*StrBufferLen < BufferSize) {
    *StrBufferLen = BufferSize;
    return EFI_BUFFER_TOO_SMALL;
  }

  *StrBufferLen = BufferSize;

  for (Index = 0; Index < Len; Index++) {
    BackupChar = ConfigString[4];
    ConfigString[4] = L'\0';

    HexStringToBuf ((UINT8 *) UnicodeString, &BufferSize, ConfigString, NULL);

    ConfigString[4] = BackupChar;

    ConfigString += 4;
    UnicodeString += 1;
  }

  //
  // Add tailing '\0' character
  //
  *UnicodeString = L'\0';

  return EFI_SUCCESS;
}

/**
  Converts the low nibble of a byte  to hex unicode character.

  @param[in] Nibble - lower nibble of a byte.

  Hex unicode character.

**/
CHAR16
NibbleToHexChar (
  IN UINT8      Nibble
  )
{
  Nibble &= 0x0F;
  if (Nibble <= 0x9) {
    return (CHAR16)(Nibble + L'0');
  }

  return (CHAR16)(Nibble - 0xA + L'A');
}

/**
  Converts binary buffer to Unicode string.
  At a minimum, any blob of data could be represented as a hex string.

  @param[in, out] Str                   - Pointer to the string.
  @param[in, out] HexStringBufferLength - Length in bytes of buffer to hold the hex string. Includes tailing '\0' character.
                                            If routine return with EFI_SUCCESS, containing length of hex string buffer.
                                            If routine return with EFI_BUFFER_TOO_SMALL, containing length of hex string buffer desired.
  @param[in] Buf                        - Buffer to be converted from.
  @param[in] Len                        - Length in bytes of the buffer to be converted.

  @return EFI_SUCCESS: Routine success.
  @return EFI_BUFFER_TOO_SMALL: The hex string buffer is too small.

**/
EFI_STATUS
BufToHexString (
  IN OUT CHAR16                    *Str,
  IN OUT UINTN                     *HexStringBufferLength,
  IN     UINT8                     *Buf,
  IN     UINTN                      Len
  )
{
  UINTN       Idx;
  UINT8       Byte;
  UINTN       StrLen;

  //
  // Make sure string is either passed or allocate enough.
  // It takes 2 Unicode characters (4 bytes) to represent 1 byte of the binary buffer.
  // Plus the Unicode termination character.
  //
  StrLen = Len * 2;
  if (StrLen > ((*HexStringBufferLength) - 1)) {
    *HexStringBufferLength = StrLen + 1;
    return EFI_BUFFER_TOO_SMALL;
  }

  *HexStringBufferLength = StrLen + 1;
  //
  // Ends the string.
  //
  Str[StrLen] = L'\0';

  for (Idx = 0; Idx < Len; Idx++) {

    Byte = Buf[Idx];
    Str[StrLen - 1 - Idx * 2] = NibbleToHexChar (Byte);
    Str[StrLen - 2 - Idx * 2] = NibbleToHexChar ((UINT8)(Byte >> 4));
  }

  return EFI_SUCCESS;
}

BOOLEAN
IsVariableNameInSupportedList (
  IN CHAR16 *VariableName
  )
{
  UINTN Index;

  if (VariableName == NULL) {
    return FALSE;
  }

  for (Index = 0; VarStoreVariableNameList[Index] != NULL; Index ++) {
    if (StrCmp (VarStoreVariableNameList[Index], VariableName) == 0) {
      return TRUE;
    }
  }

  return FALSE;
}

EFI_STATUS
EFIAPI
ExtractGuidNameFromConfigHdr (
  IN CONST EFI_STRING  ConfigHdr,
  OUT      EFI_GUID    *Guid,
  OUT      CHAR16      **Name,
  OUT      UINTN       *Offset
  )
{
  EFI_STATUS     Status;
  CHAR16         *StrPtr;
  UINTN          Length;
  CHAR16         *StrName;

  *Offset = 0;
  //
  // <ConfigHdr> ::=
  // GUID=<HexCh>32&NAME=<Char>NameStrLen&PATH=<HexChar>DevicePathStrLen <NULL>
  // | 5  |   32   |  6  |  NameStrLen*4 |  6  |    DevicePathStrLen    | 1 |
  //
  if (StrLen (ConfigHdr) <= (5 + 32 + 6)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // check syntax for guid
  //
  StrPtr = ConfigHdr + 5 + 32;
  if (*StrPtr != L'&') {
    return EFI_INVALID_PARAMETER;
  }
  *StrPtr = L'\0';

  //
  // extract guid
  //
  Length = sizeof (EFI_GUID);
  Status = HexStringToBuffer (
             (UINT8 *) Guid,
             &Length,
             ConfigHdr + 5
             );
  *StrPtr = L'&';
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "HII Config Access: Guid extraction failed\n"));
    *Offset = 5;
    return EFI_INVALID_PARAMETER;
  }

  //
  // check syntax for name
  //
  StrPtr = ConfigHdr + 5 + 32 + 6;
  while (*StrPtr != L'\0' && *StrPtr != L'&') {
    StrPtr++;
  }
  if (*StrPtr != L'&') {
    *Offset = 5 + 32 + 6;
    return EFI_INVALID_PARAMETER;
  }

  //
  // extract name
  //
  *StrPtr = L'\0';
  Length = StrSize (ConfigHdr + 5 + 32 + 6);
  StrName = AllocatePool (Length);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = ConfigStringToUnicode (
             StrName,
             &Length,
             ConfigHdr + 5 + 32 + 6
             );
  *StrPtr = L'&';
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "HII Config Access: Name extraction failed\n"));
    *Offset = 5 + 32 + 6;
    return EFI_INVALID_PARAMETER;
  }

  if (!IsVariableNameInSupportedList (StrName)) {
    return EFI_NOT_FOUND;
  }
  *Name = StrName;

  return EFI_SUCCESS;
}

/**
  Make Result data and Progress with all setup data

  @param[in]       HiiVarInfo        Point of current form variable table.
  @param[in]       SetupTableSize    Table size
  @param[in]       DriverHandle      Current driver handle.
  @param[out]      ReturnData        Result data.
  @param[out]      DriverHandle      Progress data.

  @return EFI_SUCCESS                Routine done.
  @retval EFI_OUT_OF_RESOURCES       Unable to allocate required resources or DriverHandle incorrect.
  @retval Other                      If return other EFI_ERROR code, should be BlockToConfig () handle.
**/
EFI_STATUS
MakeResultData (
  IN  SETUP_HII_VARIABLE_INFO  *HiiVarInfo,
  IN  UINTN                    SetupTableSize,
  IN  EFI_HANDLE               DriverHandle,
  OUT EFI_STRING               *ReturnData,
  OUT EFI_STRING               *ReturnProgress
  )
{
  EFI_STATUS                   Status;
  UINTN                        Index;
  UINT8                        *Var;
  BOOLEAN                      HasSomething;
  UINTN                        Length;
  UINTN                        MaxLen;
  EFI_STRING                   ConfigRequest;
  EFI_STRING                   ConfigRequestHdr;
  EFI_STRING                   TempResults;


  HasSomething     = FALSE;
  TempResults      = NULL;
  *ReturnData = (EFI_STRING) AllocateZeroPool (MAX_STRING_LENGTH);
  if (*ReturnData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Request has no request element, construct full request string.
  // Allocate and fill a buffer large enough to hold the <ConfigHdr> template
  // followed by "&OFFSET=0&WIDTH=WWWWWWWWWWWWWWWW" followed by a Null-terminator.
  //

  for (Index = 0;Index < SetupTableSize;Index++) {
    //
    // Get Variable
    //
    DEBUG ((DEBUG_INFO, "Request value for Storage %s\n", HiiVarInfo[Index].VarName));
    Length  = 0;
    Var     = NULL;
    Status  = gRT->GetVariable (HiiVarInfo[Index].VarName, HiiVarInfo[Index].VarGuid, NULL, &Length, Var);
    if (Status == EFI_BUFFER_TOO_SMALL) {
      Var = AllocateZeroPool (Length);
      ASSERT (Var != NULL);
      Status  = gRT->GetVariable (HiiVarInfo[Index].VarName, HiiVarInfo[Index].VarGuid, NULL, &Length, Var);
    }
    if (EFI_ERROR (Status)) {
      //
      // If GetVariable failure skip it
      //
      if (Var != NULL) {
        FreePool (Var);
      }
      continue;
    }
    if (HasSomething) {
      AppendToMultiString (ReturnData, L"&");
    }
    ConfigRequestHdr = HiiConstructConfigHdr (HiiVarInfo[Index].VarGuid, HiiVarInfo[Index].VarName, DriverHandle);
    if (ConfigRequestHdr == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    MaxLen = (StrLen (ConfigRequestHdr) + 32 + 1) * sizeof (CHAR16);
    ConfigRequest = AllocateZeroPool (MaxLen);
    UnicodeSPrint (ConfigRequest, MaxLen, L"%s&OFFSET=0&WIDTH=%016LX", ConfigRequestHdr, HiiVarInfo[Index].VarSize);
    Status  = mConfigRouting->BlockToConfig (mConfigRouting, ConfigRequest, Var, Length, &TempResults, ReturnProgress);
    if (EFI_ERROR (Status)) {
      if (TempResults != NULL) {
        FreePool (TempResults);
      }
      if (ConfigRequest != NULL) {
        FreePool (ConfigRequest);
      }
      return Status;
    }
    AppendToMultiString (ReturnData, TempResults);
    if (Status == EFI_SUCCESS) {
      HasSomething = TRUE;
    }
    FreePool (TempResults);
    FreePool (ConfigRequestHdr);
    FreePool (ConfigRequest);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
HiiAccessExtractConfig (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN CONST EFI_STRING                     Request,
  OUT EFI_STRING                          *Progress,
  OUT EFI_STRING                          *Results
  )
{
  EFI_STATUS                      Status;
  UINTN                           Length;
  EFI_GUID                        Guid;
  CHAR16                          *Name;
  UINT8                           *Var;
  UINTN                           Offset;
  EFI_HANDLE                      DriverHandle;
  EFI_HII_HANDLE                  HiiHandle;
  EFI_STRING                      ConfigRequest;
  BOOLEAN                         AllocatedRequest;
  EFI_STRING                      ConfigRequestHdr;
  UINTN                           Index;
  UINTN                           SetupTableSize;
  UINTN                           MatchSetupTable;
  EFI_GUID                        SetupFromGuid;
  SETUP_HII_VARIABLE_INFO         *CurrentVarTable;
  UINTN                           ConfigHdrLength;

  if (Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  HiiHandle        = NULL;
  ConfigRequest    = Request;
  *Progress        = Request;
  DriverHandle     = NULL;
  AllocatedRequest = FALSE;
  MatchSetupTable  = 0;
  CurrentVarTable  = NULL;
  ConfigHdrLength  = 0;

  HiiHandle = _CR (This, CALLBACK_INFO, pFormCallback)->HiiHandle;
  SetupFromGuid = _CR (This, CALLBACK_INFO, pFormCallback)->pGuid;
  SetupTableSize = SwitchSetupForm (SetupFromGuid, &CurrentVarTable);
  if (SetupTableSize == 0) {
    return EFI_NOT_FOUND;
  }

  if (HiiHandle != NULL) {
    if (NULL == gIfrLibHiiDatabase) {
      Status = gBS->LocateProtocol (
                  &gEfiHiiDatabaseProtocolGuid,
                  NULL,
                  (VOID **) &gIfrLibHiiDatabase
                  );
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
    Status = gIfrLibHiiDatabase->GetPackageListHandle(gIfrLibHiiDatabase, HiiHandle, &DriverHandle);
  }
  if (DriverHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Request == NULL) {
    //
    // Request is NULL
    //
    Status = MakeResultData (CurrentVarTable, SetupTableSize, DriverHandle, Results, Progress);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    //
    // Set Progress string to the original request string or the string's null terminator.
    //
    if (Request == NULL) {
      *Progress = NULL;
    } else if (StrStr (Request, L"OFFSET") == NULL) {
      *Progress = Request + StrLen (Request);
    }
    return Status;
  } else {
    //
    // Request is not NULL
    //
    Status = ExtractGuidNameFromConfigHdr (ConfigRequest, &Guid, &Name, &Offset);
    if (EFI_ERROR (Status)) {
      *Progress = ConfigRequest + Offset;
      return Status;
    }
    if (StrCmp (L"IccAdvancedSetupDataVar", Name) == 0) {
      IccExtractConfig();
    }
    if (StrCmp (L"MeSetupStorage", Name) == 0) {
      MeExtractConfig();
    }
    //
    // get variable by name and guid from nvram
    //
    Length  = 0;
    Var     = NULL;
    Status  = gRT->GetVariable (Name, &Guid, NULL, &Length, Var);
    if (Status == EFI_BUFFER_TOO_SMALL) {
      Var = AllocateZeroPool (Length);
      ASSERT (Var != NULL);
      Status  = gRT->GetVariable (Name, &Guid, NULL, &Length, Var);
    }
    if (EFI_ERROR (Status)) {
      if (Var != NULL) {
        FreePool (Var);
      }
      *Progress = Request;
      FreePool (Name);
      return Status;
    }
    //
    // Check whether Request includes Request Element.
    //
    if (EfiStrStr (ConfigRequest, L"OFFSET") == NULL) {
      //
      // MatchSetupTable:
      // 0: not match
      // 1: Guid match, Name not match
      // 2: all match
      //
      for (Index = 0;Index < SetupTableSize;Index++) {
        MatchSetupTable = 0;
        if (CompareGuid (&Guid, CurrentVarTable[Index].VarGuid)) {
          MatchSetupTable++;
          if (StrCmp (Name, CurrentVarTable[Index].VarName) == 0) {
            MatchSetupTable++;
            break;
          }
        }
      }
      if (MatchSetupTable != 2) {
        return EFI_NOT_FOUND;
      }
      ConfigRequestHdr = HiiConstructConfigHdr (CurrentVarTable[Index].VarGuid, CurrentVarTable[Index].VarName, DriverHandle);
      if (ConfigRequestHdr == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      ConfigHdrLength = (StrLen (ConfigRequestHdr) + 32 + 1) * sizeof (CHAR16);
      ConfigRequest = AllocatePool (Length);
      AllocatedRequest = TRUE;
      if (ConfigRequest == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      UnicodeSPrint (ConfigRequest, ConfigHdrLength, L"%s&OFFSET=0&WIDTH=%016LX", ConfigRequestHdr, CurrentVarTable[Index].VarSize);
    }
  }

  DEBUG ((DEBUG_INFO, "Request value for Storage1 %s\n", Name));

  Status  = mConfigRouting->BlockToConfig (mConfigRouting, ConfigRequest, Var, Length, Results, Progress);


  FreePool (Name);
  if (Var != NULL) {
    FreePool (Var);
  }

  if (AllocatedRequest) {
    FreePool (ConfigRequest);
    //
    // Set Progress string to the original request string.
    //
    if (Request == NULL) {
      *Progress = NULL;
    } else {
      *Progress = Request + StrLen (Request);
    }
  }

  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
HiiAccessRouteConfig (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN CONST EFI_STRING                     Configuration,
  OUT EFI_STRING                          *Progress
  )
{
  EFI_STATUS                      Status;
  UINTN                           Length;
  UINTN                           VarSize;
  UINTN                           Offset;
  EFI_GUID                        Guid;
  CHAR16                          *Name;
  UINT8                           *Var;
  EFI_HANDLE                      Handle;
  UINT32                          Attributes;

  if (Progress == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  *Progress = Configuration;

  if (Configuration == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Attributes = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS;

  Status = ExtractGuidNameFromConfigHdr (Configuration, &Guid, &Name, &Offset);
  if (EFI_ERROR (Status)) {
    *Progress = Configuration + Offset;
    return Status;
  }

  //
  // get variable by name and guid from nvram
  //
  Var     = NULL;
  Length  = 0;
  Status  = gRT->GetVariable (Name, &Guid, &Attributes, &Length, Var);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    Var = AllocateZeroPool (Length);
    if (Var == NULL) {
      ASSERT (FALSE);
      return EFI_OUT_OF_RESOURCES;
    }
    Status  = gRT->GetVariable (Name, &Guid, &Attributes, &Length, Var);
  }
  if (EFI_ERROR (Status)) {
    //
    // requested variable not found
    //
    if (Var != NULL) {
      FreePool (Var);
    }
    Var = NULL;
    Length = 0;
    Status  = mConfigRouting->ConfigToBlock (mConfigRouting, Configuration, Var, &Length, Progress);
    if (Status == EFI_INVALID_PARAMETER) {
      Var = AllocateZeroPool (Length);
      if (Var == NULL) {
        ASSERT (FALSE);
        return EFI_OUT_OF_RESOURCES;
      }
    } else {
      return EFI_NOT_FOUND;
    }
  }

  VarSize = Length;
  Status  = mConfigRouting->ConfigToBlock (mConfigRouting, Configuration, Var, &Length, Progress);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    //
    // block not large enough
    //
    if (Var != NULL) {
      FreePool (Var);
    }

    VarSize = Length;
    Var  = AllocateZeroPool (Length);
    if (Var == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    Status  = mConfigRouting->ConfigToBlock (mConfigRouting, Configuration, Var, &Length, Progress);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  if (EFI_ERROR (Status)) {
    FreePool (Var);
    return Status;
  }

  gRT->SetVariable (
         Name,
         &Guid,
         Attributes,
         VarSize,
         Var
         );

  if (StrCmp (L"IccAdvancedSetupDataVar", Name) == 0) {
    IccRouteConfig();
  }

  if (StrCmp (L"MeSetupStorage", Name) == 0) {
    MeRouteConfig();
  }

  FreePool (Var);

  //
  // Trigger gSetupNvramUpdateGuid event when save value on UI.
  // The code is in Greens Glacier SetupBrowser driver but as we are using Tiano SetupBrowser driver, move
  // the code here.
  //
  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gSetupNvramUpdateGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->UninstallProtocolInterface (
                  Handle,
                  &gSetupNvramUpdateGuid,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
HiiAccessFormCallback (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_BROWSER_ACTION                   Action,
  IN EFI_QUESTION_ID                      QuestionId,
  IN UINT8                                Type,
  IN EFI_IFR_TYPE_VALUE                   *Value,
  OUT EFI_BROWSER_ACTION_REQUEST          *ActionRequest
  )
{
  return EFI_UNSUPPORTED;
}

EFI_HII_CONFIG_ACCESS_PROTOCOL DefaultConfigAccess = {
  HiiAccessExtractConfig,
  HiiAccessRouteConfig,
  HiiAccessFormCallback
};

VOID
EFIAPI
UpdateCallBack (
  CALLBACK_INFO *pCallBackFound
  )
{
  if (pCallBackFound->pFormCallback.ExtractConfig == NULL) {
    pCallBackFound->pFormCallback.ExtractConfig = HiiAccessExtractConfig;
  }
  if (pCallBackFound->pFormCallback.RouteConfig == NULL) {
    pCallBackFound->pFormCallback.RouteConfig = HiiAccessRouteConfig;
  }
  if (pCallBackFound->pFormCallback.Callback == NULL) {
    pCallBackFound->pFormCallback.Callback = HiiAccessFormCallback;
  }
  if ((pCallBackFound->pFormCallback.ExtractConfig == NULL) &&
      (pCallBackFound->pFormCallback.RouteConfig == NULL)   &&
      (pCallBackFound->pFormCallback.Callback == NULL)) {
    pCallBackFound->pFormCallback = DefaultConfigAccess;
  }
}

/**
  Returns a pointer to an allocated buffer that contains the best matching language
  from a set of supported languages.

  This function supports both ISO 639-2 and RFC 4646 language codes, but language
  code types may not be mixed in a single call to this function.  The language
  code returned is allocated using AllocatePool().  The caller is responsible for
  freeing the allocated buffer using FreePool().  This function supports a variable
  argument list that allows the caller to pass in a prioritized list of language
  codes to test against all the language codes in SupportedLanguages.

  If SupportedLanguages is NULL, then ASSERT()..

  @param[in] SupportedLanguages   -   A pointer to a Null-terminated ASCII string that
                                      contains a set of language codes in the format
                                      specified by Iso639Language.
  @param[in] Iso639Language       -   If TRUE, then all language codes are assumed to be
                                      in ISO 639-2 format.  If FALSE, then all language
                                      codes are assumed to be in RFC 4646 language format.
  ...                             -   A variable argument list that contains pointers to
                                      Null-terminated ASCII strings that contain one or more
                                      language codes in the format specified by Iso639Language.
                                      The first language code from each of these language
                                      code lists is used to determine if it is an exact or
                                      close match to any of the language codes in
                                      SupportedLanguages.  Close matches only apply to RFC 4646
                                      language codes, and the matching algorithm from RFC 4647
                                      is used to determine if a close match is present.  If
                                      an exact or close match is found, then the matching
                                      language code from SupportedLanguages is returned.  If
                                      no matches are found, then the next variable argument
                                      parameter is evaluated.  The variable argument list
                                      is terminated by a NULL

  @return NULL                    -   The best matching language could not be found in SupportedLanguages.
  @return NULL                    -   There are not enough resources available to return the best matching
                                      language.
  @return Other                   -   A pointer to a Null-terminated ASCII string that is the best matching
                                      language in SupportedLanguages.

**/
CHAR8 *
EFIAPI
HiiLibGetBestLanguage (
  IN CHAR8        *SupportedLanguages,
  IN BOOLEAN      Iso639Language,
  ...
  )
{
  VA_LIST      Args;
  CHAR8        *Language;
  UINTN        CompareLength;
  UINTN        LanguageLength;
  CHAR8        *Supported;
  CHAR8        *BestLanguage;

  ASSERT (SupportedLanguages != NULL);
  if (SupportedLanguages == NULL) {
    return NULL;
  }

  VA_START (Args, Iso639Language);
  while ((Language = VA_ARG (Args, CHAR8 *)) != NULL) {
    //
    // Default to ISO 639-2 mode
    //
    CompareLength  = 3;
    LanguageLength = AsciiStrLen (Language);
    if (LanguageLength > 3) {
      LanguageLength = 3;
    }

    //
    // If in RFC 4646 mode, then determine the length of the first RFC 4646 language code in Language
    //
    if (!Iso639Language) {
      for (LanguageLength = 0; Language[LanguageLength] != 0 && Language[LanguageLength] != ';'; LanguageLength++);
    }

    //
    // Trim back the length of Language used until it is empty
    //
    while (LanguageLength > 0) {
      //
      // Loop through all language codes in SupportedLanguages
      //
      for (Supported = SupportedLanguages; *Supported != '\0'; Supported += CompareLength) {
        //
        // In RFC 4646 mode, then Loop through all language codes in SupportedLanguages
        //
        if (!Iso639Language) {
          //
          // Skip ';' characters in Supported
          //
          for (; *Supported != '\0' && *Supported == ';'; Supported++);
          //
          // Determine the length of the next language code in Supported
          //
          for (CompareLength = 0; Supported[CompareLength] != 0 && Supported[CompareLength] != ';'; CompareLength++);
          //
          // If Language is longer than the Supported, then skip to the next language
          //
          if (LanguageLength > CompareLength) {
            continue;
          }
        }
        //
        // See if the first LanguageLength characters in Supported match Language
        //
        if (AsciiStrnCmp (Supported, Language, LanguageLength) == 0) {
          VA_END (Args);
          //
          // Allocate, copy, and return the best matching language code from SupportedLanguages
          //
          BestLanguage = AllocateZeroPool (CompareLength + 1);
          if (BestLanguage == NULL) {
            return NULL;
          }
          CopyMem (BestLanguage, Supported, CompareLength);
          return BestLanguage;
        }
      }

      if (Iso639Language) {
        //
        // If ISO 639 mode, then each language can only be tested once
        //
        LanguageLength = 0;
      } else {
        //
        // If RFC 4646 mode, then trim Language from the right to the next '-' character
        //
        for (LanguageLength--; LanguageLength > 0 && Language[LanguageLength] != '-'; LanguageLength--);
      }
    }
  }
  VA_END (Args);

  //
  // No matches were found
  //
  return NULL;
}

EFI_STATUS
HiiLibGetString (
  IN  EFI_HII_HANDLE                  PackageList,
  IN  EFI_STRING_ID                   StringId,
  OUT EFI_STRING                      String,
  IN  OUT UINTN                       *StringSize
  )
{
  CHAR8       PlatformLanguage[RFC_3066_ENTRY_SIZE];
  CHAR8       *SupportedLanguages;
  CHAR8       *BestLanguage;
  UINTN       Size;
  EFI_STATUS  Status;

  //
  // Get current language setting
  //
  Size = sizeof (PlatformLanguage);
  Status = gRT->GetVariable (
                  L"PlatformLang",
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &Size,
                  PlatformLanguage
                  );
  if (EFI_ERROR (Status)) {
    AsciiStrCpyS (PlatformLanguage, sizeof (PlatformLanguage) / sizeof (CHAR8), (CHAR8 *) "en-US");
  }

  SupportedLanguages = GetSupportedLanguages (PackageList);
  if (SupportedLanguages == NULL) {
    //
    // No supported language.
    //
    return EFI_NOT_FOUND;
  }

  //
  // Get the best matching language from SupportedLanguages
  //
  BestLanguage = HiiLibGetBestLanguage (
                   SupportedLanguages,
                   FALSE,                                             // RFC 4646 mode
                   PlatformLanguage,                                  // Highest priority
                   SupportedLanguages,                                // Lowest priority
                   NULL
                   );

  if (NULL == gIfrLibHiiString) {
    Status = gBS->LocateProtocol (
                    &gEfiHiiStringProtocolGuid,
                    NULL,
                    (VOID **) &gIfrLibHiiString
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  Status = gIfrLibHiiString->GetString (
                               gIfrLibHiiString,
                               BestLanguage,
                               PackageList,
                               StringId,
                               String,
                               StringSize,
                               NULL
                               );

  FreePool (SupportedLanguages);
  FreePool (BestLanguage);

  return Status;
}
#endif // AMI_OVERRIDE_END - For our Setup menu design.

/**
  Construct Request String (L"&OFFSET=%x&WIDTH=%x") base on the input Offset and Width.
  If the input RequestString is not NULL, new request will be cat at the end of it. The full
  request string will be constructed and return. Caller is responsible to free it.

  @param[in] RequestString   Current request string.
  @param[in] Offset          Offset of data in Storage.
  @param[in] Width           Width of data.

  @return String             Request string with input Offset and Width.
**/
EFI_STRING
EFIAPI
HiiConstructRequestString (
  IN EFI_STRING      RequestString, OPTIONAL
  IN UINTN           Offset,
  IN UINTN           Width
  )
{
  CHAR16             RequestElement[30];
  UINTN              StringLength;
  EFI_STRING         NewString;

  StringLength = UnicodeSPrint (
                   RequestElement,
                   sizeof (RequestElement),
                   L"&OFFSET=%x&WIDTH=%x",
                   Offset,
                   Width
                   );
  if (RequestString != NULL) {
    StringLength = StringLength + StrLen (RequestString);
  }
  NewString = AllocateZeroPool ((StringLength + 1) * sizeof (CHAR16));
  if (NewString == NULL) {
    return NULL;
  }
  if (RequestString != NULL) {
    StrCatS (NewString, ((StringLength + 1) * sizeof (CHAR16)) / sizeof (CHAR16), RequestString);
    FreePool (RequestString);
  }
  StrCatS (NewString, ((StringLength + 1) * sizeof (CHAR16)) / sizeof (CHAR16), RequestElement);
  return NewString;
}
