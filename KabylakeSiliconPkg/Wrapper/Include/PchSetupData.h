
    //
    // Miscellaneous options
    //
    UINT8   BiosGuard;

    UINT8   SensorHubType;
    
    UINT8   ExternalDma;
    UINT16  RootPortDid;              // not a SETUP item, used by BIOS to pass Root Port DID to SETUP items
    
    UINT8   DeepSxMode;
    UINT8   LanWakeFromDeepSx;
    UINT8   DisableDsxAcPresentPulldown;
    UINT8   DebugUsbUart;
    UINT8   PmcReadDisable;    
    UINT8   OsDebugPort;
    UINT8   PchLan;
    UINT8   PchDciEn;
    UINT8   PchDciAutoDetect;
    UINT8   PchWakeOnLan;
    UINT8   SlpLanLowDc;
    UINT8   PchLanK1Off;
    UINT8   PchWakeOnWlan;
    UINT8   PchWakeOnWlanDeepSx;
    UINT8   PchLpcClockRun;  
    UINT8   PchDisableEnergyReport;
    UINT8   EnableTcoTimer;
    UINT8   PchCapsuleResetType;    
    UINT8   PchCrid;
    UINT8   PchRtcLock;
    UINT8   PchBiosLock;
    
    UINT8   Hpet;
    UINT8   HpetBootTime;
    UINT8   StateAfterG3;
    UINT8   IchPort80Route;
    UINT8   EnhancePort8xhDecoding;    
    UINT8   PchSirqMode;
    UINT8   PchPort61hSmm;
    UINT8   PciePllSsc;
    UINT8   WdtEnable;    
    //
    // Usb Config
    //
    UINT8   PchUsb20[2];
    UINT8   PchUsbPortDisable;
    UINT8   PchUsbHsPort[PCH_H_XHCI_MAX_USB2_PORTS];
    UINT8   PchUsbSsPort[PCH_H_XHCI_MAX_USB3_PORTS];    
    UINT8   DisableComplianceMode;
    UINT8   UsbSensorHub;
    UINT8   UsbSsicEnable[PCH_XHCI_MAX_SSIC_PORT_COUNT];
    //
    // xDCI Config
    //
    UINT8   PchXdciSupport;
    
    //
    // Sata CONFIG
    //
    UINT8   PchSata;
    UINT8   SataInterfaceMode;
    
    UINT8   SataPort[PCH_SATA_MAX_PORTS];
    UINT8   SataHotPlug[PCH_SATA_MAX_PORTS];
    UINT8   SataMechanicalSw[PCH_SATA_MAX_PORTS];
    UINT8   SataSpinUp[PCH_SATA_MAX_PORTS];
    UINT8   SataExternal[PCH_SATA_MAX_PORTS];
    UINT8   SataType[PCH_SATA_MAX_PORTS];
    UINT8   SataTopology[PCH_SATA_MAX_PORTS];
    UINT8   RaidDeviceId;
    UINT8   SataRstOptaneMemory;
    UINT8   SataRstCpuAttachedStorage;
    UINT8   HybridStorageMode;
    UINT8   SataRaidR0;
    UINT8   SataRaidR1;
    UINT8   SataRaidR10;
    UINT8   SataRaidR5;
    UINT8   SataRaidIrrt;
    UINT8   SataRaidOub;
    UINT8   SataHddlk;
    UINT8   SataLedl;
    UINT8   SataRaidIooe;
    UINT8   SataRaidSrt;
    UINT8   SataRaidOromDelay;
    UINT8   SataRstForceForm;
    UINT8   SataAlternateId;
    UINT8   SataSalp;
    UINT8   SataControllerSpeed;    
    UINT8   SataTestMode;
#if 0
    UINT8   HddAcousticPowerManagement;
    UINT8   HddAcousticMode;
#endif
    
    //
    // SATA Device Sleep
    //
    UINT8   PxDevSlp[PCH_SATA_MAX_PORTS];
    UINT8   EnableDitoConfig[PCH_SATA_MAX_PORTS];
    UINT16  DitoVal[PCH_SATA_MAX_PORTS];
    UINT8   DmVal[PCH_SATA_MAX_PORTS];
    UINT8   SataLegacyOrom;
    
    //
    // PCI_EXPRESS_CONFIG
    //
    UINT8   PcieClockGatingDisabled;
    UINT8   PchLegacyIoLowLatency;
    UINT8   PcieRootPort8xhDecode;
    UINT8   Pcie8xhDecodePortIndex;
    UINT8   PcieRootPortPeerMemoryWriteEnable;
    UINT8   PcieComplianceTestMode;
    UINT8   PcieUsbGlitchWa;
    UINT8   RstPcieStorageRemap[PCH_MAX_RST_PCIE_STORAGE_CR];
    UINT8   RstPcieStorageRemapPort[PCH_MAX_RST_PCIE_STORAGE_CR];    
    UINT8   PcieRootPortEn[PCH_PCIE_MAX_ROOT_PORTS];
    UINT8   PcieRootPortAspm[PCH_PCIE_MAX_ROOT_PORTS]; 
    UINT8   PcieRootPortURE[PCH_PCIE_MAX_ROOT_PORTS];  
    UINT8   PcieRootPortFEE[PCH_PCIE_MAX_ROOT_PORTS];  
    UINT8   PcieRootPortNFE[PCH_PCIE_MAX_ROOT_PORTS];  
    UINT8   PcieRootPortCEE[PCH_PCIE_MAX_ROOT_PORTS];  
    UINT8   PcieRootPortCTD[PCH_PCIE_MAX_ROOT_PORTS];  
    UINT8   PcieRootPortPIE[PCH_PCIE_MAX_ROOT_PORTS];  
    UINT8   PcieRootPortSFE[PCH_PCIE_MAX_ROOT_PORTS];  
    UINT8   PcieRootPortSNE[PCH_PCIE_MAX_ROOT_PORTS];  
    UINT8   PcieRootPortSCE[PCH_PCIE_MAX_ROOT_PORTS];   
    UINT8   PcieRootPortPMCE[PCH_PCIE_MAX_ROOT_PORTS]; 
    UINT8   PcieRootPortHPE[PCH_PCIE_MAX_ROOT_PORTS];
    UINT8   PcieRootPortAER[PCH_PCIE_MAX_ROOT_PORTS];
    UINT8   PcieRootPortSpeed[PCH_PCIE_MAX_ROOT_PORTS];
    UINT8   PcieRootPortTHS[PCH_PCIE_MAX_ROOT_PORTS];
    UINT8   PcieRootPortL1SubStates[PCH_PCIE_MAX_ROOT_PORTS];
    UINT8   PcieRootPortACS[PCH_PCIE_MAX_ROOT_PORTS];
    UINT8   PcieRPDetectNonComplaint[PCH_PCIE_MAX_ROOT_PORTS];  // AMI Feature
    UINT8   PcieRootPortEqPh3Method[PCH_PCIE_MAX_ROOT_PORTS];
    UINT8   PcieRootPortUptp[PCH_PCIE_MAX_ROOT_PORTS];
    UINT8   PcieRootPortDptp[PCH_PCIE_MAX_ROOT_PORTS];
    UINT8   PcieLaneCm[PCH_PCIE_MAX_ROOT_PORTS];
    UINT8   PcieLaneCp[PCH_PCIE_MAX_ROOT_PORTS];
    UINT8   PcieTopology[PCH_PCIE_MAX_ROOT_PORTS];
    UINT16  PcieDetectTimeoutMs[PCH_PCIE_MAX_ROOT_PORTS];
    UINT8   RpFunctionSwap;

    UINT8   PcieSwEqOverride;
    UINT8   PcieSwEqCoeffCm[5];
    UINT8   PcieSwEqCoeffCp[5];    
    
    //
    // PCI Bridge Resources
    //
    UINT8   PcieExtraBusRsvd[PCH_PCIE_MAX_ROOT_PORTS];
    UINT16  PcieMemRsvd[PCH_PCIE_MAX_ROOT_PORTS];
    UINT8   PcieMemRsvdalig[PCH_PCIE_MAX_ROOT_PORTS];          // AMI Feature for Thunderbolt
    UINT16  PciePFMemRsvd[PCH_PCIE_MAX_ROOT_PORTS];
    UINT8   PciePFMemRsvdalig[PCH_PCIE_MAX_ROOT_PORTS];        // AMI Feature for Thunderbolt
    UINT8   PcieIoRsvd[PCH_PCIE_MAX_ROOT_PORTS];
    
    //
    // PCIe LTR Configuration
    //
    UINT8   PchPcieLtrEnable[PCH_PCIE_MAX_ROOT_PORTS];
    UINT8   PchPcieLtrConfigLock[PCH_PCIE_MAX_ROOT_PORTS];
    UINT8   PchPcieSnoopLatencyOverrideMode[PCH_PCIE_MAX_ROOT_PORTS];
    UINT8   PchPcieSnoopLatencyOverrideMultiplier[PCH_PCIE_MAX_ROOT_PORTS];
    UINT8   PchPcieNonSnoopLatencyOverrideMode[PCH_PCIE_MAX_ROOT_PORTS];
    UINT8   PchPcieNonSnoopLatencyOverrideMultiplier[PCH_PCIE_MAX_ROOT_PORTS];
    UINT16  PchPcieSnoopLatencyOverrideValue[PCH_PCIE_MAX_ROOT_PORTS];
    UINT16  PchPcieNonSnoopLatencyOverrideValue[PCH_PCIE_MAX_ROOT_PORTS];
    UINT8   PchPcieForceLtrOverride[PCH_PCIE_MAX_ROOT_PORTS];    

    //
    // PCIe CLKREQ# Configuration
    //
    UINT8   PchPcieClkReqOverride[PCH_PCIE_MAX_ROOT_PORTS];
    UINT8   PchPcieClkReqNumber[PCH_PCIE_MAX_ROOT_PORTS];
    
    
    //
    // DMI Configuration
    //
    UINT8   PchDmiAspm;
    
    //
    // Recovery Configuration
    //
    UINT8   IsRecoveryJumper;
    
    //
    // HD-Audio Configuration
    //
    UINT8   PchHdAudio;
    UINT8   PchHdAudioDsp;
    UINT8   PchHdAudioDspUaaCompliance;
    UINT8   PchHdaIDisplayCodecDisconnect;    
    UINT8   PchHdAudioPme;
    UINT8   PchHdAudioIoBufferOwnership;
    UINT8   PchHdAudioIoBufferVoltage;
    UINT8   PchHdaHdAudioLinkFreq;
    UINT8   PchHdaIDisplayLinkFreq;
    UINT8   PchHdaIDisplayLinkTmode;
    UINT8   PchHdAudioCodecSelect;
    UINT8   PchHdAudioI2sCodecSelect;
    UINT8   PchHdAudioNhltEndpointDmic;
    UINT8   PchHdAudioNhltEndpointBluetooth;
    UINT8   PchHdAudioNhltEndpointI2s;
    UINT8   PchHdAudioFeature[HDAUDIO_FEATURES];
    UINT8   PchHdAudioPostProcessingMod[HDAUDIO_PP_MODULES];
    CHAR16  PchHdAudioPostProcessingModCustomGuid1[GUID_CHARS_NUMBER];
    CHAR16  PchHdAudioPostProcessingModCustomGuid2[GUID_CHARS_NUMBER];
    CHAR16  PchHdAudioPostProcessingModCustomGuid3[GUID_CHARS_NUMBER];
    
    //
    // Interrupt Configuration
    //
    UINT8   PchIoApic24119Entries;
    
    //
    // PCH Thermal Throttling
    //
    UINT8   PchCrossThrottling;
    
    UINT8   PchSkyCamCio2Enable;
    
    //
    // SkyCam CIO2 configuration
    //
    UINT8   PchSkyCamPortATermOverride;
    UINT8   PchSkyCamPortBTermOverride;
    UINT8   PchSkyCamPortCTermOverride;
    UINT8   PchSkyCamPortDTermOverride;

    UINT8   PchSkyCamPortATrimEnable;
    UINT8   PchSkyCamPortBTrimEnable;
    UINT8   PchSkyCamPortCTrimEnable;
    UINT8   PchSkyCamPortDTrimEnable;

    UINT8   PchSkyCamPortACtleEnable;
    UINT8   PchSkyCamPortBCtleEnable;
    UINT8   PchSkyCamPortCDCtleEnable;

    UINT16  PchSkyCamPortADataTrimValue;
    UINT16  PchSkyCamPortBDataTrimValue;
    UINT16  PchSkyCamPortCDDataTrimValue;

    UINT8   PchSkyCamPortACtleCapValue;
    UINT8   PchSkyCamPortBCtleCapValue;
    UINT8   PchSkyCamPortCDCtleCapValue;

    UINT8   PchSkyCamPortACtleResValue;
    UINT8   PchSkyCamPortBCtleResValue;
    UINT8   PchSkyCamPortCDCtleResValue;

    UINT8   PchSkyCamPortAClkTrimValue;
    UINT8   PchSkyCamPortBClkTrimValue;
    UINT8   PchSkyCamPortCClkTrimValue;
    UINT8   PchSkyCamPortDClkTrimValue;    

    //
    // SCS Configuration
    //
    UINT8   PchScsEmmcEnabled;
    UINT8   PchScsEmmcHs400Enabled;
    UINT8   PchScsEmmcHs400DriverStrength;
    UINT8   PchScsSdCardEnabled;
    UINT8   PchScsSdCardSidebandEventsSupport;    
    
    //
    // Integrated Sensor Hub (ISH) configuration
    //
    UINT8   PchIshEnable;
    UINT8   PchIshSpiGpioAssign;
    UINT8   PchIshUart0GpioAssign;
    UINT8   PchIshUart1GpioAssign;
    UINT8   PchIshI2c0GpioAssign;
    UINT8   PchIshI2c1GpioAssign;
    UINT8   PchIshI2c2GpioAssign;
    UINT8   PchIshGp0GpioAssign;
    UINT8   PchIshGp1GpioAssign;
    UINT8   PchIshGp2GpioAssign;
    UINT8   PchIshGp3GpioAssign;
    UINT8   PchIshGp4GpioAssign;
    UINT8   PchIshGp5GpioAssign;
    UINT8   PchIshGp6GpioAssign;
    UINT8   PchIshGp7GpioAssign;
    UINT8   PchIshPdtUnlock;
    
    //
    // Serial IO configuration
    //
    UINT8   PchSerialIo;
    UINT8   PchSerialIoAcpiMode;
    UINT8   PchSerialIoDevice[11];
    UINT8   PchGpio;
    UINT8   PchGpioIrqRoute;
    UINT8   PchAdditionalSerialIoDevices;
    UINT16  PchGpioDebounce;
    UINT8   PchGpioTestDevices;
    UINT8   PchI2cVoltageSelect[6];
    UINT8   PchSpiCsPolaritySelect[2];
    UINT8   PchUartHwFlowCtrl[3];
    UINT8   PchI2c0SensorDevice;
    UINT8   PchI2c0SensorIrqMode;
    UINT8   PchI2c1SensorDevice;
    UINT8   PchI2c1SensorIrqMode;
    UINT8   PchI2c1TouchpanelBusAddress;
    UINT16  PchI2c1TouchpanelHidAddress;
    UINT8   PchI2c1TouchpanelSpeed;
    UINT8   PchI2c1WacompanelSpeed;
    UINT8   PchI2c2SensorDevice;
    UINT8   PchI2c3SensorDevice;
    UINT8   PchI2c4SensorDevice;
    UINT8   PchSpi1SensorDevice;
    UINT8   PchSpi1SensorIrqMode;
    UINT8   PchUart0SensorDevice;
    UINT8   PchUart0SensorIrqMode;
    UINT8   PchI2c5SensorDevice;
    UINT8   PchUart1SensorDevice;
    UINT8   PchUart2SensorDevice;
    UINT8   PchI2cWittDevice;
    UINT8   PchI2cWittVersion;
    UINT8   PchUartUtkDevice;
    UINT8   GnssConnection;
    UINT8   GnssModel;
    UINT8   PchFpsEnable;
    
    UINT8   PchSerialIoUseTimingParameters;
    UINT16  PchSerialIoTimingSSHI;
    UINT16  PchSerialIoTimingSSLI;
    UINT16  PchSerialIoTimingSSDI;
    UINT16  PchSerialIoTimingFMHI;
    UINT16  PchSerialIoTimingFMLI;
    UINT16  PchSerialIoTimingFMDI;
    UINT16  PchSerialIoTimingFPHI;
    UINT16  PchSerialIoTimingFPLI;
    UINT16  PchSerialIoTimingFPDI;
    UINT16  PchSerialIoTimingM0CI;
    UINT16  PchSerialIoTimingM1CI;
    UINT16  PchSerialIoTimingM0CS;
    UINT16  PchSerialIoTimingM1CS;
    UINT16  PchSerialIoTimingM0CU;
    UINT16  PchSerialIoTimingM1CU;
    
    UINT8   TouchPanelMuxSelector;
    UINT8   BtUartMuxSelector;
    
    //
    //TraceHub Setup Options
    //
    UINT8   TraceHubEnableMode;
    UINT8   MemRegion0BufferSize;
    UINT8   MemRegion1BufferSize;

    //
    // Thermal Throttling Level options
    //
    UINT8   PchTtLevelSuggestSet;
    UINT16  PchThrmT0Level;
    UINT16  PchThrmT1Level;
    UINT16  PchThrmT2Level;
    UINT8   PchThrmTtEnable;
    UINT8   PchThrmTtState13Enable;
    UINT8   PchThrmTtLock;

    //
    // DMI Thermal Throttling
    //
    UINT8   PchDmiTsSuggestSet;
    UINT8   PchTs0Width;
    UINT8   PchTs1Width;
    UINT8   PchTs2Width;
    UINT8   PchTs3Width;
    UINT8   PchDmiTsawEn;
    //
    // SATA Thermal Throttling
    //
    UINT8   PchSataTsSuggestSet;
    UINT8   PchP0T1M;
    UINT8   PchP0T2M;
    UINT8   PchP0T3M;
    UINT8   PchP0TDisp;
    UINT8   PchP0Tinact;
    UINT8   PchP0TDispFinit;
    UINT8   PchP1T1M;
    UINT8   PchP1T2M;
    UINT8   PchP1T3M;
    UINT8   PchP1TDisp;
    UINT8   PchP1Tinact;
    UINT8   PchP1TDispFinit;

    //
    // SMBUS
    //
    UINT8   SmbusSpdWriteDisable;
    //
    // P2SB
    //
    UINT8   PchP2sbUnlock;
    //
    // Flash Protection configuration
    //
    UINT8   FprrEnable;    
    
    //
    // Miscellaneous options
    //
    UINT8   RstPcieRemapEnabled[PCH_PCIE_MAX_ROOT_PORTS];

    // Option to Enable/Disable UCMC/UCSI device
    UINT8   PchUcsiUcmcDevice;

    // SB Thermal configuration in Advance page
    UINT8   AutoThermalReporting;
    UINT8   Ac1TripPoint;
    UINT8   Ac0TripPoint;
    UINT8   Ac1FanSpeed;
    UINT8   Ac0FanSpeed;
    UINT8   PassiveThermalTripPoint;
    UINT8   CriticalThermalTripPoint;
    UINT8   PassiveTc1Value;
    UINT8   PassiveTc2Value;
    UINT8   PassiveTspValue;

    UINT8   DisableActiveTripPoints;
    UINT8   DisablePassiveTripPoints;
    UINT8   DisableCriticalTripPoints;
   
    UINT8   ThermalDeviceEnable;
    UINT8   CPUTempReadEnable;
    UINT8   CPUEnergyReadEnable;    
    UINT8   PCHTempReadEnable;
    UINT8   AlertEnableLock;
    UINT8   PchAlert;
    UINT8   DimmAlert;
    UINT8   CpuTemp;
    UINT8   CpuFanSpeed;

//    UINT8   TPV_Restrict_Enable;
    UINT8   TenSecondPowerButtonEnable;
    
    UINT8   TrEnabled;
    
    // EC turbo control test mode 
    UINT8   ECTurboControlMode;
    UINT8   ACBrickCapacity;
    UINT8   ECPollingPeriod;
    UINT8   ECGuardBandValue;
    UINT8   ECAlgorithmSel;
    
    UINT8   MemoryThermalManagement;
    UINT8   ExttsViaTsOnBoard;   
    UINT8   ExttsViaTsOnDimm;
   
    UINT8   SlpS0VmSupport;    
    UINT8   DvrmVmIoExpCapabilitySupport;
    UINT8   WWANEnable;
	
    // Thunderbolt
    UINT8   TbtSupport;
    UINT8   TbtSelector1;
    UINT8   Multicontroller;

    UINT8   AudioConnector;
