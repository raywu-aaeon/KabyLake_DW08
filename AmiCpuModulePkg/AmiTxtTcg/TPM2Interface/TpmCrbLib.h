/** @file
  Platform Trust Technology (FTPM) PTP ( Platform TPM Profile ) Device Library

@copyright
  Copyright (c) 2012 -2014 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/

#include "TpmCrbReg.h"

#ifndef _TPM20_PTP_CRB_LIB_H_
#define _TPM20_PTP_CRB_LIB_H_

#define RESPONSE_HEADER_SIZE  12

#pragma pack (1)

typedef struct _GENERIC_RESP_HDR_
{
    UINT16 Tag;
    UINT32 RespSize;
    UINT32 RespCode;
}GENERIC_RESP_HDR;

typedef struct _TPM_CRB_ACCESS_REG_
{
    UINT32 TpmlocState; //00-03
    UINT32 Reserved;    //04-07
    UINT32 TpmlocCtrl;  //08-0B
    UINT32 TpmlocSts;   //0C-0F
    UINT32 Reserved1[0x8];    //10-2F
    UINT32 TpmCrbIntfId[2];   //30-37
    UINT32 TpmCrbCtrlExt[2];  //38-3F
    UINT32 TpmCrbCtrlReq;     //40-43
    UINT32 TpmCrbCtrlSts;     //44-47
    UINT32 TpmCrbCtrlCncl;    //48-4B
    UINT32 TpmCrbCtrlStrt;    //4C-4F
    UINT32 TpmCrbCtrlInt[2];  //50-57
    UINT32 TpmCrbCtrlCmdSize; //58-5B
    UINT32 TpmCrbCtrlCmdLAddr; //5C-5F
    UINT32 TpmCrbCtrlCmdHAddr; //60-63
    UINT32 TpmCrbCtrlRespSize; //64-67
    UINT32 TpmCrbCtrlResdLAddr; //68-6B
    UINT32 TpmCrbCtrlResdHAddr; //6C-6F
    UINT32 Reserved2[0x4];     //70-7F
}TPM_CRB_ACCESS_REG;

#pragma pack ()

typedef volatile TPM_CRB_ACCESS_REG *TPM_CRB_ACCESS_REG_PTR;

EFI_STATUS
EFIAPI
PttSubmitCommand(
  IN VOID*          This,
  IN UINT32         InputParameterBlockSize,
  IN UINT8          *InputParameterBlock,
  IN UINT32         OutputParameterBlockSize,
  IN UINT8          *OutputParameterBlock
  );

#endif
