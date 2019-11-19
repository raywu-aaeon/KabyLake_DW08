//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

#ifndef __ACPI_SETUP_DATA_H__ // To Avoid this header get compiled twice
#define __ACPI_SETUP_DATA_H__


#include <Efi.h>

#ifdef __cplusplus
extern "C" {
#endif
   
typedef struct  _ACPI_SETUP_DATA  ACPI_SETUP_DATA;

#pragma pack(push, 1)

typedef struct _ACPI_SETUP_DATA {
  UINT8   AcpiAuto;
  UINT8   AcpiSleepState;
  UINT8   Acpi50Enabled;
  UINT8   AcpiHibernate;
  UINT8   AcpiTableRevision;
  UINT8   PciExpNative;
  UINT8   NativeAspmEnable;
  UINT8   PtidSupport;
  // AcpiDebug Setup Options
  UINT8   AcpiDebug;
  UINT32  AcpiDebugAddress;
  UINT8   PciDelayOptimizationEcr;
// AMI_OVERRIDE_START >> EIP236974 : ZPODD ASL code implement
  UINT8   ZpOddSupport;
// AMI_OVERRIDE_END <<
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
  UINT8   LpitResidencyCounter;
  UINT8   IrmtConfiguration;
  // AMI_OVERRIDE_ACPI0009_START  
  UINT8   IrmtHwNotification;
  UINT8   IrmtState;
  // AMI_OVERRIDE_ACPI0009_END   
  // Reading SSDT table from file
  UINT8   LoadSSDTFromFile;
  //
  // On Screen Branding
  //
  UINT16  OperatingSystem;
  UINT16  OemBadgingBrand;
  UINT8   BootFirstToShell;

  UINT8   CdClock;
  UINT8   SensorHubType;
  UINT8   DebugUsbUart;
  UINT8   PxeRom;
  UINT8   EfiNetworkSupport;
  UINT8   Reserved; // Get rid of "EnableClockSpreadSpec"
  UINT8   UsbSensorHub;
  UINT8   EfiRaid; ///<@deprecated

  UINT8   HddAcousticPowerManagement;
  UINT8   HddAcousticMode;

  //
  // Recovery Configuration
  //
  UINT8   IsRecoveryJumper;

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
  UINT8   PmicVccLevel;
  UINT8   PmicVddqLevel;
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

  UINT8   CsmControl;

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
  UINT8   TbtXConnectSupport;     ///< 1=Supported; 0=Not Supported
//AMI_OVERRIDE_END - EIP322154 - Remove Thunderbolt DGPU related setup items
  UINT8   TbtUsbOn;
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
  UINT8   TbtSelector1;
  UINT8   Multicontroller;  
  UINT8   PepIsp;
  UINT8   Win10Support;
  UINT8   Gpio3ForcePwr1;
  UINT8   TrOsup;

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
  UINT8   UcsiRetryEcEnable;
  UINT16  Rtd3TbtOffDelay;
  UINT8   PepCsme;
  UINT8   WwanInterface;
  UINT8   Rtd3Tbt;
  UINT8   Rtd3TbtClkReq;
  UINT16  Rtd3TbtClkReqDelay;
  UINT8   KblRx4SlotGpioPowerPinEnable; ///<@deprecated since revision 25
  UINT8   Rtd3WwanDsm;
} ACPI_SETUP_DATA;

#pragma pack(pop)

VOID GetAcpiSetupData (
    IN VOID                   *Service,
    IN OUT ACPI_SETUP_DATA    *AcpiSetupData
);

typedef VOID (ACPI_OEM_SETUP_CALLBACK) (
    IN VOID                   *Services,
    IN OUT ACPI_SETUP_DATA    *AcpiSetupData,
    IN SETUP_DATA             *SetupData
);
/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif 

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
