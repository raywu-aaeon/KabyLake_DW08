/** @file SbPciDxeInitLib.c
    This file provide whole SB devices Pci init routine for PciBus
    call out.

    This template just provide two example.
    If you want append or modify please follow below naming rule.

    1. Append CspSbPciInit prior to your particular device name.
    for example : CspSbPciInitPcieSwapping, CspSbPciInitXXX

**/
//*************************************************************************

#include <AmiDxeLib.h>
#include <Token.h>
#include <PchAccess.h>
#include <PciBus.h>
#include <PciHostBridge.h>
#include <Setup.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciIo.h>
#include <Protocol/AmiBoardInitPolicy.h>
#include <Library/AmiPciBusLib.h>
#include <Library/PchPcieRpLib.h>
#include <Library/PchInfoLib.h>
#include <Library/DebugLib.h>

BOOLEAN gDev1bChecked = FALSE;
BOOLEAN gDev1cChecked = FALSE;
BOOLEAN gDev1dChecked = FALSE;

/**
    Remap Port 0 to which is replaced to
        
    @param  AMI_BOARD_INIT_PROTOCOL*
            UINTN*
            AMI_BOARD_INIT_PARAMETER_BLOCK*

    @retval EFI_STATUS
**/
EFI_STATUS EFIAPI CspSbPciInitPcieSwapping (
    IN AMI_BOARD_INIT_PROTOCOL              *This,
    IN UINTN                                *Function,
    IN OUT AMI_BOARD_INIT_PARAMETER_BLOCK   *ParameterBlock
)
{
    // Update Standard parameter block
    AMI_BOARD_INIT_PARAMETER_BLOCK  *Args = ParameterBlock;
    PCI_INIT_STEP                   InitStep = (PCI_INIT_STEP)Args->InitStep;
    PCI_DEV_INFO                    *dev = (PCI_DEV_INFO*)Args->Param1;
    EFI_STATUS                      Status = EFI_UNSUPPORTED;
    UINTN                           RpIndex;


    //---------------------------------
    // Check if parameters passed are VALID and
    if(Args->Signature != AMI_PCI_PARAM_SIG) return EFI_INVALID_PARAMETER;
    if(InitStep>=isPciMaxStep) return EFI_INVALID_PARAMETER;

    // ==============================================================
    // 
    // ==============================================================
    if (InitStep == isPciQueryDevice) {
        AMI_BOARD_INFO2_PROTOCOL    *AmiBoardInfo2Protocol = NULL;
        AMI_SDL_PCI_DATA            *SdlPciData = NULL;
        UINT32                      i = 0;
        UINT32                      Port0Index = 0;
        UINT32                      SwappedIndex = 0;
        AMI_SDL_PCI_DEV_INFO        *SdlPciDevInfo;
        PCH_SERIES                  PchSeries;

        
        PchSeries = GetPchSeries ();
        //
        // Get PCIDEVICE SDL Data of AmiBoardInfo2Protocol
        //
        Status = pBS->LocateProtocol(&gAmiBoardInfo2ProtocolGuid, NULL,(VOID**)&AmiBoardInfo2Protocol);
        if(EFI_ERROR(Status)){
            DEBUG((DEBUG_ERROR, "ERROR: Failed to locate AMI_BOARD_INFO2_PROTOCOL. Status=%r\n",Status));
            return Status;
        } else {
            SdlPciData = AmiBoardInfo2Protocol->PciBrdData;
        }
        //
        // Find out the index number of B0:D28:F0-7 (P0-P7)
        //
        if (!gDev1cChecked) {
          for (i = 0; i < SdlPciData->PciDevCount; i++){
            SdlPciDevInfo = &SdlPciData->PciDevices[i];
            if ((SdlPciDevInfo->Device == PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1)&&(SdlPciDevInfo->Function == 0)) {
              Port0Index = i;
//              DEBUG((DEBUG_INFO, "\nB0:D1C:F0 Port0Index = 0x%x\n", Port0Index));

              GetPchPcieRpNumber ( \
                      PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1, \
                      PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_1, \
                      &RpIndex);
//              DEBUG((DEBUG_INFO, "B0:D1C:F0 Physical Port Number = 0x%x (0-based)\n", RpIndex));
            }
            
            
            if ((SdlPciDevInfo->Device == PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_1)&&(SdlPciDevInfo->Function == RpIndex)) {
                SwappedIndex = i;
//                DEBUG((DEBUG_INFO, "B0:D1C:F%x SwappedIndex = 0x%x \n", RpIndex, SwappedIndex));
       	    }
          }
          //
          // Replace the ParentIndex to Port1 if the devices is behind Portx
          //
          if (Port0Index != SwappedIndex) {
            for (i = 0; i < SdlPciData->PciDevCount; i++){
              SdlPciDevInfo = &SdlPciData->PciDevices[i];
              if (SdlPciDevInfo->ParentIndex == SwappedIndex) {
                SdlPciDevInfo->ParentIndex = Port0Index;
//                  DEBUG((DEBUG_INFO, "Port0Index and SwappedIndex swapped!\n"));
              }
            }
          }
          gDev1cChecked = TRUE;
        }
        
        //
        // Find out the index number of B0:D29:F0-7 (P8-P15)
        //
        if (!gDev1dChecked) {
          for (i = 0; i < SdlPciData->PciDevCount; i++){
            SdlPciDevInfo = &SdlPciData->PciDevices[i];
            if ((SdlPciDevInfo->Device == PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2)&&(SdlPciDevInfo->Function == 0)) {
              Port0Index = i;
//              DEBUG((DEBUG_INFO, "\nB0:D1D:F0 Port0Index = 0x%x\n", Port0Index));
              GetPchPcieRpNumber ( \
                      PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2, \
                      PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_9, \
                      &RpIndex);
//              DEBUG((DEBUG_INFO, "B0:D1D:F0 Physical Port Number = 0x%x (0-based)\n", RpIndex));
              RpIndex -= 8; //Port 8 ~ 15
            }
            
            if ((SdlPciDevInfo->Device == PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_2)&&(SdlPciDevInfo->Function == RpIndex)) {
              SwappedIndex = i;
//              DEBUG((DEBUG_INFO, "B0:D1D:F%x SwappedIndex = 0x%x \n", RpIndex, SwappedIndex));
            }
          }
          //
          // Replace the ParentIndex to Port1 if the devices is behind Portx
          //
          if (Port0Index != SwappedIndex) {
            for (i = 0; i < SdlPciData->PciDevCount; i++){
              SdlPciDevInfo = &SdlPciData->PciDevices[i];
              if (SdlPciDevInfo->ParentIndex == SwappedIndex) {
                SdlPciDevInfo->ParentIndex = Port0Index;
//                DEBUG((DEBUG_INFO, "Port0Index and SwappedIndex swapped!\n"));
              }
            }
          }
          gDev1dChecked = TRUE;
        }
        
        if (PchSeries == PchH) {
          //
          // Find out the index number of B0:D27:F0-3 (P16-P19)
          //
          if (!gDev1bChecked) {
            for (i = 0; i < SdlPciData->PciDevCount; i++){
              SdlPciDevInfo = &SdlPciData->PciDevices[i];
              if ((SdlPciDevInfo->Device == PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_3)&&(SdlPciDevInfo->Function == 0)) {
                Port0Index = i;
//                DEBUG((DEBUG_INFO, "\nB0:D1B:F0 Port0Index = 0x%x\n", Port0Index));
                GetPchPcieRpNumber ( \
                        PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_3, \
                        PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_17, \
                        &RpIndex);
//                DEBUG((DEBUG_INFO, "B0:D1B:F0 Physical Port Number = 0x%x (0-based)\n", RpIndex));
                RpIndex -= 16; //Port 16 ~ 19
              }
            
              if ((SdlPciDevInfo->Device == PCI_DEVICE_NUMBER_PCH_PCIE_DEVICE_3)&&(SdlPciDevInfo->Function == RpIndex)) {
                SwappedIndex = i;
//                DEBUG((DEBUG_INFO, "B0:D1B:F%x SwappedIndex = 0x%x \n", RpIndex, SwappedIndex));
              }
            }
            //
            // Replace the ParentIndex to Port1 if the devices is behind Portx
            //
            if (Port0Index != SwappedIndex) {
              for (i = 0; i < SdlPciData->PciDevCount; i++){
                SdlPciDevInfo = &SdlPciData->PciDevices[i];
                if (SdlPciDevInfo->ParentIndex == SwappedIndex) {
                  SdlPciDevInfo->ParentIndex = Port0Index;
//                  DEBUG((DEBUG_INFO, "Port0Index and SwappedIndex swapped!\n"));
                }
              }
            }
            gDev1bChecked = TRUE;
          }
        } // (PchSeries == PchH)
    } // (InitStep == isPciQueryDevice)

    return Status;
}

PCI_BAD_BAR gAmiSbBadPciDevList[]={ 
//Intel EMMC Interface  B0 D30 F4
{icBad64BitBar, 0x8086,     0x9D2B,     0xFFFF,     0x0000,     tBarMaxType,0x0010,     0,          0           }, 
//Intel SDIO  B0 D30 F5
{icBad64BitBar, 0x8086,     0x9D2C,     0xFFFF,     0x0000,     tBarMaxType,0x0010,     0,          0           }, 
//Intel SDXC  B0 D30 F6
{icBad64BitBar, 0x8086,     0x9D2D,     0xFFFF,     0x0000,     tBarMaxType,0x0010,     0,          0           }, 		
//-----------------------------------------------------------------------------------------------------------------------
//enum			UINT16		UINT16		UINT16		UINT16	  PCI_BAR_TYPE 	UINTN		UINT64		UINT64
//IncompType;	VendorId;	DeviceId;	DevIdMask;	Reserved;	BarType;	BarOffset;	BarBase;	BarLength;
//-----------------------------------------------------------------------------------------------------------------------
{icBad64BitBar, 0x8086,     0x9D60,     0xFFFF,     0x0000,     tBarMaxType,0x0010,     0,          0           }, 		
{icBad64BitBar, 0x8086,     0x9D61,     0xFFFF,     0x0000,     tBarMaxType,0x0010,     0,          0           }, 		
{icBad64BitBar, 0x8086,     0x9D62,     0xFFFF,     0x0000,     tBarMaxType,0x0010,     0,          0           }, 		
{icBad64BitBar, 0x8086,     0x9D63,     0xFFFF,     0x0000,     tBarMaxType,0x0010,     0,          0           }, 		
{icBad64BitBar, 0x8086,     0x9D64,     0xFFFF,     0x0000,     tBarMaxType,0x0010,     0,          0           }, 		
{icBad64BitBar, 0x8086,     0x9D65,     0xFFFF,     0x0000,     tBarMaxType,0x0010,     0,          0           }, 		
		
};
UINTN gAmiSbBadPciDevCount=sizeof(gAmiSbBadPciDevList)/sizeof(PCI_BAD_BAR);

EFI_STATUS EFIAPI CspSbBadPciDevOverride (
    IN AMI_BOARD_INIT_PROTOCOL              *This,
    IN UINTN                                *Function,
    IN OUT AMI_BOARD_INIT_PARAMETER_BLOCK   *ParameterBlock
)
{
	
    AMI_BOARD_INIT_PARAMETER_BLOCK  *Args    = ParameterBlock;
    PCI_INIT_STEP                   InitStep = (PCI_INIT_STEP)Args->InitStep;
    PCI_DEV_INFO                    *dev     = (PCI_DEV_INFO*)Args->Param1;
    EFI_STATUS                      Status   = EFI_UNSUPPORTED;
    UINTN                           PciDevCount;
    UINTN                           BarCount;
    PCI_BAD_BAR                     *AmiSbBadPciDevice;
    UINT32                          PcieDevVenId;

    if (InitStep == isPciQueryDevice) {
      for (PciDevCount = 0; PciDevCount < gAmiSbBadPciDevCount; PciDevCount++) {
    	  AmiSbBadPciDevice = &gAmiSbBadPciDevList[PciDevCount];
    	if (AmiSbBadPciDevice->IncompType == icBad64BitBar) {
    		PcieDevVenId = *(UINT32*)&AmiSbBadPciDevice->VendorId;
    	   if (PcieDevVenId == dev->DevVenId.DEV_VEN_ID) {
              for(BarCount = 0; BarCount <= PCI_MAX_BAR_NO; BarCount++) {
                if( dev->Bar[BarCount].Offset == AmiSbBadPciDevice->BarOffset) {
                  if ( dev->Bar[BarCount].Type == tBarMmio64 ) { // Checking the allocate type.
                    dev->Bar[BarCount].Type = tBarMmio32;
                  } else if ( dev->Bar[BarCount].Type == tBarMmio64pf) {
                    dev->Bar[BarCount].Type = tBarMmio32pf;
                  }
                  Status = EFI_SUCCESS;  // Devices are match in the bad list, bios returns EFI_SUCCESS.
                }
              }  
    	   }
         } // if (AmiSbBadPciDevice->IncompType == icBad64BitBar)
      }
      return Status;
    } // isPciQueryDevice
    
    if (InitStep == isPciSkipDevice) {

        return Status;
    }

    return Status;
}

