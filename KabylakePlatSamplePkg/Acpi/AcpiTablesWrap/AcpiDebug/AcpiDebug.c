/** @file
  These routines get called during ACPI Platform Init to install the ACPI table.

@copyright
 Copyright (c) 2014 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains a 'Sample Driver' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may be modified by the user, subject to
 the additional terms of the license agreement.

@par Specification Reference:
**/

#include "AcpiDebug.h"

EFI_GUID gAcpiDebugTableStorageGuid = ACPI_DEBUG_TABLE_STORAGE_GUID;

/**
  Routine to search the FV for compiled AcpiDebug SSDT AML binary and install it.

  @retval EFI_SUCCESS    IRMT ACPI tables are initialized successfully
  @retval EFI_NOT_FOUND  IRMT ACPI tables not found
**/
EFI_STATUS
InitializeAcpiDebugAcpiTables (
  VOID
  ) 
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         NumberOfHandles;
  EFI_FV_FILETYPE               FileType;
  UINT32                        FvStatus;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINTN                         Size;
  UINTN                         Index;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *FwVol;
  INTN                          Instance;
  EFI_ACPI_COMMON_HEADER        *CurrentTable;
  UINTN                         TableHandle;
  EFI_ACPI_TABLE_PROTOCOL       *AcpiTable;
  EFI_ACPI_DESCRIPTION_HEADER   *TableHeader;
  UINT8                         *CurrPtr;
  UINT32                        *Signature;
  NAME_LAYOUT            		*NamePtr;
  UINT8                         UpdateCounter;
  EFI_PHYSICAL_ADDRESS          BaseAddressMem = 0;
  UINT32                        BufferIndex;
  UINT32                        BufferEnd;
  ACPI_SETUP_DATA 				mSystemConfiguration = {0};  
  
  FwVol         = NULL;

  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, &AcpiTable);
  DEBUG((DEBUG_INFO, "AcpiDebug :gBS->LocateProtocol->AcpiTable Status = %r \n", Status));
  ASSERT_EFI_ERROR (Status);

  //
  // Locate FV Protocol. There is little chance we can't find an FV protocol
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  DEBUG((DEBUG_INFO, "AcpiDebug :gBS->LocateHandleBuffer Status = %r\n", Status));
  ASSERT_EFI_ERROR (Status);
  //
  // Looking for FV with ACPI storage file
  //
  for (Index = 0; Index < NumberOfHandles; Index++) {
    //
    // Get the protocol on this handle
    // This should not fail because of LocateHandleBuffer
    //
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    &FwVol
                    );
    DEBUG((DEBUG_INFO, "AcpiDebug :gBS->HandleProtocol Status = %r\n", Status));
    ASSERT_EFI_ERROR (Status);

    //
    // See if it has the ACPI storage file
    //
    Size      = 0;
    FvStatus  = 0;
    Status = FwVol->ReadFile (
                      FwVol,
                      &gAcpiDebugTableStorageGuid,
                      NULL,
                      &Size,
                      &FileType,
                      &Attributes,
                      &FvStatus
                      );
    DEBUG((DEBUG_INFO, "AcpiDebug :FwVol->ReadFile Status = %r\n", Status));
    //
    // If we found it, then we are done
    //
    if (Status == EFI_SUCCESS) {
      break;
    }
  }
  //
  // Free any allocated buffers
  //
  gBS->FreePool (HandleBuffer);

  //
  // Sanity check that we found our data file
  //
  ASSERT (FwVol != NULL);
  if (FwVol == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // Our exit status is determined by the success of the previous operations
  // If the protocol was found, Instance already points to it.
  // Read tables from the storage file.
  //
  Instance      = 0;
  CurrentTable  = NULL;
  while (Status == EFI_SUCCESS) {
    Status = FwVol->ReadSection (
                      FwVol,
                      &gAcpiDebugTableStorageGuid,
                      EFI_SECTION_RAW,
                      Instance,
                      &CurrentTable,
                      &Size,
                      &FvStatus
                      );
    DEBUG((DEBUG_INFO, "AcpiDebug :FwVol->ReadSection Status = %r\n", Status));

    if (!EFI_ERROR (Status)) {
    	
      TableHeader = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
      //
      // Look for AcpiDebugTable table.
      //
      if (((EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable)->OemTableId == SIGNATURE_64 ('A', 'D', 'e', 'b', 'T', 'a', 'b', 'l')) {
        DEBUG((DEBUG_INFO, "AcpiDebug: Publishing ADebTabl\n"));
        
        GetAcpiSetupData (gRT, &mSystemConfiguration);
//        Size = sizeof(SETUP_DATA);
//    	Status = gRT->GetVariable(
//    	                          L"Setup",
//    	                          &gSetupVariableGuid,
//    	                          NULL,
//    	                          &Size,
//    	                          &mSystemConfiguration
//    	                          );
//    	ASSERT_EFI_ERROR (Status);
        
        if (mSystemConfiguration.AcpiDebug) {
          
  	      BaseAddressMem = 0xffffffff;
  	      Status  = gBS->AllocatePages (
  	                       AllocateMaxAddress,
  	                       EfiReservedMemoryType,
  	                       EFI_SIZE_TO_PAGES (AcpiDebugBufferSize),
  	                       &BaseAddressMem
  	                       );
  	      ASSERT_EFI_ERROR (Status);
          BufferIndex = (UINT32) BaseAddressMem;
          BufferEnd = BufferIndex + AcpiDebugBufferSize;
  	       //
  	       // Clear the 64kb buffer
  	       //
          gBS->SetMem ((VOID *)(UINTN)BaseAddressMem, AcpiDebugBufferSize, 0x78);
  	      //
  	      // Write a signature to the first line of the buffer, "INTEL ACPI DEBUG".
  	      //
          gBS->CopyMem ((VOID *)(UINTN)BufferIndex, ACPI_DEBUG_STR, sizeof(ACPI_DEBUG_STR) - 1);
  	      //
  	      // leave the Index after the signature
  	      //
  	      BufferIndex += sizeof(ACPI_DEBUG_STR) - 1;

       
          //
          // Patch some pointers for the ASL code before loading the SSDT.
          //
  	      UpdateCounter = 1;
//AMI_OVERRIDE_ACPI0018_START >>> EIP244373 Static code analysis issues found in AptioV SkylakePlatPkg module.  	      		  
#if 0
  	      for (CurrPtr = (UINT8 *) TableHeader; CurrPtr <= ((UINT8 *) TableHeader + TableHeader->Length), UpdateCounter < 4; CurrPtr++) {
#else
  	      for (CurrPtr = (UINT8 *) TableHeader; CurrPtr <= ((UINT8 *) TableHeader + TableHeader->Length) && UpdateCounter < 4; CurrPtr++) { 
#endif
//AMI_OVERRIDE_ACPI0018_END <<< Static code analysis issues found in AptioV SkylakePlatPkg module.   	    	  
  	         Signature = (UINT32 *) (CurrPtr + 1);
  	         //
  	         // patch DPTR (address of Acpi debug memory buffer)
  	         //
  	         if ((*CurrPtr == AML_NAME_OP) && *Signature == SIGNATURE_32 ('D', 'P', 'T', 'R')) {
  	           NamePtr = (NAME_LAYOUT *) CurrPtr;
  	           NamePtr->Value  = (UINT32) BaseAddressMem;
  	           UpdateCounter++;
  	         }
  	         //
  	         // patch EPTR (end of Acpi debug memory buffer)
  	         //
  	         if ((*CurrPtr == AML_NAME_OP) && *Signature == SIGNATURE_32 ('E', 'P', 'T', 'R')) {
  	           NamePtr = (NAME_LAYOUT *) CurrPtr;
  	           NamePtr->Value  = (UINT32) BufferEnd;
  	           UpdateCounter++;
  	         }
  	         //
  	         // patch CPTR (used as an index that starts after the buffer signature)
  	         //
  	         if ((*CurrPtr == AML_NAME_OP) && *Signature == SIGNATURE_32 ('C', 'P', 'T', 'R')) {
               NamePtr = (NAME_LAYOUT *) CurrPtr;
               NamePtr->Value  = (UINT32) BufferIndex;
               UpdateCounter++;
             }
          }

          //
          // Install the table
          //
          TableHandle = 0;
          Status = AcpiTable->InstallAcpiTable (
                              AcpiTable,
                              CurrentTable,
                              CurrentTable->Length,
                              &TableHandle
                              );
        } // if (mSystemConfiguration.AcpiDebugSupport) {
        return EFI_SUCCESS;

      }
      //
      // Increment the instance
      //
      Instance++;
      CurrentTable = NULL;
    }
  }
  return Status;
}


/**
  Callback to install AcpiDebug ACPI table at ready to boot. 

  @param[in] Event    The event that triggered this notification function
  @param[in] Context  Pointer to the notification functions context
**/
VOID
AcpiDebugOnReadyToBoot (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  ) 
{
  EFI_STATUS  Status;

  DEBUG ((DEBUG_INFO, "AcpiDebugOnReadyToBoot()\n"));

  Status = InitializeAcpiDebugAcpiTables ();
  DEBUG((DEBUG_INFO, "Initializes AcpiDebug SSDT tables Status = %r \n", Status));

  gBS->CloseEvent (Event);

}

/**
  AcpiDebug ACPI driver entry point function

  @param[in] ImageHandle Image handle for this driver image
  @param[in] SystemTable Pointer to the EFI System Table

  @retval EFI_OUT_OF_RESOURCES - no enough memory resource when installing reference code information protocol
  @retval EFI_SUCCESS          - function completed successfully
**/
EFI_STATUS
EFIAPI
InstallADebTabl (
  IN EFI_HANDLE                      ImageHandle,
  IN EFI_SYSTEM_TABLE                *SystemTable
  )
{
  EFI_STATUS                    Status;
  EFI_EVENT                     Event;

  DEBUG ((DEBUG_INFO, "InstallADebTabl: Entry Point...\n"));

  //
  // Register ready to boot event for installing AcpiDebug and updating IGD op-region
  //
  Status = EfiCreateEventReadyToBootEx (
            TPL_NOTIFY,
            AcpiDebugOnReadyToBoot,
            NULL,
            &Event
            );
  DEBUG((DEBUG_INFO, "Create ReadyToBoot event for AcpiDebug Status = %r\n", Status));

  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }
  DEBUG ((DEBUG_INFO, "InstallADebTabl: Entry End...\n"));

  return EFI_SUCCESS;
}
