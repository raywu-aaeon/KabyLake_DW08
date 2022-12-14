/** @file
  This Protocol will be installed at the end of S3 resume phase in SMM environment. 
  It allows for smm drivers to hook this point and do the requried tasks.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials                          
  are licensed and made available under the terms and conditions of the BSD License         
  which accompanies this distribution.  The full text of the license may be found at        
  http://opensource.org/licenses/bsd-license.php                                            

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

**/

#ifndef __SMM_END_OF_S3_RESUME_H__
#define __SMM_END_OF_S3_RESUME_H__

#define EDKII_SMM_END_OF_S3_RESUME_PROTOCOL_GUID \
  { \
    0x96f5296d, 0x05f7, 0x4f3c, {0x84, 0x67, 0xe4, 0x56, 0x89, 0x0e, 0x0c, 0xb5 } \
  }

extern EFI_GUID gEdkiiSmmEndOfS3ResumeProtocolGuid;

#endif
