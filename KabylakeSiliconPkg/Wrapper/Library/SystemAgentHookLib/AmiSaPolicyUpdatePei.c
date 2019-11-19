//*************************************************************************
//**                                                                     **
//**   Copyright (c) 1985-2019, American Megatrends International LLC.   **
//**                                                                     **
//**      All rights reserved. Subject to AMI licensing agreement.       **
//**                                                                     **
//*************************************************************************
//-------------------------------------------------------------------------
// Include(s)
//-------------------------------------------------------------------------

#include <Efi.h>
#include <Library/DebugLib.h>
#include <AmiPeiLib.h>
#include <AmiCspLib.h>
#include <NbChipsetGuid.h>
#include <NbSetupData.h>
#include <PchAccess.h>
#include <MrcInterface.h>
#include <MemInfoHob.h>
#include <SaDataHob.h>
#include <Setup/SaSetup.h>
#include <Ppi/SiPolicy.h>
#include <SaPolicyCommon.h>
#include <Library/PchInfoLib.h>

#define MAX_TOLUD_0_5G       20 
#define MAX_TOLUD_0_75G      21 

//----------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//----------------------------------------------------------------------------
typedef struct {
    UINT8  Bus;
    UINT8  Dev;
    UINT8  Fun;
    UINT16 PegBitOffset;
} DEVICES_AFTER_PCIIO;

//----------------------------------------------------------------------------
// Variable and External Declaration(s)
//----------------------------------------------------------------------------
EFI_GUID  gSetupGuid            = SETUP_GUID;

//UINT32 NBPcieBridge[] =
//{
//  {(UINT32)NB_PCIE_CFG_ADDRESS(PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN, PCI_VENDOR_ID_OFFSET)},
//  {(UINT32)NB_PCIE_CFG_ADDRESS(PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN1, PCI_VENDOR_ID_OFFSET)},
//  {(UINT32)NB_PCIE_CFG_ADDRESS(PCIEBRN_BUS, PCIEBRN_DEV, PCIEBRN_FUN2, PCI_VENDOR_ID_OFFSET)},
//  {0xFFFFFFFF}
//};

#ifdef RC_PEG_0 
DEVICES_AFTER_PCIIO gDisablePegDevicesTable[] = {
#if RC_PEG_0 == 0
    { PCIEBRN_BUS, PCIEBRN_DEV,  PCIEBRN_FUN,  0 },
#endif
#if !(defined RC_PEG_1) || RC_PEG_1 == 0
    { PCIEBRN_BUS, PCIEBRN_DEV,  PCIEBRN_FUN1, 0 },
#endif
#if !(defined RC_PEG_2) || RC_PEG_2 == 0
    { PCIEBRN_BUS, PCIEBRN_DEV,  PCIEBRN_FUN2, 0 },
#endif
    { 0xFF,        0xFF,         0xFF,         0 }
};

UINTN gDisablePegCount = sizeof(gDisablePegDevicesTable) / sizeof(DEVICES_AFTER_PCIIO);
#endif
//----------------------------------------------------------------------------

/**
    Check Disable Peg Devices.   

    @param 
  
    @retval 
                     
**/
VOID DisablePegDevices()
{
    UINT16              PegDisFun;
    UINT16              i;
#ifdef RC_PEG_0 
   for ( i = 0; (i < gDisablePegCount) && (gDisablePegDevicesTable[i].Dev != 0xFF) ; i++) 
   {
     UINTN       PegAddress;
     UINT8       PegBus;
     UINT8       PegDev;
     UINT8       PegFun;

     PegBus = gDisablePegDevicesTable[i].Bus;
     PegDev = gDisablePegDevicesTable[i].Dev;
     PegFun = gDisablePegDevicesTable[i].Fun;

     PegAddress = (UINTN)NB_PCIE_CFG_ADDRESS(PegBus, PegDev, PegFun, 0);
 
     if (READ_MEM32(PegAddress) == 0xFFFFFFFF) continue;

     // Disable Link
//     CapPtr = NbFindCapPtr(PegAddress, 0x10);
//     SET_MEM8((PegAddress + CapPtr + 0x10), BIT04);
     SET_MEM8 ((PegAddress + R_SA_PEG_LCTL_OFFSET), BIT4);
     SET_MEM8 ((PegAddress + R_SA_PEG_DEBUP2_OFFSET), BIT0);
     SET_MEM32 ((PegAddress + R_SA_PEG_PEGCOMLCGCTRL_OFFSET), BIT30);
   }

    PegDisFun = 0
#if RC_PEG_0 == 0
             | B_SA_DEVEN_D1F0EN_MASK
#endif
#if !(defined RC_PEG_1) || RC_PEG_1 == 0
             | B_SA_DEVEN_D1F1EN_MASK
#endif
#if !(defined RC_PEG_2) || RC_PEG_2 == 0
             | B_SA_DEVEN_D1F2EN_MASK
#endif
            ; 

   // Disable PEG 
   if(PegDisFun != 0)
      RESET_PCI16_NB(R_SA_DEVEN, PegDisFun); // 0x54

#endif //#ifdef RC_PEG_0 
}

EFI_STATUS
EFIAPI
AmiUpdatePeiSaPolicyPreMem ( 
  IN OUT  VOID          *SiPreMemPolicyPpiPtr
  )
/*++

Routine Description:

  This function performs SA PEI Platform Prememory Policy initialization.

Arguments:

  SiPreMemPolicyPpiPtr     The SA Platform Policy PPI instance

Returns:

  EFI_SUCCESS             The PPI is installed and initialized.
  EFI ERRORS              The PPI is not successfully installed.
  EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver

--*/
{
  EFI_STATUS                          Status;
  CONST EFI_PEI_SERVICES              **PeiServices = GetPeiServicesTablePointer (); 
  MEMORY_CONFIGURATION                *MemConfig = NULL;
  MEMORY_CONFIG_NO_CRC                *MemConfigNoCrc = NULL;
  SA_MISC_PEI_PREMEM_CONFIG           *MiscConfig = NULL;  
  EFI_PEI_READ_ONLY_VARIABLE2_PPI     *ReadOnlyVariable = NULL; 
  UINTN                               VariableSize;
  UINTN                               MemoryCeiling = 0;
  NB_SETUP_DATA                       *NbSetupData = NULL;
  SI_PREMEM_POLICY_PPI                *SiPreMemPolicyPpi = NULL;
  SWITCHABLE_GRAPHICS_CONFIG          *SgGpioData = NULL;
#ifdef SG_SUPPORT
  PCH_SERIES                           PchSeries;
#endif 
  
#ifdef MEM_DOWN_FLAG
#if defined (OEM_RCOMP_SUPPORT) && (OEM_RCOMP_SUPPORT == 1)
  UINTN                               Index = 0;
  // Reference RCOMP resistors on motherboard
  UINT16                              RcompResistor[SA_MRC_MAX_RCOMP] = NB_OEM_RCOMP_RESISTOR;
  // RCOMP target values for DqOdt, DqDrv, CmdDrv, CtlDrv, ClkDrv
  UINT16                              RcompTarget[SA_MRC_MAX_RCOMP_TARGETS] = NB_OEM_RCOMP_TARGET;
#endif
#endif // MEM_DOWN_FLAG  
  
  DEBUG ((DEBUG_INFO, "AmiUpdatePeiSaPolicyPreMem Start.\n"));
  
  SiPreMemPolicyPpi = (SI_PREMEM_POLICY_PPI*)SiPreMemPolicyPpiPtr;
  
  Status = (*PeiServices)->AllocatePool (PeiServices, sizeof (NB_SETUP_DATA), (VOID **) &NbSetupData);
  ASSERT_PEI_ERROR (PeiServices, Status);
  
  (*PeiServices)->SetMem ((VOID*) NbSetupData, sizeof (NB_SETUP_DATA), 0);
  GetNbSetupData (PeiServices, NbSetupData, TRUE);
  
  Status = GetConfigBlock((VOID *)SiPreMemPolicyPpi, &gMemoryConfigGuid, (VOID *)&MemConfig);    
  ASSERT_PEI_ERROR (PeiServices, Status);    
  
  Status = GetConfigBlock((VOID *)SiPreMemPolicyPpi, &gMemoryConfigNoCrcGuid, (VOID *)&MemConfigNoCrc);    
  ASSERT_PEI_ERROR (PeiServices, Status);
  
  Status = GetConfigBlock((VOID *)SiPreMemPolicyPpi, &gSaMiscPeiPreMemConfigGuid, (VOID *)&MiscConfig);    
  ASSERT_PEI_ERROR (PeiServices, Status);

  Status = GetConfigBlock((VOID *)SiPreMemPolicyPpi, &gSwitchableGraphicsConfigGuid, (VOID *)&SgGpioData);
  ASSERT_PEI_ERROR (PeiServices, Status);
  
#ifndef AMI_CRB_EC_SUPPORT_FLAG
    
    if(NbSetupData->SaMemoryDownInfo.IsRunMemoryDown) {
        
        (*PeiServices)->SetMem  ((VOID *) MemConfigNoCrc->SpdData->SpdData[0][0], SA_MC_MAX_SPD_SIZE, 0);
        (*PeiServices)->SetMem  ((VOID *) MemConfigNoCrc->SpdData->SpdData[1][0], SA_MC_MAX_SPD_SIZE, 0);
        
        (*PeiServices)->CopyMem (MemConfigNoCrc->DqByteMap,\
                                 (VOID *) NbSetupData->SaMemoryDownInfo.OemDqByteMap,\
                                 sizeof (UINT8) * SA_MC_MAX_CHANNELS * SA_MRC_ITERATION_MAX * 2);
        (*PeiServices)->CopyMem (MemConfigNoCrc->DqsMap,\
                                 (VOID *) NbSetupData->SaMemoryDownInfo.OemDqsMap,\
                                 sizeof (UINT8) * SA_MC_MAX_CHANNELS * SA_MC_MAX_BYTES_NO_ECC);
        (*PeiServices)->CopyMem (MemConfigNoCrc->SpdData->SpdData[0][0],\
                                 NbSetupData->SaMemoryDownInfo.MemoryDownDdrSpd[0],\
                                 NbSetupData->SaMemoryDownInfo.MemoryDownDdrSpdSize[0]);
        (*PeiServices)->CopyMem (MemConfigNoCrc->SpdData->SpdData[0][1],\
                                 NbSetupData->SaMemoryDownInfo.MemoryDownDdrSpd[1],\
                                 NbSetupData->SaMemoryDownInfo.MemoryDownDdrSpdSize[1]);
        (*PeiServices)->CopyMem (MemConfigNoCrc->SpdData->SpdData[1][0],\
                                 NbSetupData->SaMemoryDownInfo.MemoryDownDdrSpd[2],\
                                 NbSetupData->SaMemoryDownInfo.MemoryDownDdrSpdSize[2]);
        (*PeiServices)->CopyMem (MemConfigNoCrc->SpdData->SpdData[1][1],\
                                 NbSetupData->SaMemoryDownInfo.MemoryDownDdrSpd[3],\
                                 NbSetupData->SaMemoryDownInfo.MemoryDownDdrSpdSize[3]);
        
        (*PeiServices)->SetMem (MiscConfig->SpdAddressTable, sizeof (MiscConfig->SpdAddressTable), 0);
    }
    
    if(NbSetupData->SaSpdAddressInfo.IsOverrideSpdAddress) {
        MiscConfig->SpdAddressTable[0] = NbSetupData->SaSpdAddressInfo.SpdAddressTable[0];
        MiscConfig->SpdAddressTable[1] = NbSetupData->SaSpdAddressInfo.SpdAddressTable[1];
        MiscConfig->SpdAddressTable[2] = NbSetupData->SaSpdAddressInfo.SpdAddressTable[2];
        MiscConfig->SpdAddressTable[3] = NbSetupData->SaSpdAddressInfo.SpdAddressTable[3];
    }
    
#endif

  // Locate Variable Ppi
  Status = (*PeiServices)->LocatePpi (PeiServices, &gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **) &ReadOnlyVariable);
  ASSERT_PEI_ERROR (PeiServices, Status);

  MemConfig->PwdwnIdleCounter        = NbSetupData->PwdwnIdleCounter;
  MemConfig->RankInterleave          = NbSetupData->RankInterleave;
  MemConfig->EnhancedInterleave      = NbSetupData->EnhancedInterleave;
  MemConfig->WeaklockEn              = NbSetupData->WeaklockEn;
  MemConfig->EnCmdRate               = NbSetupData->EnCmdRate;
  MemConfig->CmdTriStateDis          = NbSetupData->CmdTriStateDis;
#if defined MEMORY_CHANNEL_INTERLEAVED_OVERRIDE && MEMORY_CHANNEL_INTERLEAVED_OVERRIDE == 1    
  MemConfig->DqPinsInterleaved       = NbSetupData->DqPinsInterleaved;
#endif  

#ifdef EFI_DEBUG 
#if MRC_DEBUG_PRINT_SUPPORT
  MemConfig->MrcTimeMeasure         = FALSE;
  MemConfigNoCrc->SerialDebugLevel  = MSG_LEVEL_NOTE;
#else // #if MRC_DEBUG_PRINT_SUPPORT
  MemConfig->MrcTimeMeasure         = FALSE;
  MemConfigNoCrc->SerialDebugLevel  = MSG_LEVEL_WARNING;
#endif // #if MRC_DEBUG_PRINT_SUPPORT
#else   // EFI_DEBUG
  MemConfig->MrcTimeMeasure         = FALSE;
  MemConfigNoCrc->SerialDebugLevel  = MSG_LEVEL_NEVER;
#endif  // EFI_DEBUG

  // If Maximum TOLUD set to Dynamic assignment in BIOS setup, then set TOLUD/TOLM based on largest MMIO length of graphic controllers

  if (NbSetupData->MaxTolud != MAX_TOLUD_DYNAMIC) {
    VariableSize = sizeof(UINT32);
    Status = ReadOnlyVariable->GetVariable(
                                     ReadOnlyVariable,
                                     L"MemCeil.",
                                     &gAmiChipsetVariableGuid,
                                     NULL,
                                     &VariableSize,
                                     &MemoryCeiling);

    if(!EFI_ERROR(Status)) {
      if(MemoryCeiling >= 0xE0000000) {
          NbSetupData->MaxTolud = MAX_TOLUD_3_5G;
      } else if(MemoryCeiling >= 0xD0000000) {
          NbSetupData->MaxTolud = MAX_TOLUD_3_25G;
      } else if(MemoryCeiling >= 0xC0000000) {
          NbSetupData->MaxTolud = MAX_TOLUD_3G;
      } else if(MemoryCeiling >= 0xB0000000) {
          NbSetupData->MaxTolud = MAX_TOLUD_2_75G;
      } else if(MemoryCeiling >= 0xA0000000) {
          NbSetupData->MaxTolud = MAX_TOLUD_2_5G;
      } else if(MemoryCeiling >= 0x90000000) {
          NbSetupData->MaxTolud = MAX_TOLUD_2_25G;
      } else if(MemoryCeiling >= 0x80000000) {
          NbSetupData->MaxTolud = MAX_TOLUD_2G;
      } else if(MemoryCeiling >= 0x70000000) {
          NbSetupData->MaxTolud = MAX_TOLUD_1_75G; 
      } else if(MemoryCeiling >= 0x60000000) {
          NbSetupData->MaxTolud = MAX_TOLUD_1_5G;
      } else if(MemoryCeiling >= 0x50000000) {
          NbSetupData->MaxTolud = MAX_TOLUD_1_25G;
      } else if(MemoryCeiling >= 0x40000000) {
          NbSetupData->MaxTolud = MAX_TOLUD_1G;
      } else if(MemoryCeiling >= 0x30000000) {
          NbSetupData->MaxTolud = MAX_TOLUD_0_75G;
      } else if(MemoryCeiling <= 0x20000000) {
          NbSetupData->MaxTolud = MAX_TOLUD_0_5G;
      }
    }
  }

  //
  // Based on BIOS setup to determine maximum top of memory size below 4G, and reserved for MMIO
  //

  switch (NbSetupData->MaxTolud) {
    case MAX_TOLUD_0_5G:
      MiscConfig->MmioSize = 0xE00;
      break;
    
    case MAX_TOLUD_0_75G:
      MiscConfig->MmioSize = 0xD00;
      break;
    
    case MAX_TOLUD_1G:
      MiscConfig->MmioSize = 0xC00;
      break;
    
    case MAX_TOLUD_1_25G:
      MiscConfig->MmioSize = 0xB00;
      break;
    
    case MAX_TOLUD_1_5G:
      MiscConfig->MmioSize = 0xA00;
      break;
    
    case MAX_TOLUD_1_75G:
      MiscConfig->MmioSize = 0x900;
      break;
    
    case MAX_TOLUD_2G:
      MiscConfig->MmioSize = 0x800;
      break;
    
    case MAX_TOLUD_2_25G:
      MiscConfig->MmioSize = 0x700;
      break;
    
    case MAX_TOLUD_2_5G:
      MiscConfig->MmioSize = 0x600;
      break;
    
    case MAX_TOLUD_2_75G:
      MiscConfig->MmioSize = 0x500;
      break;
     
    case MAX_TOLUD_3G:
      MiscConfig->MmioSize = 0x400;
      break;
  
    case MAX_TOLUD_3_25G:
      MiscConfig->MmioSize = 0x300;
      break;
    
    case MAX_TOLUD_3_5G:
      MiscConfig->MmioSize = 0x200;
      break;

    case MAX_TOLUD_DYNAMIC: 
    default:  
      MiscConfig->MmioSize = 0;
      break;
  }
  
  //AMI_OVERRIDE_START >>>
  if((MiscConfig->MmioSize == 0x200) && (NbSetupData->MaxTolud != 0) && (PCIEX_BASE_ADDRESS == 0xE0000000)){
    MiscConfig->MmioSize += 0x100;
  }
  //AMI_OVERRIDE_END <<<

  MemConfig->SpdProfileSelected = NbSetupData->SpdProfileSelected;
 
#ifdef SG_SUPPORT
  PchSeries   = GetPchSeries ();
  if (PchSeries == PchLp) {   
    SgGpioData->RootPortDev                           = SG_ULT_PORT_DEV;
    SgGpioData->RootPortFun                           = SG_ULT_PORT_FUNC;
  }else if (PchSeries == PchH){
    SgGpioData->RootPortDev                           = SG_H_PORT_DEV;
    SgGpioData->RootPortFun                           = SG_H_PORT_FUNC;      
  }
  
  SgGpioData->SaRtd3Pcie0Gpio.GpioSupport = SG_PCIE0_GPIO_SUPPORT;
  SgGpioData->SaRtd3Pcie1Gpio.GpioSupport = SG_PCIE1_GPIO_SUPPORT;
  SgGpioData->SaRtd3Pcie2Gpio.GpioSupport = SG_PCIE2_GPIO_SUPPORT;
  if (SgGpioData->SaRtd3Pcie0Gpio.GpioSupport) {
   SgGpioData->SaRtd3Pcie0Gpio.HoldRst.ExpanderNo  = EXPANDER_dGPU_HOLD_RST;
   SgGpioData->SaRtd3Pcie0Gpio.HoldRst.GpioNo      = GPIO_dGPU_HOLD_RST;
   SgGpioData->SaRtd3Pcie0Gpio.HoldRst.Active      = ACTIVE_dGPU_HOLD_RST;

   SgGpioData->SaRtd3Pcie0Gpio.PwrEnable.ExpanderNo = EXPANDER_dGPU_PWR_EN;
   SgGpioData->SaRtd3Pcie0Gpio.PwrEnable.GpioNo     = GPIO_dGPU_PWR_EN;
   SgGpioData->SaRtd3Pcie0Gpio.PwrEnable.Active     = ACTIVE_dGPU_PWR_EN;
  }
  if (SgGpioData->SaRtd3Pcie1Gpio.GpioSupport) {
   SgGpioData->SaRtd3Pcie1Gpio.HoldRst.ExpanderNo  = EXPANDER_dGPU_HOLD_RST;
   SgGpioData->SaRtd3Pcie1Gpio.HoldRst.GpioNo      = GPIO_dGPU_HOLD_RST;
   SgGpioData->SaRtd3Pcie1Gpio.HoldRst.Active      = ACTIVE_dGPU_HOLD_RST;

   SgGpioData->SaRtd3Pcie1Gpio.PwrEnable.ExpanderNo = EXPANDER_dGPU_PWR_EN;
   SgGpioData->SaRtd3Pcie1Gpio.PwrEnable.GpioNo     = GPIO_dGPU_PWR_EN;
   SgGpioData->SaRtd3Pcie1Gpio.PwrEnable.Active     = ACTIVE_dGPU_PWR_EN;
  }
  if (SgGpioData->SaRtd3Pcie2Gpio.GpioSupport) {
   SgGpioData->SaRtd3Pcie2Gpio.HoldRst.ExpanderNo  = EXPANDER_dGPU_HOLD_RST;
   SgGpioData->SaRtd3Pcie2Gpio.HoldRst.GpioNo      = GPIO_dGPU_HOLD_RST;
   SgGpioData->SaRtd3Pcie2Gpio.HoldRst.Active      = ACTIVE_dGPU_HOLD_RST;

   SgGpioData->SaRtd3Pcie2Gpio.PwrEnable.ExpanderNo = EXPANDER_dGPU_PWR_EN;
   SgGpioData->SaRtd3Pcie2Gpio.PwrEnable.GpioNo     = GPIO_dGPU_PWR_EN;
   SgGpioData->SaRtd3Pcie2Gpio.PwrEnable.Active     = ACTIVE_dGPU_PWR_EN;
  }
#endif  
  
#ifdef MEM_DOWN_FLAG
#if defined (OEM_RCOMP_SUPPORT) && (OEM_RCOMP_SUPPORT == 1)
  DEBUG ((DEBUG_INFO, "Override RcompData Support.\n"));
  if (MemConfigNoCrc->RcompData != NULL) {
    for (Index = 0; Index < SA_MRC_MAX_RCOMP; Index++) {
      MemConfigNoCrc->RcompData->RcompResistor[Index] = RcompResistor[Index];
      DEBUG ((DEBUG_INFO, "RcompResistor[%u]: %u\n", Index, MemConfigNoCrc->RcompData->RcompResistor[Index]));
    }
    for (Index = 0; Index < SA_MRC_MAX_RCOMP_TARGETS; Index++) {
      MemConfigNoCrc->RcompData->RcompTarget[Index] = RcompTarget[Index];
      DEBUG ((DEBUG_INFO, "RcompTarget[%u]: %u\n", Index, MemConfigNoCrc->RcompData->RcompTarget[Index]));
    }
  }
#endif
#endif // MEM_DOWN_FLAG  

  DEBUG ((DEBUG_INFO, "AmiUpdatePeiSaPolicyPreMem end.\n"));  
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
AmiUpdatePeiSaPolicy ( 
  IN OUT  VOID          *SiPolicyPpiPtr
  )
/*++

Routine Description:

  This function performs SA PEI Platform Policy initialization.

Arguments:

  SiPolicyPpiPtr     The SA Platform Policy PPI instance

Returns:

  EFI_SUCCESS             The PPI is installed and initialized.
  EFI ERRORS              The PPI is not successfully installed.
  EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver

--*/
{
	  VTD_CONFIG                          *Vtd = NULL;
	  EFI_STATUS                          Status;
	  CONST EFI_PEI_SERVICES              **PeiServices = GetPeiServicesTablePointer ();
	  NB_SETUP_DATA                       *NbSetupData = NULL;
	  PCIE_PEI_CONFIG                     *PciePeiConfig;

	  DEBUG ((DEBUG_INFO, "AmiUpdatePeiSaPolicy Start.\n"));
	  
	  Status = GetConfigBlock((VOID *)SiPolicyPpiPtr, &gVtdConfigGuid, (VOID *)&Vtd);
	  if (EFI_ERROR (Status)) {
	  	ASSERT_EFI_ERROR (Status);
	  }

	  Status = GetConfigBlock((VOID *)SiPolicyPpiPtr, &gSaPciePeiConfigGuid, (VOID *)&PciePeiConfig);
	  if (EFI_ERROR (Status)) {
	  	ASSERT_EFI_ERROR (Status);
	  }

	  Status = (*PeiServices)->AllocatePool (PeiServices, sizeof (NB_SETUP_DATA), (VOID **) &NbSetupData);
	  if (EFI_ERROR (Status)) {
	  	ASSERT_PEI_ERROR (PeiServices, Status);
	  }
	  
	  (*PeiServices)->SetMem ((VOID*) NbSetupData, sizeof (NB_SETUP_DATA), 0);
	  GetNbSetupData (PeiServices, NbSetupData, TRUE);

#if defined Thunderbolt_SUPPORT && Thunderbolt_SUPPORT == 1
	  if ((NbSetupData->TbtSupport == 1) &&
	    (NbSetupData->TbtHostTempLocationNB >= 25) &&
	    (NbSetupData->TbtHostTempLocationNB <= 27)) {
	    PciePeiConfig->PegMaxPayload[NbSetupData->TbtHostTempLocationNB - 25] = 0;
	  }
#endif

	  // if VT-d is enabled, passed VT-d base address by token, it used for customerization. (VT-d disable, base address is 0.)
	  if (Vtd->VtdDisable == 1) {
	    Vtd->BaseAddress[0] = 0;
	    Vtd->BaseAddress[1] = 0;
	  } else {
	    Vtd->BaseAddress[0] = NB_VTD_BASE_ADDRESS;
	    Vtd->BaseAddress[1] = NB_VTD_BASE_ADDRESS + 0x1000;
	  }
	  
	  DEBUG ((DEBUG_INFO, "AmiUpdatePeiSaPolicy End.\n"));
	  
  return EFI_SUCCESS;
}
