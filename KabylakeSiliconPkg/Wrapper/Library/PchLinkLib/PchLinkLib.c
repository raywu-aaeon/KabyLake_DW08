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
/** @file PchLinkLib.c
    North Bridge Library for ELINK functions

**/
//*************************************************************************

//-------------------------------------------------------------------------
// Include(s)
//-------------------------------------------------------------------------
#include <PiPei.h>
#include <Token.h>
#include <Setup.h>
#include <SbSetupData.h>
#include <AmiCspLib.h>
#include <Register/PchRegsLpc.h>
#include <Library/DebugLib.h>
#include <Library/PchInfoLib.h>

//-------------------------------------------------------------------------
// Variable and External Declaration(s)
//-------------------------------------------------------------------------
// Variable Declaration(s)


/**
    This function returns Pch setup data from system SetupData
    variable 

    @param *Services    - Pointer to PeiServices or RuntimeServices
        structure  
    @param *NbSetupData - Pointer to custom setup data to return
    @param *SetupData   - Pointer to system setup data.
    @param Pei          - Pei flag. If TRUE we are in PEI phase

    @retval VOID
**/
VOID PchSetupCallbacks (
    IN VOID              *Services,
    IN OUT SB_SETUP_DATA *SbSetupData,
    IN SETUP_DATA        *SetupData,
    IN BOOLEAN           Pei )
{
    UINT8                               Index;

    DEBUG ((EFI_D_ERROR, "PchSetupCallbacks starts.....\n"));

    // Porting Start
    //  Update SB_SETUP_DATA according to the default values.
    
    SbSetupData->SensorHubType                       = 0;
    SbSetupData->DeepSxMode                          = 0;
    SbSetupData->LanWakeFromDeepSx                   = 1;
    SbSetupData->PmcReadDisable                      = 1;   
    SbSetupData->OsDebugPort                         = 0;
    SbSetupData->PchDciEn                            = 0;
    SbSetupData->PchLpcClockRun                      = 0;
    SbSetupData->PchCrid                             = 0;
    SbSetupData->PchDisableEnergyReport              = 0;
    SbSetupData->EnableTcoTimer                      = 0;
    SbSetupData->PchCapsuleResetType                 = 0;    
    SbSetupData->PchIoApic24119Entries               = 1;
    SbSetupData->PchP2sbUnlock                       = 0; 
    SbSetupData->FprrEnable                          = 0;     
    SbSetupData->Hpet                                = 1;
    SbSetupData->HpetBootTime                        = 0;
    SbSetupData->StateAfterG3                        = 0;
    SbSetupData->IchPort80Route                      = 0;
    SbSetupData->EnhancePort8xhDecoding              = 1;   
    SbSetupData->PchSirqMode                         = 0;
    SbSetupData->PchPort61hSmm                       = 1;
    SbSetupData->PciePllSsc                          = 0xFF;    
    SbSetupData->WdtEnable                           = 1; 
    // Security 
    SbSetupData->PchBiosLock                         = 0;
    SbSetupData->PchRtcLock                          = 1;
    
    // USB
    SbSetupData->PchUsb20[0]                         = 0;
    SbSetupData->PchUsb20[1]                         = 0;
    SbSetupData->DisableComplianceMode               = 0;
    SbSetupData->PchUsbPortDisable                   = 0;
    SbSetupData->UsbSensorHub                        = 0;
    SbSetupData->UsbSsicEnable[0]                    = 1;
    SbSetupData->UsbSsicEnable[1]                    = 1;
    for(Index = 0; Index < GetPchXhciMaxUsb2PortNum(); Index++)
    {
        SbSetupData->PchUsbHsPort[Index]             = 1;
    }
    SbSetupData->PchUsbHsPort[14]                 = 0;
    SbSetupData->PchUsbHsPort[15]                 = 0;
    for(Index = 0; Index < GetPchXhciMaxUsb3PortNum(); Index++)
    {
        SbSetupData->PchUsbSsPort[Index]             = 1;
    }
    
    // Xdci
    SbSetupData->PchXdciSupport                      = 0;
    
    // Lan
    SbSetupData->PchLan                              = 1;
    SbSetupData->PchWakeOnLan                        = 1;
    SbSetupData->PchWakeOnWlan                       = 0;
    SbSetupData->PchWakeOnWlanDeepSx                 = 0;
    SbSetupData->DisableDsxAcPresentPulldown         = 0;
    SbSetupData->SlpLanLowDc                         = 1;
    SbSetupData->PchLanK1Off                         = 1;
    
    // Audio
#ifndef SIMICS_FLAG
    SbSetupData->PchHdAudio                          = 2;
#else
    SbSetupData->PchHdAudio                          = 1;    
#endif    
    SbSetupData->PchHdAudioDsp                       = 1;
    SbSetupData->PchHdAudioDspUaaCompliance          = 0;
    SbSetupData->PchHdaIDisplayCodecDisconnect       = 0;
    SbSetupData->PchHdAudioPme                       = 0;
    SbSetupData->PchHdAudioIoBufferOwnership         = 0;
    SbSetupData->PchHdAudioIoBufferVoltage           = 0;
    SbSetupData->PchHdaHdAudioLinkFreq               = 2;
    SbSetupData->PchHdaIDisplayLinkFreq              = 4;
    SbSetupData->PchHdaIDisplayLinkTmode             = 0;
    SbSetupData->PchHdAudioCodecSelect               = 0;
    SbSetupData->PchHdAudioI2sCodecSelect            = 0;
    SbSetupData->PchHdAudioNhltEndpointDmic          = 2;
    SbSetupData->PchHdAudioNhltEndpointBluetooth     = 0;
    SbSetupData->PchHdAudioNhltEndpointI2s           = 0;
    for(Index = 0; Index < HDAUDIO_FEATURES; Index++) {
        SbSetupData->PchHdAudioFeature[Index] = 0;        
    }
    for(Index = 0; Index < HDAUDIO_PP_MODULES; Index++) {
        SbSetupData->PchHdAudioPostProcessingMod[Index] = 0;
    }
    
    // SATA
    SbSetupData->PchSata                             = 1;
    SbSetupData->SataInterfaceMode                   = 0;
    SbSetupData->SataRaidR0                          = 1;
    SbSetupData->SataRaidR1                          = 1;
    SbSetupData->SataRaidR10                         = 1;
    SbSetupData->SataRaidR5                          = 1;
    SbSetupData->SataRaidIrrt                        = 1;
    SbSetupData->SataRaidOub                         = 1;
    SbSetupData->SataHddlk                           = 1;
    SbSetupData->SataLedl                            = 1;
    SbSetupData->SataRaidIooe                        = 1;
    SbSetupData->SataRaidSrt                         = 1;
    SbSetupData->SataRaidOromDelay                   = 0;
    SbSetupData->SataRstForceForm                    = 0;
    SbSetupData->RaidDeviceId                        = 0;
    SbSetupData->SataSalp                            = 1;
    SbSetupData->SataControllerSpeed                 = 0;    
    SbSetupData->SataTestMode                        = 0;
    for(Index = 0; Index < GetPchMaxSataPortNum(); Index++)
    {
        SbSetupData->SataPort[Index]                 = 1;
        SbSetupData->SataHotPlug[Index]              = 0;
        SbSetupData->SataMechanicalSw[Index]         = 1;
        SbSetupData->SataSpinUp[Index]               = 0;
        SbSetupData->SataExternal[Index]             = 0;
        SbSetupData->SataType[Index]                 = 0;
        SbSetupData->SataTopology[Index]             = 0;        
        SbSetupData->PxDevSlp[Index]                 = 0;
        SbSetupData->EnableDitoConfig[Index]         = 0;
        SbSetupData->DitoVal[Index]                  = 625;
        SbSetupData->DmVal[Index]                    = 15;
    }            
    
    SbSetupData->RstPcieStorageRemap[0]              = 0;
    SbSetupData->RstPcieStorageRemapPort[0]          = 0;
    SbSetupData->RstPcieStorageRemap[1]              = 0;
    SbSetupData->RstPcieStorageRemapPort[1]          = 0;
    SbSetupData->RstPcieStorageRemap[2]              = 0;
    SbSetupData->RstPcieStorageRemapPort[2]          = 0;
    
    // PCIE
    SbSetupData->PchDmiAspm                          = 1;
    for(Index = 0; Index < GetPchMaxPciePortNum(); Index++) 
    {
        SbSetupData->PcieExtraBusRsvd[Index]        = 0;
        SbSetupData->PcieMemRsvd[Index]             = 0xA;
        SbSetupData->PcieIoRsvd[Index]              = 0x4;
        SbSetupData->PciePFMemRsvd[Index]           = 0xA;
    }
    
    SbSetupData->SataLegacyOrom                      = 0;
    SbSetupData->SataRstOptaneMemory                 = 1;
    SbSetupData->SataRstCpuAttachedStorage           = 1;
    SbSetupData->HybridStorageMode                   = 0;
    SbSetupData->PchLegacyIoLowLatency               = 0;
    SbSetupData->PcieClockGatingDisabled             = 0;
    SbSetupData->PcieUsbGlitchWa                     = 0;
    SbSetupData->PcieRootPort8xhDecode               = 0;
    SbSetupData->Pcie8xhDecodePortIndex              = 0;
    SbSetupData->PcieRootPortPeerMemoryWriteEnable   = 0;
    SbSetupData->PcieComplianceTestMode              = 0;
    SbSetupData->RpFunctionSwap                      = 1;    
    for(Index = 0; Index < GetPchMaxPciePortNum(); Index++)
    {
        SbSetupData->PcieRootPortEn[Index]           = 1;
        SbSetupData->PcieTopology[Index]             = 0;
        SbSetupData->PcieDetectTimeoutMs[Index]      = 0;        
        SbSetupData->PcieRootPortAspm[Index]         = 4;
        SbSetupData->PcieRootPortURE[Index]          = 0;  
        SbSetupData->PcieRootPortFEE[Index]          = 0;  
        SbSetupData->PcieRootPortNFE[Index]          = 0;  
        SbSetupData->PcieRootPortCEE[Index]          = 0;  
        SbSetupData->PcieRootPortCTD[Index]          = 0;  
        SbSetupData->PcieRootPortPIE[Index]          = 0;  
        SbSetupData->PcieRootPortSFE[Index]          = 0;  
        SbSetupData->PcieRootPortSNE[Index]          = 0;  
        SbSetupData->PcieRootPortSCE[Index]          = 0;  
        SbSetupData->PcieRootPortPMCE[Index]         = 1; 
        SbSetupData->PcieRootPortHPE[Index]          = 0;
        SbSetupData->PcieRootPortAER[Index]          = 1;
        SbSetupData->PcieRootPortSpeed[Index]        = 0;
        SbSetupData->PcieRootPortTHS[Index]          = 0;
        SbSetupData->PcieRootPortL1SubStates[Index]  = 3;
        SbSetupData->PcieRootPortACS[Index]          = 1;
        
        SbSetupData->PcieRootPortEqPh3Method[Index]  = 2;
        SbSetupData->PcieRootPortUptp[Index]         = 5;
        SbSetupData->PcieRootPortDptp[Index]         = 7;                
        SbSetupData->PcieLaneCm[Index]               = 6;
        SbSetupData->PcieLaneCp[Index]               = 2;
        
        SbSetupData->PcieRPDetectNonComplaint[Index] = 0;
        SbSetupData->PcieMemRsvdalig[Index]          = 1;
        SbSetupData->PciePFMemRsvdalig[Index]        = 1;
        
        SbSetupData->PchPcieLtrEnable[Index]         = 1;
        SbSetupData->PchPcieLtrConfigLock[Index]     = 0;
        SbSetupData->PchPcieSnoopLatencyOverrideMode[Index]          = 2;
        SbSetupData->PchPcieSnoopLatencyOverrideMultiplier[Index]    = 2;
        SbSetupData->PchPcieSnoopLatencyOverrideValue[Index]         = 60;
        SbSetupData->PchPcieNonSnoopLatencyOverrideMode[Index]       = 2;
        SbSetupData->PchPcieNonSnoopLatencyOverrideMultiplier[Index] = 2;
        SbSetupData->PchPcieNonSnoopLatencyOverrideValue[Index]      = 60;
        SbSetupData->PchPcieForceLtrOverride[Index] = 0;        
        SbSetupData->PchPcieClkReqOverride[Index] = 0;
        SbSetupData->PchPcieClkReqNumber[Index] = 0;
        SbSetupData->RstPcieRemapEnabled[Index] = 0;
    }
    SbSetupData->PcieSwEqOverride = 0;
    SbSetupData->PcieSwEqCoeffCm[0] = 6; 	    
    SbSetupData->PcieSwEqCoeffCp[0] = 2;	    
    SbSetupData->PcieSwEqCoeffCm[1] = 4;	    
    SbSetupData->PcieSwEqCoeffCp[1] = 2;
    SbSetupData->PcieSwEqCoeffCm[2] = 8;	    
    SbSetupData->PcieSwEqCoeffCp[2] = 2;		    
    SbSetupData->PcieSwEqCoeffCm[3] = 2;	    
    SbSetupData->PcieSwEqCoeffCp[3] = 2;	    
    SbSetupData->PcieSwEqCoeffCm[4] = 10; 	    
    SbSetupData->PcieSwEqCoeffCp[4] = 2;		    
		    
    // Thermal
    SbSetupData->PchCrossThrottling                  = 0;
    //SbSetupData->PchDisableEnergyReport              = 1;
    SbSetupData->TenSecondPowerButtonEnable          = 0;
    
    // SkyCam CIO2 configuration
    SbSetupData->PchSkyCamCio2Enable   						= 1;
    SbSetupData->PchSkyCamPortATermOverride   				= 0;    
    SbSetupData->PchSkyCamPortBTermOverride   				= 0; 
    SbSetupData->PchSkyCamPortCTermOverride  			 	= 0; 
    SbSetupData->PchSkyCamPortDTermOverride   				= 0; 
    SbSetupData->PchSkyCamPortATrimEnable = 1;
    SbSetupData->PchSkyCamPortBTrimEnable = 1;
    SbSetupData->PchSkyCamPortCTrimEnable = 1;
    SbSetupData->PchSkyCamPortDTrimEnable = 1;
    SbSetupData->PchSkyCamPortACtleEnable = 1;
    SbSetupData->PchSkyCamPortBCtleEnable = 1;
    SbSetupData->PchSkyCamPortCDCtleEnable = 1;
    SbSetupData->PchSkyCamPortADataTrimValue = 0xBBBB;
    SbSetupData->PchSkyCamPortBDataTrimValue = 0xBBBB;
    SbSetupData->PchSkyCamPortCDDataTrimValue = 0xCCCC;
    SbSetupData->PchSkyCamPortACtleCapValue =0xE;
    SbSetupData->PchSkyCamPortBCtleCapValue =0xE;
    SbSetupData->PchSkyCamPortCDCtleCapValue =0xE;
    SbSetupData->PchSkyCamPortACtleResValue =0xD;
    SbSetupData->PchSkyCamPortBCtleResValue =0xD;
    SbSetupData->PchSkyCamPortCDCtleResValue =0xD;
    SbSetupData->PchSkyCamPortAClkTrimValue =0xA;
    SbSetupData->PchSkyCamPortBClkTrimValue =0xA;
    SbSetupData->PchSkyCamPortCClkTrimValue =0x9;
    SbSetupData->PchSkyCamPortDClkTrimValue =0xA;  
    
    // Serial IO
    SbSetupData->PchGpio                             = 1;
    SbSetupData->PchGpioIrqRoute                     = 14;
    SbSetupData->PchAdditionalSerialIoDevices        = 0;
    SbSetupData->PchGpioDebounce                     = 0;
    SbSetupData->PchGpioTestDevices                  = 0;
    SbSetupData->PchSpiCsPolaritySelect[0]           = 1;
    SbSetupData->PchSpiCsPolaritySelect[1]           = 0;
    SbSetupData->PchSerialIo                         = 0;
    SbSetupData->PchSerialIoAcpiMode                 = 0;
    for(Index = 0; Index < PCH_SERIALIO_MAX_CONTROLLERS; Index++)
    {
        if (Index < PCH_SERIALIO_MAX_I2C_CONTROLLERS)
            SbSetupData->PchI2cVoltageSelect[Index]  = 0;
        
        if((Index == 0) || (Index == 1) || (Index == 8))
            SbSetupData->PchSerialIoDevice[Index]    = 1;
        else
            SbSetupData->PchSerialIoDevice[Index]    = 0;
    }
    SbSetupData->PchI2cVoltageSelect[2]              = 1;
    SbSetupData->PchI2cVoltageSelect[3]              = 1;
    SbSetupData->PchI2c0SensorDevice                 = 0;
    SbSetupData->PchI2c0SensorIrqMode                = 1;
    SbSetupData->PchI2c1SensorDevice                 = 0;
    SbSetupData->PchI2c1SensorIrqMode                = 1;
    SbSetupData->PchI2c1TouchpanelBusAddress         = 0;
    SbSetupData->PchI2c1TouchpanelHidAddress         = 0;
    SbSetupData->PchI2c1TouchpanelSpeed              = 0;    
    SbSetupData->PchI2c2SensorDevice                 = 0;
    SbSetupData->PchI2c3SensorDevice                 = 0;
    SbSetupData->PchI2c4SensorDevice                 = 0;
    SbSetupData->PchSpi1SensorDevice                 = 0;
    SbSetupData->PchSpi1SensorIrqMode                = 1;
    SbSetupData->PchUart0SensorDevice                = 0;
    SbSetupData->PchUart0SensorIrqMode               = 1;
    SbSetupData->PchUartHwFlowCtrl[0]                = 1;
    SbSetupData->PchUartHwFlowCtrl[1]                = 1;
    SbSetupData->PchUartHwFlowCtrl[2]                = 1;
    SbSetupData->PchI2c5SensorDevice                 = 0;
    SbSetupData->PchUart1SensorDevice                = 0;
    SbSetupData->PchUart2SensorDevice                = 0;
    SbSetupData->PchI2cWittDevice                    = 0;
    SbSetupData->PchI2cWittVersion                   = 1;
    SbSetupData->PchUartUtkDevice                    = 0;
    SbSetupData->GnssConnection                      = 0;
    SbSetupData->GnssModel                           = 0;
    SbSetupData->PchFpsEnable                        = 2;    
    SbSetupData->PchSerialIoUseTimingParameters      = 0;
    SbSetupData->PchSerialIoTimingSSHI               = 528;
    SbSetupData->PchSerialIoTimingSSLI               = 640;
    SbSetupData->PchSerialIoTimingSSDI               = 30;
    SbSetupData->PchSerialIoTimingFMHI               = 128;
    SbSetupData->PchSerialIoTimingFMLI               = 160;
    SbSetupData->PchSerialIoTimingFMDI               = 30;
    SbSetupData->PchSerialIoTimingFPHI               = 48;
    SbSetupData->PchSerialIoTimingFPLI               = 64;
    SbSetupData->PchSerialIoTimingFPDI               = 30;
    SbSetupData->PchSerialIoTimingM0CI               = 200;
    SbSetupData->PchSerialIoTimingM1CI               = 2000;
    SbSetupData->PchSerialIoTimingM0CS               = 200;
    SbSetupData->PchSerialIoTimingM1CS               = 2000;
    SbSetupData->PchSerialIoTimingM0CU               = 200;
    SbSetupData->PchSerialIoTimingM1CU               = 200;
    SbSetupData->TouchPanelMuxSelector               = 0;
    SbSetupData->BtUartMuxSelector                   = 0;
    
    // SCS
    SbSetupData->PchScsSdCardEnabled                 = 1;
    SbSetupData->PchScsEmmcEnabled                   = 1;
    SbSetupData->PchScsSdCardSidebandEventsSupport   = 0;    
    SbSetupData->PchScsEmmcHs400Enabled              = 1;
    SbSetupData->PchScsEmmcHs400DriverStrength       = 0;
    
    // Integrated Sensor Hub (ISH) configuration
    SbSetupData->PchIshEnable                        = 1;
    SbSetupData->PchIshSpiGpioAssign                 = 0;
    SbSetupData->PchIshUart0GpioAssign               = 0;
    SbSetupData->PchIshUart1GpioAssign               = 0;
    SbSetupData->PchIshI2c0GpioAssign                = 0;
    SbSetupData->PchIshI2c1GpioAssign                = 0;
    SbSetupData->PchIshI2c2GpioAssign                = 0;
    SbSetupData->PchIshGp0GpioAssign                 = 0;
    SbSetupData->PchIshGp1GpioAssign                 = 0;
    SbSetupData->PchIshGp2GpioAssign                 = 0;
    SbSetupData->PchIshGp3GpioAssign                 = 0;
    SbSetupData->PchIshGp4GpioAssign                 = 0;
    SbSetupData->PchIshGp5GpioAssign                 = 0;
    SbSetupData->PchIshGp6GpioAssign                 = 0;
    SbSetupData->PchIshGp7GpioAssign                 = 0;
    SbSetupData->PchIshPdtUnlock                     = 0;
            
    // TraceHub
    SbSetupData->TraceHubEnableMode                  = 0; // TraceHub Host Debugger
    SbSetupData->MemRegion0BufferSize                = 1;
    SbSetupData->MemRegion1BufferSize                = 1;
    
    //
    // Thermal Throttling Level options
    //
    SbSetupData->PchTtLevelSuggestSet                =1;
    SbSetupData->PchThrmT0Level                      =0;
    SbSetupData->PchThrmT1Level                      =0;
    SbSetupData->PchThrmT2Level                      =0;
    SbSetupData->PchThrmTtEnable                     =0;
    SbSetupData->PchThrmTtState13Enable              =0;
    SbSetupData->PchThrmTtLock                       =0;

    //
    // DMI Thermal Throttling
    //
    SbSetupData->PchDmiTsSuggestSet                  =1;
    SbSetupData->PchTs0Width                         =0;
    SbSetupData->PchTs1Width                         =0;
    SbSetupData->PchTs2Width                         =0;
    SbSetupData->PchTs3Width                         =0;
    SbSetupData->PchDmiTsawEn                        =0;
    //
    // SATA Thermal Throttling
    //
    SbSetupData->PchSataTsSuggestSet                 =1;
    SbSetupData->PchP0T1M                            =1;
    SbSetupData->PchP0T2M                            =2;
    SbSetupData->PchP0T3M                            =3;
    SbSetupData->PchP0TDisp                          =0;
    SbSetupData->PchP0Tinact                         =0;
    SbSetupData->PchP0TDispFinit                     =0;
    SbSetupData->PchP1T1M                            =1;
    SbSetupData->PchP1T2M                            =2;
    SbSetupData->PchP1T3M                            =3;
    SbSetupData->PchP1TDisp                          =0;
    SbSetupData->PchP1Tinact                         =0;
    SbSetupData->PchP1TDispFinit                     =0;
    SbSetupData->SmbusSpdWriteDisable                =0;
   
    //  PCH Thermal
    SbSetupData->AutoThermalReporting                = 0;
    SbSetupData->Ac1TripPoint                        = 55;
    SbSetupData->Ac0TripPoint                        = 71;
    SbSetupData->Ac0FanSpeed                         = 100;
    SbSetupData->Ac1FanSpeed                         = 75;
    SbSetupData->PassiveThermalTripPoint             = 95;
    SbSetupData->CriticalThermalTripPoint            = 119;
    SbSetupData->PassiveTc1Value                     = 1;
    SbSetupData->PassiveTc2Value                     = 5;
    SbSetupData->PassiveTspValue                     = 10;
    
    SbSetupData->DisableActiveTripPoints             = 1;
    SbSetupData->DisablePassiveTripPoints            = 0;
    SbSetupData->DisableCriticalTripPoints           = 1;
    
    SbSetupData->ThermalDeviceEnable                 = 1; 
    SbSetupData->CPUTempReadEnable                   = 1;
    SbSetupData->CPUEnergyReadEnable                 = 1;    
    SbSetupData->PCHTempReadEnable                   = 1;    
    SbSetupData->AlertEnableLock                     = 0;
    SbSetupData->PchAlert                            = 0;
    SbSetupData->DimmAlert                           = 0;
    SbSetupData->CpuTemp                             = 72;
    SbSetupData->CpuFanSpeed                         = 65;

//    SbSetupData->TPV_Restrict_Enable      = 0;
    
    // EC turbo control test mode
    SbSetupData->ECTurboControlMode                  = 0;
    SbSetupData->ACBrickCapacity                     = 1;
    SbSetupData->ECPollingPeriod                     = 1;
    SbSetupData->ECGuardBandValue                    = 0;
    SbSetupData->ECAlgorithmSel                      = 1;
    SbSetupData->BiosGuard                           = 0;
    SbSetupData->MemoryThermalManagement             = 0;
    SbSetupData->ExttsViaTsOnBoard                   = 0;   
    SbSetupData->ExttsViaTsOnDimm                    = 0; 
    SbSetupData->SlpS0VmSupport                      = 0;        
    SbSetupData->DvrmVmIoExpCapabilitySupport        = 0;
    SbSetupData->WWANEnable                          = 1;    
    
    // Thunderbolt
    SbSetupData->TbtSupport                          = 2;
    
    SbSetupData->AudioConnector                      = 0;    
    
    // Porting End
    
    if (SetupData != NULL) {
        // Porting Start
        SbSetupData->SensorHubType                       = SetupData->SensorHubType;
        SbSetupData->DeepSxMode                          = SetupData->DeepSxMode;
        SbSetupData->LanWakeFromDeepSx                   = SetupData->LanWakeFromDeepSx;
        SbSetupData->PmcReadDisable                      = SetupData->PmcReadDisable;        
        SbSetupData->OsDebugPort                         = SetupData->OsDebugPort;
        SbSetupData->PchDciEn                            = SetupData->PchDciEn;
        SbSetupData->PchLpcClockRun                      = SetupData->PchLpcClockRun;
        SbSetupData->PchCrid                             = SetupData->PchCrid;
        SbSetupData->PchDisableEnergyReport              = SetupData->PchDisableEnergyReport;
        SbSetupData->EnableTcoTimer                      = SetupData->EnableTcoTimer;
        SbSetupData->PchCapsuleResetType                 = SetupData->PchCapsuleResetType;
        SbSetupData->PchIoApic24119Entries               = SetupData->PchIoApic24119Entries;
        SbSetupData->PchP2sbUnlock                       = SetupData->PchP2sbUnlock;      
        SbSetupData->FprrEnable                          = SetupData->FprrEnable;        
        SbSetupData->Hpet                                = SetupData->Hpet;
        SbSetupData->StateAfterG3                        = SetupData->StateAfterG3;
        SbSetupData->IchPort80Route                      = SetupData->IchPort80Route;
        SbSetupData->EnhancePort8xhDecoding              = SetupData->EnhancePort8xhDecoding;        
        SbSetupData->PchSirqMode                         = SetupData->PchSirqMode;
        SbSetupData->PchPort61hSmm                       = SetupData->PchPort61hSmm;
        SbSetupData->PciePllSsc                          = SetupData->PciePllSsc;        
        SbSetupData->WdtEnable                           = SetupData->WdtEnable; 
        
        // security
        SbSetupData->PchBiosLock                         = SetupData->PchBiosLock;
        SbSetupData->PchRtcLock                          = SetupData->PchRtcLock;
        
        // USB
        //SbSetupData->PchUsb20[0]                         = SetupData->PchUsb20[0];
        //SbSetupData->PchUsb20[1]                         = SetupData->PchUsb20[1];
        SbSetupData->DisableComplianceMode               = SetupData->DisableComplianceMode;       
        SbSetupData->PchUsbPortDisable                   = SetupData->PchUsbPortDisable;
        SbSetupData->UsbSensorHub                        = SetupData->UsbSensorHub;
        SbSetupData->UsbSsicEnable[0]                    = SetupData->UsbSsicEnable[0];
        SbSetupData->UsbSsicEnable[1]                    = SetupData->UsbSsicEnable[1];
        for(Index = 0; Index < GetPchXhciMaxUsb2PortNum(); Index++)
        {
            SbSetupData->PchUsbHsPort[Index]             = SetupData->PchUsbHsPort[Index];
        }
        for(Index = 0; Index < GetPchXhciMaxUsb3PortNum(); Index++)
        {
            SbSetupData->PchUsbSsPort[Index]             = SetupData->PchUsbSsPort[Index];
        }
        
        // Xdci
        SbSetupData->PchXdciSupport                      = SetupData->PchXdciSupport;
        
        // Lan
        SbSetupData->PchLan                              = SetupData->PchLan;
        SbSetupData->PchWakeOnLan                        = SetupData->PchWakeOnLan;
        SbSetupData->PchWakeOnWlan                       = SetupData->PchWakeOnWlan;
        SbSetupData->PchWakeOnWlanDeepSx                 = SetupData->PchWakeOnWlanDeepSx;
        SbSetupData->DisableDsxAcPresentPulldown         = SetupData->DisableDsxAcPresentPulldown;
        SbSetupData->SlpLanLowDc                         = SetupData->SlpLanLowDc;
        SbSetupData->PchLanK1Off                         = SetupData->PchLanK1Off;
        
        // Audio
        SbSetupData->PchHdAudio                          = SetupData->PchHdAudio;
        SbSetupData->PchHdAudioDsp                       = SetupData->PchHdAudioDsp;
        SbSetupData->PchHdAudioDspUaaCompliance          = SetupData->PchHdAudioDspUaaCompliance;
        SbSetupData->PchHdaIDisplayCodecDisconnect       = SetupData->PchHdaIDisplayCodecDisconnect;        
        SbSetupData->PchHdAudioPme                       = SetupData->PchHdAudioPme;
        SbSetupData->PchHdAudioIoBufferOwnership         = SetupData->PchHdAudioIoBufferOwnership;
        SbSetupData->PchHdAudioIoBufferVoltage           = SetupData->PchHdAudioIoBufferVoltage;
        SbSetupData->PchHdaHdAudioLinkFreq               = SetupData->PchHdaHdAudioLinkFreq;
        SbSetupData->PchHdaIDisplayLinkFreq              = SetupData->PchHdaIDisplayLinkFreq;
        SbSetupData->PchHdaIDisplayLinkTmode             = SetupData->PchHdaIDisplayLinkTmode;
        SbSetupData->PchHdAudioCodecSelect               = SetupData->PchHdAudioCodecSelect;
        SbSetupData->PchHdAudioI2sCodecSelect            = SetupData->PchHdAudioI2sCodecSelect;
        SbSetupData->PchHdAudioNhltEndpointDmic          = SetupData->PchHdAudioNhltEndpointDmic;
        SbSetupData->PchHdAudioNhltEndpointBluetooth     = SetupData->PchHdAudioNhltEndpointBluetooth;
        SbSetupData->PchHdAudioNhltEndpointI2s           = SetupData->PchHdAudioNhltEndpointI2s;
        for(Index = 0; Index < HDAUDIO_PP_MODULES; Index++)
        {
            if (Index < HDAUDIO_FEATURES) SbSetupData->PchHdAudioFeature[Index] = SetupData->PchHdAudioFeature[Index];
            SbSetupData->PchHdAudioPostProcessingMod[Index] = SetupData->PchHdAudioPostProcessingMod[Index];
        }
        
        // SATA
        SbSetupData->PchSata                             = SetupData->PchSata;
        SbSetupData->SataInterfaceMode                   = SetupData->SataInterfaceMode;
        SbSetupData->SataRaidR0                          = SetupData->SataRaidR0;
        SbSetupData->SataRaidR1                          = SetupData->SataRaidR1;
        SbSetupData->SataRaidR10                         = SetupData->SataRaidR10;
        SbSetupData->SataRaidR5                          = SetupData->SataRaidR5;
        SbSetupData->SataRaidIrrt                        = SetupData->SataRaidIrrt;
        SbSetupData->SataRaidOub                         = SetupData->SataRaidOub;
        SbSetupData->SataHddlk                           = SetupData->SataHddlk;
        SbSetupData->SataLedl                            = SetupData->SataLedl;
        SbSetupData->SataRaidIooe                        = SetupData->SataRaidIooe;
        SbSetupData->SataRaidSrt                         = SetupData->SataRaidSrt;
        SbSetupData->SataRaidOromDelay                   = SetupData->SataRaidOromDelay;
        SbSetupData->SataRstForceForm                    = SetupData->SataRstForceForm;
        SbSetupData->RaidDeviceId                        = SetupData->RaidDeviceId;
        SbSetupData->SataSalp                            = SetupData->SataSalp;
        SbSetupData->SataControllerSpeed                 = SetupData->SataControllerSpeed;        
        SbSetupData->SataTestMode                        = SetupData->SataTestMode;
        for(Index = 0; Index < GetPchMaxSataPortNum(); Index++)
        {
            SbSetupData->SataPort[Index]                 = SetupData->SataPort[Index];
            SbSetupData->SataHotPlug[Index]              = SetupData->SataHotPlug[Index];
            SbSetupData->SataMechanicalSw[Index]         = SetupData->SataMechanicalSw[Index];
            SbSetupData->SataSpinUp[Index]               = SetupData->SataSpinUp[Index];
            SbSetupData->SataExternal[Index]             = SetupData->SataExternal[Index];
            SbSetupData->SataType[Index]                 = SetupData->SataType[Index];
            SbSetupData->SataTopology[Index]             = SetupData->SataTopology[Index];            
            SbSetupData->PxDevSlp[Index]                 = SetupData->PxDevSlp[Index];
            SbSetupData->EnableDitoConfig[Index]         = SetupData->EnableDitoConfig[Index];
            SbSetupData->DitoVal[Index]                  = SetupData->DitoVal[Index];
            SbSetupData->DmVal[Index]                    = SetupData->DmVal[Index];
        }
        
        SbSetupData->RstPcieStorageRemap[0]              = SetupData->RstPcieStorageRemap[0];
        SbSetupData->RstPcieStorageRemapPort[0]          = SetupData->RstPcieStorageRemapPort[0];
        SbSetupData->RstPcieStorageRemap[1]              = SetupData->RstPcieStorageRemap[1];
        SbSetupData->RstPcieStorageRemapPort[1]          = SetupData->RstPcieStorageRemapPort[1];
        SbSetupData->RstPcieStorageRemap[2]              = SetupData->RstPcieStorageRemap[2];
        SbSetupData->RstPcieStorageRemapPort[2]          = SetupData->RstPcieStorageRemapPort[2];
            
        // PCIE
        SbSetupData->PchDmiAspm                          = SetupData->PchDmiAspm;
        for(Index = 0; Index < GetPchMaxPciePortNum(); Index++) 
        {
            SbSetupData->PcieExtraBusRsvd[Index]        = SetupData->PcieExtraBusRsvd[Index];
            SbSetupData->PcieMemRsvd[Index]             = SetupData->PcieMemRsvd[Index];
            SbSetupData->PcieIoRsvd[Index]              = SetupData->PcieIoRsvd[Index];
#if defined(SB_SETUP_SUPPORT) && (SB_SETUP_SUPPORT == 1)            
            SbSetupData->PciePFMemRsvd[Index]           = SetupData->PciePFMemRsvd[Index];
#endif            
        }
        SbSetupData->SataLegacyOrom                      = SetupData->SataLegacyOrom;
        SbSetupData->SataRstOptaneMemory                 = SetupData->SataRstOptaneMemory;
        SbSetupData->SataRstCpuAttachedStorage           = SetupData->SataRstCpuAttachedStorage;
        SbSetupData->HybridStorageMode                   = SetupData->HybridStorageMode;
        SbSetupData->PchLegacyIoLowLatency               = SetupData->PchLegacyIoLowLatency;
        SbSetupData->PcieClockGatingDisabled             = SetupData->PcieClockGatingDisabled;
        SbSetupData->PcieUsbGlitchWa                     = SetupData->PcieUsbGlitchWa;
        SbSetupData->PcieRootPort8xhDecode               = SetupData->PcieRootPort8xhDecode;
        SbSetupData->Pcie8xhDecodePortIndex              = SetupData->Pcie8xhDecodePortIndex;
        SbSetupData->PcieRootPortPeerMemoryWriteEnable   = SetupData->PcieRootPortPeerMemoryWriteEnable;
        SbSetupData->PcieComplianceTestMode              = SetupData->PcieComplianceTestMode;
        SbSetupData->RpFunctionSwap                      = SetupData->RpFunctionSwap;   
        for(Index = 0; Index < GetPchMaxPciePortNum(); Index++)
        {
            SbSetupData->PcieRootPortEn[Index]           = SetupData->PcieRootPortEn[Index];
            SbSetupData->PcieTopology[Index]             = SetupData->PcieTopology[Index];
            SbSetupData->PcieDetectTimeoutMs[Index]      = SetupData->PcieDetectTimeoutMs[Index];
            SbSetupData->PcieRootPortAspm[Index]         = SetupData->PcieRootPortAspm[Index];
            SbSetupData->PcieRootPortURE[Index]          = SetupData->PcieRootPortURE[Index];  
            SbSetupData->PcieRootPortFEE[Index]          = SetupData->PcieRootPortFEE[Index];  
            SbSetupData->PcieRootPortNFE[Index]          = SetupData->PcieRootPortNFE[Index];  
            SbSetupData->PcieRootPortCEE[Index]          = SetupData->PcieRootPortCEE[Index];  
            SbSetupData->PcieRootPortCTD[Index]          = SetupData->PcieRootPortCTD[Index];  
            SbSetupData->PcieRootPortPIE[Index]          = SetupData->PcieRootPortPIE[Index];  
            SbSetupData->PcieRootPortSFE[Index]          = SetupData->PcieRootPortSFE[Index];  
            SbSetupData->PcieRootPortSNE[Index]          = SetupData->PcieRootPortSNE[Index];  
            SbSetupData->PcieRootPortSCE[Index]          = SetupData->PcieRootPortSCE[Index];  
            SbSetupData->PcieRootPortPMCE[Index]         = SetupData->PcieRootPortPMCE[Index]; 
            SbSetupData->PcieRootPortHPE[Index]          = SetupData->PcieRootPortHPE[Index];
            SbSetupData->PcieRootPortAER[Index]          = SetupData->PcieRootPortAER[Index];
            SbSetupData->PcieRootPortSpeed[Index]        = SetupData->PcieRootPortSpeed[Index];
            SbSetupData->PcieRootPortTHS[Index]          = SetupData->PcieRootPortTHS[Index];
            SbSetupData->PcieRootPortL1SubStates[Index]  = SetupData->PcieRootPortL1SubStates[Index];
            SbSetupData->PcieRootPortACS[Index]          = SetupData->PcieRootPortACS[Index];
            
            SbSetupData->PcieRootPortEqPh3Method[Index]  = SetupData->PcieRootPortEqPh3Method[Index];
            SbSetupData->PcieRootPortUptp[Index]         = SetupData->PcieRootPortUptp[Index];
            SbSetupData->PcieRootPortDptp[Index]         = SetupData->PcieRootPortDptp[Index];              
            SbSetupData->PcieLaneCm[Index]               = SetupData->PcieLaneCm[Index];
            SbSetupData->PcieLaneCp[Index]               = SetupData->PcieLaneCp[Index];
#if defined(SB_SETUP_SUPPORT) && (SB_SETUP_SUPPORT == 1)            
            SbSetupData->PcieRPDetectNonComplaint[Index] = SetupData->PcieRPDetectNonComplaint[Index];
            SbSetupData->PcieMemRsvdalig[Index]          = SetupData->PcieMemRsvdalig[Index];
            SbSetupData->PciePFMemRsvdalig[Index]        = SetupData->PciePFMemRsvdalig[Index];
#endif            
            
            SbSetupData->PchPcieLtrEnable[Index]         = SetupData->PchPcieLtrEnable[Index];
            SbSetupData->PchPcieLtrConfigLock[Index]     = SetupData->PchPcieLtrConfigLock[Index];
            SbSetupData->PchPcieSnoopLatencyOverrideMode[Index]          = SetupData->PchPcieSnoopLatencyOverrideMode[Index];
            SbSetupData->PchPcieSnoopLatencyOverrideMultiplier[Index]    = SetupData->PchPcieSnoopLatencyOverrideMultiplier[Index];
            SbSetupData->PchPcieSnoopLatencyOverrideValue[Index]         = SetupData->PchPcieSnoopLatencyOverrideValue[Index];
            SbSetupData->PchPcieNonSnoopLatencyOverrideMode[Index]       = SetupData->PchPcieNonSnoopLatencyOverrideMode[Index];
            SbSetupData->PchPcieNonSnoopLatencyOverrideMultiplier[Index] = SetupData->PchPcieNonSnoopLatencyOverrideMultiplier[Index];
            SbSetupData->PchPcieNonSnoopLatencyOverrideValue[Index]      = SetupData->PchPcieNonSnoopLatencyOverrideValue[Index];
            SbSetupData->PchPcieForceLtrOverride[Index]                  = SetupData->PchPcieForceLtrOverride[Index];
            SbSetupData->PchPcieClkReqOverride[Index]                    = SetupData->PchPcieClkReqOverride[Index];
            SbSetupData->PchPcieClkReqNumber[Index]                      = SetupData->PchPcieClkReqNumber[Index];
            SbSetupData->RstPcieRemapEnabled[Index]                      = SetupData->RstPcieRemapEnabled[Index];
        }
        
        SbSetupData->PcieSwEqOverride = SetupData->PcieSwEqOverride;
        for(Index = 0; Index < 5; Index++)
        {        
            SbSetupData->PcieSwEqCoeffCm[Index] = SetupData->PcieSwEqCoeffCm[Index]; 	    
            SbSetupData->PcieSwEqCoeffCp[Index] = SetupData->PcieSwEqCoeffCp[Index];	    
        }
        
        // SkyCam CIO2 configuration
        SbSetupData->PchSkyCamCio2Enable                                = SetupData->PchSkyCamCio2Enable;       
        SbSetupData->PchSkyCamPortATermOverride                         = SetupData->PchSkyCamPortATermOverride;
        SbSetupData->PchSkyCamPortBTermOverride                         = SetupData->PchSkyCamPortBTermOverride;
        SbSetupData->PchSkyCamPortCTermOverride                         = SetupData->PchSkyCamPortCTermOverride;
        SbSetupData->PchSkyCamPortDTermOverride                         = SetupData->PchSkyCamPortDTermOverride;

        SbSetupData->PchSkyCamPortATrimEnable = SetupData->PchSkyCamPortATrimEnable;
        SbSetupData->PchSkyCamPortBTrimEnable = SetupData->PchSkyCamPortBTrimEnable;
        SbSetupData->PchSkyCamPortCTrimEnable = SetupData->PchSkyCamPortCTrimEnable;
        SbSetupData->PchSkyCamPortDTrimEnable = SetupData->PchSkyCamPortDTrimEnable;
        SbSetupData->PchSkyCamPortACtleEnable = SetupData->PchSkyCamPortACtleEnable;
        SbSetupData->PchSkyCamPortBCtleEnable = SetupData->PchSkyCamPortBCtleEnable;
        SbSetupData->PchSkyCamPortCDCtleEnable = SetupData->PchSkyCamPortCDCtleEnable;
        SbSetupData->PchSkyCamPortADataTrimValue = SetupData->PchSkyCamPortADataTrimValue;
        SbSetupData->PchSkyCamPortBDataTrimValue = SetupData->PchSkyCamPortBDataTrimValue;
        SbSetupData->PchSkyCamPortCDDataTrimValue = SetupData->PchSkyCamPortCDDataTrimValue;
        SbSetupData->PchSkyCamPortACtleCapValue =SetupData->PchSkyCamPortACtleCapValue;
        SbSetupData->PchSkyCamPortBCtleCapValue =SetupData->PchSkyCamPortBCtleCapValue;
        SbSetupData->PchSkyCamPortCDCtleCapValue =SetupData->PchSkyCamPortCDCtleCapValue;
        SbSetupData->PchSkyCamPortACtleResValue =SetupData->PchSkyCamPortACtleResValue;
        SbSetupData->PchSkyCamPortBCtleResValue =SetupData->PchSkyCamPortBCtleResValue;
        SbSetupData->PchSkyCamPortCDCtleResValue =SetupData->PchSkyCamPortCDCtleResValue;
        SbSetupData->PchSkyCamPortAClkTrimValue =SetupData->PchSkyCamPortAClkTrimValue;
        SbSetupData->PchSkyCamPortBClkTrimValue =SetupData->PchSkyCamPortBClkTrimValue;
        SbSetupData->PchSkyCamPortCClkTrimValue =SetupData->PchSkyCamPortCClkTrimValue;
        SbSetupData->PchSkyCamPortDClkTrimValue =SetupData->PchSkyCamPortDClkTrimValue;        
        
        // Serial IO
        SbSetupData->PchGpio                             = SetupData->PchGpio;
        SbSetupData->PchGpioIrqRoute                     = SetupData->PchGpioIrqRoute;
        SbSetupData->PchAdditionalSerialIoDevices        = SetupData->PchAdditionalSerialIoDevices;
        SbSetupData->PchGpioDebounce                     = SetupData->PchGpioDebounce;
        SbSetupData->PchGpioTestDevices                  = SetupData->PchGpioTestDevices;
        SbSetupData->PchSpiCsPolaritySelect[0]           = SetupData->PchSpiCsPolaritySelect[0];
        SbSetupData->PchSpiCsPolaritySelect[1]           = SetupData->PchSpiCsPolaritySelect[1];
        SbSetupData->PchSerialIo                         = SetupData->PchSerialIo;
        SbSetupData->PchSerialIoAcpiMode                 = SetupData->PchSerialIoAcpiMode;
        for(Index = 0; Index < PCH_SERIALIO_MAX_CONTROLLERS; Index++)
        {
            if (Index < PCH_SERIALIO_MAX_I2C_CONTROLLERS)
                SbSetupData->PchI2cVoltageSelect[Index]  = SetupData->PchI2cVoltageSelect[Index];
            
            SbSetupData->PchSerialIoDevice[Index]        = SetupData->PchSerialIoDevice[Index];
        }
        SbSetupData->PchI2c0SensorDevice                 = SetupData->PchI2c0SensorDevice;
        SbSetupData->PchI2c0SensorIrqMode                = SetupData->PchI2c0SensorIrqMode;
        SbSetupData->PchI2c1SensorDevice                 = SetupData->PchI2c1SensorDevice;
        SbSetupData->PchI2c1SensorIrqMode                = SetupData->PchI2c1SensorIrqMode;
        SbSetupData->PchI2c1TouchpanelBusAddress         = SetupData->PchI2c1TouchpanelBusAddress;
        SbSetupData->PchI2c1TouchpanelHidAddress         = SetupData->PchI2c1TouchpanelHidAddress;
        SbSetupData->PchI2c1TouchpanelSpeed              = SetupData->PchI2c1TouchpanelSpeed;        
        SbSetupData->PchI2c2SensorDevice                 = SetupData->PchI2c2SensorDevice;
        SbSetupData->PchI2c3SensorDevice                 = SetupData->PchI2c3SensorDevice;
        SbSetupData->PchI2c4SensorDevice                 = SetupData->PchI2c4SensorDevice;
        SbSetupData->PchSpi1SensorDevice                 = SetupData->PchSpi1SensorDevice;
        SbSetupData->PchSpi1SensorIrqMode                = SetupData->PchSpi1SensorIrqMode;
        SbSetupData->PchUart0SensorDevice                = SetupData->PchUart0SensorDevice;
        SbSetupData->PchUart0SensorIrqMode               = SetupData->PchUart0SensorIrqMode;
        SbSetupData->PchUartHwFlowCtrl[0]                = SetupData->PchUartHwFlowCtrl[0];
        SbSetupData->PchUartHwFlowCtrl[1]                = SetupData->PchUartHwFlowCtrl[1];
        SbSetupData->PchUartHwFlowCtrl[2]                = SetupData->PchUartHwFlowCtrl[2];
        SbSetupData->PchI2c5SensorDevice                 = SetupData->PchI2c5SensorDevice;
        SbSetupData->PchUart1SensorDevice                = SetupData->PchUart1SensorDevice;
        SbSetupData->PchUart2SensorDevice                = SetupData->PchUart2SensorDevice;
        SbSetupData->PchI2cWittDevice                    = SetupData->PchI2cWittDevice;
        SbSetupData->PchI2cWittVersion                   = SetupData->PchI2cWittVersion;
        SbSetupData->PchUartUtkDevice                    = SetupData->PchUartUtkDevice;
        SbSetupData->GnssConnection                      = SetupData->GnssConnection;
        SbSetupData->GnssModel                           = SetupData->GnssModel;
        SbSetupData->PchFpsEnable                        = SetupData->PchFpsEnable;          
        SbSetupData->PchSerialIoUseTimingParameters      = SetupData->PchSerialIoUseTimingParameters;
        SbSetupData->PchSerialIoTimingSSHI               = SetupData->PchSerialIoTimingSSHI;
        SbSetupData->PchSerialIoTimingSSLI               = SetupData->PchSerialIoTimingSSLI;
        SbSetupData->PchSerialIoTimingSSDI               = SetupData->PchSerialIoTimingSSDI;
        SbSetupData->PchSerialIoTimingFMHI               = SetupData->PchSerialIoTimingFMHI;
        SbSetupData->PchSerialIoTimingFMLI               = SetupData->PchSerialIoTimingFMLI;
        SbSetupData->PchSerialIoTimingFMDI               = SetupData->PchSerialIoTimingFMDI;
        SbSetupData->PchSerialIoTimingFPHI               = SetupData->PchSerialIoTimingFPHI;
        SbSetupData->PchSerialIoTimingFPLI               = SetupData->PchSerialIoTimingFPLI;
        SbSetupData->PchSerialIoTimingFPDI               = SetupData->PchSerialIoTimingFPDI;
        SbSetupData->PchSerialIoTimingM0CI               = SetupData->PchSerialIoTimingM0CI;
        SbSetupData->PchSerialIoTimingM1CI               = SetupData->PchSerialIoTimingM1CI;
        SbSetupData->PchSerialIoTimingM0CS               = SetupData->PchSerialIoTimingM0CS;
        SbSetupData->PchSerialIoTimingM1CS               = SetupData->PchSerialIoTimingM1CS;
        SbSetupData->PchSerialIoTimingM0CU               = SetupData->PchSerialIoTimingM0CU;
        SbSetupData->PchSerialIoTimingM1CU               = SetupData->PchSerialIoTimingM1CU;
        SbSetupData->TouchPanelMuxSelector               = SetupData->TouchPanelMuxSelector;
        SbSetupData->BtUartMuxSelector                   = SetupData->BtUartMuxSelector;
        
        // SCS
        SbSetupData->PchScsEmmcEnabled                   = SetupData->PchScsEmmcEnabled;
        SbSetupData->PchScsSdCardEnabled                 = SetupData->PchScsSdCardEnabled;
        SbSetupData->PchScsSdCardSidebandEventsSupport   = SetupData->PchScsSdCardSidebandEventsSupport;        
        SbSetupData->PchScsEmmcHs400Enabled              = SetupData->PchScsEmmcHs400Enabled;
        SbSetupData->PchScsEmmcHs400DriverStrength       = SetupData->PchScsEmmcHs400DriverStrength;
        
        // Integrated Sensor Hub (ISH) configuration
        SbSetupData->PchIshEnable                        = SetupData->PchIshEnable;
        SbSetupData->PchIshSpiGpioAssign                 = SetupData->PchIshSpiGpioAssign;
        SbSetupData->PchIshUart0GpioAssign               = SetupData->PchIshUart0GpioAssign;
        SbSetupData->PchIshUart1GpioAssign               = SetupData->PchIshUart1GpioAssign;
        SbSetupData->PchIshI2c0GpioAssign                = SetupData->PchIshI2c0GpioAssign;
        SbSetupData->PchIshI2c1GpioAssign                = SetupData->PchIshI2c1GpioAssign;
        SbSetupData->PchIshI2c2GpioAssign                = SetupData->PchIshI2c2GpioAssign;
        SbSetupData->PchIshGp0GpioAssign                 = SetupData->PchIshGp0GpioAssign;
        SbSetupData->PchIshGp1GpioAssign                 = SetupData->PchIshGp1GpioAssign;
        SbSetupData->PchIshGp2GpioAssign                 = SetupData->PchIshGp2GpioAssign;
        SbSetupData->PchIshGp3GpioAssign                 = SetupData->PchIshGp3GpioAssign;
        SbSetupData->PchIshGp4GpioAssign                 = SetupData->PchIshGp4GpioAssign;
        SbSetupData->PchIshGp5GpioAssign                 = SetupData->PchIshGp5GpioAssign;
        SbSetupData->PchIshGp6GpioAssign                 = SetupData->PchIshGp6GpioAssign;
        SbSetupData->PchIshGp7GpioAssign                 = SetupData->PchIshGp7GpioAssign;
        SbSetupData->PchIshPdtUnlock                     = SetupData->PchIshPdtUnlock;
        
        // TraceHub
        SbSetupData->TraceHubEnableMode                  = SetupData->TraceHubEnableMode;
        SbSetupData->MemRegion0BufferSize                = SetupData->MemRegion0BufferSize;
        SbSetupData->MemRegion1BufferSize                = SetupData->MemRegion1BufferSize;

        //
        // Thermal Throttling Level options
        //
        SbSetupData->PchTtLevelSuggestSet                = SetupData->PchTtLevelSuggestSet;
        SbSetupData->PchThrmT0Level                      = SetupData->PchThrmT0Level;
        SbSetupData->PchThrmT1Level                      = SetupData->PchThrmT1Level;
        SbSetupData->PchThrmT2Level                      = SetupData->PchThrmT2Level;
        SbSetupData->PchThrmTtEnable                     = SetupData->PchThrmTtEnable;
        SbSetupData->PchThrmTtState13Enable              = SetupData->PchThrmTtState13Enable;
        SbSetupData->PchThrmTtLock                       = SetupData->PchThrmTtLock;

        //
        // DMI Thermal Throttling
        //
        SbSetupData->PchDmiTsSuggestSet                  = SetupData->PchDmiTsSuggestSet;
        SbSetupData->PchTs0Width                         = SetupData->PchTs0Width;
        SbSetupData->PchTs1Width                         = SetupData->PchTs1Width;
        SbSetupData->PchTs2Width                         = SetupData->PchTs2Width;
        SbSetupData->PchTs3Width                         = SetupData->PchTs3Width;
        SbSetupData->PchDmiTsawEn                        = SetupData->PchDmiTsawEn;
        //
        // SATA Thermal Throttling
        //
        SbSetupData->PchSataTsSuggestSet                 = SetupData->PchSataTsSuggestSet;
        SbSetupData->PchP0T1M                            = SetupData->PchP0T1M;
        SbSetupData->PchP0T2M                            = SetupData->PchP0T2M;
        SbSetupData->PchP0T3M                            = SetupData->PchP0T3M;
        SbSetupData->PchP0TDisp                          = SetupData->PchP0TDisp;
        SbSetupData->PchP0Tinact                         = SetupData->PchP0Tinact;
        SbSetupData->PchP0TDispFinit                     = SetupData->PchP0TDispFinit;
        SbSetupData->PchP1T1M                            = SetupData->PchP1T1M;
        SbSetupData->PchP1T2M                            = SetupData->PchP1T2M;
        SbSetupData->PchP1T3M                            = SetupData->PchP1T3M;
        SbSetupData->PchP1TDisp                          = SetupData->PchP1TDisp;
        SbSetupData->PchP1Tinact                         = SetupData->PchP1Tinact;
        SbSetupData->PchP1TDispFinit                     = SetupData->PchP1TDispFinit;        
        SbSetupData->SmbusSpdWriteDisable                = SetupData->SmbusSpdWriteDisable;
        
        //  PCH Thermal
        SbSetupData->PchCrossThrottling                  = SetupData->PchCrossThrottling;
        //SbSetupData->PchDisableEnergyReport              = SetupData->PchDisableEnergyReport;
        SbSetupData->AutoThermalReporting                = SetupData->AutoThermalReporting;
        SbSetupData->Ac1TripPoint                        = SetupData->Ac1TripPoint;
        SbSetupData->Ac0TripPoint                        = SetupData->Ac0TripPoint;
        SbSetupData->Ac0FanSpeed                         = SetupData->Ac0FanSpeed;
        SbSetupData->Ac1FanSpeed                         = SetupData->Ac1FanSpeed;
        SbSetupData->PassiveThermalTripPoint             = SetupData->PassiveThermalTripPoint;
        SbSetupData->CriticalThermalTripPoint            = SetupData->CriticalThermalTripPoint;
        SbSetupData->PassiveTc1Value                     = SetupData->PassiveTc1Value;
        SbSetupData->PassiveTc2Value                     = SetupData->PassiveTc2Value;
        SbSetupData->PassiveTspValue                     = SetupData->PassiveTspValue;
        
        SbSetupData->DisableActiveTripPoints             = SetupData->DisableActiveTripPoints;
        SbSetupData->DisablePassiveTripPoints            = SetupData->DisablePassiveTripPoints;
        SbSetupData->DisableCriticalTripPoints           = SetupData->DisableCriticalTripPoints;

        SbSetupData->ThermalDeviceEnable                 = SetupData->ThermalDeviceEnable;
        SbSetupData->CPUTempReadEnable                   = SetupData->CPUTempReadEnable;
        SbSetupData->CPUEnergyReadEnable                 = SetupData->CPUEnergyReadEnable;
        SbSetupData->PCHTempReadEnable                   = SetupData->PCHTempReadEnable;                
        SbSetupData->AlertEnableLock                     = SetupData->AlertEnableLock;
        SbSetupData->PchAlert                            = SetupData->PchAlert;
        SbSetupData->DimmAlert                           = SetupData->DimmAlert;
        SbSetupData->CpuTemp                             = SetupData->CpuTemp;
        SbSetupData->CpuFanSpeed                         = SetupData->CpuFanSpeed;

//      SbSetupData->TPV_Restrict_Enable    = SetupData->TPV_Restrict_Enable;
       
        SbSetupData->ECTurboControlMode                  = SetupData->EcTurboControlMode;
        SbSetupData->ACBrickCapacity                     = SetupData->AcBrickCapacity;
        SbSetupData->ECPollingPeriod                     = SetupData->EcPollingPeriod;
        SbSetupData->ECGuardBandValue                    = SetupData->EcGuardBandValue;
        SbSetupData->ECAlgorithmSel                      = SetupData->EcAlgorithmSel;
        SbSetupData->TenSecondPowerButtonEnable          = SetupData->TenSecondPowerButtonEnable;
#if defined BIOS_GUARD_SETUP_SUPPORT && BIOS_GUARD_SETUP_SUPPORT == 1    
        SbSetupData->BiosGuard                           = SetupData->BiosGuard;
#endif
        SbSetupData->MemoryThermalManagement             = SetupData->MemoryThermalManagement;
        SbSetupData->ExttsViaTsOnBoard                   = SetupData->ExttsViaTsOnBoard;
        SbSetupData->ExttsViaTsOnDimm                    = SetupData->ExttsViaTsOnDimm;
        SbSetupData->SlpS0VmSupport                      = SetupData->SlpS0VmSupport;         
        SbSetupData->DvrmVmIoExpCapabilitySupport        = SetupData->DvrmVmIoExpCapabilitySupport;
        SbSetupData->WWANEnable                          = SetupData->WWANEnable;
        
        // Thunderbolt
        SbSetupData->TbtSupport                          = SetupData->TbtSupport;

        SbSetupData->AudioConnector                      = SetupData->AudioConnector;
    } // end if SetupData != NULL
    
    DEBUG ((EFI_D_ERROR, "PchSetupCallbacks ends.....\n"));
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
