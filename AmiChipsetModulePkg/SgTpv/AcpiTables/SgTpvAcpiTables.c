//***************************** *****************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		SgTpvAcpiTables.c
//
// Description:	This file contains the routine LoadTpvAcpiTables which installs 
//              Acpi Tables for Tpv's Switchable Graphics. 
//              File is linked with Intel's SwitchableGraphicsDxe module
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#include <SgTpvAcpiTables.h>

extern EFI_GUID       gSaGlobalNvsAreaProtocolGuid;

PCH_SERIES            PchSeries;
UINT8                 EndpointBus;
UINT8                 RootPortDev;
UINT8                 RootPortFun;

SYSTEM_AGENT_GLOBAL_NVS_AREA_PROTOCOL  *SaGlobalNvsArea;

// GUID Definition(s)
EFI_GUID              gSetupGuid     = SETUP_GUID;
SETUP_DATA            *SetupData     = NULL;
NB_SETUP_DATA         NbSetupData; 

//<AMI_THDR_START>
//----------------------------------------------------------------------------
// Procedure: NvidiaOpRegion & AmdOpRegion
//
// Description: Gfx Vendor specific OperationRegion data structures. 
//              Must match ones defined in Asl code
//
//----------------------------------------------------------------------------
//<AMI_THDR_END>
NVIDIA_OPREGION             NvidiaOpRegion;
AMD_OPREGION                AmdOpRegion;


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: LoadTpvAcpiTables
//
// Description:   Load Third part graphics vendor support SSDT Tables
//
// Input: VOID
//
// Output:   
//      EFI_SUCCESS     - SSDT Table load successful. 
//      EFI_UNSUPPORTED - Supported SSDT not found.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
EFIAPI
SgTpvAcpiTablesInit (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )        
{
  UINTN              		  VariableSize = 0;
  EFI_STATUS                    Status;
  EFI_EVENT                     gEvent;
  InitAmiLib(ImageHandle, SystemTable);
  DEBUG ((DEBUG_INFO, "SG_TPV:: SgTpv SgTpvAcpiTablesInit \n"));    
  ///
  ///  Locate the SA Global NVS Protocol.
  ///
  Status = pBS->LocateProtocol (
                  &gSaGlobalNvsAreaProtocolGuid,
                  NULL,
                  (VOID **) &SaGlobalNvsArea
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "SG_TPV:: Locate gSaGlobalNvsAreaProtocolGuid failed.  \n"));  
    return Status;
  }
    
  // Initialize Setup Data 
  Status = GetEfiVariable(
             L"Setup",
             &gSetupGuid,
             NULL,
             &VariableSize,
             &SetupData
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "SG_TPV:: Get Setup variable failed.  \n"));  
    return Status;
  }
  GetNbSetupData (pRS, &NbSetupData, FALSE);    
    
  NvidiaOpRegion.NvIgOpRegion = NULL;
  AmdOpRegion.ApXmOpRegion = NULL;  
  SaGlobalNvsArea->Area->VbiosSize = 0;
#if defined(SGOEMSSDT_SUPPORT) && SGOEMSSDT_SUPPORT == 1
  #if defined(TBT_DETACHABLE_GFX_SUPPORT) && TBT_DETACHABLE_GFX_SUPPORT == 1
    #if defined(SGOEMSSDT_ATI_SUPPORT) && SGOEMSSDT_ATI_SUPPORT == 1
      if ((SetupData->TbtSupport == 1) && (SetupData->TbtXConnectSupport == 1)) {
        Status = CreateReadyToBootEvent(
                   TPL_NOTIFY,
                   LoadOemTpvAcpiTables,
                   NULL,
                   &gEvent
                   );
      }
    #endif
  #else
    Status = CreateReadyToBootEvent( 
               TPL_NOTIFY, 
               LoadOemTpvAcpiTables,
               NULL, 
               &gEvent 
               ); 
  #endif
#endif  
  ///
  /// If Select SG mode
  /// 
  if (NbSetupData.PrimaryDisplay == 4) {
    ///
    /// Create ReadyToBoot callback for SG
    ///
    Status = CreateReadyToBootEvent( 
               TPL_NOTIFY, 
               LoadSgTpvAcpiTables,
               NULL, 
               &gEvent 
               ); 
  }
  return Status;
}
  
#if defined(SGOEMSSDT_SUPPORT) && SGOEMSSDT_SUPPORT == 1
EFI_STATUS
EFIAPI
InstallOemTpvAcpiTables (
  IN EFI_GUID   *OemRefAcpiTableGuid
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  BOOLEAN                       LoadTable;
  UINTN                         NumberOfHandles;
  UINTN                         Index;
  INTN                          Instance;
  UINTN                         Size;
  UINT32                        FvStatus;
  UINTN                         TableHandle;
  EFI_FV_FILETYPE               FileType;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *FwVol;
  EFI_ACPI_TABLE_PROTOCOL       *AcpiTable;
  EFI_ACPI_TABLE_VERSION        Version;
  EFI_ACPI_DESCRIPTION_HEADER   *TableHeader;
  EFI_ACPI_COMMON_HEADER        *Table;
    
  FwVol         = NULL;
  Table         = NULL;
  DEBUG ((DEBUG_INFO, "LoadOemTpvAcpiTables::LoadOemTpvAcpiTables start \n"));
  ///
  /// Locate FV protocol.
  ///
  Status = pBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Look for FV with ACPI storage file
  ///
  for (Index = 0; Index < NumberOfHandles; Index++) {
    ///
    /// Get the protocol on this handle
    /// This should not fail because of LocateHandleBuffer
    ///
    Status = pBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID **) &FwVol
                    );
    ASSERT_EFI_ERROR (Status);
    if (FwVol == NULL) {
      return Status;
    }
    ///
    /// See if it has the ACPI storage file
    ///
    Size      = 0;
    FvStatus  = 0;
    Status = FwVol->ReadFile (
                      FwVol,
                      OemRefAcpiTableGuid,
                      NULL,
                      &Size,
                      &FileType,
                      &Attributes,
                      &FvStatus
                      );

    ///
    /// If we found it, then we are done
    ///
    if (!EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "LoadOemTpvAcpiTables::It has the ACPI storage file \n"));
      break;
    }
  }
  ///
  /// Our exit status is determined by the success of the previous operations
  /// If the protocol was found, Instance already points to it.
  ///
  ///
  /// Free any allocated buffers
  ///
  (pBS->FreePool) (HandleBuffer);

  ///
  /// Sanity check that we found our data file
  ///
  ASSERT (FwVol);

  ///
  /// By default, a table belongs in all ACPI table versions published.
  ///
  Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;

  ///
  /// Locate ACPI tables
  ///
  Status = pBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &AcpiTable);

  ///
  /// Read tables from the storage file.
  ///
  if (FwVol == NULL) {
    ASSERT_EFI_ERROR (EFI_NOT_FOUND);
    return Status;
  }
  Instance = 0;

  while (Status == EFI_SUCCESS) {
    ///
    /// Read the ACPI tables
    ///
    Status = FwVol->ReadSection (
                      FwVol,
                      OemRefAcpiTableGuid,
                      EFI_SECTION_RAW,
                      Instance,
                      (VOID **) &Table,
                      &Size,
                      &FvStatus
                      );
    if (!EFI_ERROR (Status)) {
      ///
      /// check and load SwitchableGraphics SSDT table
      ///
      LoadTable   = FALSE;
      TableHeader = (EFI_ACPI_DESCRIPTION_HEADER *) Table;
      //if (OemRefAcpiTableGuid == SGOEM_ACPI_NV_SSDT_GUID){
        if (((EFI_ACPI_DESCRIPTION_HEADER *) TableHeader)->OemTableId == SIGNATURE_64 (
            'O',
            'e',
            'm',
            'N',
            'v',
            'T',
            0,
            0
            )
            ) {
          DEBUG ((DEBUG_INFO, "LoadOemTpvAcpiTables:: Found out SSDT:OemNvt.\n"));
          LoadTable = TRUE;
          Status    = InstallNvidiaOpRegion ();
          if (EFI_ERROR (Status)) {
            return Status;
          }
        }
      //} else if (OemRefAcpiTableGuid == SGOEM_ACPI_ATI_SSDT_GUID){
        if (((EFI_ACPI_DESCRIPTION_HEADER *) TableHeader)->OemTableId == SIGNATURE_64 (
            'O',
            'e',
            'm',
            'A',
            't',
            'i',
            0,
            0
            )
            ) {
        DEBUG ((DEBUG_INFO, "LoadOemTpvAcpiTables:: Found out SSDT:OemAti.\n"));
        LoadTable = TRUE;
        }        	
      //}
      ///
      /// Add the table
      ///
      if (LoadTable) {
        TableHandle = 0;
        Status = AcpiTable->InstallAcpiTable (
                                  AcpiTable,
                                  TableHeader,
                                  TableHeader->Length,
                                  &TableHandle
                                  );
      }
      ///
      /// Increment the instance
      ///
      Instance++;
      Table = NULL;
    }
  }
  DEBUG ((DEBUG_INFO, "LoadOemTpvAcpiTables::LoadOemTpvAcpiTables End \n"));
   
  return Status;
}

VOID
LoadOemTpvAcpiTables (
  IN EFI_EVENT Event,
  IN VOID      *Context
  ) 
{
#if ((defined(SGOEMSSDT_NVIDIA_SUPPORT) && SGOEMSSDT_NVIDIA_SUPPORT == 1) || (defined(SGOEMSSDT_ATI_SUPPORT) && SGOEMSSDT_ATI_SUPPORT == 1))
  EFI_STATUS                    Status;
#endif
  EFI_GUID                      gSgOemAcpiNvSsdtGuid = SGOEM_ACPI_NV_SSDT_GUID;
  EFI_GUID                      gSgOemAcpiAtiSsdtGuid = SGOEM_ACPI_ATI_SSDT_GUID;
#if defined(SGOEMSSDT_NVIDIA_SUPPORT) && SGOEMSSDT_NVIDIA_SUPPORT == 1
  UINT16                        VendorId;
#endif	
  DEBUG ((DEBUG_INFO, "SG_TPV:: SgTpv LoadOemTpvAcpiTables Callback enter. \n"));
  pBS->CloseEvent (Event);
  
#if defined(SGOEMSSDT_ATI_SUPPORT) && SGOEMSSDT_ATI_SUPPORT == 1
  // This sample is for ATI detachable graphics feature
  // Always Install this ACPI table.
  Status = InstallOemTpvAcpiTables(&gSgOemAcpiAtiSsdtGuid); 
#endif
  
#if defined(SGOEMSSDT_NVIDIA_SUPPORT) && SGOEMSSDT_NVIDIA_SUPPORT == 1
  if ((NbSetupData.PrimaryDisplay == 1) || (NbSetupData.PrimaryDisplay == 3)) {
    if (PchSeries == PchLp) {
      /// dGPU is present on PCH RootPort
      RootPortDev = SG_ULT_PORT_DEV;
      RootPortFun = SG_ULT_PORT_FUNC;
    } else {
      /// dGPU is present on PEG RootPort by default
      RootPortDev = SG_H_PORT_DEV;
      RootPortFun = SG_H_PORT_FUNC;
    }	  
    //
    // This sample is for NVIDIA dGPU only platform.
    // It assume your platform has NVIDIA dGPU on your platform as primary display card.
    //
    EndpointBus = MmioRead8 (MmPciBase (0, RootPortDev, RootPortFun) + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET) ;
    //
    //  Endpoint Device Not found
    //
    if (EndpointBus == 0xFF) {	  
      DEBUG ((DEBUG_WARN, "LoadOemTpvAcpiTables:: Endpoint Device Not found NVIDIA card. \n"));
      return;
    }  
    VendorId = MmioRead16 (MmPciBase (EndpointBus, 0, 0) + PCI_VENDOR_ID_OFFSET) ;
  
    if ( SaGlobalNvsArea->Area->EndpointBaseAddress == 0){
      SaGlobalNvsArea->Area->EndpointBaseAddress = (UINT32) MmPciBase (EndpointBus, 0, 0);
    }
    Status = InstallOemTpvAcpiTables(&gSgOemAcpiNvSsdtGuid);
    DEBUG ((DEBUG_INFO, "LoadOemTpvAcpiTables:: NDID:0x%x\n", SaGlobalNvsArea->Area->NumberOfValidDeviceId));
    DEBUG ((DEBUG_INFO, "LoadOemTpvAcpiTables:: DID1:0x%x\n", SaGlobalNvsArea->Area->DeviceId1));
    DEBUG ((DEBUG_INFO, "LoadOemTpvAcpiTables:: DID2:0x%x\n", SaGlobalNvsArea->Area->DeviceId2));
    DEBUG ((DEBUG_INFO, "LoadOemTpvAcpiTables:: DID3:0x%x\n", SaGlobalNvsArea->Area->DeviceId3));
    DEBUG ((DEBUG_INFO, "LoadOemTpvAcpiTables:: DID4:0x%x\n", SaGlobalNvsArea->Area->DeviceId4));
    DEBUG ((DEBUG_INFO, "LoadOemTpvAcpiTables:: DID5:0x%x\n", SaGlobalNvsArea->Area->DeviceId5));
    DEBUG ((DEBUG_INFO, "LoadOemTpvAcpiTables:: DID6:0x%x\n", SaGlobalNvsArea->Area->DeviceId6));
    DEBUG ((DEBUG_INFO, "LoadOemTpvAcpiTables:: DID7:0x%x\n", SaGlobalNvsArea->Area->DeviceId7));
    DEBUG ((DEBUG_INFO, "LoadOemTpvAcpiTables:: DID8:0x%x\n", SaGlobalNvsArea->Area->DeviceId8)); 
  }
#endif
   
  DEBUG ((DEBUG_INFO, "SG_TPV:: SgTpv LoadOemTpvAcpiTables Callback exit. \n"));
  return;
}
#endif    
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: LoadTpvAcpiTables
//
// Description:   Load Third part graphics vendor support SSDT Tables
//
// Input: VOID
//
// Output:   
//      EFI_SUCCESS     - SSDT Table load successful. 
//      EFI_UNSUPPORTED - Supported SSDT not found.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
//EFI_STATUS
VOID
LoadSgTpvAcpiTables (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )        
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  BOOLEAN                       LoadTable;
  INTN                          Instance;
  UINTN                         NumberOfHandles;
  UINTN                         Index;
  UINTN                         Size;
  UINTN                         TableHandle;
  UINT32                        FvStatus;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *FwVol;
  EFI_FV_FILETYPE               FileType;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  EFI_ACPI_DESCRIPTION_HEADER   *TableHeader;
  EFI_ACPI_TABLE_VERSION        Version;
  EFI_GUID                      SGTPV_AcpiTableGuid;  
  EFI_GUID                      gSgtpvAcpiSsdtGuid= SGTPV_ACPI_SSDT_GUID;
  EFI_GUID                      gSgtpvAcpiPchSsdtGuid= SGTPV_ACPIPCH_SSDT_GUID;
  EFI_ACPI_TABLE_PROTOCOL       *AcpiTable;
  EFI_ACPI_COMMON_HEADER        *Table;
  UINT16                        VendorId;
  
  FwVol = NULL;
  Table = NULL;
  
  DEBUG ((DEBUG_INFO, "SG_TPV:: SgTpv LoadTpvAcpiTables Callback. \n"));
  pBS->CloseEvent (Event);

  PchSeries = GetPchSeries();  
  if (PchSeries == PchLp) {
    /// For SwitchableGraphics support the dGPU is present on PCH RootPort
    RootPortDev = SG_ULT_PORT_DEV;
    RootPortFun = SG_ULT_PORT_FUNC;
    SGTPV_AcpiTableGuid = gSgtpvAcpiPchSsdtGuid;
  } else {
    /// For SwitchableGraphics support the dGPU is present on PEG RootPort by default
    RootPortDev = SG_H_PORT_DEV;
    RootPortFun = SG_H_PORT_FUNC;
    SGTPV_AcpiTableGuid  = gSgtpvAcpiSsdtGuid;
  }    
  
  EndpointBus = MmioRead8 (MmPciBase (0, RootPortDev, RootPortFun) + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET) ;
  //
  //  Endpoint Device Not found
  //
  if (EndpointBus == 0xFF) {	  
    return ;
  }
  
  VendorId = MmioRead16 (MmPciBase (EndpointBus, 0, 0) + PCI_VENDOR_ID_OFFSET) ;
  DEBUG ((DEBUG_INFO, "SG TPV Vendor ID %X\n", VendorId));
  
  if ( SaGlobalNvsArea->Area->EndpointBaseAddress == 0){
    SaGlobalNvsArea->Area->EndpointBaseAddress = (UINT32) MmPciBase (EndpointBus, 0, 0);
  }
  
  switch(VendorId){
    case NVIDIA_VID:
      if ( SaGlobalNvsArea->Area->DgpuSsid == 0){  
        SaGlobalNvsArea->Area->DgpuSsid =  MmioRead32( MmPciBase (EndpointBus, 0, 0)+ NVOPT_SSID_OFFSET);
      }
      //
      // Set VendorId if PEG is NVIDIA and supports HG
      //
      VendorId = NVIDIA_VID;
      break;
    case AMD_VID:
      if ( SaGlobalNvsArea->Area->DgpuSsid == 0){  
        SaGlobalNvsArea->Area->DgpuSsid =  MmioRead32( MmPciBase (EndpointBus, 0, 0)+ AMD_SVID_OFFSET);
      }
      //
      // Set VendorId if PEG is AMD and supports HG
      //
      VendorId = AMD_VID;
      break;
    default:
      //
      // either means the Device ID is not on the list of devices we know - we return from this function
      //
      DEBUG ((DEBUG_WARN, "SG TPV Unsupported Vendor ID\n"));
      return ;//EFI_UNSUPPORTED;
  }
  //
  // Locate FV protocol.
  //
  Status = pBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Look for FV with ACPI storage file
  //
  for (Index = 0; Index < NumberOfHandles; Index++) {
    //
    // Get the protocol on this handle
    // This should not fail because of LocateHandleBuffer
    //
    Status = pBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    &FwVol
                    );
    ASSERT_EFI_ERROR (Status);
    //
    // See if it has the ACPI storage file
    //
    Size      = 0;
    FvStatus  = 0;
    Status = FwVol->ReadFile (
                      FwVol,
                      &SGTPV_AcpiTableGuid,
                      NULL,
                      &Size,
                      &FileType,
                      &Attributes,
                      &FvStatus
                      );
    //
    // If we found it, then we are done
    //
    if (!EFI_ERROR (Status)) {
      break;
    }
  }
  //
  // Our exit status is determined by the success of the previous operations
  // If the protocol was found, Instance already points to it.
  //
  //
  // Free any allocated buffers
  //
  (pBS->FreePool) (HandleBuffer);

  //
  // Sanity check that we found our data file
  //
  ASSERT (FwVol);

  //
  // By default, a table belongs in all ACPI table versions published.
  //
  Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;

  //
  // Locate ACPI tables
  //
  Status = pBS->LocateProtocol (
                  &gEfiAcpiTableProtocolGuid, 
                  NULL, 
                  &AcpiTable
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Read tables from the storage file.
  //
  Instance = 0;

  while (Status == EFI_SUCCESS) {
    //
    // Read the ACPI tables
    //
    Status = FwVol->ReadSection (
                      FwVol,
                      &SGTPV_AcpiTableGuid,
                      EFI_SECTION_RAW,
                      Instance,
                      &Table,
                      &Size,
                      &FvStatus
                      );
    if (!EFI_ERROR (Status)) {
      //
      // check for SwitchableGraphics tables and decide which SSDT should be loaded
      //
      LoadTable   = FALSE;
      TableHeader = (EFI_ACPI_DESCRIPTION_HEADER *) Table;

      DEBUG ((DEBUG_INFO, "SG TPV SSDT TblID %X\n", TableHeader->OemTableId));

      switch (((EFI_ACPI_DESCRIPTION_HEADER *) TableHeader)->OemTableId) {
        case SIGNATURE_64 ('O', 'p', 't', 'T', 'a', 'b', 'l', 0):
          if (VendorId != NVIDIA_VID || 
              SaGlobalNvsArea->Area->SgMode != SgModeMuxless){
            break;
          }
          DEBUG ((DEBUG_INFO, "SG TPV Nvidia Optimus Table\n"));
          //
          // This is nVidia Optimus SSDT
          //
          LoadTable = TRUE;
          Status = InstallNvidiaOpRegion();
          if (EFI_ERROR(Status)) {
            DEBUG ((DEBUG_ERROR, "InstallNvidiaOpRegion Status = %r\n",Status));		  
            return ;
          }
          break;
        case SIGNATURE_64 ('A', 'm', 'd', 'T', 'a', 'b', 'l', 0):
          if(VendorId != AMD_VID) {
            break;        	  
          }
          //
          // This is Amd SSDT
          //
          LoadTable = TRUE;
          Status    = InstallAmdOpRegion ();
          if (EFI_ERROR (Status)) {
            return ;
          }
          break;
        default:
          DEBUG ((DEBUG_WARN, "WARNING: SG TPV Unsupported SSDT Signature...\n"));
          break;
      }

      //
      // Add the table
      //
      if (LoadTable) {         
        TableHandle = 0;
        Status = AcpiTable->InstallAcpiTable (
                              AcpiTable,
                              TableHeader,
                              Table->Length,
                              &TableHandle
                              );        
        ASSERT_EFI_ERROR (Status);
        break;  // only one ACPI SG/Optimus table should be loaded
      }
      //
      // Increment the instance
      //
      Instance++;
      Table = NULL;
    }
  }

  if (PchSeries == PchLp) {
    Status = SgUpdateDsdt();
  }  
  
  DEBUG ((DEBUG_INFO, "SGtpv:: NDID:0x%x\n", SaGlobalNvsArea->Area->NumberOfValidDeviceId));
  DEBUG ((DEBUG_INFO, "SGtpv:: DID1:0x%x\n", SaGlobalNvsArea->Area->DeviceId1));
  DEBUG ((DEBUG_INFO, "SGtpv:: DID2:0x%x\n", SaGlobalNvsArea->Area->DeviceId2));
  DEBUG ((DEBUG_INFO, "SGtpv:: DID3:0x%x\n", SaGlobalNvsArea->Area->DeviceId3));
  DEBUG ((DEBUG_INFO, "SGtpv:: DID4:0x%x\n", SaGlobalNvsArea->Area->DeviceId4));
  DEBUG ((DEBUG_INFO, "SGtpv:: DID5:0x%x\n", SaGlobalNvsArea->Area->DeviceId5));
  DEBUG ((DEBUG_INFO, "SGtpv:: DID6:0x%x\n", SaGlobalNvsArea->Area->DeviceId6));
  DEBUG ((DEBUG_INFO, "SGtpv:: DID7:0x%x\n", SaGlobalNvsArea->Area->DeviceId7));
  DEBUG ((DEBUG_INFO, "SGtpv:: DID8:0x%x\n", SaGlobalNvsArea->Area->DeviceId8));
  DEBUG ((DEBUG_INFO, "SG TPV Gfx acpi UPDATE - COMPLETED\n"));
  
  return ;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
//Procedure: InstallNvidiaOpRegion
//
//Description: Nvidia SG specific Asl OpRegion installation function.
//
//Input:
//
//  ImageHandle     Handle for this drivers loaded image protocol.
//  SystemTable     EFI system table.
//
//Output:
//
//  EFI_SUCCESS     The driver installed without error.
//  EFI_ABORTED     The driver encountered an error and could not complete
//                  installation of the ACPI tables.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
InstallNvidiaOpRegion (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINTN                         Size;
  //
  // Locate MXM3 protocol and get Mxm data pointer from function->MxmReturnStructure
  //
  EFI_GUID  EfiMxm3ProtocolGuid = MXM3_EFI_GUID;
#if MXM30_SUPPORT == 1
  MXM3_EFI_PROTOCOL           *Mxm30Protocol;
  VOID                        *MxmLegMemAddr = NULL;
  UINT32                       MxmLegMemSize = 0;
#endif  
  //
  // Allocate an ACPI NVS memory buffer as the Nvidia NVIG OpRegion, zero initialize
  // the entire 1K, and set the Nvidia NVIG OpRegion pointer in the Global NVS
  // area structure.
  //
  Size    = sizeof (NVIG_OPREGION);
  Status  = (pBS->AllocatePool) (
                    EfiACPIMemoryNVS, 
                    Size, 
                    &NvidiaOpRegion.NvIgOpRegion
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  (pBS->SetMem) (NvidiaOpRegion.NvIgOpRegion, Size, 0);
    
  //
  // Set up DeviceID values for _DOD.
  // Note that Display Subtype bits[15-12] and Port index bits[7:4] are set as per NV Switchable 3.0 spec.
  // Not used by Intel driver.
  //
  //
  // Display Type - CRT
  //
  SaGlobalNvsArea->Area->DeviceId1 = 0x80010100;
  DEBUG ((DEBUG_INFO, "SGtpv:: ActiveLFP:0x%x\n", SaGlobalNvsArea->Area->ActiveLFP));    
  if (SaGlobalNvsArea->Area->ActiveLFP == 3) {
    //
    // If Active LFP = EDP_A
    //
    // Display type - LFP Device Sub Type - eDP
    //
    SaGlobalNvsArea->Area->DeviceId2 = 0x8001A420;
  } else {
    //
    // Display Type - LFP Device Sub Type - LVDS
    //
    SaGlobalNvsArea->Area->DeviceId2 = 0x80010410;
  }
  //
  // Display type - EFP  Device Sub type - DisplayPort 1.1
  //
  SaGlobalNvsArea->Area->DeviceId3 = 0x80016330 | ((SaGlobalNvsArea->Area->SgMuxDid3 & 0xFF00) << 10);
    
  //
  // Display type - EFP  Device Sub type - HDMI 1.2 or 1.3a
  //
  SaGlobalNvsArea->Area->DeviceId4 = 0x80017331 | ((SaGlobalNvsArea->Area->SgMuxDid4 & 0xFF00) << 10);
    
  //
  // Display type - EFP  Device Sub type - HDMI 1.2 or 1.3a
  //
  SaGlobalNvsArea->Area->DeviceId5 = 0x80017342 | ((SaGlobalNvsArea->Area->SgMuxDid5 & 0xFF00) << 10);
    
  //
  // Display type - EFP  Device Sub type - DisplayPort 1.1
  //
  SaGlobalNvsArea->Area->DeviceId6 = 0x80016353 | ((SaGlobalNvsArea->Area->SgMuxDid6 & 0xFF00) << 10);
    
  //
  // Display type - EFP  Device Sub type - HDMI 1.2 or 1.3a
  //
  SaGlobalNvsArea->Area->DeviceId7 = 0x80017354 | ((SaGlobalNvsArea->Area->SgMuxDid7 & 0xFF00) << 10);
    
  //
  // DeviceId8 is not being used on HuronRiver SG
  //
  SaGlobalNvsArea->Area->DeviceId8              = 0x0;
    
  //
  // NDID
  //
  SaGlobalNvsArea->Area->NumberOfValidDeviceId  = VALIDDIDS;//0x7;
    
  //
  // NVIG
  //
  SaGlobalNvsArea->Area->NvIgOpRegionAddress = (UINT32)(UINTN)(NvidiaOpRegion.NvIgOpRegion);
    
  //
  // NVIG Header
  //
  (pBS->CopyMem) (NvidiaOpRegion.NvIgOpRegion->NISG, NVIG_HEADER_SIGNATURE, sizeof (NVIG_HEADER_SIGNATURE));
  NvidiaOpRegion.NvIgOpRegion->NISZ = NVIG_OPREGION_SIZE;
  NvidiaOpRegion.NvIgOpRegion->NIVR = NVIG_OPREGION_VER;
    
  //
  // Panel Scaling Preference
  //
  NvidiaOpRegion.NvIgOpRegion->GPSP = SaGlobalNvsArea->Area->IgdPanelScaling;
    
  // Save Link Control register
  NvidiaOpRegion.NvIgOpRegion->ELCL= MemoryRead16((UINTN)SaGlobalNvsArea->Area->EndpointBaseAddress + 0x10);
    
  //
  // Allocate an ACPI NVS memory buffer as the Nvidia NVHM OpRegion, zero initialize
  // the entire 62K, and set the Nvidia NVHM OpRegion pointer in the Global NVS
  // area structure.
  //
  Size    = sizeof (NVHM_OPREGION);
  Status  = (pBS->AllocatePool) (
                    EfiACPIMemoryNVS, 
                    Size, 
                    &NvidiaOpRegion.NvHmOpRegion
                    );
  if (EFI_ERROR (Status)) {
    (pBS->FreePool) (NvidiaOpRegion.NvIgOpRegion);
    return Status;
  }

  (pBS->SetMem) (NvidiaOpRegion.NvHmOpRegion, Size, 0);
    
  //
  // NVHM
  //
  SaGlobalNvsArea->Area->NvHmOpRegionAddress = (UINT32) (UINTN) (NvidiaOpRegion.NvHmOpRegion);

  //
  // Nvdia Audio Flag for enable/disable in _on/_off method.
  //  
  SaGlobalNvsArea->Area->NvdiaAudioFlag = NV_HDA_SUPPORT;  
  
  //
  // NVHM Header Signature, Size, Version
  //
  (pBS->CopyMem) (NvidiaOpRegion.NvHmOpRegion->NVSG, NVHM_HEADER_SIGNATURE, sizeof (NVHM_HEADER_SIGNATURE));
  NvidiaOpRegion.NvHmOpRegion->NVSZ = NVHM_OPREGION_SIZE;
  NvidiaOpRegion.NvHmOpRegion->NVVR = NVHM_OPREGION_VER;

  //
  // NVHM opregion address
  //
  NvidiaOpRegion.NvHmOpRegion->NVHO = (UINT32) (UINTN) (NvidiaOpRegion.NvHmOpRegion);
        
  //
  // Copy Oprom to allocated space in NV Opregion
  //
  NvidiaOpRegion.NvHmOpRegion->RVBS = SaGlobalNvsArea->Area->VbiosSize;
  (pBS->CopyMem) ((VOID *) (UINTN) NvidiaOpRegion.NvHmOpRegion->RBUF,  (VOID*)SaGlobalNvsArea->Area->VbiosAddress, SaGlobalNvsArea->Area->VbiosSize);
  DEBUG ((DEBUG_INFO, "VbiosSize %x\nVbiosAddress %x (%X)\n",  NvidiaOpRegion.NvHmOpRegion->RVBS, NvidiaOpRegion.NvHmOpRegion->RBUF, *(UINTN*)(NvidiaOpRegion.NvHmOpRegion->RBUF)));        

  //
  //  Locate the MXM3 Protocol and update Mxm struct pointers
  //
#if MXM30_SUPPORT == 1
  Status = pBS->LocateProtocol (
                  &EfiMxm3ProtocolGuid,
                  NULL,
                  &Mxm30Protocol
                  );
  DEBUG ((DEBUG_INFO, "==Locate Mxm30Protocol  %r====\n", Status));           
  if (!EFI_ERROR(Status)) {
    Status = Mxm30Protocol->MxmReturnStructure(
                              Mxm30Protocol, 
                              NULL,
                              (CHAR16*)&MxmLegMemSize,
                              (CHAR16)EFI30_DataBlockID, 
                              (CHAR8**)&MxmLegMemAddr
                              );
    ASSERT_EFI_ERROR (Status);
    DEBUG ((DEBUG_INFO, "==MxmLegMemSize %x\nMxmLegMemAddr %x", MxmLegMemSize, MxmLegMemAddr));           
    //
    // Copy MXM3 data structure to allocated space in NV Opregion
    //
    NvidiaOpRegion.NvHmOpRegion->MXML = MxmLegMemSize;
    (pBS->CopyMem) ((VOID *) (UINTN) NvidiaOpRegion.NvHmOpRegion->MXM3, MxmLegMemAddr, MxmLegMemSize);
  } else {  
    Status = EFI_SUCCESS;
  }
#endif        
  if (Status != EFI_SUCCESS) {
    (pBS->FreePool) (NvidiaOpRegion.NvIgOpRegion);
    (pBS->FreePool) (NvidiaOpRegion.NvHmOpRegion);
  }    
  return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
//Procedure: InstallAmdOpRegion
//
//Description: Amd(ATI) SG specific Asl Graphics OpRegion installation function.
//
//Input:
//
//  ImageHandle     Handle for this drivers loaded image protocol.
//  SystemTable     EFI system table.
//
//Output:
//
//  EFI_SUCCESS     The driver installed without error.
//  EFI_ABORTED     The driver encountered an error and could not complete
//                  installation of the ACPI tables.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
InstallAmdOpRegion (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINTN                         Size;
    
  //
  // Allocate an ACPI NVS memory buffer as the Amd APXM OpRegion, zero initialize
  // the entire 1K, and set the Amd APXM OpRegion pointer in the Global NVS
  // area structure.
  //
  Size    = sizeof (APXM_OPREGION);
  Status  = (pBS->AllocatePool) (
                    EfiACPIMemoryNVS, 
                    Size, 
                    &AmdOpRegion.ApXmOpRegion
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  (pBS->SetMem) (AmdOpRegion.ApXmOpRegion, Size, 0);

  //
  // APXM address
  //
  SaGlobalNvsArea->Area->ApXmOpRegionAddress  = (UINT32) (UINTN) (AmdOpRegion.ApXmOpRegion);
  AmdOpRegion.ApXmOpRegion->APXA            = (UINT32) (UINTN) (AmdOpRegion.ApXmOpRegion);

  //
  // Note. All fields in Amd OpRegion will be initialized with Zeroes. 
  //       Only update those that differ from 0.

  // Set up DIDx values for _DOD
  //
  // Device ID - CRT on IGPU
  //
  SaGlobalNvsArea->Area->DeviceId1 = 0x80010100;

  //
  // Device ID - LFP (LVDS or eDP)
  //
  SaGlobalNvsArea->Area->DeviceId2 = 0x80010400;

  //
  // Display type - EFP  Device Sub type - DisplayPort 1.1
  //
  SaGlobalNvsArea->Area->DeviceId3 = 0x80010300;

  //
  // Display type - EFP  Device Sub type - HDMI 1.2 or 1.3a
  //
  SaGlobalNvsArea->Area->DeviceId4 = 0x80010301;

  //
  // Display type - EFP  Device Sub type - HDMI 1.2 or 1.3a
  //
  SaGlobalNvsArea->Area->DeviceId5 = 0x80010302;

  //
  // Display type - EFP  Device Sub type - DisplayPort 1.1
  //
  SaGlobalNvsArea->Area->DeviceId6 = 0x80010303;

  //
  // Display type - EFP  Device Sub type - HDMI 1.2 or 1.3a
  //

  //
  // SG Feature List for ASL usage
  //
  if (SaGlobalNvsArea->Area->SgFeatureList & 1) {  // WIRELESSDISPLAY
    SaGlobalNvsArea->Area->DeviceId7 = 0x80010306;
  } else {
    SaGlobalNvsArea->Area->DeviceId7 = 0x80010304;
  }    
  //
  // DeviceId8 is not being used on HuronRiver SG
  //
  SaGlobalNvsArea->Area->DeviceId8              = 0x0;

  //
  // NDID
  //
  SaGlobalNvsArea->Area->NumberOfValidDeviceId  = VALIDDIDS;//0x7;

  //
  // APXM Header
  //
  (pBS->CopyMem) (AmdOpRegion.ApXmOpRegion->APSG, APXM_HEADER_SIGNATURE, sizeof (APXM_HEADER_SIGNATURE));
  AmdOpRegion.ApXmOpRegion->APSZ = APXM_OPREGION_SIZE;
  AmdOpRegion.ApXmOpRegion->APVR = APXM_OPREGION_VER;

  //
  // Total number of toggle list entries
  //
  AmdOpRegion.ApXmOpRegion->NTLE = 15;

  //
  // The display combinations in the list...
  //
  //
  // CRT
  //
  AmdOpRegion.ApXmOpRegion->TLEX[0] = 0x0002;
  //
  // LFP
  //
  AmdOpRegion.ApXmOpRegion->TLEX[1] = 0x0001;
  //
  // DP_B
  //
  AmdOpRegion.ApXmOpRegion->TLEX[2] = 0x0008;
  //
  // HDMI_B
  //
  AmdOpRegion.ApXmOpRegion->TLEX[3] = 0x0080;
  //
  // HDMI_C
  //
  AmdOpRegion.ApXmOpRegion->TLEX[4] = 0x0200;
  //
  // DP_D
  //
  AmdOpRegion.ApXmOpRegion->TLEX[5] = 0x0400;
  //
  // HDMI_D
  //
  AmdOpRegion.ApXmOpRegion->TLEX[6] = 0x0800;
  //
  // LFP+CRT
  //
  AmdOpRegion.ApXmOpRegion->TLEX[7] = 0x0003;
  //
  // LFP+DP_B
  //
  AmdOpRegion.ApXmOpRegion->TLEX[8] = 0x0009;
  //
  // LFP+HDMI_B
  //
  AmdOpRegion.ApXmOpRegion->TLEX[9] = 0x0081;
  //
  // LFP+HDMI_C
  //
  AmdOpRegion.ApXmOpRegion->TLEX[10] = 0x0201;
  //
  // LFP+DP_D
  //
  AmdOpRegion.ApXmOpRegion->TLEX[11] = 0x0401;
  //
  // LFP+HDMI_D
  //
  AmdOpRegion.ApXmOpRegion->TLEX[12] = 0x0801;
  //
  // Dummy 1
  //
  AmdOpRegion.ApXmOpRegion->TLEX[13] = 0x0;
  //
  // Dummy 2
  //
  AmdOpRegion.ApXmOpRegion->TLEX[14] = 0x0;

  //
  // Panel Scaling Preference
  //
  AmdOpRegion.ApXmOpRegion->EXPM = SaGlobalNvsArea->Area->IgdPanelScaling;

  // Save Link Control register
  AmdOpRegion.ApXmOpRegion->ELCL= MemoryRead16((UINTN)SaGlobalNvsArea->Area->EndpointBaseAddress + 0x10);

  //
  // Copy Oprom to allocated space in ATI Opregion
  //
  AmdOpRegion.ApXmOpRegion->RVBS = SaGlobalNvsArea->Area->VbiosSize;
  (pBS->CopyMem) ((VOID *) (UINTN) AmdOpRegion.ApXmOpRegion->RBUF, (VOID*)SaGlobalNvsArea->Area->VbiosAddress, SaGlobalNvsArea->Area->VbiosSize);
  DEBUG ((DEBUG_INFO, "VbiosSize %x, AMDbiosAddress %x (%X)\n",  AmdOpRegion.ApXmOpRegion->RVBS, AmdOpRegion.ApXmOpRegion->RBUF, *(UINTN*)(AmdOpRegion.ApXmOpRegion->RBUF))); 

  return Status;
}

EFI_STATUS
SgUpdateDsdt (	
  VOID
  )
{
  EFI_STATUS                 Status;
  FACP_20                    *Table = NULL;
  EFI_ACPI_TABLE_VERSION     Version;
  UINTN                      Handle;
  ACPI_HDR                   *DsdtPtr = NULL;
  ASL_OBJ_INFO               ObjInfo;
  UINT32                     Index;
  UINT16                     ASLDeviceOP;
  UINT8                      ASLDeviceOPLength;
  UINT8                      *ptr;
  UINT8                      i;
  UINT8                      j;
  UINT8                      *ptr1;
  UINT8                      *ptr2;
  UINT16                     PXSXASLDeviceOP;
  UINT16                     PXSXASLDeviceOP1;
  UINT8                      PXSXASLDeviceOPLength;
  UINT16                     PXSXASLDeviceOPLength16;
  UINT8                      HPMEASLMethodOP;
  UINT8                      HPMEASLMethodOP1;
  UINT8                      HPMEASLMethodOPLength;
  UINT16                     HPMEASLMethodOPLength16;
  UINT16                     ASLDeviceOPLength16;
  UINT32                     *Signature;
  EFI_ACPI_SUPPORT_PROTOCOL  *AcpiSupport2 = NULL;

  Status = pBS->LocateProtocol (
                  &gEfiAcpiSupportProtocolGuid, 
                  NULL, 
                  &AcpiSupport2
                  );  

  DEBUG ((DEBUG_INFO, "SgTpvAcpiTables :: Update Dsdt Start. \n"));	          
  //
  // Find DSDT ACPI Table
  //
  for (Index = 0; Index < 0x100; Index++) {	      
    Status = AcpiSupport2->GetAcpiTable(
                             AcpiSupport2, 
                             Index, 
                             &Table, 
                             &Version, 
                             &Handle
                             );  
    if (EFI_ERROR(Status)){
      DEBUG ((DEBUG_ERROR, "SgTpvAcpiTables :: Did not found FACP table to get DSDT pointer.\n"));
      return EFI_UNSUPPORTED;
    }
    
    if ((Table->Header.Signature == FACP_SIG) && (DsdtPtr == NULL)) {
      DsdtPtr = (ACPI_HDR*)Table->DSDT;
      DEBUG ((DEBUG_INFO, "SgTpvAcpiTables :: Table->Header.Revision = %x\n", Table->Header.Revision));
      DEBUG ((DEBUG_INFO, "SgTpvAcpiTables :: Found DSDT%x Table at 0x%08X\n", Index,DsdtPtr));

      break;
    }
  }
  //
  // Find the root port of dGPU.
  //	    
  if (SG_ULT_PORT_FUNC == 0x04){
    Status = GetAslObj( (UINT8*)(DsdtPtr + 1), 
               DsdtPtr->Length - sizeof(ACPI_HDR) - 1, 
               "RP05", 
               otDevice, 
               &ObjInfo 
               );
  } else if (SG_ULT_PORT_FUNC == 0x00){
    if (SG_ULT_PORT_DEV == 0x1c){    
      Status = GetAslObj( (UINT8*)(DsdtPtr + 1), 
                 DsdtPtr->Length - sizeof(ACPI_HDR) - 1, 
                 "RP01", 
                 otDevice, 
                 &ObjInfo 
                 );
    } else if (SG_ULT_PORT_DEV == 0x1d){
      Status = GetAslObj( (UINT8*)(DsdtPtr + 1), 
                 DsdtPtr->Length - sizeof(ACPI_HDR) - 1, 
                 "RP09", 
                 otDevice, 
                 &ObjInfo 
                 );			
    }                    
  } else {
    DEBUG ((DEBUG_ERROR, "SgTpvAcpiTables :: Get ASL root port device fail\n"));    
    return EFI_UNSUPPORTED;
  }
	    
  if (!EFI_ERROR(Status)) {
    ptr = (UINT8*)(ObjInfo.DataStart) - 8;
    ASLDeviceOP = (UINT16)*(UINT32*)(ptr);
    ASLDeviceOPLength = (UINT8)*(UINT32*)(ptr + 2);
    ASLDeviceOPLength16 = (ASLDeviceOPLength & 0x0F);
    ASLDeviceOPLength = (UINT8)*(UINT32*)(ptr + 3);
    ASLDeviceOPLength16 = (ASLDeviceOPLength << 4) | ASLDeviceOPLength16; 

    DEBUG ((DEBUG_INFO, "Device PTR=0x%08X, ASLDeviceOP=0x%X, ASLDeviceOPLength=0x%X\n", ptr, (UINT16)ASLDeviceOP, (UINT16)ASLDeviceOPLength16));
    if (ASLDeviceOP == 0x825B) {  // Is ASL DeviceOP ?
      //
      //Rename the _STA to XSTA
      //
      Status = GetAslObj ( (UINT8*)(ptr + 1), 
                 ASLDeviceOPLength16 - sizeof(ACPI_HDR) - 1, 
                 "_STA", 
                 otMethod, 
                 &ObjInfo 
                 );
      if (!EFI_ERROR(Status)){
        ptr1 = (UINT8*)(ObjInfo.DataStart) - 5; 
        DEBUG ((DEBUG_INFO, "_STA Method PTR=0x%08X\n", ptr1));
        *(UINT8*)ptr1 = 'X';
      }

      //
      //Find the device PXSX under the root port of dGPU
      //
      Status = GetAslObj ( (UINT8*)(ptr + 1), 
                 ASLDeviceOPLength16 - sizeof(ACPI_HDR) - 1, 
                 "PXSX", 
                 otDevice, 
                 &ObjInfo 
                 );
      if (!EFI_ERROR(Status)) {
        ptr1 = (UINT8*)(ObjInfo.DataStart);  
        DEBUG ((DEBUG_INFO, "PXSX key word PTR=0x%08X \n", ptr1));
        ptr1 = (UINT8*)(ObjInfo.DataStart) - 7;
        PXSXASLDeviceOP = (UINT16)*(UINT32*)(ptr1);
        ptr2 = (UINT8*)(ObjInfo.DataStart) - 8;
        PXSXASLDeviceOP1 = (UINT16)*(UINT32*)(ptr2);

        if (PXSXASLDeviceOP == 0x825B) {  // Is ASL DeviceOP ?
          PXSXASLDeviceOPLength = (UINT8)*(UINT32*)(ptr1 + 2);
          DEBUG ((DEBUG_INFO, "PXSX Device PTR=0x%08X, ASLDeviceOP=0x%X, ASLDeviceOPLength=0x%X\n", ptr1, (UINT16)PXSXASLDeviceOP, (UINT8)PXSXASLDeviceOPLength));
	             
          for ( i = 0; i < PXSXASLDeviceOPLength + 2; i++) {
            *(UINT8*)(ptr1 + i) = 0;
          }
        } else if (PXSXASLDeviceOP1 == 0x825B) {		        
          PXSXASLDeviceOPLength = (UINT8)*(UINT32*)(ptr2 + 2);
          PXSXASLDeviceOPLength16 = (PXSXASLDeviceOPLength & 0x0F);
          PXSXASLDeviceOPLength = (UINT8)*(UINT32*)(ptr2 + 3);
          PXSXASLDeviceOPLength16 = (PXSXASLDeviceOPLength << 4) | PXSXASLDeviceOPLength16; 
		        
          DEBUG ((DEBUG_INFO, "PXSX Device PTR=0x%08X, ASLDeviceOP=0x%X, ASLDeviceOPLength=0x%X\n", ptr2, (UINT16)PXSXASLDeviceOP1, (UINT16)PXSXASLDeviceOPLength16));
		             
          for ( i = 0; i < PXSXASLDeviceOPLength16 + 2; i++) {
            *(UINT8*)(ptr2 + i) = 0;
          }
        }
      }

      Status = GetAslObj( (UINT8*)(ptr + 1), ASLDeviceOPLength16 - sizeof(ACPI_HDR) - 1, 
								     "HPME", otMethod, &ObjInfo );
      if (!EFI_ERROR(Status)) {
        ptr1 = (UINT8*)(ObjInfo.DataStart) - 7;
        HPMEASLMethodOP = (UINT8)*(UINT32*)(ptr1);
        ptr2 = (UINT8*)(ObjInfo.DataStart) - 8;
        HPMEASLMethodOP1 = (UINT8)*(UINT32*)(ptr2);

        if (HPMEASLMethodOP == 0x14) {  // Is ASL DeviceOP ?
          HPMEASLMethodOPLength = (UINT8)*(UINT32*)(ptr1 + 1);
          DEBUG ((DEBUG_INFO, "HPME Method PTR=0x%08X, ASLDeviceOP=0x%X, ASLDeviceOPLength=0x%X\n", ptr1, (UINT8)HPMEASLMethodOP, (UINT8)HPMEASLMethodOPLength));

          for ( i = 0; i < HPMEASLMethodOPLength ; i++) {
            Signature = (UINT32 *) (ptr1 + i);
            if(*Signature == EFI_SIGNATURE_32 ('P', 'X', 'S', 'X')) {
              DEBUG ((DEBUG_INFO, "PXSX Notify PTR = 0x%08X\n", Signature));
              for ( j = 0; j < 7 ; j++) {
                *(UINT8*)(ptr1 + i + j -1) = 0;
              }
              break;
            }
          }	                       
        } else if (HPMEASLMethodOP1 == 0x14) {		        
          HPMEASLMethodOPLength = (UINT8)*(UINT32*)(ptr2 + 1);
          HPMEASLMethodOPLength16 = (HPMEASLMethodOPLength & 0x0F);
          HPMEASLMethodOPLength = (UINT8)*(UINT32*)(ptr2 + 2);
          HPMEASLMethodOPLength16 = (HPMEASLMethodOPLength << 4) | HPMEASLMethodOPLength16; 
		        
          DEBUG ((DEBUG_INFO, "HPME Method PTR=0x%08X, ASLDeviceOP=0x%X, ASLDeviceOPLength=0x%X\n", ptr2, (UINT8)HPMEASLMethodOP1, (UINT8)HPMEASLMethodOPLength16));
			     		             
          for ( i = 0; i < HPMEASLMethodOPLength16 ; i++) {
            Signature = (UINT32 *) (ptr2 + i);
            if (*Signature == EFI_SIGNATURE_32 ('P', 'X', 'S', 'X')){
              DEBUG ((DEBUG_INFO, "PXSX Notify PTR = 0x%08X\n", Signature));
                for ( j = 0; j < 7 ; j++) {
                  *(UINT8*)(ptr2 + i + j -1) = 0;
                }
              break;
            }
          }
        }
      }
    }
  }	
  DEBUG ((DEBUG_INFO, "==============Sg Update Dsdt End==============\n"));	
  return Status;		
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
