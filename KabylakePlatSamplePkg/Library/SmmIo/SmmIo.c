/** @file
  SMM I/O access utility implementation file, for Ia32

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

  This file contains a 'Sample Driver' and is licensed as such under the terms
  of your license agreement with Intel or your vendor. This file may be modified
  by the user, subject to the additional terms of the license agreement.

@par Specification Reference:

**/

//
// Include files
//

// AMI_OVERRIDE_START >>> Support MdePkg >= 15
//#include <Library/SmmIoLib.h>
#include <KabylakePlatSamplePkg/Include/Library/SmmIoLib.h>
// AMI_OVERRIDE_END <<<


/**
  Do a one byte IO read

  @param[in] Address - IO address to read

  @retval Data read
**/
UINT8
SmmIoRead8 (
  IN UINT16 Address
  )
{
  UINT8 Buffer;

  EFI_STATUS        Status;

  Status = gSmst->SmmIo.Io.Read (
                    &gSmst->SmmIo,
                    SMM_IO_UINT8,
                    Address,
                    1,
                    &Buffer
                    );

  ASSERT_EFI_ERROR (Status);
  return Buffer;
}

/**
  Do a one byte IO write

  @param[in] Address - IO address to write
  @param[in] Data    - Data to write
**/
VOID
SmmIoWrite8 (
  IN UINT16 Address,
  IN UINT8  Data
  )
{
  EFI_STATUS        Status;

  Status = gSmst->SmmIo.Io.Write (
                    &gSmst->SmmIo,
                    SMM_IO_UINT8,
                    Address,
                    1,
                    &Data
                    );

  ASSERT_EFI_ERROR (Status);
}

/**
  Do a two byte IO read

  @param[in] Address - IO address to read

  @retval Data read
**/
UINT16
SmmIoRead16 (
  IN UINT16 Address
  )
{
  UINT16 Buffer;

   EFI_STATUS        Status;

   Status = gSmst->SmmIo.Io.Read (
                     &gSmst->SmmIo,
                     SMM_IO_UINT16,
                     Address,
                     1,
                     &Buffer
                     );

   ASSERT_EFI_ERROR (Status);
  return Buffer;
}

/**
  Do a two byte IO write

  @param[in] Address - IO address to write
  @param[in] Data    - Data to write
**/
VOID
SmmIoWrite16 (
  IN UINT16 Address,
  IN UINT16 Data
  )
{
  EFI_STATUS        Status;

  Status = gSmst->SmmIo.Io.Write (
                    &gSmst->SmmIo,
                    SMM_IO_UINT16,
                    Address,
                    1,
                    &Data
                    );

  ASSERT_EFI_ERROR (Status);
}

/**
  Do a four byte IO read

  @param[in] Address - IO address to read

  @retval Data read
**/
UINT32
SmmIoRead32 (
  IN UINT16 Address
  )
{
  UINT32 Buffer;
     EFI_STATUS        Status;

     Status = gSmst->SmmIo.Io.Read (
                       &gSmst->SmmIo,
                       SMM_IO_UINT32,
                       Address,
                       1,
                       &Buffer
                       );

     ASSERT_EFI_ERROR (Status);
  return Buffer;
}

/**
  Do a four byte IO write

  @param[in] Address - IO address to write
  @param[in] Data    - Data to write
**/
VOID
SmmIoWrite32 (
  IN UINT16 Address,
  IN UINT32 Data
  )
{
  EFI_STATUS        Status;

  Status = gSmst->SmmIo.Io.Write (
                    &gSmst->SmmIo,
                    SMM_IO_UINT32,
                    Address,
                    1,
                    &Data
                    );

  ASSERT_EFI_ERROR (Status);
}
