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
//*****************************************************************************
/** @file SaLinkLib.c
    North Bridge Library for ELINK functions

**/
//*************************************************************************

//-------------------------------------------------------------------------
// Include(s)
//-------------------------------------------------------------------------
#include <PiPei.h>
#include <Token.h>
#include <Setup.h>
#include <NbSetupData.h>
#include <Library/DebugLib.h>
#include <SaRegs.h>
#include <Library/CpuPlatformLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <SetupVariable.h>
#include <Library/MemoryAllocationLib.h>
//-------------------------------------------------------------------------
// Variable and External Declaration(s)
//-------------------------------------------------------------------------
// Variable Declaration(s)


/**
    This function returns NB Chipset setup data from system SetupData
    variable 

    @param Services Pointer to PeiServices or RuntimeServices
        structure  
    @param NbSetupData Pointer to custom setup data to return
    @param SetupData Pointer to system setup data.
    @param Pei Pei flag. If TRUE we are in PEI phase

    @retval VOID
**/
VOID SaSetupCallbacks (
    IN VOID              *Services,
    IN OUT NB_SETUP_DATA *NbSetupData,
    IN SETUP_DATA        *SetupData,
    IN BOOLEAN           Pei )
{
    UINTN                   i = 0;
	//AMI_OVERRIDE_START >>> Fix cpp check error
    //CPU_FAMILY              CpuFamilyId; 
    
    //CpuFamilyId = GetCpuFamily();    
	//AMI_OVERRIDE_END <<< Fix cpp check error
    
    NbSetupData->GdxcIotSize           = 4;
    NbSetupData->GdxcMotSize           = 12;
    NbSetupData->MemoryTrace           = 0;

    NbSetupData->ClttConfig            = 0;
    NbSetupData->Altitude              = 0;
    NbSetupData->Tcrit                 = 0;
    NbSetupData->Thigh                 = 0;
    NbSetupData->BwLimitTf             = 1023;
    NbSetupData->WarmThreshold         = 0;
    NbSetupData->BwLimitWarm           = 0;
    NbSetupData->HotThreshold          = 0;
    NbSetupData->BwLimitHot            = 0;
    NbSetupData->DoubleRefreshControl  = 0;

    NbSetupData->ReadVref              = 0;
    NbSetupData->InternalReadVref      = 0;

    // SSID is programed in ProgramNBSubId of NbPei.c
    NbSetupData->NBDxeSubSystemVendorId  = 0;
    NbSetupData->NBDxeSubSystemId        = 0;

    NbSetupData->IgdBootType           = 0;
    NbSetupData->DisplayPipeB          = 0;
    NbSetupData->LcdPanelType          = 0;
    NbSetupData->LcdPanelScaling       = 0;        
    NbSetupData->UncoreVoltageOffset   = 0;        
    NbSetupData->UncoreVoltageOffsetPrefix = 0;    
    NbSetupData->GtsMaxOcRatio         = 0;
    NbSetupData->GtsVoltageMode        = 0;
    NbSetupData->GtsVoltageOffset      = 0;
    NbSetupData->GtsVoltageOffsetPrefix = 0;
    NbSetupData->GtsVoltageOverride    = 0;    
    NbSetupData->GtsExtraTurboVoltage  = 0;
    NbSetupData->GtusMaxOcRatio        = 0;
    NbSetupData->GtusVoltageMode       = 0;
    NbSetupData->GtusVoltageOffset     = 0;    
    NbSetupData->GtusVoltageOffsetPrefix = 0;
    NbSetupData->GtusVoltageOverride   = 0;
    NbSetupData->GtusExtraTurboVoltage = 0;
    NbSetupData->SaVoltageOffset       = 0;
    NbSetupData->GtVoltageMode         = 0;
    NbSetupData->IoaVoltageOffset      = 0;
    NbSetupData->IodVoltageOffset      = 0;
    NbSetupData->EdramRatio            = 0;    

    NbSetupData->IgdLcdBlc             = 0;
    NbSetupData->IgdLcdIBia            = 6;
    NbSetupData->IgdLcdSSCC            = 1;
//    NbSetupData->IgdTV1Standard        = 0;
//    NbSetupData->IgdTV2Standard        = 0;
    NbSetupData->AlsEnable             = 0;
    NbSetupData->PanelPowerEnable      = 1;
    NbSetupData->ActiveLFP             = 1;
    NbSetupData->LfpColorDepth         = 0;

    NbSetupData->GopConfigEnable       = 0;
    NbSetupData->BclkChangeWarmReset   = 1;
    NbSetupData->BclkChangeContinues   = 0;
//
// KblGBegin
//
    NbSetupData->DelayAfterOffMethod   = 200;
    NbSetupData->DelayAfterLinkEnable  = 250;
    NbSetupData->GenSpeedChangeEnable  = 1;
    
    NbSetupData->PegComplianceModeTest = 0;
//
// KblGEnd
//
    // IUER
    NbSetupData->SlateIndicatorRT      = 1;
    NbSetupData->SlateIndicatorSx      = 0;
    NbSetupData->DockIndicatorRT       = 0;
    NbSetupData->DockIndicatorSx       = 0;
    NbSetupData->IuerButtonEnable      = 0;
    NbSetupData->IuerConvertibleEnable = 0;
    NbSetupData->IuerDockEnable        = 0;
    NbSetupData->DllBwEn0 = 0;
    NbSetupData->DllBwEn1 = 1;
    NbSetupData->DllBwEn2 = 2;
    NbSetupData->DllBwEn3 = 2;
    
    NbSetupData->Peg0Enable            = 2;
    NbSetupData->Peg1Enable            = 2;
    NbSetupData->Peg2Enable            = 2;
    NbSetupData->Peg0MaxLinkSpeed      = 0;
    NbSetupData->Peg1MaxLinkSpeed      = 0;
    NbSetupData->Peg2MaxLinkSpeed      = 0;
    NbSetupData->Peg0MaxLinkWidth      = 0;
    NbSetupData->Peg1MaxLinkWidth      = 0;
    NbSetupData->Peg2MaxLinkWidth      = 0;
    NbSetupData->Peg0PowerDownUnusedLanes = 1;
    NbSetupData->Peg1PowerDownUnusedLanes = 1;
    NbSetupData->Peg2PowerDownUnusedLanes = 1;
    NbSetupData->Peg0Gen3EqPh2Enable   = 2;
    NbSetupData->Peg1Gen3EqPh2Enable   = 2;
    NbSetupData->Peg2Gen3EqPh2Enable   = 2;    

    NbSetupData->SkipExtGfxScan        = 0;
    NbSetupData->X2ApicOptOut          = 1;
    NbSetupData->SaImguEnable          = 1;
    NbSetupData->EdramTestMode         = 2;
    NbSetupData->EdramTestModeJ0       = 0;
    NbSetupData->IedMemEnable          = 0; 
    NbSetupData->DelayAfterPwrEn       = 100;
    NbSetupData->DelayAfterHoldReset   = 300;
    
    for(i = 0; i < 3; i++) {
        NbSetupData->PegAspm[i] = 3;
        NbSetupData->PegAspmL0s[i] = 3;
        NbSetupData->PegDeEmphasis[i] = 1;
        NbSetupData->LtrMaxSnoopLatency[i]     = V_SA_LTR_MAX_SNOOP_LATENCY_VALUE;
        NbSetupData->LtrMaxNoSnoopLatency[i]   = V_SA_LTR_MAX_NON_SNOOP_LATENCY_VALUE;
        NbSetupData->PowerDownUnusedBundles[i] = 0xFF;
        NbSetupData->PegRootPortHPE[i]         = 0;
        NbSetupData->PegSlotPowerLimitScale[i] = 75;
        NbSetupData->PegSlotPowerLimitValue[i] = 0;        
        NbSetupData->PegPhysicalSlotNumber[i]  = (UINT16)(i+1);         
        NbSetupData->PegExtraBusRsvd[i]        = 0;
        NbSetupData->PegMemRsvd[i]             = 10;
        NbSetupData->PegIoRsvd[i]              = 4;
        NbSetupData->PegMaxPayload[i]          = 0xff;
    }
    NbSetupData->Peg0LtrEnable               = 1;
    NbSetupData->Peg1LtrEnable               = 1;
    NbSetupData->Peg2LtrEnable               = 1;
    NbSetupData->Peg0ObffEnable              = 1;
    NbSetupData->Peg1ObffEnable              = 1;
    NbSetupData->Peg2ObffEnable              = 1;
    
    NbSetupData->PegGenerateBdatMarginTable = 0;  
    NbSetupData->PegRxCemTestingMode        = 0;
    NbSetupData->PegRxCemLoopbackLane     = 0;
    NbSetupData->PegRxCemNonProtocolAwareness   = 0;       
    NbSetupData->PegDisableSpreadSpectrumClocking   = 0;     
    NbSetupData->PegGen3ProgramStaticEq   = 1;

    NbSetupData->Gen3SwEqAlwaysAttempt = 0;
    NbSetupData->Gen3SwEqNumberOfPresets = 0;
    NbSetupData->Gen3SwEqJitterDwellTime = 1000;

    NbSetupData->Gen3SwEqJitterErrorTarget = 2;
    NbSetupData->Gen3SwEqVocDwellTime         = 10000;
    NbSetupData->Gen3SwEqVocErrorTarget       = 2;
    //do not use i[16] 
    for(i = 0; i < 16; i++) {
       NbSetupData->PegGen3RootPortPreset[i]    = 8;
       NbSetupData->PegGen3EndPointPreset[i]    = 7;
       NbSetupData->PegGen3EndPointHint[i]   = 2; 
    }

    for (i = 0; i < 8 ; i++ ) {
       NbSetupData->PegGen3RxCtlePeaking[i]     = 8;
    }
    NbSetupData->PegGen3RxCtleOverride = 0;

    NbSetupData->InitAspmAfterOprom = 0;
    NbSetupData->PegGen3AllowGpioReset = 1;
    NbSetupData->Gen3SwEqEnableVocTest = 1;
    NbSetupData->AllowPerstGpio        = 50;
    NbSetupData->AllowPerstGpioActive  = 0;
    NbSetupData->GfxTurboIMON          = 31;
    NbSetupData->SkipExtGfxScan        = 0;
    NbSetupData->PmSupport             = 1;
    NbSetupData->PeiGraphicsPeimInit   = 0;   
    NbSetupData->PavpEnable            = 1;
    NbSetupData->CdynmaxClampEnable    = 1;    
    NbSetupData->GtFreqMax             = 0xFF;        
    NbSetupData->EnableRenderStandby   = 1;
    NbSetupData->PrimaryDisplay        = 3;
    NbSetupData->PcieCardSelect        = 2;
#if defined (NB_SETUP_SUPPORT) && NB_SETUP_SUPPORT == 1
    NbSetupData->DetectNonComplaint    = 0;
    NbSetupData->PrimaryPeg            = 0;
    NbSetupData->PrimaryPcie           = 0;
#endif
    NbSetupData->InternalGraphics      = 2;
    NbSetupData->GTTSize               = 3; 
    NbSetupData->CdClock               = 3;
    NbSetupData->ApertureSize          = 1;
    NbSetupData->IgdDvmt50PreAlloc     = 1;
    NbSetupData->IgdDvmt50TotalAlloc   = 1;
    NbSetupData->LowPowerMode          = 1;
    NbSetupData->EnableVtd             = 0;
    NbSetupData->SaDevice7             = 0;
    NbSetupData->SaDevice4             = 0;
    NbSetupData->GmmEnable             = 1;
    
    NbSetupData->CridEnable            = 0;
    NbSetupData->ChHashEnable          = 1;
    NbSetupData->ChHashMask            = 0x30C8;
    NbSetupData->ChHashInterleaveBit   = 1;
    NbSetupData->Vc1ReadMeter          = 1;
    NbSetupData->Vc1ReadMeterTimeWindow  = 0x320;
    NbSetupData->Vc1ReadMeterThreshold   = 0x118;
    NbSetupData->StrongWkLeaker        = 7;
    NbSetupData->MrcTimeMeasure         = 0;
#if defined MEMORY_CHANNEL_INTERLEAVED_OVERRIDE && MEMORY_CHANNEL_INTERLEAVED_OVERRIDE == 1   
    NbSetupData->DqPinsInterleaved     = MEMORY_CHANNEL_INTERLEAVED;
#endif
    NbSetupData->MaxTolud              = DEFAULT_FIXED_MMIO_SIZE;
    NbSetupData->DdrFreqLimit          = 0;
    NbSetupData->DdrRefClk             = 0;
    NbSetupData->DdrRatio              = 0; 
    NbSetupData->DdrOddRatioMode       = 0; 
    NbSetupData->SaGv                  = 3;
    NbSetupData->FreqSaGvLow           = 0;
    NbSetupData->EvLoader              = 0;
    NbSetupData->EvLoaderDelay         = 1;
    NbSetupData->EpgEnable             = 0;
    NbSetupData->Idd3n                 = 26;
    NbSetupData->Idd3p                 = 11;
    NbSetupData->MrcSafeConfig          = 0;
    NbSetupData->MemoryThermalManagement  = 1;
    NbSetupData->ThermalThresholdWarm     = 80;
    NbSetupData->ThermalThresholdHot      = 90;
    NbSetupData->ThermalThresholdCritical = 100;
    NbSetupData->PeciInjectedTemp        = 0;
    NbSetupData->ExttsViaTsOnBoard       = 0;
    NbSetupData->ExttsViaTsOnDimm        = 0;
    NbSetupData->VirtualTempSensor       = 0;
    NbSetupData->SpdProfileSelected      = 0;
    NbSetupData->OverclockingSupport     = 0;
    NbSetupData->HobBufferSize          = 0; 
    NbSetupData->EccSupport             = 1;
    NbSetupData->NModeSupport           = 0;
    NbSetupData->ScramblerSupport       = 1;
    NbSetupData->WeaklockEn             = 1;
    NbSetupData->BdatTestType           = 0;
    NbSetupData->BdatSchema             = 1;
    NbSetupData->RMTLoopCount           = 0;
    NbSetupData->MemTestOnWarmBoot      = 1;
    NbSetupData->EnBER                  = 1;
    NbSetupData->tRRD_L                 = 0;
    NbSetupData->tRRD_S                 = 0;
    NbSetupData->tWTR_L                 = 0;
    NbSetupData->tWTR_S                 = 0;
    NbSetupData->tRRDLDefault           = 0;
    NbSetupData->tRRDSDefault           = 0;
    NbSetupData->tWTRLDefault           = 0;
    NbSetupData->tWTRSDefault           = 0;
    NbSetupData->EnCmdRate              = 3;
    NbSetupData->CmdTriStateDis         = 0;    
    NbSetupData->RhPrevention           = 0; 
    NbSetupData->RhSolution             = 0; 
    NbSetupData->RhActProbability       = 11;
    NbSetupData->ExitOnFailure          = 1;    
    NbSetupData->RetrainOnFastFail      = 1;    
    NbSetupData->McLock                 = 1;
    NbSetupData->ProbelessTrace         = 0;
    NbSetupData->AutoSelfRefreshSupport = TRUE;
    NbSetupData->ExtTemperatureSupport  = TRUE;
    NbSetupData->Ddr4MixedUDimm2DpcLimit= 0;
    NbSetupData->CleanMemory            = 0;    

    NbSetupData->CAVrefCtlOffset    = 6;
    NbSetupData->Ch0VrefCtlOffset   = 6;
    NbSetupData->Ch1VrefCtlOffset   = 6;
    NbSetupData->Ch0ClkPiCodeOffset = 6;
    NbSetupData->Ch1ClkPiCodeOffset = 6;
    NbSetupData->Ch0RcvEnOffset     = 3;
    NbSetupData->Ch0RxDqsOffset     = 3;
    NbSetupData->Ch0TxDqOffset      = 3;
    NbSetupData->Ch0TxDqsOffset     = 3;
    NbSetupData->Ch0VrefOffset      = 6;
    NbSetupData->Ch1RcvEnOffset     = 3;
    NbSetupData->Ch1RxDqsOffset     = 3;
    NbSetupData->Ch1TxDqOffset      = 3;
    NbSetupData->Ch1TxDqsOffset     = 3;
    NbSetupData->Ch1VrefOffset      = 6;

    NbSetupData->BclkFrequency      = 100 * 100;  // Setup BCLK value is in 10kHz units.
    NbSetupData->MaxRttWr           = 0;
    
#if defined MRC_DEBUG_PRINT_SUPPORT && MRC_DEBUG_PRINT_SUPPORT == 1
    NbSetupData->MrcFastBoot           = 0;
    NbSetupData->RMT                   = 1;
#else
    NbSetupData->MrcFastBoot           = 0;
    NbSetupData->RMT                   = 0;
#endif
    NbSetupData->RemapEnable           = 1;
    NbSetupData->DisableDimmChannel0   = 0;
    NbSetupData->DisableDimmChannel1   = 0;
    NbSetupData->ForceSingleRank       = 0;
    NbSetupData->LpddrMemWriteLatencySet = 1;
    NbSetupData->MemoryVoltage         = 0;
//    NbSetupData->PowerDownMode0        = 0xff;
//    NbSetupData->PowerDownMode1        = 0xff;
    NbSetupData->PwdwnIdleCounter      = 0x00;
    NbSetupData->EnhancedInterleave    = 1;
    NbSetupData->RankInterleave        = 1;
    NbSetupData->DmiVc1                = 0;
    NbSetupData->DmiGen3ProgramStaticEq = 1;
    
    for (i = 0; i < 4 ; i++ ) {
       NbSetupData->DmiGen3RootPortPreset[i] = 8;
       NbSetupData->DmiGen3EndPointPreset[i] = 7;
       NbSetupData->DmiGen3EndPointHint[i] = 2;
    }
    
    for (i = 0; i < 2 ; i++ ) {
       NbSetupData->DmiGen3RxCtlePeaking[i] = 0;
    }
    
    NbSetupData->DmiVcm                = 1;
    NbSetupData->DmiMaxLinkSpeed       = 0;
    NbSetupData->DmiGen3EqPh2Enable    = 2;
    NbSetupData->DmiGen3EqPh3Method    = 0;
    NbSetupData->DmiDeEmphasis         = 0;
    NbSetupData->DmiIot                = 0;
//    NbSetupData->C7Allowed             = 0;

    NbSetupData->PegGenx0              = 0;
    NbSetupData->PegGenx1              = 0;
    NbSetupData->PegGenx2              = 0;
    NbSetupData->DmiAspm               = 0;
    NbSetupData->DmiExtSync            = 0;
    
    //
    //MRC Training Algorithms
    //
    NbSetupData->ECT                   = 0;
    NbSetupData->SOT                   = 1;
    NbSetupData->ERDMPRTC2D            = 1;
    NbSetupData->RDMPRT                = 1;
    NbSetupData->RCVET                 = 1;
    NbSetupData->JWRL                  = 1;
    NbSetupData->EWRTC2D               = 1;
    NbSetupData->ERDTC2D               = 1;
    NbSetupData->WRTC1D                = 1;
    NbSetupData->WRVC1D                = 1;
    NbSetupData->RDTC1D                = 1;
    NbSetupData->DIMMODTT              = 0;
    NbSetupData->RDODTT                = 1;
    NbSetupData->RDEQT                 = 1;
    NbSetupData->RDAPT                 = 1;
    NbSetupData->WRTC2D                = 1;
    NbSetupData->RDTC2D                = 1;
    NbSetupData->CMDVC                 = 1;
    NbSetupData->WRVC2D                = 1;
    NbSetupData->RDVC2D                = 1;
    NbSetupData->LCT                   = 1;
    NbSetupData->RTL                   = 1;
    NbSetupData->TAT                   = 1;
    NbSetupData->RMT                   = 0;
    NbSetupData->MEMTST                = 0;
    NbSetupData->DIMMRONT              = 1;
    NbSetupData->WRDSEQT               = 0;
    NbSetupData->WRSRT                 = 1;
    NbSetupData->ALIASCHK              = 1;
    NbSetupData->RCVENC1D              = 1;
    NbSetupData->RMC                   = 0;
    NbSetupData->WRDSUDT               = 0; 
    NbSetupData->CMDSR                 = 1; 
    NbSetupData->CMDDSEQ               = 1; 
    NbSetupData->CMDNORM               = 1;     
    NbSetupData->EWRDSEQ               = 1;
    NbSetupData->TXDQVREF              = 1;
    
    NbSetupData->LockPTMregs           = 0;
    NbSetupData->Refresh2X             = 0;
    NbSetupData->DdrThermalSensor      = 1;
    NbSetupData->SrefCfgEna            = 1;
    NbSetupData->SrefCfgIdleTmr        = 512;
    NbSetupData->ThrtCkeMinDefeat      = 0;
    NbSetupData->ThrtCkeMinTmr         = 48;
    NbSetupData->ThrtCkeMinDefeatLpddr = 1;
    NbSetupData->ThrtCkeMinTmrLpddr    = 0x40;
   
    NbSetupData->EnablePwrDnLpddr      = 0;
    NbSetupData->EnableExtts           = 0;
    NbSetupData->EnableCltm            = 0;
    NbSetupData->EnableOltm            = 0;
    NbSetupData->Ddr4SkipRefreshEn     = 1;
    NbSetupData->EnablePwrDn           = 1;    
    NbSetupData->WarmThresholdCh0Dimm0 = 0xFF;
    NbSetupData->WarmThresholdCh0Dimm1 = 0xFF;
    NbSetupData->WarmThresholdCh1Dimm0 = 0xFF;
    NbSetupData->WarmThresholdCh1Dimm1 = 0xFF;
    NbSetupData->HotThresholdCh0Dimm0  = 0xFF;
    NbSetupData->HotThresholdCh0Dimm1  = 0xFF;
    NbSetupData->HotThresholdCh1Dimm0  = 0xFF;
    NbSetupData->HotThresholdCh1Dimm1  = 0xFF;
    NbSetupData->WarmBudgetCh0Dimm0    = 0xFF;
    NbSetupData->WarmBudgetCh0Dimm1    = 0xFF;
    NbSetupData->WarmBudgetCh1Dimm0    = 0xFF;
    NbSetupData->WarmBudgetCh1Dimm1    = 0xFF;
    NbSetupData->HotBudgetCh0Dimm0     = 0xFF;
    NbSetupData->HotBudgetCh0Dimm1     = 0xFF;
    NbSetupData->HotBudgetCh1Dimm0     = 0xFF;
    NbSetupData->HotBudgetCh1Dimm1     = 0xFF;
    NbSetupData->UserPowerWeightsEn    = 0;
    NbSetupData->EnergyScaleFact       = 3;
    NbSetupData->IdleEnergyCh0Dimm1    = 10;
    NbSetupData->IdleEnergyCh0Dimm0    = 10;
    NbSetupData->IdleEnergyCh1Dimm1    = 10;
    NbSetupData->IdleEnergyCh1Dimm0    = 10;
    NbSetupData->PdEnergyCh0Dimm1      = 6;
    NbSetupData->PdEnergyCh0Dimm0      = 6;
    NbSetupData->PdEnergyCh1Dimm1      = 6;
    NbSetupData->PdEnergyCh1Dimm0      = 6;
    NbSetupData->ActEnergyCh0Dimm1     = 172;
    NbSetupData->ActEnergyCh0Dimm0     = 172;
    NbSetupData->ActEnergyCh1Dimm1     = 172;
    NbSetupData->ActEnergyCh1Dimm0     = 172;
    NbSetupData->RdEnergyCh0Dimm1      = 212;
    NbSetupData->RdEnergyCh0Dimm0      = 212;
    NbSetupData->RdEnergyCh1Dimm1      = 212;
    NbSetupData->RdEnergyCh1Dimm0      = 212;
    NbSetupData->WrEnergyCh0Dimm1      = 221;
    NbSetupData->WrEnergyCh0Dimm0      = 221;
    NbSetupData->WrEnergyCh1Dimm1      = 221;
    NbSetupData->WrEnergyCh1Dimm0      = 221;
    NbSetupData->RaplPwrFlCh1          = 0;
    NbSetupData->RaplPwrFlCh0          = 0;
    NbSetupData->RaplLim2Lock          = 0;
    NbSetupData->RaplLim2WindX         = 0;
    NbSetupData->RaplLim2WindY         = 0;
    NbSetupData->RaplLim2Ena           = 0;
    NbSetupData->RaplLim2Pwr           = 0;
    NbSetupData->RaplLim1WindX         = 0;
    NbSetupData->RaplLim1WindY         = 0;
    NbSetupData->RaplLim1Ena           = 0;
    NbSetupData->RaplLim1Pwr           = 0;
    NbSetupData->tCL                   = 0;
    NbSetupData->tRCDtRP               = 0;
    NbSetupData->tRAS                  = 0;
    NbSetupData->tWR                   = 0;
    NbSetupData->tRFC                  = 0;
    NbSetupData->tRRD                  = 0;
    NbSetupData->tWTR                  = 0;
    NbSetupData->tRTP                  = 0;
    NbSetupData->tFAW                  = 0;
    NbSetupData->tCWL                  = 0;
    NbSetupData->tREFI                 = 0;
    NbSetupData->LowPowerS0Idle        = 1;
    NbSetupData->EnableAbove4GBMmio    = 0;
    NbSetupData->DDISelection          = 0;   
    NbSetupData->EnableSaDevice        = 1;
    NbSetupData->EnableDptf            = 0;
    NbSetupData->PciExpressLength      = 3;
    NbSetupData->SaMemoryDownInfo.IsRunMemoryDown   = 0;
    for (i = 0; i < 4 ; i++ ) {
       NbSetupData->SaMemoryDownInfo.MemoryDownDdrSpdSize[i] = 0;
       NbSetupData->SaMemoryDownInfo.MemoryDownDdrSpd[i] = NULL;
    }       
    NbSetupData->SaMemoryDownInfo.OemDqByteMap     = 0;
    NbSetupData->SaMemoryDownInfo.OemDqsMap        = 0;
    NbSetupData->TbtSupport         = 0;
    NbSetupData->TbtHostTempLocationNB         = 0x19; // AMI_OVERRIDE_START >>> (EIP279338)
    NbSetupData->TBThostRouter      = 1;
    NbSetupData->XmpProfileEnable = 0;
    NbSetupData->MemoryVoltageDefault = 0;
    NbSetupData->DdrRefClkDefault = 0; 
    NbSetupData->DdrRatioDefault = 0;
    NbSetupData->tCLDefault = 0;
    NbSetupData->tCWLDefault = 0;
    NbSetupData->tFAWDefault = 0;
    NbSetupData->tRASDefault = 0;
    NbSetupData->tRCDtRPDefault = 0;
    NbSetupData->tREFIDefault = 0;
    NbSetupData->tRFCDefault = 0;
    NbSetupData->tRRDDefault = 0;
    NbSetupData->tRTPDefault = 0;
    NbSetupData->tWRDefault = 0; 
    NbSetupData->tWTRDefault = 0;
    NbSetupData->NModeDefault = 0;
    NbSetupData->GtDid = 0;
    NbSetupData->GdxcEnable = 0;
    NbSetupData->BclkOverride = 0;
    NbSetupData->UncoreVoltageOffsetDefault = 0;
    NbSetupData->UncoreVoltageOffsetPrefixDefault = 0;
    NbSetupData->GtsMaxOcRatioDefault = 0;
    NbSetupData->GtsVoltageModeDefault = 0;
    NbSetupData->GtsVoltageOffsetDefault = 0;
    NbSetupData->GtsVoltageOffsetPrefixDefault = 0;
    NbSetupData->GtsVoltageOverrideDefault = 0;
    NbSetupData->GtsExtraTurboVoltageDefault = 0;
    NbSetupData->GtusMaxOcRatioDefault = 0;
    NbSetupData->GtusVoltageModeDefault = 0;
    NbSetupData->GtusVoltageOffsetDefault = 0;
    NbSetupData->GtusVoltageOffsetPrefixDefault = 0;
    NbSetupData->GtusVoltageOverrideDefault = 0;
    NbSetupData->GtusExtraTurboVoltageDefault = 0;
    NbSetupData->RealtimeMemoryTiming = 0;    
#ifdef UP_SERVER_FLAG 
    NbSetupData->UserThresholdEnable = 0;
    NbSetupData->UserBudgetEnable = 0;
    NbSetupData->TsodTcritMax = 105;
    NbSetupData->TsodEventMode = 0;
    NbSetupData->TsodEventPolarity = 0;
    NbSetupData->TsodCriticalEventOnly = 0;
    NbSetupData->TsodEventOutputControl = 0;
    NbSetupData->TsodAlarmwindowLockBit = 0;
    NbSetupData->TsodCriticaltripLockBit = 0;
    NbSetupData->TsodShutdownMode = 0;
    NbSetupData->TsodThigMax = 93;
    NbSetupData->TsodManualEnable = 0;
#endif
    
#if defined INTEL_MMA_SUPPORT && INTEL_MMA_SUPPORT == 1
    NbSetupData->SaGv = 0;
    NbSetupData->EvLoader = 1;
    NbSetupData->MrcFastBoot = 0;
    NbSetupData->BdatEnable = 1;
#endif // #if defined INTEL_MMA_SUPPORT && INTEL_MMA_SUPPORT == 1
    
    if (SetupData != NULL) {
        NbSetupData->CdClock                = SetupData->CdClock;
#if defined (NB_SETUP_SUPPORT) && NB_SETUP_SUPPORT == 1
        NbSetupData->PrimaryPeg             = SetupData->PrimaryPeg;
        NbSetupData->PrimaryPcie            = SetupData->PrimaryPcie;
        NbSetupData->DetectNonComplaint     = SetupData->DetectNonComplaint;
#endif
        NbSetupData->PwdwnIdleCounter       = SetupData->PwdwnIdleCounter; 
        NbSetupData->WeaklockEn             = SetupData->WeaklockEn;
        NbSetupData->BdatTestType           = SetupData->BdatTestType;
        NbSetupData->BdatSchema             = SetupData->BdatSchema;
        NbSetupData->RMTLoopCount           = SetupData->RMTLoopCount;
        NbSetupData->MemTestOnWarmBoot      = SetupData->MemTestOnWarmBoot;
        NbSetupData->EnBER                  = SetupData->EnBER;
        NbSetupData->tRRD_L                 = SetupData->tRRD_L;
        NbSetupData->tRRD_S                 = SetupData->tRRD_S;
        NbSetupData->tWTR_L                 = SetupData->tWTR_L;
        NbSetupData->tWTR_S                 = SetupData->tWTR_S;
        NbSetupData->tRRDLDefault           = SetupData->tRRDLDefault;
        NbSetupData->tRRDSDefault           = SetupData->tRRDSDefault;
        NbSetupData->tWTRLDefault           = SetupData->tWTRLDefault;
        NbSetupData->tWTRSDefault           = SetupData->tWTRSDefault;
        NbSetupData->EnCmdRate              = SetupData->EnCmdRate;
        NbSetupData->MaxRttWr               = SetupData->MaxRttWr;
        
#if defined (Dptf_SUPPORT) && Dptf_SUPPORT == 1        
        NbSetupData->EnableSaDevice         = SetupData->EnableSaDevice; 
        NbSetupData->EnableDptf             = SetupData->EnableDptf; 
#endif
        NbSetupData->PciExpressLength       = SetupData->PciExpressLength;
#if defined (LOW_POWER_S0_IDLE_CAPABLE) && LOW_POWER_S0_IDLE_CAPABLE == 1
    #if defined (PCH_SKU) && PCH_SKU == 1
        NbSetupData->LowPowerS0Idle = SetupData->LowPowerS0Idle;
    #else
        NbSetupData->LowPowerS0Idle = 0;
    #endif
#else
    NbSetupData->LowPowerS0Idle = 0;
#endif   
#if defined (Thunderbolt_SUPPORT) && Thunderbolt_SUPPORT
        NbSetupData->TbtSupport            = SetupData->TbtSupport; 
        NbSetupData->TbtHostTempLocationNB = SetupData->TbtHostTempLocationNB;
        NbSetupData->TBThostRouter         = SetupData->TBThostRouter;
#endif         

        // Porting Start
        NbSetupData->GdxcIotSize           = SetupData->GdxcIotSize;
        NbSetupData->GdxcMotSize           = SetupData->GdxcMotSize;
        NbSetupData->MemoryTrace           = SetupData->MemoryTrace;

        NbSetupData->IgdBootType           = SetupData->IgdBootType;
        NbSetupData->DisplayPipeB          = SetupData->DisplayPipeB;
        NbSetupData->LcdPanelType          = SetupData->LcdPanelType;

        NbSetupData->LcdPanelScaling       = SetupData->LcdPanelScaling;
        NbSetupData->IgdLcdBlc             = SetupData->IgdLcdBlc;
        NbSetupData->IgdLcdIBia            = SetupData->IgdLcdIBia;
        NbSetupData->IgdLcdSSCC            = SetupData->IgdLcdSSCC;
        NbSetupData->AlsEnable             = SetupData->AlsEnable;
        NbSetupData->PanelPowerEnable      = SetupData->PanelPowerEnable;
        NbSetupData->ActiveLFP             = SetupData->ActiveLFP;
        NbSetupData->LfpColorDepth         = SetupData->LfpColorDepth;

        NbSetupData->BclkChangeWarmReset   = SetupData->BclkChangeWarmReset;
        NbSetupData->BclkChangeContinues   = SetupData->BclkChangeContinues;
        NbSetupData->DelayAfterOffMethod   = SetupData->DelayAfterOffMethod;
        NbSetupData->DelayAfterLinkEnable  = SetupData->DelayAfterLinkEnable;
        NbSetupData->GenSpeedChangeEnable  = SetupData->GenSpeedChangeEnable;

        NbSetupData->PegComplianceModeTest = SetupData->PegComplianceModeTest;
        
        // IUER
        NbSetupData->SlateIndicatorRT      = SetupData->SlateIndicatorRT;
        NbSetupData->SlateIndicatorSx      = SetupData->SlateIndicatorSx;
        NbSetupData->DockIndicatorRT       = SetupData->DockIndicatorRT;
        NbSetupData->DockIndicatorSx       = SetupData->DockIndicatorSx;
        NbSetupData->IuerButtonEnable      = SetupData->IuerButtonEnable;
        NbSetupData->IuerConvertibleEnable = SetupData->IuerConvertibleEnable;
        NbSetupData->IuerDockEnable        = SetupData->IuerDockEnable;

        NbSetupData->Peg0Enable            = SetupData->Peg0Enable;
        NbSetupData->Peg1Enable            = SetupData->Peg1Enable;
        NbSetupData->Peg2Enable            = SetupData->Peg2Enable;
        NbSetupData->Peg0MaxLinkSpeed      = SetupData->Peg0MaxLinkSpeed;
        NbSetupData->Peg1MaxLinkSpeed      = SetupData->Peg1MaxLinkSpeed;
        NbSetupData->Peg2MaxLinkSpeed      = SetupData->Peg2MaxLinkSpeed;
        NbSetupData->Peg0MaxLinkWidth      = SetupData->Peg0MaxLinkWidth;
        NbSetupData->Peg1MaxLinkWidth      = SetupData->Peg1MaxLinkWidth;
        NbSetupData->Peg2MaxLinkWidth      = SetupData->Peg2MaxLinkWidth;
        NbSetupData->Peg0PowerDownUnusedLanes = SetupData->Peg0PowerDownUnusedLanes;
        NbSetupData->Peg1PowerDownUnusedLanes = SetupData->Peg1PowerDownUnusedLanes;
        NbSetupData->Peg2PowerDownUnusedLanes = SetupData->Peg2PowerDownUnusedLanes;        
        NbSetupData->Peg0Gen3EqPh2Enable   = SetupData->Peg0Gen3EqPh2Enable;
        NbSetupData->Peg1Gen3EqPh2Enable   = SetupData->Peg1Gen3EqPh2Enable;
        NbSetupData->Peg2Gen3EqPh2Enable   = SetupData->Peg2Gen3EqPh2Enable;
        
        for(i = 0; i < 3; i++) {
            NbSetupData->PegAspm[i]             = SetupData->PegAspm[i];
            NbSetupData->PegAspmL0s[i]          = SetupData->PegAspmL0s[i];
            NbSetupData->PegDeEmphasis[i]       = SetupData->PegDeEmphasis[i];
            NbSetupData->PegRootPortHPE[i]      = SetupData->PegRootPortHPE[i];
            NbSetupData->PegSlotPowerLimitScale[i] = SetupData->PegSlotPowerLimitScale[i];
            NbSetupData->PegSlotPowerLimitValue[i] = SetupData->PegSlotPowerLimitValue[i];        
            NbSetupData->PegPhysicalSlotNumber[i]  = SetupData->PegPhysicalSlotNumber[i];             
            NbSetupData->PegExtraBusRsvd[i]     = SetupData->PegExtraBusRsvd[i];
            NbSetupData->PegMemRsvd[i]          = SetupData->PegMemRsvd[i];
            NbSetupData->PegIoRsvd[i]           = SetupData->PegIoRsvd[i];
            NbSetupData->PegMaxPayload[i]       = SetupData->PegMaxPayload[i];
        }
        NbSetupData->Peg0LtrEnable               = SetupData->Peg0LtrEnable;
        NbSetupData->Peg1LtrEnable               = SetupData->Peg1LtrEnable;
        NbSetupData->Peg2LtrEnable               = SetupData->Peg2LtrEnable;
        NbSetupData->Peg0ObffEnable              = SetupData->Peg0ObffEnable;
        NbSetupData->Peg1ObffEnable              = SetupData->Peg1ObffEnable;
        NbSetupData->Peg2ObffEnable              = SetupData->Peg2ObffEnable;

        NbSetupData->PegGenerateBdatMarginTable     = SetupData->PegGenerateBdatMarginTable;  
        NbSetupData->PegRxCemTestingMode            = SetupData->PegRxCemTestingMode;  
        NbSetupData->PegRxCemLoopbackLane           = SetupData->PegRxCemLoopbackLane;
        NbSetupData->PegRxCemNonProtocolAwareness   = SetupData->PegRxCemNonProtocolAwareness;       
        NbSetupData->PegDisableSpreadSpectrumClocking   = SetupData->PegDisableSpreadSpectrumClocking;       
        NbSetupData->PegGen3ProgramStaticEq         = SetupData->PegGen3ProgramStaticEq;

        NbSetupData->Gen3SwEqAlwaysAttempt          = SetupData->Gen3SwEqAlwaysAttempt;
        NbSetupData->Gen3SwEqNumberOfPresets        = SetupData->Gen3SwEqNumberOfPresets;
        NbSetupData->Gen3SwEqJitterDwellTime        = SetupData->Gen3SwEqJitterDwellTime;
        NbSetupData->Gen3SwEqJitterErrorTarget      = SetupData->Gen3SwEqJitterErrorTarget;
        NbSetupData->Gen3SwEqVocDwellTime           = SetupData->Gen3SwEqVocDwellTime;
        NbSetupData->Gen3SwEqVocErrorTarget         = SetupData->Gen3SwEqVocErrorTarget;
        //do not use i[16] 
        for(i = 0; i < 16; i++) {
           NbSetupData->PegGen3RootPortPreset[i] = SetupData->PegGen3RootPortPreset[i];
           NbSetupData->PegGen3EndPointPreset[i] = SetupData->PegGen3EndPointPreset[i];
           NbSetupData->PegGen3EndPointHint[i]   = SetupData->PegGen3EndPointHint[i];
        }

        for (i = 0; i < 8 ; i++ ) {
           NbSetupData->PegGen3RxCtlePeaking[i]     = SetupData->PegGen3RxCtlePeaking[i];
        }
        
        NbSetupData->PegGen3RxCtleOverride  = SetupData->PegGen3RxCtleOverride;        
        NbSetupData->InitAspmAfterOprom     = SetupData->InitAspmAfterOprom;
        NbSetupData->PegGen3AllowGpioReset  = SetupData->PegGen3AllowGpioReset;
        NbSetupData->Gen3SwEqEnableVocTest  = SetupData->Gen3SwEqEnableVocTest;
        NbSetupData->GfxTurboIMON           = SetupData->GfxTurboIMON;
        NbSetupData->SkipExtGfxScan         = SetupData->SkipExtGfxScan;
        NbSetupData->PmSupport              = SetupData->PmSupport;
        NbSetupData->PeiGraphicsPeimInit    = SetupData->PeiGraphicsPeimInit;         
        NbSetupData->PavpEnable             = SetupData->PavpEnable;
        NbSetupData->CdynmaxClampEnable     = SetupData->CdynmaxClampEnable;
        NbSetupData->GtFreqMax              = SetupData->GtFreqMax;
        NbSetupData->EnableRenderStandby    = SetupData->EnableRenderStandby;
        NbSetupData->PrimaryDisplay         = SetupData->PrimaryDisplay;
        NbSetupData->InternalGraphics       = SetupData->InternalGraphics;
        NbSetupData->GTTSize                = SetupData->GTTSize;            
        NbSetupData->ApertureSize           = SetupData->ApertureSize;
        NbSetupData->IgdDvmt50PreAlloc      = SetupData->IgdDvmt50PreAlloc;
        NbSetupData->IgdDvmt50TotalAlloc    = SetupData->IgdDvmt50TotalAlloc;
        NbSetupData->LowPowerMode           = SetupData->LowPowerMode;
        NbSetupData->EnableVtd              = SetupData->EnableVtd;
        NbSetupData->SaDevice7              = SetupData->SaDevice7;
        NbSetupData->SaDevice4              = SetupData->SaDevice4;
//        NbSetupData->C7Allowed              = SetupData->C7Allowed;
 //       NbSetupData->SaHdmiCodecPortB      = SetupData->SaHdmiCodecPortB;
 //       NbSetupData->SaHdmiCodecPortC      = SetupData->SaHdmiCodecPortC;
 //       NbSetupData->SaHdmiCodecPortD      = SetupData->SaHdmiCodecPortD;
        NbSetupData->GmmEnable             = SetupData->GmmEnable;
        NbSetupData->CridEnable            = SetupData->CridEnable;
        NbSetupData->X2ApicOptOut          = SetupData->X2ApicOptOut;
        NbSetupData->SaImguEnable          = SetupData->SaImguEnable;
        NbSetupData->EdramTestMode         = SetupData->EdramTestMode;
        NbSetupData->IedMemEnable          = SetupData->IedMemEnable;         
        NbSetupData->ChHashEnable          = SetupData->ChHashEnable;
        NbSetupData->ChHashMask            = SetupData->ChHashMask;      
        NbSetupData->ChHashInterleaveBit   = SetupData->ChHashInterleaveBit;
        NbSetupData->Vc1ReadMeter          = SetupData->Vc1ReadMeter;
        NbSetupData->Vc1ReadMeterTimeWindow  = SetupData->Vc1ReadMeterTimeWindow;
        NbSetupData->Vc1ReadMeterThreshold   = SetupData->Vc1ReadMeterThreshold;
        NbSetupData->StrongWkLeaker          = SetupData->StrongWkLeaker;
        NbSetupData->MrcTimeMeasure          = SetupData->MrcTimeMeasure;
        
        NbSetupData->BdatEnable            = SetupData->BdatEnable;      
        NbSetupData->MaxTolud              = SetupData->MaxTolud;
        NbSetupData->SaGv                  = SetupData->SaGv;
        NbSetupData->FreqSaGvLow           = SetupData->FreqSaGvLow;
        NbSetupData->EvLoader              = SetupData->EvLoader;
        NbSetupData->EvLoaderDelay         = SetupData->EvLoaderDelay;
        NbSetupData->DdrFreqLimit          = SetupData->DdrFreqLimit;
        NbSetupData->DdrRefClk             = SetupData->DdrRefClk;
        NbSetupData->DdrRatio              = SetupData->DdrRatio; 
        NbSetupData->DdrOddRatioMode       = SetupData->DdrOddRatioMode; 
        NbSetupData->MrcSafeConfig          = SetupData->MrcSafeConfig;
        NbSetupData->MemoryThermalManagement = SetupData->MemoryThermalManagement;
        NbSetupData->PeciInjectedTemp        = SetupData->PeciInjectedTemp;
        NbSetupData->ExttsViaTsOnBoard       = SetupData->ExttsViaTsOnBoard;
        NbSetupData->ExttsViaTsOnDimm        = SetupData->ExttsViaTsOnDimm;
        NbSetupData->VirtualTempSensor       = SetupData->VirtualTempSensor;
        NbSetupData->SpdProfileSelected      = SetupData->SpdProfileSelected;
        NbSetupData->HobBufferSize         = SetupData->HobBufferSize;
        NbSetupData->EccSupport            = SetupData->EccSupport;
        NbSetupData->NModeSupport          = SetupData->NModeSupport;
        NbSetupData->ScramblerSupport      = SetupData->ScramblerSupport;
        NbSetupData->MrcFastBoot           = SetupData->MrcFastBoot;

        NbSetupData->RemapEnable           = SetupData->RemapEnable;
        NbSetupData->DisableDimmChannel0   = SetupData->DisableDimmChannel0;
        NbSetupData->DisableDimmChannel1   = SetupData->DisableDimmChannel1;
        NbSetupData->ForceSingleRank       = SetupData->ForceSingleRank;
        NbSetupData->LpddrMemWriteLatencySet = SetupData->LpddrMemWriteLatencySet;
//        NbSetupData->PowerDownMode0        = SetupData->PowerDownMode0;
//        NbSetupData->PowerDownMode1        = SetupData->PowerDownMode1;
        NbSetupData->CmdTriStateDis        = SetupData->CmdTriStateDis;
        NbSetupData->RhPrevention          = SetupData->RhPrevention;
        NbSetupData->RhSolution            = SetupData->RhSolution;
        NbSetupData->RhActProbability      = SetupData->RhActProbability;
        NbSetupData->ExitOnFailure         = SetupData->ExitOnFailure;       
        NbSetupData->RetrainOnFastFail     = SetupData->RetrainOnFastFail;        
        NbSetupData->McLock                = SetupData->McLock;
        NbSetupData->ProbelessTrace        = SetupData->ProbelessTrace;
        NbSetupData->DmiVc1                = SetupData->DmiVc1;
        NbSetupData->DmiGen3ProgramStaticEq                = SetupData->DmiGen3ProgramStaticEq;
        NbSetupData->DmiVcm                = SetupData->DmiVcm;
        NbSetupData->DmiMaxLinkSpeed       = SetupData->DmiMaxLinkSpeed;
        NbSetupData->DmiGen3EqPh2Enable    = SetupData->DmiGen3EqPh2Enable;
        NbSetupData->DmiGen3EqPh3Method    = SetupData->DmiGen3EqPh3Method;
        for (i = 0; i < 4 ; i++ ) {
           NbSetupData->DmiGen3RootPortPreset[i] = SetupData->DmiGen3RootPortPreset[i];
           NbSetupData->DmiGen3EndPointPreset[i] = SetupData->DmiGen3EndPointPreset[i];
           NbSetupData->DmiGen3EndPointHint[i] = SetupData->DmiGen3EndPointHint[i];
        }
        
        for (i = 0; i < 2 ; i++ ) {
           NbSetupData->DmiGen3RxCtlePeaking[i] = SetupData->DmiGen3RxCtlePeaking[i];
        }
        NbSetupData->DmiDeEmphasis         = SetupData->DmiDeEmphasis;
        NbSetupData->DmiIot                = SetupData->DmiIot;
        NbSetupData->DmiAspm               = SetupData->DmiAspm;
        NbSetupData->DmiExtSync            = SetupData->DmiExtSync;

        //
        // MRC Training Algorithms
        //
        NbSetupData->ECT                   = SetupData->ECT;
        NbSetupData->SOT                   = SetupData->SOT;
        NbSetupData->ERDMPRTC2D            = SetupData->ERDMPRTC2D;
        NbSetupData->RDMPRT                = SetupData->RDMPRT;
        NbSetupData->RCVET                 = SetupData->RCVET;
        NbSetupData->JWRL                  = SetupData->JWRL;
        NbSetupData->EWRTC2D               = SetupData->EWRTC2D;
        NbSetupData->ERDTC2D               = SetupData->ERDTC2D;
        NbSetupData->WRTC1D                = SetupData->WRTC1D;
        NbSetupData->WRVC1D                = SetupData->WRVC1D;
        NbSetupData->RDTC1D                = SetupData->RDTC1D;
        NbSetupData->DIMMODTT              = SetupData->DIMMODTT;
        NbSetupData->RDODTT                = SetupData->RDODTT;
        NbSetupData->RDEQT                 = SetupData->RDEQT;
        NbSetupData->RDAPT                 = SetupData->RDAPT;
        NbSetupData->WRTC2D                = SetupData->WRTC2D;
        NbSetupData->RDTC2D                = SetupData->RDTC2D;
        NbSetupData->CMDVC                 = SetupData->CMDVC;
        NbSetupData->WRVC2D                = SetupData->WRVC2D;
        NbSetupData->RDVC2D                = SetupData->RDVC2D;
        NbSetupData->LCT                   = SetupData->LCT;
        NbSetupData->RTL                   = SetupData->RTL;
        NbSetupData->TAT                   = SetupData->TAT;
        NbSetupData->RMT                   = SetupData->RMT;
        NbSetupData->MEMTST                = SetupData->MEMTST;
        NbSetupData->WRDSEQT               = SetupData->WRDSEQT;
        NbSetupData->WRSRT                 = SetupData->WRSRT;
        NbSetupData->ALIASCHK              = SetupData->ALIASCHK;
        NbSetupData->RCVENC1D              = SetupData->RCVENC1D;
        NbSetupData->RMC                   = SetupData->RMC;
        NbSetupData->WRDSUDT               = SetupData->WRDSUDT; 
        NbSetupData->CMDSR                 = SetupData->CMDSR; 
        NbSetupData->CMDDSEQ               = SetupData->CMDDSEQ; 
        NbSetupData->CMDNORM               = SetupData->CMDNORM;  
        NbSetupData->EWRDSEQ               = SetupData->EWRDSEQ;
        
        NbSetupData->LockPTMregs           = SetupData->LockPTMregs;
        NbSetupData->Refresh2X             = SetupData->Refresh2X;
        NbSetupData->DdrThermalSensor      = SetupData->DdrThermalSensor;
        NbSetupData->SrefCfgEna            = SetupData->SrefCfgEna;
        NbSetupData->SrefCfgIdleTmr        = SetupData->SrefCfgIdleTmr;
        NbSetupData->ThrtCkeMinDefeat      = SetupData->ThrtCkeMinDefeat;
        NbSetupData->ThrtCkeMinTmr         = SetupData->ThrtCkeMinTmr;
        NbSetupData->ThrtCkeMinDefeatLpddr = SetupData->ThrtCkeMinDefeatLpddr;
        NbSetupData->ThrtCkeMinTmrLpddr    = SetupData->ThrtCkeMinTmrLpddr;
        NbSetupData->EnablePwrDnLpddr      = SetupData->EnablePwrDnLpddr;
        NbSetupData->EnableExtts           = SetupData->EnableExtts;
        NbSetupData->EnableCltm            = SetupData->EnableCltm;
        NbSetupData->EnableOltm            = SetupData->EnableOltm;
        NbSetupData->Ddr4SkipRefreshEn     = SetupData->Ddr4SkipRefreshEn;
        NbSetupData->WarmThresholdCh0Dimm0 = SetupData->WarmThresholdCh0Dimm0;
        NbSetupData->WarmThresholdCh0Dimm1 = SetupData->WarmThresholdCh0Dimm1;
        NbSetupData->WarmThresholdCh1Dimm0 = SetupData->WarmThresholdCh1Dimm0;
        NbSetupData->WarmThresholdCh1Dimm1 = SetupData->WarmThresholdCh1Dimm1;
        NbSetupData->HotThresholdCh0Dimm0  = SetupData->HotThresholdCh0Dimm0;
        NbSetupData->HotThresholdCh0Dimm1  = SetupData->HotThresholdCh0Dimm1;
        NbSetupData->HotThresholdCh1Dimm0  = SetupData->HotThresholdCh1Dimm0;
        NbSetupData->HotThresholdCh1Dimm1  = SetupData->HotThresholdCh1Dimm1;
        NbSetupData->WarmBudgetCh0Dimm0    = SetupData->WarmBudgetCh0Dimm0;
        NbSetupData->WarmBudgetCh0Dimm1    = SetupData->WarmBudgetCh0Dimm1;
        NbSetupData->WarmBudgetCh1Dimm0    = SetupData->WarmBudgetCh1Dimm0;
        NbSetupData->WarmBudgetCh1Dimm1    = SetupData->WarmBudgetCh1Dimm1;
        NbSetupData->HotBudgetCh0Dimm0     = SetupData->HotBudgetCh0Dimm0;
        NbSetupData->HotBudgetCh0Dimm1     = SetupData->HotBudgetCh0Dimm1;
        NbSetupData->HotBudgetCh1Dimm0     = SetupData->HotBudgetCh1Dimm0;
        NbSetupData->HotBudgetCh1Dimm1     = SetupData->HotBudgetCh1Dimm1;

        NbSetupData->UserPowerWeightsEn    = SetupData->UserPowerWeightsEn;
        NbSetupData->EnergyScaleFact       = SetupData->EnergyScaleFact;
        NbSetupData->IdleEnergyCh0Dimm1    = SetupData->IdleEnergyCh0Dimm1;
        NbSetupData->IdleEnergyCh0Dimm0    = SetupData->IdleEnergyCh0Dimm0;
        NbSetupData->IdleEnergyCh1Dimm1    = SetupData->IdleEnergyCh1Dimm1;
        NbSetupData->IdleEnergyCh1Dimm0    = SetupData->IdleEnergyCh1Dimm0;
        NbSetupData->PdEnergyCh0Dimm1      = SetupData->PdEnergyCh0Dimm1;
        NbSetupData->PdEnergyCh0Dimm0      = SetupData->PdEnergyCh0Dimm0;
        NbSetupData->PdEnergyCh1Dimm1      = SetupData->PdEnergyCh1Dimm1;
        NbSetupData->PdEnergyCh1Dimm0      = SetupData->PdEnergyCh1Dimm0;
        NbSetupData->ActEnergyCh0Dimm1     = SetupData->ActEnergyCh0Dimm1;
        NbSetupData->ActEnergyCh0Dimm0     = SetupData->ActEnergyCh0Dimm0;
        NbSetupData->ActEnergyCh1Dimm1     = SetupData->ActEnergyCh1Dimm1;
        NbSetupData->ActEnergyCh1Dimm0     = SetupData->ActEnergyCh1Dimm0;
        NbSetupData->RdEnergyCh0Dimm1     = SetupData->RdEnergyCh0Dimm1;
        NbSetupData->RdEnergyCh0Dimm0     = SetupData->RdEnergyCh0Dimm0;
        NbSetupData->RdEnergyCh1Dimm1     = SetupData->RdEnergyCh1Dimm1;
        NbSetupData->RdEnergyCh1Dimm0     = SetupData->RdEnergyCh1Dimm0;
        NbSetupData->WrEnergyCh0Dimm1     = SetupData->WrEnergyCh0Dimm1;
        NbSetupData->WrEnergyCh0Dimm0     = SetupData->WrEnergyCh0Dimm0;
        NbSetupData->WrEnergyCh1Dimm1     = SetupData->WrEnergyCh1Dimm1;
        NbSetupData->WrEnergyCh1Dimm0     = SetupData->WrEnergyCh1Dimm0;

        NbSetupData->RaplPwrFlCh1          = SetupData->RaplPwrFlCh1;
        NbSetupData->RaplPwrFlCh0          = SetupData->RaplPwrFlCh0;
        NbSetupData->RaplLim2Lock          = SetupData->RaplLim2Lock;
        NbSetupData->RaplLim2WindX         = SetupData->RaplLim2WindX;
        NbSetupData->RaplLim2WindY         = SetupData->RaplLim2WindY;
        NbSetupData->RaplLim2Ena           = SetupData->RaplLim2Ena;
        NbSetupData->RaplLim2Pwr           = SetupData->RaplLim2Pwr;
        NbSetupData->RaplLim1WindX         = SetupData->RaplLim1WindX;
        NbSetupData->RaplLim1WindY         = SetupData->RaplLim1WindY;
        NbSetupData->RaplLim1Ena           = SetupData->RaplLim1Ena;
        NbSetupData->RaplLim1Pwr           = SetupData->RaplLim1Pwr;

        NbSetupData->tCL                   = SetupData->tCL;
        NbSetupData->tRCDtRP               = SetupData->tRCDtRP;
        NbSetupData->tRAS                  = SetupData->tRAS;
        NbSetupData->tWR                   = SetupData->tWR;
        NbSetupData->tRFC                  = SetupData->tRFC;
        NbSetupData->tRRD                  = SetupData->tRRD;
        NbSetupData->tWTR                  = SetupData->tWTR;
        NbSetupData->tRTP                  = SetupData->tRTP;
        NbSetupData->tFAW                  = SetupData->tFAW;
        NbSetupData->tCWL                  = SetupData->tCWL;
        NbSetupData->tREFI                 = SetupData->tREFI;
        
        NbSetupData->EnableAbove4GBMmio    = SetupData->EnableAbove4GBMmio; 
        
        NbSetupData->EpgEnable             = SetupData->EpgEnable;
        NbSetupData->Idd3n                 = SetupData->Idd3n;
        NbSetupData->Idd3p                 = SetupData->Idd3p;

        NbSetupData->UncoreVoltageOffset   = SetupData->UncoreVoltageOffset;
        NbSetupData->UncoreVoltageOffsetPrefix = SetupData->UncoreVoltageOffsetPrefix;
        
        NbSetupData->GtsMaxOcRatio         = SetupData->GtsMaxOcRatio;
        NbSetupData->GtsVoltageMode        = SetupData->GtsVoltageMode; 
        NbSetupData->GtsVoltageOffset      = SetupData->GtsVoltageOffset;
        NbSetupData->GtsVoltageOffsetPrefix = SetupData->GtsVoltageOffsetPrefix;
        NbSetupData->GtsVoltageOverride    = SetupData->GtsVoltageOverride;
        NbSetupData->GtsExtraTurboVoltage  = SetupData->GtsExtraTurboVoltage;
        NbSetupData->GtusMaxOcRatio        = SetupData->GtusMaxOcRatio;
        NbSetupData->GtusVoltageMode       = SetupData->GtusVoltageMode;        
        NbSetupData->GtusVoltageOverride   = SetupData->GtusVoltageOverride;
        NbSetupData->GtusExtraTurboVoltage = SetupData->GtusExtraTurboVoltage;                
        NbSetupData->GtDid = SetupData->GtDid;   // not a SETUP item, used by BIOS to pass GT SKU DID to SETUP items
        NbSetupData->GdxcEnable = SetupData->GdxcEnable;  // not a SETUP item.
        
#ifdef SG_SUPPORT 
        NbSetupData->PcieCardSelect        = SetupData->PcieCardSelect; 
        NbSetupData->DelayAfterPwrEn       = SetupData->DelayAfterPwrEn; 
        NbSetupData->DelayAfterHoldReset   = SetupData->DelayAfterHoldReset; 
#endif
#ifdef OC_SUPPORT 
        NbSetupData->OverclockingSupport = SetupData->OverclockingSupport;
        NbSetupData->XmpProfileEnable = SetupData->XmpProfileEnable;
        NbSetupData->MemoryVoltageDefault = SetupData->MemoryVoltageDefault;// not a SETUP item.
        NbSetupData->DdrRefClkDefault = SetupData->DdrRefClkDefault; // not a SETUP item.
        NbSetupData->DdrRatioDefault = SetupData->DdrRatioDefault; // not a SETUP item.
        NbSetupData->tCLDefault = SetupData->tCLDefault;    // not a SETUP item.
        NbSetupData->tCWLDefault = SetupData->tCWLDefault;  // not a SETUP item.
        NbSetupData->tFAWDefault = SetupData->tFAWDefault;  // not a SETUP item.
        NbSetupData->tRASDefault = SetupData->tRASDefault;  // not a SETUP item.
        NbSetupData->tRCDtRPDefault = SetupData->tRCDtRPDefault;  // not a SETUP item.
        NbSetupData->tREFIDefault = SetupData->tREFIDefault;  // not a SETUP item.
        NbSetupData->tRFCDefault = SetupData->tRFCDefault;  // not a SETUP item.
        NbSetupData->tRRDDefault = SetupData->tRRDDefault;  // not a SETUP item.
        NbSetupData->tRTPDefault = SetupData->tRTPDefault;  // not a SETUP item.
        NbSetupData->tWRDefault = SetupData->tWRDefault;  // not a SETUP item.
        NbSetupData->tWTRDefault = SetupData->tWTRDefault;  // not a SETUP item.
        NbSetupData->NModeDefault = SetupData->NModeDefault;  // not a SETUP item.
        NbSetupData->BclkOverride = SetupData->BclkOverride;  // not a SETUP item.
        NbSetupData->UncoreVoltageOffsetDefault = SetupData->UncoreVoltageOffsetDefault;  // not a SETUP item.
        NbSetupData->UncoreVoltageOffsetPrefixDefault = SetupData->UncoreVoltageOffsetPrefixDefault;  // not a SETUP item.
        NbSetupData->GtsMaxOcRatioDefault = SetupData->GtsMaxOcRatioDefault;  // not a SETUP item.
        NbSetupData->GtsVoltageModeDefault = SetupData->GtsVoltageModeDefault;  // not a SETUP item.
        NbSetupData->GtsVoltageOffsetDefault = SetupData->GtsVoltageOffsetDefault;  // not a SETUP item.
        NbSetupData->GtsVoltageOffsetPrefixDefault = SetupData->GtsVoltageOffsetPrefixDefault;  // not a SETUP item.
        NbSetupData->GtsVoltageOverrideDefault = SetupData->GtsVoltageOverrideDefault;  // not a SETUP item.
        NbSetupData->GtsExtraTurboVoltageDefault = SetupData->GtsExtraTurboVoltageDefault;  // not a SETUP item.
        NbSetupData->GtusMaxOcRatioDefault = SetupData->GtusMaxOcRatioDefault;  // not a SETUP item.
        NbSetupData->GtusVoltageModeDefault = SetupData->GtusVoltageModeDefault;  // not a SETUP item.
        NbSetupData->GtusVoltageOffsetDefault = SetupData->GtusVoltageOffsetDefault;  // not a SETUP item.
        NbSetupData->GtusVoltageOffsetPrefixDefault = SetupData->GtusVoltageOffsetPrefixDefault;  // not a SETUP item.
        NbSetupData->GtusVoltageOverrideDefault = SetupData->GtusVoltageOverrideDefault;  // not a SETUP item.
        NbSetupData->GtusExtraTurboVoltageDefault = SetupData->GtusExtraTurboVoltageDefault;  // not a SETUP item.
        NbSetupData->RealtimeMemoryTiming = SetupData->RealtimeMemoryTiming;
        NbSetupData->BclkFrequency = SetupData->BclkFrequency;
        NbSetupData->DllBwEn0 = SetupData->DllBwEn0;
        NbSetupData->DllBwEn1 = SetupData->DllBwEn1;
        NbSetupData->DllBwEn2 = SetupData->DllBwEn2;
        NbSetupData->DllBwEn3 = SetupData->DllBwEn3;
#endif
#ifdef UP_SERVER_FLAG 
        NbSetupData->UserThresholdEnable = SetupData->UserThresholdEnable;
        NbSetupData->UserBudgetEnable = SetupData->UserBudgetEnable;
        NbSetupData->TsodTcritMax = SetupData->TsodTcritMax;
        NbSetupData->TsodEventMode = SetupData->TsodEventMode;
        NbSetupData->TsodEventPolarity = SetupData->TsodEventPolarity;
        NbSetupData->TsodCriticalEventOnly = SetupData->TsodCriticalEventOnly;
        NbSetupData->TsodEventOutputControl = SetupData->TsodEventOutputControl;
        NbSetupData->TsodAlarmwindowLockBit = SetupData->TsodAlarmwindowLockBit;
        NbSetupData->TsodCriticaltripLockBit = SetupData->TsodCriticaltripLockBit;
        NbSetupData->TsodShutdownMode = SetupData->TsodShutdownMode;
        NbSetupData->TsodThigMax = SetupData->TsodThigMax;
        NbSetupData->TsodManualEnable = SetupData->TsodManualEnable;
#endif 
#if defined(CSM_SUPPORT) && (CSM_SUPPORT == 1)        
        NbSetupData->VideoOpRom = SetupData->VideoOpRom;        
#endif        
        // Porting End
    }

    DEBUG ((EFI_D_ERROR, "SaSetupCallbacks ends.....\n"));

}

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
