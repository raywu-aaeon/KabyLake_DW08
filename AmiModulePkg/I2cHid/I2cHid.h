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

/** @file I2cHid.h
    Header of the I2c hid Component 

**/

#ifndef _EFI_I2CHID_H_
#define _EFI_I2CHID_H_

//---------------------------------------------------------------------------

#include <Protocol/AbsPointerProtocol.h>
#include <Protocol/SimplePointer.h>

//---------------------------------------------------------------------------

typedef struct _HID_CODE_STRUCT {
    UINT8   HidUsageId;
    UINT16  EfiScanCode;
    CHAR16  UnicodeChar;
    CHAR16  ShiftUnicodeChar;
} HID_CODE_STRUCT;

#define HID_CODE_TABLE_END  0x0
#define LEFT_CTRL           0x01
#define LEFT_SHIFT          0x02
#define LEFT_ALT            0x04
#define RIGHT_CTRL          0x10
#define RIGHT_SHIFT         0x20
#define RIGHT_ALT           0x40

#define HID_CAPS_LOCK       0x39
#define NUM_LOCK_LED        0x1
#define CAPS_LOCK_LED       0x2
#define SCROL_LOCK_LED      0x4

#define HID_CODE_TABLE_END  0x0

#ifndef SCAN_NULL
#define SCAN_NULL       EFI_SCAN_NULL
#endif

#ifndef SCAN_UP
#define SCAN_UP         EFI_SCAN_UP
#endif

#ifndef SCAN_DOWN
#define SCAN_DOWN       EFI_SCAN_DN
#endif

#ifndef SCAN_RIGHT
#define SCAN_RIGHT      EFI_SCAN_RIGHT
#endif

#ifndef SCAN_LEFT
#define SCAN_LEFT       EFI_SCAN_LEFT
#endif

#ifndef SCAN_HOME
#define SCAN_HOME       EFI_SCAN_HOME
#endif

#ifndef SCAN_END
#define SCAN_END        EFI_SCAN_END
#endif

#ifndef SCAN_INSERT
#define SCAN_INSERT     EFI_SCAN_INS
#endif

#ifndef SCAN_DELETE
#define SCAN_DELETE     EFI_SCAN_DEL
#endif

#ifndef SCAN_PAGE_UP
#define SCAN_PAGE_UP    EFI_SCAN_PGUP
#endif

#ifndef SCAN_PAGE_DOWN
#define SCAN_PAGE_DOWN  EFI_SCAN_PGDN
#endif

#ifndef SCAN_F1
#define SCAN_F1         EFI_SCAN_F1
#endif

#ifndef SCAN_F2
#define SCAN_F2         EFI_SCAN_F2
#endif

#ifndef SCAN_F3
#define SCAN_F3         EFI_SCAN_F3
#endif

#ifndef SCAN_F4
#define SCAN_F4         EFI_SCAN_F4
#endif

#ifndef SCAN_F5
#define SCAN_F5         EFI_SCAN_F5
#endif

#ifndef SCAN_F6
#define SCAN_F6         EFI_SCAN_F6
#endif

#ifndef SCAN_F7
#define SCAN_F7         EFI_SCAN_F7
#endif

#ifndef SCAN_F8
#define SCAN_F8         EFI_SCAN_F8
#endif

#ifndef SCAN_F9
#define SCAN_F9         EFI_SCAN_F9
#endif

#ifndef SCAN_F10
#define SCAN_F10        EFI_SCAN_F10
#endif

#ifndef SCAN_F11
#define SCAN_F11        EFI_SCAN_F11
#endif

#ifndef SCAN_F12
#define SCAN_F12        EFI_SCAN_F12
#endif

#ifndef SCAN_ESC
#define SCAN_ESC        EFI_SCAN_ESC
#endif


#pragma pack(1)

#define AMI_I2C_PROTOCOL_GUID	{0xE10C45AC, 0xA6FD, 0x4732, { 0x84, 0x9, 0x3F, 0xBD, 0x54, 0xE5, 0x77, 0xA7}}

#define I2C_HID_DESCROPTOR_LENGTH   0x001E

#define DELAY_5MS		5000
#define DELAY_10MS		10000
#define DELAY_100MS		100000

#define TIMER_500MS		500000
#define TIMER_100MS		100000
#define TIMER_8000MS	8000000

#define LOW_VOLT		0x00
#define HIGH_VOLT		0x01

#define DATA_SIZE		0x80
#define TIME_OUT_COUNT	0x20

#define MIN_CHECK		5
#define MAX_CHECK		11

#define HID_RESET           0x01
#define HID_GET_REPORT      0x02
#define HID_SET_REPORT      0x03
#define HID_GET_IDLE        0x04
#define HID_SET_IDLE        0x05
#define HID_GET_PROTOCOL    0x06
#define HID_SET_PROTOCOL    0x07
#define HID_SET_POWER       0x08

//----------------------------------------------------------------------------
// HID report item format
//----------------------------------------------------------------------------
#define HID_ITEM_FORMAT_SHORT   0
#define HID_ITEM_FORMAT_LONG    1

//----------------------------------------------------------------------------
// HID report descriptor item type (prefix bit 2,3)
//----------------------------------------------------------------------------
#define HID_ITEM_TYPE_MAIN      0
#define HID_ITEM_TYPE_GLOBAL    1
#define HID_ITEM_TYPE_LOCAL     2
#define HID_ITEM_TYPE_RESERVED  3

//----------------------------------------------------------------------------
// HID report descriptor main item tags
//----------------------------------------------------------------------------
#define HID_MAIN_ITEM_TAG_INPUT             8
#define HID_MAIN_ITEM_TAG_OUTPUT            9
#define HID_MAIN_ITEM_TAG_FEATURE           0xb
#define HID_MAIN_ITEM_TAG_BEGIN_COLLECTION  0xa
#define HID_MAIN_ITEM_TAG_END_COLLECTION    0xc

//----------------------------------------------------------------------------
// HID report descriptor main item contents
//----------------------------------------------------------------------------
#define HID_MAIN_ITEM_CONSTANT      0x001
#define HID_MAIN_ITEM_VARIABLE      0x002
#define HID_MAIN_ITEM_RELATIVE      0x004
#define HID_MAIN_ITEM_WRAP          0x008
#define HID_MAIN_ITEM_NONLINEAR     0x010
#define HID_MAIN_ITEM_NO_PREFERRED  0x020
#define HID_MAIN_ITEM_NULL_STATE    0x040
#define HID_MAIN_ITEM_VOLATILE      0x080
#define HID_MAIN_ITEM_BUFFERED_BYTE 0x100

//----------------------------------------------------------------------------
// HID report descriptor collection item types
//----------------------------------------------------------------------------
#define HID_COLLECTION_PHYSICAL     0
#define HID_COLLECTION_APPLICATION  1
#define HID_COLLECTION_LOGICAL      2

//----------------------------------------------------------------------------
// HID report descriptor global item tags
//----------------------------------------------------------------------------
#define HID_GLOBAL_ITEM_TAG_USAGE_PAGE          0
#define HID_GLOBAL_ITEM_TAG_LOGICAL_MINIMUM     1
#define HID_GLOBAL_ITEM_TAG_LOGICAL_MAXIMUM     2
#define HID_GLOBAL_ITEM_TAG_PHYSICAL_MINIMUM    3
#define HID_GLOBAL_ITEM_TAG_PHYSICAL_MAXIMUM    4
#define HID_GLOBAL_ITEM_TAG_UNIT_EXPONENT       5
#define HID_GLOBAL_ITEM_TAG_UNIT                6
#define HID_GLOBAL_ITEM_TAG_REPORT_SIZE         7
#define HID_GLOBAL_ITEM_TAG_REPORT_ID           8
#define HID_GLOBAL_ITEM_TAG_REPORT_COUNT        9
#define HID_GLOBAL_ITEM_TAG_PUSH                0x0a
#define HID_GLOBAL_ITEM_TAG_POP                 0x0b

//----------------------------------------------------------------------------
// HID report descriptor local item tags
//----------------------------------------------------------------------------
#define HID_LOCAL_ITEM_TAG_USAGE                0
#define HID_LOCAL_ITEM_TAG_USAGE_MINIMUM        1
#define HID_LOCAL_ITEM_TAG_USAGE_MAXIMUM        2
#define HID_LOCAL_ITEM_TAG_DESIGNATOR_INDEX     3
#define HID_LOCAL_ITEM_TAG_DESIGNATOR_MINIMUM   4
#define HID_LOCAL_ITEM_TAG_DESIGNATOR_MAXIMUM   5
#define HID_LOCAL_ITEM_TAG_STRING_INDEX         7
#define HID_LOCAL_ITEM_TAG_STRING_MINIMUM       8
#define HID_LOCAL_ITEM_TAG_STRING_MAXIMUM       9
#define HID_LOCAL_ITEM_TAG_DELIMITER            0x0a

//----------------------------------------------------------------------------
// HID usage pages
//----------------------------------------------------------------------------
#define HID_UP_GENDESK                      0x01
#define HID_UP_KEYBOARD                     0x07
#define HID_UP_LED                          0x08
#define HID_UP_BUTTON                       0x09
#define HID_UP_DIGITIZER                    0x0D
#define HID_UP_FIDO                         0xF1D0	//Fast IDentity Online Alliance

#define HID_GENDESK_POINTER                 0x01
#define HID_GENDESK_MOUSE                   0x02
#define HID_GENDESK_KEYBOARD                0x06
#define HID_GENDESK_KEYPAD                  0x07
#define HID_GENDESK_X                       0x30
#define HID_GENDESK_Y                       0x31
#define HID_GENDESK_Z                       0x32
#define HID_GENDESK_WHEEL                   0x38

#define HID_KEYBOARD_LEFT_CTRL              0xE0
#define HID_KEYBOARD_LEFT_SHIFT             0xE1
#define HID_KEYBOARD_LEFT_ALT               0xE2
#define HID_KEYBOARD_LEFT    _GUI           0xE3
#define HID_KEYBOARD_RIGHT_CTRL             0xE4
#define HID_KEYBOARD_RIGHT_SHIFT            0xE5
#define HID_KEYBOARD_RIGHT_ALT              0xE6
#define HID_KEYBOARD_RIGHT_GUI              0xE7

#define HID_LED_NUM_LOCK                    0x01
#define HID_LED_CAP_LOCK                    0x02
#define HID_LED_SCROLL_LOCK                 0x03

#define HID_BUTTON_BUTTON1                  0x01

#define HID_DIGITIZER_TOUCH_SCREEN          0x04
#define HID_DIGITIZER_TIP_SWITCH            0x42
#define HID_DIGITIZER_DYNAMIC_VALUE         0x51
#define HID_DIGITIZER_DEVICE_MODE           0x52
#define HID_DIGITIZER_DEVICE_IDENTIFIER     0x53

#define HID_FIDO_U2F_AD                     0x01	// U2F Authenticator Device
#define HID_FIDO_INPUT_REPORT_DATA          0x20
#define HID_FIDO_OUTPUT_REPORT_DATA         0x21

#define HID_KEYBOARD_LENGTH					0x0B

//----------------------------------------------------------------------------
//	Report descriptor struct define
//----------------------------------------------------------------------------
#define HID_BFLAG_DATA_BIT      BIT0    //0:DATA        1:CONSTANT
#define HID_BFLAG_ARRAY_BIT     BIT1    //0:ARRAY       1:VARIABLE
#define HID_BFLAG_ABSOLUTE_BIT  BIT2    //0:ABSOLUTE    1:RELATIVE
#define HID_BFLAG_SKIP          BIT3    //1:Skip this data
#define HID_BFLAG_INPUT         BIT4    //0:OUTPUT      1:INPUT
#define HID_MAX_USAGE           0x14
//----------------------------------------------------------------------------
//  Report descriptor's hid_item
//----------------------------------------------------------------------------
typedef struct {
    UINT8   Size;
    UINT8   Type;
    UINT8   Tag;
    union {
        UINT8   U8;
        UINT16  U16;
        UINT32  U32;
    } Data;
} HID_Item;

typedef struct {
    UINT8   Flag;
    UINT8   ReportID;
    UINT8   UsagePage;
    UINT8   ReportCount;
    UINT8   ReportSize;
    UINT16  LogicalMin;
    UINT16  LogicalMax;
    UINT8   UsageCount;
    UINT8   Usage[HID_MAX_USAGE];
    UINT8   UsageMaxCount;
    UINT16  UsageMax[5];
    UINT16  UsageMin[5];
    UINT8   CollectionCount;
} HID_STRUC;

#define SUB_DEV_TYPE_KEYBOARD          	BIT0
#define SUB_DEV_TYPE_MOUSE             	BIT1
#define SUB_DEV_TYPE_POINT             	BIT2 

#define HID_REPORT_BFLAG_REPORT_PROTOCOL    BIT0    //If use report protocol
#define HID_REPORT_BFLAG_REPORT_ID          BIT1    //1:REPORT_ID EXIST
#define HID_REPORT_BFLAG_TOUCH_BUTTON_FLAG  BIT2
#define HID_REPORT_BFLAG_LED_FLAG           BIT3    //1:LED

typedef struct {
    UINT8       SubdevType;    //Mouse or Keyboard or Touch Panel
    UINT8       TotalCount;
    UINT8       Flag;
    UINT16      AbsMaxX;
    UINT16      AbsMaxY;
    HID_STRUC   Report[50];
} HIDReport_STRUC;

typedef struct {
    UINT16  HIDDescLength;
    UINT16  BcdVersion;
    UINT16  ReportDescLength;
    UINT16  ReportDescRegister;
    UINT16  InputRegister;
    UINT16  MaxInputLength;
    UINT16  OutputRegister;
    UINT16  MaxOutputLength;
    UINT16  CommandRegister;
    UINT16  DataRegister;
    UINT16  VendorID;
    UINT16  ProductID;
    UINT16  VersionID;
    UINT32  Reserved;
} I2C_HID_DESCRIPTOR;

typedef EFI_STATUS (*I2C_HID_DEV_HW_RESET) (VOID);
typedef BOOLEAN (*I2C_HID_DEV_INT_STS) (VOID);

typedef struct {
    EFI_ABSOLUTE_POINTER_PROTOCOL   AbsolutePointerProtocol;
    EFI_ABSOLUTE_POINTER_STATE      State;
    EFI_ABSOLUTE_POINTER_MODE       Mode;
} ABSOLUTE_POINTER_STRUC;

typedef struct {
    EFI_SIMPLE_POINTER_PROTOCOL     SimplePointerProtocol;
    EFI_SIMPLE_POINTER_STATE        State;
    EFI_SIMPLE_POINTER_MODE          Mode;
} SIMPLE_POINTER_STRUC;

typedef struct _HID_DEV_INFO {
    EFI_HANDLE                          DeviceHandle;
    UINT8                               BusNo;
    UINT8                               SlaveAddress;
    UINT16                              HidDescAddress;
    I2C_HID_DESCRIPTOR                  HidDescriptor;
    UINT8*                              ReportDescriptor;
    HIDReport_STRUC                     ReportStruc;
    I2C_HID_DEV_INT_STS                 IntSts;
    I2C_HID_DEV_HW_RESET                HwReset;
    ABSOLUTE_POINTER_STRUC              AbsStruc;
    SIMPLE_POINTER_STRUC                SimpStruc;
} HID_DEV_INFO;

EFI_STATUS
SetPower (
    IN HID_DEV_INFO *HidDevInfo
);

EFI_STATUS
SetReset (
    IN HID_DEV_INFO *HidDevInfo
);

EFI_STATUS
SetIdle (
    IN HID_DEV_INFO *HidDevInfo
);

EFI_STATUS
GetI2CDeviceData (
    IN HID_DEV_INFO *HidDevInfo,
    IN OUT UINT8        *Buffer,
    IN UINT16        Size
);

UINTN
GetReportMouseData (
    IN UINT8 *Data,
    IN UINTN Start,
    IN UINTN End
);    

typedef struct _HID_DEV {
    UINT8                   BusNo;
    UINT8                   SlaveAddress;
    UINT16                  HidDescAddress;
    I2C_HID_DEV_INT_STS     GetIntSts;
    I2C_HID_DEV_HW_RESET    HwReset;
} HID_DEV;

typedef struct _EFI_AMI_I2C_PROTOCOL EFI_AMI_I2C_PROTOCOL;

typedef EFI_STATUS (EFIAPI *I2CCOMBINREAD) (
    IN  UINT8 BusNo, 
    IN  UINT8 SlaveAddress,
    IN  UINT8 *WriteBuffer,
    IN OUT  UINT8 *ReadBuffer,
    IN  UINT16 WLength,
    IN  UINT16 RLength
);

typedef EFI_STATUS (EFIAPI *I2CREAD) (
    IN  UINT8 BusNo,
    IN  UINT8 SlaveAddress,
    IN OUT  UINT8 *ReadBuffer,
    IN  UINT16 RLength
);

typedef EFI_STATUS (EFIAPI *I2CWRITE) (
    IN  UINT8 BusNo,
    IN  UINT8 SlaveAddress,
    IN  UINT8 *WriteBuffer,
    IN  UINT16 WLength
);

struct _EFI_AMI_I2C_PROTOCOL {
    I2CCOMBINREAD   CombinReadI2C;  // S,Address+W,Data,Data+n,RS,Address+R,Data,Data+n ;WLength=0 => S,Address+R,Data,Data+n ;RLength=0 => S,Address+W,Data,Data+n
    I2CREAD         I2CReadData;    // S,Address+W,Data,Data+n,RS,Address+R,Data,Data+n ;WLength=0 => S,Address+R,Data,Data+n ;
    I2CWRITE        I2CWriteData;   // S,Address+W,Data,Data+n,RS,Address+R,Data,Data+n ;RLength=0 => S,Address+W,Data,Data+n
};

#pragma pack()
#endif

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
