/** @file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2016 Intel Corporation.

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

  This file contains a 'Sample Driver' and is licensed as such under the terms
  of your license agreement with Intel or your vendor. This file may be modified
  by the user, subject to the additional terms of the license agreement.

@par Specification Reference:
**/

#define GMT_MIPI_BRIDGE_I2C_ADDRESS  0x68

/**
 Mipi display initialization script
 Based on eDP2DSI_1536x2048_60f_eDP162G4_P206M_D800M8_R25M_Setup_150914_01 -  20151021.xml
**/

static UINT8 GmtMipiBridgeI2cScript_Config[] = {
  //IO Voltahge Setting -->
  BLOCK_COMMAND_I2C_WRITE, 6, 0x08, 0x00, 0x01, 0x00, 0x00, 0x00,
  //Boot Settings -->
  BLOCK_COMMAND_I2C_WRITE, 6, 0x10, 0x00, 0xA8, 0x61, 0x00, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 6, 0x10, 0x04, 0x0D, 0x05, 0x04, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 6, 0x10, 0x08, 0x0B, 0x00, 0x3E, 0x03,
  BLOCK_COMMAND_I2C_WRITE, 6, 0x10, 0x0C, 0x16, 0x03, 0x00, 0x03,
  BLOCK_COMMAND_I2C_WRITE, 6, 0x10, 0x10, 0x20, 0x00, 0x3C, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 6, 0x10, 0x14, 0x03, 0x00, 0x00, 0x00,
  BLOCK_COMMAND_WAIT,  1,
  BLOCK_COMMAND_I2C_WRITE, 2, 0x10, 0x18,
  BLOCK_COMMAND_I2C_READ,  4,
  //<!-- Additional Setting for eDP -->
  BLOCK_COMMAND_I2C_WRITE, 3, 0x80, 0x03, 0x41,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB4, 0x00, 0x0D,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB6, 0x32, 0x12,
  //<!-- DPRX CAD Register Setting -->
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x8E, 0xFF,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x8F, 0xFF,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x9A, 0xFF,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x9B, 0xFF,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x00, 0x0E,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xBB, 0x26, 0x02,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xBB, 0x01, 0x20,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0xC0, 0xF1,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0xC1, 0xF1,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0xC2, 0xF0,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0xC3, 0xF0,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0xC4, 0xF0,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0xC5, 0xF0,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0xC6, 0xF0,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0xC7, 0xF0,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x0B, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x33, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x5B, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x10, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x38, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x60, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x15, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x3D, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x65, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x1A, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x42, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x6A, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x1F, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x47, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x6F, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x24, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x4C, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x74, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x29, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x51, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x79, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x2E, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x56, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x7E, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xBB, 0x90, 0x10,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xBB, 0x91, 0x0F,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xBB, 0x92, 0xF6,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xBB, 0x93, 0x10,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xBB, 0x94, 0x0F,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xBB, 0x95, 0xF6,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xBB, 0x96, 0x10,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xBB, 0x97, 0x0F,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xBB, 0x98, 0xF6,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xBB, 0x99, 0x10,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xBB, 0x9A, 0x0F,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xBB, 0x9B, 0xF6,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x8A, 0x03,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x96, 0x03,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xBB, 0xD1, 0x07,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xBB, 0xB0, 0x07,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x8B, 0x04,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x8C, 0x45,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x8D, 0x05,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x97, 0x04,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x98, 0xE0,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x99, 0x2E,
  BLOCK_COMMAND_I2C_WRITE, 3, 0x80, 0x0E, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x30, 0xA4,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x31, 0x09,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x35, 0xA4,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x36, 0x0C,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x3F, 0x84,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x40, 0x12,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x44, 0xA0,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x45, 0x09,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x49, 0xA0,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x4A, 0x0C,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x53, 0x80,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x54, 0x12,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xBB, 0xC5, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xBB, 0xCB, 0x09,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xBB, 0xC0, 0xDD,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xBB, 0xC1, 0xCD,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xBB, 0xC2, 0x70,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xBB, 0xC5, 0x04,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xBB, 0xCB, 0x0C,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xBB, 0xC0, 0xA6,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xBB, 0xC1, 0xD3,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xBB, 0xC2, 0x70,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xBB, 0xC5, 0x0C,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xBB, 0xCB, 0x12,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xBB, 0xC0, 0x6E,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xBB, 0xC1, 0xCD,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xBB, 0xC2, 0x70,
  BLOCK_COMMAND_I2C_WRITE, 6, 0x10, 0x14, 0x07, 0x00, 0x00, 0x00,
  BLOCK_COMMAND_WAIT,  1,
  BLOCK_COMMAND_I2C_WRITE, 2, 0x10, 0x18,
  BLOCK_COMMAND_I2C_READ,  4,
  //<!-- eDP Settings for Link Training -->
  BLOCK_COMMAND_I2C_WRITE, 2, 0xB6, 0x31,
  BLOCK_COMMAND_I2C_READ,  1,
  //<!-- Check if 0xB631<bit1:0>=01b. -->
  //<!-- If not wait until 0xB631<bit1:0>=01b -->
  BLOCK_COMMAND_I2C_WRITE, 3, 0x80, 0x01, 0x06,
  BLOCK_COMMAND_I2C_WRITE, 3, 0x80, 0x02, 0x04,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB6, 0x08, 0x0B,
  BLOCK_COMMAND_I2C_WRITE, 3, 0xB8, 0x00, 0x1E,
  BLOCK_COMMAND_I2C_WRITE, 3, 0x07, 0x00, 0x00
};

/**
 Grizzly Mtn Mipi display on
 Based on eDP2DSI_1536x2048_60f_eDP162G4_P206M_D800M8_R25M_DispOn_150914_01 - 20151021.xml
**/

static UINT8 GmtMipiBridgeI2cScript_DispOn[] = {
  //<!-- After Link Training finishes -->
  //<!-- Check Link Training Status -->
  BLOCK_COMMAND_I2C_WRITE, 2, 0x82, 0x02,
  BLOCK_COMMAND_I2C_READ, 1,
  //<!-- Check if 0x8202 is expected value. -->
  //<!-- If not, Link Training has problem. -->
  BLOCK_COMMAND_I2C_WRITE, 2, 0x82, 0x03,
  BLOCK_COMMAND_I2C_READ, 1,
  //<!-- Check if 0x8203 is expected value. -->
  //<!-- If not, Link Training has problem. -->
  BLOCK_COMMAND_I2C_WRITE, 2, 0x82, 0x04,
  BLOCK_COMMAND_I2C_READ, 1,
  //<!-- Check if 0x8204 is expected value. -->
  //<!-- If not, Link Training has problem. -->

  //<!-- 20151021 Nobu modfied TLPX to 65nsec -->
  BLOCK_COMMAND_I2C_WRITE, 6, 0x41, 0x54, 0x06, 0x00, 0x0D, 0x00,

  BLOCK_COMMAND_I2C_WRITE, 6, 0x41, 0x60, 0x07, 0x00, 0x0D, 0x00,
  //<!-- DSI Start -->
  BLOCK_COMMAND_I2C_WRITE, 6, 0x40, 0x7C, 0x81, 0x00, 0x00, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 6, 0x40, 0x1C, 0x01, 0x00, 0x00, 0x00,
  BLOCK_COMMAND_WAIT, 1,
  BLOCK_COMMAND_I2C_WRITE, 2, 0x40, 0x60,
  BLOCK_COMMAND_I2C_READ, 4,
  //<!-- Check if 0x2060/4060<bit1:0>=11b. -->
  //<!-- If not wait until 0x2060/4060<bit1:0>=11b. -->
  //<!-- GPIO setting for LCD control.  (Depends on LCD specification and System configuration) -->
  BLOCK_COMMAND_WAIT, 1,
  BLOCK_COMMAND_I2C_WRITE, 6, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 6, 0x00, 0x80, 0x0F, 0x00, 0x00, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 6, 0x00, 0x84, 0x00, 0x00, 0x00, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 6, 0x00, 0x84, 0x0F, 0x00, 0x00, 0x00,
  BLOCK_COMMAND_WAIT, 1,
  //<!-- DSI Hs Clock Mode -->

  //<!-- 20151021 Nobu modified for non-continuous CLK mode -->
  BLOCK_COMMAND_I2C_WRITE, 6, 0x40, 0x50, 0x00, 0x00, 0x00, 0x00,

  BLOCK_COMMAND_WAIT, 100,

  BLOCK_COMMAND_I2C_WRITE, 6, 0x2A, 0x10, 0x10, 0x00, 0x04, 0x80,
  BLOCK_COMMAND_I2C_WRITE, 6, 0x3A, 0x10, 0x10, 0x00, 0x04, 0x80,
  BLOCK_COMMAND_I2C_WRITE, 6, 0x2A, 0x04, 0x01, 0x00, 0x00, 0x00,
  BLOCK_COMMAND_I2C_WRITE, 6, 0x3A, 0x04, 0x01, 0x00, 0x00, 0x00,
  //<!-- Check if eDP video is coming -->
  BLOCK_COMMAND_I2C_WRITE, 6, 0x01, 0x54, 0x01, 0x00, 0x00, 0x00,
  //<!-- Command Transmission After Video Start. (Depends on LCD specification) -->
  BLOCK_COMMAND_WAIT, 20,

  //<!-- Sleep Out -->
  BLOCK_COMMAND_I2C_WRITE, 6, 0x22, 0xFC, 0x05, 0x11, 0x00, 0x80,
  BLOCK_COMMAND_WAIT, 1,
  BLOCK_COMMAND_I2C_WRITE, 2, 0x22, 0x00,
  BLOCK_COMMAND_I2C_READ, 4,
  //<!-- Check if <bit0>=1 -->
  //<!-- If not, wait until <bit0>=1 -->

  BLOCK_COMMAND_WAIT, 120,

  //<!-- Interface setting -->
  BLOCK_COMMAND_I2C_WRITE, 6, 0x22, 0xFC, 0x29, 0x03, 0x00, 0x82,
  BLOCK_COMMAND_I2C_WRITE, 6, 0x23, 0x00, 0xF0, 0x5A, 0x5A, 0x00,
  BLOCK_COMMAND_WAIT, 16,
  BLOCK_COMMAND_I2C_WRITE, 2, 0x22, 0x00,
  BLOCK_COMMAND_I2C_READ, 4,
  //<!-- Check if <bit0>=1 -->
  //<!-- If not, wait until <bit0>=1 -->

  //<!-- Interface setting -->
  BLOCK_COMMAND_I2C_WRITE, 6, 0x22, 0xFC, 0x23, 0xF2, 0x63, 0x80,
  BLOCK_COMMAND_WAIT, 16,
  BLOCK_COMMAND_I2C_WRITE, 2, 0x22, 0x00,
  BLOCK_COMMAND_I2C_READ, 4,
  //<!-- Check if <bit0>=1 -->
  //<!-- If not, wait until <bit0>=1 -->

  //<!-- Interface setting -->
  BLOCK_COMMAND_I2C_WRITE, 6, 0x22, 0xFC, 0x29, 0x03, 0x00, 0x82,
  BLOCK_COMMAND_I2C_WRITE, 6, 0x23, 0x00, 0xF2, 0x63, 0x54, 0x00,
  BLOCK_COMMAND_WAIT, 16,
  BLOCK_COMMAND_I2C_WRITE, 2, 0x22, 0x00,
  BLOCK_COMMAND_I2C_READ, 4,
  //<!-- Check if <bit0>=1 -->
  //<!-- If not, wait until <bit0>=1 -->

  //<!-- Interface setting -->
  BLOCK_COMMAND_I2C_WRITE, 6, 0x22, 0xFC, 0x29, 0x03, 0x00, 0x80,
  BLOCK_COMMAND_I2C_WRITE, 6, 0x23, 0x00, 0xF0, 0xA5, 0xA5, 0x00,
  BLOCK_COMMAND_WAIT, 16,
  BLOCK_COMMAND_I2C_WRITE, 2, 0x22, 0x00,
  BLOCK_COMMAND_I2C_READ, 4,
  //<!-- Check if <bit0>=1 -->

  //<!-- Pixel setting 1 -->
  BLOCK_COMMAND_I2C_WRITE, 6, 0x22, 0xFC, 0x29, 0x03, 0x00, 0x82,
  BLOCK_COMMAND_I2C_WRITE, 6, 0x23, 0x00, 0xF0, 0x5A, 0x5A, 0x00,
  BLOCK_COMMAND_WAIT, 1,
  BLOCK_COMMAND_I2C_WRITE, 2, 0x22, 0x00,
  BLOCK_COMMAND_I2C_READ, 4,
  //<!-- Check if <bit0>=1 -->
  //<!-- If not, wait until <bit0>=1 -->

  //<!-- Pixel setting 1 -->
  BLOCK_COMMAND_I2C_WRITE, 6, 0x22, 0xFC, 0x29, 0x05, 0x00, 0x82,
  BLOCK_COMMAND_I2C_WRITE, 6, 0x23, 0x00, 0xC0, 0x00, 0x0F, 0xD8,
  BLOCK_COMMAND_I2C_WRITE, 6, 0x23, 0x04, 0xD8, 0x00, 0x00, 0x00,
  BLOCK_COMMAND_WAIT, 1,
  BLOCK_COMMAND_I2C_WRITE, 2, 0x22, 0x00,
  BLOCK_COMMAND_I2C_READ, 4,
  //<!-- Check if <bit0>=1 -->
  //<!-- If not, wait until <bit0>=1 -->

  //<!-- Pixel setting 1 -->
  BLOCK_COMMAND_I2C_WRITE, 6, 0x22, 0xFC, 0x29, 0x03, 0x00, 0x82,
  BLOCK_COMMAND_I2C_WRITE, 6, 0x23, 0x00, 0xF0, 0xA5, 0xA5, 0x00,
  BLOCK_COMMAND_WAIT, 1,
  BLOCK_COMMAND_I2C_WRITE, 2, 0x22, 0x00,
  BLOCK_COMMAND_I2C_READ, 4,
  //<!-- Check if <bit0>=1 -->
  //<!-- If not, wait until <bit0>=1 -->

  //<!-- Pixel setting 2 -->
  BLOCK_COMMAND_I2C_WRITE, 6, 0x22, 0xFC, 0x29, 0x03, 0x00, 0x82,
  BLOCK_COMMAND_I2C_WRITE, 6, 0x23, 0x00, 0xF0, 0x5A, 0x5A, 0x00,
  BLOCK_COMMAND_WAIT, 1,
  BLOCK_COMMAND_I2C_WRITE, 2, 0x22, 0x00,
  BLOCK_COMMAND_I2C_READ, 4,
  //<!-- Check if <bit0>=1 -->
  //<!-- If not, wait until <bit0>=1 -->

  //<!-- Pixel setting 2 -->
  BLOCK_COMMAND_I2C_WRITE, 6, 0x22, 0xFC, 0x29, 0x03, 0x00, 0x82,
  BLOCK_COMMAND_I2C_WRITE, 6, 0x23, 0x00, 0xFC, 0x5A, 0x5A, 0x00,
  BLOCK_COMMAND_WAIT, 1,
  BLOCK_COMMAND_I2C_WRITE, 2, 0x22, 0x00,
  BLOCK_COMMAND_I2C_READ, 4,
  //<!-- Check if <bit0>=1 -->
  //<!-- If not, wait until <bit0>=1 -->

  //<!-- Pixel setting 2 -->
  BLOCK_COMMAND_I2C_WRITE, 6, 0x22, 0xFC, 0x23, 0xB0, 0x20, 0x80,
  BLOCK_COMMAND_WAIT, 1,
  BLOCK_COMMAND_I2C_WRITE, 2, 0x22, 0x00,
  BLOCK_COMMAND_I2C_READ, 4,
  //<!-- Check if <bit0>=1 -->
  //<!-- If not, wait until <bit0>=1 -->

  //<!-- Pixel setting 2 -->
  BLOCK_COMMAND_I2C_WRITE, 6, 0x22, 0xFC, 0x23, 0xFE, 0x08, 0x80,
  BLOCK_COMMAND_WAIT, 1,
  BLOCK_COMMAND_I2C_WRITE, 2, 0x22, 0x00,
  BLOCK_COMMAND_I2C_READ, 4,
  //<!-- Check if <bit0>=1 -->
  //<!-- If not, wait until <bit0>=1 -->

  //<!-- Pixel setting 2 -->
  BLOCK_COMMAND_I2C_WRITE, 6, 0x22, 0xFC, 0x29, 0x03, 0x00, 0x82,
  BLOCK_COMMAND_I2C_WRITE, 6, 0x23, 0x00, 0xF0, 0xA5, 0xA5, 0x00,
  BLOCK_COMMAND_WAIT, 1,
  BLOCK_COMMAND_I2C_WRITE, 2, 0x22, 0x00,
  BLOCK_COMMAND_I2C_READ, 4,
  //<!-- Check if <bit0>=1 -->
  //<!-- If not, wait until <bit0>=1 -->

  //<!-- Pixel setting 2 -->
  BLOCK_COMMAND_I2C_WRITE, 6, 0x22, 0xFC, 0x29, 0x03, 0x00, 0x82,
  BLOCK_COMMAND_I2C_WRITE, 6, 0x23, 0x00, 0xFC, 0xA5, 0xA5, 0x00,
  BLOCK_COMMAND_WAIT, 1,
  BLOCK_COMMAND_I2C_WRITE, 2, 0x22, 0x00,
  BLOCK_COMMAND_I2C_READ, 4,
  //<!-- Check if <bit0>=1 -->
  //<!-- If not, wait until <bit0>=1 -->

  //<!-- Memory Address Set -->
  BLOCK_COMMAND_I2C_WRITE, 6, 0x22, 0xFC, 0x29, 0x03, 0x00, 0x82,
  BLOCK_COMMAND_I2C_WRITE, 6, 0x23, 0x00, 0xF0, 0x5A, 0x5A, 0x00,
  BLOCK_COMMAND_WAIT, 1,
  BLOCK_COMMAND_I2C_WRITE, 2, 0x22, 0x00,
  BLOCK_COMMAND_I2C_READ, 4,
  //<!-- Check if <bit0>=1 -->
  //<!-- If not, wait until <bit0>=1 -->

  //<!-- Memory Address Set -->
  BLOCK_COMMAND_I2C_WRITE, 6, 0x22, 0xFC, 0x39, 0x05, 0x00, 0x82,
  BLOCK_COMMAND_I2C_WRITE, 6, 0x23, 0x00, 0x2A, 0x00, 0x00, 0x05,
  BLOCK_COMMAND_I2C_WRITE, 6, 0x23, 0x04, 0xFF, 0x00, 0x00, 0x00,
  BLOCK_COMMAND_WAIT, 1,
  BLOCK_COMMAND_I2C_WRITE, 2, 0x22, 0x00,
  BLOCK_COMMAND_I2C_READ, 4,
  //<!-- Check if <bit0>=1 -->
  //<!-- If not, wait until <bit0>=1 -->

  //<!-- Memory Address Set -->
  BLOCK_COMMAND_I2C_WRITE, 6, 0x22, 0xFC, 0x39, 0x05, 0x00, 0x82,
  BLOCK_COMMAND_I2C_WRITE, 6, 0x23, 0x00, 0x2B, 0x00, 0x00, 0x07,
  BLOCK_COMMAND_I2C_WRITE, 6, 0x23, 0x04, 0xFF, 0x00, 0x00, 0x00,
  BLOCK_COMMAND_WAIT, 1,
  BLOCK_COMMAND_I2C_WRITE, 2, 0x22, 0x00,
  BLOCK_COMMAND_I2C_READ, 4,
  //<!-- Check if <bit0>=1 -->
  //<!-- If not, wait until <bit0>=1 -->

  //<!-- Memory Address Set -->
  BLOCK_COMMAND_I2C_WRITE, 6, 0x22, 0xFC, 0x29, 0x03, 0x00, 0x82,
  BLOCK_COMMAND_I2C_WRITE, 6, 0x23, 0x00, 0xF0, 0xA5, 0xA5, 0x00,
  BLOCK_COMMAND_WAIT, 1,
  BLOCK_COMMAND_I2C_WRITE, 2, 0x22, 0x00,
  BLOCK_COMMAND_I2C_READ, 4,
  //<!-- Check if <bit0>=1 -->
  //<!-- If not, wait until <bit0>=1 -->

  //<!-- Brightness setting table setting -->
  BLOCK_COMMAND_I2C_WRITE, 6, 0x22, 0xFC, 0x23, 0x53, 0x20, 0x80,
  BLOCK_COMMAND_WAIT, 1,
  BLOCK_COMMAND_I2C_WRITE, 2, 0x22, 0x00,
  BLOCK_COMMAND_I2C_READ, 4,
  //<!-- Check if <bit0>=1 -->
  //<!-- If not, wait until <bit0>=1 -->

  //<!-- Brightness setting table setting -->
  BLOCK_COMMAND_I2C_WRITE, 6, 0x22, 0xFC, 0x23, 0x51, 0xFF, 0x80,
  BLOCK_COMMAND_WAIT, 1,
  BLOCK_COMMAND_I2C_WRITE, 2, 0x22, 0x00,
  BLOCK_COMMAND_I2C_READ, 4,
  //<!-- Check if <bit0>=1 -->
  //<!-- If not, wait until <bit0>=1 -->

  //<!-- Brightness setting 2 -->
  BLOCK_COMMAND_I2C_WRITE, 6, 0x22, 0xFC, 0x23, 0x55, 0x00, 0x80,
  BLOCK_COMMAND_WAIT, 1,
  BLOCK_COMMAND_I2C_WRITE, 2, 0x22, 0x00,
  BLOCK_COMMAND_I2C_READ, 4,
  //<!-- Check if <bit0>=1 -->

  //<!-- Set Display On -->
  BLOCK_COMMAND_I2C_WRITE, 6, 0x22, 0xFC, 0x05, 0x29, 0x00, 0x80,
  BLOCK_COMMAND_WAIT, 35,
  BLOCK_COMMAND_I2C_WRITE, 2, 0x22, 0x00,
  BLOCK_COMMAND_I2C_READ, 4
  //<!-- Check if <bit0>=1 -->
  //<!-- If not, wait until <bit0>=1 -->
};