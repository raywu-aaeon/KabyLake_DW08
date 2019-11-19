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

/** @file I2cHid.c
    File has the functions to initialize HID interface over I2C function and 
    parsing HID description.

**/

//---------------------------------------------------------------------------

#include <Efi.h>
#include <Token.h>
#include <Protocol/DriverBinding.h>
#include <I2cHid.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

//---------------------------------------------------------------------------

EFI_AMI_I2C_PROTOCOL    *AmiI2C;

#define EFI_MOUSE_DRIVER_PROTOCOL_GUID \
{0xc7a7030c, 0xc3d8, 0x45ee, 0xbe, 0xd9, 0x5d, 0x9e, 0x76, 0x76, 0x29, 0x53}
EFI_GUID    gEfiDxeMouseDriverProtocolGuid = EFI_MOUSE_DRIVER_PROTOCOL_GUID;
EFI_GUID    gEfiAmiI2CProtocolGuid = AMI_I2C_PROTOCOL_GUID;
EFI_EVENT   gAllDriverConnectedEvent;
VOID        *gAllDriverConnectedNotifyRegistration;

typedef UINT8 HID_PARSE_PROC(HID_Item*,HID_STRUC*,HIDReport_STRUC*);

extern  HID_DEV gHidDevTable[];
extern  UINTN   gHidDevCount;

#define LOCAL_ITEM	5

EFI_STATUS
I2CAbsConfigureDevice (
    IN HID_DEV_INFO *HidDevInfo
);

EFI_STATUS
I2CMsConfigureDevice (
    IN HID_DEV_INFO *HidDevInfo
);

EFI_STATUS
I2CKbdConfigureDevice (
    IN HID_DEV_INFO *HidDevInfo
);

/**
    Add input or output item follow the Hid field.

    @param[in] HidStruc - I2C HID struct
    @param[in, out] HidReport - I2C HID report struct

    @retval None

**/

VOID
AddHidField (
    IN HID_STRUC         *HidStruc,
    IN OUT HIDReport_STRUC*  HidReport
)
{
    UINT8   Index;

    DEBUG((DEBUG_INFO, "[I2C HID] Add hid field  count is %x \n",HidReport->TotalCount));

    HidReport->Report[HidReport->TotalCount].Flag = HidStruc->Flag;
    DEBUG((DEBUG_INFO, "[I2C HID] Flag  %x \n",HidReport->Report[HidReport->TotalCount].Flag));

    HidReport->Report[HidReport->TotalCount].UsagePage = HidStruc->UsagePage;
    DEBUG((DEBUG_INFO, "[I2C HID] UsagePage  %x \n",HidStruc->UsagePage));

    if (HidStruc->ReportID != 0) HidReport->Flag |= HID_REPORT_BFLAG_REPORT_ID;
    HidReport->Report[HidReport->TotalCount].ReportID = HidStruc->ReportID;
    DEBUG((DEBUG_INFO, "[I2C HID] ReportID  %x \n",HidStruc->ReportID));

    HidReport->Report[HidReport->TotalCount].ReportCount = HidStruc->ReportCount;
    DEBUG((DEBUG_INFO, "[I2C HID] ReportCount  %x \n",HidStruc->ReportCount));

    HidReport->Report[HidReport->TotalCount].ReportSize = HidStruc->ReportSize;
    DEBUG((DEBUG_INFO, "[I2C HID] ReportSize  %x \n",HidStruc->ReportSize));

    HidReport->Report[HidReport->TotalCount].LogicalMax = HidStruc->LogicalMax;
    DEBUG((DEBUG_INFO, "[I2C HID] LogicalMax  %x \n",HidStruc->LogicalMax));

    HidReport->Report[HidReport->TotalCount].LogicalMin = HidStruc->LogicalMin;
    DEBUG((DEBUG_INFO, "[I2C HID] LogicalMin %x \n",HidStruc->LogicalMin));

    HidReport->Report[HidReport->TotalCount].UsageMaxCount = HidStruc->UsageMaxCount;
    DEBUG((DEBUG_INFO, "[I2C HID] UsageMaxCount %x \n",HidStruc->UsageMaxCount));

    for (Index=0;Index<(HidStruc->UsageMaxCount);Index++) {
        HidReport->Report[HidReport->TotalCount].UsageMax[Index] = HidStruc->UsageMax[Index];
        HidReport->Report[HidReport->TotalCount].UsageMin[Index] = HidStruc->UsageMin[Index];
        DEBUG((DEBUG_INFO, "[I2C HID] UsageMax_%x %x \t",Index,HidStruc->UsageMax[Index]));
        DEBUG((DEBUG_INFO, "[I2C HID] UsageMin_%x %x \n",Index,HidStruc->UsageMin[Index]));
    } 

    HidReport->Report[HidReport->TotalCount].UsageCount = HidStruc->UsageCount;
    DEBUG((DEBUG_INFO, "[I2C HID] UsageCount  %x \n",HidStruc->UsageCount));

    //
    //  Note: If there are several LogicalMax and LogicalMin of X-Y axis in 
    //        Report Descriptor, We use first one as AbsMax and AbsMin. 
    //
    for (Index=0;Index<(HidStruc->UsageCount);Index++) {
        HidReport->Report[HidReport->TotalCount].Usage[Index] = HidStruc->Usage[Index];

        if (HidStruc->Usage[Index]==HID_GENDESK_X && HidStruc->UsagePage == HID_UP_GENDESK) {
            // Check device type  
            if ((!(HidStruc->Flag & HID_BFLAG_ABSOLUTE_BIT)))
                HidReport->SubdevType |= SUB_DEV_TYPE_POINT;
            else 
                HidReport->SubdevType |= SUB_DEV_TYPE_MOUSE;

            if (HidReport->AbsMaxX == 0)
                HidReport->AbsMaxX = HidStruc->LogicalMax;
        }

        if (HidStruc->Usage[Index]==HID_GENDESK_Y && HidStruc->UsagePage == HID_UP_GENDESK)
        {
            if (HidReport->AbsMaxY == 0)
                HidReport->AbsMaxY = HidStruc->LogicalMax; 
        }

        DEBUG((DEBUG_INFO, "[I2C HID] num %x Usage %x \n",Index,HidStruc->Usage[Index]));
    }

    DEBUG((DEBUG_INFO, "============================================== \n"));

    return;
}

/**
    Get item data.

    @param[in, out] Item - I2C HID dsecirptor's item

    @retval Item - I2C HID dsecirptor's item

**/

UINTN
GetItemData (
    IN OUT HID_Item *Item
)
{
    switch (Item->Size) {
        case 1: return Item->Data.U8;
        case 2: return Item->Data.U16;
        case 3: return Item->Data.U32;
    }
    return 0;
}

/**
    Hid parser main.

    @param[in] Item - I2C HID dsecirptor's item
    @param[in, out] HidStruc - I2C HID struct
    @param[in, out] HidReport - I2C HID report struct

    @retval NONE

**/

UINT8
HidParserMain (
    IN HID_Item          *Item,
    IN OUT HID_STRUC         *HidStruc,
    IN OUT HIDReport_STRUC*   HidReport
)
{
    UINT8   Index;

    switch (Item->Tag) {
        case HID_MAIN_ITEM_TAG_BEGIN_COLLECTION:
            HidStruc->CollectionCount++;
            break;
        case HID_MAIN_ITEM_TAG_END_COLLECTION:
            HidStruc->CollectionCount--;
            if (HidStruc->CollectionCount == 0)
            	HidStruc->Flag &= 0xF7;
            break;
        case HID_MAIN_ITEM_TAG_INPUT:
            if (HidStruc->Flag & HID_BFLAG_SKIP)
            	break;
            HidStruc->Flag  =(UINT8)GetItemData(Item) & 7;
            HidStruc->Flag |= HID_BFLAG_INPUT;
            AddHidField(HidStruc,HidReport);
            HidReport->TotalCount++;
            break;
        case HID_MAIN_ITEM_TAG_OUTPUT:
            if (HidStruc->Flag & HID_BFLAG_SKIP)
            	break;
            HidStruc->Flag = (UINT8)GetItemData(Item) & 7;
            if (HidStruc->UsagePage == HID_UP_LED) {
                AddHidField(HidStruc,HidReport);
                HidReport->TotalCount++;
            }
            break;
        case HID_MAIN_ITEM_TAG_FEATURE:
            break;
        default:
            break;
    }

    //Clear Local Item
    HidStruc->UsageCount =  0;
    HidStruc->UsageMaxCount =  0;
    for (Index=0;Index<LOCAL_ITEM;Index++) {
        HidStruc->UsageMax[Index] = 0;
        HidStruc->UsageMin[Index] = 0;
    }

    for (Index=0;Index<HID_MAX_USAGE;Index++) {
        HidStruc->Usage[Index] = 0;
    }

    return 0;
}

/**
    Hid parser global.

    @param[in] Item - I2C HID dsecirptor's item
    @param[in, out] HidStruc - I2C HID struct
    @param[in, out] HidReport - I2C HID report struct

    @retval NONE

**/

UINT8
HidParserGlobal (
    IN HID_Item          *Item,
    IN OUT HID_STRUC         *HidStruc,
    IN OUT HIDReport_STRUC*  HidReport
)
{
    if (HidStruc->Flag & BIT3)
        return 0;

    switch (Item->Tag) {
        case HID_GLOBAL_ITEM_TAG_USAGE_PAGE:
            HidStruc->UsagePage = (UINT8)GetItemData(Item);

            //Get Led usage page
            if (HidStruc->UsagePage == HID_UP_LED)
                HidReport->Flag |= HID_REPORT_BFLAG_LED_FLAG;

            if (HidStruc->UsagePage == HID_UP_KEYBOARD)
                HidReport->SubdevType |= SUB_DEV_TYPE_KEYBOARD;

            return 0;

        case HID_GLOBAL_ITEM_TAG_LOGICAL_MINIMUM:
            HidStruc->LogicalMin = (UINT16) GetItemData(Item);
            return 0;

        case HID_GLOBAL_ITEM_TAG_LOGICAL_MAXIMUM:
            HidStruc->LogicalMax = (UINT16) GetItemData(Item);
            return 0;

        case HID_GLOBAL_ITEM_TAG_REPORT_SIZE:
            HidStruc->ReportSize = (UINT8)GetItemData(Item);
            return 0;

        case HID_GLOBAL_ITEM_TAG_REPORT_COUNT:
            HidStruc->ReportCount = (UINT8)GetItemData(Item);
            return 0;

        case HID_GLOBAL_ITEM_TAG_REPORT_ID:
            HidStruc->ReportID = (UINT8)GetItemData(Item);
            return 0;

        default:
            return -1;
    }
}

/**
    Hid parser local.

    @param[in] Item - I2C HID dsecirptor's item
    @param[in, out] HidStruc - I2C HID struct
    @param[in, out] HidReport - I2C HID report struct

    @retval NONE

**/

UINT8
HidParserLocal (
    IN HID_Item          *Item,
    IN OUT HID_STRUC         *HidStruc,
    IN OUT HIDReport_STRUC*  HidReport
)
{
    if (HidStruc->Flag & HID_BFLAG_SKIP)
        return 0;
    switch (Item->Tag) {
        case HID_LOCAL_ITEM_TAG_USAGE:	
            if (HidStruc->UsageCount < HID_MAX_USAGE)
                HidStruc->Usage[HidStruc->UsageCount] = (UINT8) GetItemData(Item);

            HidStruc->UsageCount++;

            if (HidStruc->UsagePage == HID_GENDESK_POINTER && HidStruc->Usage[HidStruc->UsageCount-1] == HID_GENDESK_MOUSE) {
                HidReport->SubdevType |= SUB_DEV_TYPE_MOUSE;
            }
            return 0;

        case HID_LOCAL_ITEM_TAG_USAGE_MINIMUM:
            if (HidStruc->UsageMaxCount < 5)
                HidStruc->UsageMin[HidStruc->UsageMaxCount] = (UINT16)GetItemData(Item);
             return 0;

        case HID_LOCAL_ITEM_TAG_USAGE_MAXIMUM:
            if (HidStruc->UsageMaxCount < 5)
                HidStruc->UsageMax[HidStruc->UsageMaxCount] = (UINT16)GetItemData(Item);
            HidStruc->UsageMaxCount++;
            return 0;

        default:
            return 0;
    }
}

/**
    Hid parser reserved.

    @param[in] Item - I2C HID dsecirptor's item
    @param[in, out] HidStruc - I2C HID struct
    @param[in, out] HidReport - I2C HID report struct

    @retval NONE

**/

UINT8
HidParserReserved (
    IN HID_Item          *Item,
    IN OUT HID_STRUC         *HidStruc,
    IN OUT HIDReport_STRUC*  HidReport
)
{
    return 0;
}

/**
    Parse report descriptor.

    @param[in] HidDesc - I2C HID descriptor
    @param[in, out] ReportDesc - I2C HID report descriptor
    @param[in, out] ReportStuc - I2C HID report struct

    @retval NONE

**/

VOID
I2CParseReportDescriptor (
    IN I2C_HID_DESCRIPTOR  *HidDesc,
    IN OUT UINT8               *ReportDesc,
    IN OUT HIDReport_STRUC     *ReportStuc
)
{
    INT32           ReportLength = (UINT32)(HidDesc->ReportDescLength);
    HID_STRUC       TempHidStruc;
    HID_Item        Item;
    UINT8           BufferData;
    HID_PARSE_PROC  *DispatchType[] = {HidParserMain,HidParserGlobal,HidParserLocal,HidParserReserved};

    gBS->SetMem(&TempHidStruc,sizeof(HID_STRUC),0);

    for (;ReportLength>0;ReportLength--) {
        BufferData = *ReportDesc++;

        //Get HID item	
        Item.Type = (BufferData>> 2) & 3;
        Item.Tag  = (BufferData>> 4) & 15;
        Item.Size = BufferData & 3;
        switch (Item.Size) {
            case 1:
                Item.Data.U8 = *ReportDesc++;
                ReportLength--;
                break;
            case 2:
                Item.Data.U16 = *(UINT16*)ReportDesc;
                ReportDesc += 2;
                ReportLength -= 2;
                break;
            case 3:
                Item.Data.U32 = *(UINT32*)ReportDesc;
                ReportDesc += 4;
                ReportLength -= 4 ;
                break;
            default:
                break;
        }

        // Parsing HID item
        DispatchType[Item.Type](&Item,&TempHidStruc,ReportStuc);
    }

    DEBUG((DEBUG_INFO,"[I2C HID]: Report Struc Type:"));

    if ( ReportStuc->SubdevType & SUB_DEV_TYPE_KEYBOARD)
        DEBUG((DEBUG_INFO,"Keyboard,"));
    if ( ReportStuc->SubdevType & SUB_DEV_TYPE_MOUSE)
        DEBUG((DEBUG_INFO,"Mouse,"));
    if (ReportStuc->SubdevType & SUB_DEV_TYPE_POINT)
        DEBUG((DEBUG_INFO,"Abs Point,"));

    DEBUG((DEBUG_INFO, "TotalCount:%x, Flag:%x, AbsMaxX:%x, AbsMaxY:%x\n",
            ReportStuc->TotalCount,
            ReportStuc->Flag,
            ReportStuc->AbsMaxX,
            ReportStuc->AbsMaxY)); 

    return;
}

/**
    Set hid over i2c device power command.

    @param[in] HidDevInfo - Struct for I2C HID device information

    @retval EFI_SUCCESS - IF the data has been successfully read or write
    @retval EFI_ERROR - Operation Failed, Device Error

**/

EFI_STATUS
SetPower (
    IN HID_DEV_INFO *HidDevInfo
)
{
	EFI_STATUS  Status;
    UINT8       LengthW;
    UINT8       WriteData[4];

    LengthW = 0x04;
    *(UINT16*)WriteData = HidDevInfo->HidDescriptor.CommandRegister;
    WriteData[2] = 0x00;
    WriteData[3] = HID_SET_POWER;

    Status = AmiI2C->CombinReadI2C( HidDevInfo->BusNo,
                                    HidDevInfo->SlaveAddress,
                                    WriteData,
                                    NULL,
                                    LengthW,
                                    0 );
    return Status;
}

/**
    Set hid over i2c device reset command.

    @param[in] HidDevInfo - Struct for I2C HID device information

    @retval EFI_SUCCESS - IF the data has been successfully read or write
    @retval EFI_ERROR - Operation Failed, Device Error

**/

EFI_STATUS
SetReset (
    IN HID_DEV_INFO *HidDevInfo
)
{
	EFI_STATUS  Status;
    UINT8       LengthW;
    UINT8       WriteData[4];

    LengthW = 0x04;
    *(UINT16*)WriteData = HidDevInfo->HidDescriptor.CommandRegister;
    WriteData[2] = 0x00;
    WriteData[3] = HID_RESET;

    Status = AmiI2C->CombinReadI2C( HidDevInfo->BusNo,
                                    HidDevInfo->SlaveAddress,
                                    WriteData,
                                    NULL,
                                    LengthW,
                                    0 );
    return Status;

}

/**
    Set hid over i2c device idle command.

    @param[in] HidDevInfo - Struct for I2C HID device information

    @retval EFI_SUCCESS - IF the data has been successfully read or write
    @retval EFI_ERROR - Operation Failed, Device Error

**/

EFI_STATUS
SetIdle (
    IN HID_DEV_INFO *HidDevInfo
)
{
	EFI_STATUS  Status;
    UINT8       LengthW;
    UINT8       WriteData[8];

    LengthW = 0x04;
    *(UINT16*)WriteData = HidDevInfo->HidDescriptor.CommandRegister;
    WriteData[2] = 0x00;
    WriteData[3] = HID_SET_IDLE;
    Status = AmiI2C->CombinReadI2C( HidDevInfo->BusNo,
                                    HidDevInfo->SlaveAddress,
                                    WriteData,
                                    NULL,
                                    LengthW,
                                    0 );
    DEBUG((DEBUG_INFO,"[I2C HID] SetIdle command 1: %r\n",Status));

    LengthW = 0x04;
    *(UINT16*)WriteData = HidDevInfo->HidDescriptor.CommandRegister;
    WriteData[2] = 0x00;
    WriteData[3] = HID_SET_IDLE;	
    Status = AmiI2C->CombinReadI2C( HidDevInfo->BusNo,
                                    HidDevInfo->SlaveAddress,
                                    WriteData,
                                    NULL,
                                    LengthW,
                                    0 );
    DEBUG((DEBUG_INFO,"[I2C HID] SetIdle command 2: %r\n",Status));

    return Status;

}

/**
    Get report descriptor from I2C device.

    @param[in, out] HidDevInfo - Struct for I2C HID device information

    @retval EFI_STATUS

**/

EFI_STATUS
GetReportDescriptor (
    IN OUT HID_DEV_INFO *HidDevInfo
)
{
    EFI_STATUS  Status;
    UINT8       WriteData[2];
    UINT8       *ReportDesc;
    UINT16      LengthW;
    UINT16      LengthR;
    
    LengthW = 0x02;
    *(UINT16*)WriteData = HidDevInfo->HidDescriptor.ReportDescRegister;

    LengthR = HidDevInfo->HidDescriptor.ReportDescLength;
    Status = gBS->AllocatePool(EfiBootServicesData, LengthR, &ReportDesc);
    if (EFI_ERROR(Status))
    	return Status;

    Status = AmiI2C->CombinReadI2C( HidDevInfo->BusNo,
                                    HidDevInfo->SlaveAddress,
                                    WriteData,
                                    ReportDesc,
                                    LengthW,
                                    LengthR );
    if (EFI_ERROR(Status)) {
        gBS->FreePool(ReportDesc);
        return Status;
    }

    HidDevInfo->ReportDescriptor = ReportDesc;
    {
        UINT16 Index = 0;
        DEBUG((DEBUG_INFO,"======= [I2C HID] Report Descriptor ===== \n"));
        while (Index < LengthR) {
            DEBUG((DEBUG_INFO,"%02x ",ReportDesc[Index]));
            Index++;
            if (Index%16 == 0) DEBUG((DEBUG_INFO," \n"));
        }
        DEBUG((DEBUG_INFO,"\n============    END    ============== \n"));
    }

    return EFI_SUCCESS;
}

/**
    Get HID descriptor from I2C device.

    @param[in, out] HidDevInfo - Struct for I2C HID device information

    @retval EFI_STATUS

**/

EFI_STATUS
GetHidDescriptor (
    IN OUT HID_DEV_INFO *HidDevInfo
)
{
    EFI_STATUS          Status;
    UINT8               WriteData[2];
    UINT16              LengthW;
    UINT16              LengthR;
    I2C_HID_DESCRIPTOR  *HidDesc = &HidDevInfo->HidDescriptor;

    LengthW = 0x02;
    *(UINT16*)WriteData = HidDevInfo->HidDescAddress;

    LengthR = I2C_HID_DESCROPTOR_LENGTH;

    Status = AmiI2C->CombinReadI2C( HidDevInfo->BusNo,
                                    HidDevInfo->SlaveAddress,
                                    WriteData,
                                    (UINT8*)HidDesc,
                                    LengthW,
                                    LengthR );
    if (EFI_ERROR(Status)) return Status;
    
    {
        UINT8   Index=0;
        UINT8   *Ptr8 = (UINT8*)&HidDevInfo->HidDescriptor;
            
        DEBUG((DEBUG_INFO,"[I2C HID] Bus%02x, Slave Address:%x\n",HidDevInfo->BusNo,HidDevInfo->SlaveAddress));
        DEBUG((DEBUG_INFO,"=============== [I2C HID] HID Descriptor ==========\n"));
        for (Index=0;Index<I2C_HID_DESCROPTOR_LENGTH;Index++) {
            DEBUG((DEBUG_INFO,"%x ",Ptr8[Index]));
        }
        DEBUG((DEBUG_INFO,"\n=================  END     ===================\n"));
    }

    //
    // Check Hid Descriptor is correct.
    // Version 1.00, HID Descriptor length is 0x1E 
    //
    if ( !(HidDesc->HIDDescLength == I2C_HID_DESCROPTOR_LENGTH && HidDesc->BcdVersion == 0x0100))
        return EFI_UNSUPPORTED;

    return EFI_SUCCESS;
}

/**
    Receive data from I2C device.

    @param[in] HidDevInfo - Struct for I2C HID device information
    @param[in, out] Buffer - Point of data buffer
    @param[in] Size - Size of data

    @retval EFI_STATUS

**/

EFI_STATUS
GetI2CDeviceData (
    IN HID_DEV_INFO *HidDevInfo,
    IN OUT UINT8    *Buffer,
    IN UINT16       Size
)
{
    EFI_STATUS  Status;

    Status = AmiI2C->CombinReadI2C( HidDevInfo->BusNo,
                                    HidDevInfo->SlaveAddress,
                                    NULL,
                                    (UINT8*)Buffer,
                                    0x00,
                                    Size );

    return Status;
}

/**
    Get the report descriptor to know which device type connected
    and configure it.

    @param[in, out] HidDevInfo - Struct for I2C HID device information

    @retval EFI_STATUS

**/

EFI_STATUS
ConfigureHidDevice (
    IN OUT HID_DEV_INFO *HidDevInfo
)
{
    EFI_STATUS  Status;

    Status = GetReportDescriptor(HidDevInfo);
    DEBUG((DEBUG_INFO,"[I2C HID] GetReportDescriptor %r\n",Status));
    if (EFI_ERROR(Status)) {
        return Status;
    }

    I2CParseReportDescriptor(   &HidDevInfo->HidDescriptor,
                                HidDevInfo->ReportDescriptor,
                                &HidDevInfo->ReportStruc);

    if (HidDevInfo->ReportStruc.SubdevType & SUB_DEV_TYPE_KEYBOARD)
        Status = I2CKbdConfigureDevice(HidDevInfo);
    else if (HidDevInfo->ReportStruc.SubdevType & SUB_DEV_TYPE_POINT)
        Status = I2CAbsConfigureDevice(HidDevInfo);
    else if (HidDevInfo->ReportStruc.SubdevType & SUB_DEV_TYPE_MOUSE)
        Status = I2CMsConfigureDevice(HidDevInfo);
    else
        return EFI_DEVICE_ERROR;

    return Status;
}

/**
    Start process to get HID information from I2C device by HID interface

    @param[in] Event
    @param[in] Context

    @retval EFI_SUCCESS The function completed successfully

**/

EFI_STATUS
EFIAPI
I2cHidStart (
    IN EFI_EVENT   Event,
    IN VOID        *Context
)
{
    EFI_STATUS      Status;
    HID_DEV_INFO    *HidDevInfo;
    HID_DEV_INFO    TempDevInfo;
    UINT8           Index;

    DEBUG((DEBUG_INFO,"[I2C HID] I2cHidStart Start\n"));

    Status = gBS->LocateProtocol(&gEfiAmiI2CProtocolGuid,NULL,&AmiI2C );
    DEBUG((DEBUG_INFO,"[I2C HID] I2cHidStart LocateProtocol Status:%r \n",Status));

    for (Index=0;Index<gHidDevCount;Index++) {
        DEBUG((DEBUG_INFO,"[I2C HID] I2cHidStart gHidDevCount:%x \n",Index));

#if I2CHID_DEVICE_NUMBER == 0x01
        if (gHidDevTable[Index].BusNo != I2CHID_DEV_0_BUS_NUMBER) continue;
#endif

        gBS->SetMem(&TempDevInfo,sizeof(HID_DEV_INFO),0);

        TempDevInfo.BusNo = gHidDevTable[Index].BusNo;
        TempDevInfo.SlaveAddress = gHidDevTable[Index].SlaveAddress;
        TempDevInfo.HidDescAddress = gHidDevTable[Index].HidDescAddress;
        TempDevInfo.IntSts = gHidDevTable[Index].GetIntSts;
        TempDevInfo.HwReset = gHidDevTable[Index].HwReset; 

        Status = GetHidDescriptor(&TempDevInfo);
        DEBUG((DEBUG_INFO,"[I2C HID] GetHidDescriptor:%r,Bus:%02x,SlaveAddr:%x\n"
                    ,Status, TempDevInfo.BusNo, TempDevInfo.SlaveAddress));
        if (EFI_ERROR(Status)) {
            continue;
        }

        gBS->AllocatePool(EfiBootServicesData,sizeof(HID_DEV_INFO),&HidDevInfo);
        gBS->CopyMem(HidDevInfo,&TempDevInfo,sizeof(HID_DEV_INFO));

        Status = ConfigureHidDevice(HidDevInfo);
        DEBUG((DEBUG_INFO,"[I2C HID] ConfigureHidDevice %r\n",Status));
        if (EFI_ERROR(Status)) {
            gBS->FreePool(HidDevInfo);
            continue;
        }
    }
    return EFI_SUCCESS;
}

/**
    HID over I2C entry point.

    @param[in] ImageHandle - Image handle for this driver
    @param[in] SystemTable - Pointer to the EFI system table

    @retval EFI_SUCCESS - The function completed successfully

**/

EFI_STATUS
EFIAPI
I2cHidEntryPoint (
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS  Status;
#if ENABLE_I2C_RESET == 0x01
    UINT8       Index;
#endif

//
// HW Reset for all I2C HID device
//
#if ENABLE_I2C_RESET == 0x01
    DEBUG((DEBUG_INFO, "[I2C HID] I2C HID device Reseting. \n"));
    for (Index=0;Index<gHidDevCount;Index++) {
        if (gHidDevTable[Index].HwReset != NULL)
           gHidDevTable[Index].HwReset();
    }
    DEBUG((DEBUG_INFO, "[I2C HID] I2C HID device Resed. \n"));
#endif

    //
    // RegisterProtocolCallback
    //
    Status = gBS->CreateEvent(
                EVT_NOTIFY_SIGNAL,
                TPL_CALLBACK, 
                I2cHidStart, 
                NULL, 
                &gAllDriverConnectedEvent);
    DEBUG((DEBUG_INFO, "[I2C HID] CreateEvent Status:%r \n",Status));
    ASSERT_EFI_ERROR(Status);
    Status = gBS->RegisterProtocolNotify(
                &gEfiDxeMouseDriverProtocolGuid, 
                gAllDriverConnectedEvent, 
                &gAllDriverConnectedNotifyRegistration);
    DEBUG((DEBUG_INFO, "[I2C HID] RegisterProtocolNotify Status:%r \n",Status));

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
