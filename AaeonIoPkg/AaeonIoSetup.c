//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file 
 * SioStaticSetup.c
 * Sub Module Strings override routines to dynamically change Setup.
**/ 
//**********************************************************************
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Setup.h>
#include <AutoId.h>
#include <AmiGenericSio.h>
#include <AaeonIoSetup.h>


/**
    Update option with the data read from register

        
    @param RegData data from SIO registers
    @param Func Fan Speed,Voltage and Temperature
    @param StrToken String token
    @param OddPos Odd position
    @param hiiHandle HII Handle

    @retval 
        Return string in Buffer

**/

VOID EFIAPI AaeonHHMCommon(
    IN      UINT64     RegData,
    IN      UINT8     Func,
    IN      UINT16    StrToken,
    IN      UINT8     OddPos,
    IN      EFI_HII_HANDLE    hiiHandle
) {
    CHAR16          TailRound[]     = L" RPM";          //Fan uint
    CHAR16          TailVoltg[]     = L" V";            //Voltage uint
//  CHAR16          TailTempt[]     = L" C";            //Temperature uint
    CHAR16          TailTempt[]     = L" \x2103";       //Temperature uint
    CHAR16          LeadingMini[]   = L": -";           //Fix
    CHAR16          LeadingPlus[]   = L": +";           //Fix
    CHAR16          LeadingSpac[]   = L": ";            //Fix
    CHAR16          AllSpace[]      = L": N/A       ";  //Fix
    CHAR16          *TempStr        = AllSpace;
    UINT8           StrLen;
    INT64           NData;
    BOOLEAN         PN_Flag=0;  //Positive(0) or Negative(1) value    
    CHAR16          StrUp[STR_BUFFER_LENGTH] = L": N/A     "; //Don't change this line
    CHAR16          *StrUpdated = StrUp;
    BOOLEAN         pflag = 1;
    
    NData = (INT64)(RegData);

    if((Func == TEMPERATURE) & (NData > 0x7F) & (NData <= 0xFF)) //If get temperature data, and data range is 128~255, define minus data
    {
        pflag = 0;               //Use symbol "-"
        NData = 0x100 - NData;     //Convert minus data
    }    
 
    //add negative value handle
    if(NData<0) {
       TRACE((-1,"NData=%d\n",NData));
       NData=-NData;
       PN_Flag=1;
       TRACE((-1,"NData=%d\n",NData));
    }    
    
    //The following may be ported by each SIO
    //As now, it support max length is five
    if(NData>9999)      StrLen = 0x5;
    else if(NData>999)  StrLen = 0x4;
    else if(NData>99)   StrLen = 0x3;
    else if(NData>9)    StrLen = 0x2;
    else                StrLen = 0x1;

    //When device not present, update to 'N/A'
    if(NData == 0x00) StrUpdated = StrUp;
    else {
        HHMEfiValueToString(TempStr, NData, 0, StrLen);
        switch(Func) {
            case    VOLTAGE:                       //Indicate it's voltage
                if(PN_Flag) {
                    AdjustString(StrUpdated, TempStr, StrLen, LeadingMini, 0x03,\
                                OddPos?TRUE:FALSE, OddPos, TailVoltg, 0x02);
                }else {
                    AdjustString(StrUpdated, TempStr, StrLen, LeadingPlus, 0x03,\
                                OddPos?TRUE:FALSE, OddPos, TailVoltg, 0x02);
                }
                break;
            case    TEMPERATURE:                   //Indicate it's Temperature                                                                      
                    AdjustString(StrUpdated, TempStr, StrLen, pflag?LeadingPlus:LeadingMini, 0x03,\
                                                OddPos?TRUE:FALSE, OddPos, TailTempt, 0x02);
                break;
            case    FAN_SPEED:                     //Indicate it's fan
                AdjustString(StrUpdated, TempStr, StrLen, LeadingSpac, 0x02,\
                            OddPos?TRUE:FALSE, OddPos, TailRound, 0x04);
                break;
            default :                              //Default to " N/A "
                break;
        }
    }
    InitString(hiiHandle, StrToken, StrUpdated);

    return;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
