/** @file
  BUP Messages - HECI messages exchanged in pre-DID phase with CSME FW

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2014 - 2017 Intel Corporation.

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

  This file contains an 'Intel Peripheral Driver' and is uniquely identified as
  "Intel Reference Module" and is licensed for Intel CPUs and chipsets under
  the terms of your license agreement with Intel or your vendor. This file may
  be modified by the user, subject to additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef _BUP_MSGS_H
#define _BUP_MSGS_H

#include <MeBiosPayloadData.h>

#pragma pack(1)

///
/// BUP group IDs
///
#define BUP_COMMON_GROUP_ID 0xF0
#define BUP_ICC_GROUP_ID    0xF1
#define BUP_HSIO_GROUP_ID   0xF2
#define BUP_PM_GROUP_ID     0xF3

///
/// Defines for Command
///
///
/// Defines for BUP_COMMON_GROUP_ID Command
///
#define DRAM_INIT_DONE_CMD       0x01
#define MBP_REQUEST_CMD          0x02
#define ME_ENABLE_CMD            0x03
#define HMRFPO_DISABLE_CMD       0x04
#define MANUF_RESET_AND_HALT_CMD 0x0E
///
/// Defines for BUP_ICC_GROUP_ID Command
///
#define ICC_CMD  0x01
///
/// Defines for BUP_HSIO_GROUP_ID Command
///
#define HSIO_CMD  0x1

///
/// Defines for BUP_PM_GROUP_ID Command
///
#define NPCR_NOTIFICATION_CMD  0x1

///
/// Common group definitions
///

///
/// DRAM Init Done definitions
///
typedef union {
  struct {
    UINT32 UmaBaseAddr    :16; ///< BIT0-15
    UINT32 UmaBARsvrd     :7;  ///< BIT16-BIT22
    UINT32 FfsEntryExit   :1;  ///< BIT23
    UINT32 MemInitStatus  :4;  ///< BIT24 - BIT27
    UINT32 Reserved       :4;  ///< BIT28 - BIT31
  } Fields;
  UINT32 ul;
} DRAM_INIT_DONE_DATA;

typedef struct {
   MKHI_MESSAGE_HEADER  MkhiHeader;
   DRAM_INIT_DONE_DATA  DIDData;
} DRAM_INIT_DONE_CMD_REQ;

///
/// Memory Init Status codes
///
#define BIOS_MSG_DID_SUCCESS              0
#define BIOS_MSG_DID_NO_MEMORY            0x1
#define BIOS_MSG_DID_INIT_ERROR           0x2
#define BIOS_MSG_DID_MEM_NOT_PRESERVED    0x3

typedef struct {
   MKHI_MESSAGE_HEADER  MkhiHeader;
   UINT8                BiosAction;
   UINT8                Reserved[3];
} DRAM_INIT_DONE_CMD_RESP;

///
/// BIOS action codes
///
#define DID_ACK_NON_PCR       0x1
#define DID_ACK_PCR           0x2
#define DID_ACK_RSVD3         0x3
#define DID_ACK_RSVD4         0x4
#define DID_ACK_RSVD5         0x5
#define DID_ACK_GRST          0x6
#define DID_ACK_CONTINUE_POST 0x7

#define MAX_MBP_SIZE          0x1000

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT32              SkipMbp;
} MBP_CMD_REQ;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  MBP_HEADER          MbpHeader;
  UINT32              MbpItems[1];
} MBP_CMD_RESP;


typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} ME_ENABLE_CMD_REQ;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} ME_ENABLE_CMD_RESP;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} HMRFPO_DISABLE_CMD_REQ;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} HMRFPO_DISABLE_CMD_RESP;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} MANUF_RESET_AND_HALT_REQ;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} MANUF_RESET_AND_HALT_RESP;

typedef union {
  MANUF_RESET_AND_HALT_REQ  Request;
  MANUF_RESET_AND_HALT_RESP Response;
} MANUF_RESET_AND_HALT_BUFFER;

///
/// ICC group defines
///

typedef struct {
  UINT32 BClkRampOnWarmResetEn:1;   ///< BIT0
  UINT32 Reserved:31;               ///< BIT1-31
}ICC_MESSAGE_REQ_DATA;

typedef struct {
  UINT32 FwNeedsWarmResetFlag:1;    ///< BIT0
  UINT32 Reserved:31;               ///< BIT1-31
}ICC_MESSAGE_RESP_DATA;

typedef struct {
  MKHI_MESSAGE_HEADER  MkhiHeader;
  ICC_MESSAGE_REQ_DATA Data;
} ICC_CMD_REQ;

typedef struct {
  MKHI_MESSAGE_HEADER   MkhiHeader;
  ICC_MESSAGE_RESP_DATA Data;
} ICC_CMD_RESP;

///
/// HSIO group defines
///

///
/// BIOS HSIO request types
///
#define HSIO_CLOSE_INTERFACE_REQ    0x0
#define HSIO_REPORT_VERSION_REQ     0x1

typedef struct {
  MKHI_MESSAGE_HEADER   MkhiHeader;
} HSIO_CMD_REQ;

typedef struct {
  UINT16 Crc;
  UINT8  Version;
  UINT8  Family;
} HSIO_MESSAGE_RESP_DATA;

typedef struct {
  MKHI_MESSAGE_HEADER    MkhiHeader;
  HSIO_MESSAGE_RESP_DATA Data;
} HSIO_CMD_RESP;

typedef union {
  HSIO_CMD_REQ  Request;
  HSIO_CMD_RESP Response;
} HSIO_CMD_BUFFER;

///
/// PM group defines
///
typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} HR_NOTIFICATION_CMD;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT8               ResetRequested;
} HR_NOTIFICATION_CMD_RESP;

typedef union {
  HR_NOTIFICATION_CMD      Request;
  HR_NOTIFICATION_CMD_RESP Response;
} HR_NOTIFICATION_BUFFER;

//
// Defines the HECI request buffer format for the ICC_HSIO_WRITE_SETTINGS_CMD.
//
typedef struct {
   ICC_HEADER  Header;      // Standard ICC HECI Header
   UINT32      Reserved;    // Reserved for future use
} HSIO_WRITE_SETTINGS_REQ;

#pragma pack()

#endif // _BUP_MSGS_H

