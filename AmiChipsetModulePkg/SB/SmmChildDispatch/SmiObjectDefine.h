//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

#ifndef _AMI_GENERAL_SMI_OBJECT_DEFINE_H_
#define _AMI_GENERAL_SMI_OBJECT_DEFINE_H_

typedef enum
{
    SMI_OBJ_GENERAL_TYPE = 0,
    SMI_OBJ_CONTEXT_TYPE,
    SMI_OBJ_HANDLE_TYPE,
    SMI_OBJ_MAX_TYPE
} SMI_OBJECT_TYPE;

typedef enum
{
    SMI_STATUS_NULL = 0,
    SMI_STATUS_NORMAL,
    SMI_STATUS_DELETED,
    SMI_STATUS_MAX
} SMI_CONTEXT_STATUS;

typedef enum
{
    SMI_GENERAL_CONTEXT = 0,
    SMI_AMI_CONTEXT,
    SMI_PI_0_9_CONTEXT,
    SMI_PI_1_1_CONTEXT,
    SMI_MAX_TYPE
} SMI_CONTEXT_TYPE;

typedef enum
{
    SMICONTEXT_EXTCMD_GET_REGISTER_CONTEXT = 0,
    SMICONTEXT_EXTCMD_REGISTER,
    SMICONTEXT_EXTCMD_UNREGISTER,
    SMICONTEXT_EXTCMD_MAX
} SMI_CONTEXT_EXTEND_CMD;

typedef enum
{
    SMIHANDLE_EXTCMD_ALLOCATE_CONTEXT = 0,
    SMIHANDLE_EXTCMD_GET_CONTEXT,
    SMIHANDLE_EXTCMD_DISPATCH_SMI_START,
    SMIHANDLE_EXTCMD_DISPATCH_SMI_END,
    SMIHANDLE_EXTCMD_REGISTER_START,
    SMIHANDLE_EXTCMD_REGISTER_END,
    SMIHANDLE_EXTCMD_UNREGISTER_START,
    SMIHANDLE_EXTCMD_UNREGISTER_END,
    SMIHANDLE_EXTCMD_MAX
} SMI_HANDLE_EXTEND_CMD;


// Function Definition(s)

#define SB_SMM_INIT_HANDLE 0xC0010000
#define SB_SMM_DISPATCH_SMI 0xC0020000
#define SB_SMM_REGISTER 0xC0030000
#define SB_SMM_UNREGISTER 0xC0040000

EFI_STATUS SbSmiDispatch (
    UINT32 CmdCode,
    VOID* ObjBuffer,
    VOID* DataPointer,
    VOID* Handle
);

#endif

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
