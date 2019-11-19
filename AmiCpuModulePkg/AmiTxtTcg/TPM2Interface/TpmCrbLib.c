#include <Token.h>
#include <Efi.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <TpmCrbLib.h>
#include <AmiTxtTcgPeim.h>

EFI_STATUS AuxCopyMem( VOID* Dest, VOID* Src, UINTN _size );

#define CopyMem         AuxCopyMem
#define SwapBytes16     TPM_H2NS
#define SwapBytes32     TPM_H2NL

#ifndef MAX_ADDRESS
#define MAX_ADDRESS (-1)
#endif

UINTN
EFIAPI
MicroSecondDelay (
  IN      UINTN                     MicroSeconds
  );

UINT8 *
EFIAPI
CrbMmioReadBuffer8 (
  IN  UINTN       StartAddress,
  IN  UINTN       Length,
  OUT UINT8      *Buffer
  )
{
    UINT8   *ReturnBuffer;

    ASSERT ((Length - 1) <=  (MAX_ADDRESS - StartAddress));
    ASSERT ((Length - 1) <=  (MAX_ADDRESS - (UINTN) Buffer));

    ReturnBuffer = Buffer;

    while (Length-- != 0) {
     *(Buffer++) = MmioRead8 (StartAddress++);
    }

   return ReturnBuffer;
}


UINT32 *
EFIAPI
CrbMmioWriteBuffer8 (
  IN  UINTN        StartAddress,
  IN  UINTN        Length,
  IN  CONST UINT8  *Buffer
  )
{
   VOID* ReturnBuffer;

   ASSERT ((Length - 1) <=  (MAX_ADDRESS - StartAddress));
   ASSERT ((Length - 1) <=  (MAX_ADDRESS - (UINTN) Buffer));

   ReturnBuffer = (UINT8 *) Buffer;

   while (Length-- != 0) {
      MmioWrite8 (StartAddress++, *(Buffer++));
   }

   return ReturnBuffer;
}

BOOLEAN dTPMCrbLocality0Granted(TPM_CRB_ACCESS_REG_PTR dCrbAccessRegPtr)
{
    UINTN       ActiveLocality = 0;

    ActiveLocality = dCrbAccessRegPtr->TpmlocState >> 2;

    if( ((ActiveLocality & 0x7)==0)             &&      // Locality is 0.
        (dCrbAccessRegPtr->TpmlocState & 0x02)  &&      // Locality is assign.
        (dCrbAccessRegPtr->TpmlocState & 0x80)  &&      // TPM is Valid.
        (dCrbAccessRegPtr->TpmlocSts & 0x01)            // Locality has been granted.
    )

    {
        return TRUE;
    }

    return FALSE;
}



EFI_STATUS dTPMCrbSetLocality(TPM_CRB_ACCESS_REG_PTR dCrbAccessRegPtr)
{
    EFI_STATUS  Status = EFI_DEVICE_ERROR;
    UINTN       DelayTime = 50;
    UINTN       Timeout   = 50, Count =0;


    while( FALSE == dTPMCrbLocality0Granted(dCrbAccessRegPtr) )
    {
        dCrbAccessRegPtr->TpmlocCtrl = 0x1;
        MicroSecondDelay(DelayTime); //delay 50us
        ++Count;
        if(Count == Timeout)return EFI_DEVICE_ERROR;
    }

    return EFI_SUCCESS;
}


EFI_STATUS dTPMCrbWaitStrtClr(TPM_CRB_ACCESS_REG_PTR dCrbAccessRegPtr)
{
    UINTN  DelayTime = 50;
    UINTN  Timeout   = 100000, Count =0;

    while(dCrbAccessRegPtr->TpmCrbCtrlStrt != 0x0)
    {
       MicroSecondDelay(DelayTime); //delay 50us
       if(Count == Timeout)break;
       Count +=1;
    }

    if(Count == Timeout)return EFI_DEVICE_ERROR;
    return EFI_SUCCESS;
}

UINT8 isTPMIdle(TPM_CRB_ACCESS_REG_PTR dCrbAccessRegPtr)
{
    if((dCrbAccessRegPtr->TpmCrbCtrlSts & 1) == 1){
        return 0xFF; //FATAL TPM ERROR
    }
    if((dCrbAccessRegPtr->TpmCrbCtrlSts & 2) == 2){
        return 1; //idle state
    }
    return 0;
}


EFI_STATUS dTPMCrbSetReqReadyState(TPM_CRB_ACCESS_REG_PTR dCrbAccessRegPtr)
{
    UINTN  DelayTime = 50;
    UINTN  Timeout   = 10000, Count =0;

    do
    {
        dCrbAccessRegPtr->TpmCrbCtrlReq = 0x01;
        MicroSecondDelay(DelayTime); //delay 50us
        if(Count == Timeout)break;
        Count +=1;
    } while( (isTPMIdle(dCrbAccessRegPtr)) == 1 );  //wait till ready

    if(Count == Timeout)return EFI_DEVICE_ERROR;
    return EFI_SUCCESS;
}


EFI_STATUS dTPMCrbSetReqIdleState(TPM_CRB_ACCESS_REG_PTR dCrbAccessRegPtr)
{
    UINTN  DelayTime = 100;
    UINTN  Timeout   = 50000, Count =0;


    do{
       dCrbAccessRegPtr->TpmCrbCtrlReq = 0x2;
       MicroSecondDelay(DelayTime); //delay 100us
       if(Count == Timeout)break;
       Count +=1;
    } while( (isTPMIdle(dCrbAccessRegPtr)) == 0 ); //wait till idle

    if(Count == Timeout)return EFI_DEVICE_ERROR;
    return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
CrbSend(
  IN     UINT8      *InputBuffer,
  IN     UINT32     DataLength
)
{
    TPM_CRB_ACCESS_REG_PTR  dCrbAccessRegPtr = (TPM_CRB_ACCESS_REG_PTR)(( UINTN ) (PORT_TPM_IOMEMBASE));
    UINTN                   CmdBase;
    EFI_STATUS              Status;

    Status = dTPMCrbSetLocality(dCrbAccessRegPtr);
    if(EFI_ERROR(Status))return Status;

    Status = dTPMCrbSetReqReadyState(dCrbAccessRegPtr);
    if(EFI_ERROR(Status)){
        return Status;
    }

    if( dCrbAccessRegPtr->TpmCrbCtrlCmdLAddr == 0 )
    {
        dCrbAccessRegPtr->TpmCrbCtrlCmdLAddr = 0xFED40080;
        dCrbAccessRegPtr->TpmCrbCtrlCmdSize = 0x500;
        dCrbAccessRegPtr->TpmCrbCtrlResdLAddr = 0xFED40080;
        dCrbAccessRegPtr->TpmCrbCtrlRespSize = 0x500;
    }

    if(dCrbAccessRegPtr->TpmCrbCtrlCmdLAddr !=0)
    {

        CmdBase = (UINTN)dCrbAccessRegPtr->TpmCrbCtrlCmdLAddr;

        CrbMmioWriteBuffer8(CmdBase, DataLength, InputBuffer);

        Status = dTPMCrbWaitStrtClr(dCrbAccessRegPtr);
        if(EFI_ERROR(Status))return Status;

        dCrbAccessRegPtr->TpmCrbCtrlStrt = 1;
    }

    return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
CrbReceive(
  OUT     UINT8     *OutBuffer,
  OUT     UINT32    *RespSize
)
{
    TPM_CRB_ACCESS_REG_PTR dCrbAccessRegPtr = (TPM_CRB_ACCESS_REG_PTR)(( UINTN ) (PORT_TPM_IOMEMBASE));
    UINTN RspBase;
    UINT16 Data16;
    UINT32 Data32;
    EFI_STATUS Status;

    Status = dTPMCrbWaitStrtClr(dCrbAccessRegPtr);
    if(EFI_ERROR(Status))return Status;

    if(dCrbAccessRegPtr->TpmCrbCtrlResdLAddr !=0)
    {
        RspBase = (UINTN) dCrbAccessRegPtr->TpmCrbCtrlResdLAddr;

        CrbMmioReadBuffer8(RspBase, RESPONSE_HEADER_SIZE, OutBuffer);

        CopyMem (&Data16, OutBuffer, sizeof (UINT16));

        if(SwapBytes16(Data16) == TPM_ST_RSP_COMMAND)
        {
           return EFI_DEVICE_ERROR;
        }

        CopyMem(&Data32, (OutBuffer + 2), sizeof(UINT32));

        *RespSize = SwapBytes32(Data32);

        if(*RespSize > dCrbAccessRegPtr->TpmCrbCtrlRespSize)
        {
            return EFI_BUFFER_TOO_SMALL;
        }

        if(*RespSize <  sizeof(GENERIC_RESP_HDR))
        {
            return EFI_DEVICE_ERROR;
        }

        CrbMmioReadBuffer8(RspBase, *RespSize,  OutBuffer);

//#if defined (SET_TPM_IDLE) && (SET_TPM_IDLE == 1)
        Status = dTPMCrbSetReqIdleState(dCrbAccessRegPtr);
        if(EFI_ERROR(Status))return Status;
//#endif
    }

    return EFI_SUCCESS;
}



EFI_STATUS
EFIAPI
CrbSubmitCmd(
  IN      UINT8     *InputBuffer,
  IN      UINT32     InputBufferSize,
  OUT     UINT8     *OutputBuffer,
  OUT     UINT32    *OutputBufferSize
  )
{
    EFI_STATUS Status;

    Status = CrbSend(InputBuffer, InputBufferSize);
    if (EFI_ERROR (Status))  {
      return Status;
    }

    ///
    /// Receive the response data from TPM
    ///
    Status = CrbReceive(OutputBuffer, OutputBufferSize);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    return Status;

}

EFI_STATUS
EFIAPI
PttSubmitCommand(
  IN VOID*          This,
  IN UINT32         InputParameterBlockSize,
  IN UINT8          *InputParameterBlock,
  IN UINT32         OutputParameterBlockSize,
  IN UINT8          *OutputParameterBlock
  )
{
    return CrbSubmitCmd(
                InputParameterBlock,
                InputParameterBlockSize,
                OutputParameterBlock,
                &OutputParameterBlockSize
            );
}
