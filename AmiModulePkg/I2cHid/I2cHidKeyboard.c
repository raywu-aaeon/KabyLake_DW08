//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file I2cHidKeyboard.c
    AMI I2C Keyboard support implementation

**/

//---------------------------------------------------------------------------

#include <Efi.h>
#include <Library/DebugLib.h>
#include <Protocol/DriverBinding.h>
#include <I2cHid.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/SimpleTextInEx.h>
#include <Protocol/AmiKeycode.h>
#include <Library/UefiBootServicesTableLib.h>

//---------------------------------------------------------------------------

HID_DEV_INFO *SimpHidDevInfo;

extern EFI_GUID  gEfiSimpleTextInProtocolGuid;

EFI_INPUT_KEY   I2cKeyData;
UINT16          EfiKeyData;
EFI_HANDLE      InstallHandle = NULL;
UINT8           ShiftFlag=0;
UINT8           AltFlag=0;
UINT8           CtrlFlag=0;
UINT8           LocksLEDbyte=0;
UINT8           HidKeyoffset = 0;
UINT8           HidkeyRepeattimerFlg = 0;
UINT8           ReadData[HID_KEYBOARD_LENGTH] = {0};
UINT8           PreReadData[HID_KEYBOARD_LENGTH] = {0};
UINT8           ChangedFlg = 0;
#define CHANGE_TRUE     0x55
#define CHANGE_CLEAR    0
#define REPEAT_ENABLE   0xAA
#define REPEAT_DISABLE  0
#define SHIFT_KEY		3
EFI_EVENT HidRepeatKeyEvent;
EFI_EVENT HidGetKeyEvent;

HID_CODE_STRUCT ConvertKeyboardHidToEfiKey[] = {
    {0x04,                  SCAN_NULL,          L'a',       L'A'},
    {0x05,                  SCAN_NULL,          L'b',       L'B'},
    {0x06,                  SCAN_NULL,          L'c',       L'C'},
    {0x07,                  SCAN_NULL,          L'd',       L'D'},
    {0x08,                  SCAN_NULL,          L'e',       L'E'},
    {0x09,                  SCAN_NULL,          L'f',       L'F'},
    {0x0A,                  SCAN_NULL,          L'g',       L'G'},
    {0x0B,                  SCAN_NULL,          L'h',       L'H'},
    {0x0C,                  SCAN_NULL,          L'i',       L'I'},
    {0x0D,                  SCAN_NULL,          L'j',       L'J'},
    {0x0E,                  SCAN_NULL,          L'k',       L'K'},
    {0x0F,                  SCAN_NULL,          L'l',       L'L'},
    {0x10,                  SCAN_NULL,          L'm',       L'M'},
    {0x11,                  SCAN_NULL,          L'n',       L'N'},
    {0x12,                  SCAN_NULL,          L'o',       L'O'},
    {0x13,                  SCAN_NULL,          L'p',       L'P'},
    {0x14,                  SCAN_NULL,          L'q',       L'Q'},
    {0x15,                  SCAN_NULL,          L'r',       L'R'},
    {0x16,                  SCAN_NULL,          L's',       L'S'},
    {0x17,                  SCAN_NULL,          L't',       L'T'},
    {0x18,                  SCAN_NULL,          L'u',       L'U'},
    {0x19,                  SCAN_NULL,          L'v',       L'V'},
    {0x1A,                  SCAN_NULL,          L'w',       L'W'},
    {0x1B,                  SCAN_NULL,          L'x',       L'X'},
    {0x1C,                  SCAN_NULL,          L'y',       L'Y'},
    {0x1D,                  SCAN_NULL,          L'z',       L'Z'},
    {0x1E,                  SCAN_NULL,          L'1',       L'!'},
    {0x1F,                  SCAN_NULL,          L'2',       L'@'},
    {0x20,                  SCAN_NULL,          L'3',       L'#'},
    {0x21,                  SCAN_NULL,          L'4',       L'$'},
    {0x22,                  SCAN_NULL,          L'5',       L'%'},
    {0x23,                  SCAN_NULL,          L'6',       L'^'},
    {0x24,                  SCAN_NULL,          L'7',       L'&'},
    {0x25,                  SCAN_NULL,          L'8',       L'*'},
    {0x26,                  SCAN_NULL,          L'9',       L'('},
    {0x27,                  SCAN_NULL,          L'0',       L')'},
    {0x28,                  SCAN_NULL,          0x000d,     0x000d},   // Enter
    {0x29,                  SCAN_ESC,           0x0000,     0x0000},   // Escape
    {0x2A,                  SCAN_NULL,          0x0008,     0x0008},   // BackSpace
    {0x2B,                  SCAN_NULL,          0x0009,     0x0009},   // Tab
    {0x2C,                  SCAN_NULL,          L' ',       L' '},     // Space
    {0x2D,                  SCAN_NULL,          L'-',       L'_'},
    {0x2E,                  SCAN_NULL,          L'=',       L'+'},
    {0x2F,                  SCAN_NULL,          L'[',       L'{'},
    {0x30,                  SCAN_NULL,          L']',       L'}'},
    {0x31,                  SCAN_NULL,          L'\\',      L'|'},
    {0x33,                  SCAN_NULL,          L';',       L':'},
    {0x34,                  SCAN_NULL,          L'\'',      L'"'},
    {0x35,                  SCAN_NULL,          L'`',       L'~'},
    {0x36,                  SCAN_NULL,          L',',       L'<'},
    {0x37,                  SCAN_NULL,          L'.',       L'>'},
    {0x38,                  SCAN_NULL,          L'/',       L'?'},
    {0x39,                  SCAN_NULL,          0x0000,     0x0000},   // CapsLock
    {0x3A,                  SCAN_F1,            0x0000,     0x0000},
    {0x3B,                  SCAN_F2,            0x0000,     0x0000},
    {0x3C,                  SCAN_F3,            0x0000,     0x0000},
    {0x3D,                  SCAN_F4,            0x0000,     0x0000},
    {0x3E,                  SCAN_F5,            0x0000,     0x0000},
    {0x3F,                  SCAN_F6,            0x0000,     0x0000},
    {0x40,                  SCAN_F7,            0x0000,     0x0000},
    {0x41,                  SCAN_F8,            0x0000,     0x0000},
    {0x42,                  SCAN_F9,            0x0000,     0x0000},
    {0x43,                  SCAN_F10,           0x0000,     0x0000},
    {0x44,                  SCAN_F11,           0x0000,     0x0000},
    {0x45,                  SCAN_F12,           0x0000,     0x0000},
    {0x47,                  SCAN_NULL,          0x0000,     0x0000},    // ScrollLock
    {0x49,                  SCAN_INSERT,        0x00,       0x00},      // Insert
    {0x4A,                  SCAN_HOME,          0x00,       0x00},      // Home
    {0x4B,                  SCAN_PAGE_UP,       0x00,       0x00},      // PageUp
    {0x4C,                  SCAN_DELETE,        0x00,       0x00},      // Delete
    {0x4D,                  SCAN_END,           0x00,       0x00},      // End
    {0x4E,                  SCAN_PAGE_DOWN,     0x00,       0x00},      // PageDown
    {0x4F,                  SCAN_RIGHT,         0x00,       0x00},      // RightArrow
    {0x50,                  SCAN_LEFT,          0x00,       0x00,},     // LeftArrow
    {0x51,                  SCAN_DOWN,          0x00,       0x00},      // DownArrow
    {0x52,                  SCAN_UP,            0x00,       0x00},      // UpArrow
    {0x53,                  SCAN_NULL,          0x0000,     0x0000},    // NumLock
    {HID_CODE_TABLE_END,    HID_CODE_TABLE_END, SCAN_NULL,  SCAN_NULL}
};

EFI_STATUS
EFIAPI
TextInReset (
    IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This,
    IN BOOLEAN                        ExtendedVerification
);

EFI_STATUS
EFIAPI
ReadKeyStroke (
    IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This,
    OUT EFI_INPUT_KEY                 *Key
);

EFI_STATUS
EFIAPI
TextInResetEx (
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
    IN BOOLEAN                            ExtendedVerification
);

EFI_STATUS
EFIAPI
ReadKeyStrokeEx (
    IN  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
    OUT EFI_KEY_DATA                      *KeyData
);

EFI_STATUS
EFIAPI
SetStateEx (
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
    IN EFI_KEY_TOGGLE_STATE               *KeyToggleState
);

EFI_STATUS
EFIAPI
RegisterKeyNotify (
    IN  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
    IN  EFI_KEY_DATA                       *KeyData,
    IN  EFI_KEY_NOTIFY_FUNCTION            KeyNotificationFunction,
    OUT EFI_HANDLE                         *NotifyHandle
);

EFI_STATUS
EFIAPI
UnregisterKeyNotify (
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
    IN EFI_HANDLE                         NotificationHandle
);

EFI_SIMPLE_TEXT_INPUT_PROTOCOL SimpleTextIn = {
    TextInReset,
    ReadKeyStroke,
    NULL,
};

EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL SimpleInputEx = {
    TextInResetEx,
    ReadKeyStrokeEx,
    NULL,   // WaitForKeyEx
    SetStateEx,
    RegisterKeyNotify,
    UnregisterKeyNotify,
};

AMI_EFIKEYCODE_PROTOCOL AmiKeyCodeEx;

/**
    Check keyboard data is pending.

    @param VOID

    @retval EFI_SUCCESS - Data pending
    @retval EFI_NOT_READY - No data pending

**/

EFI_STATUS
IsKeyboardDataPending(VOID)
{
    if (SimpHidDevInfo->IntSts() == FALSE) {
	    return EFI_SUCCESS; // Data pending
    } 
    else
        return EFI_NOT_READY;   // No data pending
}

/**
    Check special key whether pressed.

    @param[in] KeyVal - Key value

    @retval TRUE - Is special key pressed.
    @retval FALSE - Is not special key pressed.

**/

UINT8
CheckSpecialKey (
    IN UINT8 KeyVal
)
{
    UINT8 Special = TRUE;

    if ( ((KeyVal&LEFT_CTRL)!=0) || ((KeyVal&RIGHT_CTRL)!=0) )
        CtrlFlag = 1;
    else
        CtrlFlag = 0;

    if ( ((KeyVal&LEFT_SHIFT)!=0) || ((KeyVal&RIGHT_SHIFT)!=0) )
        ShiftFlag = 1;
    else {
        if(ShiftFlag == 1)
            Special = FALSE;

        ShiftFlag = 0;
    }

    if ( ((KeyVal&LEFT_ALT)!=0) || ((KeyVal&RIGHT_ALT)!=0) )
        AltFlag = 1;
    else
        AltFlag = 0;
    return Special;
}

/**
    Check and skip.

    @param[in] HidBuf - Read data from I2C HID keyboard

    @retval EFI_SATAUS

**/

EFI_STATUS
CheckAndSkipBreak (
    IN UINT8* HidBuf
)
{
    UINT8 Index;

    for (Index=MIN_CHECK;Index<MAX_CHECK;Index++) {
        if (HidBuf[Index] != 0)
            return EFI_SUCCESS;
    }
    return EFI_NOT_READY;
}

/**
    Check still pressed.

    @param[in] HidBuf - Read data from I2C HID keyboard

    @retval EFI_SATAUS

**/

EFI_STATUS
CheckStillPressed (
    IN UINT8* HidBuf
)
{
    UINT8 Index;

    for (Index=MIN_CHECK;Index<MAX_CHECK;Index++) {
        if (HidBuf[Index] == 0){
            HidKeyoffset = Index-1;
            return EFI_SUCCESS;
        }
    }
    return EFI_NOT_READY;
}

/**
    Parsing HID code.

    @param[in] KeyBuffer - Read data from I2C HID keyboard

    @retval EFI_SATAUS

**/

EFI_STATUS
ParsingHidCode (
    IN UINT8* KeyBuffer
)
{
    UINTN   Index;

    if (KeyBuffer[0] != HID_KEYBOARD_LENGTH) {  // Length error
        DEBUG((DEBUG_INFO, "[I2C HID] length error \r\n"));
        return EFI_NOT_READY;
}
    if (CheckSpecialKey(KeyBuffer[SHIFT_KEY]) == FALSE) {    // Check shift key
        DEBUG((DEBUG_INFO, "[I2C HID] Check shift key error \r\n"));		
       return EFI_NOT_READY;
}
    if (CheckAndSkipBreak(KeyBuffer) != EFI_SUCCESS) {
        HidkeyRepeattimerFlg = REPEAT_DISABLE;
        ChangedFlg = CHANGE_CLEAR;
        gBS->SetTimer(HidRepeatKeyEvent, TimerCancel, 0);
        return EFI_NOT_READY;
    }
    for (Index = 0; ConvertKeyboardHidToEfiKey[Index].HidUsageId != HID_CODE_TABLE_END; Index++) {
        EfiKeyData = SCAN_NULL;
        CheckStillPressed(KeyBuffer);
        DEBUG((DEBUG_INFO, "[I2C HID] KeyBuffer[%x]: %2x \r\n",HidKeyoffset, KeyBuffer[HidKeyoffset]));
        if (KeyBuffer[HidKeyoffset] == ConvertKeyboardHidToEfiKey[Index].HidUsageId) {
            EfiKeyData = KeyBuffer[HidKeyoffset];
            if (ConvertKeyboardHidToEfiKey[Index].EfiScanCode == SCAN_NULL) {    // Normal key
                I2cKeyData.ScanCode = SCAN_NULL;
                if (ShiftFlag == 0) {
                    I2cKeyData.UnicodeChar = ConvertKeyboardHidToEfiKey[Index].UnicodeChar;
                }
                else if (ShiftFlag == 1) {
                    I2cKeyData.UnicodeChar = ConvertKeyboardHidToEfiKey[Index].ShiftUnicodeChar;
                }
                if((LocksLEDbyte & CAPS_LOCK_LED) != 0) {
                    if (I2cKeyData.UnicodeChar >= L'a' && I2cKeyData.UnicodeChar <= L'z') {
                        I2cKeyData.UnicodeChar = ConvertKeyboardHidToEfiKey[Index].ShiftUnicodeChar;
                    }
                    else if (I2cKeyData.UnicodeChar >= L'A' && I2cKeyData.UnicodeChar <= L'Z') {
                        I2cKeyData.UnicodeChar = ConvertKeyboardHidToEfiKey[Index].UnicodeChar;
                    }
                }
            }
            else {
                I2cKeyData.ScanCode = ConvertKeyboardHidToEfiKey[Index].EfiScanCode;
                I2cKeyData.UnicodeChar = ConvertKeyboardHidToEfiKey[Index].UnicodeChar;
            }
            break;
        }
    } // End of for

    return EFI_SUCCESS;
}

/**
    Simple text input reset.

    @param[in] This
    @param[in] ExtendedVerification

    @retval EFI_SATAUS

**/

EFI_STATUS
EFIAPI
TextInReset (
    IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This,
    IN BOOLEAN                        ExtendedVerification
)
{
    EFI_STATUS Status;

    DEBUG((DEBUG_INFO, "[I2C HID] TextInReset \n"));
    Status = EFI_SUCCESS;
    return Status;
}

/**
    Read Keystroke.

    @param[in] This - Simple text in protocol
    @param[out] Key  - Input key

    @retval EFI_SATAUS

**/

EFI_STATUS
EFIAPI
ReadKeyStroke (
    IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This,
    OUT EFI_INPUT_KEY                 *Key
)
{
    EFI_STATUS Status;

    Status = EFI_NOT_READY;
    DEBUG((DEBUG_INFO, "[I2C HID] ReadKeyStroke \n"));
    return Status;
}

/**
    Simple text in EX reset.

    @param[in] This - Simple text in protocol
    @param[in] ExtendedVerification 

    @retval EFI_SUCCESS - The function completed successfully

**/

EFI_STATUS
EFIAPI
TextInResetEx (
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
    IN BOOLEAN                            ExtendedVerification
)
{
    EFI_STATUS Status;

    DEBUG((DEBUG_INFO, "[I2C HID] TextInResetEx \n"));
    Status = EFI_SUCCESS;
    return Status;
}

/**
    Read Keystroke EX.

    @param[in] This - Simple text in protocol
    @param[out] KeyData - Key data

    @retval Read keystroke

**/

EFI_STATUS
EFIAPI
ReadKeyStrokeEx (
    IN  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
    OUT EFI_KEY_DATA                      *KeyData
)
{
    DEBUG((DEBUG_INFO, "[I2C HID] ReadKeyStrokeEx \n"));
    KeyData->KeyState.KeyShiftState = 0;
    KeyData->KeyState.KeyToggleState = 0;
    return ReadKeyStroke (NULL, &KeyData->Key);
}

/**
    Keyboard read EFI key EX.

    @param[in] This - EFI key code protocol
    @param[out] KeyData - Key data

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI
KbdReadEfiKeyEx (
    IN AMI_EFIKEYCODE_PROTOCOL *This,
    OUT AMI_EFI_KEY_DATA *KeyData
)
{
    EFI_STATUS Status;

    Status = ParsingHidCode(ReadData);
    if (Status != EFI_SUCCESS)
        return EFI_NOT_READY;

    if (Status == EFI_SUCCESS && !(KeyData->KeyState.KeyToggleState & KEY_STATE_EXPOSED)) {
        KeyData->Key.ScanCode = I2cKeyData.ScanCode;
        KeyData->Key.UnicodeChar = I2cKeyData.UnicodeChar;
        KeyData->EfiKey = EfiKeyData;
        KeyData->EfiKeyIsValid = 1;
        KeyData->PS2ScanCode = (UINT8)I2cKeyData.ScanCode;
        KeyData->PS2ScanCodeIsValid = 1;
        if (ChangedFlg == CHANGE_TRUE){
            gBS->SetTimer(HidRepeatKeyEvent, TimerRelative, TIMER_8000MS); //8000ms for repeat Delay
            ChangedFlg = CHANGE_CLEAR;
            HidkeyRepeattimerFlg = REPEAT_DISABLE;
            return EFI_SUCCESS;
        }
        if (HidkeyRepeattimerFlg == REPEAT_ENABLE) {
            HidkeyRepeattimerFlg = REPEAT_DISABLE;
            return EFI_SUCCESS;
        }
        return EFI_NOT_READY;
    }
    return EFI_NOT_READY;
}

/**
    Set state EX.

    @param[in] This - Simple text in EX protocol
    @param[in] KeyToggleState - Key toggle state

    @retval EFI_SUCCESS - The function completed successfully

**/

EFI_STATUS
EFIAPI
SetStateEx (
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
    IN EFI_KEY_TOGGLE_STATE               *KeyToggleState
)
{
    DEBUG((DEBUG_INFO, "[I2C HID] SetStateEx \n"));
    return EFI_SUCCESS;
}

/**
    Register key notify.

    @param[in] This - Simple text in EX protocol
    @param[in] KeyData - Key data
    @param[in] KeyNotificationFunction - Key notification function
    @param[out] NotifyHandle

    @retval EFI_SUCCESS - The function completed successfully

**/

EFI_STATUS
EFIAPI
RegisterKeyNotify (
    IN  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
    IN  EFI_KEY_DATA                       *KeyData,
    IN  EFI_KEY_NOTIFY_FUNCTION            KeyNotificationFunction,
    OUT EFI_HANDLE                         *NotifyHandle
)
{
    DEBUG((DEBUG_INFO, "[I2C HID] RegisterKeyNotify \n"));
    *NotifyHandle = NULL;
    return EFI_SUCCESS;
}

/**
    Unregister key notify.

    @param[in] This
    @param[in] NotificationHandle

    @retval EFI_SUCCESS - The function completed successfully

**/

EFI_STATUS
EFIAPI
UnregisterKeyNotify (
    IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
    IN EFI_HANDLE                         NotificationHandle
)
{
    DEBUG((DEBUG_INFO, "[I2C HID] UnregisterKeyNotify \n"));
    return EFI_SUCCESS;
}

/**
    Wait for key.

    @param[in] Event
    @param[in] Context

    @retval NONE

**/

VOID
EFIAPI
WaitForKeyEvent (
    IN EFI_EVENT          Event,
    IN VOID               *Context
)
{
    EFI_STATUS Status;

    Status = IsKeyboardDataPending();

    if (!EFI_ERROR(Status)) {
        DEBUG((DEBUG_ERROR, "[I2C HID] WaitForKeyEvent EFI_SUCCESS\r\n"));
        gBS->SignalEvent (Event);
    }
}

/**
    Set timer for HID over I2C keyboard repeat 

    @param[in] Event
    @param[in] Context

    @retval NONE

**/

VOID
EFIAPI
HidRepeatCallback (
    IN  EFI_EVENT   Event,
    IN  VOID        *Context
)
{
    HidkeyRepeattimerFlg = REPEAT_ENABLE;
    gBS->SetTimer(HidRepeatKeyEvent, TimerRelative, TIMER_500MS); //500ms for repeat rate	
}

/**
    Get Key call back function.

    @param[in] Event 
    @param[in] Context 

    @retval NONE

**/

VOID
EFIAPI
HidGetKeyCallback (
    IN EFI_EVENT   Event,
    IN VOID        *Context
)
{
    UINT8 Index;
    EFI_STATUS Status;

    if (SimpHidDevInfo->IntSts() == TRUE) {
        Status = GetI2CDeviceData(SimpHidDevInfo,ReadData,SimpHidDevInfo->HidDescriptor.MaxInputLength);
        for (Index=0;Index<HID_KEYBOARD_LENGTH;Index++) {
            if (PreReadData[Index] != ReadData[Index]) {
                gBS->SetTimer(HidRepeatKeyEvent, TimerCancel, 0);
                HidkeyRepeattimerFlg = REPEAT_DISABLE;
                ChangedFlg = CHANGE_TRUE;
            }
            PreReadData[Index] = ReadData[Index];
        }
    }
}

/**
    Configure the keyboard device.

    @param[in] HidDevInfo - Struct for I2C HID device information

    @retval NONE

**/

EFI_STATUS
I2CKbdConfigureDevice (
    IN HID_DEV_INFO *HidDevInfo
)
{
EFI_STATUS  Status;
    UINT8       Buffer[DATA_SIZE];
    UINT8       TimeOutCnt = TIME_OUT_COUNT;
    EFI_EVENT   WaitKeyEvent;

    SimpHidDevInfo = HidDevInfo;

    Status = SetPower(SimpHidDevInfo);
    DEBUG((DEBUG_INFO, "[I2C HID] Set Power %r\n",Status));
    if (EFI_ERROR(Status)) {
        return Status;
    }

    Status = SetReset(SimpHidDevInfo);
    DEBUG((DEBUG_INFO, "[I2C HID] Set Reset %r\n",Status));
    if (EFI_ERROR(Status)) {
      return Status;
    }

    //
    // After device reset command, read data from device until INT pin become high.
    // Clean up all data in the device.
    //
    while (SimpHidDevInfo->IntSts() == TRUE) {
        Status = GetI2CDeviceData(SimpHidDevInfo,Buffer,SimpHidDevInfo->HidDescriptor.MaxInputLength);
        if (EFI_ERROR(Status)) return Status;
        TimeOutCnt--;

        //
        // If there are too much data (more then TimeOutCnt) in the device and can't clean them all.
        // Just leave it, don't hang here. Let Application handle these data.
        //        
        if (TimeOutCnt == 0) {
            DEBUG((DEBUG_INFO, "[I2C HID] Too much data keep in device during initial.\n"));
            break;
        }
    }

    gBS->CreateEvent (EVT_TIMER | EVT_NOTIFY_SIGNAL, TPL_NOTIFY, HidRepeatCallback, NULL, &HidRepeatKeyEvent);

    gBS->CreateEvent (EVT_TIMER | EVT_NOTIFY_SIGNAL, TPL_NOTIFY, HidGetKeyCallback, NULL, &HidGetKeyEvent);

    gBS->SetTimer(HidGetKeyEvent, TimerPeriodic, TIMER_100MS); //100ms for repeat rate

    Status = gBS->CreateEvent (
                  EVT_NOTIFY_WAIT,
                  TPL_NOTIFY,
                  WaitForKeyEvent,
                  NULL,
                  &WaitKeyEvent);
    ASSERT_EFI_ERROR (Status);
    //
    //  Device initial done.
    //  Prepare and install simple pointer protocol.
    //

    SimpleTextIn.WaitForKey = WaitKeyEvent;
    SimpleInputEx.WaitForKeyEx = WaitKeyEvent;
    AmiKeyCodeEx.WaitForKeyEx = WaitKeyEvent;

    AmiKeyCodeEx.Reset = TextInResetEx;
    AmiKeyCodeEx.ReadEfikey = KbdReadEfiKeyEx;
    AmiKeyCodeEx.SetState = SetStateEx;
    AmiKeyCodeEx.RegisterKeyNotify = RegisterKeyNotify;
    AmiKeyCodeEx.UnregisterKeyNotify = UnregisterKeyNotify;

    Status = gBS->InstallMultipleProtocolInterfaces (
                  &InstallHandle,
                  &gEfiSimpleTextInProtocolGuid,   &SimpleTextIn,
                  &gEfiSimpleTextInputExProtocolGuid, &SimpleInputEx,
                  &gAmiEfiKeycodeProtocolGuid,     &AmiKeyCodeEx,
                  NULL); 

    DEBUG((DEBUG_INFO, "[I2C HID] ConfigureKeyboardDevice exit %r\n", Status));

    gBS->ConnectController(InstallHandle,NULL,NULL,TRUE);

    return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
