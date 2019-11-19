/** @file
  Utility functions used by TPM PEI driver.
  
Copyright (c) 2005 - 2011, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials 
are licensed and made available under the terms and conditions of the BSD License 
which accompanies this distribution.  The full text of the license may be found at 
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS, 
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include <Efi.h>
#include <AmiLib.h>
#include <Library/DebugLib.h>
#include <AmiTxtTcgPeim.h>

#define CopyMem AuxCopyMem

/**
  Send TPM_Startup command to TPM.

  @param[in] PeiServices        Describes the list of possible PEI Services.
  @param[in] TpmHandle          TPM handle.  
  @param[in] BootMode           Boot mode.  
 
  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_TIMEOUT           The register can't run into the expected status in time.
  @retval EFI_BUFFER_TOO_SMALL  Response data buffer is too small.
  @retval EFI_DEVICE_ERROR      Unexpected device behavior.

**/
EFI_STATUS
TpmCommStartup (
  IN      EFI_BOOT_MODE             BootMode
  )
{
  EFI_STATUS                        Status;
  TPM_STARTUP_TYPE                  TpmSt;
  UINT32                            TpmRecvSize;
  UINT32                            TpmSendSize;
  TPM_CMD_START_UP                  SendBuffer;
  UINT8                             RecvBuffer[20];

  TpmSt = TPM_ST_CLEAR;
  if (BootMode == BOOT_ON_S3_RESUME) {
    TpmSt = TPM_ST_STATE;
  }
  //
  // send Tpm command TPM_ORD_Startup
  //
  TpmRecvSize               = 20;
  TpmSendSize               = sizeof (TPM_CMD_START_UP);
  SendBuffer.Hdr.tag        = SwapBytes16 (TPM_TAG_RQU_COMMAND);
  SendBuffer.Hdr.paramSize  = SwapBytes32 (TpmSendSize);
  SendBuffer.Hdr.ordinal    = SwapBytes32 (TPM_ORD_Startup);
  SendBuffer.TpmSt          = SwapBytes16 (TpmSt);
  Status = TpmOemSubmitCommand ( TpmSendSize, (UINT8 *)&SendBuffer, &TpmRecvSize, RecvBuffer);
  
  if( !EFI_ERROR(Status) )
  {
      TPM_RSP_COMMAND_HDR   *p_retHdr = (TPM_RSP_COMMAND_HDR*)RecvBuffer;
      if( p_retHdr->returnCode == 0)
          return EFI_SUCCESS;
      
  }
  
  return EFI_DEVICE_ERROR;
}

/**
  Send TPM_ContinueSelfTest command to TPM.

  @param[in] PeiServices        Describes the list of possible PEI Services.
  @param[in] TpmHandle          TPM handle.  
 
  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_TIMEOUT           The register can't run into the expected status in time.
  @retval EFI_BUFFER_TOO_SMALL  Response data buffer is too small.
  @retval EFI_DEVICE_ERROR      Unexpected device behavior.

**/
EFI_STATUS
TpmCommContinueSelfTest (
  )
{
  EFI_STATUS                        Status;
  UINT32                            TpmRecvSize;
  UINT32                            TpmSendSize;
  TPM_CMD_SELF_TEST                 SendBuffer;
  UINT8                             RecvBuffer[20];

  //
  // send Tpm command TPM_ORD_ContinueSelfTest
  //
  TpmRecvSize               = 20;
  TpmSendSize               = sizeof (TPM_CMD_SELF_TEST);
  SendBuffer.Hdr.tag        = SwapBytes16 (TPM_TAG_RQU_COMMAND);
  SendBuffer.Hdr.paramSize  = SwapBytes32 (TpmSendSize);  
  SendBuffer.Hdr.ordinal    = SwapBytes32 (TPM_ORD_ContinueSelfTest);
  Status = TpmOemSubmitCommand ( TpmSendSize, (UINT8 *)&SendBuffer, &TpmRecvSize, RecvBuffer);
  
  if( !EFI_ERROR(Status) )
  {
      TPM_RSP_COMMAND_HDR   *p_retHdr = (TPM_RSP_COMMAND_HDR*)RecvBuffer;
      if( p_retHdr->returnCode == 0)
          return EFI_SUCCESS;
      
  }
  
  return EFI_DEVICE_ERROR;
}

/**
  Get TPM capability flags.

  @param[in]  PeiServices       Describes the list of possible PEI Services.
  @param[in]  TpmHandle         TPM handle.  
  @param[out] Deactivated       Returns deactivated flag.
  @param[out] LifetimeLock      Returns physicalPresenceLifetimeLock permanent flag.  
  @param[out] CmdEnable         Returns physicalPresenceCMDEnable permanent flag.
 
  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_TIMEOUT           The register can't run into the expected status in time.
  @retval EFI_BUFFER_TOO_SMALL  Response data buffer is too small.
  @retval EFI_DEVICE_ERROR      Unexpected device behavior.

**/
EFI_STATUS
TpmCommGetCapability (
     OUT  BOOLEAN                   *Deactivated, OPTIONAL
     OUT  BOOLEAN                   *LifetimeLock, OPTIONAL
     OUT  BOOLEAN                   *CmdEnable OPTIONAL
  )
{
  EFI_STATUS                        Status;
  UINT32                            TpmRecvSize;
  UINT32                            TpmSendSize;
  TPM_CMD_GET_CAPABILITY            SendBuffer;
  UINT8                             RecvBuffer[40];
  TPM_PERMANENT_FLAGS               *TpmPermanentFlags;

  //
  // send Tpm command TPM_ORD_GetCapability
  //
  TpmRecvSize                   = 40;
  TpmSendSize                   = sizeof (TPM_CMD_GET_CAPABILITY);
  SendBuffer.Hdr.tag            = SwapBytes16 (TPM_TAG_RQU_COMMAND);
  SendBuffer.Hdr.paramSize      = SwapBytes32 (TpmSendSize);  
  SendBuffer.Hdr.ordinal        = SwapBytes32 (TPM_ORD_GetCapability);
  SendBuffer.Capability         = SwapBytes32 (TPM_CAP_FLAG);
  SendBuffer.CapabilityFlagSize = SwapBytes32 (sizeof (TPM_CAP_FLAG_PERMANENT));
  SendBuffer.CapabilityFlag     = SwapBytes32 (TPM_CAP_FLAG_PERMANENT);
  Status = TpmOemSubmitCommand ( TpmSendSize, (UINT8 *)&SendBuffer, &TpmRecvSize, RecvBuffer);
  if( EFI_ERROR(Status) )
      return Status;
  else
  {
      TPM_RSP_COMMAND_HDR   *p_retHdr = (TPM_RSP_COMMAND_HDR*)RecvBuffer;
      if( p_retHdr->returnCode )
          return EFI_DEVICE_ERROR;
  }
  
  TpmPermanentFlags = (TPM_PERMANENT_FLAGS *)&RecvBuffer[sizeof (TPM_RSP_COMMAND_HDR) + sizeof (UINT32)];
  if (Deactivated != NULL) {
    *Deactivated      = TpmPermanentFlags->deactivated;
  }

  if (LifetimeLock != NULL) {
    *LifetimeLock = TpmPermanentFlags->physicalPresenceLifetimeLock;
  }

  if (CmdEnable != NULL) {
    *CmdEnable = TpmPermanentFlags->physicalPresenceCMDEnable;
  }
  return Status;
}

/**
  Extend a TPM PCR.

  @param[in]  PeiServices       Describes the list of possible PEI Services.
  @param[in]  TpmHandle         TPM handle.  
  @param[in]  DigestToExtend    The 160 bit value representing the event to be recorded.  
  @param[in]  PcrIndex          The PCR to be updated.
  @param[out] NewPcrValue       New PCR value after extend.  
 
  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_TIMEOUT           The register can't run into the expected status in time.
  @retval EFI_BUFFER_TOO_SMALL  Response data buffer is too small.
  @retval EFI_DEVICE_ERROR      Unexpected device behavior.

**/
EFI_STATUS
TpmCommExtend (
  IN      TPM_DIGEST                *DigestToExtend,
  IN      TPM_PCRINDEX              PcrIndex,
     OUT  TPM_DIGEST                *NewPcrValue
  )
{
  EFI_STATUS                        Status;
  UINT32                            TpmSendSize;
  UINT32                            TpmRecvSize;
  TPM_CMD_EXTEND                    SendBuffer;
  UINT8                             RecvBuffer[10 + sizeof(TPM_DIGEST)];

  //
  // send Tpm command TPM_ORD_Extend
  //
  TpmRecvSize               = sizeof (TPM_RSP_COMMAND_HDR) + sizeof (TPM_DIGEST);
  TpmSendSize               = sizeof (TPM_CMD_EXTEND);
  SendBuffer.Hdr.tag        = SwapBytes16 (TPM_TAG_RQU_COMMAND);
  SendBuffer.Hdr.paramSize  = SwapBytes32 (TpmSendSize);
  SendBuffer.Hdr.ordinal    = SwapBytes32 (TPM_ORD_Extend);
  SendBuffer.PcrIndex       = SwapBytes32 (PcrIndex);
  CopyMem (&SendBuffer.TpmDigest, (UINT8 *)DigestToExtend, sizeof (TPM_DIGEST));
  Status = TpmOemSubmitCommand ( TpmSendSize, (UINT8 *)&SendBuffer, &TpmRecvSize, RecvBuffer);
  if( EFI_ERROR(Status) )
      return Status;
  else
  {
      TPM_RSP_COMMAND_HDR   *p_retHdr = (TPM_RSP_COMMAND_HDR*)RecvBuffer;
      if( p_retHdr->returnCode )
          return EFI_DEVICE_ERROR;
  }

  if(NewPcrValue != NULL) {
    CopyMem ((UINT8*)NewPcrValue, &RecvBuffer[10], sizeof (TPM_DIGEST));
  }

  return Status;
}


/**
  Send TSC_PhysicalPresence command to TPM.

  @param[in] PeiServices        Describes the list of possible PEI Services.
  @param[in] TpmHandle          TPM handle.  
  @param[in] PhysicalPresence   The state to set the TPMs Physical Presence flags.  
 
  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_TIMEOUT           The register can't run into the expected status in time.
  @retval EFI_BUFFER_TOO_SMALL  Response data buffer is too small.
  @retval EFI_DEVICE_ERROR      Unexpected device behavior.

**/
EFI_STATUS
TpmCommPhysicalPresence (
  IN      TPM_PHYSICAL_PRESENCE     PhysicalPresence
  )
{
  EFI_STATUS                        Status;
  UINT32                            TpmSendSize;
  UINT32                            TpmRecvSize;
  TPM_CMD_PHYSICAL_PRESENCE         SendBuffer;
  UINT8                             RecvBuffer[10];

  //
  // send Tpm command TSC_ORD_PhysicalPresence
  //
  TpmRecvSize                 = 10;
  TpmSendSize                 = sizeof (TPM_CMD_PHYSICAL_PRESENCE);
  SendBuffer.Hdr.tag          = SwapBytes16 (TPM_TAG_RQU_COMMAND);
  SendBuffer.Hdr.paramSize    = SwapBytes32 (TpmSendSize);
  SendBuffer.Hdr.ordinal      = SwapBytes32 (TSC_ORD_PhysicalPresence);
  SendBuffer.PhysicalPresence = SwapBytes16 (PhysicalPresence);
  Status = TpmOemSubmitCommand ( TpmSendSize, (UINT8 *)&SendBuffer, &TpmRecvSize, RecvBuffer);
  if( EFI_ERROR(Status) )
      return Status;
  else
  {
      TPM_RSP_COMMAND_HDR   *p_retHdr = (TPM_RSP_COMMAND_HDR*)RecvBuffer;
      if( p_retHdr->returnCode )
          return EFI_DEVICE_ERROR;
  }
  return Status;
}

EFI_STATUS Tpm12PCRRead(
    IN TPM_PCRINDEX PCRIndex,
    OUT TPM_DIGEST  *Digest )
{
    TPM_CMD_PCR_READ                SendBuffer;
    UINT32                          TpmSendSize;
    UINT32                          TpmRecvSize;
    TPM_RSP_PCR_READ                RecvBuffer;
    EFI_STATUS                      Status;

    SendBuffer.Hdr.tag       = SwapBytes16( TPM_TAG_RQU_COMMAND );
    SendBuffer.Hdr.paramSize = SwapBytes32( sizeof (TPM_CMD_PCR_READ) );

    SendBuffer.Hdr.ordinal   = SwapBytes32( TPM_ORD_PcrRead );
    SendBuffer.pcrIndex      = SwapBytes32( PCRIndex );
    
    TpmSendSize = sizeof (TPM_CMD_PCR_READ);
    TpmRecvSize = sizeof (TPM_RSP_PCR_READ);

    Status = TpmOemSubmitCommand ( TpmSendSize, (UINT8*)&SendBuffer, &TpmRecvSize, (UINT8*)&RecvBuffer);
    
    if( RecvBuffer.retHdr.returnCode == 0)
    {
        if( Digest )
            CopyMem( Digest, &RecvBuffer.pcrDigest, sizeof(TPM_DIGEST) );
        return EFI_SUCCESS;
    }

//    DEBUG ((DEBUG_INFO, "PCRRead\n" ));
//    DEBUG ((DEBUG_INFO, "Status: %r; RetCode: %x.\n", \
//                                    Status, \
//                                    TPM_H2NL(retHeader.RetCode))); 
//    DEBUG ((DEBUG_INFO, "paramSize[%x]\n",  TPM_H2NL(retHeader.ParamSize) ));

    return EFI_DEVICE_ERROR;
}

EFI_STATUS ReadTPMNVIdxData(
    UINT32              TPMIndex,
    UINTN               *DataSize,
    UINT8               *DataBuf )
{
    EFI_STATUS          Status;
    TPM_NV_READ_CMD     ReadCmd;
    UINT8               _ReadRet[0x100];
    TPM_NV_READ_RET     *ReadRet;
    UINTN               TmpVal = 0;
	UINT32				TpmRecvSize;
	UINT32				TpmSendSize;

	
	ReadRet = (VOID *)&_ReadRet;
	TpmRecvSize               = 0x100;
	TpmSendSize               = sizeof (TPM_NV_READ_CMD);
    ReadCmd.dataSize    = TPM_H2NL( (UINT32)*DataSize );
    ReadCmd.tag         = TPM_H2NS(TPM_TAG_RQU_COMMAND);
    ReadCmd.paramSize   = TPM_H2NL( sizeof(TPM_NV_READ_CMD));
    ReadCmd.ordinal     = TPM_H2NL(TPM_ORD_NV_ReadValue);
    ReadCmd.nvIndex     = TPM_H2NL(TPMIndex);
    ReadCmd.offset      = TPM_H2NL(0x00000000);

    Status = TpmOemSubmitCommand ( TpmSendSize, (UINT8 *)&ReadCmd, &TpmRecvSize, (UINT8*)ReadRet);

    if ( EFI_ERROR(Status) || (ReadRet->returnCode != 0) )
    {
    	DEBUG((EFI_D_ERROR, "(TPM Error) Status: %r; RetCode: %x.\n", Status, TPM_H2NL(ReadRet->returnCode) )); 
        Status |= EFI_ERROR_BIT;
        Status |= TPM_H2NL(ReadRet->returnCode);
//        ASSERT_EFI_ERROR(Status);
        return Status;
    }

    if( 0 == *DataSize )
        return Status;

    TmpVal = (UINTN)TPM_H2NL(ReadRet->dataSize);

    if( *DataSize < TmpVal )
    {
        ASSERT_EFI_ERROR(EFI_BUFFER_TOO_SMALL);
        return EFI_BUFFER_TOO_SMALL;
    }

    *DataSize = TmpVal;

    CopyMem (DataBuf, &ReadRet->data[0], SwapBytes32(ReadRet->dataSize));

    return EFI_SUCCESS;
}
