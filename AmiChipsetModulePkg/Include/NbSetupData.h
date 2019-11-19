//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.            **
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
// $Header: /Alaska/BIN/Chipset/Intel/NorthBridge/Haswell/Intel SystemAgent NB Board/NbSetupData.h 3     4/26/12 3:00a Yurenlai $
//
// $Revision: 3 $
//
// $Date: 4/26/12 3:00a $
//*************************************************************************
// Revision History
// ----------------
// $Log: /Alaska/BIN/Chipset/Intel/NorthBridge/Haswell/Intel SystemAgent NB Board/NbSetupData.h $
// 
//*************************************************************************

#ifndef __NB_SETUP_DATA_H__ // To Avoid this header get compiled twice
#define __NB_SETUP_DATA_H__

#include <Efi.h>
#include <Setup.h>

#define NB_SETUP_DATA_SIG   0x4453424E  // "NBSD" 

#ifdef __cplusplus
extern "C" {
#endif
   
typedef struct  _NB_SETUP_DATA  NB_SETUP_DATA;

#pragma pack(push, 1)

typedef struct {
  BOOLEAN IsRunMemoryDown;
  UINT16 MemoryDownDdrSpdSize[4];
  UINT8 *MemoryDownDdrSpd[4];
  const UINT8 *OemDqByteMap;
  const UINT8 *OemDqsMap;
} SA_SPD_DATA_BUFFER;

typedef struct {
  BOOLEAN IsOverrideSpdAddress;
  UINT8   SpdAddressTable[4];
} SA_SPD_ADDRESS_BUFFER;

typedef struct _NB_SETUP_DATA {
    //
    // Reserve 0x40 bytes for NB Setup Data.
    //
    UINT32  NbSetupString;      // 0x00
    UINT8   SmbiosLogging;      // 0x04
    UINT8   Reserve1[0x3B];     // 0x5~0x40
    UINT8   PciExpressLength;
    
    //
    // Spd Data Buffer Struct.
    // 
    SA_SPD_DATA_BUFFER   SaMemoryDownInfo;
    //
    // smbus address Struct.
    // 
    SA_SPD_ADDRESS_BUFFER SaSpdAddressInfo;
    //
    // CSM setup data.
    // 
    UINT8	VideoOpRom;
    //
    // Include Setup Data of SA RC
    //
    #include <KabylakeSiliconPkg/Wrapper/Include/SaSetupData.h>

} NB_SETUP_DATA;

#pragma pack(pop)

VOID GetNbSetupData (
    IN VOID                 *Service,
    IN OUT NB_SETUP_DATA    *NbSetupData,
    IN BOOLEAN              Pei
);

typedef VOID (NB_OEM_SETUP_CALLBACK) (
    IN VOID                 *Services,
    IN OUT NB_SETUP_DATA    *NbSetupData,
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
//**        (C)Copyright 1985-2011, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
