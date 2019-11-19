//
// Definitions
//
#ifndef MAX_CUSTOM_P_STATES
#define MAX_CUSTOM_P_STATES 40
#endif // MAX_CUSTOM_P_STATES

#define MAX_IIO_PCI_EXPRESS_ROOT_PORTS    4

#define PCH_MAX_RST_PCIE_STORAGE_CR       3
#define PCH_PCIE_MAX_ROOT_PORTS           24 // For KBL-PCH-H:24, SKL-PCH-H:20, For SKL-PCH-LP:12
#define PCH_SATA_MAX_PORTS                8  // For SPTH:8, For SPTLP:3
#define PCH_H_XHCI_MAX_USB2_PORTS         16 // For SPTH:16, For SPTLP:12
#define PCH_H_XHCI_MAX_USB3_PORTS         10 // For SPTH:10, For SPTLP:6
#define PCH_XHCI_MAX_SSIC_PORT_COUNT      2
#define MAX_PCI_ROOT_BRIDGES              1
#define SATA_MODE_AHCI                    0
#define SATA_MODE_RAID                    1
#define SATA_TEST_MODE_ENABLE             1
#define NON_CS_DEVICES                    9
#define I2C0_SENSOR_DEVICES               7
#define I2C1_SENSOR_DEVICES               13
#define UART0_SENSOR_DEVICES              2
#define UART1_SENSOR_DEVICES              1
#define SERIAL_IO_I2C0                    0x0
#define SERIAL_IO_I2C1                    0x1
#define SERIAL_IO_I2C2                    0x2
#define SERIAL_IO_I2C3                    0x3
#define SERIAL_IO_I2C4                    0x4
#define SERIAL_IO_I2C5                    0x5
#define SERIAL_IO_SPI0                    0x6
#define SERIAL_IO_SPI1                    0x7
#define SERIAL_IO_UART0                   0x8
#define SERIAL_IO_UART1                   0x9
#define SERIAL_IO_UART2                   0xA
#define HDAUDIO_FEATURES                  9
#define HDAUDIO_PP_MODULES                32
#define GUID_CHARS_NUMBER                 37 // 36 GUID chars + null termination
#define SDCARD_SIDEBAND_CD_GPP_B17        1
#define SDCARD_SIDEBAND_CD_GPP_D10        2

#define EFI_CSM_CONTROL_ALWAYS_ON         0x0
#define EFI_CSM_CONTROL_ALWAYS_OFF        0x1
#define EFI_CSM_CONTROL_DETECT_LEGACY_VGA 0x2

#define OPROM_EFI                         1
#define OPROM_LEGACY                      0

#define SKYCAM_I2C_DEVICES_COUNT          12
#define SKYCAM_GPIO_COUNT                 4
#define SKYCAM_MODULE_NAME_LENGTH         16
#define SKYCAM_HID_LENGTH                 9

#ifndef SGX_SOFTWARE_CONTROL
#define SGX_SOFTWARE_CONTROL 2
#endif
#ifdef SETUP_DATA_DEFINITION
//
// Intel Setup items
//
  UINT8   StartOfSetupData;
// AMI_OVERRIDE_ACPI0009_START  
  UINT8   IrmtHwNotification;
  UINT8   IrmtState;
// AMI_OVERRIDE_ACPI0009_END   
  UINT8   PciExpNative;
  UINT8   NativeAspmEnable;
  UINT8   PtidSupport;
  // AcpiDebug Setup Options
  UINT8   AcpiDebug;
  UINT32  AcpiDebugAddress;
  UINT8   PciDelayOptimizationEcr;
  // S5 RTC wakeup setup options
  UINT8   WakeOnRTCS5;
  UINT8   RTCWakeupDate;
  UINT8   RTCWakeupTimeHour;
  UINT8   RTCWakeupTimeMinute;
  UINT8   RTCWakeupTimeSecond;
  UINT8   PeciAccessMethod;
  UINT8   LowPowerS0Idle;
  UINT8   PepCpu;
  UINT8   PepGfx;
  UINT8   PepSataContraints;
  UINT8   PepUart;
  UINT8   PepI2c0;
  UINT8   PepI2c1;
  UINT8   PepI2c2;
  UINT8   PepI2c3;
  UINT8   PepI2c4;
  UINT8   PepI2c5;
  UINT8   PepSpi;
  UINT8   PepXhci;
  UINT8   PepAudio;
  UINT8   PepEmmc;
  UINT8   PepSdxc;
  UINT8   CSNotifyEC;
  UINT8   CSDebugLightEC;
  UINT8   EcLowPowerMode;
  UINT8   EcDebugLed;
  UINT8   ECBaseCsPwrPolicy;
  UINT8   SensorStandby;
  UINT8   PL1LimitCS;
  UINT16  PL1LimitCSValue;
  UINT8   TenSecondPowerButtonEnable;
  UINT8   LpitResidencyCounter; // AdvancedFeaturesContent
  UINT8   IrmtConfiguration;
  UINT8   LoadSSDTFromFile;

  UINT8   CdClock;
  UINT8   SensorHubType;
  UINT8   DebugUsbUart;
  UINT8   UsbSensorHub;
  UINT8   FirmwareConfiguration;

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
  // SkyCam sensor configuration menu
  //
  UINT8   SkyCam_ControlLogic0;              // None/discrete/PMIC
  UINT8   SkyCam_ControlLogic1;              // None/discrete/PMIC
  UINT8   SkyCam_ControlLogic2;              // None/discrete/PMIC
  UINT8   SkyCam_ControlLogic3;              // None/discrete/PMIC

  UINT8   SkyCam_Link0;                      // Enabled / Disabled
  UINT8   SkyCam_Link1;                      // Enabled / Disabled
  UINT8   SkyCam_Link2;                      // Enabled / Disabled
  UINT8   SkyCam_Link3;                      // Enabled / Disabled

  UINT8   SkyCam_LanesClkDiv;

  UINT8   SkyCam_ControlLogic0_Type;
  UINT8   SkyCam_ControlLogic0_CrdVersion;
  UINT8   SkyCam_ControlLogic0_GpioPinsEnabled;
  UINT8   SkyCam_ControlLogic0_I2cChannel;
  UINT16  SkyCam_ControlLogic0_I2cAddress;
  UINT8   SkyCam_ControlLogic0_Pld;
  UINT8   SkyCam_ControlLogic0_GpioGroupPadNumber[SKYCAM_GPIO_COUNT];
  UINT8   SkyCam_ControlLogic0_GpioGroupNumber[SKYCAM_GPIO_COUNT];
  UINT8   SkyCam_ControlLogic0_GpioFunction[SKYCAM_GPIO_COUNT];
  UINT8   SkyCam_ControlLogic0_GpioActiveValue[SKYCAM_GPIO_COUNT];
  UINT8   SkyCam_ControlLogic0_GpioInitialValue[SKYCAM_GPIO_COUNT];

  UINT8   SkyCam_ControlLogic1_Type;
  UINT8   SkyCam_ControlLogic1_CrdVersion;
  UINT8   SkyCam_ControlLogic1_GpioPinsEnabled;
  UINT8   SkyCam_ControlLogic1_I2cChannel;
  UINT16  SkyCam_ControlLogic1_I2cAddress;
  UINT8   SkyCam_ControlLogic1_Pld;
  UINT8   SkyCam_ControlLogic1_GpioGroupPadNumber[SKYCAM_GPIO_COUNT];
  UINT8   SkyCam_ControlLogic1_GpioGroupNumber[SKYCAM_GPIO_COUNT];
  UINT8   SkyCam_ControlLogic1_GpioFunction[SKYCAM_GPIO_COUNT];
  UINT8   SkyCam_ControlLogic1_GpioActiveValue[SKYCAM_GPIO_COUNT];
  UINT8   SkyCam_ControlLogic1_GpioInitialValue[SKYCAM_GPIO_COUNT];

  UINT8   SkyCam_ControlLogic2_Type;
  UINT8   SkyCam_ControlLogic2_CrdVersion;
  UINT8   SkyCam_ControlLogic2_GpioPinsEnabled;
  UINT8   SkyCam_ControlLogic2_I2cChannel;
  UINT16  SkyCam_ControlLogic2_I2cAddress;
  UINT8   SkyCam_ControlLogic2_Pld;
  UINT8   SkyCam_ControlLogic2_GpioGroupPadNumber[SKYCAM_GPIO_COUNT];
  UINT8   SkyCam_ControlLogic2_GpioGroupNumber[SKYCAM_GPIO_COUNT];
  UINT8   SkyCam_ControlLogic2_GpioFunction[SKYCAM_GPIO_COUNT];
  UINT8   SkyCam_ControlLogic2_GpioActiveValue[SKYCAM_GPIO_COUNT];
  UINT8   SkyCam_ControlLogic2_GpioInitialValue[SKYCAM_GPIO_COUNT];

  UINT8   SkyCam_ControlLogic3_Type;
  UINT8   SkyCam_ControlLogic3_CrdVersion;
  UINT8   SkyCam_ControlLogic3_GpioPinsEnabled;
  UINT8   SkyCam_ControlLogic3_I2cChannel;
  UINT16  SkyCam_ControlLogic3_I2cAddress;
  UINT8   SkyCam_ControlLogic3_Pld;
  UINT8   SkyCam_ControlLogic3_GpioGroupPadNumber[SKYCAM_GPIO_COUNT];
  UINT8   SkyCam_ControlLogic3_GpioGroupNumber[SKYCAM_GPIO_COUNT];
  UINT8   SkyCam_ControlLogic3_GpioFunction[SKYCAM_GPIO_COUNT];
  UINT8   SkyCam_ControlLogic3_GpioActiveValue[SKYCAM_GPIO_COUNT];
  UINT8   SkyCam_ControlLogic3_GpioInitialValue[SKYCAM_GPIO_COUNT];

  UINT8   SkyCam_Link0_SensorModel;          // IMX135 / OV5693 / IMX179 / OV8858 / OV2740-IVCAM / IMX208 / OV9728 / IMX188 / User0
  UINT16  SkyCam_Link0_UserHid[SKYCAM_HID_LENGTH];
  UINT8   SkyCam_Link0_CameraPhysicalLocation;
  UINT16  SkyCam_Link0_ModuleName[SKYCAM_MODULE_NAME_LENGTH];
  UINT8   SkyCam_Link0_I2cDevicesEnabled;    // number of I2C devices defined for this link
  UINT8   SkyCam_Link0_I2cChannel;
  UINT16  SkyCam_Link0_I2cAddress[SKYCAM_I2C_DEVICES_COUNT];
  UINT8   SkyCam_Link0_I2cDeviceType[SKYCAM_I2C_DEVICES_COUNT];
  UINT8   SkyCam_Link0_DriverData_Version;
  UINT8   SkyCam_Link0_DriverData_CrdVersion;
  UINT8   SkyCam_Link0_DriverData_LaneUsed;
  UINT8   SkyCam_Link0_DriverData_EepromType;
  UINT8   SkyCam_Link0_DriverData_VcmType;
  UINT8   SkyCam_Link0_DriverData_CustomData[4];
  UINT32  SkyCam_Link0_DriverData_Mclk;
  UINT8   SkyCam_Link0_DriverData_ControlLogic;
  UINT8   SkyCam_Link0_DriverData_FlashSupport;
  UINT8   SkyCam_Link0_DriverData_Degree;

  UINT8   SkyCam_Link1_SensorModel;          // IMX135 / OV5693 / IMX179 / OV8858 / OV2740-IVCAM / IMX208 / OV9728 / IMX188 / User1
  UINT16  SkyCam_Link1_UserHid[SKYCAM_HID_LENGTH];
  UINT8   SkyCam_Link1_CameraPhysicalLocation;
  UINT16  SkyCam_Link1_ModuleName[SKYCAM_MODULE_NAME_LENGTH];
  UINT8   SkyCam_Link1_I2cDevicesEnabled;    // number of I2C devices defined for this link
  UINT8   SkyCam_Link1_I2cChannel;
  UINT16  SkyCam_Link1_I2cAddress[SKYCAM_I2C_DEVICES_COUNT];
  UINT8   SkyCam_Link1_I2cDeviceType[SKYCAM_I2C_DEVICES_COUNT];
  UINT8   SkyCam_Link1_DriverData_Version;
  UINT8   SkyCam_Link1_DriverData_CrdVersion;
  UINT8   SkyCam_Link1_DriverData_LaneUsed;
  UINT8   SkyCam_Link1_DriverData_EepromType;
  UINT8   SkyCam_Link1_DriverData_VcmType;
  UINT8   SkyCam_Link1_DriverData_CustomData[4];
  UINT32  SkyCam_Link1_DriverData_Mclk;
  UINT8   SkyCam_Link1_DriverData_ControlLogic;
  UINT8   SkyCam_Link1_DriverData_FlashSupport;
  UINT8   SkyCam_Link1_DriverData_Degree;

  UINT8   SkyCam_Link2_SensorModel;          // IMX135 / OV5693 / IMX179 / OV8858 / OV2740-IVCAM / IMX208 / OV9728 / IMX188 / User2
  UINT16  SkyCam_Link2_UserHid[SKYCAM_HID_LENGTH];
  UINT8   SkyCam_Link2_CameraPhysicalLocation;
  UINT16  SkyCam_Link2_ModuleName[SKYCAM_MODULE_NAME_LENGTH];
  UINT8   SkyCam_Link2_I2cDevicesEnabled;    // number of I2C devices defined for this link
  UINT8   SkyCam_Link2_I2cChannel;
  UINT16  SkyCam_Link2_I2cAddress[SKYCAM_I2C_DEVICES_COUNT];
  UINT8   SkyCam_Link2_I2cDeviceType[SKYCAM_I2C_DEVICES_COUNT];
  UINT8   SkyCam_Link2_DriverData_Version;
  UINT8   SkyCam_Link2_DriverData_CrdVersion;
  UINT8   SkyCam_Link2_DriverData_LaneUsed;
  UINT8   SkyCam_Link2_DriverData_EepromType;
  UINT8   SkyCam_Link2_DriverData_VcmType;
  UINT8   SkyCam_Link2_DriverData_CustomData[4];
  UINT32  SkyCam_Link2_DriverData_Mclk;
  UINT8   SkyCam_Link2_DriverData_ControlLogic;
  UINT8   SkyCam_Link2_DriverData_FlashSupport;
  UINT8   SkyCam_Link2_DriverData_Degree;

  UINT8   SkyCam_Link3_SensorModel;          // IMX135 / OV5693 / IMX179 / OV8858 / OV2740-IVCAM / IMX208 / OV9728 / IMX188 / User3
  UINT16  SkyCam_Link3_UserHid[SKYCAM_HID_LENGTH];
  UINT8   SkyCam_Link3_CameraPhysicalLocation;
  UINT16  SkyCam_Link3_ModuleName[SKYCAM_MODULE_NAME_LENGTH];
  UINT8   SkyCam_Link3_I2cDevicesEnabled;    // number of I2C devices defined for this link
  UINT8   SkyCam_Link3_I2cChannel;
  UINT16  SkyCam_Link3_I2cAddress[SKYCAM_I2C_DEVICES_COUNT];
  UINT8   SkyCam_Link3_I2cDeviceType[SKYCAM_I2C_DEVICES_COUNT];
  UINT8   SkyCam_Link3_DriverData_Version;
  UINT8   SkyCam_Link3_DriverData_CrdVersion;
  UINT8   SkyCam_Link3_DriverData_LaneUsed;
  UINT8   SkyCam_Link3_DriverData_EepromType;
  UINT8   SkyCam_Link3_DriverData_VcmType;
  UINT8   SkyCam_Link3_DriverData_CustomData[4];
  UINT32  SkyCam_Link3_DriverData_Mclk;
  UINT8   SkyCam_Link3_DriverData_ControlLogic;
  UINT8   SkyCam_Link3_DriverData_FlashSupport;
  UINT8   SkyCam_Link3_DriverData_Degree;

  UINT8   PchI2c0SensorDevice;
  UINT8   PchI2c0TouchpadBusAddress;
  UINT16  PchI2c0TouchpadHidAddress;
  UINT8   PchI2c0TouchpadSpeed;
  UINT8   PchI2c0SensorIrqMode;
  UINT8   PchI2c1SensorDevice;
  UINT8   PchI2c1SensorIrqMode; // AdvancedFeaturesContent
  UINT8   PchI2c1TouchpanelBusAddress; // AdvancedFeaturesContent
  UINT16  PchI2c1TouchpanelHidAddress; // AdvancedFeaturesContent
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
  UINT8   GnssModel;
  UINT8   GnssConnection;
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

  UINT8   Ps2KbMsEnable;
  UINT8   TouchPanelMuxSelector;
  UINT8   BtUartMuxSelector;
  UINT8   BtWirelessChargingMode;

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
  UINT8   PmicVccLevel; // AdvancedFeaturesContent
  UINT8   PmicVddqLevel; // AdvancedFeaturesContent
  UINT8   TwoComponents;

  UINT8   ThermalDeviceEnable;
  UINT8   CPUTempReadEnable;
  UINT8   CPUEnergyReadEnable;
  UINT8   PCHTempReadEnable;
  UINT8   AlertEnableLock;
  UINT8   PchAlert;
  UINT8   DimmAlert;
  UINT8   CpuTemp;
  UINT8   CpuFanSpeed;

  //
  // DPTF SETUP items begin
  //
  UINT8   EnableDptf;
  UINT8   EnableDCFG;

  UINT8   EnableSaDevice;
  UINT8   ActiveThermalTripPointSA;
  UINT8   PassiveThermalTripPointSA;
  UINT8   CriticalThermalTripPointSA;
  UINT8   CriticalThermalTripPointSaS3;
  UINT8   HotThermalTripPointSA;
  UINT8   ThermalSamplingPeriodSA;
  UINT32  PpccStepSize;
  UINT8   LPOEnable;
  UINT8   LPOStartPState;
  UINT8   LPOStepSize;
  UINT8   LPOPowerControlSetting;
  UINT8   LPOPerformanceControlSetting;
  UINT16  MinPowerLimit0;
  UINT16  MinPowerLimit1;
  UINT16  MinPowerLimit2;
  UINT8   EnableCtdpPolicy;
  UINT8   EnableLpmPolicy;
  UINT8   CurrentLowPowerMode;
  UINT8   EnableCurrentExecutionUnit;
  UINT16  TargetGfxFreq;

  UINT8   EnableMemoryDevice; ///< @deprecated
  UINT8   ActiveThermalTripPointTMEM; ///< @deprecated
  UINT8   PassiveThermalTripPointTMEM; ///< @deprecated
  UINT8   CriticalThermalTripPointTMEM; ///< @deprecated
  UINT8   CriticalThermalTripPointTmemS3; ///< @deprecated
  UINT8   HotThermalTripPointTMEM; ///< @deprecated
  UINT8   ThermalSamplingPeriodTMEM; ///< @deprecated

  UINT8   EnableFan1Device;
  UINT8   EnableFan2Device;

  UINT8   EnableDisplayParticipant;
  UINT8   DisplayDepthUpperLimit;
  UINT8   DisplayDepthLowerLimit;

  UINT8   EnableWifiParticipant; // Wireless Participant
  UINT8   ActiveThermalTripPointWifi;
  UINT8   PassiveThermalTripPointWifi;
  UINT8   CriticalThermalTripPointWifi;
  UINT8   CriticalThermalTripPointWifiS3;
  UINT8   HotThermalTripPointWifi;

  UINT8   EnableWWANParticipant;
  UINT8   ActiveThermalTripPointWWAN;
  UINT8   PassiveThermalTripPointWWAN;
  UINT8   CriticalThermalTripPointWWAN;
  UINT8   CriticalThermalTripPointWwanS3;
  UINT8   HotThermalTripPointWWAN;

  UINT8   EnableWGigParticipant; ///< @deprecated since revision 2.
  UINT8   ActiveThermalTripPointWGig; ///< @deprecated since revision 2.
  UINT8   PassiveThermalTripPointWGig; ///< @deprecated since revision 2.
  UINT8   CriticalThermalTripPointWGig; ///< @deprecated since revision 2.
  UINT8   CriticalThermalTripPointWGigS3; ///< @deprecated since revision 2.
  UINT8   HotThermalTripPointWGig; ///< @deprecated since revision 2.

  UINT8   EnableChargerParticipant;
  UINT8   EnablePowerParticipant;
  UINT16  PowerParticipantPollingRate;
  UINT8   EnablePowerBossPolicy;

  UINT8   EnablePerCParticipant;
  UINT8   ActiveThermalTripPointPerC;
  UINT8   PassiveThermalTripPointPerC;
  UINT8   CriticalThermalTripPointPerC;
  UINT8   CriticalThermalTripPointPerCS3;
  UINT8   HotThermalTripPointPerC;

  UINT8   EnableIvCamParticipant;
  UINT8   ActiveThermalTripPointIvCam;
  UINT8   PassiveThermalTripPointIvCam;
  UINT8   CriticalThermalTripPointIvCam;
  UINT8   CriticalThermalTripPointIvCamS3;
  UINT8   HotThermalTripPointIvCam;

  UINT8   EnableStorageParticipant;
  UINT8   ActiveThermalTripPointStrg;
  UINT8   PassiveThermalTripPointStrg;
  UINT8   CriticalThermalTripPointStrg;
  UINT8   CriticalThermalTripPointStrgS3;
  UINT8   HotThermalTripPointStrg;

  UINT8   EnableVS1Participant;
  UINT8   ActiveThermalTripPointVS1;
  UINT8   PassiveThermalTripPointVS1;
  UINT8   CriticalThermalTripPointVS1;
  UINT8   CriticalThermalTripPointVS1S3;
  UINT8   HotThermalTripPointVS1;

  UINT8   EnableVS2Participant;
  UINT8   ActiveThermalTripPointVS2;
  UINT8   PassiveThermalTripPointVS2;
  UINT8   CriticalThermalTripPointVS2;
  UINT8   CriticalThermalTripPointVS2S3;
  UINT8   HotThermalTripPointVS2;

  UINT8   EnableSen1Participant;
  UINT8   ActiveThermalTripPointSen1;
  UINT8   PassiveThermalTripPointSen1;
  UINT8   CriticalThermalTripPointSen1;
  UINT8   CriticalThermalTripPointSen1S3;
  UINT8   HotThermalTripPointSen1;
  UINT8   SensorSamplingPeriodSen1;

  UINT8   EnableSen2Participant;
  UINT8   ActiveThermalTripPointSen2;
  UINT8   PassiveThermalTripPointSen2;
  UINT8   CriticalThermalTripPointSen2;
  UINT8   CriticalThermalTripPointSen2S3;
  UINT8   HotThermalTripPointSen2;
  UINT8   SensorSamplingPeriodSen2;

  UINT8   EnableSen3Participant;
  UINT8   ActiveThermalTripPointSen3;
  UINT8   PassiveThermalTripPointSen3;
  UINT8   CriticalThermalTripPointSen3;
  UINT8   CriticalThermalTripPointSen3S3;
  UINT8   HotThermalTripPointSen3;
  UINT8   SensorSamplingPeriodSen3;

  UINT8   EnableSen4Participant;
  UINT8   ActiveThermalTripPointSen4;
  UINT8   PassiveThermalTripPointSen4;
  UINT8   CriticalThermalTripPointSen4;
  UINT8   CriticalThermalTripPointSen4S3;
  UINT8   HotThermalTripPointSen4;
  UINT8   SensorSamplingPeriodSen4;

  UINT8   EnableSen5Participant;
  UINT8   ActiveThermalTripPointSen5;
  UINT8   PassiveThermalTripPointSen5;
  UINT8   CriticalThermalTripPointSen5;
  UINT8   CriticalThermalTripPointSen5S3;
  UINT8   HotThermalTripPointSen5;
  UINT8   SensorSamplingPeriodSen5;

  UINT8   EnableSen6Participant;
  UINT8   ActiveThermalTripPointSen6;
  UINT8   PassiveThermalTripPointSen6;
  UINT8   CriticalThermalTripPointSen6;
  UINT8   CriticalThermalTripPointSen6S3;
  UINT8   HotThermalTripPointSen6;
  UINT8   SensorSamplingPeriodSen6;

  UINT8   EnableSen7Participant;
  UINT8   ActiveThermalTripPointSen7;
  UINT8   PassiveThermalTripPointSen7;
  UINT8   CriticalThermalTripPointSen7;
  UINT8   CriticalThermalTripPointSen7S3;
  UINT8   HotThermalTripPointSen7;
  UINT8   SensorSamplingPeriodSen7;

  UINT8   EnableSen8Participant;
  UINT8   ActiveThermalTripPointSen8;
  UINT8   PassiveThermalTripPointSen8;
  UINT8   CriticalThermalTripPointSen8;
  UINT8   CriticalThermalTripPointSen8S3;
  UINT8   HotThermalTripPointSen8;
  UINT8   SensorSamplingPeriodSen8;

  UINT8   EnableGen1Participant;
  UINT8   ActiveThermalTripPointGen1;
  UINT8   PassiveThermalTripPointGen1;
  UINT8   CriticalThermalTripPointGen1;
  UINT8   CriticalThermalTripPointGen1S3;
  UINT8   HotThermalTripPointGen1;
  UINT8   ThermistorSamplingPeriodGen1;

  UINT8   EnableGen2Participant;
  UINT8   ActiveThermalTripPointGen2;
  UINT8   PassiveThermalTripPointGen2;
  UINT8   CriticalThermalTripPointGen2;
  UINT8   CriticalThermalTripPointGen2S3;
  UINT8   HotThermalTripPointGen2;
  UINT8   ThermistorSamplingPeriodGen2;

  UINT8   EnableGen3Participant;
  UINT8   ActiveThermalTripPointGen3;
  UINT8   PassiveThermalTripPointGen3;
  UINT8   CriticalThermalTripPointGen3;
  UINT8   CriticalThermalTripPointGen3S3;
  UINT8   HotThermalTripPointGen3;
  UINT8   ThermistorSamplingPeriodGen3;

  UINT8   EnableGen4Participant;
  UINT8   ActiveThermalTripPointGen4;
  UINT8   PassiveThermalTripPointGen4;
  UINT8   CriticalThermalTripPointGen4;
  UINT8   CriticalThermalTripPointGen4S3;
  UINT8   HotThermalTripPointGen4;
  UINT8   ThermistorSamplingPeriodGen4;

  UINT8   EnableGen5Participant;
  UINT8   ActiveThermalTripPointGen5;
  UINT8   PassiveThermalTripPointGen5;
  UINT8   CriticalThermalTripPointGen5;
  UINT8   CriticalThermalTripPointGen5S3;
  UINT8   HotThermalTripPointGen5;
  UINT8   ThermistorSamplingPeriodGen5;
  UINT8   EnableDGFCParticipant;
  UINT8   ActiveThermalTripPointDGFC;
  UINT8   PassiveThermalTripPointDGFC;
  UINT8   CriticalThermalTripPointDGFC;
  UINT8   CriticalThermalTripPointDGFCS3;
  UINT8   HotThermalTripPointDGFC;
  UINT8   SensorSamplingPeriodDGFC;
  UINT32  PpccStepSizeDGFC;
  UINT16  MinPowerLimit0DGFC;

  UINT8   EnableDGHMParticipant;
  UINT8   ActiveThermalTripPointDGHM;
  UINT8   PassiveThermalTripPointDGHM;
  UINT8   CriticalThermalTripPointDGHM;
  UINT8   CriticalThermalTripPointDGHMS3;
  UINT8   HotThermalTripPointDGHM;
  UINT8   SensorSamplingPeriodDGHM;

  UINT8   EnableMCPParticipant;
  UINT32  PpccStepSizeMCP;
  UINT32  MinPowerLimit0MCP;

  UINT8   EnableGen6Participant;
  UINT8   ActiveThermalTripPointGen6;
  UINT8   PassiveThermalTripPointGen6;
  UINT8   CriticalThermalTripPointGen6;
  UINT8   CriticalThermalTripPointGen6S3;
  UINT8   HotThermalTripPointGen6;
  UINT8   ThermistorSamplingPeriodGen6;

  UINT8   EnableGen7Participant;
  UINT8   ActiveThermalTripPointGen7;
  UINT8   PassiveThermalTripPointGen7;
  UINT8   CriticalThermalTripPointGen7;
  UINT8   CriticalThermalTripPointGen7S3;
  UINT8   HotThermalTripPointGen7;
  UINT8   ThermistorSamplingPeriodGen7;

  UINT8   EnableGen8Participant;
  UINT8   ActiveThermalTripPointGen8;
  UINT8   PassiveThermalTripPointGen8;
  UINT8   CriticalThermalTripPointGen8;
  UINT8   CriticalThermalTripPointGen8S3;
  UINT8   HotThermalTripPointGen8;
  UINT8   ThermistorSamplingPeriodGen8;

  UINT8   EnableActivePolicy;
  UINT8   EnablePassivePolicy;
  UINT8   TrtRevision;
  UINT8   EnableCriticalPolicy;
  UINT8   EnableCoolingModePolicy;
  UINT8   EnableACTPolicy;
  UINT8   EnableHDCPolicy;
  UINT8   EnableAPPolicy;
  UINT8   EnableVSPolicy;

  UINT8   PpccObject;
  UINT8   PdrtObject;
  UINT8   ArtgObject;
  UINT8   PmaxObject;
  UINT8   Tmp1Object;
  UINT8   Tmp2Object;
  UINT8   Tmp3Object;
  UINT8   Tmp4Object;
  UINT8   Tmp5Object;
  UINT8   Tmp6Object;
  UINT8   Tmp7Object;
  UINT8   Tmp8Object;
  UINT8   OptionalObjects;

  UINT8   OemDesignVariable0;
  UINT8   OemDesignVariable1;
  UINT8   OemDesignVariable2;
  UINT8   OemDesignVariable3;
  UINT8   OemDesignVariable4;
  UINT8   OemDesignVariable5;

  //
  // DPTF SETUP items end
  //


  UINT8   Rtd3Support;
  UINT8   Rtd3P0dl;
  UINT8   Rtd3P3dl;
  UINT8   Rtd3UsbPt1;
  UINT8   Rtd3UsbPt2;
  UINT8   Rtd3I2C0SensorHub;
  UINT16  Rtd3AudioDelay;
  UINT16  Rtd3TouchPadDelay;
  UINT16  Rtd3TouchPanelDelay;
  UINT16  Rtd3SensorHub;
  UINT16  VRStaggeringDelay;
  UINT16  VRRampUpDelay;
  UINT8   PstateCapping;

  UINT8   Rtd3ZPODD;
  UINT8   Rtd3Camera;
  UINT8   Rtd3WWAN;
  UINT8   Rtd3SataPort0;
  UINT8   Rtd3SataPort1;
  UINT8   Rtd3SataPort2;
  UINT8   Rtd3SataPort3;
  UINT8   Rtd3SataPort4;
  UINT8   Rtd3WaGpio;
  UINT8   Rtd3RemapCR1;
  UINT8   Rtd3RemapCR2;
  UINT8   Rtd3RemapCR3;
  UINT16  Rtd3I2C0ControllerPS0Delay;
  UINT16  Rtd3I2C1ControllerPS0Delay;

  UINT8   ConsolidatedPR;

  UINT8   Rtd3PcieNvme;
  UINT8   Rtd3I2CTouchPanel;

  UINT8   PowerSharingManagerEnable;
  UINT8   PsmSplcDomainType1;
  UINT32  PsmSplcPowerLimit1;
  UINT32  PsmSplcTimeWindow1;
  UINT8   PsmSplcDomainType2;
  UINT32  PsmSplcPowerLimit2;
  UINT32  PsmSplcTimeWindow2;

  UINT8   PsmDplcDomainType1;
  UINT8   PsmDplcDomainPreference1;
  UINT16  PsmDplcPowerLimitIndex1;
  UINT16  PsmDplcDefaultPowerLimit1;
  UINT32  PsmDplcDefaultTimeWindow1;
  UINT16  PsmDplcMinimumPowerLimit1;
  UINT16  PsmDplcMaximumPowerLimit1;
  UINT16  PsmDplcMaximumTimeWindow1;

  UINT8   PsmDplcDomainType2;
  UINT8   PsmDplcDomainPreference2;
  UINT16  PsmDplcPowerLimitIndex2;
  UINT16  PsmDplcDefaultPowerLimit2;
  UINT32  PsmDplcDefaultTimeWindow2;
  UINT16  PsmDplcMinimumPowerLimit2;
  UINT16  PsmDplcMaximumPowerLimit2;
  UINT16  PsmDplcMaximumTimeWindow2;

  UINT8   WifiEnable;
  UINT8   WifiDomainType1;
  UINT16  WifiPowerLimit1;
  UINT32  WifiTimeWindow1;
  UINT8   WifiDomainType2;
  UINT16  WifiPowerLimit2;
  UINT32  WifiTimeWindow2;
  UINT8   WifiDomainType3;
  UINT16  WifiPowerLimit3;
  UINT32  WifiTimeWindow3;
  UINT8   TRxDelay0;
  UINT8   TRxCableLength0;
  UINT8   TRxDelay1;
  UINT8   TRxCableLength1;
  UINT8   WrddDomainType1;
  UINT16  WrddCountryIndentifier1;
  UINT8   WrddDomainType2;
  UINT16  WrddCountryIndentifier2;
  UINT8   WrdsWiFiSarEnable;
  UINT8   WrdsWiFiSarTxPowerLimit1;
  UINT8   WrdsWiFiSarTxPowerLimit2;
  UINT8   WrdsWiFiSarTxPowerLimit3;
  UINT8   WrdsWiFiSarTxPowerLimit4;
  UINT8   WrdsWiFiSarTxPowerLimit5;
  UINT8   WrdsWiFiSarTxPowerLimit6;
  UINT8   WrdsWiFiSarTxPowerLimit7;
  UINT8   WrdsWiFiSarTxPowerLimit8;
  UINT8   WrdsWiFiSarTxPowerLimit9;
  UINT8   WrdsWiFiSarTxPowerLimit10;

  UINT8   CameraType;
  UINT8   IvcamDfuSupport;
  UINT8   IvcamWakeSupport;
  UINT16  IvcamPowerOnDelay;
  UINT16  IvcamPowerOffDelay;
  UINT8   IvcamRotation;
  UINT16  DsPowerOnDelay;
  UINT16  DsPowerOffDelay;
  UINT8   DsRotation;
  UINT8   AudioConnector;
  UINT8   WirelessCharging;
  UINT8   UsbFnEnable;

  UINT8   EnableEInkDfu;
// AMI_OVERRIDE_START
//  UINT8   CsmControl;  // Control by CSM module.
// AMI_OVERRIDE_END
  UINT8   FanControl;
  UINT8   CpuFanControl;
  UINT8   LowestFanSpeed;

  //
  // Thunderbolt(TM)
  //
  UINT8   TbtSupport;
  UINT8   TbtWakeupSupport;
  UINT8   ReserveBusForDGfx;
  UINT16  ReserveMemoryForDGfx;
  UINT16  ReservePMemoryForDGfx;
  UINT8   TbtPcieSupport;
//AMI_OVERRIDE_START - EIP322154 - Remove Thunderbolt DGPU related setup items
  UINT8   TbtXConnectSupport;      ///< 1=Supported; 0=Not Supported
//AMI_OVERRIDE_END - EIP322154 - Remove Thunderbolt DGPU related setup items
  UINT8   TbtUsbOn;
  UINT8   TbtVtdBaseSecurity;
  UINT8   TbtBootOn;
  UINT8   AicArSupport;
//AMI_OVERRIDE_START - EIP322170 - Follow AR BWG to implement SPR support
  UINT8   SprSupport;
//AMI_OVERRIDE_END - EIP322170 - Follow AR BWG to implement SPR support
  UINT8   TbtCacheLineSize;
  UINT8   Gpio3ForcePwr;
  UINT16  Gpio3ForcePwrDly;
  UINT8   SecurityMode;
  UINT8   CallSmiBeforeBoot;
  UINT32  TbtDgpuPciBaseAddressMem; // Thunderbolt External GPU PCI Configuration Space Buffer Address
  UINT32  TbtDebugBaseAddressMem;   // Thunderbolt debug area address
  UINT32  TbtDeviceTreeBuffer;      // Thunderbolt Device Tree Info Buffer Address
  UINT8   CardSelected;
  UINT16  ReserveMemoryPerSlot;
  UINT16  ReservePMemoryPerSlot;
  UINT8   ReserveIoPerSlot;
  UINT8   AssignIo;
  UINT8   MoreIOForPorts;
  UINT8   AssignPMem;
  UINT8   TbtNativeOsHotPlug;
  UINT8   TBTHotSMI;
  UINT8   Gpio5Filter;
  UINT8   TBTHotNotify;
  UINT8   EnableMsiInFadt;
  UINT8   TbtSelector;
  UINT8   TBTSetClkReq;
  UINT8   TbtAspm;
  UINT8   TbtLtr;
  UINT8   TBThostRouter;
// AMI_OVERRIDE_START - Setting SetupData->TbtSelector by AMI setup option.
  UINT8   TbtHostLocationGroup;
  UINT8   TbtHostTempLocationNB;
  UINT8   TbtHostTempLocationSB;
// AMI_OVERRIDE_END - Setting SetupData->TbtSelector by AMI setup option.
  UINT8   TbtPcieExtraBusRsvd;
  UINT16  TbtPcieMemRsvd;
  UINT8   TbtPcieIoRsvd;
  UINT8   TbtPcieMemAddrRngMax;
  UINT16  TbtPciePMemRsvd;
  UINT8   TbtPciePMemAddrRngMax;
  UINT8   TbtSkipPciEnumeration;
  UINT8   TbtSkipPciOprom;
  UINT8   TbtSkipIntLine;
  UINT8   TbtAcpiRemovalSupport;
//AMI_OVERRIDE_START - EIP296758 Implement Intel document 563778, version 2
  UINT8   TbtWaSwitch;
  UINT8   TbtWaPrewakeEnable;
  UINT32  TbtWaXhciActiveLtr;
  UINT32  TbtWaXhciHighLtr;
  UINT32  TbtWaXhciMediumLtr;
  UINT32  TbtWaXhciLowLtr;
//AMI_OVERRIDE_END - EIP296758 Implement Intel document 563778, version 2
//AMI_OVERRIDE_START >>> EIP353107 : Boot On command has the fail rate
  UINT16  TbtBeforeBootOnCmdDelay;
  UINT16  TbtBeforeUsbOnCmdDelay;
  UINT16  TbtBeforeSxExitCmdDelay;
//AMI_OVERRIDE_END <<< EIP353107 : Boot On command has the fail rate

  UINT8   IccWdtEnabled;
  UINT8   IccLockRegisters;
  UINT8   IccProfile;
  UINT8   ShowProfile;
  UINT8   NumProfiles;
  UINT8   IccPllShutdownEnabled;

  UINT8   CsModeFirst;
  UINT8   CsMode;
  UINT8   CsModeChanged;

  UINT8   PlatformUnstable;
  UINT8   IsOcDefaultsInitalized;

  UINT8   SecureEraseModeRealMode;
  UINT8   ForceSecureErase;

  UINT8   HidEventFilterDriverEnable;

  UINT8   Ssic7160and7260InitSetup;

  UINT8   TestMctpBroadcastCycle;
  UINT8   SlpS0VmSupport;
  UINT8   UsbTypeCAicSupport;
  UINT32  HebcValue;

  UINT8   I2cSarResetDelay;

  UINT8   WWANEnable;

  UINT8   UsbTypecBssbMode;

  UINT8   PepWiGigF1;

  UINT8   AlternateModeSynchDelay;

  UINT8   WigigRfe;
  UINT8   WiGigRfeCh1;
  UINT8   WiGigRfeCh2;
  UINT8   WiGigRfeCh3;
  UINT8   WiGigRfeCh4;
  UINT32  AwvClassIndex;

  UINT8   Rtd3RaidVolumes;
  UINT8   ThermalSamplingPeriodWifi;
  UINT8   WakeOnWiGigSupport;
  UINT8   EnablePIDPolicy;
  UINT8   EnablePowerSharePolicy;
  UINT8   PepGbe;
  UINT8   LpitRs2Wa;

  UINT8   WgdsWiFiSarDeltaGroup1PowerMax1;
  UINT8   WgdsWiFiSarDeltaGroup1PowerChainA1;
  UINT8   WgdsWiFiSarDeltaGroup1PowerChainB1;
  UINT8   WgdsWiFiSarDeltaGroup1PowerMax2;
  UINT8   WgdsWiFiSarDeltaGroup1PowerChainA2;
  UINT8   WgdsWiFiSarDeltaGroup1PowerChainB2;
  UINT8   WgdsWiFiSarDeltaGroup2PowerMax1;
  UINT8   WgdsWiFiSarDeltaGroup2PowerChainA1;
  UINT8   WgdsWiFiSarDeltaGroup2PowerChainB1;
  UINT8   WgdsWiFiSarDeltaGroup2PowerMax2;
  UINT8   WgdsWiFiSarDeltaGroup2PowerChainA2;
  UINT8   WgdsWiFiSarDeltaGroup2PowerChainB2;
  UINT8   WgdsWiFiSarDeltaGroup3PowerMax1;
  UINT8   WgdsWiFiSarDeltaGroup3PowerChainA1;
  UINT8   WgdsWiFiSarDeltaGroup3PowerChainB1;
  UINT8   WgdsWiFiSarDeltaGroup3PowerMax2;
  UINT8   WgdsWiFiSarDeltaGroup3PowerChainA2;
  UINT8   WgdsWiFiSarDeltaGroup3PowerChainB2;

  UINT8   BluetoothSar;
  UINT8   BluetoothSarBr;
  UINT8   BluetoothSarEdr2;
  UINT8   BluetoothSarEdr3;
  UINT8   BluetoothSarLe;
  UINT8   BluetoothSarLe2Mhz;
  UINT8   BluetoothSarLeLr;

  UINT8   PepTbt;
  UINT8   PowerMeterEnable;

  UINT8   SystemTimeAndAlarmSource; 
  UINT8   PepCio2;  
  UINT8   SerialPortAcpiDebug;
  UINT8   DvrmVmIoExpCapabilitySupport;

  UINT8   SkyCamControlLogic0Wled1FlashMaxCurrent;
  UINT8   SkyCamControlLogic0Wled1TorchMaxCurrent;
  UINT8   SkyCamControlLogic0Wled2FlashMaxCurrent;
  UINT8   SkyCamControlLogic0Wled2TorchMaxCurrent;
  UINT8   SkyCamControlLogic0Wled1Type;
  UINT8   SkyCamControlLogic0Wled2Type; 
  UINT8   SkyCamControlLogic1Wled1FlashMaxCurrent;
  UINT8   SkyCamControlLogic1Wled1TorchMaxCurrent;
  UINT8   SkyCamControlLogic1Wled2FlashMaxCurrent;
  UINT8   SkyCamControlLogic1Wled2TorchMaxCurrent;
  UINT8   SkyCamControlLogic1Wled1Type;
  UINT8   SkyCamControlLogic1Wled2Type; 
  UINT8   SkyCamControlLogic2Wled1FlashMaxCurrent;
  UINT8   SkyCamControlLogic2Wled1TorchMaxCurrent;
  UINT8   SkyCamControlLogic2Wled2FlashMaxCurrent;
  UINT8   SkyCamControlLogic2Wled2TorchMaxCurrent;
  UINT8   SkyCamControlLogic2Wled1Type;
  UINT8   SkyCamControlLogic2Wled2Type;
  UINT8   SkyCamControlLogic3Wled1FlashMaxCurrent;
  UINT8   SkyCamControlLogic3Wled1TorchMaxCurrent;
  UINT8   SkyCamControlLogic3Wled2FlashMaxCurrent;
  UINT8   SkyCamControlLogic3Wled2TorchMaxCurrent;
  UINT8   SkyCamControlLogic3Wled1Type;
  UINT8   SkyCamControlLogic3Wled2Type;
  UINT8   SkyCamLink0DriverDataPrivacyLed;
  UINT8   SkyCamLink0DriverDataPmicPosition;
  UINT8   SkyCamLink0DriverDataVoltageRail;
  UINT8   SkyCamLink1DriverDataPrivacyLed;
  UINT8   SkyCamLink1DriverDataPmicPosition;
  UINT8   SkyCamLink1DriverDataVoltageRail;
  UINT8   SkyCamLink2DriverDataPrivacyLed;
  UINT8   SkyCamLink2DriverDataPmicPosition;
  UINT8   SkyCamLink2DriverDataVoltageRail;
  UINT8   SkyCamLink3DriverDataPrivacyLed;
  UINT8   SkyCamLink3DriverDataPmicPosition;
  UINT8   SkyCamLink3DriverDataVoltageRail;
// AMI_OVERRIDE_START - Removed due to Intel doesn't verify it yet.
#if 0
  UINT8   TbtSelector1;
  UINT8   Multicontroller;  
#endif
// AMI_OVERRIDE_END - Removed due to Intel doesn't verify it yet.
  UINT8   PepIsp;
  UINT8   Win10Support;    // TbtWin10Support
  UINT8   Gpio3ForcePwr1;  
  UINT8   TrOsup;          // Titan Ridge Osup command

  UINT8   PepStorageControllers;
  UINT8   PepSataPort0;
  UINT8   PepSataPort1;
  UINT8   PepSataPort2;
  UINT8   PepSataPort3;
  UINT8   PepSataPort4;
  UINT8   PepSataPort5;
  UINT8   PepSataNvm1;
  UINT8   PepSataNvm2;
  UINT8   PepSataNvm3;
  UINT8   PepSataRaidVol0;
  UINT32  WwanModemResetDelay;
  UINT32  WwanPerstDelay;
  UINT32  AcpiWwanPWMGDelay;
  UINT32  AcpiWwanPWRGDelay;
  UINT32  WwanPrePowerOnDelay;
  UINT32  WwanPowerOnDelay;
  UINT32  WwanModemAssertResetDelay;  
  UINT8   UcsiRetryEcEnable;
  UINT16  Rtd3TbtOffDelay;
  UINT8   PepCsme;
  UINT8   WwanInterface;
  UINT8   Rtd3Tbt;
  UINT8   Rtd3TbtClkReq;
  UINT16  Rtd3TbtClkReqDelay;
  UINT8   KblRx4SlotGpioPowerPinEnable;
  UINT8   Rtd3WwanDsm;
  UINT8   EndOfSetupData;

//
// CPU Setup items
//
  UINT8   StartOfCpu;
// AMI_OVERRIDE_START  
  UINT8   XDBit;
  UINT8   LimitCpuid;
  UINT8	  CpuSetBootRatio;
// AMI_OVERRIDE_END  
  UINT8   CpuRatio;
  UINT8   CpuDefaultRatio;
  UINT8   CpuRatioOverride;
  UINT8   Peci;
  UINT8   HyperThreading;
  UINT8   ActiveCoreCount;
  UINT8   BistOnReset;
  UINT8   JtagC10PowerGateDisable;
  UINT8   EnableGv;
  UINT8   RaceToHalt;
  UINT8   EnableHwp;

  UINT8   BootFrequency;
  UINT8   EnableCx;
  UINT8   EnableC1e;
  UINT8   TurboMode;
  UINT32  PowerLimit1;
  UINT8   LongDurationPwrLimitOverride;
  UINT8   PowerLimit1Time;
  UINT8   PowerLimit2;
  UINT32  PowerLimit2Power;
  UINT8   PowerLimit3Override;
  UINT32  PowerLimit3;
  UINT8   PowerLimit3Time;
  UINT8   PowerLimit3DutyCycle;
  UINT8   PowerLimit3Lock;
  UINT8   PowerLimit4Override;
  UINT32  PowerLimit4;
  UINT8   PowerLimit4Lock;
  UINT8   TurboPowerLimitLock;
  UINT8   PlatformPowerLimit1Enable;
  UINT32  PlatformPowerLimit1Power;
  UINT8   PlatformPowerLimit1Time;
  UINT8   PlatformPowerLimit2Enable;
  UINT32  PlatformPowerLimit2Power;
  UINT8   EnergyEfficientPState;
  UINT8   CStatePreWake;
  UINT8   CStateAutoDemotion;
  UINT8   CStateUnDemotion;
  UINT8   PkgCStateDemotion;
  UINT8   PkgCStateUnDemotion;
  UINT8   EnableThermalMonitor;
  UINT8   PmgCstCfgCtrlLock;
  UINT8   ConfigTdpLevel;
  UINT8   ConfigTdpLock;
  UINT8   ConfigTdpBios;
  UINT8   TimedMwait;
  UINT8   PmgCstCfgCtrIoMwaitRedirection;
  UINT8   InterruptRedirectMode;

  UINT8   HdcControl;
  UINT8   PkgCStateLimit;
  UINT8   CstateLatencyControl0TimeUnit;
  UINT8   CstateLatencyControl1TimeUnit;
  UINT8   CstateLatencyControl2TimeUnit;
  UINT8   CstateLatencyControl3TimeUnit;
  UINT8   CstateLatencyControl4TimeUnit;
  UINT8   CstateLatencyControl5TimeUnit;
  UINT16  CstateLatencyControl0Irtl;
  UINT16  CstateLatencyControl1Irtl;
  UINT16  CstateLatencyControl2Irtl;
  UINT16  CstateLatencyControl3Irtl;
  UINT16  CstateLatencyControl4Irtl;
  UINT16  CstateLatencyControl5Irtl;

  UINT32  Custom1PowerLimit1Power;
  UINT32  Custom1PowerLimit2Power;
  UINT8   Custom1PowerLimit1Time;
  UINT8   Custom1TurboActivationRatio;
  UINT32  Custom2PowerLimit1Power;
  UINT32  Custom2PowerLimit2Power;
  UINT8   Custom2PowerLimit1Time;
  UINT8   Custom2TurboActivationRatio;
  UINT32  Custom3PowerLimit1Power;
  UINT32  Custom3PowerLimit2Power;
  UINT8   Custom3PowerLimit1Time;
  UINT8   Custom3TurboActivationRatio;
  UINT8   TStatesEnable;
  UINT8   EnableProcHot;
  UINT8   DisableProcHotOut;
  UINT8   DisableVrThermalAlert;
  UINT8   ProcHotLock;
  UINT8   ProcHotResponse;
  UINT8   TCCActivationOffset;
  UINT8   NumOfCustomPStates;
  UINT16  StateRatio[MAX_CUSTOM_P_STATES];
  UINT8   VT;
  UINT8   AesEnable;        // AMI
  UINT8   MachineCheck;
  UINT8   MonitorMwait;
  UINT16  DprSize;
  UINT8   HwPrefetcher;
  UINT8   AclPrefetch;
  UINT8   Txt;
  UINT8   LtDprProgramming;
  UINT8   ResetAux;
  UINT8   AcheckRequest;
  UINT8   MsegOpt;
  UINT8   MsegSize;

  UINT8   EcTurboControlMode;
  UINT8   AcBrickCapacity;
  UINT8   EcPollingPeriod;
  UINT8   EcGuardBandValue;
  UINT8   EcAlgorithmSel;

  UINT8   IsTurboRatioDefaultsInitalized;
  UINT8   FlexRatioOverrideDefault;
  UINT8   RatioLimit1Default;
  UINT8   RatioLimit2Default;
  UINT8   RatioLimit3Default;
  UINT8   RatioLimit4Default;
  UINT8   RatioLimit1;
  UINT8   RatioLimit2;
  UINT8   RatioLimit3;
  UINT8   RatioLimit4;
  UINT8   OverclockingLockDefault;
  UINT8   OverclockingLock;
#if !defined (BiosGuard_SUPPORT) || (BiosGuard_SUPPORT == 0)
  UINT8   BiosGuard;
#endif  
  UINT8   DebugInterfaceEnable;
  UINT8   DebugInterfaceLockEnable;
  UINT8   DebugDciEnable; ///< @deprecated
  UINT8   FlashWearOutProtection;

  UINT8   EnableC6Dram;  ///< @deprecated since revision 2: This feature is not supported. BIOS is required to disable.
  UINT8   EnableSgx;
  UINT8   EpochUpdate;
  UINT8   ShowEpoch;
  UINT32  PrmrrSize;
  UINT64  SgxEpoch0;
  UINT64  SgxEpoch1;
  UINT8   ApIdleManner;
  UINT8   ApHandoffManner;

  UINT8   ProcTraceOutputScheme;
  UINT8   ProcTraceMemSize;
  UINT8   ProcTraceEnable;

  UINT8   SkipStopPbet;

  UINT8   EpocFclkFreq;

  //
  // SA = 0, IA = 1, GTUS = 2, GTS = 3
  //
  UINT8   PsysSlope;
  UINT8   PsysOffset;
  UINT16  PsysPmax;
  UINT8   VrConfigEnable[4];
  UINT16  AcLoadline[4];
  UINT16  DcLoadline[4];
  UINT16  Psi1Threshold[4];
  UINT16  Psi2Threshold[4];
  UINT16  Psi3Threshold[4];
  UINT8   Psi3Enable[4];
  UINT8   Psi4Enable[4];
  UINT8   ImonSlope[4];
  UINT16  ImonOffset[4];
  UINT8   ImonOffsetPrefix[4];
  UINT16  IccMax[4];
  UINT16  VrVoltageLimit[4];
  UINT16  TdcPowerLimit[4];
  UINT8   TdcEnable[4];
  UINT8   TdcTimeWindow[4];
  UINT8   TdcLock[4];

  UINT32  TccOffsetTimeWindow;
  UINT8   TccOffsetClamp;
  UINT8   TccOffsetLock;

  //
  // CPU related
  //
  UINT8   FlexOverrideEnable;
  UINT8   MaxEfficiencyRatio;
  UINT8   MaxNonTurboRatio;
  UINT8   FlexRatioOverride;
  UINT8   EnableGvDefault;
  UINT8   TurboModeDefault;
  UINT8   PowerLimit1TimeDefault;
  UINT32  PowerLimit1Default;
  UINT32  PowerLimit2Default;
  UINT16  XeTdcLimit;
  UINT16  XeTdcLimitDefault;
  UINT16  XeTdpLimitDefault;
  UINT16  XeTdcLimitHangBootValue;
  UINT16  XeTdpLimitHangBootValue;
  UINT8   XeRatioLimit1HangBootValue;
  UINT8   XeRatioLimit2HangBootValue;
  UINT8   XeRatioLimit3HangBootValue;
  UINT8   XeRatioLimit4HangBootValue;

  UINT8   BusSpeedEnable;
  UINT8   ProcessorBusSpeedOverride;
  UINT8   ProcessorBusSpeedOverrideDefault;

  UINT8   ImonScaling;
  UINT8   PciePll;
  UINT8   EnergyEfficientTurbo;
  //
  // Saving MCFrequency to be used in PEI phase during Overclocking
  //
  UINT16  PrevBootMcFrequency;

  UINT8   WDTSupportforNextOSBoot;
  UINT16  TimeforNextOSBoot;
  UINT8   Ck505Present;

  UINT8   EnableDigitalThermalSensor;

  //
  // OverClocking setup options
  //
  UINT8   OverclockingSupport;
  UINT8   XtuInterfaceEnable;

  UINT8   CoreMaxOcRatio;
  UINT8   CoreVoltageMode;
  UINT16  CoreVoltageOverride;
  UINT16  CoreVoltageOffset;
  UINT8   CoreVoltageOffsetPrefix;
  UINT16  CoreExtraTurboVoltage;
  UINT8   Avx2RatioOffset;
  UINT8   RingMaxOcRatio;
  UINT8   VboostEnable;
  
  //
  // OverClocking Default setup options
  //
  UINT8   CoreMaxOcRatioDefault;
  UINT8   CoreVoltageModeDefault;
  UINT16  CoreVoltageOverrideDefault;
  UINT16  CoreVoltageOffsetDefault;
  UINT8   CoreVoltageOffsetPrefixDefault;
  UINT16  CoreExtraTurboVoltageDefault;
  UINT8   Avx2RatioOffsetDefault;
  UINT8   RingMaxOcRatioDefault;
  UINT8   EnableRsr;
  UINT8   VoltageOptimization;

  UINT8   RingMinOcRatio;
  UINT8   RingMinOcRatioDefault;
  UINT8   AutoThermalReporting;
  UINT16  CorePllVoltageOffset;
  
  //
  // Acoustic Noise Mitigation
  //
  UINT8   AcousticNoiseMitigation;
  UINT8   FastPkgCRampDisableIa;
  UINT8   FastPkgCRampDisableGt;
  UINT8   FastPkgCRampDisableSa;
  UINT8   SlowSlewRateForIa;
  UINT8   SlowSlewRateForGt;
  UINT8   SlowSlewRateForSa;

  UINT8   BclkAdaptiveVoltageEnable;
  UINT8   VrMbxCmd;
  UINT8   PeciC10Reset;    // Enable or Disable Peci C10 Reset command
  UINT8   PeciSxReset;
  UINT16  IccMaxDefault[4];

  //
  //  Coffeelake Overclocking features
  //
  UINT8   Avx3RatioOffset;
  UINT8   RingDownBin;
  UINT8   Avx3RatioOffsetDefault;
  UINT8   GtPllVoltageOffset;
  UINT8   RingPllVoltageOffset;
  UINT8   SaPllVoltageOffset;
  UINT8   McPllVoltageOffset;
  UINT8   TvbRatioClipping;
  UINT8   TvbVoltageOptimization;
  UINT8   VrPowerDeliveryDesign;
  UINT8   TjMaxOffset;

  UINT8   SafModeDisableBiosGuard;

//
// Support for up to 8 cores.
//

  UINT8   RatioLimit5Default;
  UINT8   RatioLimit6Default;
  UINT8   RatioLimit7Default;
  UINT8   RatioLimit8Default;
  UINT8   RatioLimit5;
  UINT8   RatioLimit6;
  UINT8   RatioLimit7;
  UINT8   RatioLimit8;  
  UINT8   ThreeStrikeCounterDisable;
  
  //
  // Platform Voltage Override setup options
  //
  UINT8   VccCoreOverrideEnable;
  UINT16  VccCoreOverrideVoltage;

  UINT8   VccStOverrideEnable;
  UINT16  VccStOverrideVoltage;
  UINT8   VccStOffsetPrefix;

  UINT8   VccSaOverrideEnable;
  UINT16  VccSaOverrideVoltage;

  UINT8   VccSfrOcOverrideEnable;
  UINT16  VccSfrOcOverrideVoltage;
  UINT8   VccSfrOcOffsetPrefix;

  UINT8   VccSfrOverrideEnable;
  UINT16  VccSfrOverrideVoltage;
  UINT8   VccSfrOffsetPrefix;

  UINT8   VccIoOverrideEnable;
  UINT16  VccIoOverrideVoltage;
  
  UINT8   IslVrCmd;
  UINT8   CoreVrDcLLOverrideEnable;
  UINT8   CoreDcLoadline;
  UINT8   EndOfCpu;
  
//
// ME Setup items
//
  UINT8   StartOfMe;
// AMI_OVERRIDE_START
  UINT8   TpmDeviceSelection;
// AMI_OVERRIDE_END
  UINT8   MeFirmwareInfo;
  UINT8   MeImageType;
  UINT8   MeFwDowngrade;
  UINT8   NfcDevice;
  UINT8   NfcAvailable;
  UINT8   CommsHubEnable;

  UINT8   HeciTimeouts;
  UINT8   DidInitStat;
  UINT8   DisableCpuReplacedPolling;
  UINT8   SendDidMsg;
  UINT8   DisableHeciRetry;
  UINT8   DisableMessageCheck;
  UINT8   SkipMbpHob;
  UINT8   HeciCommunication2;
  UINT8   KtDeviceEnable;
  UINT8   IderDeviceEnable;
  UINT8   DisableD0I3SettingForHeci;

  UINT8   ChangeEps;
  UINT8   UnconfigOnRtcAvailable;
  UINT8   CoreBiosDoneEnabled;
  UINT8   EndOfPostMessage;
  UINT8   Heci3Enabled;
  UINT8   MeJhiSupport;

  //
  // Intel AMT
  //
  UINT8   Amt;
  UINT8   AmtbxHotKeyPressed;
  UINT8   AmtbxSelectionScreen;
  UINT8   HideUnConfigureMeConfirm;
  UINT8   MebxDebugMsg;
  UINT8   UnConfigureMe;
  UINT8   AmtCiraRequest;
  UINT8   AmtCiraTimeout;
  UINT8   UsbProvision;
  UINT8   FwProgress;

  //
  // ASF
  //
  UINT8   Asf;

  //
  // WatchDog
  //
  UINT8   WatchDog;
  UINT16  WatchDogTimerOs;
  UINT16  WatchDogTimerBios;

  //
  // MEBx resolution settings
  //
  UINT8   MebxNonUiTextMode;
  UINT8   MebxUiTextMode;
  UINT8   MebxGraphicsMode;

  UINT8   OsPtpAware;
  UINT8   MeUnconfigOnRtcClear;
  UINT8   EndOfMe;

//
// SA Setup items
//
  UINT8   StartOfSa;
//AMI_OVERRIDE_START       
  UINT8   PwdwnIdleCounter; 
  UINT8   WeaklockEn;
//AMI_OVERRIDE_END  
  UINT8   BdatEnable;
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
  UINT8   EnCmdRate;
  UINT8   EdramRatio;     // @deprecated
  //
  // Memory related
  //
  UINT8   XmpProfileEnable;
  UINT16  MemoryVoltage;
  UINT16  MemoryVoltageDefault;
  UINT8   DllBwEn0;
  UINT8   DllBwEn1;
  UINT8   DllBwEn2;
  UINT8   DllBwEn3;
  //
  // Memory timing override (these options are used for Custom/User Profile)
  //
  UINT8   DdrRefClkDefault;
  UINT8   DdrRefClk;
  UINT8   DdrRatioDefault;
  UINT8   DdrRatio;
  UINT8   DdrOddRatioMode;
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
  //
  // Default values to restore memory timings to in case of Platform instability
  //
  UINT8   tCLDefault;
  UINT8   tCWLDefault;
  UINT16  tFAWDefault;
  UINT16  tRASDefault;
  UINT8   tRCDtRPDefault;
  UINT16  tREFIDefault;
  UINT16  tRFCDefault;
  UINT8   tRRDDefault;
  UINT8   tRTPDefault;
  UINT8   tWRDefault;
  UINT8   tWTRDefault;
  UINT8   NModeDefault;

  UINT16  GtDid;                    // not a SETUP item, used by BIOS to pass GT SKU DID to SETUP items
  UINT8   IgdBootType;
  UINT8   DisplayPipeB;
  UINT8   LcdPanelType;

  UINT8   LcdPanelScaling;

  UINT8   EnableRenderStandby;
  UINT8   IgdLcdBlc;
  UINT8   IgdLcdIBia; ///< @deprecated since revision 3.
  UINT8   IgdLcdSSCC; ///< @deprecated since revision 3.
  UINT8   AlsEnable;
  UINT8   IgdHdcpEnable;
  UINT8   HdcpAlgorithm;
  UINT8   LowPowerMode;
  UINT8   ActiveLFP;
  UINT8   LfpColorDepth;
  UINT8   GTTSize;
  UINT8   ApertureSize;
  UINT8   PanelPowerEnable;
  UINT8   PmSupport;
  UINT8   PeiGraphicsPeimInit;
  UINT8   PavpEnable;
  UINT8   GopConfigEnable;
  UINT8   VbiosBrightness;
  UINT8   CdynmaxClampEnable;
  UINT8   GtFreqMax;

  //
  // ICC Related
  //
  UINT8   BclkOverride;
  UINT32  BclkFrequency;

  //
  // PEG
  //
  UINT8   PegAspm[3];
  UINT8   PegAspmL0s[3];
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
  UINT8   PegRxCemTestingMode;
  UINT8   PegRxCemLoopbackLane;
  UINT8   PegRxCemNonProtocolAwareness; 
  UINT8   PegDisableSpreadSpectrumClocking;
  UINT8   Peg0Gen3EqPh3Method;
  UINT8   Peg1Gen3EqPh3Method;
  UINT8   Peg2Gen3EqPh3Method;
  UINT8   Peg0Gen3EqPh2Enable;
  UINT8   Peg1Gen3EqPh2Enable;
  UINT8   Peg2Gen3EqPh2Enable;
  UINT8   PegDeEmphasis[3];
  UINT8   PegMaxPayload[3];
  UINT8   Peg0LtrEnable;
  UINT8   Peg1LtrEnable;
  UINT8   Peg2LtrEnable;
  UINT8   Peg0ObffEnable;
  UINT8   Peg1ObffEnable;
  UINT8   Peg2ObffEnable;

  UINT8   PegGen3ProgramStaticEq;
  UINT8   PegGen3RootPortPreset[16];
  UINT8   PegGen3EndPointPreset[16];
  UINT8   PegGen3EndPointHint[16];
  UINT8   PegGen3RxCtlePeaking[8];
  UINT8   PegGen3RxCtleOverride;
  UINT16  Gen3SwEqJitterDwellTime;
  UINT8   Gen3SwEqAlwaysAttempt;
  UINT8   InitAspmAfterOprom;
  UINT8   PegGen3AllowGpioReset;
  UINT16  Gen3SwEqJitterErrorTarget;
  UINT8   PegGenerateBdatMarginTable;
  UINT8   Gen3SwEqEnableVocTest;
  UINT16  Gen3SwEqVocDwellTime;
  UINT16  Gen3SwEqVocErrorTarget;
  UINT8   Gen3SwEqNumberOfPresets;
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

  UINT8   EpgEnable;
  UINT16  Idd3n;
  UINT16  Idd3p;

  //
  // DVMT5.0 Graphic memory setting
  //
  UINT8   IgdDvmt50PreAlloc;
  UINT8   IgdDvmt50TotalAlloc;
  //
  // SA Device Control
  //
  UINT8   SaDevice7;
  UINT8   SaDevice4;
  UINT8   EnableVtd;
  UINT8   EnableAbove4GBMmio;
  UINT8   GmmEnable;
  UINT8   SaImguEnable;
  UINT8   EdramTestMode;
  UINT8   IedMemEnable;
  //
  // VT-d
  //
  UINT8   X2ApicOptOut;

  //
  // DMI
  //
  UINT8   DmiVc1;
  UINT8   DmiVcm;
  UINT8   DmiMaxLinkSpeed;
  UINT8   DmiIot;
  UINT8   DmiDeEmphasis;
  UINT8   DmiGen3EqPh2Enable;
  UINT8   DmiGen3EqPh3Method;
  UINT8   DmiGen3ProgramStaticEq;
  UINT8   DmiGen3RootPortPreset[4];
  UINT8   DmiGen3EndPointPreset[4];
  UINT8   DmiGen3EndPointHint[4];
  UINT8   DmiGen3RxCtlePeaking[2];
  // ASPM
  UINT8   DmiAspm;
  UINT8   DmiExtSync;
  // Stop Grant
  UINT8   AutoNsg;
  UINT8   NumStopGrant;
  // Primary Display Select
  UINT8   PrimaryDisplay;
  UINT8   PcieCardSelect;
  UINT16  DelayAfterPwrEn;
  UINT16  DelayAfterOffMethod;
  UINT16  DelayAfterLinkEnable;
  UINT8   GenSpeedChangeEnable;
  UINT8   PegComplianceModeTest;
  UINT16  DelayAfterHoldReset;
  UINT8   InternalGraphics;
  // Graphics Turbo IMON Current
  UINT8   GfxTurboIMON;

  //
  // MRC settings
  //
  UINT8   HobBufferSize;
  UINT8   EccSupport;
  UINT8   MaxTolud;
  UINT16  DdrFreqLimit;
  UINT8   SpdProfileSelected;
  UINT8   MrcTimeMeasure;
  UINT8   MrcFastBoot;
  UINT8   ChHashEnable;
  UINT16  ChHashMask;
  UINT8   ChHashInterleaveBit;
  UINT8   Vc1ReadMeter;
  UINT32  Vc1ReadMeterTimeWindow;
  UINT16  Vc1ReadMeterThreshold;
  UINT8   StrongWkLeaker;
  UINT8   ForceSingleRank;
  UINT8   LpddrMemWriteLatencySet;
  UINT8   EvLoader;
  UINT8   EvLoaderDelay;
  UINT8   RetrainOnFastFail;
  UINT8   CmdTriStateDis;

  UINT8   McLock;

  UINT8   GdxcEnable;
  UINT8   ProbelessTrace;
  UINT8   GdxcIotSize;
  UINT8   GdxcMotSize;
  UINT8   MemoryTrace;
  UINT8   SaGv;
  UINT16  FreqSaGvLow;

  //
  // MRC Training Algorithms
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
  UINT8   DIMMRONT;
  UINT8   WRDSEQT;
  UINT8   WRSRT;
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
  UINT8   ALIASCHK;
  UINT8   RCVENC1D;
  UINT8   RMC;
  UINT8   WRDSUDT;
  UINT8   CMDSR;
  UINT8   CMDDSEQ;
  UINT8   CMDNORM;

  //
  // Power and Thermal Throttling Options
  //
  UINT8   EnableExtts;
  UINT8   EnableCltm;
  UINT8   EnableOltm;
  UINT8   Ddr4SkipRefreshEn;
  UINT8   EnablePwrDn;
  UINT8   EnablePwrDnLpddr;
  UINT8   Refresh2X;
  UINT8   DdrThermalSensor;
  UINT8   LockPTMregs;
  UINT8   UserPowerWeightsEn;

  UINT8   EnergyScaleFact;
  UINT8   RaplPwrFlCh1;
  UINT8   RaplPwrFlCh0;

  UINT8   RaplLim2Lock;
  UINT8   RaplLim2WindX;
  UINT8   RaplLim2WindY;
  UINT8   RaplLim2Ena;
  UINT16  RaplLim2Pwr;
  UINT8   RaplLim1WindX;
  UINT8   RaplLim1WindY;
  UINT8   RaplLim1Ena;
  UINT16  RaplLim1Pwr;

  UINT8   WarmThresholdCh0Dimm0;
  UINT8   WarmThresholdCh0Dimm1;
  UINT8   WarmThresholdCh1Dimm0;
  UINT8   WarmThresholdCh1Dimm1;
  UINT8   HotThresholdCh0Dimm0;
  UINT8   HotThresholdCh0Dimm1;
  UINT8   HotThresholdCh1Dimm0;
  UINT8   HotThresholdCh1Dimm1;
  UINT8   WarmBudgetCh0Dimm0;
  UINT8   WarmBudgetCh0Dimm1;
  UINT8   WarmBudgetCh1Dimm0;
  UINT8   WarmBudgetCh1Dimm1;
  UINT8   HotBudgetCh0Dimm0;
  UINT8   HotBudgetCh0Dimm1;
  UINT8   HotBudgetCh1Dimm0;
  UINT8   HotBudgetCh1Dimm1;

  UINT8   IdleEnergyCh0Dimm1;
  UINT8   IdleEnergyCh0Dimm0;
  UINT8   PdEnergyCh0Dimm1;
  UINT8   PdEnergyCh0Dimm0;
  UINT8   ActEnergyCh0Dimm1;
  UINT8   ActEnergyCh0Dimm0;
  UINT8   RdEnergyCh0Dimm1;
  UINT8   RdEnergyCh0Dimm0;
  UINT8   WrEnergyCh0Dimm1;
  UINT8   WrEnergyCh0Dimm0;

  UINT8   IdleEnergyCh1Dimm1;
  UINT8   IdleEnergyCh1Dimm0;
  UINT8   PdEnergyCh1Dimm1;
  UINT8   PdEnergyCh1Dimm0;
  UINT8   ActEnergyCh1Dimm1;
  UINT8   ActEnergyCh1Dimm0;
  UINT8   RdEnergyCh1Dimm1;
  UINT8   RdEnergyCh1Dimm0;
  UINT8   WrEnergyCh1Dimm1;
  UINT8   WrEnergyCh1Dimm0;
  UINT8   SrefCfgEna;
  UINT16  SrefCfgIdleTmr;
  UINT8   ThrtCkeMinDefeat;
  UINT8   ThrtCkeMinTmr;
  UINT8   ThrtCkeMinDefeatLpddr;
  UINT8   ThrtCkeMinTmrLpddr;

  UINT8   MemoryThermalManagement;
  UINT8   PeciInjectedTemp;
  UINT8   ExttsViaTsOnBoard;
  UINT8   ExttsViaTsOnDimm;
  UINT8   VirtualTempSensor;
  UINT8   ScramblerSupport;
  UINT8   ForceColdReset;
  UINT8   DisableDimmChannel0;
  UINT8   DisableDimmChannel1;
  UINT8   RemapEnable;
  UINT8   MrcSafeConfig;
  UINT8   RhPrevention;
  UINT8   RhSolution;
  UINT8   RhActProbability;
  UINT8   MaxRttWr;
  UINT8   ExitOnFailure;

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
  //
  // CRID
  //
  UINT8   CridEnable;

  UINT8   SlateIndicatorRT;
  UINT8   SlateIndicatorSx;
  UINT8   DockIndicatorRT;
  UINT8   DockIndicatorSx;
  UINT8   IuerButtonEnable;
  UINT8   IuerConvertibleEnable;
  UINT8   IuerDockEnable;

  //
  // Uncore Related
  //
  UINT16  UncoreVoltageOffset;
  UINT8   UncoreVoltageOffsetPrefix;

  UINT16  UncoreVoltageOffsetDefault;
  UINT8   UncoreVoltageOffsetPrefixDefault;

  //
  // GT related
  //
  UINT8   GtsMaxOcRatio;                 ///< Maximum GT turbo ratio override
  UINT8   GtsVoltageMode;                ///< Specifies whether GT slice voltage is operating in Adaptive or Override mode - 0: Adaptive, 1: Override
  UINT16  GtsVoltageOffset;              ///< The voltage offset applied to GT slice. Valid range from -1000mv to 1000mv
  UINT8   GtsVoltageOffsetPrefix;
  UINT16  GtsVoltageOverride;            ///< The GT slice voltage override which is applied to the entire range of GT frequencies
  UINT16  GtsExtraTurboVoltage;          ///< The adaptive voltage applied during turbo frequencies. Valid range from 0 to 2000mV
  UINT8   GtusMaxOcRatio;                ///< Maximum GT turbo ratio override
  UINT8   GtusVoltageMode;               ///< Specifies whether GT unslice voltage is operating in Adaptive or Override mode - 0: Adaptive, 1: Override
  UINT16  GtusVoltageOffset;             ///< The voltage offset applied to GT unslice. Valid range from -1000mv to 1000mv
  UINT8   GtusVoltageOffsetPrefix;
  UINT16  GtusVoltageOverride;           ///< The GT unslice voltage override which is applied to the entire range of GT frequencies
  UINT16  GtusExtraTurboVoltage;         ///< The adaptive voltage applied during turbo frequencies. Valid range from 0 to 2000mV

  UINT8   GtsMaxOcRatioDefault;          ///< Maximum GT turbo ratio override
  UINT8   GtsVoltageModeDefault;         ///< Specifies whether GT slice voltage is operating in Adaptive or Override mode - 0: Adaptive, 1: Override
  UINT16  GtsVoltageOffsetDefault;       ///< The voltage offset applied to GT slice. Valid range from -1000mv to 1000mv
  UINT8   GtsVoltageOffsetPrefixDefault;
  UINT16  GtsVoltageOverrideDefault;     ///< The GT slice voltage override which is applied to the entire range of GT frequencies
  UINT16  GtsExtraTurboVoltageDefault;   ///< The adaptive voltage applied during turbo frequencies. Valid range from 0 to 2000mV
  UINT8   GtusMaxOcRatioDefault;         ///< Maximum GT turbo ratio override
  UINT8   GtusVoltageModeDefault;        ///< Specifies whether GT unslice voltage is operating in Adaptive or Override mode - 0: Adaptive, 1: Override
  UINT16  GtusVoltageOffsetDefault;      ///< The voltage offset applied to GT unslice. Valid range from -1000mv to 1000mv
  UINT8   GtusVoltageOffsetPrefixDefault;
  UINT16  GtusVoltageOverrideDefault;    ///< The GT unslice voltage override which is applied to the entire range of GT frequencies
  UINT16  GtusExtraTurboVoltageDefault;  ///< The adaptive voltage applied during turbo frequencies. Valid range from 0 to 2000mV

  // New Added Item after API lock
  UINT8   EWRDSEQ;
  UINT8   RealtimeMemoryTiming;          ///< Realtime memory timing will allow system to perform realtime memory timing changes.
  UINT8   BclkChangeWarmReset;
  UINT8   BclkChangeContinues;
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
  UINT8   EndOfSa;

//
// Pch Setup items
//
  UINT8   StartOfPch; 
// AMI_OVERRIDE_START  
  UINT8   SataControllerSpeed;
// AMI_OVERRIDE_END    
  UINT8   ExternalDma;
  UINT16  RootPortDid;              // not a SETUP item, used by BIOS to pass Root Port DID to SETUP items

  UINT8   DeepSxMode;
  UINT8   LanWakeFromDeepSx;
  UINT8   DisableDsxAcPresentPulldown;
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
  UINT8   PchCapsuleResetType; ///< @deprecated
  UINT8   PchCrid;
  UINT8   PchRtcLock;
  UINT8   PchBiosLock;

  UINT8   Hpet;
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
  UINT8   SataAlternateId; ///< @deprecated
  UINT8   SataSalp;
  UINT8   SataTestMode;
  UINT8   PxDevSlp[PCH_SATA_MAX_PORTS];
  UINT8   EnableDitoConfig[PCH_SATA_MAX_PORTS];
  UINT16  DitoVal[PCH_SATA_MAX_PORTS];
  UINT8   DmVal[PCH_SATA_MAX_PORTS];
  UINT8   SataLegacyOrom;

  //
  // PCI_EXPRESS_CONFIG, MAX 20 root ports
  //
  UINT8   PcieClockGatingDisabled;
  UINT8   PcieRootPort8xhDecode;
  UINT8   Pcie8xhDecodePortIndex;
  UINT8   PcieRootPortPeerMemoryWriteEnable;
  UINT8   PcieComplianceTestMode;
  UINT8   PcieUsbGlitchWa;
  UINT8   RstPcieStorageRemap[PCH_MAX_RST_PCIE_STORAGE_CR]; ///< @deprecated
  UINT8   RstPcieStorageRemapPort[PCH_MAX_RST_PCIE_STORAGE_CR]; ///< @deprecated
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
  UINT8   PcieRootPortEqPh3Method[PCH_PCIE_MAX_ROOT_PORTS];
  UINT8   PcieRootPortUptp[PCH_PCIE_MAX_ROOT_PORTS];
  UINT8   PcieRootPortDptp[PCH_PCIE_MAX_ROOT_PORTS];
  UINT8   PcieLaneCm[PCH_PCIE_MAX_ROOT_PORTS];
  UINT8   PcieLaneCp[PCH_PCIE_MAX_ROOT_PORTS];
  UINT8   PcieTopology[PCH_PCIE_MAX_ROOT_PORTS];
  UINT8   RpFunctionSwap;

  UINT8   PcieSwEqOverride;
  UINT8   PcieSwEqCoeffCm[5];
  UINT8   PcieSwEqCoeffCp[5];

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
  // PCI Bridge Resources
  //
  UINT8   PcieExtraBusRsvd[PCH_PCIE_MAX_ROOT_PORTS];
  UINT16  PcieMemRsvd[PCH_PCIE_MAX_ROOT_PORTS];
  UINT8   PcieIoRsvd[PCH_PCIE_MAX_ROOT_PORTS];

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

  //
  // TraceHub Setup Options
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
  // HSIO message
  //
  UINT8   ChipsetInitMessage;
  UINT8   BypassPhySyncReset;

  //
  // Miscellaneous options
  //
  UINT8   RstPcieRemapEnabled[PCH_PCIE_MAX_ROOT_PORTS];
  // Option to Enable/Disable UCMC/UCSI device
  UINT8   PchUcsiUcmcDevice;
  UINT16  PcieDetectTimeoutMs[PCH_PCIE_MAX_ROOT_PORTS];
  UINT8   PchLegacyIoLowLatency;
  UINT8   RaidDeviceId;
  UINT8   SataRstOptaneMemory;
  UINT8   SataRstCpuAttachedStorage;
  UINT8   HybridStorageMode;
  UINT8   EndOfPch;
// AMI_OVERRIDE_START >> EIP236974 : ZPODD ASL code implement
  UINT8   ZpOddSupport;
// AMI_OVERRIDE_END <<
#endif
