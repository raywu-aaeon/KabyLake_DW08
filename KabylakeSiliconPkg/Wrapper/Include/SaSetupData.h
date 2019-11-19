//
// This Header file is included in NbSetupData.h, so please don't include it directly.
//
    UINT8   ProbelessTrace;
    UINT8   GdxcIotSize;
    UINT8   GdxcMotSize;
    UINT8   MemoryTrace;
    
    UINT8   ClttConfig;
    UINT8   Altitude;
    UINT8   Tcrit;
    UINT8   Thigh;
    UINT16  BwLimitTf;
    UINT8   WarmThreshold;
    UINT8   BwLimitWarm;
    UINT8   HotThreshold;
    UINT8   BwLimitHot;
    UINT8   DoubleRefreshControl;

    UINT8   ReadVref;
    UINT8   InternalReadVref;

    UINT16  NBDxeSubSystemVendorId;
    UINT16  NBDxeSubSystemId;       

    UINT8   IgdBootType;
    UINT8   DisplayPipeB;
    UINT8   LcdPanelType;
    UINT8   LcdPanelScaling;

    UINT8   EnableRenderStandby;
    UINT8   DeepRenderStandby;
    
    // Uncore Related
    UINT16  UncoreVoltageOffset;
    UINT8   UncoreVoltageOffsetPrefix;    
      
    // GT related
    UINT8   GtsMaxOcRatio;
    UINT8   GtsVoltageMode;
    UINT16  GtsVoltageOffset;
    UINT8   GtsVoltageOffsetPrefix;
    UINT16  GtsVoltageOverride;    
    UINT16  GtsExtraTurboVoltage; 
    UINT8   GtusMaxOcRatio;
    UINT8   GtusVoltageMode;
    UINT16  GtusVoltageOffset;
    UINT8   GtusVoltageOffsetPrefix;
    UINT16  GtusVoltageOverride;    
    UINT16  GtusExtraTurboVoltage;
    INT16   SaVoltageOffset;
    UINT8   GtVoltageMode;
    INT16   IoaVoltageOffset;
    INT16   IodVoltageOffset;
    UINT8   EdramRatio;    

    // LCD
    UINT8   IgdLcdBlc;
    UINT8   IgdLcdIBia;
    UINT8   IgdLcdSSCC;
    UINT8   IgdTV1Standard;
    UINT8   IgdTV2Standard;
    UINT8   AlsEnable;
    UINT8   LowPowerMode;
    UINT8   ActiveLFP;
    UINT8   LfpColorDepth;
    UINT8   DDISelection;
    UINT8   GTTSize;
    UINT8   ApertureSize;
    UINT8   CdClock;    
    UINT8   PanelPowerEnable;
//  GOP    
    UINT8   GopConfigEnable;

    // IUER
    UINT8   SlateIndicatorRT;
    UINT8   SlateIndicatorSx;
    UINT8   DockIndicatorRT;
    UINT8   DockIndicatorSx;
    UINT8   IuerButtonEnable;
    UINT8   IuerConvertibleEnable;
    UINT8   IuerDockEnable;
    
    // PEG
    UINT8   Peg0Enable;
    UINT8   Peg1Enable;
    UINT8   Peg2Enable;
    UINT8   Peg0MaxLinkSpeed;
    UINT8   Peg1MaxLinkSpeed;
    UINT8   Peg2MaxLinkSpeed;
    UINT8   Peg0MaxLinkWidth;
    UINT8   Peg1MaxLinkWidth;
    UINT8   Peg2MaxLinkWidth;
    UINT8   Peg0PowerDownUnusedLanes;
    UINT8   Peg1PowerDownUnusedLanes;
    UINT8   Peg2PowerDownUnusedLanes;
    UINT8   Peg0Gen3EqPh3Method;
    UINT8   Peg1Gen3EqPh3Method;
    UINT8   Peg2Gen3EqPh3Method;
    UINT8   Peg0Gen3EqPh2Enable;
    UINT8   Peg1Gen3EqPh2Enable;
    UINT8   Peg2Gen3EqPh2Enable;
    
    UINT8   PegAspm[3];  
    UINT8   PegAspmL0s[3];
    UINT8   PegDeEmphasis[3];
    UINT8   PegRootPortHPE[3];
    
    UINT8   PegSlotPowerLimitScale[3];
    UINT8   PegSlotPowerLimitValue[3];
    UINT16  PegPhysicalSlotNumber[3];    
    //
    // PEG Hotplug Resources
    //
    UINT8   PegExtraBusRsvd[3];
    UINT16  PegMemRsvd[3];
    UINT8   PegIoRsvd[3];
    UINT8   PegMaxPayload[3];
    
//    UINT8   AlwaysEnablePeg;
    UINT8   PegGenerateBdatMarginTable; 
    UINT8   PegRxCemTestingMode;
    UINT8   PegRxCemLoopbackLane;
    UINT8   PegRxCemNonProtocolAwareness; 
    UINT8   PegDisableSpreadSpectrumClocking;    
    UINT8   PegGen3ProgramStaticEq;
    UINT8   Gen3SwEqAlwaysAttempt;
    UINT8   Gen3SwEqNumberOfPresets;
    UINT16  Gen3SwEqJitterDwellTime;
    UINT16  Gen3SwEqJitterErrorTarget;
    UINT16  Gen3SwEqVocDwellTime;
    UINT16  Gen3SwEqVocErrorTarget;

    UINT8   PegGen3RootPortPreset[16];
    UINT8   PegGen3EndPointPreset[16];
    UINT8   PegGen3EndPointHint[16];
    UINT8   PegGen3RxCtlePeaking[8];
    UINT8   PegGen3RxCtleOverride;
    UINT8   InitAspmAfterOprom;
    UINT8   PegGen3AllowGpioReset;
    UINT8   Gen3SwEqEnableVocTest;
    UINT8   AllowPerstGpio;
    UINT8   AllowPerstGpioActive;

    UINT8   Peg0LtrEnable;
    UINT8   Peg1LtrEnable;
    UINT8   Peg2LtrEnable;
    UINT16  LtrMaxSnoopLatency[3];
    UINT16  LtrMaxNoSnoopLatency[3];
    UINT8   Peg0ObffEnable;
    UINT8   Peg1ObffEnable;
    UINT8   Peg2ObffEnable;

    UINT8   PowerDownUnusedBundles[3];
//    UINT8   C7Allowed; 
    // DVMT5.0 Graphic memory setting
    UINT8   IgdDvmt50PreAlloc;
    UINT8   IgdDvmt50TotalAlloc;

    // SA Device Control
    UINT8   SaDevice7;
    UINT8   SaDevice4;

    // VTD  
    UINT8   EnableVtd;
    UINT8   EnableAbove4GBMmio;
    UINT8   GmmEnable;
    UINT8   SaImguEnable;
    UINT8   EdramTestMode;
    UINT8   EdramTestModeJ0;
    UINT8   IedMemEnable;
    UINT8   X2ApicOptOut;
    //SSVID WorkAround
    UINT8   CridEnable;
    // DMI 
    UINT8   DmiVc1;
    UINT8   DmiVcm;
    UINT8   DmiMaxLinkSpeed;
    UINT8   DmiGen3EqPh2Enable;
    UINT8   DmiGen3EqPh3Method;
    UINT8   DmiGen3ProgramStaticEq;
    UINT8   DmiGen3RootPortPreset[4];
    UINT8   DmiGen3EndPointPreset[4];
    UINT8   DmiGen3EndPointHint[4];
    UINT8   DmiGen3RxCtlePeaking[2];
    UINT8   DmiAspm;
    UINT8   DmiExtSync;
    UINT8   DmiDeEmphasis;
    UINT8   DmiIot;
    // ASPM
    UINT8   PrimaryDisplay;
    UINT8   PcieCardSelect;
    UINT8   DetectNonComplaint;
    UINT16  DelayAfterPwrEn;
    UINT16  DelayAfterHoldReset;
    UINT8   PrimaryPeg;
    UINT8   PrimaryPcie;
    UINT8   InternalGraphics;
    // Graphics Turbo IMON Current
    UINT8   GfxTurboIMON;
#ifdef UP_SERVER_FLAG 
    UINT8   UserThresholdEnable;
    UINT8   UserBudgetEnable;
    UINT8   TsodTcritMax;
    UINT8   TsodEventMode;
    UINT8   TsodEventPolarity;
    UINT8   TsodCriticalEventOnly;
    UINT8   TsodEventOutputControl;
    UINT8   TsodAlarmwindowLockBit;
    UINT8   TsodCriticaltripLockBit;
    UINT8   TsodShutdownMode;
    UINT8   TsodThigMax;
    UINT8   TsodManualEnable;  
#endif
    UINT8   SkipExtGfxScan;
    UINT8   PmSupport;
    UINT8   PeiGraphicsPeimInit;    
    UINT8   PavpEnable;
    UINT8   CdynmaxClampEnable;    
    UINT8   GtFreqMax;    
    // MRC
    UINT8   HobBufferSize;
    UINT8   EccSupport;
    UINT8   MrcFastBoot;
    UINT8   MrcFastBootA0B0P0;
    UINT8   MrcTimeMeasure;
    UINT8   RemapEnable;
    UINT8   DisableDimmChannel0;
    UINT8   DisableDimmChannel1;
    UINT8   CAVrefCtlOffset;       ///< REVISION_11 - CA Vref Control Offset
    UINT8   Ch0VrefCtlOffset;      ///< REVISION_11 - Channel 0 DQ Vref Control Offset
    UINT8   Ch1VrefCtlOffset;      ///< REVISION_11 - Channel 1 DQ Vref Control Offset
    UINT8   Ch0ClkPiCodeOffset;    ///< REVISION_11 - Channel 0 Clk Pi Code Offset
    UINT8   Ch1ClkPiCodeOffset;    ///< REVISION_11 - Channel 1 Clk Pi Code Offset
    UINT8   Ch0RcvEnOffset;        ///< REVISION_11 - Channel 0 Receive Enable Offset
    UINT8   Ch0RxDqsOffset;        ///< REVISION_11 - Channel 0 Read DQS Offset
    UINT8   Ch0TxDqOffset;         ///< REVISION_11 - Channel 0 Write DQ Offset
    UINT8   Ch0TxDqsOffset;        ///< REVISION_11 - Channel 0 Write DQS Offset
    UINT8   Ch0VrefOffset;         ///< REVISION_11 - Channel 0 Vref Offset
    UINT8   Ch1RcvEnOffset;        ///< REVISION_11 - Channel 1 Receive Enable Offset
    UINT8   Ch1RxDqsOffset;        ///< REVISION_11 - Channel 1 Read DQS Offset
    UINT8   Ch1TxDqOffset;         ///< REVISION_11 - Channel 1 Write DQ Offset
    UINT8   Ch1TxDqsOffset;        ///< REVISION_11 - Channel 1 Write DQS Offset
    UINT8   Ch1VrefOffset;         ///< REVISION_11 - Channel 1 Vref Offset
    BOOLEAN AutoSelfRefreshSupport;///< REVISION_11 - FALSE = No auto self refresh support, TRUE = auto self refresh support.
    BOOLEAN ExtTemperatureSupport; ///< REVISION_11 - FALSE = No extended temperature support, TRUE = extended temperature support.
    UINT32  Ddr4MixedUDimm2DpcLimit;
    BOOLEAN CleanMemory;           ///< <b>FALSE</b> = Do not Clear Memory; TRUE - Clear Memory
    UINT8   MaxRttWr;              ///< REVISION_11 - Maximum DIMM RTT_WR to use in power training 0 = Off, 1 = 120 ohms
    UINT32  BclkFrequency;
    UINT8   MaxTolud;
    UINT8   SaGv;
    UINT16  FreqSaGvLow;
    UINT8   EpgEnable;
    UINT16  Idd3n;
    UINT16  Idd3p;    
    UINT8   DdrRefClk;
    UINT8   DdrRatio;
    UINT8   DdrOddRatioMode;
    UINT16  DdrFreqLimit;
    UINT8   MrcSafeConfig;
    UINT8   SpdProfileSelected;
    UINT8   ScramblerSupport;
    UINT8   RmtCrosserEnable;
    UINT16  MemoryVoltage;
//    UINT8   PowerDownMode0;
//    UINT8   PowerDownMode1;
    UINT8   PwdwnIdleCounter;
    UINT8   WeaklockEn;
    UINT8   EnCmdRate;    
    UINT8   CmdTriStateDis;
    BOOLEAN RhPrevention;
    UINT8   RhSolution;
    UINT8   RhActProbability;
    UINT8   ExitOnFailure;
    UINT8   EvLoader;
    UINT8   EvLoaderDelay;   
    UINT8   RetrainOnFastFail;
    
    //
    // TurnAround Timing
    //
    // Read-to-Read
    UINT8   tRd2RdSG;
    UINT8   tRd2RdDG;
    UINT8   tRd2RdDR;
    UINT8   tRd2RdDD;
    // Write-to-Read
    UINT8   tWr2RdSG;
    UINT8   tWr2RdDG;
    UINT8   tWr2RdDR;
    UINT8   tWr2RdDD;
    // Write-to-Write
    UINT8   tWr2WrSG;
    UINT8   tWr2WrDG;
    UINT8   tWr2WrDR;
    UINT8   tWr2WrDD;
    // Read-to-Write
    UINT8   tRd2WrSG;
    UINT8   tRd2WrDG;
    UINT8   tRd2WrDR;
    UINT8   tRd2WrDD;
    
    UINT8   McLock;
    UINT8   EnhancedInterleave;
    UINT8   RankInterleave;
    UINT8   PegGenx0;
    UINT8   PegGenx1;
    UINT8   PegGenx2;

    UINT8   MemoryThermalManagement;
    UINT8   ThermalThresholdWarm;
    UINT8   ThermalThresholdHot;
    UINT8   ThermalThresholdCritical;
    UINT8   PeciInjectedTemp;
    UINT8   ExttsViaTsOnBoard;
    UINT8   ExttsViaTsOnDimm;
    UINT8   VirtualTempSensor;
    UINT8   ChHashEnable;
    UINT16  ChHashMask;
    UINT8   ChHashInterleaveBit;
    UINT8   Vc1ReadMeter;
    UINT32  Vc1ReadMeterTimeWindow;
    UINT16  Vc1ReadMeterThreshold;
    UINT8   StrongWkLeaker;
    UINT8   ForceSingleRank;
    UINT8   LpddrMemWriteLatencySet;
    
    BOOLEAN DqPinsInterleaved;
    
    //
    //MRC Training Algorithms
    //
    UINT8   ECT;
    UINT8   SOT;
    UINT8   ERDMPRTC2D;
    UINT8   RDMPRT;
    UINT8   RCVET;
    UINT8   JWRL;
    UINT8   EWRTC2D;
    UINT8   ERDTC2D;
    UINT8   WRTC1D;
    UINT8   WRVC1D;
    UINT8   RDTC1D;
    UINT8   DIMMODTT;
    UINT8   WRDSEQT;
    UINT8   RDODTT;
    UINT8   RDEQT;
    UINT8   RDAPT;
    UINT8   WRTC2D;
    UINT8   RDTC2D;
    UINT8   CMDVC;
    UINT8   WRVC2D;
    UINT8   RDVC2D;
    UINT8   LCT;
    UINT8   RTL;
    UINT8   TAT;
    UINT8   RMT;
    UINT8   MEMTST;
    UINT8   DIMMRONT;
    UINT8   WRSRT;
    UINT8   ALIASCHK;
    UINT8   RCVENC1D;
    UINT8   RMC;
    UINT8   WRDSUDT;
    UINT8   CMDSR;
    UINT8   CMDDSEQ;
    UINT8   CMDNORM;    
    UINT8   EWRDSEQ;
    UINT8   TXDQVREF;

    //
    // Thermal Options
    //
    UINT8   EnableExtts;           ///< REVISION_10
    UINT8   EnableCltm;            ///< REVISION_10
    UINT8   EnableOltm;            ///< REVISION_10
    UINT8   Ddr4SkipRefreshEn;     ///< REVISION_10
    UINT8   EnablePwrDn;           ///< REVISION_10
    UINT8   Refresh2X;             ///< REVISION_10
    UINT8   DdrThermalSensor;      ///< REVISION_10
    UINT8   LockPTMregs;           ///< REVISION_10
    UINT8   UserPowerWeightsEn;    ///< REVISION_10
    UINT8   EnergyScaleFact;       ///< REVISION_10
    UINT8   RaplPwrFlCh1;          ///< REVISION_10
    UINT8   RaplPwrFlCh0;          ///< REVISION_10
    UINT8   RaplLim2Lock;          ///< REVISION_10
    UINT8   RaplLim2WindX;         ///< REVISION_10
    UINT8   RaplLim2WindY;         ///< REVISION_10
    UINT8   RaplLim2Ena;           ///< REVISION_10
    UINT16  RaplLim2Pwr;           ///< REVISION_10
    UINT8   RaplLim1WindX;         ///< REVISION_10
    UINT8   RaplLim1WindY;         ///< REVISION_10
    UINT8   RaplLim1Ena;           ///< REVISION_10
    UINT16  RaplLim1Pwr;           ///< REVISION_10
    UINT8   WarmThresholdCh0Dimm0; ///< REVISION_10
    UINT8   WarmThresholdCh0Dimm1; ///< REVISION_10
    UINT8   WarmThresholdCh1Dimm0; ///< REVISION_10
    UINT8   WarmThresholdCh1Dimm1; ///< REVISION_10
    UINT8   HotThresholdCh0Dimm0;  ///< REVISION_10
    UINT8   HotThresholdCh0Dimm1;  ///< REVISION_10
    UINT8   HotThresholdCh1Dimm0;  ///< REVISION_10
    UINT8   HotThresholdCh1Dimm1;  ///< REVISION_10
    UINT8   WarmBudgetCh0Dimm0;    ///< REVISION_10
    UINT8   WarmBudgetCh0Dimm1;    ///< REVISION_10
    UINT8   WarmBudgetCh1Dimm0;    ///< REVISION_10
    UINT8   WarmBudgetCh1Dimm1;    ///< REVISION_10
    UINT8   HotBudgetCh0Dimm0;     ///< REVISION_10
    UINT8   HotBudgetCh0Dimm1;     ///< REVISION_10
    UINT8   HotBudgetCh1Dimm0;     ///< REVISION_10
    UINT8   HotBudgetCh1Dimm1;     ///< REVISION_10
    UINT8   IdleEnergyCh0Dimm1;    ///< REVISION_10
    UINT8   IdleEnergyCh0Dimm0;    ///< REVISION_10
    UINT8   PdEnergyCh0Dimm1;      ///< REVISION_10
    UINT8   PdEnergyCh0Dimm0;      ///< REVISION_10
    UINT8   ActEnergyCh0Dimm1;     ///< REVISION_10
    UINT8   ActEnergyCh0Dimm0;     ///< REVISION_10
    UINT8   RdEnergyCh0Dimm1;      ///< REVISION_10
    UINT8   RdEnergyCh0Dimm0;      ///< REVISION_10
    UINT8   WrEnergyCh0Dimm1;      ///< REVISION_10
    UINT8   WrEnergyCh0Dimm0;      ///< REVISION_10
    UINT8   IdleEnergyCh1Dimm1;    ///< REVISION_10
    UINT8   IdleEnergyCh1Dimm0;    ///< REVISION_10
    UINT8   PdEnergyCh1Dimm1;      ///< REVISION_10
    UINT8   PdEnergyCh1Dimm0;      ///< REVISION_10
    UINT8   ActEnergyCh1Dimm1;     ///< REVISION_10
    UINT8   ActEnergyCh1Dimm0;     ///< REVISION_10
    UINT8   RdEnergyCh1Dimm1;      ///< REVISION_10
    UINT8   RdEnergyCh1Dimm0;      ///< REVISION_10
    UINT8   WrEnergyCh1Dimm1;      ///< REVISION_10
    UINT8   WrEnergyCh1Dimm0;      ///< REVISION_10
    UINT8   SrefCfgEna;            ///< REVISION_10
    UINT16  SrefCfgIdleTmr;        ///< REVISION_10
    UINT8   ThrtCkeMinDefeat;      ///< REVISION_10
    UINT8   ThrtCkeMinTmr;         ///< REVISION_10
    UINT8   ThrtCkeMinDefeatLpddr; ///< REVISION_10
    UINT8   ThrtCkeMinTmrLpddr;    ///< REVISION_10
    UINT8   EnablePwrDnLpddr;      ///< REVISION_10 - Enable Power Down for LPDDR
    UINT8   IsRunMemoryDown;
    UINT8   TbtSupport;
    UINT8   TbtHostTempLocationNB;
    UINT8   TbtHostTempLocationSB;
    UINT8   TBThostRouter;
    UINT8   tCL;
    UINT8   tCWL;
    UINT16  tFAW;
    UINT16  tRAS;
    UINT8   tRCDtRP;
    UINT16  tREFI;
    UINT16  tRFC;
    UINT8   tRRD;
    UINT8   tRTP;
    UINT8   tWR;
    UINT8   tWTR;
    UINT8   NModeSupport;
    UINT16  OverclockingSupport; // Overclocking module
    UINT8   LowPowerS0Idle;     // ULT PlatformConfig
    UINT8   EnableSaDevice;      // AcpiPlatform.sd
    UINT8   EnableDptf;         // AcpiPlatform.sd
    UINT8   BdatEnable;         // AcpiPlatform.sd
    UINT8   BdatTestType;
    UINT8   BdatSchema;
    UINT8   RMTLoopCount;
    UINT8   MemTestOnWarmBoot;
    UINT8   EnBER;
    UINT8   tRRD_L;
    UINT8   tRRD_S;
    UINT8   tWTR_L;
    UINT8   tWTR_S;
    UINT8   tRRDLDefault;
    UINT8   tRRDSDefault;
    UINT8   tWTRLDefault;
    UINT8   tWTRSDefault;
    //
    //  Please refer to SaSetup.h.
    //
    UINT8   XmpProfileEnable;
    UINT16  MemoryVoltageDefault; // not a SETUP item.
    UINT8   DllBwEn0;
    UINT8   DllBwEn1;
    UINT8   DllBwEn2;
    UINT8   DllBwEn3;
    
    UINT8   DdrRefClkDefault; // not a SETUP item.
    UINT8   DdrRatioDefault; // not a SETUP item.
    UINT8   tCLDefault;// not a SETUP item.
    UINT8   tCWLDefault;// not a SETUP item.
    UINT16  tFAWDefault;// not a SETUP item.
    UINT16  tRASDefault;// not a SETUP item.
    UINT8   tRCDtRPDefault;// not a SETUP item.
    UINT16  tREFIDefault;// not a SETUP item.
    UINT16  tRFCDefault;// not a SETUP item.
    UINT8   tRRDDefault;// not a SETUP item.
    UINT8   tRTPDefault;// not a SETUP item.
    UINT8   tWRDefault;// not a SETUP item.
    UINT8   tWTRDefault;// not a SETUP item.
    UINT8   NModeDefault;// not a SETUP item.

    UINT16  GtDid;                    // not a SETUP item, used by BIOS to pass GT SKU DID to SETUP items
    UINT8   BclkOverride; // not a SETUP item.
    UINT8   GdxcEnable;// not a SETUP item.
    UINT16  UncoreVoltageOffsetDefault;// not a SETUP item.
    UINT8   UncoreVoltageOffsetPrefixDefault;// not a SETUP item.
    UINT8   GtsMaxOcRatioDefault;// not a SETUP item.
    UINT8   GtsVoltageModeDefault;// not a SETUP item.
    UINT16  GtsVoltageOffsetDefault;// not a SETUP item.
    UINT8   GtsVoltageOffsetPrefixDefault;// not a SETUP item.
    UINT16  GtsVoltageOverrideDefault;// not a SETUP item.
    UINT16  GtsExtraTurboVoltageDefault;// not a SETUP item.
    UINT8   GtusMaxOcRatioDefault;// not a SETUP item.
    UINT8   GtusVoltageModeDefault;// not a SETUP item.
    UINT16  GtusVoltageOffsetDefault;// not a SETUP item.
    UINT8   GtusVoltageOffsetPrefixDefault;// not a SETUP item.
    UINT16  GtusVoltageOverrideDefault;// not a SETUP item.
    UINT16  GtusExtraTurboVoltageDefault;// not a SETUP item.
    UINT8   RealtimeMemoryTiming;    
    UINT8   BclkChangeWarmReset;
    UINT8   BclkChangeContinues;
//
// KblGBegin
//
    UINT16  DelayAfterOffMethod;
    UINT16  DelayAfterLinkEnable;
    UINT8   GenSpeedChangeEnable;

    UINT8   PegComplianceModeTest;
//
// KblGEnd
//