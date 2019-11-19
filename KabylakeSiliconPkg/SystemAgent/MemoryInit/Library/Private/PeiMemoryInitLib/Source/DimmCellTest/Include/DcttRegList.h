/** @file
  List of registers.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 1999 - 2016 Intel Corporation.

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

@par Specification
**/
#ifndef _DCTT_REG_LIST_H_
#define _DCTT_REG_LIST_H_

// @todo: Including MRC defines until hardware abstraction
#include "McAddress.h"

//Ch1 is Ch0 + 8
#define DCTT_REUT_CH_SEQ_CFG MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG
  #define DCTT_REUT_CH_SEQ_CFG_F_SUBSEQ_END_POINTER_START              MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_Subsequence_End_Pointer_OFF
  #define DCTT_REUT_CH_SEQ_CFG_F_SUBSEQ_END_POINTER_LEN                MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_Subsequence_End_Pointer_WID
  #define DCTT_REUT_CH_SEQ_CFG_F_SUBSEQ_START_POINTER_START            MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_Subsequence_Start_Pointer_OFF
  #define DCTT_REUT_CH_SEQ_CFG_F_SUBSEQ_START_POINTER_LEN              MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_Subsequence_Start_Pointer_WID
  #define DCTT_REUT_CH_SEQ_CFG_F_INITIALIZATION_MODE_START             MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_Initialization_Mode_OFF
  #define DCTT_REUT_CH_SEQ_CFG_F_INITIALIZATION_MODE_LEN               MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_Initialization_Mode_WID
  #define DCTT_REUT_CH_SEQ_CFG_F_GLOBAL_CTRL_START                     MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_Global_Control_OFF
  #define DCTT_REUT_CH_SEQ_CFG_F_GLOBAL_CTRL_LEN                       MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_Global_Control_WID
  #define DCTT_REUT_CH_SEQ_CFG_F_ADDR_UPDATE_RATE_MODE_START           MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_Address_Update_Rate_Mode_OFF
  #define DCTT_REUT_CH_SEQ_CFG_F_ADDR_UPDATE_RATE_MODE_LEN             MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_Address_Update_Rate_Mode_WID
  #define DCTT_REUT_CH_SEQ_CFG_F_STOP_BASE_SEQUENCE_WRAP_TRIGGER_START MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_Stop_Base_Sequence_On_Wrap_Trigger_OFF
  #define DCTT_REUT_CH_SEQ_CFG_F_STOP_BASE_SEQUENCE_WRAP_TRIGGER_LEN   MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_Stop_Base_Sequence_On_Wrap_Trigger_WID
  #define DCTT_REUT_CH_SEQ_CFG_F_START_TEST_DELAY_START                MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_Start_Test_Delay_OFF
  #define DCTT_REUT_CH_SEQ_CFG_F_START_TEST_DELAY_LEN                  MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_Start_Test_Delay_WID
  #define DCTT_REUT_CH_SEQ_CFG_F_ENABLE_DUMMY_READS_START              MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_Enable_Dummy_Reads_OFF
  #define DCTT_REUT_CH_SEQ_CFG_F_ENABLE_DUMMY_READS_LEN                MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_Enable_Dummy_Reads_WID

//Ch1 is Ch0 + 4
#define DCTT_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_MCMAIN_0_REG
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_F_COL_ADDRESS_ORDER_START             MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_MCMAIN_0_Base_Column_Address_Order_OFF
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_F_COL_ADDRESS_ORDER_LEN               MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_MCMAIN_0_Base_Column_Address_Order_WID
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_F_ROW_ADDRESS_ORDER_START             MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_MCMAIN_0_Base_Row_Address_Order_OFF
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_F_ROW_ADDRESS_ORDER_LEN               MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_MCMAIN_0_Base_Row_Address_Order_WID
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_F_BANK_ADDRESS_ORDER_START            MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_MCMAIN_0_Base_Bank_Address_Order_OFF
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_F_BANK_ADDRESS_ORDER_LEN              MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_MCMAIN_0_Base_Bank_Address_Order_WID
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_F_RANK_ADDRESS_ORDER_START            MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_MCMAIN_0_Base_Rank_Address_Order_OFF
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_F_RANK_ADDRESS_ORDER_LEN              MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_MCMAIN_0_Base_Rank_Address_Order_WID
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_F_RANK_BASE_WRAP_CARRY_ENABLE_START   MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_MCMAIN_0_Rank_Base_Wrap_Carry_Enable_OFF
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_F_RANK_BASE_WRAP_CARRY_ENABLE_LEN     MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_MCMAIN_0_Rank_Base_Wrap_Carry_Enable_WID
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_F_BANK_BASE_WRAP_CARRY_ENABLE_START   MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_MCMAIN_0_Bank_Base_Wrap_Carry_Enable_OFF
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_F_BANK_BASE_WRAP_CARRY_ENABLE_LEN     MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_MCMAIN_0_Bank_Base_Wrap_Carry_Enable_WID
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_F_ROW_BASE_WRAP_CARRY_ENABLE_START    MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_MCMAIN_0_Row_Base_Wrap_Carry_Enable_OFF
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_F_ROW_BASE_WRAP_CARRY_ENABLE_LEN      MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_MCMAIN_0_Row_Base_Wrap_Carry_Enable_WID
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_F_COL_BASE_WRAP_CARRY_ENABLE_START    MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_MCMAIN_0_Column_Base_Wrap_Carry_Enable_OFF
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_F_COL_BASE_WRAP_CARRY_ENABLE_LEN      MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_MCMAIN_0_Column_Base_Wrap_Carry_Enable_WID
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_F_RANK_BASE_WRAP_TRIGGER_ENABLE_START MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_MCMAIN_0_Rank_Base_Wrap_Trigger_Enable_OFF
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_F_RANK_BASE_WRAP_TRIGGER_ENABLE_LEN   MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_MCMAIN_0_Rank_Base_Wrap_Trigger_Enable_WID
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_F_BANK_BASE_WRAP_TRIGGER_ENABLE_START MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_MCMAIN_0_Bank_Base_Wrap_Trigger_Enable_OFF
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_F_BANK_BASE_WRAP_TRIGGER_ENABLE_LEN   MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_MCMAIN_0_Bank_Base_Wrap_Trigger_Enable_WID

//Ch1 is Ch0 + 8
#define DCTT_REUT_CH_SEQ_BASE_ADDR_INC_CTL MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_0_REG
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_INC_CTL_F_COL_ADDRESS_INCREMENT_START  MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_0_Column_Base_Address_Increment_OFF
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_INC_CTL_F_COL_ADDRESS_INCREMENT_LEN    MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_0_Column_Base_Address_Increment_WID
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_INC_CTL_F_ROW_ADDRESS_INCREMENT_START  MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_0_Row_Base_Address_Increment_OFF
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_INC_CTL_F_ROW_ADDRESS_INCREMENT_LEN    MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_0_Row_Base_Address_Increment_WID
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_INC_CTL_F_BANK_ADDRESS_INCREMENT_START MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_0_Bank_Base_Address_Increment_OFF
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_INC_CTL_F_BANK_ADDRESS_INCREMENT_LEN   MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_0_Bank_Base_Address_Increment_WID
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_INC_CTL_F_RANK_ADDRESS_INCREMENT_START MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_0_Rank_Base_Address_Increment_OFF
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_INC_CTL_F_RANK_ADDRESS_INCREMENT_LEN   MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_0_Rank_Base_Address_Increment_WID
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_INC_CTL_F_COL_UPDATE_RATE_START        MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_0_Column_Base_Address_Update_Rate_OFF
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_INC_CTL_F_COL_UPDATE_RATE_LEN          MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_0_Column_Base_Address_Update_Rate_WID
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_INC_CTL_F_ROW_UPDATE_RATE_START        MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_0_Row_Base_Address_Update_Rate_OFF
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_INC_CTL_F_ROW_UPDATE_RATE_LEN          MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_0_Row_Base_Address_Update_Rate_WID
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_INC_CTL_F_BANK_UPDATE_RATE_START       MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_0_Bank_Base_Address_Update_Rate_OFF
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_INC_CTL_F_BANK_UPDATE_RATE_LEN         MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_0_Bank_Base_Address_Update_Rate_WID

//Ch1 is Ch0 + 0x400
#define DCTT_REUT_CH_PAT_WDB_CL_CTRL MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_CTRL_REG
  #define DCTT_REUT_CH_PAT_WDB_CL_CTRL_F_WDB_INCREMENT_RATE_START  MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_CTRL_WDB_Increment_Rate_OFF
  #define DCTT_REUT_CH_PAT_WDB_CL_CTRL_F_WDB_INCREMENT_RATE_LEN    MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_CTRL_WDB_Increment_Rate_WID
  #define DCTT_REUT_CH_PAT_WDB_CL_CTRL_F_WDB_INCREMENT_SCALE_START MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_CTRL_WDB_Increment_Scale_OFF
  #define DCTT_REUT_CH_PAT_WDB_CL_CTRL_F_WDB_INCREMENT_SCALE_LEN   MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_CTRL_WDB_Increment_Scale_WID
  #define DCTT_REUT_CH_PAT_WDB_CL_CTRL_F_WDB_START_POINTER_START   MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_CTRL_WDB_Start_Pointer_OFF
  #define DCTT_REUT_CH_PAT_WDB_CL_CTRL_F_WDB_START_POINTER_LEN     MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_CTRL_WDB_Start_Pointer_WID
  #define DCTT_REUT_CH_PAT_WDB_CL_CTRL_F_WDB_END_POINTER_START     MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_CTRL_WDB_End_Pointer_OFF
  #define DCTT_REUT_CH_PAT_WDB_CL_CTRL_F_WDB_END_POINTER_LEN       MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_CTRL_WDB_End_Pointer_WID

//Ch1 is Ch0 + 0x400
#define DCTT_REUT_WDB_CAPTURE_CTL MCHBAR_CH0_CR_REUT_CH_WDB_CAPTURE_CTL_REG
  #define DCTT_REUT_CH_WDB_CAPTURE_CTL_F_ENABLE_WDB_CAPTURE_START MCHBAR_CH0_CR_REUT_CH_WDB_CAPTURE_CTL_Enable_WDB_Capture_OFF
  #define DCTT_REUT_CH_WDB_CAPTURE_CTL_F_ENABLE_WDB_CAPTURE_LEN   MCHBAR_CH0_CR_REUT_CH_WDB_CAPTURE_CTL_Enable_WDB_Capture_WID
  #define DCTT_REUT_CH_WDB_CAPTURE_CTL_F_STARTING_POINTER_START   MCHBAR_CH0_CR_REUT_CH_WDB_CAPTURE_CTL_WDB_Starting_Capture_Pointer_OFF
  #define DCTT_REUT_CH_WDB_CAPTURE_CTL_F_STARTING_POINTER_LEN     MCHBAR_CH0_CR_REUT_CH_WDB_CAPTURE_CTL_WDB_Starting_Capture_Pointer_WID
  #define DCTT_REUT_CH_WDB_CAPTURE_CTL_F_ENDING_POINTER_START     MCHBAR_CH0_CR_REUT_CH_WDB_CAPTURE_CTL_WDB_Ending_Capture_Pointer_OFF
  #define DCTT_REUT_CH_WDB_CAPTURE_CTL_F_ENDING_POINTER_LEN       MCHBAR_CH0_CR_REUT_CH_WDB_CAPTURE_CTL_WDB_Ending_Capture_Pointer_WID

//Ch1 is Ch0 + 0x400
#define DCTT_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS MCHBAR_CH0_CR_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS_REG
  #define DCTT_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS_F_ECC_ERR_STATUS_START   MCHBAR_CH0_CR_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS_ECC_Error_Status_OFF
  #define DCTT_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS_F_ECC_ERR_STATUS_LEN     MCHBAR_CH0_CR_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS_ECC_Error_Status_WID
  #define DCTT_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS_F_CHUNK_ERR_STATUS_START MCHBAR_CH0_CR_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS_Chunk_Error_Status_OFF
  #define DCTT_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS_F_CHUNK_ERR_STATUS_LEN   MCHBAR_CH0_CR_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS_Chunk_Error_Status_WID
  #define DCTT_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS_F_NTH_ERROR_START        MCHBAR_CH0_CR_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS_Nth_Error_OFF
  #define DCTT_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS_F_NTH_ERROR_LEN          MCHBAR_CH0_CR_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS_Nth_Error_WID

//Ch1 is Ch0 + 0x400
#define DCTT_REUT_CH_ERR_DATA_STATUS MCHBAR_CH0_CR_REUT_CH_ERR_DATA_STATUS_REG

//Ch1 is Ch0 + 0x400
#define DCTT_REUT_CH_ERROR_Address MCHBAR_CH0_CR_REUT_CH_ERROR_ADDR_REG
  #define DCTT_REUT_CH_ERROR_Address_F_COL_ADDRESS_START  MCHBAR_CH0_CR_REUT_CH_ERROR_ADDR_Column_Address_OFF
  #define DCTT_REUT_CH_ERROR_Address_F_COL_ADDRESS_LEN    MCHBAR_CH0_CR_REUT_CH_ERROR_ADDR_Column_Address_WID
  #define DCTT_REUT_CH_ERROR_Address_F_ROW_ADDRESS_START  MCHBAR_CH0_CR_REUT_CH_ERROR_ADDR_Row_Address_OFF
  #define DCTT_REUT_CH_ERROR_Address_F_ROW_ADDRESS_LEN    MCHBAR_CH0_CR_REUT_CH_ERROR_ADDR_Row_Address_WID
  #define DCTT_REUT_CH_ERROR_Address_F_BANK_ADDRESS_START MCHBAR_CH0_CR_REUT_CH_ERROR_ADDR_Bank_Address_OFF
  #define DCTT_REUT_CH_ERROR_Address_F_BANK_ADDRESS_LEN   MCHBAR_CH0_CR_REUT_CH_ERROR_ADDR_Bank_Address_WID
  #define DCTT_REUT_CH_ERROR_Address_F_RANK_ADDRESS_START MCHBAR_CH0_CR_REUT_CH_ERROR_ADDR_Rank_Address_OFF
  #define DCTT_REUT_CH_ERROR_Address_F_RANK_ADDRESS_LEN   MCHBAR_CH0_CR_REUT_CH_ERROR_ADDR_Rank_Address_WID

#define DCTT_REUT_GLOBAL_CTRL MCDFXS_CR_REUT_GLOBAL_CTL_MCMAIN_REG
  #define DCTT_REUT_GLOBAL_CTRL_F_START_TEST_START        MCDFXS_CR_REUT_GLOBAL_CTL_MCMAIN_Global_Start_Test_OFF
  #define DCTT_REUT_GLOBAL_CTRL_F_START_TEST_LEN          MCDFXS_CR_REUT_GLOBAL_CTL_MCMAIN_Global_Start_Test_WID
  #define DCTT_REUT_GLOBAL_CTRL_F_STOP_TEST_START         MCDFXS_CR_REUT_GLOBAL_CTL_MCMAIN_Global_Stop_Test_OFF
  #define DCTT_REUT_GLOBAL_CTRL_F_STOP_TEST_LEN           MCDFXS_CR_REUT_GLOBAL_CTL_MCMAIN_Global_Stop_Test_WID
  #define DCTT_REUT_GLOBAL_CTRL_F_CLEAR_ERR_START         MCDFXS_CR_REUT_GLOBAL_CTL_MCMAIN_Global_Clear_Errors_OFF
  #define DCTT_REUT_GLOBAL_CTRL_F_CLEAR_ERR_LEN           MCDFXS_CR_REUT_GLOBAL_CTL_MCMAIN_Global_Clear_Errors_WID
  #define DCTT_REUT_GLOBAL_CTRL_F_STOP_TEST_ANY_ERR_START MCDFXS_CR_REUT_GLOBAL_CTL_MCMAIN_Global_Stop_Test_On_Any_Error_OFF
  #define DCTT_REUT_GLOBAL_CTRL_F_STOP_TEST_ANY_ERR_LEN   MCDFXS_CR_REUT_GLOBAL_CTL_MCMAIN_Global_Stop_Test_On_Any_Error_WID

#define DCTT_REUT_GLOBAL_ERR MCDFXS_CR_REUT_GLOBAL_ERR_MCMAIN_REG

//Ch1 is Ch0 + 8
#define DCTT_REUT_CH_SEQ_BASE_ADDR_START MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_START_MCMAIN_0_REG
// We want the entire address and not the cache line.
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_START_F_COL_ADDRESS_START  (0)
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_START_F_COL_ADDRESS_LEN    (11)
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_START_F_ROW_ADDRESS_START  MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_START_MCMAIN_0_Row_Address_OFF
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_START_F_ROW_ADDRESS_LEN    MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_START_MCMAIN_0_Row_Address_WID
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_START_F_BANK_ADDRESS_START MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_START_MCMAIN_0_Bank_Address_OFF
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_START_F_BANK_ADDRESS_LEN   MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_START_MCMAIN_0_Bank_Address_WID

#define DCTT_REUT_CH_SEQ_CTL MCDFXS_CR_REUT_CH_SEQ_CTL_MCMAIN_0_REG

#define DCTT_REUT_CH_SEQ_SUBSEQ_PTR_0 MCDFXS_CR_REUT_CH_SEQ_SUBSEQ_PNTR_MCMAIN_0_REG

//Ch1 is Ch0 + 0x400
#define DCTT_REUT_CH_ERR_CTL MCHBAR_CH0_CR_REUT_CH_ERR_CTL_REG
  #define DCTT_REUT_CH_ERR_CTL_STOP_NTH_ERROR_SETTING (0xffff1000)
  #define DCTT_REUT_CH_ERR_CTL_F_STOP_NTH_ERROR_START   MCHBAR_CH0_CR_REUT_CH_ERR_CTL_Stop_on_Nth_Error_OFF
  #define DCTT_REUT_CH_ERR_CTL_F_STOP_NTH_ERROR_LEN     MCHBAR_CH0_CR_REUT_CH_ERR_CTL_Stop_on_Nth_Error_WID

//Ch1 is Ch0 + 8
#define DCTT_REUT_CH_SEQ_BASE_ADDR_WRAP MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_0_REG
  // We want the entire address and not the cache line.
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_WRAP_F_COL_ADDRESS_START   (0)
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_WRAP_F_COL_ADDRESS_LEN     (12)
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_WRAP_F_ROW_ADDRESS_START   MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_0_Row_Address_OFF
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_WRAP_F_ROW_ADDRESS_LEN     MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_0_Row_Address_WID
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_WRAP_F_BANK_ADDRESS_START  MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_0_Bank_Address_OFF
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_WRAP_F_BANK_ADDRESS_LEN    MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_0_Bank_Address_WID
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_WRAP_F_RANK_ADDRESS_START  MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_0_Rank_Address_OFF
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_WRAP_F_RANK_ADDRESS_LEN    MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_0_Rank_Address_WID

#define DCTT_REUT_CH_SEQ_BASE_ADDR_WRAP_DISABLE_COL_WRAP (1 << (DCTT_REUT_CH_SEQ_BASE_ADDR_WRAP_F_COL_ADDRESS_LEN - 1))
#define DCTT_REUT_CH_SEQ_BASE_ADDR_WRAP_DISABLE_ROW_WRAP (1 << (DCTT_REUT_CH_SEQ_BASE_ADDR_WRAP_F_ROW_ADDRESS_LEN - 1))

//Ch1 is Ch0 + 0x28
//Each subsequence is equal to prev. subsequence + 4
#define DCTT_REUT_CH_SUBSEQ_CTL_0 MCDFXS_CR_REUT_CH0_SUBSEQ_CTL_MCMAIN_0_REG
  #define DCTT_REUT_CH_SUBSEQ_CTL_F_NUMBER_CACHELINES_START       MCDFXS_CR_REUT_CH0_SUBSEQ_CTL_MCMAIN_0_Number_of_Cachelines_OFF
  #define DCTT_REUT_CH_SUBSEQ_CTL_F_NUMBER_CACHELINES_LEN         MCDFXS_CR_REUT_CH0_SUBSEQ_CTL_MCMAIN_0_Number_of_Cachelines_WID
  #define DCTT_REUT_CH_SUBSEQ_CTL_F_NUMBER_CACHELINES_SCALE_START MCDFXS_CR_REUT_CH0_SUBSEQ_CTL_MCMAIN_0_Number_of_Cachelines_Scale_OFF
  #define DCTT_REUT_CH_SUBSEQ_CTL_F_NUMBER_CACHELINES_SCALE_LEN   MCDFXS_CR_REUT_CH0_SUBSEQ_CTL_MCMAIN_0_Number_of_Cachelines_Scale_WID
  #define DCTT_REUT_CH_SUBSEQ_CTL_F_SUBSEQ_WAIT_START             MCDFXS_CR_REUT_CH0_SUBSEQ_CTL_MCMAIN_0_Subsequence_Wait_OFF
  #define DCTT_REUT_CH_SUBSEQ_CTL_F_SUBSEQ_WAIT_LEN               MCDFXS_CR_REUT_CH0_SUBSEQ_CTL_MCMAIN_0_Subsequence_Wait_WID
  #define DCTT_REUT_CH_SUBSEQ_CTL_F_SUBSEQ_TYPE_START             MCDFXS_CR_REUT_CH0_SUBSEQ_CTL_MCMAIN_0_Subsequence_Type_OFF
  #define DCTT_REUT_CH_SUBSEQ_CTL_F_SUBSEQ_TYPE_LEN               MCDFXS_CR_REUT_CH0_SUBSEQ_CTL_MCMAIN_0_Subsequence_Type_WID
  #define DCTT_REUT_CH_SUBSEQ_CTL_F_DATA_ECC_ADDRESS_INV_START    MCDFXS_CR_REUT_CH0_SUBSEQ_CTL_MCMAIN_0_Data_and_ECC_Address_Inversion_OFF
  #define DCTT_REUT_CH_SUBSEQ_CTL_F_DATA_ECC_ADDRESS_INV_LEN      MCDFXS_CR_REUT_CH0_SUBSEQ_CTL_MCMAIN_0_Data_and_ECC_Address_Inversion_WID
  #define DCTT_REUT_CH_SUBSEQ_CTL_F_INV_DATA_ECC_START            MCDFXS_CR_REUT_CH0_SUBSEQ_CTL_MCMAIN_0_Invert_Data_and_ECC_OFF
  #define DCTT_REUT_CH_SUBSEQ_CTL_F_INV_DATA_ECC_LEN              MCDFXS_CR_REUT_CH0_SUBSEQ_CTL_MCMAIN_0_Invert_Data_and_ECC_WID
  #define DCTT_REUT_CH_SUBSEQ_CTL_F_STOP_BASE_TRIGGER_START       MCDFXS_CR_REUT_CH0_SUBSEQ_CTL_MCMAIN_0_Stop_Base_Subsequence_On_Wrap_Trigger_OFF
  #define DCTT_REUT_CH_SUBSEQ_CTL_F_STOP_BASE_TRIGGER_LEN         MCDFXS_CR_REUT_CH0_SUBSEQ_CTL_MCMAIN_0_Stop_Base_Subsequence_On_Wrap_Trigger_WID


//Ch1 is Ch0 + 0x28
//Each subsequence is equal to prev. subsequence + 4
#define DCTT_REUT_CH_SUBSEQ_OFFSET_CTL_0 MCDFXS_CR_REUT_CH0_SUBSEQ_OFFSET_CTL_MCMAIN_0_REG
  #define DCTT_REUT_CH_SUBSEQ_OFFSET_CTL_F_INV_DATA_ECC_START            MCDFXS_CR_REUT_CH0_SUBSEQ_OFFSET_CTL_MCMAIN_0_Base_Invert_Data_and_ECC_OFF
  #define DCTT_REUT_CH_SUBSEQ_OFFSET_CTL_F_INV_DATA_ECC_LEN              MCDFXS_CR_REUT_CH0_SUBSEQ_OFFSET_CTL_MCMAIN_0_Base_Invert_Data_and_ECC_WID
  #define DCTT_REUT_CH_SUBSEQ_OFFSET_CTL_F_ROW_INC_ENABLE_START          MCDFXS_CR_REUT_CH0_SUBSEQ_OFFSET_CTL_MCMAIN_0_Row_Increment_Enable_OFF
  #define DCTT_REUT_CH_SUBSEQ_OFFSET_CTL_F_ROW_INC_ENABLE_LEN            MCDFXS_CR_REUT_CH0_SUBSEQ_OFFSET_CTL_MCMAIN_0_Row_Increment_Enable_WID
  #define DCTT_REUT_CH_SUBSEQ_OFFSET_CTL_F_ROW_INC_ORDER_START           MCDFXS_CR_REUT_CH0_SUBSEQ_OFFSET_CTL_MCMAIN_0_Row_Increment_Order_OFF
  #define DCTT_REUT_CH_SUBSEQ_OFFSET_CTL_F_ROW_INC_ORDER_LEN             MCDFXS_CR_REUT_CH0_SUBSEQ_OFFSET_CTL_MCMAIN_0_Row_Increment_Order_WID
  #define DCTT_REUT_CH_SUBSEQ_OFFSET_CTL_F_COL_INC_ENABLE_START          MCDFXS_CR_REUT_CH0_SUBSEQ_OFFSET_CTL_MCMAIN_0_Column_Increment_Enable_OFF
  #define DCTT_REUT_CH_SUBSEQ_OFFSET_CTL_F_COL_INC_ENABLE_LEN            MCDFXS_CR_REUT_CH0_SUBSEQ_OFFSET_CTL_MCMAIN_0_Column_Increment_Enable_WID
  #define DCTT_REUT_CH_SUBSEQ_OFFSET_CTL_F_COL_INC_ORDER_START           MCDFXS_CR_REUT_CH0_SUBSEQ_OFFSET_CTL_MCMAIN_0_Column_Increment_Order_OFF
  #define DCTT_REUT_CH_SUBSEQ_OFFSET_CTL_F_COL_INC_ORDER_LEN             MCDFXS_CR_REUT_CH0_SUBSEQ_OFFSET_CTL_MCMAIN_0_Column_Increment_Order_OFF
  #define DCTT_REUT_CH_SUBSEQ_OFFSET_CTL_F_BASE_SUBSEQ_TYPE_START        MCDFXS_CR_REUT_CH0_SUBSEQ_OFFSET_CTL_MCMAIN_0_Base_Subsequence_Type_OFF
  #define DCTT_REUT_CH_SUBSEQ_OFFSET_CTL_F_BASE_SUBSEQ_TYPE_LEN          MCDFXS_CR_REUT_CH0_SUBSEQ_OFFSET_CTL_MCMAIN_0_Base_Subsequence_Type_WID
  #define DCTT_REUT_CH_SUBSEQ_OFFSET_CTL_F_OFFSET_ROW_WRAP_TRIGGER_START MCDFXS_CR_REUT_CH0_SUBSEQ_OFFSET_CTL_MCMAIN_0_Row_Offset_Wrap_Trigger_Enable_OFF
  #define DCTT_REUT_CH_SUBSEQ_OFFSET_CTL_F_OFFSET_ROW_WRAP_TRIGGER_LEN   MCDFXS_CR_REUT_CH0_SUBSEQ_OFFSET_CTL_MCMAIN_0_Row_Offset_Wrap_Trigger_Enable_WID
  #define DCTT_REUT_CH_SUBSEQ_OFFSET_CTL_F_BASE_REPEAT_RATE_START        MCDFXS_CR_REUT_CH0_SUBSEQ_OFFSET_CTL_MCMAIN_0_Base_Subsequence_Repeat_Rate_OFF
  #define DCTT_REUT_CH_SUBSEQ_OFFSET_CTL_F_BASE_REPEAT_RATE_LEN          MCDFXS_CR_REUT_CH0_SUBSEQ_OFFSET_CTL_MCMAIN_0_Base_Subsequence_Repeat_Rate_WID
  #define DCTT_REUT_CH_SUBSEQ_OFFSET_CTL_F_OFFSET_ADDR_UPDATE_RATE_START MCDFXS_CR_REUT_CH0_SUBSEQ_OFFSET_CTL_MCMAIN_0_Offset_Address_Update_Rate_OFF
  #define DCTT_REUT_CH_SUBSEQ_OFFSET_CTL_F_OFFSET_ADDR_UPDATE_RATE_LEN   MCDFXS_CR_REUT_CH0_SUBSEQ_OFFSET_CTL_MCMAIN_0_Offset_Address_Update_Rate_WID

//Ch1 is Ch0 + 4
#define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_LOWER_0 MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_LOWER_MCMAIN_0_REG
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_LOWER_F_ROW0_START MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_LOWER_MCMAIN_0_Logical_to_Physical_Row0_Swizzle_OFF
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_LOWER_F_ROW0_LEN   MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_LOWER_MCMAIN_0_Logical_to_Physical_Row0_Swizzle_WID
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_LOWER_F_ROW1_START MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_LOWER_MCMAIN_0_Logical_to_Physical_Row1_Swizzle_OFF
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_LOWER_F_ROW1_LEN   MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_LOWER_MCMAIN_0_Logical_to_Physical_Row1_Swizzle_WID
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_LOWER_F_ROW2_START MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_LOWER_MCMAIN_0_Logical_to_Physical_Row2_Swizzle_OFF
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_LOWER_F_ROW2_LEN   MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_LOWER_MCMAIN_0_Logical_to_Physical_Row2_Swizzle_WID
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_LOWER_F_ROW3_START MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_LOWER_MCMAIN_0_Logical_to_Physical_Row3_Swizzle_OFF
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_LOWER_F_ROW3_LEN   MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_LOWER_MCMAIN_0_Logical_to_Physical_Row3_Swizzle_WID
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_LOWER_F_ROW4_START MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_LOWER_MCMAIN_0_Logical_to_Physical_Row4_Swizzle_OFF
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_LOWER_F_ROW4_LEN   MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_LOWER_MCMAIN_0_Logical_to_Physical_Row4_Swizzle_WID
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_LOWER_F_ROW5_START MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_LOWER_MCMAIN_0_Logical_to_Physical_Row5_Swizzle_OFF
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_LOWER_F_ROW5_LEN   MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_LOWER_MCMAIN_0_Logical_to_Physical_Row5_Swizzle_WID

//Ch1 is Ch0 + 8
#define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_0 MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_MCMAIN_0_REG
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_F_ROW6_START  MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_MCMAIN_0_Logical_to_Physical_Row6_Swizzle_OFF
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_F_ROW6_LEN    MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_MCMAIN_0_Logical_to_Physical_Row6_Swizzle_WID
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_F_ROW7_START  MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_MCMAIN_0_Logical_to_Physical_Row7_Swizzle_OFF
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_F_ROW7_LEN    MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_MCMAIN_0_Logical_to_Physical_Row7_Swizzle_WID
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_F_ROW8_START  MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_MCMAIN_0_Logical_to_Physical_Row8_Swizzle_OFF
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_F_ROW8_LEN    MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_MCMAIN_0_Logical_to_Physical_Row8_Swizzle_WID
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_F_ROW9_START  MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_MCMAIN_0_Logical_to_Physical_Row9_Swizzle_OFF
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_F_ROW9_LEN    MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_MCMAIN_0_Logical_to_Physical_Row9_Swizzle_WID
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_F_ROW10_START MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_MCMAIN_0_Logical_to_Physical_Row10_Swizzle_OFF
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_F_ROW10_LEN   MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_MCMAIN_0_Logical_to_Physical_Row10_Swizzle_WID
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_F_ROW11_START MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_MCMAIN_0_Logical_to_Physical_Row11_Swizzle_OFF
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_F_ROW11_LEN   MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_MCMAIN_0_Logical_to_Physical_Row11_Swizzle_WID
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_F_ROW12_START MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_MCMAIN_0_Logical_to_Physical_Row12_Swizzle_OFF
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_F_ROW12_LEN   MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_MCMAIN_0_Logical_to_Physical_Row12_Swizzle_WID
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_F_ROW13_START MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_MCMAIN_0_Logical_to_Physical_Row13_Swizzle_OFF
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_F_ROW13_LEN   MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_MCMAIN_0_Logical_to_Physical_Row13_Swizzle_WID
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_F_ROW14_START MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_MCMAIN_0_Logical_to_Physical_Row14_Swizzle_OFF
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_F_ROW14_LEN   MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_MCMAIN_0_Logical_to_Physical_Row14_Swizzle_WID
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_F_ROW15_START MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_MCMAIN_0_Logical_to_Physical_Row15_Swizzle_OFF
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_F_ROW15_LEN   MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_MCMAIN_0_Logical_to_Physical_Row15_Swizzle_WID
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_F_ROW16_START MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_MCMAIN_0_Logical_to_Physical_Row16_Swizzle_OFF
  #define DCTT_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_F_ROW16_LEN   MCDFXS_CR_REUT_CH_SEQ_ROW_ADDR_SWIZZLE_UPPER_MCMAIN_0_Logical_to_Physical_Row16_Swizzle_WID

//Ch1 is Ch0 + 8
#define DCTT_REUT_CH_SEQ_OFFSET_ADDR_CURRENT_0 MCDFXS_CR_REUT_CH_SEQ_OFFSET_ADDR_CURRENT_MCMAIN_0_REG
  // We want the entire address and not the cache line.
  #define DCTT_REUT_CH_SEQ_OFFSET_ADDR_CURRENT_F_COL_ADDRESS_START (0)
  #define DCTT_REUT_CH_SEQ_OFFSET_ADDR_CURRENT_F_COL_ADDRESS_LEN   (11)
  #define DCTT_REUT_CH_SEQ_OFFSET_ADDR_CURRENT_F_ROW_ADDRESS_START MCDFXS_CR_REUT_CH_SEQ_OFFSET_ADDR_CURRENT_MCMAIN_0_Row_Address_OFF
  #define DCTT_REUT_CH_SEQ_OFFSET_ADDR_CURRENT_F_ROW_ADDRESS_LEN   MCDFXS_CR_REUT_CH_SEQ_OFFSET_ADDR_CURRENT_MCMAIN_0_Row_Address_WID

//Ch1 is Ch0 + 8
#define DCTT_REUT_CH_SEQ_BASE_ADDR_CURRENT_0 MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_CURRENT_MCMAIN_0_REG
  // We want the entire address and not the cache line.
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_CURRENT_F_COL_ADDRESS_START  (0)
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_CURRENT_F_COL_ADDRESS_LEN    (11)
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_CURRENT_F_ROW_ADDRESS_START  MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_CURRENT_MCMAIN_0_Row_Address_OFF
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_CURRENT_F_ROW_ADDRESS_LEN    MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_CURRENT_MCMAIN_0_Row_Address_WID
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_CURRENT_F_BANK_ADDRESS_START MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_CURRENT_MCMAIN_0_Bank_Address_OFF
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_CURRENT_F_BANK_ADDRESS_LEN   MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_CURRENT_MCMAIN_0_Bank_Address_WID
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_CURRENT_F_RANK_ADDRESS_START MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_CURRENT_MCMAIN_0_Rank_Address_OFF
  #define DCTT_REUT_CH_SEQ_BASE_ADDR_CURRENT_F_RANK_ADDRESS_LEN   MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_CURRENT_MCMAIN_0_Rank_Address_WID

//Ch1 is Ch0 + 4
#define DCTT_REUT_CH_SEQ_CACHELINE_STATUS_0 MCDFXS_CR_REUT_CH_SEQ_CACHELINE_STATUS_MCMAIN_0_REG

//Ch1 is Ch0 + 4
#define DCTT_REUT_CH_SEQ_DUMMYREAD_MASK_0 MCDFXS_CR_REUT_CH_SEQ_DUMMYREAD_MASK_MCMAIN_0_REG

//Ch1 is Ch0 + 4
#define DCTT_REUT_CH_SEQ_DUMMYREAD_CTL_0 MCDFXS_CR_REUT_CH_SEQ_DUMMYREAD_CTL_MCMAIN_0_REG
  #define DCTT_REUT_CH_SEQ_DUMMYREAD_CTL_F_N_COUNTER_START           MCDFXS_CR_REUT_CH_SEQ_DUMMYREAD_CTL_MCMAIN_0_N_Counter_OFF
  #define DCTT_REUT_CH_SEQ_DUMMYREAD_CTL_F_N_COUNTER_LEN             MCDFXS_CR_REUT_CH_SEQ_DUMMYREAD_CTL_MCMAIN_0_N_Counter_WID
  #define DCTT_REUT_CH_SEQ_DUMMYREAD_CTL_F_M_COUNTER_START           MCDFXS_CR_REUT_CH_SEQ_DUMMYREAD_CTL_MCMAIN_0_M_counter_OFF
  #define DCTT_REUT_CH_SEQ_DUMMYREAD_CTL_F_M_COUNTER_LEN             MCDFXS_CR_REUT_CH_SEQ_DUMMYREAD_CTL_MCMAIN_0_M_counter_WID
  #define DCTT_REUT_CH_SEQ_DUMMYREAD_CTL_F_L_COUNTER_START           MCDFXS_CR_REUT_CH_SEQ_DUMMYREAD_CTL_MCMAIN_0_L_counter_OFF
  #define DCTT_REUT_CH_SEQ_DUMMYREAD_CTL_F_L_COUNTER_LEN             MCDFXS_CR_REUT_CH_SEQ_DUMMYREAD_CTL_MCMAIN_0_L_counter_WID
  #define DCTT_REUT_CH_SEQ_DUMMYREAD_CTL_F_ENABLE_SWEEP_FREQ_START   MCDFXS_CR_REUT_CH_SEQ_DUMMYREAD_CTL_MCMAIN_0_Enable_Sweep_Frequency_OFF
  #define DCTT_REUT_CH_SEQ_DUMMYREAD_CTL_F_ENABLE_SWEEP_FREQ_LEN     MCDFXS_CR_REUT_CH_SEQ_DUMMYREAD_CTL_MCMAIN_0_Enable_Sweep_Frequency_WID
  #define DCTT_REUT_CH_SEQ_DUMMYREAD_CTL_F_L_DUMMY_READ_SELECT_START MCDFXS_CR_REUT_CH_SEQ_DUMMYREAD_CTL_MCMAIN_0_L_DummyRead_Select_OFF
  #define DCTT_REUT_CH_SEQ_DUMMYREAD_CTL_F_L_DUMMY_READ_SELECT_LEN   MCDFXS_CR_REUT_CH_SEQ_DUMMYREAD_CTL_MCMAIN_0_L_DummyRead_Select_WID

//Ch1 is Ch0 + 4
#define DCTT_REUT_CH_SEQ_SUBSEQ_POINTER_0 MCDFXS_CR_REUT_CH_SEQ_SUBSEQ_PNTR_MCMAIN_0_REG

//Ch1 is Ch0 + 4
#define DCTT_REUT_CH_SEQ_RANK_LOGICAL_TO_PHYSICAL_MAPPING_0 MCDFXS_CR_REUT_CH_SEQ_RANK_LOGICAL_TO_PHYSICAL_MAPPING_MCMAIN_0_REG
  #define DCTT_REUT_CH_SEQ_RANK_LOGICAL_TO_PHYSICAL_MAPPING_F_RANK0_START MCDFXS_CR_REUT_CH_SEQ_RANK_LOGICAL_TO_PHYSICAL_MAPPING_MCMAIN_0_Logical_to_Physical_Rank0_Mapping_OFF
  #define DCTT_REUT_CH_SEQ_RANK_LOGICAL_TO_PHYSICAL_MAPPING_F_RANK0_LEN   MCDFXS_CR_REUT_CH_SEQ_RANK_LOGICAL_TO_PHYSICAL_MAPPING_MCMAIN_0_Logical_to_Physical_Rank0_Mapping_WID
  #define DCTT_REUT_CH_SEQ_RANK_LOGICAL_TO_PHYSICAL_MAPPING_F_RANK1_START MCDFXS_CR_REUT_CH_SEQ_RANK_LOGICAL_TO_PHYSICAL_MAPPING_MCMAIN_0_Logical_to_Physical_Rank1_Mapping_OFF
  #define DCTT_REUT_CH_SEQ_RANK_LOGICAL_TO_PHYSICAL_MAPPING_F_RANK1_LEN   MCDFXS_CR_REUT_CH_SEQ_RANK_LOGICAL_TO_PHYSICAL_MAPPING_MCMAIN_0_Logical_to_Physical_Rank1_Mapping_WID

//Ch1 is Ch0 + 0x400
#define DCTT_REUT_CH_MISC_REFRESH_CTRL MCHBAR_CH0_CR_REUT_CH_MISC_REFRESH_CTRL_REG
  #define DCTT_REUT_CH_MISC_REFRESH_CTRL_F_REFRESH_RANK_MASK_START MCHBAR_CH0_CR_REUT_CH_MISC_REFRESH_CTRL_Refresh_Rank_Mask_OFF
  #define DCTT_REUT_CH_MISC_REFRESH_CTRL_F_REFRESH_RANK_MASK_LEN MCHBAR_CH0_CR_REUT_CH_MISC_REFRESH_CTRL_Refresh_Rank_Mask_WID

//Not per Channel
#define DCTT_MC_INIT_STATE_G MCDECS_CR_MC_INIT_STATE_G_MCMAIN_REG
  #define DCTT_MC_INIT_STATE_G_F_REFRESH_ENABLE_START MCDECS_CR_MC_INIT_STATE_G_MCMAIN_refresh_enable_OFF
  #define DCTT_MC_INIT_STATE_G_F_REFRESH_ENABLE_LEN MCDECS_CR_MC_INIT_STATE_G_MCMAIN_refresh_enable_WID

//Ch1 is Ch0 + 0x4
#define  DCTT_REUT_CH_SEQ_LOOPCOUNT_LIMIT MCDFXS_CR_REUT_CH_SEQ_LOOPCOUNT_LIMIT_MCMAIN_0_REG
  #define DCTT_REUT_CH_SEQ_LOOPCOUNT_LIMIT_F_LOOPCOUNT_LIMIT_START MCDFXS_CR_REUT_CH_SEQ_LOOPCOUNT_LIMIT_MCMAIN_0_Loopcount_Limit_OFF
  #define DCTT_REUT_CH_SEQ_LOOPCOUNT_LIMIT_F_LOOPCOUNT_LIMIT_LEN MCDFXS_CR_REUT_CH_SEQ_LOOPCOUNT_LIMIT_MCMAIN_0_Loopcount_Limit_WID

//Ch1 is Ch0 + 0x400
#define DCTT_CH_TC_PRE_REG MCHBAR_CH0_CR_TC_PRE_REG
#define DCTT_CH_TC_PRE_tRP_tRCD_START     MCHBAR_CH0_CR_TC_PRE_tRP_OFF
#define DCTT_CH_TC_PRE_tRP_tRCD_LEN       MCHBAR_CH0_CR_TC_PRE_tRP_WID
#define DCTT_CH_TC_PRE_tRP_tRCD_MIN       (8)
#define DCTT_CH_TC_PRE_tRP_tRCD_MAX       (63)
#define DCTT_CH_TC_PRE_tRAS_START         MCHBAR_CH0_CR_TC_PRE_tRAS_OFF
#define DCTT_CH_TC_PRE_tRAS_LEN           MCHBAR_CH0_CR_TC_PRE_tRAS_WID
#define DCTT_CH_TC_PRE_tRAS_MIN           (28)
#define DCTT_CH_TC_PRE_tRAS_MAX           (64)
#define DCTT_CH_TC_PRE_tRDPRE_START       MCHBAR_CH0_CR_TC_PRE_tRDPRE_OFF
#define DCTT_CH_TC_PRE_tRDPRE_LEN         MCHBAR_CH0_CR_TC_PRE_tRDPRE_WID
#define DCTT_CH_TC_PRE_tRDPRE_MIN         (6)
#define DCTT_CH_TC_PRE_tRDPRE_MAX         (15)
#define DCTT_CH_TC_PRE_tWRPRE_START       MCHBAR_CH0_CR_TC_PRE_tWRPRE_OFF
#define DCTT_CH_TC_PRE_tWRPRE_LEN         MCHBAR_CH0_CR_TC_PRE_tWRPRE_WID
#define DCTT_CH_TC_PRE_tWRPRE_MIN         (23)
#define DCTT_CH_TC_PRE_tWRPRE_MAX         (95)

//Ch1 is Ch0 + 0x400
#define DCTT_CH_TC_ODT_REG MCHBAR_CH0_CR_TC_ODT_REG
#define DCTT_CH_TC_ODT_tCL_START  MCHBAR_CH0_CR_TC_ODT_tCL_OFF
#define DCTT_CH_TC_ODT_tCL_LEN    MCHBAR_CH0_CR_TC_ODT_tCL_WID
#define DCTT_CH_TC_ODT_tCL_MAX    (5)
#define DCTT_CH_TC_ODT_tCL_MIN    (31)
#define DCTT_CH_TC_ODT_tCWL_START MCHBAR_CH0_CR_TC_ODT_tCWL_OFF
#define DCTT_CH_TC_ODT_tCWL_LEN   MCHBAR_CH0_CR_TC_ODT_tCWL_WID
#define DCTT_CH_TC_ODT_tCWL_MAX   (4)
#define DCTT_CH_TC_ODT_tCWL_MIN   (20)
#endif // _DCTT_REG_LIST_H_
