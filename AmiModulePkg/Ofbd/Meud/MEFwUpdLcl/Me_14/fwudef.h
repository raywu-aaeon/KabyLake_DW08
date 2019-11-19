/*++

INTEL CONFIDENTIAL
Copyright 2009-2018 Intel Corporation All Rights Reserved.

The source code contained or described herein and all documents
related to the source code ("Material") are owned by Intel Corporation
or its suppliers or licensors. Title to the Material remains with
Intel Corporation or its suppliers and licensors. The Material
contains trade secrets and proprietary and confidential information of
Intel or its suppliers and licensors. The Material is protected by
worldwide copyright and trade secret laws and treaty provisions. No
part of the Material may be used, copied, reproduced, modified,
published, uploaded, posted, transmitted, distributed, or disclosed in
any way without Intel's prior express written permission.

No license under any patent, copyright, trade secret or other
intellectual property right is granted to or conferred upon you by
disclosure or delivery of the Materials, either expressly, by
implication, inducement, estoppel or otherwise. Any license under such
intellectual property rights must be express and approved by Intel in
writing.

File Name:
   fwudef.h

--*/

#ifndef _FWUDEF_H_
#define _FWUDEF_H_

typedef UINT32 uint32_t;

typedef uint32_t STATUS;

#define MAXIMUM_IPU_SUPPORTED          20

typedef struct version
{
   UINT16      Major;
   UINT16      Minor;
   UINT16      Hotfix;
   UINT16      Build;
}version;

typedef struct _PT_ATTRB
{
   uint32_t PtNameId;
   uint32_t LoadAddress;   // Load Address of the IPU
   version  FwVer;         // FW version from IUP Manifest
   uint32_t CurrentInstId; // Current Inst ID from flash, 0 indicate invalid ID 
   uint32_t CurrentUpvVer; // Upper sig 16 bits are Major Version.
   uint32_t ExpectedInstId;// Expected Inst ID that need to be updated to
   uint32_t ExpectedUpvVer;// Upper sig 16 bits are Major Version.
   uint32_t Resv[4];
}PT_ATTRB;

typedef struct _FWU_GET_IPU_PT_ATTRB_MSG_REPLY
{
   uint32_t MessageType;      // Internal FWU tool use only
   STATUS   Status;           // Internal FWU tool use only
   version  FtpFwVer;         // FW version in Fault Tolerance Partition. 
                              // This might be used for diagnostic or debug.
   uint32_t SizeoOfPtAttrib;  // Size in bytes. Simply is the sizeof (PT_ATTRB structure)
   uint32_t NumOfPartition;   // Number of partition actually return in this reply message
   PT_ATTRB PtAttribute[MAXIMUM_IPU_SUPPORTED];
   uint32_t Resv[4];
}FWU_GET_IPU_PT_ATTRB_MSG_REPLY;

typedef struct _FWU_INFO_FLAGS
{
   uint32_t RecoveryMode:2;   // 0 = No recovery; 1 = Full Recovery Mode,2 = Partial Recovery Mode
   uint32_t IpuNeeded:1;      // IPU_NEEDED bit, if set we are in IPU_NEEDED state.
   uint32_t FwInitDone:1;     // If set indicate FW is done initialized
   uint32_t FwuInProgress:1;  // If set FWU is in progress, this will be set for IFU update as well
   uint32_t SuInprogress:1;   // If set IFU Safe FW update is in progress. 
   uint32_t NewFtTestS:1;     // If set indicate that the new FT image is in Test Needed state (Stage 2 Boot)
   uint32_t SafeBootCnt:4;    // Boot count before the operation is success
   uint32_t FsbFlag:1;        // Force Safe Boot Flag, when this bit is set, we'll boot kernel only and go into recovery mode   

   //////////////////////////////////////////////////////
   // These fields below are important for FWU tool. 
   //////////////////////////////////////////////////////
   uint32_t LivePingNeeded:1; // Use for IFU only, See Below  
                                  // FWU tool needs to send Live-Ping or perform querying to confirm update successful.
                                  // With the current implementation when LivePingNeeded is set, 
                                  // Kernel had already confirmed it. No action from the tool is needed.
   uint32_t ResumeUpdateNeeded:1; // Use for IFU only, If set FWU tool needs to resend update image
   uint32_t RollbackNeededMode:2; // FWU_ROLLBACK_NONE = 0, FWU_ROLLBACK_1, FWU_ROLLBACK_2 
                                      // If not FWU_ROLLBACK_NONE, FWU tool needs to send restore_point image. 
   uint32_t ResetNeeded:2;    // When this field is set to ME_RESET_REQUIRED, FW Kernel will
                                  // perform ME_RESET after this message. No action from the tool is needed.
   uint32_t SuState:4;        // See possible values below
   uint32_t RecoveryCause:2;  // IMAGE_CODE_INVALID = 1; IMAGE_NFT_INVALID = 2;
   uint32_t Reserve:8;
}FWU_INFO_FLAGS;

typedef enum _FWU_OPERATION
{
   FWU_FULL_UPDATE_OPERATION = 0,
   FWU_IPU_UPDATE_OPERATION
}FWU_OPERATION;

typedef enum _FWU_ENVIRONMENT
{
   FWU_ENV_MANUFACTURING = 0,   // Manufacturing update
   FWU_ENV_IFU,                 // Independent Firmware update
}FWU_ENVIRONMENT;

#define FPT_PARTITION_NAME_RBEP         0x50454252 /**< "RBEP" Rom Boot Extension Partition Name */
#define FPT_PARTITION_NAME_FTPR         0x52505446 /**< "FTPR" Fault Tolerant Partition Name*/
#define FPT_PARTITION_NAME_NFTP         0x5054464E /**< "NFTP" Non-Fault Tolerant Partition Name */  
#define FPT_PARTITION_NAME_WCOD         0x444f4357 /**< "WCOD" Partition Name */
#define FPT_PARTITION_NAME_LOCL         0x4C434F4C /**< "LOCL" Partition Name */
#define FPT_PARTITION_NAME_ISHC         0x43485349 /**< "ISHC" Partition Name*/
#define FPT_PARTITION_NAME_PMCP         0x50434D50 /**< "PMCP" Partition Name*/
#define FPT_PARTITION_NAME_OEMP         0x504D454F /**< "OEMP" Partition Name*/
#define FPT_PARTITION_NAME_IUNP         0x504E5549 /**< "IUNP" Partition Name*/
#define FPT_PARTITION_NAME_IOMP         0x504D4F49 /**< "IOMP" Partition Name*/
#define FPT_PARTITION_NAME_MGPP         0x5050474D /**< "MGPP" Partition Name*/
#define FPT_PARTITION_NAME_TBTP         0x50544254 /**< "TBTP" Partition Name*/
#define FPT_PARTITION_NAME_DPHY         0x59485044 /**< "DPHY" Partition Name */
#endif
