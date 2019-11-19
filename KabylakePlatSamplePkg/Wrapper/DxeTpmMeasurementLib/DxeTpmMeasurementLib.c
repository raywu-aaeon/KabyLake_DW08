#include <AmiTcg/sha.h>
#include <AmiTcg/TrEEProtocol.h>
#include <Protocol/TcgService.h>
#include <Token.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>

#define INTERANL_EVENT_BUFFER_SIZE  0x400
UINT8   u8IntBuf[INTERANL_EVENT_BUFFER_SIZE];

EFI_STATUS
EFIAPI
TpmMeasureAndLogData (
    UINT32              PcrIndex,
    UINT32              EventType,
    VOID                *EventLog,
    UINT32              LogLen,
    VOID                *HashData,
    UINT64              HashDataLen
)
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    EFI_TREE_PROTOCOL           *mTreeProtocol = NULL;
    TrEE_EVENT                  *Tcg2Event = NULL;
    EFI_TCG_PROTOCOL            *tcgProtocol = NULL;
    TCG_PCR_EVENT               *TcgEvent = NULL;
    EFI_PHYSICAL_ADDRESS        Last;
    UINT32                      evNum;

    // Check the TPM 2.0 Device
    do
    {
        Status = gBS->LocateProtocol (&gEfiTrEEProtocolGuid, NULL, (VOID **) &mTreeProtocol);
        if (EFI_ERROR (Status))
        {
            DEBUG(( -1, "[%d]: Locate gEfiTrEEProtocolGuid[%g], -%r\n", __LINE__, &gEfiTrEEProtocolGuid, Status));
            break;
        }

        Tcg2Event = (TrEE_EVENT*)&u8IntBuf[0];

        Tcg2Event->Header.PCRIndex   = PcrIndex;
        Tcg2Event->Header.EventType  = EventType;
        Tcg2Event->Header.HeaderSize = sizeof(Tcg2Event->Header);
        Tcg2Event->Header.HeaderVersion  = 1;

        if (LogLen > INTERANL_EVENT_BUFFER_SIZE)
        {
            Status = EFI_BUFFER_TOO_SMALL;
            DEBUG(( -1, "[%d]: Fail on Buffer too small (0x%x > 0x%x)\n", __LINE__, LogLen, INTERANL_EVENT_BUFFER_SIZE));
            ASSERT_EFI_ERROR(Status);
            break;
        }
        gBS->CopyMem (&Tcg2Event->Event[0], EventLog, LogLen);

        Tcg2Event->Size  = sizeof(TrEE_EVENT_HEADER) + sizeof(UINT32) + LogLen;

        Status = mTreeProtocol->HashLogExtendEvent(
                                            mTreeProtocol,
                                            0,
                                            (EFI_PHYSICAL_ADDRESS)HashData,
                                            HashDataLen,
                                            Tcg2Event
                                            );
        if (EFI_ERROR (Status))
        {
            DEBUG(( -1, "[%d]: Fail on TreeProtocol->HashLogExtendEvent(...) -%r\n", __LINE__, Status));
            ASSERT_EFI_ERROR(Status);
            break;
        }

        DEBUG(( -1, "[%d]: Success on TPM 2.0 TpmMeasureAndLogData(...) -%r\n", __LINE__, Status));
    } while (FALSE);

    // Check the TPM 1.2 Device
    do
    {
        if (!EFI_ERROR (Status))
        {
            break;
        }

        Status = gBS->LocateProtocol(
                                     &gEfiTcgProtocolGuid,
                                     NULL,
                                     (VOID **) &tcgProtocol );
        if (EFI_ERROR (Status))
        {
            DEBUG(( -1, "[%d]: Locate gEfiTcgProtocolGuid[%g], -%r\n", __LINE__, &gEfiTcgProtocolGuid, Status));
            break;
        }

        TcgEvent = (TCG_PCR_EVENT*)&u8IntBuf[0];

        TcgEvent->EventSize     = LogLen;
        TcgEvent->PCRIndex      = PcrIndex;
        TcgEvent->EventType     = EventType;

        if (LogLen > INTERANL_EVENT_BUFFER_SIZE)
        {
            Status = EFI_BUFFER_TOO_SMALL;
            DEBUG(( -1, "[%d]: Fail on Buffer too small (0x%x > 0x%x)\n", __LINE__, LogLen, INTERANL_EVENT_BUFFER_SIZE));
            ASSERT_EFI_ERROR(Status);
            break;
        }
        gBS->CopyMem (&TcgEvent->Event[0], EventLog, LogLen);

        Status = tcgProtocol->HashLogExtendEvent(
                                        tcgProtocol,
                                        (EFI_PHYSICAL_ADDRESS)HashData,
                                        HashDataLen,
                                        TCG_ALG_SHA,
                                        TcgEvent,
                                        &evNum,
                                        &Last );
        if (EFI_ERROR (Status))
        {
            DEBUG(( -1, "[%d]: Fail on tcgProtocol->HashLogExtendEvent(...) -%r\n", __LINE__, Status));
            ASSERT_EFI_ERROR(Status);
            break;
        }

        DEBUG(( -1, "[%d]: Success on TPM 1.2 TpmMeasureAndLogData(...) -%r\n", __LINE__, Status));
    } while (FALSE);

    return Status;
}
