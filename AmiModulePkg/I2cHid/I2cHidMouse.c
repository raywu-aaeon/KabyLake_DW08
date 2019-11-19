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

/** @file I2cHidMouse.c
    AMI I2C HID mouse support implementation

**/

//---------------------------------------------------------------------------

#include <Efi.h>
#include <Protocol/DriverBinding.h>
#include <I2cHid.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>

//---------------------------------------------------------------------------

HID_DEV_INFO *SimpHidDevInfo;

/**
    Get report data from I2C device.

    @param[in] Data - Pointer of data
    @param[in] Start - Start offset of data 
    @param[in] End - End offset of data

    @retval TempData - Report data

**/

UINTN
GetReportMouseData (
    IN UINT8 *Data,
    IN UINTN Start,
    IN UINTN End
)    
{
    UINTN   ReportSize;
    UINTN   Size;
    UINTN   PreSkip;
    UINTN   PostSkip;
    UINTN   TempData;

    ReportSize = End - Start;
    Size =  ReportSize /8;
    if ((ReportSize%8)!=0 )
    	Size++;

    switch (Size) {
        case 1: // byte
            TempData = *(Data+Start/8);
            break;
        case 2: // word
            TempData = *(UINT16*)(Data+Start/8);
            break;
        case 4: // dword
            TempData = *(UINT32*)(Data+Start/8); 
            break;
        default:
            break;
    }

    PreSkip =  Start % 8;
    PostSkip =  End % 8;

    if (PreSkip != 0)
        TempData=TempData >> PreSkip;
    if (PostSkip != 0)
    {
        TempData=TempData << PostSkip;
        TempData=TempData >> PostSkip;
    }

    return TempData;
}

/**
    Receive the data then get x,y value pass point to 
    simple pointer protocol.

    @param[in] HidDevInfo - Struct for I2C HID device information
    @param[in] Data - Read data from I2C device
    @param[in, out] State - Struct for simple pointer pointer

    @retval NONE

**/

VOID
ProcessMouseData (
    IN HID_DEV_INFO *HidDevInfo,
    IN UINT8 *Data,
    IN OUT EFI_SIMPLE_POINTER_STATE *State
)
{
    // I2C report data format
    // offset 0~1 : Length of packet
    // offset 2   : Report ID
    // offset 3~  : the following format is based on Report Descriptor
    UINT8   ReportID = Data[2];
    UINT8   Index1;
    UINT8   Index2;
	UINTN   OffsetTmp = 0;
    UINTN   XStart = 0;
    UINTN   XEnd = 0;
    UINTN   YStart = 0;
    UINTN   YEnd = 0;
    UINTN   ButtonStart = 0;
//    UINTN   ButtonEnd = 0;
    HIDReport_STRUC *Hidreport = &(HidDevInfo->ReportStruc);
    UINTN   X=0;
    UINTN   Y=0;
    UINTN   ButtonActive;

    //
    // Skip length, let pointer start from Report ID
    //
    Data += 2; 

    //
    // Serach Button active, and  X Y
    // X (page 0x01, usage 0x30)
    // Y (page 0x01, usage 0x31)
    // Button size (page 0x09, report ID 0x08)

    for (Index1=0;Index1<Hidreport->TotalCount;Index1++) {
        // Check is input?
        if ((Hidreport->Report[Index1].Flag & HID_BFLAG_INPUT )) {
            // If report id exist, check report id
            if (Hidreport->Flag & HID_REPORT_BFLAG_REPORT_ID) {
                if (Hidreport->Report[Index1].ReportID != ReportID)
                	continue;
            }

            // Check ButtonActive 
            if ((Hidreport->Report[Index1].UsagePage == HID_UP_BUTTON)&&(Hidreport->Report[Index1].UsageMin[0] == HID_BUTTON_BUTTON1)) { 
                if (ButtonStart != 0)
                	break;
//                ButtonStart=(OffsetTmp+Hidreport->Report[i].ReportSize);
                ButtonStart = OffsetTmp;                      
                if (Hidreport->Flag & HID_REPORT_BFLAG_REPORT_ID)
                	ButtonStart+=8; // First byte is Report ID, so add 8 bit

//                ButtonEnd = ButtonStart + Hidreport->Report[i].ReportSize;
//                ButtonActive = (UINT32)GetReportMouseData(Data,ButtonStart,ButtonEnd);
                ButtonStart /= 8;
                ButtonActive = *(Data+ButtonStart); 
                DEBUG((DEBUG_INFO,"[I2C HID] ProcessAbsPointerData-Tip:%x\n",ButtonActive));

            }

            // Check X,Y
            if ((Hidreport->Report[Index1].UsagePage == HID_UP_GENDESK)&&(Hidreport->Report[Index1].UsageCount)!=0) {
                // Search 
                for (Index2=0;Index2<Hidreport->Report[Index1].UsageCount;Index2++) {
                    // Find X
                    DEBUG((DEBUG_INFO,"[I2C HID] Hidreport->Report[%x].Usage[%x]:%x\n",Index1,Index2,Hidreport->Report[Index1].Usage[Index2]));
                    if (Hidreport->Report[Index1].Usage[Index2] == HID_GENDESK_X) {
                        if (XStart!=0)
                        	break;
                        XStart=(OffsetTmp+Index2*Hidreport->Report[Index1].ReportSize);
                        if (Hidreport->Flag & HID_REPORT_BFLAG_REPORT_ID)
                        	XStart+=8;
                        DEBUG((DEBUG_INFO,"[I2C HID] Hidreport->Report[i].ReportSize:%x\n",Hidreport->Report[Index1].ReportSize));

                        XEnd = XStart + Hidreport->Report[Index1].ReportSize;

                        DEBUG((DEBUG_INFO,"[I2C HID] XStart:%x XEnd:%x\n",XStart, XEnd));
                        X = GetReportMouseData(Data,XStart,XEnd);
                        DEBUG((DEBUG_INFO,"[I2C HID] ProcessAbsPointerData-X:%x\n",X));
                    }

                    // Find Y
                    if (Hidreport->Report[Index1].Usage[Index2] == HID_GENDESK_Y) {
                        if (YStart!=0)
                        	break;
                        YStart=(OffsetTmp+Index2*Hidreport->Report[Index1].ReportSize);
                        if (Hidreport->Flag & HID_REPORT_BFLAG_REPORT_ID)
                        	YStart+=8;
                        YEnd=YStart + Hidreport->Report[Index1].ReportSize;
                        Y = GetReportMouseData(Data,YStart,YEnd);
                        DEBUG((DEBUG_INFO,"[I2C HID] ProcessAbsPointerData-Y:%x\n",Y));
                    }
                }
            }

            OffsetTmp += (Hidreport->Report[Index1].ReportCount*Hidreport->Report[Index1].ReportSize);
        }
    }

    //
    //  Update the touch status
    //
    State->RelativeMovementX = (INT8)X;
    State->RelativeMovementY = (INT8)Y;
    State->RelativeMovementZ = 0;
    State->LeftButton = (ButtonActive & BIT0)?TRUE:FALSE;
    State->RightButton = (ButtonActive & BIT1)?TRUE:FALSE;
}

/**
    Read I2C HID device data

    @param[in] This - Simple pointer pointer protocol
    @param[out] State - Struct for Simple pointer pointer

    @retval EFI_STATUS

**/

static EFI_STATUS
EFIAPI
SimplePonterGetState (
    IN EFI_SIMPLE_POINTER_PROTOCOL  *This,
    OUT EFI_SIMPLE_POINTER_STATE     *State
)
{
    EFI_STATUS  Status;
    UINT8       ReadData[DATA_SIZE];
//    UINT8       Index;
//    UINT16      *PtrX, *PtrY;

    //
    // If there is no data in device, leave this procedure.
    //
    if (SimpHidDevInfo->IntSts() == FALSE) return EFI_NOT_READY;

    gBS->SetMem(ReadData,sizeof(ReadData),0);

    Status = GetI2CDeviceData(SimpHidDevInfo,ReadData,SimpHidDevInfo->HidDescriptor.MaxInputLength);
    if (EFI_ERROR(Status)) {
        DEBUG((DEBUG_ERROR,"[I2C HID] SimpGetState error %r\n",Status));
        return Status;
    }
/*
    DEBUG((DEBUG_INFO,"[I2C HID] SimplePonterGetState:\n "));
    for (Index=0;Index<SimpHidDevInfo->HidDescriptor.MaxInputLength;Index++){
        DEBUG((DEBUG_INFO,"%02X ", ReadData[Index]));
    }
    DEBUG((DEBUG_INFO,"\n"));
*/

    ProcessMouseData(SimpHidDevInfo,ReadData,State);
/*
    PtrX = (INT16*)&ReadData[4];
    PtrY = (INT16*)&ReadData[6];

    State->RelativeMovementX = *PtrX;
    State->RelativeMovementY = *PtrY;
	State->RelativeMovementZ = 0;
    State->LeftButton = (ReadData[3] & BIT0)?TRUE:FALSE;
    State->RightButton = (ReadData[3] & BIT1)?TRUE:FALSE;
*/
    DEBUG((DEBUG_INFO,"[I2C HID] X:%x\n",State->RelativeMovementX));
    DEBUG((DEBUG_INFO,"[I2C HID] Y:%x\n",State->RelativeMovementY));
    DEBUG((DEBUG_INFO,"[I2C HID] Left:%x\n",State->LeftButton));
    DEBUG((DEBUG_INFO,"[I2C HID] Right:%x\n",State->RightButton));

    return EFI_SUCCESS;
}

/**
    Reset I2C HID device.

    @param This - Simple pointer protocol
    @param ExtendedVerification

    @retval EFI_SUCCESS - The function completed successfully

**/

static EFI_STATUS
EFIAPI
SimplePointerReset (
    IN EFI_SIMPLE_POINTER_PROTOCOL  *This,
    IN BOOLEAN                      ExtendedVerification
)
{
    DEBUG((DEBUG_INFO,"[I2C HID] AbsReset\n"));
    return EFI_SUCCESS;
}

/**
    Wait input then trigger event.

    @param[in] Event 
    @param[in] Context 

    @retval NONE

**/

static VOID
EFIAPI
SimplePointerWaitForInputEvent (
    IN EFI_EVENT Event,
    IN VOID      *Context
)
{
    HID_DEV_INFO *SimpHidDevInfo = (HID_DEV_INFO*)Context;

    // There is data if INT pin is low. signal a event.
    if (SimpHidDevInfo->IntSts() == TRUE) {
        gBS->SignalEvent(Event);
    }

    return;
}

/**
    Configure the simple pointer device.

    @param[in] pHidDevInfo - Struct for I2C HID device information

    @retval NONE

**/

EFI_STATUS
I2CMsConfigureDevice (
    IN HID_DEV_INFO *HidDevInfo
)
{

    EFI_STATUS              Status;
    UINT8                   Buffer[DATA_SIZE];
    UINT8                   TimeOutCnt = TIME_OUT_COUNT;
    EFI_HANDLE              DeviceHandle;
    SIMPLE_POINTER_STRUC    *SimpStruc = &HidDevInfo->SimpStruc;

    SimpHidDevInfo = HidDevInfo;

    Status = SetPower(SimpHidDevInfo);
    DEBUG((DEBUG_INFO,"[I2C HID] Set Power %r\n",Status));
    if (EFI_ERROR(Status)) {
        return Status;
    }
    Status = SetReset(SimpHidDevInfo);
    DEBUG((DEBUG_INFO,"[I2C HID] Set Reset %r\n",Status));
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
            DEBUG((DEBUG_INFO,"[I2C HID] Too much data keep in device during initial.\n"));
            break;
        }
    }

    //
    // Device initial done.
    // Prepare and install absolute pointer protocol.
    //

    SimpStruc->SimplePointerProtocol.GetState = SimplePonterGetState;
    SimpStruc->SimplePointerProtocol.Reset = SimplePointerReset;
    SimpStruc->SimplePointerProtocol.Mode = &SimpStruc->Mode;

    SimpStruc->Mode.ResolutionX = 2;
    SimpStruc->Mode.ResolutionY = 2;
    SimpStruc->Mode.ResolutionZ = 2;
    SimpStruc->Mode.RightButton = TRUE;
    SimpStruc->Mode.LeftButton = TRUE;

    gBS->SetMem (&SimpStruc->State, sizeof(EFI_SIMPLE_POINTER_STATE), 0);

    Status = gBS->CreateEvent (
            EFI_EVENT_NOTIFY_WAIT,
            TPL_NOTIFY,
            SimplePointerWaitForInputEvent,
            SimpHidDevInfo,
            &((SimpStruc->SimplePointerProtocol).WaitForInput));
    if (EFI_ERROR(Status)) return Status;

    //
    //  Set DeviceHandle as null for creating a new handle for device.
    //
    DeviceHandle = NULL;

    Status = gBS->InstallProtocolInterface(
            &DeviceHandle,
            &gEfiSimplePointerProtocolGuid,
            EFI_NATIVE_INTERFACE,
            &SimpStruc->SimplePointerProtocol);
    if (EFI_ERROR(Status)) return Status;

    gBS->ConnectController(DeviceHandle,NULL,NULL,TRUE);
    SimpHidDevInfo->DeviceHandle = DeviceHandle;

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
