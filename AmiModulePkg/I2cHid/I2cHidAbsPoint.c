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

/** @file I2cHidAbsPoint.c
    AMI I2C Absolute Device support implementation

**/

//---------------------------------------------------------------------------

#include <Efi.h>
#include <Protocol/DriverBinding.h>
#include <I2cHid.h>
#include <Protocol/AbsPointerProtocol.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

//---------------------------------------------------------------------------

UINT8           PreviousTipStatus=0;
HID_DEV_INFO    *AbsHidDevInfo;

/**
    Get information from device.

    @param[in] AbsPointProtocol - Absolute pointer protocol

    @retval HidDevInfo - Device information

**/

HID_DEV_INFO *
GetDeviceInfoInterface (
    IN EFI_ABSOLUTE_POINTER_PROTOCOL *AbsPointProtocol
)
{
//    HID_DEV_INFO *AbsHidDevInfo;

//    AbsHidDevInfo = OUTTER(AbsPointProtocol, AbsStruc, HID_DEV_INFO);
//    return AbsHidDevInfo;
    return NULL;
}

/**
    Get report data from I2C device.

    @param[in] Data - Pointer of data
    @param[in] Start - Start offset of data 
    @param[in] End - End offset of data

    @retval temp_data - Report data

**/

UINTN
GetReportData (
    IN UINT8 *Data,
    IN UINTN Start,
    IN UINTN End
)    
{
    UINT8   ReportSize;
    UINT8   Size;
    UINT8   PreSkip;
    UINT8   PostSkip;
    UINTN   TempData;

    ReportSize = (UINT8)(End - Start);
    Size =  ReportSize / 8;
    if ((ReportSize%8)!=0 ) Size++;

    switch (Size) {
        case 1: // Byte
            TempData = *(Data+Start/8);
            break;		
        case 2: // Word
            TempData = *(UINT16*)(Data+Start/8);
            break;
        case 4: // Dword
            TempData = *(UINT32*)(Data+Start/8); 
            break;
        default:
            break;
    }

    PreSkip = Start % 8;
    PostSkip = End % 8;

    if (PreSkip != 0)
        TempData = TempData >> PreSkip;
    if (PostSkip != 0) {
        TempData = TempData << PostSkip; 
        TempData = TempData >> PostSkip;
    }

    return TempData;
}

/**
    Receive the data then get x,y value pass point to absolute protocol.

    @param[in] HidDevInfo - Struct for I2C HID device information
    @param[in] Data - Read data from I2C device
    @param[in, out] State - Struct for Absolute pointer

    @retval NONE

**/

VOID
ProcessAbsPointerData (
    IN HID_DEV_INFO *HidDevInfo,
    IN UINT8 *Data,
    IN OUT EFI_ABSOLUTE_POINTER_STATE *State
)
{
    // I2C report data format
    // offset 0~1 : Length of packet
    // offset 2   : Report ID
    // offset 3~  : the following format is based on Report Descriptor
    UINT8   ReportID = Data[2];
    UINT8   Index1;
    UINT8   Index2;
	UINTN   OffsetTemp = 0;
    UINTN   XStart = 0;
    UINTN   XEnd = 0;
    UINTN   YStart = 0;
    UINTN   YEnd = 0;
    UINTN   TipStart = 0;
    UINTN   TipEnd = 0;
    UINTN   ContactIDStart = 0;
    UINTN   ContactIDEnd = 0;
    HIDReport_STRUC *Hidreport = &(HidDevInfo->ReportStruc);
    UINTN   Tip;
    UINTN   X;
    UINTN   Y;
    UINTN   ContactID;
    UINTN   MinContactID;

    //
    // Skip length, let pointer start from Report ID
    //
    Data += 2; 

    //
    // Search tip, contact ID, and X Y
    // X (page 0x01, usage 0x30)
    // Y (page 0x01, usage 0x31)
    // Tip switch (page 0x0D, usage 0x42)
    // Contact ID (page 0x0D, usage 0x51)
    //
    for (Index1=0;Index1<Hidreport->TotalCount;Index1++) {
        //Check is input?
        if ((Hidreport->Report[Index1].Flag & HID_BFLAG_INPUT )) {
            //if report id exist, check report id
            if (Hidreport->Flag & HID_REPORT_BFLAG_REPORT_ID) {
                if (Hidreport->Report[Index1].ReportID != ReportID)
                	continue;
            }

            //Check touch tip and touch identifier
            if ((Hidreport->Report[Index1].UsagePage == HID_UP_DIGITIZER)&&(Hidreport->Report[Index1].UsageCount)!=0) { 
                // Search 
                for (Index2=0;Index2<Hidreport->Report[Index1].UsageCount;Index2++) {
                    //Check Tip Switch
                    if (Hidreport->Report[Index1].Usage[Index2] == HID_DIGITIZER_TIP_SWITCH) {
                        if (TipStart != 0)
                        	break;
                        TipStart=(OffsetTemp+Index2*Hidreport->Report[Index1].ReportSize);                        
                        if (Hidreport->Flag & HID_REPORT_BFLAG_REPORT_ID)
                        	TipStart += 8; // First byte is Report ID, so add 8 bit
                        TipEnd = TipStart + Hidreport->Report[Index1].ReportSize;
                        Tip = GetReportData(Data,TipStart,TipEnd);
                    }

                    //Check Contact ID (for mult touch device)
                    if (Hidreport->Report[Index1].Usage[Index2] == HID_DIGITIZER_DYNAMIC_VALUE) {
                        if (ContactIDStart != 0)
                        	break;
                        ContactIDStart = (OffsetTemp+Index2*Hidreport->Report[Index1].ReportSize);
                        if (Hidreport->Flag & HID_REPORT_BFLAG_REPORT_ID)
                        	ContactIDStart += 8; // First byte is Report ID, so add 8 bit
                        ContactIDEnd = ContactIDStart + Hidreport->Report[Index1].ReportSize;
                        ContactID = GetReportData(Data,ContactIDStart,ContactIDEnd);
                        MinContactID = Hidreport->Report[Index1].LogicalMin;

                        //
                        // If these report data is not for first contact point,
                        // skip this data. We don't handle mult touch condition.
                        //
                        if (ContactID != MinContactID) {
                            DEBUG((DEBUG_INFO,"[I2C HID] Mult touch!, skip this data\n"));
                            return;
                        }

                    }
                }
            }

            //Check X,Y
            if ((Hidreport->Report[Index1].UsagePage == HID_UP_GENDESK)&&(Hidreport->Report[Index1].UsageCount)!=0) {
                // Search 
                for (Index2=0;Index2<Hidreport->Report[Index1].UsageCount;Index2++)
                {
                    // Find X
                    if (Hidreport->Report[Index1].Usage[Index2] == HID_GENDESK_X) {
                        if (XStart!=0)
                        	break;
                        XStart = (OffsetTemp+Index2*Hidreport->Report[Index1].ReportSize);
                        if (Hidreport->Flag & HID_REPORT_BFLAG_REPORT_ID)
                        	XStart += 8;
                        XEnd = XStart + Hidreport->Report[Index1].ReportSize;
                        X = GetReportData(Data,XStart,XEnd);
                        
                    }

                    // Find Y
                    if (Hidreport->Report[Index1].Usage[Index2] == HID_GENDESK_Y) {
                        if (YStart!=0)
                        	break;
                        YStart = (OffsetTemp+Index2*Hidreport->Report[Index1].ReportSize);
                        if (Hidreport->Flag & HID_REPORT_BFLAG_REPORT_ID)
                        	YStart += 8;  
                        YEnd=YStart + Hidreport->Report[Index1].ReportSize;
                        Y = GetReportData(Data,YStart,YEnd);
                    }
                }
            }

            OffsetTemp += (Hidreport->Report[Index1].ReportCount*Hidreport->Report[Index1].ReportSize);
        }
    }

    //
    // Update the touch status
    //
    State->CurrentX = X;
    State->CurrentY = Y;
    State->ActiveButtons = (UINT32)Tip;

}

/**
    Read I2C HID device data

    @param[in] This - Absolute pointer protocol
    @param[out] State - Struct for Absolute pointer

    @retval EFI_STATUS

**/

static EFI_STATUS
EFIAPI
AbsGetState (
    IN EFI_ABSOLUTE_POINTER_PROTOCOL  *This,
    OUT EFI_ABSOLUTE_POINTER_STATE     *State
)
{
    EFI_STATUS      Status;
//    HID_DEV_INFO    *HidDevInfo;
    UINT8           ReadData[DATA_SIZE];
//    UINT8           Index;

    gBS->Stall(DELAY_5MS);

//    AbsHidDevInfo = GetDeviceInfoInterface(This);

    //
    // If there is no data in device, leave this procedure.
    //
    if (AbsHidDevInfo->IntSts()==FALSE) {
        //DEBUG((DEBUG_INFO,"[I2C HID] AbsNotRead Data \n"));
        return EFI_NOT_READY;
    }

    //
    // Read data from device until INT pin become high (no data) 
    //
    while (AbsHidDevInfo->IntSts()==TRUE) {
//        DEBUG((DEBUG_INFO,"[I2C HID] AbsGetting Data \n"));
        Status = GetI2CDeviceData(AbsHidDevInfo,ReadData,AbsHidDevInfo->HidDescriptor.MaxInputLength);
        if (EFI_ERROR(Status)) {
//            DEBUG((DEBUG_ERROR,"[I2C HID] AbsGetState error\n"));
            return Status;
        }
//        DEBUG((DEBUG_INFO,"[I2C HID] AbsGetState Status %r\n",Status));
//        for (Index=0;Index<AbsHidDevInfo->HidDescriptor.MaxInputLength;Index++) {
//        	DEBUG((DEBUG_INFO, "%02X ", ReadData[Index]));
//        }
//        DEBUG((DEBUG_INFO, "\n"));
//if (ReadData[0] == 0)
//    return EFI_NOT_READY;
        ProcessAbsPointerData(AbsHidDevInfo,ReadData,State);

        DEBUG((DEBUG_INFO,"[I2C HID] X:%x\n",State->CurrentX));
        DEBUG((DEBUG_INFO,"[I2C HID] Y:%x\n",State->CurrentY));
        DEBUG((DEBUG_INFO,"[I2C HID] Tip:%x\n",State->ActiveButtons));

        //
        // If tip status changed, must report this data
        //
        if (PreviousTipStatus != State->ActiveButtons)goto AbsGetStateExit; // break; //return EFI_SUCCESS;
        PreviousTipStatus = State->ActiveButtons;

        //
        // If finger is leaved, must report this data
        //
        if (State->ActiveButtons == 0) goto AbsGetStateExit; //break; //return EFI_SUCCESS;
    }
AbsGetStateExit:
    PreviousTipStatus = State->ActiveButtons;
//    DEBUG((DEBUG_INFO,"[I2C HID] AbsData Exit \n"));
    return EFI_SUCCESS;
}

/**
    Reset I2C HID device.

    @param[in] This - Absolute pointer protocol
    @param[in] ExtendedVerification

    @retval EFI_SUCCESS - The function completed successfully

**/

static EFI_STATUS
EFIAPI
AbsReset (
    IN EFI_ABSOLUTE_POINTER_PROTOCOL    *This,
    IN BOOLEAN                          ExtendedVerification
)
{   
    DEBUG((DEBUG_INFO,"[I2C HID] AbsReset\n"));
    return EFI_SUCCESS;
}

/**
    Wait input then trigger event.

    @param[in] Event 
    @param[in, out] Context 

    @retval NONE

**/

static VOID
EFIAPI
AbsWaitForInputEvent (
    IN EFI_EVENT Event,
    IN OUT VOID      *Context
)
{
    HID_DEV_INFO *AbsHidDevInfo = (HID_DEV_INFO*)Context;

    // There is data if INT pin is low. signal a event.
    if (AbsHidDevInfo->IntSts()==TRUE) {
//        DEBUG((DEBUG_INFO, "[I2C HID] AbsWaitForInputEvent Data \n"));
        gBS->Stall(DELAY_100MS);
        gBS->SignalEvent(Event);
    }
    return;
}

/**
    Configure the absolute device.

    @param[in] HidDevInfo - Struct for I2C HID device information

    @retval NONE

**/

EFI_STATUS
I2CAbsConfigureDevice (
    IN HID_DEV_INFO *HidDevInfo
)
{
    EFI_STATUS              Status;
    UINT8                   Buffer[DATA_SIZE];
    UINT8                   TimeOutCnt = TIME_OUT_COUNT;
    EFI_HANDLE              DeviceHandle;
    ABSOLUTE_POINTER_STRUC  *AbsStruc = &HidDevInfo->AbsStruc;
    
    AbsHidDevInfo = HidDevInfo;

    Status = SetPower(AbsHidDevInfo);
    DEBUG((DEBUG_INFO, "[I2C HID] Set Power %r\n",Status));
    if (EFI_ERROR(Status)) {
        return Status;
    } 

    Status = SetReset(AbsHidDevInfo);
    DEBUG((DEBUG_INFO, "[I2C HID] Set Reset %r\n",Status));
    if (EFI_ERROR(Status)) {
        return Status;
    } 

    //
    // After device reset command, read data from device until INT pin become high.
    // Clean up all data in the device.
    //
    while (AbsHidDevInfo->IntSts() == TRUE) {
        Status = GetI2CDeviceData(AbsHidDevInfo,Buffer,AbsHidDevInfo->HidDescriptor.MaxInputLength);
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

    //
    //  Device initial done.
    //  Prepare and install absolute pointer protocol.
    //
/*    
    Status = SetIdle(AbsHidDevInfo);    
    if (EFI_ERROR(Status)) 
    {
        DEBUG((DEBUG_INFO, "[I2C HID] Set SetIdle %r\n",Status));
        return Status;
    } 
*/    
    AbsStruc->AbsolutePointerProtocol.GetState = AbsGetState;
    AbsStruc->AbsolutePointerProtocol.Reset = AbsReset;
    AbsStruc->AbsolutePointerProtocol.Mode = &AbsStruc->Mode;
    AbsStruc->Mode.Attributes = EFI_ABSP_SupportsPressureAsZ;

    AbsStruc->Mode.AbsoluteMinX = 0;
    AbsStruc->Mode.AbsoluteMinY = 0;
    AbsStruc->Mode.AbsoluteMinZ = 0;     
    AbsStruc->Mode.AbsoluteMaxX = AbsHidDevInfo->ReportStruc.AbsMaxX;
    AbsStruc->Mode.AbsoluteMaxY = AbsHidDevInfo->ReportStruc.AbsMaxY;
    AbsStruc->Mode.AbsoluteMaxZ = 0;    

    gBS->SetMem (&AbsStruc->State, sizeof(EFI_ABSOLUTE_POINTER_STATE), 0);

    Status = gBS->CreateEvent (
            EFI_EVENT_NOTIFY_WAIT,
            TPL_NOTIFY,
            AbsWaitForInputEvent,
            AbsHidDevInfo,
            &((AbsStruc->AbsolutePointerProtocol).WaitForInput));
    if (EFI_ERROR(Status)) return Status;

    //
    // Set DeviceHandle as null for creating a new handle for device.
    //
    DeviceHandle = NULL;

    Status = gBS->InstallProtocolInterface (
            &DeviceHandle,
            &gEfiAbsolutePointerProtocolGuid,
            EFI_NATIVE_INTERFACE,
            &AbsStruc->AbsolutePointerProtocol);
    if (EFI_ERROR(Status)) return Status;

    AbsHidDevInfo->DeviceHandle = DeviceHandle;

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
