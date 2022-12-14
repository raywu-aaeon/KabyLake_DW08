/** @file
  ESRT operation protocol definition.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2013 - 2016 Intel Corporation.

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Intel Corporation or its suppliers or
  licensors. Title to the Material remains with Intel Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary and
  confidential information of Intel Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Intel in writing.

  Unless otherwise agreed by Intel in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Intel or
  Intel's suppliers or licensors in any way.

  This file contains 'Framework Code' and is licensed as such under the terms
  of your license agreement with Intel or your vendor. This file may not be 
  modified, except as allowed by additional terms of your license agreement.

@par Specification Reference:

**/

#ifndef _ESRT_OPERATION_H_
#define _ESRT_OPERATION_H_

#pragma pack(1)
typedef struct {
  UINT32                        NumEntries;
  EFI_SYSTEM_RESOURCE_ENTRY     FwEntries[256];
} EFI_SYSTEM_RESOURCE_ENTRY_LIST;
#pragma pack()

typedef
EFI_STATUS
(EFIAPI *ESRT_POPULATE_TABLE) (
);

typedef
EFI_STATUS
(EFIAPI *ESRT_UPDATE_TABLE_ENTRY_BY_GUID) (
  IN EFI_GUID FwEntryGuid,
  IN EFI_SYSTEM_RESOURCE_ENTRY *FwEntry
);

typedef
EFI_STATUS
(EFIAPI *ESRT_GET_FW_ENTRY_BY_GUID) (
  IN EFI_GUID FwEntryGuid,
  OUT EFI_SYSTEM_RESOURCE_ENTRY *FwEntry
);

typedef struct _ESRT_OPERATION_PROTOCOL {
   ESRT_POPULATE_TABLE             EsrtPopulateTable;
   ESRT_UPDATE_TABLE_ENTRY_BY_GUID EsrtUpdateTableEntryByGuid;
   ESRT_GET_FW_ENTRY_BY_GUID       EsrtGetFwEntryByGuid;
} ESRT_OPERATION_PROTOCOL;

extern EFI_GUID gEsrtOperationProtocolGuid;

#endif
