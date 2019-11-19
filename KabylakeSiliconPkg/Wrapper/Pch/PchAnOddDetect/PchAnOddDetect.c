//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//**********************************************************************
// Revision History
// ----------------
// $Log:$
// 
// 
//**********************************************************************
//<AMI_FHDR_START>
//
// Name: AnOddDetect.c
//
// Description:	Code to enable WRV on disk drives
//
//<AMI_FHDR_END>
//**********************************************************************
#include <AhciBus.h>
#include <Token.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/AtaPassThru.h>
#include <Library/DebugLib.h>
#include <Register/PchRegsSata.h>
#include <PchAccess.h>
#include <Library/MmPciLib.h>
#include <Library/PchInfoLib.h>

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   SendIdentifyCommand
//
// Description: Send the Identify Command for the given ATA Pass Protocol 
//               and outputs the Identify Data
//
// Input:       EFI_ATA_PASS_THRU_PROTOCOL AtaPassThru   
//                                   - The ATA Pass Thru Protocol on the 
//                                      handle
//              UINT16 CurrentPort   - The port number of the current device
//              UINT16 CurrentDevice - The current device number
//
// Output:      IDENTIFY_DATA *IdentifyData - The Identify Data of the device
//
// Returns:     EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS SendIdentifyCommand(
    IN  EFI_ATA_PASS_THRU_PROTOCOL *AtaPassThru,
    IN  UINT16                     CurrentPort,
    IN  UINT16                     CurrentDevice,
    OUT IDENTIFY_DATA              *IdentifyData)
{
  EFI_STATUS                       Status          = EFI_SUCCESS;
  EFI_ATA_PASS_THRU_COMMAND_PACKET CommandPacket;
  EFI_ATA_STATUS_BLOCK             AtaStatusBlock;
	EFI_ATA_COMMAND_BLOCK            AtaCommandBlock;

  gBS->SetMem(&AtaStatusBlock, sizeof(EFI_ATA_STATUS_BLOCK), 0);
  gBS->SetMem(&AtaCommandBlock, sizeof(EFI_ATA_COMMAND_BLOCK), 0);
  gBS->SetMem(&CommandPacket, sizeof(EFI_ATA_PASS_THRU_COMMAND_PACKET), 0);
  
  // Setup the Command Block for the Command Packet for 
  AtaCommandBlock.AtaCommand = IDENTIFY_PACKET_COMMAND; // Identify Packet for ODDs

  // Setup the Command Packet for Identify Command
  CommandPacket.Asb               = &AtaStatusBlock;
  CommandPacket.Acb               = &AtaCommandBlock;
  CommandPacket.Timeout           = 30000000; // 30000000*100ns = 3 seconds
  CommandPacket.InDataBuffer      = (VOID *) IdentifyData;
  CommandPacket.OutDataBuffer     = NULL;
  CommandPacket.InTransferLength  = sizeof(IDENTIFY_DATA);
  CommandPacket.OutTransferLength = 0;
  CommandPacket.Protocol          = EFI_ATA_PASS_THRU_PROTOCOL_PIO_DATA_IN; 
  CommandPacket.Length            = EFI_ATA_PASS_THRU_LENGTH_BYTES | EFI_ATA_PASS_THRU_LENGTH_SECTOR_COUNT;

  // Send the Identify Command
  Status = AtaPassThru->PassThru(AtaPassThru, CurrentPort, CurrentDevice, &CommandPacket, NULL);
  
  return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   AnOddInit
//
// Description: Handles the PCH programming for Async Notify ODD support
//
// Returns:     EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS PchAnOddInitEntry (    
	IN EFI_HANDLE          ImageHandle,
	IN EFI_SYSTEM_TABLE    *SystemTable
) 
{
	EFI_ATA_PASS_THRU_PROTOCOL    *PassThru;
	EFI_STATUS                    Status;
	UINT16                        Port = 0xFFFF;
	UINT16                        Device = 0xFFFF;
	UINT32                        Data32Or;
	UINT32                        DisableDynamicPowerGateBitMap = 0;
	UINTN                         PciSataRegBase = MmPciBase (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SATA, PCI_FUNCTION_NUMBER_PCH_SATA);
	PCH_SERIES                    Pch = GetPchSeries();
  IDENTIFY_DATA                 IdentifyData;    
  //UINT32                        SizeOfIdentifyData = sizeof(IDENTIFY_DATA);  //AMI_OVERRIDE >>> Fix cpp check error

  // Locate ATA_PASS_THRU_PROTOCOL
  Status = gBS->LocateProtocol(&gEfiAtaPassThruProtocolGuid, NULL, &PassThru);
	if (EFI_ERROR(Status)) {
      DEBUG((-1, "Locate ATA Pass Thru Protocol\n"));
      return Status;
	}

  // Go through all populated ports to check for AN support
	while (PassThru->GetNextPort(PassThru, &Port) == EFI_SUCCESS) {
		DEBUG((-1, "\nPort Number: %d\n", Port));
		Device = 0xFFFF;
		while (PassThru->GetNextDevice(PassThru, Port, &Device) == EFI_SUCCESS) {
			DEBUG((-1, "->Device: %d\n", Device));

			// Clear and get identify data
		  gBS->SetMem(&IdentifyData, sizeof(IDENTIFY_DATA), 0);
			Status = SendIdentifyCommand(PassThru, Port, Device, &IdentifyData);
			DEBUG((-1, "  ->SendIdentifyCommand(): %r\n", Status));
			if (EFI_ERROR(Status))
				continue;

			// Check for AN support
			if (IdentifyData.Reserved_76_79[2] & BIT5) {	// Word 78, Bit 5 = Supports Asynchronous Notification
			  DEBUG((-1, "    ->Asynchronous Notification supported on this device\n"));
				DisableDynamicPowerGateBitMap |= 1u << Port; // Not 100% sure this mapping is correct
				// Check for AN enabled
				if (IdentifyData.Reserved_76_79[3] & BIT5)	// Word 79, Bit 5 = Asynchronous Notification Enabled
					DEBUG((-1, "      ->Asynchronous Notification enabled on this device\n"));
				else
					DEBUG((-1, "      ->Asynchronous Notification disabled on this device\n"));
			} else
			  DEBUG((-1, "    ->Asynchronous Notification NOT supported on this device\n"));
		}
	}
	
	DEBUG((-1, "\nDisableDynamicPowerGateBitMap: %08x\n", DisableDynamicPowerGateBitMap));
	DEBUG((-1, "Pch: %x\n", Pch));

	if (DisableDynamicPowerGateBitMap != 0x0) {
		if (Pch == PchH) {
			///
			/// Step 1
			/// Set SATA SIR Index 90h[7:0] = 1'S
			///
			Data32Or = 0xFF;
			//
			//  [MPHY Dynamic Power gating] SIR offset 90h[7:0] . bit0=port0...bit7=port7
			//
			Data32Or &= (UINT32)~DisableDynamicPowerGateBitMap;
			MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_90);
			
			DEBUG((-1, "R_PCH_SATA_SIR_90: %x\n", MmioRead32(PciSataRegBase + R_PCH_SATA_STRD)));
			
			// Changed to AND to add disables to previously created disables.
			MmioAnd32 (PciSataRegBase + R_PCH_SATA_STRD, Data32Or);
			
			DEBUG((-1, "R_PCH_SATA_SIR_90: %x\n", MmioRead32(PciSataRegBase + R_PCH_SATA_STRD)));
	
			//
			//  [Slumber Squelch off] SIR offset 80h[16:23] . bit16=port0... bit23=port7
			//
			MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_80);
			DEBUG((-1, "R_PCH_SATA_SIR_80: %x\n", MmioRead32(PciSataRegBase + R_PCH_SATA_STRD)));
			
			MmioOr32 (PciSataRegBase + R_PCH_SATA_STRD, (DisableDynamicPowerGateBitMap << 16));
			
			DEBUG((-1, "R_PCH_SATA_SIR_80: %x\n", MmioRead32(PciSataRegBase + R_PCH_SATA_STRD)));
		} else if (Pch == PchLp) {
			///
			/// Step 1
			/// Set SATA SIR Index 50h[18:16] = 111b
			///
			Data32Or = BIT18 | BIT17 | BIT16;
			//
			//  [MPHY Dynamic Power gating] SIR offset 50h[18:16] bit16=port0... bit18=port2
			//
			Data32Or &= (UINT32)~(DisableDynamicPowerGateBitMap << 16);
			MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_50);
			
			DEBUG((-1, "R_PCH_SATA_SIR_50: %x\n", MmioRead32(PciSataRegBase + R_PCH_SATA_STRD)));
			
			// Changed to AND to add disables to previously created disables.
			MmioAnd32 (PciSataRegBase + R_PCH_SATA_STRD, Data32Or);
	
			DEBUG((-1, "R_PCH_SATA_SIR_50: %x\n", MmioRead32(PciSataRegBase + R_PCH_SATA_STRD)));
	
			///
			/// Step 2
			/// Set SATA SIR Index 54h[30, 26:24, 10:8] = 1's
			///
			Data32Or = 0x47000700;
			//
			//  [Slumber Squelch off] SIR offset 54h[2:0]  bit0=port0 ... bit2=port2
			//
			Data32Or |= DisableDynamicPowerGateBitMap;
			MmioWrite8 (PciSataRegBase + R_PCH_SATA_SIRI, R_PCH_SATA_SIR_54);
			DEBUG((-1, "R_PCH_SATA_SIR_54: %x\n", MmioRead32(PciSataRegBase + R_PCH_SATA_STRD)));
			MmioOr32 (PciSataRegBase + R_PCH_SATA_STRD, Data32Or);
			
			DEBUG((-1, "R_PCH_SATA_SIR_54: %x", MmioRead32(PciSataRegBase + R_PCH_SATA_STRD)));
		}
	}

	DEBUG((-1, "\n"));
	return EFI_SUCCESS;
}



//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
