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

/** @file AmiPciExpressLib.h
    

**/

#ifndef _AMI_PCI_EXPRESS_LIB_H_
#define _AMI_PCI_EXPRESS_LIB_H_
#ifdef __cplusplus
extern "C" {
#endif

//-------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------
#include <Efi.h>
#include <Pci.h>
#include <PciE.h>
#include <PciE21.h>
#include <PciE30.h>
#include <PciBus.h>
#include <AmiLib.h>
#include <AcpiRes.h>
#include <Protocol/PciIo.h>

//-------------------------------------------------------------------------
// Constants, Macros and Type Definitions
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
//Variable, Prototype, and External Declarations
//-------------------------------------------------------------------------
extern T_ITEM_LIST  gPcieEpList;



/**

    @param Brg Pointer to PCI Device Private Data structure.
    @param DevArrd Device Address on PCI Bus.
    @param VenDevId Pointer Data Buffer.

    @retval EFI_STATUS Return Status
    @retval EFI_SUCCESS             When everything is going on fine!
    @retval EFI_OUT_OF_RESOURCES    When system runs out of resources.
    @retval EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS PcieDoubleCheckCard(PCI_DEV_INFO *Brg, PCI_CFG_ADDR *DevAddr, UINT32 *VenDevId);


/**
    This function will calculate MAX PayLoad Size needed to work
    correctly.

        
    @param EndPoint Pointer to PCI Device Private Data structure.

    @retval MaxPayLoad
       

**/
UINT16 PcieCalcMaxPayLoad(PCI_DEV_INFO *EndPoint);


/**
    This function will create S3 Resume Boot Script
    for Device's Virtual Channels feature initialization if device
    supports Virtual Channels.

        
    @param Brg Pointer to PCI Device Private Data structure.
    @param S3SaveStateProtocol Pointer to EFI_S3_SAVE_STATE_PROTOCOL instance

    @retval VOID

**/
VOID PcieRecordBootScript(PCI_DEV_INFO *Brg, EFI_S3_SAVE_STATE_PROTOCOL *S3SaveStateProtocol, BOOLEAN RecordScript);


/**
    This function will convert Latency value from Values read from.
    PPCIe.LNK_CNT_REG to Nanoseconds and opposite.

        
    @param Latency Latency Value to convert
    @param L0s Tells if L0s or L1 Exit latency
    @param ToNs Tells how to convert:
        TRUE  - to the units of nano Seconds;
        FALSE - to the LNK_CNT_REG.LxExitLat register value

    @retval EFI_STATUS
        UINTN               Converted Value (-1) - means Invalid Value passed;

**/
//Return value in nS of Max L0s, L1 Exit latency .
UINTN PcieConvertLatency(UINTN Latency, BOOLEAN L0s, BOOLEAN ToNs);



/**
    This function will calculate MAX Latency needed to exit from
    ome of the ASPM Sattes L0s or L1.

        
    @param Device Pointer to PCI Device Private Data structure.
    @param L0s Tells which ASPM state Latency we are calculating

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
UINTN PcieCalcLatency(PCI_DEV_INFO *EndPoint, BOOLEAN L0s);

/**
    This function will calculate MAX PayLoad Size needed to work
    correctly.

        
    @param EndPoint Pointer to PCI Device Private Data structure.

    @retval UINT16
       

**/
UINT16 PcieCalcMaxPayLoad(PCI_DEV_INFO *EndPoint);


/**
    This function will allocate memory for PCI Express Data of the Device.
    
    @param Device Pointer to PCI Device Private Data structure. 
    @param PcieCapOffset Current Offset of PCIE Std Compatibility Registers Block.

    @retval EFI_STATUS Return Status
    @retval EFI_SUCCESS Set successfully.
    @retval EFI_INVALID_PARAMETER the Input parameter is invalid.

**/
EFI_STATUS PcieAllocateInitPcieData(PCI_DEV_INFO *Device, UINT8 PcieCapOffset);


/**
    This function will update Device's referenced as "Func0"
    Clock Power Management based on Setup Settings and hardware capabilities.

        
    @param Func0 Pointer to PCI Device Private Data structure.
    @param LnkCnt Pointer to the Device's LNK_CNT register data.

    @retval NOTHING

    @note  Must be called only for PCI Express devices (Device->PciExpress!=NULL)

**/
VOID PcieUpdateClockPm(PCI_DEV_INFO *Func0, PCIE_LNK_CNT_REG *LnkCnt);

/**
    This function will Disable Link of DNSTREAM port referenced as
    "Device" based on Setup Settings and hardware STATUS. Used to disable "EMPTY"
    links to save some POWER or "Force" LINK to Disable state if Link can not be
    trained correctly.

        
    @param Device Pointer to PCI Device Private Data structure.
    @param Force Flag to indicate to disable link unconditionally.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

    @note  Must be called only for PCI Express devices (Device->PciExpress!=NULL)

**/
EFI_STATUS PcieDisableLink(PCI_DEV_INFO *Device, BOOLEAN Force);

/**
    This function will Program Link with provided VALUEs
    trained correctly.

        
    @param Device Pointer to PCI Device Private Data structure.
    @param Data Value to put in LNK_CNT register
    @param Reg2 Flag to select LNK_CNT or LNK_CNT2 register.

    @retval EFI_STATUS Return Status
    @retval EFI_SUCCESS             When everything is going on fine!
    @retval EFI_OUT_OF_RESOURCES    When system runs out of resources.
    @retval EFI_DEVICE_ERROR        When driver fails access PCI Bus.

    @note  Must be called only for PCI Express devices (Device->PciExpress!=NULL)

**/

EFI_STATUS PcieProgramLink(PCI_DEV_INFO *Device, UINT16 Data, BOOLEAN Reg2);

/**
    This function will issue HOT RESET on a LINK referenced by "Device".

    @param Device Pointer to PCI Device Private Data structure.
    @param LnkSpeed check LNK SPEED VECTOR. 

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

    @note  Must be called only for PCI Express devices (Device->PciExpress!=NULL)

**/
EFI_STATUS PcieResetLink(PCI_DEV_INFO *Device, UINT8 LnkSpeed);

/**
    This function will Initialize SLT_CNT and SLT_STA registers
    of the "Device".

        
    @param Device Pointer to PCI Device Private Data structure.

    @retval EFI_STATUS
        EFI_SUCCESS             When everything is going on fine!
        EFI_DEVICE_ERROR        When driver fails access PCI Bus.

    @note  Must be called only for PCI Express devices (Device->PciExpress!=NULL)

**/

EFI_STATUS PcieSetSlotProperties(PCI_DEV_INFO *Device);


/**
    This function will initiate LINK training of the
    Down Stream Port referenced as "Device".

        
    @param Device Pointer to PCI Device Private Data structure.
    @param Data Value to put in LNK_CNT register.
    @param LnkSpeed check LNK SPEED VECTOR.
    @param RetryCount count for retry.
    @param Timeout microseconds for stall execution.

    @retval EFI_STATUS Return Status
    @retval EFI_SUCCESS             When everything is going on fine!
    @retval EFI_NOT_FOUND           When Device not present in the system.
    @retval EFI_DEVICE_ERROR        When driver fails access PCI Bus.

    @note  Must be called only for PCI Express devices (Device->PciExpress!=NULL)

**/
EFI_STATUS PcieRetrainLink(	PCI_DEV_INFO        *Device,
                            UINT16              Data,
                            UINT8               LnkSpeed,
                            UINTN               RetryCount,
                            UINTN               TimeOut);

/**
    This function will Collect PCI Express Capabilities Data
    of the "Device".

        
    @param Device Pointer to PCI Device Private Data structure.

    @retval EFI_STATUS
    @retval EFI_SUCCESS             When everything is going on fine!
    @retval EFI_DEVICE_ERROR        When driver fails access PCI Bus.

    @note  Must be called only for PCI Express devices (Device->PciExpress!=NULL)

**/
EFI_STATUS PcieGetCaps(PCI_DEV_INFO *Device);

/**
    This function will Select values for Link Control2 register on
    both sides of the LINK based on Setup Settiongs and hardware capabilities.

        
    @param DnStreamPort Pointer to PCI Device Private Data of Downstream Port of the link.
    @param DnLnkCnt Pointer to the LNK_CNT Reg of the Downstream Port of the link.
    @param UpStreamPort Pointer to PCI Device Private Data of Upwnstream Port of the link.
    @param UpLnkCnt Pointer to the LNK_CNT Reg of the Downstream Port of the link.
    @param LinkTrainingRequired Flag to modify if Link will need RETRAINING after programming.

    @retval EFI_STATUS
    @retval EFI_SUCCESS             When everything is going on fine!
    @retval EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS PcieSetLnkProperties( PCI_DEV_INFO           *DnStreamPort,
                                 PCIE_LNK_CNT_REG       *DnLnkCnt,
                                 PCI_DEV_INFO           *UpStreamPort,
                                 PCIE_LNK_CNT_REG       *UpLnkCnt,
                                 BOOLEAN                *LinkTrainingRequired
                                 );

/**
    This function will Initialize Pcie Link on both sides starting
    fronm DownStream Port.

        
    @param DnPort Pointer to PCI Device Private Data structure.

    @retval EFI_STATUS
    @retval EFI_SUCCESS             When everything is going on fine!
    @retval EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS PcieInitLink(PCI_DEV_INFO 	*DnPort);



/**
    This function will check if *PCI_DEV_INFO passed belongs to
    PCI Express DOWNSTREAM PORT:
    - RootPort of PCIe Root Complex;
    - DownStream Port of PCIe Switch;
    - PCI/PCI-X to PCIe Bridge.

        
    @param Device Pointer to PCI Device Private Data structure.

    @retval BOOLEAN
        TRUE         When PCI_DEV_INFO passed belongs to DOWNSTREAM PORT
        FALSE        All other cases...

    @note  Must be called only for PCI Express devices (Device->PciExpress!=NULL)

**/
BOOLEAN PcieIsDownStreamPort(PCI_DEV_INFO *Device);


/**
    Checks if PCIe features supported by the Device
    
    @param Device pointer to device internal data. 

    @retval BOOLEAN
        TRUE    Device is Function 0 of Mulifunctional device.
        FALSE   Device is not Function 0 of Mulifunctional device.

**/
BOOLEAN PcieCheckPcieCompatible(PCI_DEV_INFO	*Device);


/**
    This function will Fill out structure of PciE Ext Cap data

    @param Device Pointer to PCI Device Private Data structure.
    @param Dataptr pointer to holding data.
    @param DataSize size of data to allocate memory.
    @param CapHdrOffset offset for Capability Register header.
    @param Cap16 TRUE/FALSE
    @param OffsetFromCapHdr offset from Capability Register header. 

    @retval EFI_STATUS Return Status
    @retval EFI_SUCCESS             When everything is going on fine!
    @retval EFI_NOT_FOUND           When Device not present in the system.
    @retval EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS PcieCollectDeviceExtCap(	PCI_DEV_INFO    *Device,
                                    VOID            **DataPtr,
                                    UINTN           DataSize,
                                    UINT32          CapHdrOffset,
                                    BOOLEAN         Cap16,
                                    UINTN           OffsetFromCapHdr);

/**
    This function will power ON or OFF PCI Express Slot

        
    @param Dev Pointer to PCI Device Private Data structure.
    @param PwrOn Tells to turn slot power ON or OFF

    @retval EFI_STATUS
    @retval EFI_SUCCESS             When everything is going on fine!
    @retval EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS PcieSetSlotPower(PCI_DEV_INFO* Device, BOOLEAN PwrOn);

/**
    This function will removed Device referencesd as EndPoint from
    the gPcieEpLst data structure used as a database of available EndPoint devices.

        
    @param EndPoint Pointer to PCI Device Private Data structure.

    @retval EFI_STATUS
    @retval EFI_SUCCESS             When everything is going on fine!
    @retval EFI_NOT_FOUND           EndPoint not in the gPcieEpLst.

**/
EFI_STATUS PcieRemoveEpFromLst(PCI_DEV_INFO *EndPoint, BOOLEAN AllFunc);


/**
    This function will Initialize "Device"'s Virtual Channel properties
    based on Setup Settings and hardware capabilities.

        
    @param Device Pointer to PCI Device Private Data.

    @retval EFI_STATUS
    @retval EFI_SUCCESS             When everything is going on fine!
    @retval EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS PcieInitVc(PCI_DEV_INFO  *Device);


/**
    This function will Select values for Link Control2 register on
    both sides of the LINK based on Setup Settings and hardware capabilities.

        
    @param DnPort Pointer to PCI Device Private Data of Downstream Port of the link.
    @param UpPort Pointer to PCI Device Private Data of Upwnstream Port of the link.
    @param Aspm ASPM value to programm.
    @param LinkHotResetRequired Flag to modify if Link will need HOT RESET after programming.

    @retval EFI_STATUS
    @retval EFI_SUCCESS             When everything is going on fine!
    @retval EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS PcieSetAspm(PCI_DEV_INFO *DnPort,PCI_DEV_INFO *UpPort,UINT16 Aspm);


/**
    This function will Select values for DEVICE CONTROL register
    based on Setup Settings and hardware capabilities.

        
    @param Device Pointer to PCI Device Private Data.
    @param MaxPayload Pointer to the MaxPayload value.

    @retval EFI_STATUS
    @retval EFI_SUCCESS             When everything is going on fine!
    @retval EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS PcieSetDevProperties(PCI_DEV_INFO *Device, UINT16 MaxPayload );


/**
    This function will Select values for DEVICE CONTROL register
    based on Setup Settings and hardware capabilities.

        
    @param EndPoint Pointer to PCI Device Private Data.
    @param MaxPayload MaxPayload value.
    @param LatencyL0s Latency L0s value.
    @param LatencyL1 Latency L1 value.

    @retval EFI_STATUS
    @retval EFI_SUCCESS             When everything is going on fine!
    @retval EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS PcieApplyDevSettings(PCI_DEV_INFO *EndPoint, UINT16 MaxPayLoad, UINT16 LatencyL0s, UINT16 LatencyL1);


/**
    This function will detect optimal settings and programm -
    MaxPayLoadSize, ReadRequestSize, ASPM settings and Virtual Channels
    for all devices in the PCIE Links Chain.

        
    @param Device Pointer to PCI Device Private Data structure.

    @retval EFI_STATUS
    @retval EFI_SUCCESS             When everything is going on fine!
    @retval EFI_DEVICE_ERROR        When driver fails access PCI Bus.

    @note  
  RootPortComplex<=>Switch0...<=>...SwitchN<=>EndPoint
**/
EFI_STATUS PcieInitDevChain(PCI_DEV_INFO* Device);


/**
    This function will Init VC_DATA structure of the "Device".

        
    @param Device Pointer to PCI Device Private Data.
    @param DevAddr Pointer to Device Address value.

    @retval EFI_STATUS
    @retval EFI_SUCCESS             When everything is going on fine!
    @retval EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS PcieInitDeviceVc(PCI_DEV_INFO *Device, PCI_CFG_ADDR *DevAddr);

/**
    This function will collect information about PCIE Device

        
    @param Device Pointer to PCI Device Private Data structure.
    @param SriovSupport TRUE/FALSE.

    @retval EFI_STATUS Return Status
    @retval EFI_SUCCESS             When everything is going on fine!
    @retval EFI_OUT_OF_RESOURCES    When system runs out of resources.
    @retval EFI_DEVICE_ERROR        When driver fails access PCI Bus.

    @note  The function will do the following steps
        1.disable ASPM if any;
        2.disable PMI Generation if any;
        3.power up PCIE Slot (if capable) if the slot has device behind it;
        4.if nothing connected to slot, power it down to make it HOTPLUG READY;
**/
//EFI_STATUS PcieProbeDevice(PCI_DEV_INFO *Device, UINT8 *MaxBusFound, BOOLEAN SriovSupport);
EFI_STATUS PcieProbeDevice(PCI_DEV_INFO *Device, BOOLEAN SriovSupport);

/**
    This function will Initialize PCIE Device to it's default settings
        
    @param Device Pointer to PCI Device Private Data structure.

    @retval EFI_STATUS Return Status
    @retval EFI_SUCCESS             When everything is going on fine!
    @retval EFI_OUT_OF_RESOURCES    When system runs out of resources.
    @retval EFI_DEVICE_ERROR        When driver fails access PCI Bus.

    @note  The function will do the following steps
        1.disable ASPM if any;
        2.disable PMI Generation if any;
        3.power up PCIE Slot (if capable) if the slot has device behind it;
        4.if nothing connected to slot, power it down to make it HOTPLUG READY;
**/
EFI_STATUS PcieInitDevice(PCI_DEV_INFO *Device, UINT8 *MaxBusFound);

/**
    Checks and initializes according to the setup settings 
    Integratred PCI Express Device like Integrated End Point and RC 
    Event Collector. This Device/Port type value == PCIE_TYPE_RC_INTEGR_EP
    or PCIE_TYPE_RC_EVT_COLLECT don't have a Link assotiated with them and 
    from standard PCIe registers have only Device CAP|CTL|STA registers set.
    This devices not supporting ASPM and only settings thy can accept -  
    - Enable Relaxed Ordering;
    - Extended Tag Field Enable;
    - Enable No Snoop;
    - Max_Payload_Size;
    - Max_Read_Request_Size;

        
    @param IntPcieDev a Pointer to PCI Device Private Data structure.

    @retval EFI_STATUS Return Status
    @retval EFI_SUCCESS             When everything is going on fine!
    @retval EFI_OUT_OF_RESOURCES    When system runs out of resources.
    @retval EFI_DEVICE_ERROR        When driver fails access PCI Bus.

**/
EFI_STATUS PcieCheckIntegratedDevice(PCI_DEV_INFO *IntPcieDev);





/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif //_AMI_PCI_EXPRESS_LIB_H_
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

