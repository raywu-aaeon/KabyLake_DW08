/*++

Copyright (c) 2005 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  TpmLib.h

Abstract:

  Common code of TPM driver in both PEI and DXE
--*/
#include <Efi.h>
#include <AmiLib.h>
#include <Token.h>
#include <TpmTisLib.h>
#include <AmiTxtTcgPeim.h>

extern EFI_STATUS CountTime ( IN UINTN	DelayTime,  IN	UINT16	BaseAddr); // only needs to be 16 bit for I/O address)
#define Wait  TPM_DRIVER_WAIT 
#define ACCESS_WAITCOUNT    (750 * 1000 / 100)    // 750MS (750 * 10000 /1000)
#define ACCESS_WAITCOUNTB   (  3 * 1000000/100) // 3 seconds delay

#define TIS_TIMEOUT_A   ACCESS_WAITCOUNT
#define TIS_TIMEOUT_B   ACCESS_WAITCOUNTB
#define TIS_TIMEOUT_C   ACCESS_WAITCOUNT
#define TIS_TIMEOUT_D   ACCESS_WAITCOUNT


#define TisCompleteCommand          TisPrepareSendCommand

UINTN
EFIAPI
MicroSecondDelay (
  IN      UINTN                     MicroSeconds
  );

EFI_STATUS TcgCountTime (
      IN  UINTN   DelayTime
  )
{
    MicroSecondDelay(DelayTime);
    return EFI_SUCCESS;
}

//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   FixedDelay
//
// Description: This routine delays for specified number of micro seconds
//
// Input:       IN UINT32  dCount      Amount of delay (count in 20microsec)
//
// Output:      NONE
//
// Modified:
//
// Referrals:   Div64, CountTime
//
// Notes:       
//<AMI_PHDR_END>
//**********************************************************************

void
FixedDelay(UINT32 dCount)
{
  UINTN  MicroSDelay = DELAY_AMOUNT;

  while(dCount) {
      TcgCountTime(MicroSDelay);  
      dCount--;
  }
}



static
UINT8
CheckAccessBit (
  IN      volatile UINT8   *Sts,
  IN      UINT8             Bit,
  IN      UINT32             Timeout    
  )
/*++
Routine Description:
  Function to check bits in TPM access register
Arguments:
  *Sts      - A Pointer to Status register
  Bit       - Bit position
  Timeout    - Timeout amount to wait till the specified bit
Returns:
  EFI_Status
--*/
{
  UINT32  AccessCount = Timeout;            
// Fix for ST TPM Chip for Hardware protocol +>
//  if(!(*Sts & TPM_STS_VALID))
//    return 0;
// <+ End for ST TPM Chip.
  do {

     FixedDelay((UINT32)Wait);  
//    if((*Sts & Bit)){ return *Sts & Bit; }    // Improvement for ST TPM Hardware protocol.
     if((*Sts & Bit) && (*Sts & TPM_STS_VALID) ){ return *Sts & Bit; }  // Improvement for ST TPM Hardware protocol.
     AccessCount--;
  } while (AccessCount);

  return 0;    
}



static
UINT8
CheckStsBit (
  IN      volatile UINT8            *Sts,
  IN      UINT8                     Bit
  )
/*++
Routine Description:
  Function to check status a specific TPM status bit 
Arguments:
  *Sts  - A Pointer to Status register
  Bit   - Bit position
Returns:
  EFI_Status
--*/
{
//  while (!(*Sts & TPM_STS_VALID));
  while (!(*Sts & TPM_STS_VALID))
  {
	FixedDelay((UINT32)Wait);
  }
  return *Sts & Bit;
}




static
UINT16
ReadBurstCount (
  IN      TPM_1_2_REGISTERS_PTR     TpmReg
)
/*++
Routine Description:
  Gets the number of bytes (burstCount) that the TPM can return
  on reads or accept on writes without inserting LPC long wait 
  states on the LPC bus.
  burstCount is TPM_STS_x register bits 8..23
Arguments:
  TpmReg           - A Pointer to the TPM Register Space
Returns:
  0                - Any Errors
  burstCount       - TPM_STS_x.burstCount
--*/  
{
  UINT16   burstCount;
  UINT64   Deadline = TIS_TIMEOUT_D;

  burstCount = 0;

  do {
    //
    // burstCount is little-endian bit ordering
    //
    FixedDelay((UINT32)Wait);
    Deadline--;
    burstCount = TpmReg->BurstCount;
  }while (!burstCount && (Deadline > 0));
  
  return burstCount;
}



EFI_STATUS
TisRequestLocality (
  IN      TPM_1_2_REGISTERS_PTR     TpmReg
  )
/*++
Routine Description:
  Requests TPM locality 0
Arguments:
  TpmReg           - A Pointer to the TPM Register Space
Returns:
  EFI_Status
--*/  
{
  if ((TpmReg->Access & TPM_ACC_ACTIVE_LOCALITY)==TPM_ACC_ACTIVE_LOCALITY){
    return EFI_SUCCESS;//EFI_ALREADY_STARTED;
  }
  TpmReg->Access = TPM_ACC_REQUEST_USE;
  FixedDelay((UINT32)0x14);
  if (CheckAccessBit(&TpmReg->Access, TPM_ACC_ACTIVE_LOCALITY, (UINT32)TIS_TIMEOUT_B)) {
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_READY;
  }
}



#pragma optimize("",off)
EFI_STATUS
TisReleaseLocality (
  IN      TPM_1_2_REGISTERS_PTR     TpmReg
  )
/*++
Routine Description:
  Release TPM locality 0
Arguments:
  TpmReg           - A Pointer to the TPM Register Space
Returns:
  EFI_Status
--*/
{
  UINT32    AccessCount=ACCESS_WAITCOUNT;    
  if (!CheckStsBit (&TpmReg->Access, TPM_ACC_ACTIVE_LOCALITY)) {
    return EFI_DEVICE_ERROR;
  }

  TpmReg->Access = TPM_ACC_ACTIVE_LOCALITY;
  FixedDelay((UINT32)0x14);
  if (CheckStsBit(&TpmReg->Access, TPM_ACC_ACTIVE_LOCALITY)) {
    do{/*give locality time to be released*/
     FixedDelay((UINT32)Wait); 
       AccessCount--;
    }while(((CheckStsBit(&TpmReg->Access, TPM_ACC_ACTIVE_LOCALITY)) && AccessCount));
    if(CheckStsBit(&TpmReg->Access, TPM_ACC_ACTIVE_LOCALITY)){return EFI_DEVICE_ERROR;}
    else{return EFI_SUCCESS;}
  } else {
    return EFI_SUCCESS;
  }
}
#pragma optimize("",on)




EFI_STATUS
TisPrepareSendCommand (
  IN      TPM_1_2_REGISTERS_PTR     TpmReg
  )
/*++
Routine Description:
  Helper function to prepare to send a TPM command
Arguments:
  TpmReg           - A Pointer to the TPM Register Space
Returns:
  EFI_Status
--*/
{

 if((TpmReg->Access & TPM_ACC_ACTIVE_LOCALITY)!=TPM_ACC_ACTIVE_LOCALITY){
    return EFI_NOT_STARTED;
  }

  do {
    TpmReg->Sts = TPM_STS_READY;
  } while (!(TpmReg->Sts & TPM_STS_READY));
  return EFI_SUCCESS;
}

EFI_STATUS
TisSendCommand (
  IN      TPM_1_2_REGISTERS_PTR     TpmReg,
  IN      const VOID                *CmdStream,
  IN      UINTN                     Size,
  IN      BOOLEAN                   Last
  )
/*++
Routine Description:
  Function to send TPM command
Arguments:
  TpmReg       - A Pointer to the TPM Register Space
  *CmdStream   - A Pointer to the command stream to be sent to TPM Fifo
  Size         - Size in bytes of the command stream
  Last         - Boolean to signify the last byte?
Returns:
  EFI_Status
--*/
{
  UINT8                             *Ptr;
  UINT16                            burstCount;

  if (Size == 0) {
    return Last ? EFI_INVALID_PARAMETER : EFI_SUCCESS;
  }

  if ((TpmReg->Access & TPM_ACC_ACTIVE_LOCALITY)!=TPM_ACC_ACTIVE_LOCALITY) {
    return EFI_ABORTED;
  }

  Ptr = (UINT8*)CmdStream;

  if (Last) {
    Size--;
  }

  while (Size > 0) {
      burstCount = ReadBurstCount(TpmReg);
    if (burstCount == 0) {
      // Cannot get the correct burstCount value
      return EFI_TIMEOUT;
    }

    for (; burstCount > 0 && Size > 0; burstCount--) {
     *(UINT8*)&TpmReg->DataFifo = *Ptr;
      Ptr++;
      Size--;
    }
   }

  if (Last) {
    if (!CheckStsBit (&TpmReg->Sts, TPM_STS_EXPECT)) {
      return EFI_ABORTED;
    }

    *(UINT8*)&TpmReg->DataFifo = *Ptr;

    if (CheckStsBit (&TpmReg->Sts, TPM_STS_EXPECT)) {
      return EFI_ABORTED;
    }

    TpmReg->Sts = TPM_STS_GO;
            
  }
  return EFI_SUCCESS;
}




EFI_STATUS
TisWaitForResponse (
  IN      TPM_1_2_REGISTERS_PTR     TpmReg
  )
/*++
Routine Description:
  Waits till TPM result is available
Arguments:
  TpmReg           - A Pointer to the TPM Register Space
Returns:
  EFI_Status
--*/
{
  if((TpmReg->Access & TPM_ACC_ACTIVE_LOCALITY)!=TPM_ACC_ACTIVE_LOCALITY){
    return EFI_NOT_STARTED;
  }

  while (!CheckStsBit (&TpmReg->Sts, TPM_STS_DATA_AVAILABLE));
  return EFI_SUCCESS;
}




EFI_STATUS
TisReceiveResponse (
  IN      TPM_1_2_REGISTERS_PTR     TpmReg,
  OUT     VOID                      *Buffer,
  OUT     UINTN                     *Size
  )
/*++
Routine Description:
  Function to recieve TPM command results
Arguments:
  TpmReg       - A Pointer to the TPM Register Space
  *Buffer      - A Pointer to buffer for recieving result data
  Size         - buffer size
Returns:
  EFI_Status
--*/
{
  UINT8                             *Ptr, *BufEnd;
  UINT16                             burstCount;

  if((TpmReg->Access & TPM_ACC_ACTIVE_LOCALITY)!=TPM_ACC_ACTIVE_LOCALITY) {
      return EFI_ABORTED;
  }

  Ptr = (UINT8*)Buffer;
  BufEnd = Ptr + *Size;
  while (Ptr < BufEnd &&
         CheckStsBit (&TpmReg->Sts, TPM_STS_DATA_AVAILABLE)) {
      
        burstCount = ReadBurstCount(TpmReg);
          if (burstCount == 0) {
              return EFI_TIMEOUT;
        }            
        while(burstCount && Ptr < BufEnd  && CheckStsBit (&TpmReg->Sts, TPM_STS_DATA_AVAILABLE )){
            *Ptr++ = *(UINT8*)&TpmReg->DataFifo;
            burstCount--;    }
  }

  *Size -= BufEnd - Ptr;
  if (CheckStsBit (&TpmReg->Sts, TPM_STS_DATA_AVAILABLE)) {
    return EFI_BUFFER_TOO_SMALL;
  } else {
    return EFI_SUCCESS;
  }
}



VOID
TisResendResponse (
  IN      TPM_1_2_REGISTERS_PTR     TpmReg
  )
/*++
Routine Description:
  Sets Bit to resend TPM command
Arguments:
  TpmReg           - A Pointer to the TPM Register Space
Returns:
  EFI_Status
--*/
{
  TpmReg->Sts = TPM_STS_RESPONSE_RETRY;
}

EFI_STATUS
IsTpmPresent (
  IN      TPM_1_2_REGISTERS_PTR     TpmReg
  )
{
  if (TpmReg->Access == 0xff) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}




EFI_STATUS
TpmLibPassThrough (
  IN      TPM_1_2_REGISTERS_PTR     TpmReg,
  IN      UINTN                     NoInputBuffers,
  IN      TPM_TRANSMIT_BUFFER       *InputBuffers,
  IN      UINTN                     NoOutputBuffers,
  IN OUT  TPM_TRANSMIT_BUFFER       *OutputBuffers
  )
/*++
Routine Description:
  Higher level function to send a recieve commands to the TPM
Arguments:
  TpmReg           - A Pointer to the TPM Register Space
  NoInputBuffers   - Number count of Input buffers
  *InputBuffers    - Pointer to InputBuffers[0]
  NoOutputBuffers  - Number count of Output buffers
  *OutputBuffers   - Pointer to OutputBuffers[0]
Returns:
  EFI_Status
--*/
{
  EFI_STATUS                        Status;
  UINTN                             i;

  if (NoInputBuffers == 0 || InputBuffers->Size < sizeof (TPM_1_2_CMD_HEADER)) {
    return EFI_INVALID_PARAMETER;
  }

  do {
    NoInputBuffers--;
  } while (InputBuffers[NoInputBuffers].Size == 0 && NoInputBuffers > 0);

  if (InputBuffers[NoInputBuffers].Size == 0) {
    return EFI_INVALID_PARAMETER;
  }

  Status = TisPrepareSendCommand (TpmReg);

  for (i = 0; !EFI_ERROR (Status) && i < NoInputBuffers; i++) {
    Status = TisSendCommand (
      TpmReg,
      InputBuffers[i].Buffer,
      InputBuffers[i].Size,
      FALSE
      );
  }

  if (!EFI_ERROR (Status)) {
    Status = TisSendCommand (
      TpmReg,
      InputBuffers[i].Buffer,
      InputBuffers[i].Size,
      TRUE
      );
  }

  if (!EFI_ERROR (Status)) {
    Status = TisWaitForResponse (TpmReg);
  }

  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  Status = EFI_BUFFER_TOO_SMALL;
  for (i = 0; Status == EFI_BUFFER_TOO_SMALL && i < NoOutputBuffers; i++) {
    Status = TisReceiveResponse (
      TpmReg,
      OutputBuffers[i].Buffer,
      &OutputBuffers[i].Size
      );
  }

Exit:
  TisCompleteCommand (TpmReg);
  return Status;
}

EFI_STATUS EFIAPI TisTcgPassThroughToTpm(
    IN UINT32           TpmInputParamterBlockSize,
    IN UINT8            *TpmInputParamterBlock,
    IN UINT32           TpmOutputParameterBlockSize,
    IN UINT8            *TpmOutputParameterBlock )
{
    TPM_TRANSMIT_BUFFER InBuffer[1], OutBuffer[1];
    EFI_STATUS Status;
 
    InBuffer[0].Buffer  = TpmInputParamterBlock;
    InBuffer[0].Size    = TpmInputParamterBlockSize;
    OutBuffer[0].Buffer = TpmOutputParameterBlock;
    OutBuffer[0].Size   = TpmOutputParameterBlockSize;

    // Init
    TisRequestLocality( (TPM_1_2_REGISTERS_PTR)(UINTN)TPM_BASE_ADDRESS );

    Status = TpmLibPassThrough(
                        (TPM_1_2_REGISTERS_PTR)(UINTN)TPM_BASE_ADDRESS,
                        sizeof (InBuffer) / sizeof (*InBuffer),
                        (TPM_TRANSMIT_BUFFER*)(UINTN)InBuffer,
                        sizeof (OutBuffer) / sizeof (*OutBuffer),
                        (TPM_TRANSMIT_BUFFER*)(UINTN)OutBuffer
                         );

    // Close
    TisReleaseLocality( (TPM_1_2_REGISTERS_PTR)(UINTN)TPM_BASE_ADDRESS );

    return Status;
}

EFI_STATUS
TpmLibPassThroughEX (
    IN VOID*          This,
    IN UINT32         InputParameterBlockSize,
    IN UINT8          *InputParameterBlock,
    IN UINT32         OutputParameterBlockSize,
    IN UINT8          *OutputParameterBlock
)
{
    EFI_STATUS          Status;
    
    Status =  TisTcgPassThroughToTpm(
                    InputParameterBlockSize,
                    InputParameterBlock,
                    OutputParameterBlockSize,
                    OutputParameterBlock
                    );
    
    return Status;
}
