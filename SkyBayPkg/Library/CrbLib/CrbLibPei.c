//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
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
/** @file CrbLib.c
    This file contains Chipset Reference Board related code 
    that is needed for both PEI & DXE stage. 
    To avoid code duplication this file is made as a library 
    and linked both in PEI & DXE CRB FFS.

    @note  MAKE SURE NO PEI OR DXE SPECIFIC CODE IS NEEDED

**/
//*************************************************************************

//----------------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------------

#include <Efi.h>
#include <Token.h>
#include <Setup.h>
#include <NbSetupData.h>
#include <SbSetupData.h>
#include <AmiCspLib.h>
#include <PlatformBoardId.h>
#include <Ppi/SiPolicy.h>
#include <Library/DebugLib.h>
// Produced Protocols

// Consumed Protocols

//----------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//----------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)

//----------------------------------------------------------------------------
// Variable and External Declaration(s)
//----------------------------------------------------------------------------
// Variable Declaration(s)

// GUID Definition(s)

// Protocol Definition(s)

// External Declaration(s)

// Function Definition(s)

//----------------------------------------------------------------------------
UINT32 DW10_OemHdaVerbTblSample[] = {
//===== HDA Codec Subsystem ID Verb-table =====
//HDA Codec Subsystem ID  : 0x10EC0000
0x00172000,
0x00172100,
0x001722EC,
0x00172310,


//===== Pin Widget Verb-table =====
//Widget node 0x01 :
0x0017FF00,
0x0017FF00,
0x0017FF00,
0x0017FF00,
//Pin widget 0x11 - S/PDIF-OUT2
0x01171CF0,
0x01171D11,
0x01171E11,
0x01171F41,
//Pin widget 0x12 - DMIC
0x01271CF0,
0x01271D11,
0x01271E11,
0x01271F41,
//Pin widget 0x14 - FRONT (Port-D)
0x01471C10,
0x01471D40,
0x01471E01,
0x01471F01,
//Pin widget 0x15 - SURR (Port-A)
0x01571CF0,
0x01571D11,
0x01571E11,
0x01571F41,
//Pin widget 0x16 - CEN/LFE (Port-G)
0x01671CF0,
0x01671D11,
0x01671E11,
0x01671F41,
//Pin widget 0x17 - SIDESURR (Port-H)
0x01771CF0,
0x01771D11,
0x01771E11,
0x01771F41,
//Pin widget 0x18 - MIC1 (Port-B)
0x01871C20,
0x01871D90,
0x01871EA1,
0x01871F01,
//Pin widget 0x19 - MIC2 (Port-F)
0x01971CF0,
0x01971D11,
0x01971E11,
0x01971F41,
//Pin widget 0x1A - LINE1 (Port-C)
0x01A71C2F,
0x01A71D30,
0x01A71E81,
0x01A71F01,
//Pin widget 0x1B - LINE2 (Port-E)
0x01B71CF0,
0x01B71D11,
0x01B71E11,
0x01B71F41,
//Pin widget 0x1C - CD-IN
0x01C71CF0,
0x01C71D11,
0x01C71E11,
0x01C71F41,
//Pin widget 0x1D - BEEP-IN
0x01D71CF0,
0x01D71D11,
0x01D71E11,
0x01D71F41,
//Pin widget 0x1E - S/PDIF-OUT
0x01E71CF0,
0x01E71D11,
0x01E71E11,
0x01E71F41,
//Pin widget 0x1F - S/PDIF-IN
0x01F71CF0,
0x01F71D11,
0x01F71E11,
0x01F71F41,
//Widget node 0x20 :
0x02050004,
0x02040001,
0x02050004,
0x02040001
};

//ray_override / [XI-BringUp] Bring Up Porting / Tuning Board Override / Support ALC662 / Added >>
UINT32 Deepware_H110_ALC662[] = {
//===== HDA Codec Subsystem ID Verb-table =====
//HDA Codec Subsystem ID  : 0x10EC0662
0x00172062,
0x00172106,
0x001722EC,
0x00172310,

//===== Pin Widget Verb-table =====
//Widget node 0x01 :
0x0017FF00,
0x0017FF00,
0x0017FF00,
0x0017FF00,
//Pin widget 0x14 - FRONT (Port-D)
0x01471C10,
0x01471D40,
0x01471E01,
0x01471F01,
//Pin widget 0x15 - SURR (Port-A)
0x01571C00,
0x01571DC0,
0x01571E0C,
0x01571F40,
//Pin widget 0x16 - CEN/LFE (Port-G)
0x01671CF0,
0x01671D11,
0x01671E11,
0x01671F41,
//Pin widget 0x18 - MIC1 (Port-B)
0x01871C20,
0x01871D90,
0x01871EA1,
0x01871F01,
//Pin widget 0x19 - MIC2 (Port-F)
0x01971CF0,
0x01971D11,
0x01971E11,
0x01971F41,
//Pin widget 0x1A - LINE1 (Port-C)
0x01A71C2F,
0x01A71D30,
0x01A71E81,
0x01A71F01,
//Pin widget 0x1B - LINE2 (Port-E)
0x01B71CF0,
0x01B71D11,
0x01B71E11,
0x01B71F41,
//Pin widget 0x1C - CD-IN
0x01C71CF0,
0x01C71D11,
0x01C71E11,
0x01C71F41,
//Pin widget 0x1D - BEEP-IN
0x01D71C01,
0x01D71D00,
0x01D71EB0,
0x01D71F40,
//Pin widget 0x1E - S/PDIF-OUT
0x01E71CF0,
0x01E71D11,
0x01E71E11,
0x01E71F41,
//Widget node 0x20 :
0x02050004,
0x02040001,
0x02050004,
0x02040001
};
//ray_override / [XI-BringUp] Bring Up Porting / Tuning Board Override / Support ALC662 / Added <<

UINT32 OemHdaVerbTableDisplayAudio[] = {
    //
    // Display Audio Verb Table
    //   
    // Enable the third converter and Pin first (NID 08h)
    0x00878101,
    // Pin Widget 5 - PORT B - Configuration Default: 0x18560010
    0x00571C10,
    0x00571D00,
    0x00571E56,
    0x00571F18,
    // Pin Widget 6 - PORT C - Configuration Default: 0x18560020
    0x00671C20,
    0x00671D00,
    0x00671E56,
    0x00671F18,
    // Pin Widget 7 - PORT D - Configuration Default: 0x18560030
    0x00771C30,
    0x00771D00,
    0x00771E56,
    0x00771F18,
    // Disable the third converter and third Pin (NID 08h)
    0x00878100
};

#ifndef AMI_CRB_EC_SUPPORT_FLAG
UINT32 OemHdaVerbTblSample[] = {
//
// Rear Audio Verb Table 0x10EC0888
//
// (NID 01h)
//===== HDA Codec Subsystem ID Verb-table =====
//HDA Codec Subsystem ID  : 0x105B0CE3
  0x001720E3,
  0x0017210C, 
  0x0017225B, 
  0x00172310,
//===== Pin Widget Verb-table =====
  //Pin Complex 1     (NID 0x14 )
  0x01471C10,
  0x01471D44,
  0x01471E01,
  0x01471F01,

  //Pin Complex 2     (NID 0x15 )
  0x01571C12,
  0x01571D14,
  0x01571E01,
  0x01571F01,

  //Pin Complex 3     (NID 0x16 )
  0x01671C11,
  0x01671D64,
  0x01671E01,
  0x01671F01,

  //Pin Complex 4     (NID 0x17 )
  0x01771C14,
  0x01771D24,
  0x01771E01,
  0x01771F01,

  //Pin Complex 5     (NID 0x18 )
  0x01871C40,
  0x01871D9C,
  0x01871EA1,
  0x01871F01,

  //Pin Complex 6     (NID 0x19 )
  0x01971C50,
  0x01971D9C,
  0x01971EA1,
  0x01971F02,

  //Pin Complex 7     (NID 0x1A )
  0x01A71C4F,
  0x01A71D34,
  0x01A71E81,
  0x01A71F01,

  //Pin Complex 8     (NID 0x1B )
  0x01B71C20,
  0x01B71D4C,
  0x01B71E21,
  0x01B71F02,

  //Pin Complex 9     (NID 0x1C )
  0x01C71CF0,
  0x01C71D01,
  0x01C71E33,
  0x01C71F59,

  //Pin Complex 10    (NID 0x1D )
  0x01D71C01,
  0x01D71DE6,
  0x01D71E05,
  0x01D71F40,

  //Pin Complex 11    (NID 0x1E )
  0x01E71C30,
  0x01E71D61,
  0x01E71E4B,
  0x01E71F01,

  //Pin Complex 12    (NID 0x1F )
  0x01F71C60,
  0x01F71D71,
  0x01F71ECB,
  0x01F71F01
};

GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 OemDqByteMapSkl_Y_S[2][6][2] = {
  // Channel 0:
  {
    { 0x0F, 0xF0 }, // CLK0 goes to package 0 - Bytes[3:0], CLK1 goes to package 1 - Bytes[7:4]
    { 0x00, 0xF0 }, // CmdN does not have CAA, CAB goes to Bytes[7:4]
    { 0x0F, 0xF0 }, // CmdS CAA goes to Bytes[3:0], CmdS CAB goes to Byte[7:4]
    { 0x0F, 0x00 }, // CKE CAA goes to Bytes[3:0], CKE does not have CAB
    { 0xFF, 0x00 }, // CTL (CS) goes to all bytes
    { 0xFF, 0x00 }  // CA Vref is one for all bytes
  },
  // Channel 1:
  {
    { 0x33, 0xCC }, // CLK0 goes to package 0 - Bytes[3:0], CLK1 goes to package 1 - Bytes[7:4]
    { 0x00, 0xCC }, // CmdN does not have CAA, CAB goes to Bytes[7:4]
    { 0x33, 0xCC }, // CmdS CAA goes to Bytes[3:0], CmdS CAB goes to Byte[7:4]
    { 0x33, 0x00 }, // CKE CAA goes to Bytes[3:0], CKE does not have CAB
    { 0xFF, 0x00 }, // CTL (CS) goes to all bytes
    { 0xFF, 0x00 }  // CA Vref is one for all bytes
  }
};

GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 OemDqByteMapSkl_U[2][6][2] = {
  // Channel 0:
  {
    { 0x0F, 0xF0 }, // CLK0 goes to package 0 - Bytes[3:0], CLK1 goes to package 1 - Bytes[7:4]
    { 0x00, 0xF0 }, // CmdN does not have CAA, CAB goes to Bytes[7:4]
    { 0x0F, 0xF0 }, // CmdS CAA goes to Bytes[3:0], CmdS CAB goes to Byte[7:4]
    { 0x0F, 0x00 }, // CKE CAA goes to Bytes[3:0], CKE does not have CAB
    { 0xFF, 0x00 }, // CTL (CS) goes to all bytes
    { 0xFF, 0x00 }  // CA Vref is one for all bytes
  },
  // Channel 1:
  {
    { 0x0F, 0xF0 }, // CLK0 goes to package 0 - Bytes[3:0], CLK1 goes to package 1 - Bytes[7:4]
    { 0x00, 0xF0 }, // CmdN does not have CAA, CAB goes to Bytes[7:4]
    { 0x0F, 0xF0 }, // CmdS CAA goes to Bytes[3:0], CmdS CAB goes to Byte[7:4]
    { 0x0F, 0x00 }, // CKE CAA goes to Bytes[3:0], CKE does not have CAB
    { 0xFF, 0x00 }, // CTL (CS) goes to all bytes
    { 0xFF, 0x00 }  // CA Vref is one for all bytes
  }
};

//
// DQS byte swizzling between CPU and DRAM - for SKL RVP1, RVP3, RVP13
//
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 OemDqsMapCpu2DramSkl_Y[2][8] = {
  { 0, 1, 3, 2, 4, 5, 6, 7 }, // Channel 0
  { 1, 0, 4, 5, 2, 3, 6, 7 }  // Channel 1
};

//
// DQS byte swizzling between CPU and DRAM - for SKL RVP5
//
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 OemDqsMapCpu2DramSkl_U[2][8] = {
  { 1, 0, 3, 2, 5, 4, 6, 7 }, // Channel 0
  { 2, 3, 1, 0, 6, 4, 5, 7 }  // Channel 1
};

//
// DQS byte swizzling between CPU and DRAM - for SKL SDS
//
GLOBAL_REMOVE_IF_UNREFERENCED const UINT8 OemDqsMapCpu2DramSkl_S[2][8] = {
  { 2, 0, 3, 1, 5, 7, 6, 4 }, // Channel 0
  { 3, 1, 6, 4, 2, 0, 5, 7 }  // Channel 1
};
#endif

#if (defined MRC_MEMORY_DOWN_SUPPORT && (MRC_MEMORY_DOWN_SUPPORT == 1))

static const UINT8 Dimm0SpdTbl[] = NB_OEM_DIMM0_SPD_DATA;
static const UINT8 Dimm1SpdTbl[] = NB_OEM_DIMM1_SPD_DATA;
static const UINT8 Dimm2SpdTbl[] = NB_OEM_DIMM2_SPD_DATA;
static const UINT8 Dimm3SpdTbl[] = NB_OEM_DIMM3_SPD_DATA;
#ifdef OEM_DQS_MAP_CPU2DRAM
static const UINT8 OemDqsMapCpu2Dram[2][8] = OEM_DQS_MAP_CPU2DRAM;
#endif
#ifdef OEM_DQ_BYTE_MAP
static const UINT8 OemDqByteMap[2][6][2] = OEM_DQ_BYTE_MAP;
#endif
#endif // MRC_MEMORY_DOWN_SUPPORT

VOID CrbSaSetupCallbacks (
    IN VOID              *Services,
    IN OUT NB_SETUP_DATA *NbSetupData,
    IN SETUP_DATA        *SetupData,
    IN BOOLEAN           Pei )
{
	
#ifndef AMI_CRB_EC_SUPPORT_FLAG	
    NbSetupData->SaSpdAddressInfo.IsOverrideSpdAddress = NB_DIMM_SMBUS_ADDRESS_OVERRIDE;
#if defined NB_DIMM_SMBUS_ADDRESS_OVERRIDE && NB_DIMM_SMBUS_ADDRESS_OVERRIDE == 1  		
    NbSetupData->SaSpdAddressInfo.SpdAddressTable[0] = NB_DIMM0_SMBUS_ADDRESS;
    NbSetupData->SaSpdAddressInfo.SpdAddressTable[1] = NB_DIMM1_SMBUS_ADDRESS;
    NbSetupData->SaSpdAddressInfo.SpdAddressTable[2] = NB_DIMM2_SMBUS_ADDRESS;
    NbSetupData->SaSpdAddressInfo.SpdAddressTable[3] = NB_DIMM3_SMBUS_ADDRESS;
#endif
#endif
    
	if (!Pei) return; 
    
#ifndef AMI_CRB_EC_SUPPORT_FLAG
#if defined MRC_MEMORY_DOWN_SUPPORT && MRC_MEMORY_DOWN_SUPPORT == 1    
    
    NbSetupData->SaMemoryDownInfo.IsRunMemoryDown = 1;
           
#if (defined CRB_BOARD && (CRB_BOARD == 0 || CRB_BOARD == 3 || CRB_BOARD == 5 || CRB_BOARD == 7 || CRB_BOARD == 10 || CRB_BOARD == 12)) // for Y, S , H ,and G
    NbSetupData->SaMemoryDownInfo.OemDqByteMap     = (UINT8 *)OemDqByteMapSkl_Y_S;
#if defined CRB_BOARD && CRB_BOARD == 0
    NbSetupData->SaMemoryDownInfo.OemDqsMap        = (UINT8 *)OemDqsMapCpu2DramSkl_Y;
#else
    NbSetupData->SaMemoryDownInfo.OemDqsMap        = (UINT8 *)OemDqsMapCpu2DramSkl_S;
#endif

#elif (defined CRB_BOARD && (CRB_BOARD == 1 || CRB_BOARD == 2 || CRB_BOARD == 8 || CRB_BOARD == 11)) // for U and R
    NbSetupData->SaMemoryDownInfo.OemDqByteMap     = (UINT8 *)OemDqByteMapSkl_U;
    NbSetupData->SaMemoryDownInfo.OemDqsMap        = (UINT8 *)OemDqsMapCpu2DramSkl_U;
#endif

#ifdef OEM_DQS_MAP_CPU2DRAM
    NbSetupData->SaMemoryDownInfo.OemDqsMap        = (UINT8 *)OemDqsMapCpu2Dram;
#endif
    
#ifdef OEM_DQ_BYTE_MAP
    NbSetupData->SaMemoryDownInfo.OemDqByteMap     = (UINT8 *)OemDqByteMap;
#endif
    
    NbSetupData->SaMemoryDownInfo.MemoryDownDdrSpd[0] = (UINT8 *)Dimm0SpdTbl;
    NbSetupData->SaMemoryDownInfo.MemoryDownDdrSpdSize[0] = sizeof (Dimm0SpdTbl);
    NbSetupData->SaMemoryDownInfo.MemoryDownDdrSpd[1] = (UINT8 *)Dimm1SpdTbl;
    NbSetupData->SaMemoryDownInfo.MemoryDownDdrSpdSize[1] = sizeof (Dimm1SpdTbl);
    NbSetupData->SaMemoryDownInfo.MemoryDownDdrSpd[2] = (UINT8 *)Dimm2SpdTbl;
    NbSetupData->SaMemoryDownInfo.MemoryDownDdrSpdSize[2] = sizeof (Dimm2SpdTbl);
    NbSetupData->SaMemoryDownInfo.MemoryDownDdrSpd[3] = (UINT8 *)Dimm3SpdTbl;
    NbSetupData->SaMemoryDownInfo.MemoryDownDdrSpdSize[3] = sizeof (Dimm3SpdTbl);

#endif
#endif
}

#if defined USB_OVERCURRENT_PIN_OVERRIDE_SUPPORT && USB_OVERCURRENT_PIN_OVERRIDE_SUPPORT == 1 && USB_OVERCURRENT_PIN_OVERRIDE_USE_ELINK
static UINT8 USB20OverCurrentMapping[] = {USB_OVER_CURRENT_MAPPING_SETTINGS};
static UINT8 USB30OverCurrentMapping[] = {USB30_OVER_CURRENT_MAPPING_SETTINGS};
#endif

VOID CrbUsbOverCurrentPinSetupCallbacks (
    IN VOID              *Services,
    IN OUT SB_SETUP_DATA *SbSetupData,
    IN SETUP_DATA        *SetupData,
    IN BOOLEAN           Pei )
{	
	if (!Pei) return; 

#ifndef AMI_CRB_EC_SUPPORT_FLAG	
#if defined USB_OVERCURRENT_PIN_OVERRIDE_SUPPORT && USB_OVERCURRENT_PIN_OVERRIDE_SUPPORT == 1 && USB_OVERCURRENT_PIN_OVERRIDE_USE_ELINK
	SbSetupData->UsbOverCurrentPinMapping.UsbOverCurrentPinOverride = 1;	
	
	// Check PCH spec for the total USB overcurrent pin num
	// And set the correct amount data to USB_OVER_CURRENT_MAPPING_SETTINGS and USB30_OVER_CURRENT_MAPPING_SETTINGS 
	// If the Pin number is not match the PCH, code will not override
	SbSetupData->UsbOverCurrentPinMapping.Usb20PinNum = sizeof(USB20OverCurrentMapping);
	SbSetupData->UsbOverCurrentPinMapping.Usb30PinNum = sizeof(USB30OverCurrentMapping);
	SbSetupData->UsbOverCurrentPinMapping.Usb20PinMapping = USB20OverCurrentMapping;
	SbSetupData->UsbOverCurrentPinMapping.Usb30PinMapping = USB30OverCurrentMapping;	
#endif	
#endif
}

VOID CrbPchPolicyUpdateCallBack (
    IN VOID				**Services,
    IN SI_POLICY_PPI    *SiPolicy,
    IN UINT16           BoardId
    )
{
#ifndef AMI_CRB_EC_SUPPORT_FLAG 
    switch (BoardId) {
    case BoardIdSkylakeA0Rvp3:
    // Update PchPolicy 
        break;
    case BoardIdSkylakeURvp7:
        // Update PchPolicy 
        break;
    case BoardIdSkylakeHaloDdr4Rvp11:
        // Update PchPolicy 
        break;
    case BoardIdKabylakeDdr4Rvp17:
        // Update PchPolicy 
        break;        
    default:
      break;
    }
#endif
}

#if defined CRB_AMI_GET_PEI_MEMORY_INFO_SUPPORT && CRB_AMI_GET_PEI_MEMORY_INFO_SUPPORT
#include <Pei.h>
#include <AmiPeiLib.h>
#include <Core/Pei/PeiMain.h>

EFI_STATUS DumpPeiMemoryInS3 (
            IN EFI_PEI_SERVICES **PeiServices, 
            IN UINT8 CheckPoint)
{

    if (CheckPoint == 0xE3) { /// Before exit S3, 80 port will be 0xe3
       EFI_STATUS          Status = EFI_SUCCESS;
       EFI_PHYSICAL_ADDRESS    PeiMemLen;
       EFI_PHYSICAL_ADDRESS    HobMemoryFreeBottom;
       EFI_PHYSICAL_ADDRESS              HobMemoryFreeTop;
       EFI_PHYSICAL_ADDRESS              UsedPeiMemSize;
       EFI_PHYSICAL_ADDRESS              FreePeiMemSize;
       PEI_CORE_INSTANCE                       *PrivateData;
       EFI_PEI_HOB_POINTERS                    Hob;

       PrivateData = PEI_CORE_INSTANCE_FROM_PS_THIS (PeiServices);
       
       if (!PrivateData->PeiMemoryInstalled) return EFI_NOT_READY;

       Hob.Raw     = PrivateData->HobList.Raw;
       PeiMemLen = PrivateData->PhysicalMemoryLength;
   
       HobMemoryFreeTop     = Hob.HandoffInformationTable->EfiFreeMemoryTop;
       HobMemoryFreeBottom  = Hob.HandoffInformationTable->EfiFreeMemoryBottom;
       UsedPeiMemSize = (Hob.HandoffInformationTable->EfiMemoryTop - HobMemoryFreeTop) + (HobMemoryFreeBottom - Hob.HandoffInformationTable->EfiMemoryBottom);
       FreePeiMemSize = HobMemoryFreeTop - HobMemoryFreeBottom;
       
       DEBUG((DEBUG_INFO, "Pei Memory Base = %x \n", Hob.HandoffInformationTable->EfiMemoryBottom));
       DEBUG((DEBUG_INFO, "Pei Memory Size = %x \n", PeiMemLen));
       DEBUG((DEBUG_INFO, "Used Pei Memory size = %d Bytes. \n", UsedPeiMemSize));
       DEBUG((DEBUG_INFO, "Free Pei Memory size = %d Bytes. \n", FreePeiMemSize));
       
#if defined CMOS_MANAGER_SUPPORT && CMOS_MANAGER_SUPPORT  
       //
       // Save to CMOS for free Pei memory kb Size in S3
       //
       WriteCmos(CRB_CMOS_GET_FREE_PEI_MEM_SIZE_REG, (UINT8)(FreePeiMemSize/1024));
       WriteCmos(CRB_CMOS_GET_FREE_PEI_MEM_SIZE_REG+1, (UINT8)((FreePeiMemSize/1024)>>8));
#endif       
    }
   
    return EFI_SUCCESS;
}
#endif
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
