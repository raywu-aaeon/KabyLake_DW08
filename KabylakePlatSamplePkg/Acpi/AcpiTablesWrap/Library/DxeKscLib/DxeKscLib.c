//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
//
// $Header: $
//
// $Revision: $
//
// $Date: $
//
//*****************************************************************************
// Revision History
// ----------------
// $Log: $
// 
//
//*****************************************************************************

//-------------------------------------------------------------------------
// Include(s)
//-------------------------------------------------------------------------
#include <Efi.h>
#include <AmiDxeLib.h>
#include <Library\UefiBootServicesTableLib.h>
#include <Library\DxeKscLib.h>
#include <PlatformInfo.h>
#include <Guid\PlatformInfoHob.h>
#include <Protocol\CpuIo2.h>

//----------------------------------------------------------------------------
// Variable and External Declaration(s)
//----------------------------------------------------------------------------
EFI_CPU_IO2_PROTOCOL    *mDxeKscLibCpuIo = NULL;
BOOLEAN                 mDxeKscLibInitialized = FALSE;

//----------------------------------------------------------------------------

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   InitializeKscLib
//
// Description: Initialize the library.
//              The DXE library only requires CPU IO protocol, so this will 
//              locate CPU IO protocol and save it for future use. 
//
// Input:       None
//
// Output:      EFI_SUCCESS       - KscLib is successfully initialized.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
InitializeKscLib (
  VOID
  )
{
  EFI_STATUS                  Status;
  PLATFORM_INFO                 *PlatformInfo;

  Status = gBS->LocateProtocol (&gPlatformInfoProtocolGuid, NULL, &PlatformInfo);

  if (EFI_ERROR (Status)) {
  	return Status;
  }
  
  if (PlatformInfo->EcPresent == TRUE) {
    Status = gBS->LocateProtocol (&gEfiCpuIo2ProtocolGuid, NULL, &mDxeKscLibCpuIo);
    ASSERT_EFI_ERROR (Status);

    mDxeKscLibInitialized = TRUE;
  	Status = EFI_SUCCESS;
  } else {
    mDxeKscLibInitialized = FALSE;   
    Status = EFI_NOT_FOUND;
  } 

  return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   SendKscCommand
//
// Description: Sends command to Keyboard System Controller.
//
// Input:       Command  - Command byte to send
//
// Output:      EFI_SUCCESS       - Command success
//              EFI_DEVICE_ERROR  - Command error
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
SendKscCommand (
  UINT8                 Command
  ) 
{
#ifndef SOFTSDV_FLAG
  UINTN         Index;
  UINT8         KscStatus;
  EFI_STATUS    Status;

  //
  // Verify if KscLib has been initialized, NOT if EC dose not exist.
  //
  if (mDxeKscLibInitialized == FALSE){
    return EFI_DEVICE_ERROR;
  }

  KscStatus = 0;
  
  //
  // Verify library has been initialized properly
  //
  ASSERT (mDxeKscLibCpuIo);

  Index = 0;
  
  //
  // Wait for KSC to be ready (with a timeout)
  //
  ReceiveKscStatus (&KscStatus); 
  while (((KscStatus & KSC_S_IBF) != 0) && (Index < KSC_TIME_OUT)) {
    gBS->Stall (15);
    ReceiveKscStatus (&KscStatus);
    Index++;
  }
  if (Index >= KSC_TIME_OUT) {
    return EFI_DEVICE_ERROR;
  }

  //
  // Send the KSC command
  //
  Status = mDxeKscLibCpuIo->Io.Write (
                             mDxeKscLibCpuIo,
                             EfiCpuIoWidthUint8,
                             KSC_C_PORT,
                             1,
                             &Command
                             );
  ASSERT_EFI_ERROR (Status);

#endif  // SOFTSDV_FLAG

  return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   ReceiveKscStatus
//
// Description: Receives status from Keyboard System Controller.
//
// Input:       Status  - Status byte to receive
//
// Output:      EFI_SUCCESS       - Always success
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
ReceiveKscStatus (
  UINT8                 *KscStatus
  )
{ 
  EFI_STATUS    Status = EFI_SUCCESS;

  // 
  // Verify if KscLib has been initialized, NOT if EC dose not exist.
  //
  if(mDxeKscLibInitialized == FALSE){
    return EFI_DEVICE_ERROR;
  }

  //
  // Verify library has been initialized properly
  //
  ASSERT (mDxeKscLibCpuIo);   

  //
  // Read and return the status 
  //
  Status = mDxeKscLibCpuIo->Io.Read (
                             mDxeKscLibCpuIo,
                             EfiCpuIoWidthUint8,
                             KSC_C_PORT,
                             1,
                             KscStatus
                             );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   SendKscData
//
// Description: Sends data to Keyboard System Controller.
//
// Input:       Data  - Data byte to send
//
// Output:      EFI_SUCCESS       - Success
//              EFI_TIMEOUT       - Timeout
//              Other             - Failed
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
SendKscData (
  UINT8                 Data
  ) 
{
#ifndef SOFTSDV_FLAG

  UINTN         Index;
  UINT8         KscStatus;
  EFI_STATUS    Status = EFI_SUCCESS;

  // 
  // Verify if KscLib has been initialized, NOT if EC dose not exist.
  //
  if(mDxeKscLibInitialized == FALSE){
    return EFI_DEVICE_ERROR;
  }

  //
  // Verify library has been initialized properly
  //
  ASSERT (mDxeKscLibCpuIo);

  Index = 0;

  //
  // Wait for KSC to be ready (with a timeout)
  //
  ReceiveKscStatus (&KscStatus);
  while (((KscStatus & KSC_S_IBF) != 0) && (Index < KSC_TIME_OUT)) {
    gBS->Stall (15);
    ReceiveKscStatus (&KscStatus);
    Index++;
  }
  if (Index >= KSC_TIME_OUT) {
    return EFI_DEVICE_ERROR;
  }

  //
  // Send the data and return
  //
  Status = mDxeKscLibCpuIo->Io.Write (
                             mDxeKscLibCpuIo,
                             EfiCpuIoWidthUint8,
                             KSC_D_PORT,
                             1,
                             &Data
                             );
  ASSERT_EFI_ERROR (Status);

#endif  // SOFTSDV_FLAG

  return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   ReceiveKscData
//
// Description: Receives data from Keyboard System Controller.
//
// Input:       Data  - Data byte received
//
// Output:      EFI_SUCCESS       - Read success
//              EFI_DEVICE_ERROR  - Read error
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
ReceiveKscData (
  UINT8   *Data
 )
{
#ifndef SOFTSDV_FLAG

  UINTN         Index;
  UINT8         KscStatus;  
  EFI_STATUS    Status = EFI_SUCCESS;

  // 
  // Verify if KscLib has been initialized, NOT if EC dose not exist.
  //
  if(mDxeKscLibInitialized == FALSE){
    return EFI_DEVICE_ERROR;
  }

  //
  // Verify library has been initialized properly
  //
  ASSERT (mDxeKscLibCpuIo);

  Index = 0;

  //
  // Wait for KSC to be ready (with a timeout)
  //
  ReceiveKscStatus (&KscStatus);
  while (((KscStatus & KSC_S_OBF) == 0) && (Index < KSC_TIME_OUT)) {
    gBS->Stall (15);
    ReceiveKscStatus (&KscStatus);
    Index++;
  }
  if (Index >= KSC_TIME_OUT) {
    return EFI_DEVICE_ERROR;
  }

  //
  // Read KSC data and return
  //
  Status = mDxeKscLibCpuIo->Io.Read (
                             mDxeKscLibCpuIo,
                             EfiCpuIoWidthUint8,
                             KSC_D_PORT,
                             1,
                             Data
                             );
  ASSERT_EFI_ERROR (Status);

#else
  *Data = 0;  // Always return 0 in SoftSDV
#endif // SOFTSDV_FLAG

  return EFI_SUCCESS;
}
