

#ifndef _TPM2_COMMON_LIB_H_
#define _TPM2_COMMON_LIB_H_

#include "TPM20CommLib/Tpm2CommandLib.h"
#include "Tpm12Comm.h"

#define SwapBytes16 TPM_H2NS
#define SwapBytes32 TPM_H2NL

BOOLEAN AuxChkIsTpmPresent();
BOOLEAN AuxChkIsTpmPTPCRB();
BOOLEAN AuxChkIsTpmTis();

EFI_STATUS
EFIAPI
TpmOemSubmitCommand (
  IN UINT32            InputParameterBlockSize,
  IN UINT8             *InputParameterBlock,
  IN OUT UINT32        *OutputParameterBlockSize,
  IN UINT8             *OutputParameterBlock
  );

VOID 
PrintBuf(
    UINTN   unBufSize,
    UINT8*  _buf
);

UINT16
EFIAPI
ReadUnaligned16 (
  IN CONST UINT16              *Buffer
  );

UINT16
EFIAPI
WriteUnaligned16 (
  OUT UINT16                    *Buffer,
  IN  UINT16                    Value
  );

UINT32
EFIAPI
ReadUnaligned24 (
  IN CONST UINT32              *Buffer
  );

UINT32
EFIAPI
WriteUnaligned24 (
  OUT UINT32                    *Buffer,
  IN  UINT32                    Value
  );

UINT32
EFIAPI
ReadUnaligned32 (
  IN CONST UINT32              *Buffer
  );

UINT32
EFIAPI
WriteUnaligned32 (
  OUT UINT32                    *Buffer,
  IN  UINT32                    Value
  );

EFI_STATUS 
AuxCopyMem( 
  VOID* Dest, 
  VOID* Src, 
  UINTN _size 
  );

#endif
