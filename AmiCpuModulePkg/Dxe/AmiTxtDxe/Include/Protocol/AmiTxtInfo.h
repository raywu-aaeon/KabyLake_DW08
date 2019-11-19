//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1987-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file AmiTxtInfo.h
    AmiTxt Info header file.

**/

#ifndef __AMI_TXT_INFO_H__
#define __AMI_TXT_INFO_H__

#ifdef __cplusplus
extern "C" {
#endif

#define AMI_TXT_INFO_PROTOCOL_GUID \
    {0x1c92f0ab, 0x3351, 0x1be5, 0xaf, 0xba, 0xc1, 0x25, 0x61, 0xbb, 0x32, 0xa3}

#define AMI_TXT_INFO_PROTOCOL_VERSION 1


typedef struct {
  BOOLEAN               ChipsetIsTxtCapable; ///< Value is set to 1 if chipset is Intel TXT capable.  
  BOOLEAN               ChipsetIsProduction; ///<TRUE Production Fuse, FALSE Debug Fuse>
  BOOLEAN               BiosAcmIsProduction; ///<TRUE Production Fuse, FALSE Debug Fuse>
  BOOLEAN               CpuIsTxtCapable;
  UINT32                TxtErrCode;
} AMI_TXT_INFO_DATA;

typedef struct _AMI_TXT_INFO_PROTOCOL AMI_TXT_INFO_PROTOCOL;

typedef EFI_STATUS (*GET_TXT_INFO) (
	IN AMI_TXT_INFO_PROTOCOL    *AMI_TXT_INFO_PROTOCOL,
    OUT VOID                    **AmiTxtInfoData
);

struct _AMI_TXT_INFO_PROTOCOL {
    //Protocol Ver 0
    UINT32                  ProtocolVer;
    UINT64                  Flags;    
    GET_TXT_INFO            GetTxtInfo;
};
/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1987-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
