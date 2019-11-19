#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Board/BoardConfigInit.h>

EFI_STATUS
AmiGetBoardInfo (
  OUT UINT16          *BoardId,
  OUT UINT16          *BoardIdOrgValue
  )
{
  BOARD_PRIVATE_DATA *BoardInfo;

  BoardInfo = (BOARD_PRIVATE_DATA *) PcdGetPtr (PcdBoardPrivateData);

  *BoardId = BoardInfo->BoardId;
  *BoardIdOrgValue = BoardInfo->BoardIdOrgValue;

  DEBUG ((DEBUG_INFO, "AmiGetBoardInfo:: Got BoardInfo->BoardId: 0x%x  \n", *BoardId));
  DEBUG ((DEBUG_INFO, "AmiGetBoardInfo:: Got BoardInfo->BoardIdOrgValue: 0x%x  \n", *BoardIdOrgValue));

  return EFI_SUCCESS;
}
