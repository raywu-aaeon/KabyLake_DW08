//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
//  Name:           BoardSetup.h
//
//  Description:    
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#ifndef _SioLdnDefaultDisable_H_
#define _SioLdnDefaultDisable_H_


typedef struct {
	CHAR16*      LdnName;  //LdnName refert to build/GSIOSETUPVAR.H
	BOOLEAN	     Enable;  //True-->Default Enable
	                      //FALSE-->Default Disable.
} AAEON_LDN_Default;

typedef struct {
	UINT8   PatchFlag;
} AAEON_SIOD4PATCH_DATA;

#endif

//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2010, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**              5555 Oakbrook Pkwy, Suite 200 Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
