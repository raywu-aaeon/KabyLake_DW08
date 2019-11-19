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

/** @file AcpiPlatformSetup.c
    Get Acpi Platfrom Setup

**/
//----------------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------------

#include <Token.h>
#include <AmiCspLib.h>
#include <Library/DebugLib.h>
#include <AcpiGetSetupData/AcpiSetupData.h>

/**
    This function returns ACPI setup data from system SetupData
    variable 

    @param Services Pointer to PeiServices or RuntimeServices
        structure  
    @param AcpiSetupData Pointer to custom setup data to return
    @param SetupData Pointer to system setup data.
    @param Pei Pei flag. If TRUE we are in PEI phase

    @retval VOID
**/
VOID AcpiSetupCallbacks (
  IN VOID                *Services,
  IN OUT ACPI_SETUP_DATA *AcpiSetupData,
  IN SETUP_DATA          *SetupData
  )
{
  UINT8   i; 

  DEBUG ((DEBUG_INFO, "AcpiSetupCallbacks:: Entry\n"));
  
  if (SetupData == NULL) {
    DEBUG ((EFI_D_ERROR, "AcpiSetupCallbacks:: Did not get the setup data successfully.\n"));
    ASSERT_EFI_ERROR (EFI_NOT_FOUND);
  } else {
    DEBUG ((DEBUG_INFO, "AcpiSetupCallbacks:: Start to update Acpi setup data.\n"));
    
    //UINT8   AcpiAuto;
    //UINT8   AcpiSleepState;;
    //UINT8   Acpi50Enabled;
    //UINT8   AcpiHibernate;
    //UINT8   AcpiTableRevision;        
    AcpiSetupData->PciExpNative = SetupData->PciExpNative;
    AcpiSetupData->NativeAspmEnable = SetupData->NativeAspmEnable;  
    AcpiSetupData->PtidSupport = SetupData->PtidSupport;	  
    // AcpiDebug Setup Options	  
#if defined RTCWakeup_SUPPORT_FLAG && RTCWakeup_SUPPORT_FLAG == 1
    AcpiSetupData->AcpiDebug = SetupData->AcpiDebug;
    //UINT32  AcpiDebugAddress
#endif
    AcpiSetupData->PciDelayOptimizationEcr = SetupData->PciDelayOptimizationEcr;    
// AMI_OVERRIDE_ACPI0019_START >>>  EIP236974 : ZPODD ASL code implement    
#if defined (ZPODD_SUPPORT) && ZPODD_SUPPORT == 1 
    AcpiSetupData->ZpOddSupport = SetupData->ZpOddSupport;
#endif    
// AMI_OVERRIDE_ACPI0019_END <<< 
#if defined RTCWakeup_SUPPORT && RTCWakeup_SUPPORT == 1
    // S5 RTC wakeup setup options    
    AcpiSetupData->WakeOnRTCS5 = SetupData->WakeOnRTCS5;
    AcpiSetupData->RTCWakeupDate = SetupData->RTCWakeupDate;
    AcpiSetupData->RTCWakeupTimeHour = SetupData->RTCWakeupTimeHour;
    AcpiSetupData->RTCWakeupTimeMinute = SetupData->RTCWakeupTimeMinute;
    AcpiSetupData->RTCWakeupTimeSecond = SetupData->RTCWakeupTimeSecond;
#endif
#if defined (PTID_SUPPORT) && PTID_SUPPORT == 1     
    AcpiSetupData->PeciAccessMethod = SetupData->PeciAccessMethod;
#endif
    AcpiSetupData->LowPowerS0Idle = SetupData->LowPowerS0Idle;
    AcpiSetupData->PepCpu = SetupData->PepCpu;
    AcpiSetupData->PepGfx = SetupData->PepGfx;
//    AcpiSetupData->PepSataContraints = SetupData->PepSataContraints; ///<@deprecated
    AcpiSetupData->PepUart = SetupData->PepUart;
    AcpiSetupData->PepI2c0 = SetupData->PepI2c0;
    AcpiSetupData->PepI2c1 = SetupData->PepI2c1; 
    AcpiSetupData->PepI2c2 = SetupData->PepI2c2;
    AcpiSetupData->PepI2c3 = SetupData->PepI2c3; 
    AcpiSetupData->PepI2c4 = SetupData->PepI2c4;
    AcpiSetupData->PepI2c5 = SetupData->PepI2c5;
    AcpiSetupData->PepSpi = SetupData->PepSpi; 
    AcpiSetupData->PepXhci = SetupData->PepXhci;
    AcpiSetupData->PepAudio = SetupData->PepAudio;
    AcpiSetupData->PepEmmc = SetupData->PepEmmc; 
    AcpiSetupData->PepSdxc = SetupData->PepSdxc;
#if defined (LOW_POWER_S0_IDLE_CAPABLE) && LOW_POWER_S0_IDLE_CAPABLE  == 1      
    AcpiSetupData->CSNotifyEC = SetupData->CSNotifyEC;
    AcpiSetupData->CSDebugLightEC = SetupData->CSDebugLightEC;
    AcpiSetupData->EcLowPowerMode = SetupData->EcLowPowerMode;
    AcpiSetupData->EcDebugLed = SetupData->EcDebugLed;
    AcpiSetupData->ECBaseCsPwrPolicy = SetupData->ECBaseCsPwrPolicy;    
    AcpiSetupData->SensorStandby = SetupData->SensorStandby;
    AcpiSetupData->PL1LimitCS = SetupData->PL1LimitCS;
    AcpiSetupData->PL1LimitCSValue = SetupData->PL1LimitCSValue;
#endif 
    AcpiSetupData->TenSecondPowerButtonEnable = SetupData->TenSecondPowerButtonEnable;    
#if defined IntelRMT_SUPPORT && IntelRMT_SUPPORT == 1
    AcpiSetupData->IrmtConfiguration = SetupData->IrmtConfiguration;
// AMI_OVERRIDE_ACPI0009_START
    AcpiSetupData->IrmtHwNotification = SetupData->IrmtHwNotification;
    AcpiSetupData->IrmtState = SetupData->IrmtState;
// AMI_OVERRIDE_ACPI0009_END 
#endif     
    // Reading SSDT table from file
    //UINT8   LoadSSDTFromFile;
    //
    // On Screen Branding
    //
    //UINT16  OperatingSystem;
    //UINT16  OemBadgingBrand;
    //UINT8   BootFirstToShell;

    //UINT8   CdClock;
    //UINT8   SensorHubType;
    //UINT8   DebugUsbUart;
    //UINT8   PxeRom;
    //UINT8   EfiNetworkSupport;
    //UINT8   Reserved; // Get rid of "EnableClockSpreadSpec"
    AcpiSetupData->UsbSensorHub = SetupData->UsbSensorHub;
    //UINT8   EfiRaid;   
    
    //UINT8   HddAcousticPowerManagement;
    //UINT8   HddAcousticMode;

    //
    // Recovery Configuration
    //
    //UINT8   IsRecoveryJumper;    
    
    //
    // SkyCam CIO2 configuration
    //
    AcpiSetupData->PchSkyCamPortATermOverride = SetupData->PchSkyCamPortATermOverride;
    AcpiSetupData->PchSkyCamPortBTermOverride = SetupData->PchSkyCamPortBTermOverride;
    AcpiSetupData->PchSkyCamPortCTermOverride = SetupData->PchSkyCamPortCTermOverride;
    AcpiSetupData->PchSkyCamPortDTermOverride = SetupData->PchSkyCamPortDTermOverride;

    AcpiSetupData->PchSkyCamPortATrimEnable = SetupData->PchSkyCamPortATrimEnable;
    AcpiSetupData->PchSkyCamPortBTrimEnable = SetupData->PchSkyCamPortBTrimEnable;
    AcpiSetupData->PchSkyCamPortCTrimEnable = SetupData->PchSkyCamPortCTrimEnable;
    AcpiSetupData->PchSkyCamPortDTrimEnable = SetupData->PchSkyCamPortDTrimEnable;

    AcpiSetupData->PchSkyCamPortACtleEnable = SetupData->PchSkyCamPortACtleEnable;
    AcpiSetupData->PchSkyCamPortBCtleEnable = SetupData->PchSkyCamPortBCtleEnable;
    AcpiSetupData->PchSkyCamPortCDCtleEnable = SetupData->PchSkyCamPortCDCtleEnable;

    AcpiSetupData->PchSkyCamPortADataTrimValue = SetupData->PchSkyCamPortADataTrimValue;
    AcpiSetupData->PchSkyCamPortBDataTrimValue = SetupData->PchSkyCamPortBDataTrimValue;
    AcpiSetupData->PchSkyCamPortCDDataTrimValue = SetupData->PchSkyCamPortCDDataTrimValue;

    AcpiSetupData->PchSkyCamPortACtleCapValue = SetupData->PchSkyCamPortACtleCapValue;
    AcpiSetupData->PchSkyCamPortBCtleCapValue = SetupData->PchSkyCamPortBCtleCapValue;
    AcpiSetupData->PchSkyCamPortCDCtleCapValue = SetupData->PchSkyCamPortCDCtleCapValue;

    AcpiSetupData->PchSkyCamPortACtleResValue = SetupData->PchSkyCamPortACtleResValue;
    AcpiSetupData->PchSkyCamPortBCtleResValue = SetupData->PchSkyCamPortBCtleResValue;
    AcpiSetupData->PchSkyCamPortCDCtleResValue = SetupData->PchSkyCamPortCDCtleResValue;

    AcpiSetupData->PchSkyCamPortAClkTrimValue = SetupData->PchSkyCamPortAClkTrimValue;
    AcpiSetupData->PchSkyCamPortBClkTrimValue = SetupData->PchSkyCamPortBClkTrimValue;
    AcpiSetupData->PchSkyCamPortCClkTrimValue = SetupData->PchSkyCamPortCClkTrimValue;
    AcpiSetupData->PchSkyCamPortDClkTrimValue = SetupData->PchSkyCamPortDClkTrimValue;  
    
    //
    // SkyCam sensor configuration menu
    //
    AcpiSetupData->SkyCam_ControlLogic0 = SetupData->SkyCam_ControlLogic0;    
    AcpiSetupData->SkyCam_ControlLogic1 = SetupData->SkyCam_ControlLogic1;    
    AcpiSetupData->SkyCam_ControlLogic2 = SetupData->SkyCam_ControlLogic2;    
    AcpiSetupData->SkyCam_ControlLogic3 = SetupData->SkyCam_ControlLogic3;    

    AcpiSetupData->SkyCam_Link0 = SetupData->SkyCam_Link0;    
    AcpiSetupData->SkyCam_Link1 = SetupData->SkyCam_Link1; 
    AcpiSetupData->SkyCam_Link2 = SetupData->SkyCam_Link2; 
    AcpiSetupData->SkyCam_Link3 = SetupData->SkyCam_Link3; 
    
    AcpiSetupData->SkyCam_LanesClkDiv = SetupData->SkyCam_LanesClkDiv;     
    
    AcpiSetupData-> SkyCam_ControlLogic0_Type = SetupData->SkyCam_ControlLogic0_Type;
    AcpiSetupData->SkyCam_ControlLogic0_CrdVersion = SetupData->SkyCam_ControlLogic0_CrdVersion;
    AcpiSetupData->SkyCam_ControlLogic0_GpioPinsEnabled = SetupData->SkyCam_ControlLogic0_GpioPinsEnabled;
    AcpiSetupData->SkyCam_ControlLogic0_I2cChannel = SetupData->SkyCam_ControlLogic0_I2cChannel;
    AcpiSetupData->SkyCam_ControlLogic0_I2cAddress = SetupData->SkyCam_ControlLogic0_I2cAddress;    
    AcpiSetupData->SkyCam_ControlLogic0_Pld = SetupData->SkyCam_ControlLogic0_Pld;   
    for (i = 0; i < SKYCAM_GPIO_COUNT; i++) {
      AcpiSetupData->SkyCam_ControlLogic0_GpioGroupPadNumber[i] = SetupData->SkyCam_ControlLogic0_GpioGroupPadNumber[i];
      AcpiSetupData->SkyCam_ControlLogic0_GpioGroupNumber[i] = SetupData->SkyCam_ControlLogic0_GpioGroupNumber[i];
      AcpiSetupData->SkyCam_ControlLogic0_GpioFunction[i] = SetupData->SkyCam_ControlLogic0_GpioFunction[i];
      AcpiSetupData->SkyCam_ControlLogic0_GpioActiveValue[i] = SetupData->SkyCam_ControlLogic0_GpioActiveValue[i];
      AcpiSetupData->SkyCam_ControlLogic0_GpioInitialValue[i] = SetupData->SkyCam_ControlLogic0_GpioInitialValue[i];
    }

    AcpiSetupData->SkyCam_ControlLogic1_Type = SetupData->SkyCam_ControlLogic1_Type;
    AcpiSetupData->SkyCam_ControlLogic1_CrdVersion = SetupData->SkyCam_ControlLogic1_CrdVersion;
    AcpiSetupData->SkyCam_ControlLogic1_GpioPinsEnabled = SetupData->SkyCam_ControlLogic1_GpioPinsEnabled;
    AcpiSetupData->SkyCam_ControlLogic1_I2cChannel = SetupData->SkyCam_ControlLogic1_I2cChannel;
    AcpiSetupData->SkyCam_ControlLogic1_I2cAddress = SetupData->SkyCam_ControlLogic1_I2cAddress;    
    AcpiSetupData->SkyCam_ControlLogic1_Pld = SetupData->SkyCam_ControlLogic1_Pld;    
    for (i = 0; i < SKYCAM_GPIO_COUNT; i++) {
      AcpiSetupData->SkyCam_ControlLogic1_GpioGroupPadNumber[i] = SetupData->SkyCam_ControlLogic1_GpioGroupPadNumber[i];
      AcpiSetupData->SkyCam_ControlLogic1_GpioGroupNumber[i] = SetupData->SkyCam_ControlLogic1_GpioGroupNumber[i];
      AcpiSetupData->SkyCam_ControlLogic1_GpioFunction[i] = SetupData->SkyCam_ControlLogic1_GpioFunction[i];
      AcpiSetupData->SkyCam_ControlLogic1_GpioActiveValue[i] = SetupData->SkyCam_ControlLogic1_GpioActiveValue[i];
      AcpiSetupData->SkyCam_ControlLogic1_GpioInitialValue[i] = SetupData->SkyCam_ControlLogic1_GpioInitialValue[i];
    }
    
    AcpiSetupData->SkyCam_ControlLogic2_Type = SetupData->SkyCam_ControlLogic2_Type;
    AcpiSetupData->SkyCam_ControlLogic2_CrdVersion = SetupData->SkyCam_ControlLogic2_CrdVersion;
    AcpiSetupData->SkyCam_ControlLogic2_GpioPinsEnabled = SetupData->SkyCam_ControlLogic2_GpioPinsEnabled;
    AcpiSetupData->SkyCam_ControlLogic2_I2cChannel = SetupData->SkyCam_ControlLogic2_I2cChannel;
    AcpiSetupData->SkyCam_ControlLogic2_I2cAddress = SetupData->SkyCam_ControlLogic2_I2cAddress;    
    AcpiSetupData->SkyCam_ControlLogic2_Pld = SetupData->SkyCam_ControlLogic2_Pld;   
    for(i = 0; i < SKYCAM_GPIO_COUNT; i++) {
      AcpiSetupData->SkyCam_ControlLogic2_GpioGroupPadNumber[i] = SetupData->SkyCam_ControlLogic2_GpioGroupPadNumber[i];
      AcpiSetupData->SkyCam_ControlLogic2_GpioGroupNumber[i] = SetupData->SkyCam_ControlLogic2_GpioGroupNumber[i];
      AcpiSetupData->SkyCam_ControlLogic2_GpioFunction[i] = SetupData->SkyCam_ControlLogic2_GpioFunction[i];
      AcpiSetupData->SkyCam_ControlLogic2_GpioActiveValue[i] = SetupData->SkyCam_ControlLogic2_GpioActiveValue[i];
      AcpiSetupData->SkyCam_ControlLogic2_GpioInitialValue[i] = SetupData->SkyCam_ControlLogic2_GpioInitialValue[i];
    }
    
    AcpiSetupData->SkyCam_ControlLogic3_Type = SetupData->SkyCam_ControlLogic3_Type;
    AcpiSetupData->SkyCam_ControlLogic3_CrdVersion = SetupData->SkyCam_ControlLogic3_CrdVersion;
    AcpiSetupData->SkyCam_ControlLogic3_GpioPinsEnabled = SetupData->SkyCam_ControlLogic3_GpioPinsEnabled;
    AcpiSetupData->SkyCam_ControlLogic3_I2cChannel = SetupData->SkyCam_ControlLogic3_I2cChannel;
    AcpiSetupData->SkyCam_ControlLogic3_I2cAddress = SetupData->SkyCam_ControlLogic3_I2cAddress;    
    AcpiSetupData->SkyCam_ControlLogic3_Pld = SetupData->SkyCam_ControlLogic3_Pld;   
    for (i = 0; i < SKYCAM_GPIO_COUNT; i++) {
      AcpiSetupData->SkyCam_ControlLogic3_GpioGroupPadNumber[i] = SetupData->SkyCam_ControlLogic3_GpioGroupPadNumber[i];
      AcpiSetupData->SkyCam_ControlLogic3_GpioGroupNumber[i] = SetupData->SkyCam_ControlLogic3_GpioGroupNumber[i];
      AcpiSetupData->SkyCam_ControlLogic3_GpioFunction[i] = SetupData->SkyCam_ControlLogic3_GpioFunction[i];
      AcpiSetupData->SkyCam_ControlLogic3_GpioActiveValue[i] = SetupData->SkyCam_ControlLogic3_GpioActiveValue[i];
      AcpiSetupData->SkyCam_ControlLogic3_GpioInitialValue[i] = SetupData->SkyCam_ControlLogic3_GpioInitialValue[i];
    } 

    AcpiSetupData->SkyCam_Link0_SensorModel = SetupData->SkyCam_Link0_SensorModel;
    for (i = 0; i < SKYCAM_HID_LENGTH; i++) {
      AcpiSetupData->SkyCam_Link0_UserHid[i] = SetupData->SkyCam_Link0_UserHid[i];
    }
    AcpiSetupData->SkyCam_Link0_CameraPhysicalLocation = SetupData->SkyCam_Link0_CameraPhysicalLocation;    
    for (i = 0; i < SKYCAM_MODULE_NAME_LENGTH; i++) {
      AcpiSetupData->SkyCam_Link0_ModuleName[i] = SetupData->SkyCam_Link0_ModuleName[i];
    }
    AcpiSetupData->SkyCam_Link0_I2cDevicesEnabled = SetupData->SkyCam_Link0_I2cDevicesEnabled;
    AcpiSetupData->SkyCam_Link0_I2cChannel = SetupData->SkyCam_Link0_I2cChannel;
    for (i = 0; i < SKYCAM_I2C_DEVICES_COUNT; i++) {
      AcpiSetupData->SkyCam_Link0_I2cAddress[i] = SetupData->SkyCam_Link0_I2cAddress[i];
      AcpiSetupData->SkyCam_Link0_I2cDeviceType[i] = SetupData->SkyCam_Link0_I2cDeviceType[i];
    }      
    AcpiSetupData->SkyCam_Link0_DriverData_Version = SetupData->SkyCam_Link0_DriverData_Version;
    AcpiSetupData->SkyCam_Link0_DriverData_CrdVersion = SetupData->SkyCam_Link0_DriverData_CrdVersion;
    AcpiSetupData->SkyCam_Link0_DriverData_LaneUsed = SetupData->SkyCam_Link0_DriverData_LaneUsed;
    AcpiSetupData->SkyCam_Link0_DriverData_EepromType = SetupData->SkyCam_Link0_DriverData_EepromType;
    AcpiSetupData->SkyCam_Link0_DriverData_VcmType = SetupData->SkyCam_Link0_DriverData_VcmType;
    //UINT8   SkyCam_Link0_DriverData_CustomData[4];
    AcpiSetupData->SkyCam_Link0_DriverData_Mclk = SetupData->SkyCam_Link0_DriverData_Mclk;    
    AcpiSetupData->SkyCam_Link0_DriverData_ControlLogic = SetupData->SkyCam_Link0_DriverData_ControlLogic;    
    AcpiSetupData->SkyCam_Link0_DriverData_FlashSupport = SetupData->SkyCam_Link0_DriverData_FlashSupport;
    AcpiSetupData->SkyCam_Link0_DriverData_Degree = SetupData->SkyCam_Link0_DriverData_Degree;
    
    AcpiSetupData->SkyCam_Link1_SensorModel = SetupData->SkyCam_Link1_SensorModel;
    for (i = 0; i < SKYCAM_HID_LENGTH; i++) {
      AcpiSetupData->SkyCam_Link1_UserHid[i] = SetupData->SkyCam_Link1_UserHid[i];
    }
    AcpiSetupData->SkyCam_Link1_CameraPhysicalLocation = SetupData->SkyCam_Link1_CameraPhysicalLocation;    
    for (i = 0; i < SKYCAM_MODULE_NAME_LENGTH; i++) {
      AcpiSetupData->SkyCam_Link1_ModuleName[i] = SetupData->SkyCam_Link1_ModuleName[i];
    }
    AcpiSetupData->SkyCam_Link1_I2cDevicesEnabled = SetupData->SkyCam_Link1_I2cDevicesEnabled;
    AcpiSetupData->SkyCam_Link1_I2cChannel = SetupData->SkyCam_Link1_I2cChannel;
    for (i = 0; i < SKYCAM_I2C_DEVICES_COUNT; i++) {
      AcpiSetupData->SkyCam_Link1_I2cAddress[i] = SetupData->SkyCam_Link1_I2cAddress[i];
      AcpiSetupData->SkyCam_Link1_I2cDeviceType[i] = SetupData->SkyCam_Link1_I2cDeviceType[i];
    }      
    AcpiSetupData->SkyCam_Link1_DriverData_Version = SetupData->SkyCam_Link1_DriverData_Version;
    AcpiSetupData->SkyCam_Link1_DriverData_CrdVersion = SetupData->SkyCam_Link1_DriverData_CrdVersion;
    AcpiSetupData->SkyCam_Link1_DriverData_LaneUsed = SetupData->SkyCam_Link1_DriverData_LaneUsed;
    AcpiSetupData->SkyCam_Link1_DriverData_EepromType = SetupData->SkyCam_Link1_DriverData_EepromType;
    AcpiSetupData->SkyCam_Link1_DriverData_VcmType = SetupData->SkyCam_Link1_DriverData_VcmType;
    //UINT8   SkyCam_Link1_DriverData_CustomData[4];
    AcpiSetupData->SkyCam_Link1_DriverData_Mclk = SetupData->SkyCam_Link1_DriverData_Mclk;    
    AcpiSetupData->SkyCam_Link1_DriverData_ControlLogic = SetupData->SkyCam_Link1_DriverData_ControlLogic;    
    AcpiSetupData->SkyCam_Link1_DriverData_FlashSupport = SetupData->SkyCam_Link1_DriverData_FlashSupport;
    AcpiSetupData->SkyCam_Link1_DriverData_Degree = SetupData->SkyCam_Link1_DriverData_Degree;
    
    AcpiSetupData->SkyCam_Link2_SensorModel = SetupData->SkyCam_Link2_SensorModel;
    for (i = 0; i < SKYCAM_HID_LENGTH; i++) {
      AcpiSetupData->SkyCam_Link2_UserHid[i] = SetupData->SkyCam_Link2_UserHid[i];
    }
    AcpiSetupData->SkyCam_Link2_CameraPhysicalLocation = SetupData->SkyCam_Link2_CameraPhysicalLocation;    
    for (i = 0; i < SKYCAM_MODULE_NAME_LENGTH; i++) {
      AcpiSetupData->SkyCam_Link2_ModuleName[i] = SetupData->SkyCam_Link2_ModuleName[i];
    }
    AcpiSetupData->SkyCam_Link2_I2cDevicesEnabled = SetupData->SkyCam_Link2_I2cDevicesEnabled;
    AcpiSetupData->SkyCam_Link2_I2cChannel = SetupData->SkyCam_Link2_I2cChannel;
    for (i = 0; i < SKYCAM_I2C_DEVICES_COUNT; i++) {
      AcpiSetupData->SkyCam_Link2_I2cAddress[i] = SetupData->SkyCam_Link2_I2cAddress[i];
      AcpiSetupData->SkyCam_Link2_I2cDeviceType[i] = SetupData->SkyCam_Link2_I2cDeviceType[i];
    }      
    AcpiSetupData->SkyCam_Link2_DriverData_Version = SetupData->SkyCam_Link2_DriverData_Version;
    AcpiSetupData->SkyCam_Link2_DriverData_CrdVersion = SetupData->SkyCam_Link2_DriverData_CrdVersion;
    AcpiSetupData->SkyCam_Link2_DriverData_LaneUsed = SetupData->SkyCam_Link2_DriverData_LaneUsed;
    AcpiSetupData->SkyCam_Link2_DriverData_EepromType = SetupData->SkyCam_Link2_DriverData_EepromType;
    AcpiSetupData->SkyCam_Link2_DriverData_VcmType = SetupData->SkyCam_Link2_DriverData_VcmType;
    //UINT8   SkyCam_Link2_DriverData_CustomData[4];
    AcpiSetupData->SkyCam_Link2_DriverData_Mclk = SetupData->SkyCam_Link2_DriverData_Mclk;    
    AcpiSetupData->SkyCam_Link2_DriverData_ControlLogic = SetupData->SkyCam_Link2_DriverData_ControlLogic;    
    AcpiSetupData->SkyCam_Link2_DriverData_FlashSupport = SetupData->SkyCam_Link2_DriverData_FlashSupport;
    AcpiSetupData->SkyCam_Link2_DriverData_Degree = SetupData->SkyCam_Link2_DriverData_Degree;
    
    AcpiSetupData->SkyCam_Link3_SensorModel = SetupData->SkyCam_Link3_SensorModel;
    for (i = 0; i < SKYCAM_HID_LENGTH; i++) {
      AcpiSetupData->SkyCam_Link3_UserHid[i] = SetupData->SkyCam_Link3_UserHid[i];
    }
    AcpiSetupData->SkyCam_Link3_CameraPhysicalLocation = SetupData->SkyCam_Link3_CameraPhysicalLocation;    
    for (i = 0; i < SKYCAM_MODULE_NAME_LENGTH; i++) {
      AcpiSetupData->SkyCam_Link3_ModuleName[i] = SetupData->SkyCam_Link3_ModuleName[i];
    }
    AcpiSetupData->SkyCam_Link3_I2cDevicesEnabled = SetupData->SkyCam_Link3_I2cDevicesEnabled;
    AcpiSetupData->SkyCam_Link3_I2cChannel = SetupData->SkyCam_Link3_I2cChannel;
    for (i = 0; i < SKYCAM_I2C_DEVICES_COUNT; i++) {
      AcpiSetupData->SkyCam_Link3_I2cAddress[i] = SetupData->SkyCam_Link3_I2cAddress[i];
      AcpiSetupData->SkyCam_Link3_I2cDeviceType[i] = SetupData->SkyCam_Link3_I2cDeviceType[i];
    }      
    AcpiSetupData->SkyCam_Link3_DriverData_Version = SetupData->SkyCam_Link3_DriverData_Version;
    AcpiSetupData->SkyCam_Link3_DriverData_CrdVersion = SetupData->SkyCam_Link3_DriverData_CrdVersion;
    AcpiSetupData->SkyCam_Link3_DriverData_LaneUsed = SetupData->SkyCam_Link3_DriverData_LaneUsed;
    AcpiSetupData->SkyCam_Link3_DriverData_EepromType = SetupData->SkyCam_Link3_DriverData_EepromType;
    AcpiSetupData->SkyCam_Link3_DriverData_VcmType = SetupData->SkyCam_Link3_DriverData_VcmType;
    //UINT8   SkyCam_Link3_DriverData_CustomData[4];
    AcpiSetupData->SkyCam_Link3_DriverData_Mclk = SetupData->SkyCam_Link3_DriverData_Mclk;    
    AcpiSetupData->SkyCam_Link3_DriverData_ControlLogic = SetupData->SkyCam_Link3_DriverData_ControlLogic;    
    AcpiSetupData->SkyCam_Link3_DriverData_FlashSupport = SetupData->SkyCam_Link3_DriverData_FlashSupport;
    AcpiSetupData->SkyCam_Link3_DriverData_Degree = SetupData->SkyCam_Link3_DriverData_Degree;
    
    AcpiSetupData->PchI2c0SensorDevice = SetupData->PchI2c0SensorDevice;    
    AcpiSetupData->PchI2c0TouchpadBusAddress = SetupData->PchI2c0TouchpadBusAddress;
    AcpiSetupData->PchI2c0TouchpadHidAddress = SetupData->PchI2c0TouchpadHidAddress;
    AcpiSetupData->PchI2c0TouchpadSpeed = SetupData->PchI2c0TouchpadSpeed;    
    AcpiSetupData->PchI2c0SensorIrqMode = SetupData->PchI2c0SensorIrqMode;
    AcpiSetupData->PchI2c1SensorDevice = SetupData->PchI2c1SensorDevice;
    AcpiSetupData->PchI2c1SensorIrqMode = SetupData->PchI2c1SensorIrqMode;
    AcpiSetupData->PchI2c1TouchpanelBusAddress = SetupData->PchI2c1TouchpanelBusAddress;
    AcpiSetupData->PchI2c1TouchpanelHidAddress = SetupData->PchI2c1TouchpanelHidAddress;
    AcpiSetupData->PchI2c2SensorDevice = SetupData->PchI2c2SensorDevice;
    AcpiSetupData->PchI2c3SensorDevice = SetupData->PchI2c3SensorDevice;
    AcpiSetupData->PchI2c4SensorDevice = SetupData->PchI2c4SensorDevice;
    AcpiSetupData->PchSpi1SensorDevice = SetupData->PchSpi1SensorDevice;
    AcpiSetupData->PchSpi1SensorIrqMode = SetupData->PchSpi1SensorIrqMode;
    AcpiSetupData->PchUart0SensorDevice = SetupData->PchUart0SensorDevice;
    AcpiSetupData->PchUart0SensorIrqMode = SetupData->PchUart0SensorIrqMode;    
    AcpiSetupData->PchI2c5SensorDevice = SetupData->PchI2c5SensorDevice;
    AcpiSetupData->PchUart1SensorDevice = SetupData->PchUart1SensorDevice;
    AcpiSetupData->PchUart2SensorDevice = SetupData->PchUart2SensorDevice;
    AcpiSetupData->PchI2cWittDevice = SetupData->PchI2cWittDevice;    
    AcpiSetupData->PchI2cWittVersion = SetupData->PchI2cWittVersion;
    AcpiSetupData->PchUartUtkDevice = SetupData->PchUartUtkDevice;
    AcpiSetupData->GnssModel= SetupData->GnssModel;
    AcpiSetupData->GnssConnection = SetupData->GnssConnection;
    //UINT8   PchFpsEnable;    //PCH
    
    AcpiSetupData->PchSerialIoUseTimingParameters = SetupData->PchSerialIoUseTimingParameters;
    AcpiSetupData->PchSerialIoTimingSSHI = SetupData->PchSerialIoTimingSSHI;
    AcpiSetupData->PchSerialIoTimingSSLI = SetupData->PchSerialIoTimingSSLI;
    AcpiSetupData->PchSerialIoTimingSSDI = SetupData->PchSerialIoTimingSSDI;
    AcpiSetupData->PchSerialIoTimingFMHI = SetupData->PchSerialIoTimingFMHI;
    AcpiSetupData->PchSerialIoTimingFMLI = SetupData->PchSerialIoTimingFMLI;
    AcpiSetupData->PchSerialIoTimingFMDI = SetupData->PchSerialIoTimingFMDI;
    AcpiSetupData->PchSerialIoTimingFPHI = SetupData->PchSerialIoTimingFPHI;
    AcpiSetupData->PchSerialIoTimingFPLI = SetupData->PchSerialIoTimingFPLI;
    AcpiSetupData->PchSerialIoTimingFPDI = SetupData->PchSerialIoTimingFPDI;
    AcpiSetupData->PchSerialIoTimingM0CI = SetupData->PchSerialIoTimingM0CI;
    AcpiSetupData->PchSerialIoTimingM1CI = SetupData->PchSerialIoTimingM1CI;
    AcpiSetupData->PchSerialIoTimingM0CS = SetupData->PchSerialIoTimingM0CS;
    AcpiSetupData->PchSerialIoTimingM1CS = SetupData->PchSerialIoTimingM1CS;
    AcpiSetupData->PchSerialIoTimingM0CU = SetupData->PchSerialIoTimingM0CU;
    AcpiSetupData->PchSerialIoTimingM1CU = SetupData->PchSerialIoTimingM1CU; 

    AcpiSetupData->Ps2KbMsEnable = SetupData->Ps2KbMsEnable;
    //UINT8   TouchPanelMuxSelector;    //PCH
    //UINT8   BtUartMuxSelector;    //PCH
    //UINT8   BtWirelessChargingMode;    //PCH
    
    AcpiSetupData->PowerMeterEnable = SetupData->PowerMeterEnable;

    AcpiSetupData->SystemTimeAndAlarmSource = SetupData->SystemTimeAndAlarmSource;
    AcpiSetupData->PepCio2 = SetupData->PepCio2;
    AcpiSetupData->SerialPortAcpiDebug = SetupData->SerialPortAcpiDebug;
    AcpiSetupData->DvrmVmIoExpCapabilitySupport = SetupData->DvrmVmIoExpCapabilitySupport;

    AcpiSetupData->Ac1TripPoint = SetupData->Ac1TripPoint;
    AcpiSetupData->Ac0TripPoint = SetupData->Ac0TripPoint;
    AcpiSetupData->Ac1FanSpeed = SetupData->Ac1FanSpeed;
    AcpiSetupData->Ac0FanSpeed = SetupData->Ac0FanSpeed;
    AcpiSetupData->PassiveThermalTripPoint = SetupData->PassiveThermalTripPoint;
    AcpiSetupData->CriticalThermalTripPoint = SetupData->CriticalThermalTripPoint;    
    AcpiSetupData->PassiveTc1Value = SetupData->PassiveTc1Value;
    AcpiSetupData->PassiveTc2Value = SetupData->PassiveTc2Value;
    AcpiSetupData->PassiveTspValue = SetupData->PassiveTspValue;
    AcpiSetupData->DisableActiveTripPoints = SetupData->DisableActiveTripPoints;
    AcpiSetupData->DisablePassiveTripPoints = SetupData->DisablePassiveTripPoints;
    AcpiSetupData->DisableCriticalTripPoints = SetupData->DisableCriticalTripPoints;
    AcpiSetupData->PmicVccLevel = SetupData->PmicVccLevel;
    AcpiSetupData->PmicVddqLevel = SetupData->PmicVddqLevel;
    //UINT8   TwoComponents;
    
    AcpiSetupData->ThermalDeviceEnable = SetupData->ThermalDeviceEnable;
    AcpiSetupData->CPUTempReadEnable = SetupData->CPUTempReadEnable;
    AcpiSetupData->CPUEnergyReadEnable = SetupData->CPUEnergyReadEnable;
    AcpiSetupData->PCHTempReadEnable = SetupData->PCHTempReadEnable;
    AcpiSetupData->AlertEnableLock = SetupData->AlertEnableLock;
    AcpiSetupData->PchAlert = SetupData->PchAlert;
    AcpiSetupData->DimmAlert = SetupData->DimmAlert;
    AcpiSetupData->CpuTemp = SetupData->CpuTemp;
    AcpiSetupData->CpuFanSpeed = SetupData->CpuFanSpeed;
    
#if defined (Dptf_SUPPORT) && Dptf_SUPPORT == 1
    //
    // DPTF SETUP items begin
    //
    AcpiSetupData->EnableDptf = SetupData->EnableDptf;
    AcpiSetupData->EnableDCFG = SetupData->EnableDCFG;
    
    AcpiSetupData->EnableSaDevice = SetupData->EnableSaDevice;
    AcpiSetupData->ActiveThermalTripPointSA = SetupData->ActiveThermalTripPointSA;
    AcpiSetupData->PassiveThermalTripPointSA = SetupData->PassiveThermalTripPointSA;
    AcpiSetupData->CriticalThermalTripPointSA = SetupData->CriticalThermalTripPointSA;
    AcpiSetupData->CriticalThermalTripPointSaS3 = SetupData->CriticalThermalTripPointSaS3;
    AcpiSetupData->HotThermalTripPointSA = SetupData->HotThermalTripPointSA;
    AcpiSetupData->ThermalSamplingPeriodSA = SetupData->ThermalSamplingPeriodSA;
    AcpiSetupData->PpccStepSize = SetupData->PpccStepSize;
    AcpiSetupData->LPOEnable = SetupData->LPOEnable;
    AcpiSetupData->LPOStartPState = SetupData->LPOStartPState;
    AcpiSetupData->LPOStepSize = SetupData->LPOStepSize;
    AcpiSetupData->LPOPowerControlSetting = SetupData->LPOPowerControlSetting; 
    AcpiSetupData->LPOPerformanceControlSetting = SetupData->LPOPerformanceControlSetting;
    AcpiSetupData->MinPowerLimit0 = SetupData->MinPowerLimit0; 
    AcpiSetupData->MinPowerLimit1 = SetupData->MinPowerLimit1;
    AcpiSetupData->MinPowerLimit2 = SetupData->MinPowerLimit2;
    AcpiSetupData->EnableCtdpPolicy = SetupData->EnableCtdpPolicy;
    AcpiSetupData->EnableLpmPolicy = SetupData->EnableLpmPolicy; 
    AcpiSetupData->CurrentLowPowerMode = SetupData->CurrentLowPowerMode;
    AcpiSetupData->EnableCurrentExecutionUnit = SetupData->EnableCurrentExecutionUnit;
    AcpiSetupData->TargetGfxFreq = SetupData->TargetGfxFreq;
    AcpiSetupData->EnableMemoryDevice = SetupData->EnableMemoryDevice;
    AcpiSetupData->ActiveThermalTripPointTMEM = SetupData->ActiveThermalTripPointTMEM;
    AcpiSetupData->PassiveThermalTripPointTMEM = SetupData->PassiveThermalTripPointTMEM;
    AcpiSetupData->CriticalThermalTripPointTMEM = SetupData->CriticalThermalTripPointTMEM;
    AcpiSetupData->CriticalThermalTripPointTmemS3  = SetupData->CriticalThermalTripPointTmemS3;
    AcpiSetupData->HotThermalTripPointTMEM = SetupData->HotThermalTripPointTMEM;
    AcpiSetupData->ThermalSamplingPeriodTMEM =  SetupData->ThermalSamplingPeriodTMEM;
    
    AcpiSetupData->EnableFan1Device = SetupData->EnableFan1Device;
    AcpiSetupData->EnableFan2Device = SetupData->EnableFan2Device; 
    
    AcpiSetupData->EnableDisplayParticipant = SetupData->EnableDisplayParticipant;    
    AcpiSetupData->DisplayDepthUpperLimit = SetupData->DisplayDepthUpperLimit;
    AcpiSetupData->DisplayDepthLowerLimit = SetupData->DisplayDepthLowerLimit;    
    
    AcpiSetupData->EnableWifiParticipant = SetupData->EnableWifiParticipant;
    AcpiSetupData->ActiveThermalTripPointWifi = SetupData->ActiveThermalTripPointWifi;
    AcpiSetupData->PassiveThermalTripPointWifi = SetupData->PassiveThermalTripPointWifi;
    AcpiSetupData->CriticalThermalTripPointWifi = SetupData->CriticalThermalTripPointWifi;
    AcpiSetupData->CriticalThermalTripPointWifiS3 = SetupData->CriticalThermalTripPointWifiS3;
    AcpiSetupData->HotThermalTripPointWifi = SetupData->HotThermalTripPointWifi;    

    AcpiSetupData->EnableWWANParticipant = SetupData->EnableWWANParticipant;
    AcpiSetupData->ActiveThermalTripPointWWAN = SetupData->ActiveThermalTripPointWWAN;
    AcpiSetupData->PassiveThermalTripPointWWAN = SetupData->PassiveThermalTripPointWWAN;
    AcpiSetupData->CriticalThermalTripPointWWAN = SetupData->CriticalThermalTripPointWWAN;
    AcpiSetupData->CriticalThermalTripPointWwanS3 = SetupData->CriticalThermalTripPointWwanS3;
    AcpiSetupData->HotThermalTripPointWWAN = SetupData->HotThermalTripPointWWAN;
    
    AcpiSetupData->EnableWGigParticipant = SetupData->EnableWGigParticipant;
    AcpiSetupData->ActiveThermalTripPointWGig = SetupData->ActiveThermalTripPointWGig;
    AcpiSetupData->PassiveThermalTripPointWGig = SetupData->PassiveThermalTripPointWGig;
    AcpiSetupData->CriticalThermalTripPointWGig = SetupData->CriticalThermalTripPointWGig;
    AcpiSetupData->CriticalThermalTripPointWGigS3 = SetupData->CriticalThermalTripPointWGigS3;
    AcpiSetupData->HotThermalTripPointWGig = SetupData->HotThermalTripPointWGig;
    
    AcpiSetupData->EnableChargerParticipant = SetupData->EnableChargerParticipant;
    AcpiSetupData->EnablePowerParticipant = SetupData->EnablePowerParticipant;
    AcpiSetupData->PowerParticipantPollingRate = SetupData->PowerParticipantPollingRate;
    AcpiSetupData->EnablePowerBossPolicy = SetupData->EnablePowerBossPolicy;
    AcpiSetupData->EnablePerCParticipant = SetupData->EnablePerCParticipant;

    AcpiSetupData->EnableStorageParticipant = SetupData->EnableStorageParticipant;
    AcpiSetupData->ActiveThermalTripPointStrg = SetupData->ActiveThermalTripPointStrg;
    AcpiSetupData->PassiveThermalTripPointStrg = SetupData->PassiveThermalTripPointStrg;
    AcpiSetupData->CriticalThermalTripPointStrg = SetupData->CriticalThermalTripPointStrg;
    AcpiSetupData->CriticalThermalTripPointStrgS3 = SetupData->CriticalThermalTripPointStrgS3;
    AcpiSetupData->HotThermalTripPointStrg = SetupData->HotThermalTripPointStrg;
    
    AcpiSetupData->EnableVS1Participant = SetupData->EnableVS1Participant;
    AcpiSetupData->ActiveThermalTripPointVS1 = SetupData->ActiveThermalTripPointVS1;
    AcpiSetupData->PassiveThermalTripPointVS1 = SetupData->PassiveThermalTripPointVS1;
    AcpiSetupData->CriticalThermalTripPointVS1 = SetupData->CriticalThermalTripPointVS1;
    AcpiSetupData->CriticalThermalTripPointVS1S3 = SetupData->CriticalThermalTripPointVS1S3;
    AcpiSetupData->HotThermalTripPointVS1 = SetupData->HotThermalTripPointVS1;

    AcpiSetupData->EnableVS2Participant = SetupData->EnableVS2Participant;
    AcpiSetupData->ActiveThermalTripPointVS2 = SetupData->ActiveThermalTripPointVS2;
    AcpiSetupData->PassiveThermalTripPointVS2 = SetupData->PassiveThermalTripPointVS2;
    AcpiSetupData->CriticalThermalTripPointVS2 = SetupData->CriticalThermalTripPointVS2;
    AcpiSetupData->CriticalThermalTripPointVS2S3 = SetupData->CriticalThermalTripPointVS2S3;
    AcpiSetupData->HotThermalTripPointVS2 = SetupData->HotThermalTripPointVS2;  
    
    AcpiSetupData->EnableSen1Participant = SetupData->EnableSen1Participant;
    AcpiSetupData->ActiveThermalTripPointSen1 = SetupData->ActiveThermalTripPointSen1;
    AcpiSetupData->PassiveThermalTripPointSen1 = SetupData->PassiveThermalTripPointSen1;
    AcpiSetupData->CriticalThermalTripPointSen1 = SetupData->CriticalThermalTripPointSen1;
    AcpiSetupData->CriticalThermalTripPointSen1S3 = SetupData->CriticalThermalTripPointSen1S3;
    AcpiSetupData->HotThermalTripPointSen1 = SetupData->HotThermalTripPointSen1;
    AcpiSetupData->SensorSamplingPeriodSen1 = SetupData->SensorSamplingPeriodSen1;
    
    AcpiSetupData->EnableSen2Participant = SetupData->EnableSen2Participant;
    AcpiSetupData->ActiveThermalTripPointSen2 = SetupData->ActiveThermalTripPointSen2;
    AcpiSetupData->PassiveThermalTripPointSen2 = SetupData->PassiveThermalTripPointSen2;
    AcpiSetupData->CriticalThermalTripPointSen2 = SetupData->CriticalThermalTripPointSen2;
    AcpiSetupData->CriticalThermalTripPointSen2S3 = SetupData->CriticalThermalTripPointSen2S3;
    AcpiSetupData->HotThermalTripPointSen2  = SetupData->HotThermalTripPointSen2;
    AcpiSetupData->SensorSamplingPeriodSen2 = SetupData->SensorSamplingPeriodSen2;
    
    AcpiSetupData->EnableSen3Participant = SetupData->EnableSen3Participant;
    AcpiSetupData->ActiveThermalTripPointSen3 = SetupData->ActiveThermalTripPointSen3;
    AcpiSetupData->PassiveThermalTripPointSen3 = SetupData->PassiveThermalTripPointSen3;
    AcpiSetupData->CriticalThermalTripPointSen3 = SetupData->CriticalThermalTripPointSen3;
    AcpiSetupData->CriticalThermalTripPointSen3S3 = SetupData->CriticalThermalTripPointSen3S3;
    AcpiSetupData->HotThermalTripPointSen3 = SetupData->HotThermalTripPointSen3;
    AcpiSetupData->SensorSamplingPeriodSen3 = SetupData->SensorSamplingPeriodSen3;    
    
    AcpiSetupData->EnableSen4Participant = SetupData->EnableSen4Participant;
    AcpiSetupData->ActiveThermalTripPointSen4 = SetupData->ActiveThermalTripPointSen4;
    AcpiSetupData->PassiveThermalTripPointSen4 = SetupData->PassiveThermalTripPointSen4;
    AcpiSetupData->CriticalThermalTripPointSen4 = SetupData->CriticalThermalTripPointSen4;
    AcpiSetupData->CriticalThermalTripPointSen4S3 = SetupData->CriticalThermalTripPointSen4S3;
    AcpiSetupData->HotThermalTripPointSen4 = SetupData->HotThermalTripPointSen4;
    AcpiSetupData->SensorSamplingPeriodSen4 = SetupData->SensorSamplingPeriodSen4;    
    
    AcpiSetupData->EnableSen5Participant = SetupData->EnableSen5Participant;
    AcpiSetupData->ActiveThermalTripPointSen5 = SetupData->ActiveThermalTripPointSen5;
    AcpiSetupData->PassiveThermalTripPointSen5 = SetupData->PassiveThermalTripPointSen5;
    AcpiSetupData->CriticalThermalTripPointSen5 = SetupData->CriticalThermalTripPointSen5;
    AcpiSetupData->CriticalThermalTripPointSen5S3 = SetupData->CriticalThermalTripPointSen5S3;
    AcpiSetupData->HotThermalTripPointSen5 = SetupData->HotThermalTripPointSen5;
    AcpiSetupData->SensorSamplingPeriodSen5 = SetupData->SensorSamplingPeriodSen5;
    
    //UINT8   EnableSen6Participant;
    //UINT8   ActiveThermalTripPointSen6;
    //UINT8   PassiveThermalTripPointSen6;
    //UINT8   CriticalThermalTripPointSen6;
    //UINT8   CriticalThermalTripPointSen6S3;
    //UINT8   HotThermalTripPointSen6;
    //UINT8   SensorSamplingPeriodSen6;

    //UINT8   EnableSen7Participant;
    //UINT8   ActiveThermalTripPointSen7;
    //UINT8   PassiveThermalTripPointSen7;
    //UINT8   CriticalThermalTripPointSen7;
    //UINT8   CriticalThermalTripPointSen7S3;
    //UINT8   HotThermalTripPointSen7;
    //UINT8   SensorSamplingPeriodSen7;

    //UINT8   EnableSen8Participant;
    //UINT8   ActiveThermalTripPointSen8;
    //UINT8   PassiveThermalTripPointSen8;
    //UINT8   CriticalThermalTripPointSen8;
    //UINT8   CriticalThermalTripPointSen8S3;
    //UINT8   HotThermalTripPointSen8;
    //UINT8   SensorSamplingPeriodSen8;
    
    AcpiSetupData->EnableGen1Participant = SetupData->EnableGen1Participant;
    AcpiSetupData->ActiveThermalTripPointGen1 = SetupData->ActiveThermalTripPointGen1;
    AcpiSetupData->PassiveThermalTripPointGen1 = SetupData->PassiveThermalTripPointGen1;
    AcpiSetupData->CriticalThermalTripPointGen1 = SetupData->CriticalThermalTripPointGen1;
    AcpiSetupData->CriticalThermalTripPointGen1S3 = SetupData->CriticalThermalTripPointGen1S3;
    AcpiSetupData->HotThermalTripPointGen1 = SetupData->HotThermalTripPointGen1;
    AcpiSetupData->ThermistorSamplingPeriodGen1 = SetupData->ThermistorSamplingPeriodGen1;
    
    AcpiSetupData->EnableGen2Participant = SetupData->EnableGen2Participant;
    AcpiSetupData->ActiveThermalTripPointGen2 = SetupData->ActiveThermalTripPointGen2;
    AcpiSetupData->PassiveThermalTripPointGen2 = SetupData->PassiveThermalTripPointGen2;
    AcpiSetupData->CriticalThermalTripPointGen2 = SetupData->CriticalThermalTripPointGen2;
    AcpiSetupData->CriticalThermalTripPointGen2S3 = SetupData->CriticalThermalTripPointGen2S3;
    AcpiSetupData->HotThermalTripPointGen2 = SetupData->HotThermalTripPointGen2;
    AcpiSetupData->ThermistorSamplingPeriodGen2 = SetupData->ThermistorSamplingPeriodGen2;
    
    AcpiSetupData->EnableGen3Participant = SetupData->EnableGen3Participant;
    AcpiSetupData->ActiveThermalTripPointGen3 = SetupData->ActiveThermalTripPointGen3;
    AcpiSetupData->PassiveThermalTripPointGen3 = SetupData->PassiveThermalTripPointGen3;
    AcpiSetupData->CriticalThermalTripPointGen3 = SetupData->CriticalThermalTripPointGen3;
    AcpiSetupData->CriticalThermalTripPointGen3S3 = SetupData->CriticalThermalTripPointGen3S3;
    AcpiSetupData->HotThermalTripPointGen3 = SetupData->HotThermalTripPointGen3;
    AcpiSetupData->ThermistorSamplingPeriodGen3 = SetupData->ThermistorSamplingPeriodGen3;
    
    AcpiSetupData->EnableGen4Participant = SetupData->EnableGen4Participant;
    AcpiSetupData->ActiveThermalTripPointGen4 = SetupData->ActiveThermalTripPointGen4;
    AcpiSetupData->PassiveThermalTripPointGen4 = SetupData->PassiveThermalTripPointGen4;
    AcpiSetupData->CriticalThermalTripPointGen4 = SetupData->CriticalThermalTripPointGen4;
    AcpiSetupData->CriticalThermalTripPointGen4S3 = SetupData->CriticalThermalTripPointGen4S3;
    AcpiSetupData->HotThermalTripPointGen4 = SetupData->HotThermalTripPointGen4;
    AcpiSetupData->ThermistorSamplingPeriodGen4 = SetupData->ThermistorSamplingPeriodGen4;
    
    AcpiSetupData->EnableGen5Participant = SetupData->EnableGen5Participant;
    AcpiSetupData->ActiveThermalTripPointGen5 = SetupData->ActiveThermalTripPointGen5;
    AcpiSetupData->PassiveThermalTripPointGen5 = SetupData->PassiveThermalTripPointGen5;
    AcpiSetupData->CriticalThermalTripPointGen5 = SetupData->CriticalThermalTripPointGen5;
    AcpiSetupData->CriticalThermalTripPointGen5S3 = SetupData->CriticalThermalTripPointGen5S3;
    AcpiSetupData->HotThermalTripPointGen5 = SetupData->HotThermalTripPointGen5;
    AcpiSetupData->ThermistorSamplingPeriodGen5 = SetupData->ThermistorSamplingPeriodGen5;    

    //UINT8   EnableGen6Participant;
    //UINT8   ActiveThermalTripPointGen6;
    //UINT8   PassiveThermalTripPointGen6;
    //UINT8   CriticalThermalTripPointGen6;
    //UINT8   CriticalThermalTripPointGen6S3;
    //UINT8   HotThermalTripPointGen6;
    //UINT8   ThermistorSamplingPeriodGen6;

    //UINT8   EnableGen7Participant;
    //UINT8   ActiveThermalTripPointGen7;
    //UINT8   PassiveThermalTripPointGen7;
    //UINT8   CriticalThermalTripPointGen7;
    //UINT8   CriticalThermalTripPointGen7S3;
    //UINT8   HotThermalTripPointGen7;
    //UINT8   ThermistorSamplingPeriodGen7;

    //UINT8   EnableGen8Participant;
    //UINT8   ActiveThermalTripPointGen8;
    //UINT8   PassiveThermalTripPointGen8;
    //UINT8   CriticalThermalTripPointGen8;
    //UINT8   CriticalThermalTripPointGen8S3;
    //UINT8   HotThermalTripPointGen8;
    //UINT8   ThermistorSamplingPeriodGen8;
    
    AcpiSetupData->EnableActivePolicy = SetupData->EnableActivePolicy;
    AcpiSetupData->EnablePassivePolicy = SetupData->EnablePassivePolicy;
    AcpiSetupData->TrtRevision = SetupData->TrtRevision;
    AcpiSetupData->EnableCriticalPolicy = SetupData->EnableCriticalPolicy;
    AcpiSetupData->EnableCoolingModePolicy = SetupData->EnableCoolingModePolicy;    
    AcpiSetupData->EnableACTPolicy = SetupData->EnableACTPolicy;
    AcpiSetupData->EnableHDCPolicy = SetupData->EnableHDCPolicy; 
    AcpiSetupData->EnableAPPolicy = SetupData->EnableAPPolicy;
    AcpiSetupData->EnableVSPolicy = SetupData->EnableVSPolicy;
    
    AcpiSetupData->PpccObject = SetupData->PpccObject;
    AcpiSetupData->PdrtObject = SetupData->PdrtObject;
    AcpiSetupData->ArtgObject = SetupData->ArtgObject;
    AcpiSetupData->PmaxObject = SetupData->PmaxObject;
    AcpiSetupData->Tmp1Object = SetupData->Tmp1Object;
    AcpiSetupData->Tmp2Object = SetupData->Tmp2Object;
    AcpiSetupData->Tmp3Object = SetupData->Tmp3Object;
    AcpiSetupData->Tmp4Object = SetupData->Tmp4Object;
    AcpiSetupData->Tmp5Object = SetupData->Tmp5Object;
    AcpiSetupData->Tmp6Object = SetupData->Tmp6Object;
    AcpiSetupData->Tmp7Object = SetupData->Tmp7Object;
    AcpiSetupData->Tmp8Object = SetupData->Tmp8Object;
    AcpiSetupData->OptionalObjects = SetupData->OptionalObjects; 
    
    AcpiSetupData->OemDesignVariable0 = SetupData->OemDesignVariable0;
    AcpiSetupData->OemDesignVariable1 = SetupData->OemDesignVariable1;
    AcpiSetupData->OemDesignVariable2 = SetupData->OemDesignVariable2;
    AcpiSetupData->OemDesignVariable3 = SetupData->OemDesignVariable3;
    AcpiSetupData->OemDesignVariable4 = SetupData->OemDesignVariable4;
    AcpiSetupData->OemDesignVariable5 = SetupData->OemDesignVariable5; 
//
// KblGBegin
//
  // Discrete Graphics Power Share Participants

    AcpiSetupData->EnableDGFCParticipant = SetupData->EnableDGFCParticipant;
    AcpiSetupData->ActiveThermalTripPointDGFC = SetupData->ActiveThermalTripPointDGFC;
    AcpiSetupData->PassiveThermalTripPointDGFC = SetupData->PassiveThermalTripPointDGFC;
    AcpiSetupData->CriticalThermalTripPointDGFC = SetupData->CriticalThermalTripPointDGFC;
    AcpiSetupData->CriticalThermalTripPointDGFCS3 = SetupData->CriticalThermalTripPointDGFCS3;
    AcpiSetupData->HotThermalTripPointDGFC = SetupData->HotThermalTripPointDGFC;
    AcpiSetupData->SensorSamplingPeriodDGFC = SetupData->SensorSamplingPeriodDGFC;
    AcpiSetupData->PpccStepSizeDGFC = SetupData->PpccStepSizeDGFC;
    AcpiSetupData->MinPowerLimit0DGFC = SetupData->MinPowerLimit0DGFC;

    AcpiSetupData->EnableDGHMParticipant = SetupData->EnableDGHMParticipant;
    AcpiSetupData->ActiveThermalTripPointDGHM = SetupData->ActiveThermalTripPointDGHM;
    AcpiSetupData->PassiveThermalTripPointDGHM = SetupData->PassiveThermalTripPointDGHM;
    AcpiSetupData->CriticalThermalTripPointDGHM = SetupData->CriticalThermalTripPointDGHM;
    AcpiSetupData->CriticalThermalTripPointDGHMS3 = SetupData->CriticalThermalTripPointDGHMS3;
    AcpiSetupData->HotThermalTripPointDGHM = SetupData->HotThermalTripPointDGHM;
    AcpiSetupData->SensorSamplingPeriodDGHM = SetupData->SensorSamplingPeriodDGHM;

    AcpiSetupData->EnableMCPParticipant = SetupData->EnableMCPParticipant;
    AcpiSetupData->PpccStepSizeMCP = SetupData->PpccStepSizeMCP;
    AcpiSetupData->MinPowerLimit0MCP = SetupData->MinPowerLimit0MCP;
//
// KblGEnd
//    
    
    //
    // DPTF SETUP items end
    //
#endif
    
#if defined (RTD3_SUPPORT) && RTD3_SUPPORT == 1  
    AcpiSetupData->Rtd3Support = SetupData->Rtd3Support;
    AcpiSetupData->Rtd3P0dl = SetupData->Rtd3P0dl;
    AcpiSetupData->Rtd3P3dl = SetupData->Rtd3P3dl;
    AcpiSetupData->Rtd3UsbPt1 = SetupData->Rtd3UsbPt1;
    AcpiSetupData->Rtd3UsbPt2 = SetupData->Rtd3UsbPt2;
    AcpiSetupData->Rtd3I2C0SensorHub = SetupData->Rtd3I2C0SensorHub;
    AcpiSetupData->Rtd3AudioDelay = SetupData->Rtd3AudioDelay;
    AcpiSetupData->Rtd3TouchPadDelay = SetupData->Rtd3TouchPadDelay;
    AcpiSetupData->Rtd3TouchPanelDelay = SetupData->Rtd3TouchPanelDelay;    
    AcpiSetupData->Rtd3SensorHub = SetupData->Rtd3SensorHub;
    AcpiSetupData->VRStaggeringDelay = SetupData->VRStaggeringDelay;
    AcpiSetupData->VRRampUpDelay = SetupData->VRRampUpDelay;
    AcpiSetupData->PstateCapping = SetupData->PstateCapping;
    AcpiSetupData->Rtd3ZPODD = SetupData->Rtd3ZPODD;    
    AcpiSetupData->Rtd3Camera = SetupData->Rtd3Camera;
    AcpiSetupData->Rtd3WWAN = SetupData->Rtd3WWAN; 
    AcpiSetupData->Rtd3SataPort0 = SetupData->Rtd3SataPort0;    
    AcpiSetupData->Rtd3SataPort1 = SetupData->Rtd3SataPort1;
    AcpiSetupData->Rtd3SataPort2 = SetupData->Rtd3SataPort2;    
    AcpiSetupData->Rtd3SataPort3 = SetupData->Rtd3SataPort3;
    AcpiSetupData->Rtd3SataPort4 = SetupData->Rtd3SataPort4;     
    AcpiSetupData->Rtd3WaGpio = SetupData->Rtd3WaGpio;
    AcpiSetupData->Rtd3RemapCR1 = SetupData->Rtd3RemapCR1;    
    AcpiSetupData->Rtd3RemapCR2 = SetupData->Rtd3RemapCR2;    
    AcpiSetupData->Rtd3RemapCR3 = SetupData->Rtd3RemapCR3;
    AcpiSetupData->Rtd3RaidVolumes = SetupData->Rtd3RaidVolumes;    
    AcpiSetupData->Rtd3I2C0ControllerPS0Delay = SetupData->Rtd3I2C0ControllerPS0Delay;
    AcpiSetupData->Rtd3I2C1ControllerPS0Delay = SetupData->Rtd3I2C1ControllerPS0Delay;    
    AcpiSetupData->ConsolidatedPR = SetupData->ConsolidatedPR;    
    AcpiSetupData->Rtd3PcieNvme = SetupData->Rtd3PcieNvme;
    AcpiSetupData->Rtd3I2CTouchPanel = SetupData->Rtd3I2CTouchPanel;
#endif    

    AcpiSetupData->PowerSharingManagerEnable = SetupData->PowerSharingManagerEnable;
    AcpiSetupData->PsmSplcDomainType1 = SetupData->PsmSplcDomainType1;
    AcpiSetupData->PsmSplcPowerLimit1 = SetupData->PsmSplcPowerLimit1;
    AcpiSetupData->PsmSplcTimeWindow1 = SetupData->PsmSplcTimeWindow1;
    AcpiSetupData->PsmSplcDomainType2 = SetupData->PsmSplcDomainType2;
    AcpiSetupData->PsmSplcPowerLimit2 = SetupData->PsmSplcPowerLimit2;
    AcpiSetupData->PsmSplcTimeWindow2 = SetupData->PsmSplcTimeWindow2;
    
    AcpiSetupData->PsmDplcDomainType1 = SetupData->PsmDplcDomainType1;
    AcpiSetupData->PsmDplcDomainPreference1 = SetupData->PsmDplcDomainPreference1;
    AcpiSetupData->PsmDplcPowerLimitIndex1 = SetupData->PsmDplcPowerLimitIndex1;
    AcpiSetupData->PsmDplcDefaultPowerLimit1 = SetupData->PsmDplcDefaultPowerLimit1;
    AcpiSetupData->PsmDplcDefaultTimeWindow1 = SetupData->PsmDplcDefaultTimeWindow1;
    AcpiSetupData->PsmDplcMinimumPowerLimit1 = SetupData->PsmDplcMinimumPowerLimit1;
    AcpiSetupData->PsmDplcMaximumPowerLimit1 = SetupData->PsmDplcMaximumPowerLimit1;
    AcpiSetupData->PsmDplcMaximumTimeWindow1 = SetupData->PsmDplcMaximumTimeWindow1;    
    
    AcpiSetupData->PsmDplcDomainType2 = SetupData->PsmDplcDomainType2;
    AcpiSetupData->PsmDplcDomainPreference2 = SetupData->PsmDplcDomainPreference2;
    AcpiSetupData->PsmDplcPowerLimitIndex2 = SetupData->PsmDplcPowerLimitIndex2;
    AcpiSetupData->PsmDplcDefaultPowerLimit2 = SetupData->PsmDplcDefaultPowerLimit2;
    AcpiSetupData->PsmDplcDefaultTimeWindow2 = SetupData->PsmDplcDefaultTimeWindow2;
    AcpiSetupData->PsmDplcMinimumPowerLimit2 = SetupData->PsmDplcMinimumPowerLimit2;
    AcpiSetupData->PsmDplcMaximumPowerLimit2 = SetupData->PsmDplcMaximumPowerLimit2;
    AcpiSetupData->PsmDplcMaximumTimeWindow2 = SetupData->PsmDplcMaximumTimeWindow2;     
    
    AcpiSetupData->WifiEnable = SetupData->WifiEnable;
    AcpiSetupData->WifiDomainType1 = SetupData->WifiDomainType1;
    AcpiSetupData->WifiPowerLimit1 = SetupData->WifiPowerLimit1;
    AcpiSetupData->WifiTimeWindow1  = SetupData->WifiTimeWindow1;
    AcpiSetupData->WifiDomainType2 = SetupData->WifiDomainType2;
    AcpiSetupData->WifiPowerLimit2 = SetupData->WifiPowerLimit2;
    AcpiSetupData->WifiTimeWindow2 = SetupData->WifiTimeWindow2; 
    AcpiSetupData->WifiDomainType3 = SetupData->WifiDomainType3;
    AcpiSetupData->WifiPowerLimit3 = SetupData->WifiPowerLimit3;
    AcpiSetupData->WifiTimeWindow3 = SetupData->WifiTimeWindow3;    
    AcpiSetupData->TRxDelay0 = SetupData->TRxDelay0;
    AcpiSetupData->TRxCableLength0 = SetupData->TRxCableLength0;
    AcpiSetupData->TRxDelay1 = SetupData->TRxDelay1;
    AcpiSetupData->TRxCableLength1 = SetupData->TRxCableLength1;
    AcpiSetupData->WrddDomainType1 = SetupData->WrddDomainType1;
    AcpiSetupData->WrddCountryIndentifier1 = SetupData->WrddCountryIndentifier1;
    AcpiSetupData->WrddDomainType2 = SetupData->WrddDomainType2;
    AcpiSetupData->WrddCountryIndentifier2 = SetupData->WrddCountryIndentifier2;    
    AcpiSetupData->WrdsWiFiSarEnable= SetupData->WrdsWiFiSarEnable;
    AcpiSetupData->WrdsWiFiSarTxPowerLimit1= SetupData->WrdsWiFiSarTxPowerLimit1;
    AcpiSetupData->WrdsWiFiSarTxPowerLimit2= SetupData->WrdsWiFiSarTxPowerLimit2;
    AcpiSetupData->WrdsWiFiSarTxPowerLimit3= SetupData->WrdsWiFiSarTxPowerLimit3;
    AcpiSetupData->WrdsWiFiSarTxPowerLimit4= SetupData->WrdsWiFiSarTxPowerLimit4;
    AcpiSetupData->WrdsWiFiSarTxPowerLimit5= SetupData->WrdsWiFiSarTxPowerLimit5;
    AcpiSetupData->WrdsWiFiSarTxPowerLimit6= SetupData->WrdsWiFiSarTxPowerLimit6;
    AcpiSetupData->WrdsWiFiSarTxPowerLimit7= SetupData->WrdsWiFiSarTxPowerLimit7;
    AcpiSetupData->WrdsWiFiSarTxPowerLimit8= SetupData->WrdsWiFiSarTxPowerLimit8;
    AcpiSetupData->WrdsWiFiSarTxPowerLimit9= SetupData->WrdsWiFiSarTxPowerLimit9;
    AcpiSetupData->WrdsWiFiSarTxPowerLimit10= SetupData->WrdsWiFiSarTxPowerLimit10;
    AcpiSetupData->WigigRfe= SetupData->WigigRfe;
    AcpiSetupData->WiGigRfeCh1= SetupData->WiGigRfeCh1;
    AcpiSetupData->WiGigRfeCh2= SetupData->WiGigRfeCh2;
    AcpiSetupData->WiGigRfeCh3= SetupData->WiGigRfeCh3;
    AcpiSetupData->WiGigRfeCh4= SetupData->WiGigRfeCh4;
    AcpiSetupData->AwvClassIndex= SetupData->AwvClassIndex;    
    
    AcpiSetupData->ThermalSamplingPeriodWifi= SetupData->ThermalSamplingPeriodWifi;
    AcpiSetupData->WakeOnWiGigSupport= SetupData->WakeOnWiGigSupport; 
    
    AcpiSetupData->EnablePIDPolicy= SetupData->EnablePIDPolicy;
    AcpiSetupData->EnablePowerSharePolicy= SetupData->EnablePowerSharePolicy;  // KblGContent
    AcpiSetupData->PepGbe= SetupData->PepGbe;
    AcpiSetupData->LpitRs2Wa= SetupData->LpitRs2Wa;

    AcpiSetupData->WgdsWiFiSarDeltaGroup1PowerMax1    = SetupData->WgdsWiFiSarDeltaGroup1PowerMax1;
    AcpiSetupData->WgdsWiFiSarDeltaGroup1PowerChainA1 = SetupData->WgdsWiFiSarDeltaGroup1PowerChainA1;
    AcpiSetupData->WgdsWiFiSarDeltaGroup1PowerChainB1 = SetupData->WgdsWiFiSarDeltaGroup1PowerChainB1;
    AcpiSetupData->WgdsWiFiSarDeltaGroup1PowerMax2    = SetupData->WgdsWiFiSarDeltaGroup1PowerMax2;
    AcpiSetupData->WgdsWiFiSarDeltaGroup1PowerChainA2 = SetupData->WgdsWiFiSarDeltaGroup1PowerChainA2;
    AcpiSetupData->WgdsWiFiSarDeltaGroup1PowerChainB2 = SetupData->WgdsWiFiSarDeltaGroup1PowerChainB2;
    AcpiSetupData->WgdsWiFiSarDeltaGroup2PowerMax1    = SetupData->WgdsWiFiSarDeltaGroup2PowerMax1;
    AcpiSetupData->WgdsWiFiSarDeltaGroup2PowerChainA1 = SetupData->WgdsWiFiSarDeltaGroup2PowerChainA1;
    AcpiSetupData->WgdsWiFiSarDeltaGroup2PowerChainB1 = SetupData->WgdsWiFiSarDeltaGroup2PowerChainB1;
    AcpiSetupData->WgdsWiFiSarDeltaGroup2PowerMax2    = SetupData->WgdsWiFiSarDeltaGroup2PowerMax2;
    AcpiSetupData->WgdsWiFiSarDeltaGroup2PowerChainA2 = SetupData->WgdsWiFiSarDeltaGroup2PowerChainA2;
    AcpiSetupData->WgdsWiFiSarDeltaGroup2PowerChainB2 = SetupData->WgdsWiFiSarDeltaGroup2PowerChainB2;
    AcpiSetupData->WgdsWiFiSarDeltaGroup3PowerMax1    = SetupData->WgdsWiFiSarDeltaGroup3PowerMax1;
    AcpiSetupData->WgdsWiFiSarDeltaGroup3PowerChainA1 = SetupData->WgdsWiFiSarDeltaGroup3PowerChainA1;
    AcpiSetupData->WgdsWiFiSarDeltaGroup3PowerChainB1 = SetupData->WgdsWiFiSarDeltaGroup3PowerChainB1;
    AcpiSetupData->WgdsWiFiSarDeltaGroup3PowerMax2    = SetupData->WgdsWiFiSarDeltaGroup3PowerMax2;
    AcpiSetupData->WgdsWiFiSarDeltaGroup3PowerChainA2 = SetupData->WgdsWiFiSarDeltaGroup3PowerChainA2;
    AcpiSetupData->WgdsWiFiSarDeltaGroup3PowerChainB2 = SetupData->WgdsWiFiSarDeltaGroup3PowerChainB2;

    AcpiSetupData->BluetoothSar = SetupData->BluetoothSar;
    if (AcpiSetupData->BluetoothSar == 1) {
      AcpiSetupData->BluetoothSarBr     = SetupData->BluetoothSarBr;
      AcpiSetupData->BluetoothSarEdr2   = SetupData->BluetoothSarEdr2;
      AcpiSetupData->BluetoothSarEdr3   = SetupData->BluetoothSarEdr3;
      AcpiSetupData->BluetoothSarLe     = SetupData->BluetoothSarLe;
      AcpiSetupData->BluetoothSarLe2Mhz = SetupData->BluetoothSarLe2Mhz;
      AcpiSetupData->BluetoothSarLeLr   = SetupData->BluetoothSarLeLr;
    } else {
      AcpiSetupData->BluetoothSarBr     = 0x0;
      AcpiSetupData->BluetoothSarEdr2   = 0x0;
      AcpiSetupData->BluetoothSarEdr3   = 0x0;
      AcpiSetupData->BluetoothSarLe     = 0x0;
    }
    
    AcpiSetupData->PepTbt= SetupData->PepTbt;
    
    AcpiSetupData->SkyCamControlLogic0Wled1FlashMaxCurrent= SetupData->SkyCamControlLogic0Wled1FlashMaxCurrent;
    AcpiSetupData->SkyCamControlLogic0Wled1TorchMaxCurrent= SetupData->SkyCamControlLogic0Wled1TorchMaxCurrent;
    AcpiSetupData->SkyCamControlLogic0Wled2FlashMaxCurrent= SetupData->SkyCamControlLogic0Wled2FlashMaxCurrent;
    AcpiSetupData->SkyCamControlLogic0Wled2TorchMaxCurrent= SetupData->SkyCamControlLogic0Wled2TorchMaxCurrent;
    AcpiSetupData->SkyCamControlLogic0Wled1Type= SetupData->SkyCamControlLogic0Wled1Type;
    AcpiSetupData->SkyCamControlLogic0Wled2Type= SetupData->SkyCamControlLogic0Wled2Type; 
    AcpiSetupData->SkyCamControlLogic1Wled1FlashMaxCurrent= SetupData->SkyCamControlLogic1Wled1FlashMaxCurrent;
    AcpiSetupData->SkyCamControlLogic1Wled1TorchMaxCurrent= SetupData->SkyCamControlLogic1Wled1TorchMaxCurrent;
    AcpiSetupData->SkyCamControlLogic1Wled2FlashMaxCurrent= SetupData->SkyCamControlLogic1Wled2FlashMaxCurrent;
    AcpiSetupData->SkyCamControlLogic1Wled2TorchMaxCurrent= SetupData->SkyCamControlLogic1Wled2TorchMaxCurrent;
    AcpiSetupData->SkyCamControlLogic1Wled1Type= SetupData->SkyCamControlLogic1Wled1Type;
    AcpiSetupData->SkyCamControlLogic1Wled2Type= SetupData->SkyCamControlLogic1Wled2Type; 
    AcpiSetupData->SkyCamControlLogic2Wled1FlashMaxCurrent= SetupData->SkyCamControlLogic2Wled1FlashMaxCurrent;
    AcpiSetupData->SkyCamControlLogic2Wled1TorchMaxCurrent= SetupData->SkyCamControlLogic2Wled1TorchMaxCurrent;
    AcpiSetupData->SkyCamControlLogic2Wled2FlashMaxCurrent= SetupData->SkyCamControlLogic2Wled2FlashMaxCurrent;
    AcpiSetupData->SkyCamControlLogic2Wled2TorchMaxCurrent= SetupData->SkyCamControlLogic2Wled2TorchMaxCurrent;
    AcpiSetupData->SkyCamControlLogic2Wled1Type= SetupData->SkyCamControlLogic2Wled1Type;
    AcpiSetupData->SkyCamControlLogic2Wled2Type= SetupData->SkyCamControlLogic2Wled2Type;
    AcpiSetupData->SkyCamControlLogic3Wled1FlashMaxCurrent= SetupData->SkyCamControlLogic3Wled1FlashMaxCurrent;
    AcpiSetupData->SkyCamControlLogic3Wled1TorchMaxCurrent= SetupData->SkyCamControlLogic3Wled1TorchMaxCurrent;
    AcpiSetupData->SkyCamControlLogic3Wled2FlashMaxCurrent= SetupData->SkyCamControlLogic3Wled2FlashMaxCurrent;
    AcpiSetupData->SkyCamControlLogic3Wled2TorchMaxCurrent= SetupData->SkyCamControlLogic3Wled2TorchMaxCurrent;
    AcpiSetupData->SkyCamControlLogic3Wled1Type= SetupData->SkyCamControlLogic3Wled1Type;
    AcpiSetupData->SkyCamControlLogic3Wled2Type= SetupData->SkyCamControlLogic3Wled2Type;
    AcpiSetupData->SkyCamLink0DriverDataPrivacyLed= SetupData->SkyCamLink0DriverDataPrivacyLed;
    AcpiSetupData->SkyCamLink0DriverDataPmicPosition= SetupData->SkyCamLink0DriverDataPmicPosition;
    AcpiSetupData->SkyCamLink0DriverDataVoltageRail= SetupData->SkyCamLink0DriverDataVoltageRail;
    AcpiSetupData->SkyCamLink1DriverDataPrivacyLed= SetupData->SkyCamLink1DriverDataPrivacyLed;
    AcpiSetupData->SkyCamLink1DriverDataPmicPosition= SetupData->SkyCamLink1DriverDataPmicPosition;
    AcpiSetupData->SkyCamLink1DriverDataVoltageRail= SetupData->SkyCamLink1DriverDataVoltageRail;
    AcpiSetupData->SkyCamLink2DriverDataPrivacyLed= SetupData->SkyCamLink2DriverDataPrivacyLed;
    AcpiSetupData->SkyCamLink2DriverDataPmicPosition= SetupData->SkyCamLink2DriverDataPmicPosition;
    AcpiSetupData->SkyCamLink2DriverDataVoltageRail= SetupData->SkyCamLink2DriverDataVoltageRail;
    AcpiSetupData->SkyCamLink3DriverDataPrivacyLed= SetupData->SkyCamLink3DriverDataPrivacyLed;
    AcpiSetupData->SkyCamLink3DriverDataPmicPosition= SetupData->SkyCamLink3DriverDataPmicPosition;
    AcpiSetupData->SkyCamLink3DriverDataVoltageRail= SetupData->SkyCamLink3DriverDataVoltageRail;    
    
    AcpiSetupData->CameraType = SetupData->CameraType;
    AcpiSetupData->IvcamDfuSupport = SetupData->IvcamDfuSupport;
    AcpiSetupData->IvcamWakeSupport = SetupData->IvcamWakeSupport;
    AcpiSetupData->IvcamPowerOnDelay = SetupData->IvcamPowerOnDelay;
    AcpiSetupData->IvcamPowerOffDelay = SetupData->IvcamPowerOffDelay;
    AcpiSetupData->IvcamRotation = SetupData->IvcamRotation;
    AcpiSetupData->DsPowerOnDelay = SetupData->DsPowerOnDelay;
    AcpiSetupData->DsPowerOffDelay = SetupData->DsPowerOffDelay;
    AcpiSetupData->DsRotation = SetupData->DsRotation;
    AcpiSetupData->AudioConnector = SetupData->AudioConnector;
    AcpiSetupData->WirelessCharging = SetupData->WirelessCharging;
    AcpiSetupData->UsbFnEnable = SetupData->UsbFnEnable;
    
    AcpiSetupData->EnableEInkDfu = SetupData->EnableEInkDfu;
    
    //UINT8   CsmControl;    //Not a setup item.
    
    //UINT8   FanControl;    //Not use.
    //UINT8   CpuFanControl;    //Not use.
    //UINT8   LowestFanSpeed;    //Not use.

#if defined Thunderbolt_SUPPORT && Thunderbolt_SUPPORT == 1
    //
    // Thunderbolt(TM)
    //
    AcpiSetupData->TbtSupport = SetupData->TbtSupport;
    //UINT8   TbtWakeupSupport;
    //UINT8   ReserveBusForDGfx;
    //UINT16  ReserveMemoryForDGfx;
    //UINT16  ReservePMemoryForDGfx;
    //UINT8   TbtPcieSupport;
    //UINT8   TbtDetachableGfxSupport;     ///< 1=Supported; 0=Not Supported
    //UINT8   TbtUsbOn;
    //UINT8   TbtBootOn;
    //UINT8   AicArSupport;
    //UINT8   AicSupport;
    //UINT8   TbtCacheLineSize;
    //UINT8   Gpio3ForcePwr;
    //UINT16  Gpio3ForcePwrDly;
    //UINT8   SecurityMode;
    //UINT8   CallSmiBeforeBoot;
    //UINT32  TbtDgpuPciBaseAddressMem; // Thunderbolt External GPU PCI Configuration Space Buffer Address
    AcpiSetupData->TbtDebugBaseAddressMem = SetupData->TbtDebugBaseAddressMem; 
    //UINT32  TbtDeviceTreeBuffer;      // Thunderbolt Device Tree Info Buffer Address
    //UINT8   CardSelected;
    //UINT16  ReserveMemoryPerSlot;
    //UINT16  ReservePMemoryPerSlot;
    //UINT8   ReserveIoPerSlot;
    //UINT8   AssignIo;
    //UINT8   MoreIOForPorts;
    //UINT8   AssignPMem;
    //UINT8   TbtNativeOsHotPlug;
    //UINT8   TBTHotSMI;
    //UINT8   Gpio5Filter;
    //UINT8   TBTHotNotify;
    //UINT8   EnableMsiInFadt;
    //UINT8   TbtSelector;
    //UINT8   TBTSetClkReq;
    //UINT8   TbtAspm;
    //UINT8   TbtLtr;
    //UINT8   TBThostRouter;
    //UINT8   TbtPcieExtraBusRsvd;
    //UINT16  TbtPcieMemRsvd;
    //UINT8   TbtPcieIoRsvd;
    //UINT8   TbtPcieMemAddrRngMax;
    //UINT16  TbtPciePMemRsvd;
    //UINT8   TbtPciePMemAddrRngMax;
    //UINT8   TbtSkipPciEnumeration;
    //UINT8   TbtSkipPciOprom;
    //UINT8   TbtSkipIntLine;
    //UINT8   TbtAcpiRemovalSupport;
    AcpiSetupData->EnableMsiInFadt = SetupData->EnableMsiInFadt;
// AMI_OVERRIDE_START - Removed due to Intel doesn't verify it yet.
#if 0
    AcpiSetupData->TbtSelector1 = SetupData->TbtSelector1; 
    AcpiSetupData->Multicontroller = SetupData->Multicontroller;     
#endif
// AMI_OVERRIDE_END - Removed due to Intel doesn't verify it yet.
#endif
    
    //UINT8   IccWdtEnabled;
    //UINT8   IccLockRegisters;
    //UINT8   IccProfile;
    //UINT8   ShowProfile;
    //UINT8   NumProfiles;
    //UINT8   IccPllShutdownEnabled;

    //UINT8   CsModeFirst;    //Not use.
    //UINT8   CsMode;    //Not use.
    //UINT8   CsModeChanged;    //Not use.

    //UINT8   PlatformUnstable;    //OverClock
    //UINT8   IsOcDefaultsInitalized;    //OverClock

    //UINT8   SecureEraseModeRealMode;    //Amt
    //UINT8   ForceSecureErase    //Amt;

    AcpiSetupData->HidEventFilterDriverEnable =  SetupData->HidEventFilterDriverEnable;

    AcpiSetupData->Ssic7160and7260InitSetup =  SetupData->Ssic7160and7260InitSetup;

    //UINT8   TestMctpBroadcastCycle;

    AcpiSetupData->SlpS0VmSupport =  SetupData->SlpS0VmSupport;
    AcpiSetupData->UsbTypeCAicSupport =  SetupData->UsbTypeCAicSupport;
    AcpiSetupData->HebcValue =  SetupData->HebcValue;

    AcpiSetupData->PepStorageControllers = SetupData->PepStorageControllers;
    AcpiSetupData->PepSataPort0 = SetupData->PepSataPort0;
    AcpiSetupData->PepSataPort1 = SetupData->PepSataPort1;
    AcpiSetupData->PepSataPort2 = SetupData->PepSataPort2;
    AcpiSetupData->PepSataPort3 = SetupData->PepSataPort3;
    AcpiSetupData->PepSataPort4 = SetupData->PepSataPort4;
    AcpiSetupData->PepSataPort5 = SetupData->PepSataPort5;
    AcpiSetupData->PepSataNvm1 = SetupData->PepSataNvm1;
    AcpiSetupData->PepSataNvm2 = SetupData->PepSataNvm2;
    AcpiSetupData->PepSataNvm3 = SetupData->PepSataNvm3;
    AcpiSetupData->PepIsp = SetupData->PepIsp;    
    AcpiSetupData->PepCsme = SetupData->PepCsme;
    AcpiSetupData->PepSataRaidVol0 = SetupData->PepSataRaidVol0;
    AcpiSetupData->WwanModemResetDelay = SetupData->WwanModemResetDelay;
    AcpiSetupData->WwanPerstDelay = SetupData->WwanPerstDelay;
    AcpiSetupData->AcpiWwanPWMGDelay = SetupData->AcpiWwanPWMGDelay;
    AcpiSetupData->AcpiWwanPWRGDelay = SetupData->AcpiWwanPWRGDelay;
    AcpiSetupData->UcsiRetryEcEnable = SetupData->UcsiRetryEcEnable;

    DEBUG ((DEBUG_INFO, "AcpiSetupCallbacks:: End\n"));

    return;
  }
}

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
