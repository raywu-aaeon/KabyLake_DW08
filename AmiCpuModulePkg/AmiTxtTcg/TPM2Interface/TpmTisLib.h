
#ifndef _TPMTISLIB_H_
#define _TPMTISLIB_H_

#pragma pack (push)
#pragma pack (1)

typedef struct _TPM_1_2_REGISTERS
{
    UINT8  Access;            // 0
    UINT8  Reserved1[7];      // 1
    UINT32 IntEnable;         // 8
    UINT8  IntVector;         // 0ch
    UINT8  Reserved2[3];      // 0dh
    UINT32 IntSts;            // 10h
    UINT32 IntfCapability;    // 14h
    UINT8  Sts;               // 18h
    UINT16 BurstCount;        // 19h
    UINT8  Reserved3[9];
    UINT32 DataFifo;          // 24h
    UINT8  Reserved4[8];      // 28h
    UINT32 InterfaceID;       // 30h
    UINT8  Reserved5[0xecc];  // 34h
    UINT16 Vid;               // 0f00h
    UINT16 Did;               // 0f02h
    UINT8  Rid;               // 0f04h
    UINT8  TcgDefined[0x7b];  // 0f05h
    UINT32 LegacyAddress1;    // 0f80h
    UINT32 LegacyAddress1Ex;  // 0f84h
    UINT32 LegacyAddress2;    // 0f88h
    UINT32 LegacyAddress2Ex;  // 0f8ch
    UINT8  VendorDefined[0x70]; // 0f90h
} TPM_1_2_REGISTERS;

typedef volatile TPM_1_2_REGISTERS *TPM_1_2_REGISTERS_PTR;

typedef struct _TPM_TRANSMIT_BUFFER {
  VOID                              *Buffer;
  UINTN                             Size;
} TPM_TRANSMIT_BUFFER;

typedef struct _TPM_1_2_CMD_HEADER
{
    UINT16          Tag;
    UINT32          ParamSize;
    UINT32          Ordinal;
} TPM_1_2_CMD_HEADER;

typedef struct _TPM_1_2_RET_HEADER
{
    UINT16          Tag;
    UINT32          ParamSize;
    UINT32          RetCode;
} TPM_1_2_RET_HEADER;

#pragma pack (pop)

typedef volatile TPM_1_2_REGISTERS* TPM_1_2_REGISTERS_PTR;

#define SHTBIT(x)               ( 1 << (x) )

#define TPM_ACC_VALID               SHTBIT( 7 )
#define TPM_ACC_ACTIVE_LOCALITY     SHTBIT( 5 )
#define TPM_ACC_BEEN_SEIZED         SHTBIT( 4 )
#define TPM_ACC_SEIZE               SHTBIT( 3 )
#define TPM_ACC_PENDING_REQUEST     SHTBIT( 2 )
#define TPM_ACC_REQUEST_USE         SHTBIT( 1 )
#define TPM_ACC_ESTABLISHMENT       SHTBIT( 0 )

#define TPM_STS_VALID               SHTBIT( 7 )
#define TPM_STS_READY               SHTBIT( 6 )
#define TPM_STS_GO                  SHTBIT( 5 )
#define TPM_STS_DATA_AVAILABLE      SHTBIT( 4 )
#define TPM_STS_EXPECT              SHTBIT( 3 )
#define TPM_STS_RESPONSE_RETRY      SHTBIT( 1 )

#define TPM_BASE_ADDRESS            0xFED40000

EFI_STATUS
TpmLibPassThrough (
  IN      TPM_1_2_REGISTERS_PTR     TpmReg,
  IN      UINTN                     NoInputBuffers,
  IN      TPM_TRANSMIT_BUFFER       *InputBuffers,
  IN      UINTN                     NoOutputBuffers,
  IN OUT  TPM_TRANSMIT_BUFFER       *OutputBuffers
);

EFI_STATUS
TisRequestLocality (
  IN      TPM_1_2_REGISTERS_PTR     TpmReg
);

EFI_STATUS
TisReleaseLocality (
  IN      TPM_1_2_REGISTERS_PTR     TpmReg
);

EFI_STATUS
TpmLibPassThroughEX (
    IN VOID*          This,
    IN UINT32         InputParameterBlockSize,
    IN UINT8          *InputParameterBlock,
    IN UINT32         OutputParameterBlockSize,
    IN UINT8          *OutputParameterBlock
);

#endif
