/** @file
  This library class provides CMOS access functions.

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2010 - 2016 Intel Corporation.

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

#ifndef _CMOS_ACCESS_LIB_H_
#define _CMOS_ACCESS_LIB_H_

/**
  Returns the value from a CMOS location. 

  If the passed address is beyond the max address return RETURN_NOT_FOUND.
  If the function completed successfully return RETURN_SUCCESS
  
  @param[in]  CmosAddress  Location to read from CMOS
  @param[out] Data         The Value read from CMOS location

  @retval     Status
**/
RETURN_STATUS
EFIAPI
ReadCmos (
  IN  UINT8 CmosAddress,
  OUT UINT8 *Data
  );


/**
  Write the value from a CMOS location.

  If the passed address is beyond the max address return RETURN_NOT_FOUND.
  If the function completed successfully return RETURN_SUCCESS
  
  @param[in] CmosAddress  Location to write to CMOS
  @param[in] Data         Value to be written to the CMOS

  @retval    Status
**/
RETURN_STATUS
EFIAPI
WriteCmos (
  IN UINT8 CmosAddress,
  IN UINT8 *Data
  );


/**
  Returns the 1 Byte value from a CMOS location. 

  If the passed address is beyond the max address return 0.

  @param[in]  CmosAddress  Location to read from CMOS

  @retval     Data The Value read from CMOS location
**/
UINT8
EFIAPI
ReadCmos8 (
  IN  UINT8  CmosAddress
  );

/**
  Write the 1 Byte value from a CMOS location.

  If the passed address is beyond the max address return RETURN_NOT_FOUND.
  If the function completed successfully return RETURN_SUCCESS

  @param[in]  CmosAddress  Location to write to CMOS
  @param[in]  Data         Value to be written to the CMOS

  @retval     Status
**/
RETURN_STATUS
EFIAPI
WriteCmos8 (
  IN UINT8  CmosAddress,
  IN UINT8  Data
  );

/**
  Returns the 2 Bytes value from a CMOS location. 

  If the passed address is beyond the max address return 0.

  @param[in]  CmosAddress  Location to read from CMOS

  @retval     Data         The Value read from CMOS location
**/
UINT16
EFIAPI
ReadCmos16 (
  IN  UINT8  CmosAddress
  );

/**
  Write the 2 Bytes value from a CMOS location.

  If the passed address is beyond the max address return RETURN_NOT_FOUND.
  If the function completed successfully return RETURN_SUCCESS
  
  @param[in]  CmosAddress  Location to write to CMOS
  @param[in]  Data         Value to be written to the CMOS

  @retval     Status
**/
RETURN_STATUS
EFIAPI
WriteCmos16 (
  IN UINT8  CmosAddress,
  IN UINT16 Data
  );

/**
  Returns the 4 Bytes value from a CMOS location. 

  If the passed address is beyond the max address return 0.
  
  @param[in]  CmosAddress  Location to read from CMOS
  @retval     Data         The Value read from CMOS location
**/
UINT32
EFIAPI
ReadCmos32 (
  IN  UINT8  CmosAddress
  );

/**
  Write the 4 Bytes value from a CMOS location.

  If the passed address is beyond the max address return RETURN_NOT_FOUND.
  If the function completed successfully return RETURN_SUCCESS
  
  @param[in]  CmosAddress  Location to write to CMOS
  @param[in]  Data         Value to be written to the CMOS

  @retval     Status
**/
RETURN_STATUS
EFIAPI
WriteCmos32 (
  IN UINT8  CmosAddress,
  IN UINT32 Data
  );

/**
  Funtion to Initialize the CMOS.

  Checks the presence of CMOS battery, else it initialize CMOS to default.
  Perform a checksum computation and verify if the checksum is correct.
  If the input parameter ForceInit is TRUE, initialize all the CMOS 
  location to their default values 

  @param[in]  ForceInit         A boolean variable to initialize the CMOS to its default
                                without checking the RTC_PWR_STS or verifying the checksum.

  @param[out] DefaultsRestored  A boolean variable to indicate if the defaults were restored

  @retval     Status
**/
RETURN_STATUS
EFIAPI
InitCmos (
  IN  BOOLEAN     ForceInit,
  OUT BOOLEAN     *DefaultsRestored
  );

#endif // _CMOS_ACCESS_LIB_H_
