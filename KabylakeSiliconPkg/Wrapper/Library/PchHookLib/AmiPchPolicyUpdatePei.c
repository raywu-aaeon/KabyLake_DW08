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
#include <Token.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <AmiCspLib.h>
#include <SbChipsetGuid.h>
#include <Library/PchInfoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <SbSetupData.h>
#include <Library/MmPciLib.h>
#include <Ppi/CpuIo.h>
#include <Ppi/Stall.h>
#include <Library/SbHdaVerbTableLib.h>
#include <PlatformInfo.h>
#include <SbPolicyUpdate.h>
#include <Library/PeiPlatformHookLib.h>
#include <Pi/PiFirmwareVolume.h>
#include <Pi/PiFirmwareFile.h>
#include <AmiPeiLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#if defined iME_SUPPORT && iME_SUPPORT
#include <MeBiosExtensionSetup.h>
#endif
#include <SioSetup.h>
#include <Library/AmiSioPeiLib.h>
#include <Library/EcLib.h>
// AMI_OVERRIDE_NCD_START >>> EIP222861 Implement Detect Non-Compliance Device item function (Parts of South Bridge)
#include <Library/PchPcieRpLib.h>
// AMI_OVERRIDE_NCD_END <<< EIP222861 Implement Detect Non-Compliance Device item function (Parts of South Bridge)
#include <Library/ConfigBlockLib.h>
#include <Ppi/Spi.h>
#include <Library/HobLib.h>
#include <Private/PchConfigHob.h>

#ifndef KSC_C_DOCK_STATUS
#define KSC_C_DOCK_STATUS 0x8A
#endif

#ifndef KSC_B_DOCK_STATUS_ATTACH
#define KSC_B_DOCK_STATUS_ATTACH 0x01
#endif

#ifndef CRB_EC_SUPPORT
#define CRB_EC_SUPPORT 0x00
#endif

// AMI_OVERRIDE >>>
#define RETRAIN_DELAY      50
// AMI_OVERRIDE <<<

typedef EFI_STATUS (HOOK_SB_POLICY)(
    IN VOID                             *Services,        
    IN OUT VOID                         *PchPlatformPolicyPtr,
    IN SETUP_DATA                       *SetupData,
    IN BOOLEAN                          Pei    
);

typedef struct {
    EFI_FFS_FILE_HEADER FfsHdr;
    EFI_COMMON_SECTION_HEADER SecHdr;
    EFI_GUID            SectionGuid;
    UINT8               FwCapHdr[0];
} AMI_FFS_COMMON_SECTION_HEADER;

#define AMI_PCI_DATA_SIG  0x4154414449435024 //"$PCIDATA"

#define PCH_DEV_IP_OFFSET(Dev)                R_PCH_RCRB_D##Dev##IP
#define PCH_DEV_IP_SETTING(Func, IntPin)      (UINT32) (IntPin << (4 * Func))
#define PCH_DEV_IR_OFFSET(Dev)                R_PCH_RCRB_D##Dev##IR
#define PCH_DEV_IR_SETTING(Iar, Ibr, Icr,Idr) (UINT16) (Idr << N_PCH_RCRB_DXXIR_IDR | \
                                                        Icr << N_PCH_RCRB_DXXIR_ICR | \
                                                        Ibr << N_PCH_RCRB_DXXIR_IBR | \
                                                        Iar << N_PCH_RCRB_DXXIR_IAR)

#define ALIGNMENT_4KB     0x1000 

typedef enum {
  AmiUndefinedType        = 0,
  AmiDescriptorType       = 1,
  AmiBiosType             = 2,
  AmiMeType               = 3,
  AmiGbeType              = 4,
  AmiPlatformDataType     = 5
} AMI_PCH_SPI_REGION_TYPE;

typedef struct _AMI_SPI_PROTECTED_RANGE_CONIFG{
    AMI_PCH_SPI_REGION_TYPE  AmiPchSpiRegionType;   
    BOOLEAN                  WriteProtectionEnable;
    BOOLEAN                  ReadProtectionEnable;
    UINT32                   ProtectedRangeBase;
    UINT32                   ProtectedRangeLength;
} AMI_SPI_PROTECTED_RANGE_CONIFG;

EFI_STATUS
EFIAPI
OverrideFprrEndOfPei (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

static EFI_PEI_NOTIFY_DESCRIPTOR  mOverrideFprrEndOfPeiNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiEndOfPeiSignalPpiGuid,
  (EFI_PEIM_NOTIFY_ENTRY_POINT) OverrideFprrEndOfPei
};

//
// Interrupt pin configuration table
// 2 inputs per entry for customizing PCH interrupt pin in DXXIP register,
// one is device number and the other is INTx pins to the corresponding functions.
// PCH_DEV_IP_OFFSET() helps to convert device number to DXXIP offset and
// PCH_DEV_IP_SETTING() helps to convert INTx pin to the corresponding bitmap
// by function in DXXIP register
//
/*
PCH_DEVICE_INTERRUPT_PIN_CONFIG AmiDxxIpConfigH[] = { 
  { PCH_DEV_IP_OFFSET(31),         // Device number
    AMI_INTERRUPT_PIN_CONFIG_DEV31_H
  },
  { PCH_DEV_IP_OFFSET(29),
    AMI_INTERRUPT_PIN_CONFIG_DEV29_H
  },
  { PCH_DEV_IP_OFFSET(28),
    AMI_INTERRUPT_PIN_CONFIG_DEV28_H
  },
  { PCH_DEV_IP_OFFSET(27),
    AMI_INTERRUPT_PIN_CONFIG_DEV27_H
  },
  { PCH_DEV_IP_OFFSET(26),
    AMI_INTERRUPT_PIN_CONFIG_DEV26_H
  },
  { PCH_DEV_IP_OFFSET(25),
    AMI_INTERRUPT_PIN_CONFIG_DEV25_H
  },
  { PCH_DEV_IP_OFFSET(20),
    AMI_INTERRUPT_PIN_CONFIG_DEV20_H
  }  
};

PCH_DEVICE_INTERRUPT_PIN_CONFIG AmiDxxIpConfigLp[] = { 
  { PCH_DEV_IP_OFFSET(31),         // Device number
    AMI_INTERRUPT_PIN_CONFIG_DEV31_LP
  },
  { PCH_DEV_IP_OFFSET(29),
    AMI_INTERRUPT_PIN_CONFIG_DEV29_LP
  },
  { PCH_DEV_IP_OFFSET(28),
    AMI_INTERRUPT_PIN_CONFIG_DEV28_LP
  },
  { PCH_DEV_IP_OFFSET(27),
    AMI_INTERRUPT_PIN_CONFIG_DEV27_LP
  },
  { PCH_DEV_IP_OFFSET(25),
    AMI_INTERRUPT_PIN_CONFIG_DEV25_LP
  },
  { PCH_DEV_IP_OFFSET(20),
    AMI_INTERRUPT_PIN_CONFIG_DEV20_LP
  }
};

//
// Interrupt pin route configuration table
// 2 inputs per entry for customizing INTx -> PIRQx routing for certain
// device in DXXIR register, one is device number and the other is IRQx
// route to interrupt pins.
// PCH_DEV_IR_OFFSET() helps to convert device number to DXXIR offset and
// PCH_DEV_IR_SETTING() helps to convert IRQx setting to corresponding 
// bitmap in DXXIR register.
//
static PCH_DEVICE_INTERRUPT_ROUTE_CONFIG AmiDxxIrConfigH[] = {
// Device number          IAR,    IBR,    ICR,    IDR 		
  {PCH_DEV_IR_OFFSET(31), AMI_INTERRUPT_ROUTE_CONFIG_DEV31_H },
  {PCH_DEV_IR_OFFSET(29), AMI_INTERRUPT_ROUTE_CONFIG_DEV29_H },
  {PCH_DEV_IR_OFFSET(28), AMI_INTERRUPT_ROUTE_CONFIG_DEV28_H },
  {PCH_DEV_IR_OFFSET(27), AMI_INTERRUPT_ROUTE_CONFIG_DEV27_H },
  {PCH_DEV_IR_OFFSET(26), AMI_INTERRUPT_ROUTE_CONFIG_DEV26_H },
  {PCH_DEV_IR_OFFSET(25), AMI_INTERRUPT_ROUTE_CONFIG_DEV25_H },
  {PCH_DEV_IR_OFFSET(20), AMI_INTERRUPT_ROUTE_CONFIG_DEV20_H }
};

static PCH_DEVICE_INTERRUPT_ROUTE_CONFIG AmiDxxIrConfigLp[] = { 
// Device number          IAR,    IBR,    ICR,    IDR 
  {PCH_DEV_IR_OFFSET(31), AMI_INTERRUPT_ROUTE_CONFIG_DEV31_LP },
  {PCH_DEV_IR_OFFSET(29), AMI_INTERRUPT_ROUTE_CONFIG_DEV29_LP },
  {PCH_DEV_IR_OFFSET(28), AMI_INTERRUPT_ROUTE_CONFIG_DEV28_LP },
  {PCH_DEV_IR_OFFSET(27), AMI_INTERRUPT_ROUTE_CONFIG_DEV27_LP },
  {PCH_DEV_IR_OFFSET(26), AMI_INTERRUPT_ROUTE_CONFIG_DEV26_LP },
  {PCH_DEV_IR_OFFSET(25), AMI_INTERRUPT_ROUTE_CONFIG_DEV25_LP }, 
  {PCH_DEV_IR_OFFSET(23), AMI_INTERRUPT_ROUTE_CONFIG_DEV23_LP },
  {PCH_DEV_IR_OFFSET(21), AMI_INTERRUPT_ROUTE_CONFIG_DEV21_LP }, 
  {PCH_DEV_IR_OFFSET(20), AMI_INTERRUPT_ROUTE_CONFIG_DEV20_LP },  
  {PCH_DEV_IR_OFFSET(19), AMI_INTERRUPT_ROUTE_CONFIG_DEV19_LP }  
};
*/
//----------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//----------------------------------------------------------------------------
static EFI_GUID  gSetupGuid             = SETUP_GUID;
static EFI_GUID  FwFfsGuid              = {0x7B9A0A12, 0x42F8, 0x4d4c, 0x82, 0xB6, 0x32, 0xF0, 0xCA, 0x19, 0x53, 0xF4};
static EFI_GUID  FwSectionGuid          = {0x9BEC7109, 0x6D7A, 0x413A, 0x8E, 0x4B, 0x01, 0x9C, 0xED, 0x05, 0x03, 0xE1};

/**
  AmiPxRcConfig[] table contains data for 8259 routing (how PIRQx is mapped to IRQy).
  This information is used by systems which choose to use legacy PIC
  interrupt controller. Only IRQ3-7,9-12,14,15 are valid. Values from this table
  will be programmed into ITSS.PxRC registers.
**/
UINT8 AmiPxRcConfig[] = AMI_LEGACY_INTERRUPT_CFG;

// AMI_OVERRIDE_NCD_START >>> EIP222861 Implement Detect Non-Compliance Device item function (Parts of South Bridge) 
VOID
SbPcieDetectNonComplaintPcieDevice (
  IN EFI_PEI_SERVICES       **PeiServices,
  IN EFI_PEI_STALL_PPI      *StallPpi,
  IN UINTN                  Index,
  IN SI_POLICY_PPI          *SiPolicy,
  IN SB_SETUP_DATA          *SbSetupData
)
{
  EFI_STATUS            Status;    
  PCH_PCIE_CONFIG       *PcieRpConfig;    
  UINTN                 Pcie_RP_Dev = 0;
  UINTN                 Pcie_RP_Fun = 0;
  
  Status = GetConfigBlock ((VOID *) SiPolicy, &gPcieRpConfigGuid, (VOID *) &PcieRpConfig);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);   
  } 

  if ((PcieRpConfig->RootPort[Index].PcieSpeed == PchPcieAuto) && \
      (SbSetupData->PcieRootPortEn[Index]!= 0)) {
     GetPchPcieRpDevFun (Index, &Pcie_RP_Dev, &Pcie_RP_Fun);

     DEBUG ((DEBUG_INFO, "Enhance Detect Non-Compliance PCIE Device @B:0|D:0x%x|F:0x%x Start .\n", Pcie_RP_Dev, Pcie_RP_Fun));

     // Assign temp bus
     DEBUG ((DEBUG_INFO, "Assign temp bus ...\n"));
     WRITE_PCI32 (SB_BUS, Pcie_RP_Dev, Pcie_RP_Fun, R_PCI_BRIDGE_BNUM, 0x00010100);
     
     //
     // A config write is required in order for the device to re-capture the Bus number,
     // according to PCI Express Base Specification, 2.2.6.2
     // Write to a read-only register VendorID to not cause any side effects.
     //
     WRITE_PCI32(1, 0, 0, PCI_VENDOR_ID_OFFSET, 0x12345678);

     if (READ_PCI16(1, 0, 0, PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
       DEBUG ((DEBUG_INFO, "Can't find Device... Retrain device first.\n"));
       // Link Disable
       WRITE_PCI8(SB_BUS, Pcie_RP_Dev, Pcie_RP_Fun, R_PCH_PCIE_LCTL, B_PCIE_LCTL_LD);
       StallPpi->Stall( (CONST EFI_PEI_SERVICES **) PeiServices, StallPpi, (RETRAIN_DELAY * 10) ); //delay 500us
       // Retrain Link
       WRITE_PCI8(SB_BUS, Pcie_RP_Dev, Pcie_RP_Fun, R_PCH_PCIE_LCTL, B_PCIE_LCTL_RL); 
       StallPpi->Stall( (CONST EFI_PEI_SERVICES **) PeiServices, StallPpi, (RETRAIN_DELAY * 8000) ); //delay 400ms       
       if (READ_PCI16(1, 0, 0, PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
         DEBUG ((DEBUG_INFO, "Still can't find Device in Gen2 Speed... Speed is setted in Gen1 and delay 200 ms.\n"));
         // Set Speed to Gen1
         RW_PCI8(SB_BUS, Pcie_RP_Dev, Pcie_RP_Fun, R_PCH_PCIE_LCTL2, 0x01, 0x03);
         StallPpi->Stall( (CONST EFI_PEI_SERVICES **) PeiServices, StallPpi, (RETRAIN_DELAY * 4000) ); //delay 200ms

         RW_PCI16(SB_BUS, Pcie_RP_Dev, Pcie_RP_Fun, R_PCH_PCIE_MPC, 0x01, BIT13);  // V_PCH_PCIE_MPC_PCIESD_GEN1 << N_PCH_PCIE_MPC_PCIESD
         StallPpi->Stall( (CONST EFI_PEI_SERVICES **) PeiServices, StallPpi, (RETRAIN_DELAY * 4000) ); //delay 200ms

         if (READ_PCI16(1, 0, 0, PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
           DEBUG ((DEBUG_INFO, "Still can't find Device in Gen1 Speed... Retrain device again !!!\n"));
           // Link Disable
           WRITE_PCI8(SB_BUS, Pcie_RP_Dev, Pcie_RP_Fun, R_PCH_PCIE_LCTL, B_PCIE_LCTL_LD);
           StallPpi->Stall( (CONST EFI_PEI_SERVICES **) PeiServices, StallPpi, (RETRAIN_DELAY * 10) ); //delay 500us 
           // Retrain Link
           WRITE_PCI8(SB_BUS, Pcie_RP_Dev, Pcie_RP_Fun, R_PCH_PCIE_LCTL, B_PCIE_LCTL_RL);
           StallPpi->Stall( (CONST EFI_PEI_SERVICES **) PeiServices, StallPpi, (RETRAIN_DELAY * 8000) ); //delay 400ms 

           if (READ_PCI16(1, 0, 0, PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
             PcieRpConfig->RootPort[Index].PcieSpeed = PchPcieGen1;   
           }
         } else {
           PcieRpConfig->RootPort[Index].PcieSpeed = PchPcieGen1;
         }
       }
     }

     // Remove temp bus
     DEBUG ((DEBUG_INFO, "Remove temp bus.\n"));
     WRITE_PCI32(SB_BUS, Pcie_RP_Dev, Index, PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET, 0xFF000000);

     DEBUG ((DEBUG_INFO, "Enhance Detect Non-Compliance PCIE Device end.\n"));
  }
  
}
// AMI_OVERRIDE_NCD_END <<< EIP222861 Implement Detect Non-Compliance Device item function (Parts of South Bridge) 

VOID
AmiUpdatePchUsbConfig (
    IN CONST EFI_PEI_SERVICES   **PeiServices,
    IN SB_SETUP_DATA            *SbSetupData,
    IN OUT SI_POLICY_PPI        *SiPolicy )
{
    EFI_STATUS          Status;        
#if defined USB_OVERCURRENT_PIN_OVERRIDE_SUPPORT && USB_OVERCURRENT_PIN_OVERRIDE_SUPPORT == 1  
    UINTN               i;
#endif
    PCH_USB_CONFIG      *PchUsbConfig;    
#if defined USB_OVERCURRENT_PIN_OVERRIDE_SUPPORT && USB_OVERCURRENT_PIN_OVERRIDE_SUPPORT == 1 && !USB_OVERCURRENT_PIN_OVERRIDE_USE_ELINK
    UINT8 USB20OverCurrentMapping[] = {USB_OVER_CURRENT_MAPPING_SETTINGS};
    UINT8 USB30OverCurrentMapping[] = {USB30_OVER_CURRENT_MAPPING_SETTINGS};
#endif    
    DEBUG ((DEBUG_INFO, "AmiUpdatePchUsbConfig() - Start\n"));
    
    Status = GetConfigBlock ((VOID *) SiPolicy, &gUsbConfigGuid, (VOID *) &PchUsbConfig);
    if (EFI_ERROR (Status)) {
        ASSERT_EFI_ERROR (Status);
    }
    
    
#if defined USB_OVERCURRENT_PIN_OVERRIDE_SUPPORT && USB_OVERCURRENT_PIN_OVERRIDE_SUPPORT == 1  
    i = 0;
#if USB_OVERCURRENT_PIN_OVERRIDE_USE_ELINK         
    if (SbSetupData->UsbOverCurrentPinMapping.UsbOverCurrentPinOverride == 1) {
        if(SbSetupData->UsbOverCurrentPinMapping.Usb20PinNum == GetPchUsbMaxPhysicalPortNum()) {
            for(i = 0; i < GetPchUsbMaxPhysicalPortNum(); i++) {
                PchUsbConfig->PortUsb20[i].OverCurrentPin = SbSetupData->UsbOverCurrentPinMapping.Usb20PinMapping[i];
                DEBUG((EFI_D_INFO, "PortUsb20[%d] = %d\n", i, PchUsbConfig->PortUsb20[i].OverCurrentPin));
            }
        }
          
        if(SbSetupData->UsbOverCurrentPinMapping.Usb30PinNum == GetPchXhciMaxUsb3PortNum()) {
            for(i = 0; i < GetPchXhciMaxUsb3PortNum(); i++) {
                PchUsbConfig->PortUsb30[i].OverCurrentPin = SbSetupData->UsbOverCurrentPinMapping.Usb30PinMapping[i];
                DEBUG((EFI_D_INFO, "PortUsb30[%d] = %d\n", i, PchUsbConfig->PortUsb30[i].OverCurrentPin));
            }
        }       
    }
#else
    if(sizeof(USB20OverCurrentMapping) == GetPchUsbMaxPhysicalPortNum()) {
        for(i = 0; i < GetPchUsbMaxPhysicalPortNum(); i++) {
            PchUsbConfig->PortUsb20[i].OverCurrentPin = USB20OverCurrentMapping[i];
            DEBUG((EFI_D_INFO, "PortUsb20[%d] = %d\n", i, PchUsbConfig->PortUsb20[i].OverCurrentPin));
        }
    }
          
    if(sizeof(USB30OverCurrentMapping) == GetPchXhciMaxUsb3PortNum()) {
        for(i = 0; i < GetPchXhciMaxUsb3PortNum(); i++) {
            PchUsbConfig->PortUsb30[i].OverCurrentPin = USB30OverCurrentMapping[i];
            DEBUG((EFI_D_INFO, "PortUsb30[%d] = %d\n", i, PchUsbConfig->PortUsb30[i].OverCurrentPin));
        }
    }          
#endif
#endif

    //
    // This policy option if set will move Port Disable Override register programming to ReadyToBoot event during DXE phase.
    // If not enabled it will program Port Disable Override register in PEI phase during PCH initialization
    // FALSE: Program in PEI phase (Default)
    // TRUE : Program in DXE phase
    //
#ifndef OEM_USB_PER_PORT_DISABLE_FLAG
    PchUsbConfig->DelayPdoProgramming = FALSE;
#else
    PchUsbConfig->DelayPdoProgramming = TRUE;
#endif

    DEBUG ((DEBUG_INFO, "AmiUpdatePchUsbConfig() - End\n"));
}

VOID AmiUpdateVerbTablesSize (
    IN EFI_PEI_SERVICES     **PeiServices,
    IN SI_POLICY_PPI        *SiPolicy
)
{
    EFI_STATUS              Status;
    PCH_HDAUDIO_CONFIG      *HdaConfig;
    HDAUDIO_VERB_TABLE      *VerbTable = NULL;
    UINTN                   Num;
    UINT16                   Index;

    Status = GetConfigBlock ((VOID *) SiPolicy, &gHdAudioConfigGuid, (VOID *) &HdaConfig);
    if (EFI_ERROR(Status)) {
      DEBUG((DEBUG_ERROR, "Fail to get HdAudioConfig, Skip updating VerbTable Size\n"));
      return;
    }

    DEBUG((DEBUG_INFO, "Verb Table Entry number is 0x%x\n", HdaConfig->VerbTableEntryNum));

    for (Num = 0; Num < HdaConfig->VerbTableEntryNum; ++Num) {
      VerbTable = ((HDAUDIO_VERB_TABLE **) (HdaConfig->VerbTablePtr)) [Num];
      if ((VerbTable != NULL) && (VerbTable->Header.DataDwords == 0x00)) {
        for (Index = 0; ; Index++) {
          if (VerbTable->Data[Index] == 0xFFFFFFFF) {
            VerbTable->Header.DataDwords = Index;
            DEBUG((DEBUG_INFO, "Update the size of Verb Table[%d] to 0x%x\n", Num, Index));
            break;
          }
        }
      }
    }
}

VOID
AmiUpdatePchAudioConfig (
    IN CONST EFI_PEI_SERVICES   **PeiServices,
    IN SB_SETUP_DATA            *SbSetupData,
    IN OUT SI_POLICY_PPI        *SiPolicy )
{
    DEBUG ((DEBUG_INFO, "AmiUpdatePchAudioConfig() - Start\n"));
    
    if(SbSetupData->PchHdAudio != 0){
        
#ifdef OEM_HDA_VERB_TABLE_INSTALL
        //Install OEM Verb Table
        AmiInstallOemHdaVerbTables((EFI_PEI_SERVICES **) PeiServices, SiPolicy);
#else
        AmiUpdateVerbTablesSize((EFI_PEI_SERVICES **) PeiServices, SiPolicy);
#endif
    } // PchHdAudio is enable
    
    DEBUG ((DEBUG_INFO, "AmiUpdatePchAudioConfig() - End\n"));
}

VOID
AmiUpdatePchSataConfig (
    IN CONST EFI_PEI_SERVICES   **PeiServices,
    IN SB_SETUP_DATA            *SbSetupData,
    IN OUT SI_POLICY_PPI        *SiPolicy )
{
    DEBUG ((DEBUG_INFO, "AmiUpdatePchSataConfig() - Start\n"));
    
    DEBUG ((DEBUG_INFO, "AmiUpdatePchSataConfig() - End\n"));
}

VOID
AmiUpdatePchPcieConfig (
    IN CONST EFI_PEI_SERVICES   **PeiServices,
    IN SB_SETUP_DATA            *SbSetupData,
    IN OUT SI_POLICY_PPI        *SiPolicy )
{
    DEBUG ((DEBUG_INFO, "AmiUpdatePchPcieConfig() - Start\n"));
    
    DEBUG ((DEBUG_INFO, "AmiUpdatePchPcieConfig() - End\n"));
}

VOID
AmiUpdatePchLanConfig (
    IN CONST EFI_PEI_SERVICES   **PeiServices,
    IN SB_SETUP_DATA            *SbSetupData,
    IN OUT SI_POLICY_PPI        *SiPolicy )
{
    DEBUG ((DEBUG_INFO, "AmiUpdatePchLanConfig() - Start\n"));
    
    DEBUG ((DEBUG_INFO, "AmiUpdatePchLanConfig() - End\n"));
}

VOID
AmiUpdatePchSerialIoConfig (
    IN CONST EFI_PEI_SERVICES   **PeiServices,
    IN SB_SETUP_DATA            *SbSetupData,
    IN OUT SI_POLICY_PPI        *SiPolicy )
{
    DEBUG ((DEBUG_INFO, "AmiUpdatePchSerialIoConfig() - Start\n"));
    
    DEBUG ((DEBUG_INFO, "AmiUpdatePchSerialIoConfig() - End\n"));
}

/**
    This Parses SDL Data Table Buffer and returnd pointer to the data table 
    with given "Signature"

         
    @param DataPtr Pointer to the SDL Data Buffer. 
    @param DataLen Total Data Buffer Length in bytes.
    @param Signature Total Data Buffer Length in bytes.
    @param TblPtr Pointer to the data table with "Signature". 
          
    @retval EFI_SUCCESS Function executed successfully,
    @retval EFI_ALREADY_STARTED driver already started
    @retval EFI_OUT_OF_RESOURCES not enough memory to perform operation

**/
EFI_STATUS ParseAmiSdlData(UINT8 *DataPtr, UINTN DataLen, UINT64 Signature, OUT VOID **TblPtr){
    UINT8   *start=DataPtr;
    UINT64  *sig;
    UINTN   i;
//-----------------
    if(start==NULL || TblPtr==NULL || DataLen==0 ) return EFI_INVALID_PARAMETER;

    for(i=0; i<DataLen-sizeof(UINT64); i++){
        sig=(UINT64*)(&start[i]);
        if(*sig == Signature) {
            *TblPtr=sig;
            return EFI_SUCCESS;
        }
    }
    return EFI_NOT_FOUND;
}

VOID
AmiUpdatePchDevIntConfig (
    IN CONST EFI_PEI_SERVICES   **PeiServices,
    IN SB_SETUP_DATA            *SbSetupData,
    IN OUT SI_POLICY_PPI        *SiPolicy )
{
    EFI_STATUS                      Status;
    EFI_FIRMWARE_VOLUME_HEADER      *pFV;
    UINTN                           FvNum=0;
    UINTN                           DataLen;
    BOOLEAN                         Found = FALSE;
    AMI_FFS_COMMON_SECTION_HEADER   *FileSection;
    AMI_SDL_PCI_DATA                *DataPtr;
    UINT8                           NumOfDevIntConfig;
    PCH_DEVICE_INTERRUPT_CONFIG     *DevIntConfig;
    UINT32                          PciDevCount;
    AMI_SDL_PCI_DEV_INFO            *PciDevice;
    UINT8                           i, Index, Index2, Func;
    UINT8                           SkipDeviceTable[] = {25};
    BOOLEAN                         SkipDevFound;
    PCH_INTERRUPT_CONFIG            *InterruptConfig;    
    SI_CONFIG                       *SiConfig;
    
    DEBUG ((DEBUG_INFO, "AmiUpdatePchDevIntConfig() - Start\n"));
    
    Status = GetConfigBlock ((VOID *) SiPolicy, &gInterruptConfigGuid, (VOID *) &InterruptConfig);
    ASSERT_EFI_ERROR (Status);  
    
    Status = GetConfigBlock ((VOID *) SiPolicy, &gSiConfigGuid, (VOID *) &SiConfig);
    ASSERT_EFI_ERROR (Status);
    
    SiConfig->TempPciBusMin   = PCH_PCIE_TEMP_RP_BUS_NUM_MIN;
    SiConfig->TempPciBusMax   = PCH_PCIE_TEMP_RP_BUS_NUM_MAX;
    
    Status = (*PeiServices)->FfsFindNextVolume (PeiServices, FvNum, (EFI_PEI_FV_HANDLE *) &pFV);
    
    while ( TRUE ) {
        Status = (*PeiServices)->FfsFindNextVolume( PeiServices, FvNum, (EFI_PEI_FV_HANDLE *) &pFV );
        if ( EFI_ERROR( Status ) ) {
            return;
        }

        FileSection = NULL;

        while ( TRUE ) {
            Status = (*PeiServices)->FfsFindNextFile( PeiServices,
                                                EFI_FV_FILETYPE_ALL,
                                               pFV,
                                               (EFI_PEI_FILE_HANDLE *) &FileSection );

            if ( Status == EFI_NOT_FOUND ) {
                break;
            }

            if(!guidcmp(&FwFfsGuid, &(FileSection->FfsHdr.Name)) &&
               !guidcmp(&FwSectionGuid, &(FileSection->SectionGuid))) {
                Found = TRUE;
                break;
            }
        }

        if ( Found ) {
            break;
        } else {
            FvNum++;
        }
    }
    if (Found == FALSE) {
        DEBUG((EFI_D_ERROR, "Can't found AMI Board Info FFS\n"));
        return;
    }
    
    DEBUG((EFI_D_INFO, "Found AMI Board Info FFS\n"));
    
    DataLen = FileSection->FfsHdr.Size[0] | (FileSection->FfsHdr.Size[1] << 8) | (FileSection->FfsHdr.Size[2] << 16);
    
    //Get PCI Data pointer AMI_PCI_DATA_SIG $PCIDATA
    Status=ParseAmiSdlData((UINT8 *)FileSection,DataLen,AMI_PCI_DATA_SIG, (VOID **) &DataPtr);
    ASSERT_EFI_ERROR(Status);
    PciDevCount = DataPtr->PciDevCount;
    PciDevice = DataPtr->PciDevices;
    
    NumOfDevIntConfig = InterruptConfig->NumOfDevIntConfig;
    DevIntConfig = InterruptConfig->DevIntConfig;
    
#ifndef MDEPKG_NDEBUG     
    DEBUG((EFI_D_INFO, "Before Update PCI IRQ\n"));
    for (i = 0; i < NumOfDevIntConfig; i++) {
        DEBUG((EFI_D_INFO, "{%d, %d, %d, %d}\n", DevIntConfig[i].Device, DevIntConfig[i].Function, DevIntConfig[i].IntX, DevIntConfig[i].Irq));
    }
#endif
    
    for (Index = 18; Index < 32; Index++) {
        
        SkipDevFound = FALSE;
        for (i = 0; i < sizeof(SkipDeviceTable); i++) {
            if (Index == SkipDeviceTable[i]) {
                DEBUG((EFI_D_INFO, "Skip Dev %d\n", Index));
                SkipDevFound  = TRUE;
            }
        }
                
        if (SkipDevFound)
            continue;
        
        // Find B0 D18~31 Func 0 PciDevice
        for (i = 0; i < PciDevCount; i++) { 
            if ((PciDevice[i].Bus == 0) && (PciDevice[i].Device == Index) && (PciDevice[i].Function == 0)) {
                break;
            }
        }        
        
        if (i != PciDevCount) {
            for (Index2 = 0; Index2 < NumOfDevIntConfig; Index2++) {
                if (DevIntConfig[Index2].Device == Index) {
                    Func = DevIntConfig[Index2].Function;
                    //if (PciDevice[i].PicIrq[Func % 4].ChipsetReg != 0)
                    DevIntConfig[Index2].IntX = (Func % 4) + 1;
                    //if (PciDevice[i].ApicIrq[Func % 4].IoApicItin != 0)
                    DevIntConfig[Index2].Irq = PciDevice[i].ApicIrq[Func % 4].IoApicItin;
                }
            }
        }
    }
      
    for (i = 0; i < PciDevCount; i++) {
        if ((PciDevice[i].Bus != 0) || (PciDevice[i].Device < 18) || (PciDevice[i].Device > 31))
            continue;
        
        for (Index = 0; Index < NumOfDevIntConfig; Index++) {
            if ((DevIntConfig[Index].Device == PciDevice[i].Device) && (DevIntConfig[Index].Function == PciDevice[i].Function))
                break;
        }
        
        if (Index != NumOfDevIntConfig) {
            for (Index2 = 0; Index2 < 4; Index2++) {
                if ((PciDevice[i].PicIrq[Index2].ChipsetReg != 0) || (PciDevice[i].ApicIrq[Index2].IoApicItin != 0)) {
                    DevIntConfig[Index].IntX = Index2 + 1;
                    DevIntConfig[Index].Irq = PciDevice[i].ApicIrq[Index2].IoApicItin;
                    break;
                }
            }
        }
    }

#ifndef MDEPKG_NDEBUG 
    DEBUG((EFI_D_INFO, "After Update PCI IRQ\n"));
    for (i = 0; i < NumOfDevIntConfig; i++) {
        DEBUG((EFI_D_INFO, "{%d, %d, %d, %d}\n", DevIntConfig[i].Device, DevIntConfig[i].Function, DevIntConfig[i].IntX, DevIntConfig[i].Irq));
    }    
#endif    
    
    DEBUG ((DEBUG_INFO, "AmiUpdatePchDevIntConfig() - End\n"));
}

VOID
OemUpdateLpcDecode (
  IN EFI_PEI_SERVICES       **PeiServices
  )
{
    EFI_STATUS                          Status;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI     *ReadOnlyVariable;
    SIO_DEV_NV_DATA                     SioDevNvData;
    EFI_GUID                            ssg = SIO_VARSTORE_GUID;
    UINTN                               VariableSize;
    
    DEBUG((EFI_D_INFO, "OemUpdateLpcDecode\n"));
    
    Status = (*PeiServices)->LocatePpi ((CONST EFI_PEI_SERVICES **) PeiServices, 
                                        &gEfiPeiReadOnlyVariable2PpiGuid,
                                        0,
                                        NULL,
                                        (VOID **) &ReadOnlyVariable);
    if (EFI_ERROR(Status)) return;
    
    VariableSize = sizeof(SIO_DEV_NV_DATA);
    // Get COMA decode setting from variable
    Status = ReadOnlyVariable->GetVariable (ReadOnlyVariable,
#if (SIO_SETUP_USE_APTIO_4_STYLE==1)		// [ EIP435639 ]
											L"PNP0501_0_NV",
#else
                                            L"NV_SIO0_LD0",
#endif	// #if (SIO_SETUP_USE_APTIO_4_STYLE==1)
                                            &ssg,
                                            NULL,
                                            &VariableSize,
                                            &SioDevNvData); 
    
    if(!EFI_ERROR(Status) && (SioDevNvData.DevEnable != 0)) {
      switch(SioDevNvData.DevPrsId) {
        default:
        case 0:
        case 1:
        case 2:
            AmiSioLibSetLpcDeviceDecoding(NULL, 0x3F8, 0, dsUART); 
            break;
        case 3:
            AmiSioLibSetLpcDeviceDecoding(NULL, 0x2F8, 0, dsUART); 
            break;
        case 4:
            AmiSioLibSetLpcDeviceDecoding(NULL, 0x3E8, 0, dsUART); 
            break;
        case 5:
            AmiSioLibSetLpcDeviceDecoding(NULL, 0x2E8, 0, dsUART); 
            break;
      }
    }

    // Get COMB decode setting from variable
    Status = ReadOnlyVariable->GetVariable (ReadOnlyVariable,
#if (SIO_SETUP_USE_APTIO_4_STYLE==1)		// [ EIP435639 ]
											L"PNP0501_1_NV",
#else
                                            L"NV_SIO0_LD1",
#endif	// #if (SIO_SETUP_USE_APTIO_4_STYLE==1)
                                            &ssg,
                                            NULL,
                                            &VariableSize,
                                            &SioDevNvData); 
    
    if(!EFI_ERROR(Status) && (SioDevNvData.DevEnable != 0)) {
      switch(SioDevNvData.DevPrsId) {
        default:
        case 0:
        case 1:
        case 2:
            AmiSioLibSetLpcDeviceDecoding(NULL, 0x3F8, 1, dsUART); 
            break;
        case 3:
            AmiSioLibSetLpcDeviceDecoding(NULL, 0x2F8, 1, dsUART); 
            break;
        case 4:
            AmiSioLibSetLpcDeviceDecoding(NULL, 0x3E8, 1, dsUART); 
            break;
        case 5:
            AmiSioLibSetLpcDeviceDecoding(NULL, 0x2E8, 1, dsUART); 
            break;
      }
    }
    
    // Get LPT decode setting from variable
    Status = ReadOnlyVariable->GetVariable (ReadOnlyVariable,
#if (SIO_SETUP_USE_APTIO_4_STYLE==1)		// [ EIP435639 ]
											L"PNP0501_2_NV",
#else
                                            L"NV_SIO0_LD2",
#endif	// #if (SIO_SETUP_USE_APTIO_4_STYLE==1)
                                            &ssg,
                                            NULL,
                                            &VariableSize,
                                            &SioDevNvData); 
    
    if(!EFI_ERROR(Status) && (SioDevNvData.DevEnable != 0)) {
      switch(SioDevNvData.DevPrsId) {
        default:
        case 0:
        case 1:
        case 2:
        case 5:
            AmiSioLibSetLpcDeviceDecoding(NULL, 0x378, 0, dsLPT); 
            break;
        case 3:
        case 6:     
            AmiSioLibSetLpcDeviceDecoding(NULL, 0x278, 0, dsLPT);
            break;
        case 4:
        case 7:
            AmiSioLibSetLpcDeviceDecoding(NULL, 0x3BC, 0, dsLPT);
            break;
      }    
    }  
}

EFI_STATUS
EFIAPI
OverrideFprrEndOfPei (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
    EFI_BOOT_MODE           BootMode;
    EFI_PEI_HOB_POINTERS    HobPtr;
    UINTN                   Index;
    PCH_CONFIG_HOB          *PchConfigHob;
        
    BootMode = GetBootModeHob ();

    if (BootMode != BOOT_IN_RECOVERY_MODE)
        return EFI_SUCCESS;
    
    DEBUG((DEBUG_INFO, "Clear FPRR settings\n"));
    
    HobPtr.Guid   = GetFirstGuidHob (&gPchConfigHobGuid);
    ASSERT (HobPtr.Guid != NULL);
    PchConfigHob = (PCH_CONFIG_HOB *) GET_GUID_HOB_DATA (HobPtr.Guid);
    
    for (Index = 0; Index < PCH_FLASH_PROTECTED_RANGES; Index++) {
        PchConfigHob->ProtectRange[Index].WriteProtectionEnable = 0;
        PchConfigHob->ProtectRange[Index].ReadProtectionEnable = 0;
        PchConfigHob->ProtectRange[Index].ProtectedRangeBase = 0;
        PchConfigHob->ProtectRange[Index].ProtectedRangeLimit = 0;
    }
    return EFI_SUCCESS;
}

VOID
AmiUpdatePchFlashProtectedRange (
    IN CONST EFI_PEI_SERVICES   **PeiServices,
    IN SB_SETUP_DATA            *SbSetupData,
    IN OUT SI_POLICY_PPI        *SiPolicy 
)
{
    EFI_STATUS                      Status;
    PCH_FLASH_PROTECTION_CONFIG     *FlashProtectionConfig;
    AMI_SPI_PROTECTED_RANGE_CONIFG  AmiSpiProtectedRange[] = { SPI_PROTECTED_RANGE_0, SPI_PROTECTED_RANGE_1, SPI_PROTECTED_RANGE_2, SPI_PROTECTED_RANGE_3, SPI_PROTECTED_RANGE_4 };
    UINTN                           Index;
    PCH_SPI_PPI                     *SpiPpi;
    UINT32                          BaseAddr = 0;
    UINT32                          RegionSize = 0;
    EFI_BOOT_MODE                   BootMode;
    
    BootMode = GetBootModeHob ();
    
    // if Setup is not enable or bootmode is BOOT_ON_FLASH_UPDATE/BOOT_IN_RECOVERY_MODE, skip program FPRR register.
    if (SbSetupData->FprrEnable == 0 || (BootMode == BOOT_ON_FLASH_UPDATE) || (BootMode == BOOT_IN_RECOVERY_MODE)) {
        DEBUG((DEBUG_INFO, "Skip programming FlashProtectedRange(BootMode = %x)\n", BootMode));
        return;
    }
    
    Status = GetConfigBlock ((VOID *) SiPolicy, &gFlashProtectionConfigGuid, (VOID *) &FlashProtectionConfig);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) {
        DEBUG((DEBUG_ERROR, "ERROR! Fail to get FlashProtectionConfig.(%r)\n", Status));
        return;
    }
    
    Status = (*PeiServices)->LocatePpi (PeiServices, 
                                        &gPchSpiPpiGuid,
                                        0,
                                        NULL,
                                        (VOID **) &SpiPpi);   
    if (EFI_ERROR(Status)) {
        DEBUG((DEBUG_ERROR, "ERROR! Fail to locate Spi ppi.(%r)\n", Status));
        return;
    }
    
    for (Index =0; Index < PCH_FLASH_PROTECTED_RANGES; Index++) {
        FlashProtectionConfig->ProtectRange[Index].WriteProtectionEnable = FALSE;
        FlashProtectionConfig->ProtectRange[Index].ReadProtectionEnable = FALSE;
        FlashProtectionConfig->ProtectRange[Index].ProtectedRangeBase = 0;
        FlashProtectionConfig->ProtectRange[Index].ProtectedRangeLimit = 0;
        
        if ( (AmiSpiProtectedRange[Index].WriteProtectionEnable == FALSE) &&
             (AmiSpiProtectedRange[Index].ReadProtectionEnable == FALSE) ) {
            DEBUG ((DEBUG_INFO, "WriteProtectionEnable(%d) and ReadProtectionEnable(%d) are disabled\n", Index, Index));
            continue;
        }
        
        if ( (AmiSpiProtectedRange[Index].ProtectedRangeLength == 0) ||
             (AmiSpiProtectedRange[Index].ProtectedRangeLength % ALIGNMENT_4KB != 0) ||
             (AmiSpiProtectedRange[Index].ProtectedRangeBase % ALIGNMENT_4KB != 0) ) {
           DEBUG ((DEBUG_INFO, "Please check ProtectedRangeLength(%d) or ProtectedRangeBase(%d) is incorrect\n", Index, Index));
           ASSERT (0);
           continue;
        }
        
        switch ( AmiSpiProtectedRange[Index].AmiPchSpiRegionType ) {
            case (AmiUndefinedType):
                FlashProtectionConfig->ProtectRange[Index].ProtectedRangeBase = (UINT16)(AmiSpiProtectedRange[Index].ProtectedRangeBase >> 12);
                FlashProtectionConfig->ProtectRange[Index].ProtectedRangeLimit = (UINT16)((AmiSpiProtectedRange[Index].ProtectedRangeBase + AmiSpiProtectedRange[Index].ProtectedRangeLength -1) >> 12);
                FlashProtectionConfig->ProtectRange[Index].WriteProtectionEnable = AmiSpiProtectedRange[Index].WriteProtectionEnable;
                FlashProtectionConfig->ProtectRange[Index].ReadProtectionEnable = AmiSpiProtectedRange[Index].ReadProtectionEnable;
                continue;
                break;
                
            case (AmiDescriptorType):
                Status = SpiPpi->GetRegionAddress (SpiPpi, FlashRegionDescriptor, &BaseAddr, &RegionSize);
                break;
            
            case (AmiBiosType):
                Status = SpiPpi->GetRegionAddress (SpiPpi, FlashRegionBios, &BaseAddr, &RegionSize);
                break;
            
            case (AmiMeType):
                Status = SpiPpi->GetRegionAddress (SpiPpi, FlashRegionMe, &BaseAddr, &RegionSize);
                break;            
        
            case (AmiGbeType):
                Status = SpiPpi->GetRegionAddress (SpiPpi, FlashRegionGbE, &BaseAddr, &RegionSize);  
                break;
            
            case (AmiPlatformDataType):
                Status = SpiPpi->GetRegionAddress (SpiPpi, FlashRegionPlatformData, &BaseAddr, &RegionSize);                 
                break;
            
            default:
                DEBUG ((DEBUG_INFO, "The AmiPchSpiRegionType(%d) = %d is incorrect.\n", Index, AmiSpiProtectedRange[Index].AmiPchSpiRegionType));
                ASSERT (0);
                break;
        }
                
        if (EFI_ERROR(Status)) {
            DEBUG((DEBUG_ERROR, "Index(%d) Region type(%d) Status(%r)\n", Index, AmiSpiProtectedRange[Index].AmiPchSpiRegionType, Status));
            ASSERT_EFI_ERROR (Status);  
            continue;
        }
        
        DEBUG((DEBUG_INFO, "Index(%d), Region Type(%d), BaseAddr = %x, RegionSize = %x\n", Index, AmiSpiProtectedRange[Index].AmiPchSpiRegionType, BaseAddr, RegionSize));
        
        if (AmiSpiProtectedRange[Index].AmiPchSpiRegionType == AmiBiosType) {
            FlashProtectionConfig->ProtectRange[Index].ProtectedRangeBase = (UINT16)((BaseAddr + AmiSpiProtectedRange[Index].ProtectedRangeBase) >> 12);
            FlashProtectionConfig->ProtectRange[Index].ProtectedRangeLimit = (UINT16)((BaseAddr + AmiSpiProtectedRange[Index].ProtectedRangeBase + AmiSpiProtectedRange[Index].ProtectedRangeLength -1) >> 12);
        } else {
            FlashProtectionConfig->ProtectRange[Index].ProtectedRangeBase = (UINT16)(BaseAddr >> 12);
            FlashProtectionConfig->ProtectRange[Index].ProtectedRangeLimit = (UINT16)((BaseAddr + RegionSize) >> 12);
        }
        
        FlashProtectionConfig->ProtectRange[Index].WriteProtectionEnable = AmiSpiProtectedRange[Index].WriteProtectionEnable;
        FlashProtectionConfig->ProtectRange[Index].ReadProtectionEnable = AmiSpiProtectedRange[Index].ReadProtectionEnable;
    }
    
    Status = PeiServicesNotifyPpi (&mOverrideFprrEndOfPeiNotifyList);
    ASSERT_EFI_ERROR(Status);
}

/**
    AmiUpdatePeiPchPolicyPreMem

    @param PchPolicy 
    @retval EFI_SUCCESS
                     
**/
EFI_STATUS
EFIAPI
AmiUpdatePeiPchPolicyPreMem ( 
    IN OUT SI_PREMEM_POLICY_PPI         *SiPreMemPolicy   
    )
{
    DEBUG ((DEBUG_INFO, "AmiUpdatePeiPchPolicyPreMem() - Start\n"));
     
 
    DEBUG ((DEBUG_INFO, "AmiUpdatePeiPchPolicyPreMem() - End\n"));

    return EFI_SUCCESS;
}


/**
    AmiUpdatePeiPchPolicy

    @param PeiServices 
    @param PchPolicy 
    @retval EFI_SUCCESS
                     
**/
EFI_STATUS
EFIAPI
AmiUpdatePeiPchPolicy ( 
    IN OUT SI_POLICY_PPI        *SiPolicy
    )
{
    EFI_STATUS                        Status;
    CONST EFI_PEI_SERVICES            **PeiServices;	 
    EFI_PEI_STALL_PPI                 *StallPpi;
    SB_SETUP_DATA                     SbSetupData       = {0};
    UINTN                             Index             = 0;
    PLATFORM_INFO                     *PlatformInfo;
    PCH_GENERAL_CONFIG                *PchGeneralConfig;  
    PCH_PCIE_CONFIG                   *PcieRpConfig;
    PCH_INTERRUPT_CONFIG              *InterruptConfig;
    PCH_LOCK_DOWN_CONFIG              *LockDownConfig;
    PCH_PM_CONFIG                     *PmConfig;    
    SETUP_DATA                        SetupData = {0};
    EFI_PEI_READ_ONLY_VARIABLE2_PPI   *ReadOnlyVariable = NULL;
    UINTN                             VariableSize = sizeof(SETUP_DATA);
        
    DEBUG ((DEBUG_INFO, "AmiUpdatePeiPchPolicy() - Start\n"));
    
    PeiServices = GetPeiServicesTablePointer ();
    
    Status = GetConfigBlock ((VOID *) SiPolicy, &gPchGeneralConfigGuid, (VOID *) &PchGeneralConfig);
    ASSERT_EFI_ERROR (Status);   
    
    Status = GetConfigBlock ((VOID *) SiPolicy, &gPcieRpConfigGuid, (VOID *) &PcieRpConfig);
    ASSERT_EFI_ERROR (Status);    
    
    Status = GetConfigBlock ((VOID *) SiPolicy, &gInterruptConfigGuid, (VOID *) &InterruptConfig);
    ASSERT_EFI_ERROR (Status);  
    
    Status = GetConfigBlock ((VOID *) SiPolicy, &gLockDownConfigGuid, (VOID *) &LockDownConfig);
    ASSERT_EFI_ERROR (Status); 
    
    Status = GetConfigBlock ((VOID *) SiPolicy, &gPmConfigGuid, (VOID *) &PmConfig);
    ASSERT_EFI_ERROR (Status);    
    
    Status = (*PeiServices)->LocatePpi( \
		               	   	   PeiServices, \
                               &gEfiPeiReadOnlyVariable2PpiGuid, \
                               0, \
                               NULL, \
                               (VOID **) &ReadOnlyVariable );

    if (!EFI_ERROR(Status)) {
      Status = ReadOnlyVariable->GetVariable( \
		                   ReadOnlyVariable, \
                                   L"Setup", \
                                   &gSetupGuid, \
                                   NULL, \
                                   &VariableSize, \
                                   &SetupData );
    }
    
    GetSbSetupData (PeiServices, &SbSetupData, TRUE);
    
    Status = GetPlatformInfo (&PlatformInfo);
    ASSERT_EFI_ERROR (Status);
    OemPchPolicyUpdateCallBack((EFI_PEI_SERVICES **) PeiServices, SiPolicy, PlatformInfo->BoardID);
    
    // Update LPC Decode,       
//ray_override / [XI-Fixed] Issue Fixed : LPC Decode too Late / Removed >>
//    OemUpdateLpcDecode ((EFI_PEI_SERVICES **) PeiServices); 
//ray_override / [XI-Fixed] Issue Fixed : LPC Decode too Late / Removed <<
        
    // Locate Pei Stall Ppi
    Status = (*PeiServices)->LocatePpi( PeiServices, &gEfiPeiStallPpiGuid, 0, NULL, (VOID **) &StallPpi );
    ASSERT_EFI_ERROR (Status);
        
    // Update Audio Setting for OEM
    AmiUpdatePchAudioConfig(PeiServices, &SbSetupData, SiPolicy);
    
    //
    // Update Usb Setting for OEM
    //
    AmiUpdatePchUsbConfig(PeiServices, &SbSetupData, SiPolicy);
    
    //
    // Update Pch Device Interrupt setting
    //
    AmiUpdatePchDevIntConfig(PeiServices, &SbSetupData, SiPolicy);

    //
    // Update Pch FPRR settings
    //
    AmiUpdatePchFlashProtectedRange(PeiServices, &SbSetupData, SiPolicy);
    
    //
    // Update SATA Setting for OEM
    //
    
    //
    // Update PCIE Setting for OEM
    //
    for (Index = 0; Index < GetPchMaxPciePortNum (); Index++) {
      PcieRpConfig->RootPort[Index].PhysicalSlotNumber             = (UINT8) Index + 4;
// AMI_OVERRIDE_NCD_START >>> EIP222861 Implement Detect Non-Compliance Device item function (Parts of South Bridge) 
      //Enhance Detect Non-Compliance PCIE Device
      if ((SbSetupData.PcieRPDetectNonComplaint[Index] == 1) && (SbSetupData.PcieRootPortEn[Index] != 0))
         SbPcieDetectNonComplaintPcieDevice((EFI_PEI_SERVICES **) PeiServices, StallPpi, Index, SiPolicy, &SbSetupData);
// AMI_OVERRIDE_NCD_END <<< EIP222861 Implement Detect Non-Compliance Device item function (Parts of South Bridge) 
    }
    //
    // Update Serial IO Setting for OEM
    //
    
    //
    // Thermal configuration - Initialize policy to default values when variable isn't found.
    //
    
    //
    // Update Legacy Interrupt Setting for OEM
    //
    for (Index=0;Index<PCH_MAX_PXRC_CONFIG;Index++){
        InterruptConfig->PxRcConfig[Index] = AmiPxRcConfig[Index];
    }
    
    LockDownConfig->SpiEiss = SMM_BIOS_WRITE_PROTECT;
    
#ifndef INSTALL_INTEL_SSID
    //
    // Disable RC update SVID and SSID
    //
    PchGeneralConfig->SubSystemVendorId = 0;
    PchGeneralConfig->SubSystemId = 0;
#endif
    
    PmConfig->PchSlpS3MinAssert   = PCH_SLP_S3_MIN_ASSERT_VALUE;
    PmConfig->PchSlpS4MinAssert   = PCH_SLP_S4_MIN_ASSERT_VALUE;
    PmConfig->PchSlpSusMinAssert  = PCH_SLP_SUS_MIN_ASSERT_VALUE;
    PmConfig->PchSlpAMinAssert    = PCH_SLPA_MIN_ASSERT;
    PmConfig->PchPwrCycDur        = PCH_RESET_CYCLE_DURATION;
    PmConfig->SlpStrchSusUp       = SLP_STRCH_SUS_UP;
    
#ifndef AMI_CRB_EC_SUPPORT_FLAG    
    // PeiPlatformHooklib.c
    if (SetupData.UsbTypeCAicSupport){
      PcdSetBoolS(PcdUsbTypeCSupport, TRUE);
      DEBUG((DEBUG_INFO, "USB Type-C Support Enabled \n"));
    } else {
      PcdSetBoolS(PcdUsbTypeCSupport, FALSE);
      DEBUG((DEBUG_INFO, "USB Type-C Support Disabled \n"));
    }
#endif    
    
    DEBUG ((DEBUG_INFO, "AmiUpdatePeiPchPolicy() - End\n"));

    return EFI_SUCCESS;
}
