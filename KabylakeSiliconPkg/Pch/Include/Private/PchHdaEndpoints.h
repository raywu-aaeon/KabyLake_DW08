/** @file
  Header file for PchHdaLib Endpoint descriptors.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2014 - 2016 Intel Corporation.

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

#ifndef _PCH_HDA_ENDPOINTS_H_
#define _PCH_HDA_ENDPOINTS_H_

#include <Private/Library/DxePchHdaNhlt.h>

typedef enum {
  HdaDmicX1       = 0,
  HdaDmicX2,
  HdaDmicX4,
  HdaBtRender,
  HdaBtCapture,
  HdaI2sRender1,
  HdaI2sRender2,
  HdaI2sCapture,
  HdaEndpointMax
} NHLT_ENDPOINT;

typedef struct {
  NHLT_ENDPOINT EndpointType;
  UINT32        EndpointFormatsBitmask;
  BOOLEAN       Enable;
} PCH_HDA_NHLT_ENDPOINTS;

#define PCH_HDA_NHLT_TABLE_SIZE 0x2000

// Format bitmask
#define B_HDA_DMIC_2CH_48KHZ_16BIT_FORMAT                BIT0
#define B_HDA_DMIC_2CH_48KHZ_32BIT_FORMAT                BIT1
#define B_HDA_DMIC_4CH_48KHZ_16BIT_FORMAT                BIT2
#define B_HDA_DMIC_4CH_48KHZ_32BIT_FORMAT                BIT3
#define B_HDA_DMIC_1CH_48KHZ_16BIT_FORMAT                BIT4
#define B_HDA_BT_NARROWBAND_FORMAT                       BIT5
#define B_HDA_BT_WIDEBAND_FORMAT                         BIT6
#define B_HDA_BT_A2DP_FORMAT                             BIT7
#define B_HDA_I2S_RTK298_RENDER_4CH_48KHZ_24BIT_FORMAT   BIT8
#define B_HDA_I2S_RTK298_CAPTURE_4CH_48KHZ_24BIT_FORMAT  BIT9
#define B_HDA_I2S_RTK286S_2CH_48KHZ_24BIT_FORMAT         BIT10
#define B_HDA_I2S_ADI_4CH_48KHZ_32BIT_FORMAT             BIT11
#define B_HDA_I2S_48KHZ_24BIT_MASTER_FORMAT              BIT12
#define B_HDA_I2S_48KHZ_24BIT_SLAVE_FORMAT               BIT13
#define B_HDA_I2S_PCM_8KHZ_16BIT_MASTER_FORMAT           BIT14
#define B_HDA_I2S_PCM_8KHZ_16BIT_SLAVE_FORMAT            BIT15
#define V_HDA_FORMAT_MAX                                 16

// Formats
extern CONST WAVEFORMATEXTENSIBLE Ch1_48kHz16bitFormat;
extern CONST WAVEFORMATEXTENSIBLE Ch2_48kHz16bitFormat;
extern CONST WAVEFORMATEXTENSIBLE Ch2_48kHz24bitFormat;
extern CONST WAVEFORMATEXTENSIBLE Ch2_48kHz32bitFormat;
extern CONST WAVEFORMATEXTENSIBLE Ch4_48kHz16bitFormat;
extern CONST WAVEFORMATEXTENSIBLE Ch4_48kHz32bitFormat;
extern CONST WAVEFORMATEXTENSIBLE NarrowbandFormat;
extern CONST WAVEFORMATEXTENSIBLE WidebandFormat;
extern CONST WAVEFORMATEXTENSIBLE A2dpFormat;

// Format Config
extern CONST UINT32 DmicStereo16BitFormatConfig[];
extern CONST UINT32 DmicStereo16BitFormatConfigSize;
extern CONST UINT32 DmicStereo32BitFormatConfig[];
extern CONST UINT32 DmicStereo32BitFormatConfigSize;
extern CONST UINT32 DmicQuad16BitFormatConfig[];
extern CONST UINT32 DmicQuad16BitFormatConfigSize;
extern CONST UINT32 DmicQuad32BitFormatConfig[];
extern CONST UINT32 DmicQuad32BitFormatConfigSize;
extern CONST UINT32 DmicMono16BitFormatConfig[];
extern CONST UINT32 DmicMono16BitFormatConfigSize;
extern CONST UINT32 I2sRtk298Render4ch48kHz24bitFormatConfig[];
extern CONST UINT32 I2sRtk298Render4ch48kHz24bitFormatConfigSize;
extern CONST UINT32 I2sRtk298Capture4ch48kHz24bitFormatConfig[];
extern CONST UINT32 I2sRtk298Capture4ch48kHz24bitFormatConfigSize;
extern CONST UINT32 I2sRtk286S2ch48kHzFormatConfig[];
extern CONST UINT32 I2sRtk286S2ch48kHzFormatConfigSize;
extern CONST UINT32 I2sAdi4ch48kHzFormatConfig[];
extern CONST UINT32 I2sAdi4ch48kHzFormatConfigSize;
extern CONST UINT32 I2s48kHz24bitMasterFormatConfig[];
extern CONST UINT32 I2s48kHz24bitMasterFormatConfigSize;
extern CONST UINT32 I2s48kHz24bitSlaveFormatConfig[];
extern CONST UINT32 I2s48kHz24bitSlaveFormatConfigSize;
extern CONST UINT32 I2sPcm8kHz16bitMasterFormatConfig[];
extern CONST UINT32 I2sPcm8kHz16bitMasterFormatConfigSize;
extern CONST UINT32 I2sPcm8kHz16bitSlaveFormatConfig[];
extern CONST UINT32 I2sPcm8kHz16bitSlaveFormatConfigSize;
extern CONST UINT32 BtFormatConfig[];
extern CONST UINT32 BtFormatConfigSize;

// Endpoints
extern ENDPOINT_DESCRIPTOR  HdaEndpointDmicX1;
extern ENDPOINT_DESCRIPTOR  HdaEndpointDmicX2;
extern ENDPOINT_DESCRIPTOR  HdaEndpointDmicX4;
extern ENDPOINT_DESCRIPTOR  HdaEndpointBtRender;
extern ENDPOINT_DESCRIPTOR  HdaEndpointBtCapture;
extern ENDPOINT_DESCRIPTOR  HdaEndpointI2sRender;
extern ENDPOINT_DESCRIPTOR  HdaEndpointI2sCapture;

// Endpoint Config
extern CONST UINT8  DmicX1Config[];
extern CONST UINT32 DmicX1ConfigSize;
extern CONST UINT8  DmicX2Config[];
extern CONST UINT32 DmicX2ConfigSize;
extern CONST UINT8  DmicX4Config[];
extern CONST UINT32 DmicX4ConfigSize;
extern CONST UINT8  BtConfig[];
extern CONST UINT32 BtConfigSize;
extern CONST UINT8  I2sRender1Config[];
extern CONST UINT32 I2sRender1ConfigSize;
extern CONST UINT8  I2sRender2Config[];
extern CONST UINT32 I2sRender2ConfigSize;
extern CONST UINT8  I2sCaptureConfig[];
extern CONST UINT32 I2sCaptureConfigSize;

// Oed Configuration
extern CONST UINT32 NhltConfiguration[];
extern CONST UINT32 NhltConfigurationSize;

#endif // _PCH_HDA_ENDPOINTS_H_
