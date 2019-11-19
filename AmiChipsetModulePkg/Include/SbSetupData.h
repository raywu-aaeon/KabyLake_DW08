//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//*************************************************************************
// $Header: /Alaska/BIN/Chipset/Intel/SouthBridge/LynxPoint/Intel Pch SB Board/SbSetupData.h 3     5/03/12 6:33a Victortu $
//
// $Revision: 3 $
//
// $Date: 5/03/12 6:33a $
//*************************************************************************
/** @file SbSetupData.h
    South Bridge setup data header file, define all the South
    Bridge setup items and a structures in this file. 

    @note  The context of the SB_SETUP_DATA may be able to copy from
              SB.SD directly 

**/
//*************************************************************************

#ifndef __SB_SETUP_DATA_H__ // To Avoid this header get compiled twice
#define __SB_SETUP_DATA_H__

#include <Efi.h>

#include <Setup.h>
#include <SbPeiInitElink.h>

#define SB_SETUP_DATA_SIG   0x44534253  // "SBSD" 

#ifdef __cplusplus
extern "C" {
#endif

typedef struct  _SB_SETUP_DATA  SB_SETUP_DATA;

#pragma pack(push, 1)

typedef struct {
  BOOLEAN UsbOverCurrentPinOverride;
  UINT8	  Usb20PinNum;
  UINT8	  Usb30PinNum;
  UINT8   *Usb20PinMapping;
  UINT8   *Usb30PinMapping;
} USB_OVER_CURRENT_MAPPING;

typedef struct _SB_SETUP_DATA {
    UINT8   EhciConDisConWakeUp;
#if USB_OVERCURRENT_PIN_OVERRIDE_USE_ELINK    
    //
    // USB OverCurrent Pin Mapping
    //
    USB_OVER_CURRENT_MAPPING	UsbOverCurrentPinMapping;
#endif    
    //
    // Include Setup Data of PCH RC
    //
    #include <KabylakeSiliconPkg/Wrapper/Include/PchSetupData.h>

} SB_SETUP_DATA;

#pragma pack(pop)

VOID GetSbSetupData (
    IN VOID                 *Service,
    IN OUT SB_SETUP_DATA    *SbSetupData,
    IN BOOLEAN              Pei
);

typedef VOID (SB_OEM_SETUP_CALLBACK) (
    IN VOID                 *Services,
    IN OUT SB_SETUP_DATA    *SbSetupData,
    IN SETUP_DATA           *SetupData,
    IN BOOLEAN              Pei
);
/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif 

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
