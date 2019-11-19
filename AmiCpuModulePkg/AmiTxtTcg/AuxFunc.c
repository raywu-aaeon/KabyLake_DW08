//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file AuxFunc.c
    TPM related function.

**/

#include <Efi.h>
#include <Library/DebugLib.h>
#include <AmiTxtTcgPeim.h>

/**
    Check if TPM device is present.

    @retval BOOLEAN Return true if TPMdevice is present.
**/

BOOLEAN AuxChkIsTpmPresent()
{
    UINT8   TpmBase = *(UINT8*)TPM_BASE_ADDRESS;
    
    if( TpmBase == 0xFF || TpmBase == 0x00 )
        return FALSE;
    
    return TRUE;
}

/**
    Check if TPM device is PTPCRB.

    @retval BOOLEAN Return true if TPMdevice is PTPCRB.
**/

BOOLEAN AuxChkIsTpmPTPCRB()
{
    UINT32  TpmInterfaceID;
    UINT8   TpmLocState;
    
    TpmInterfaceID = *(UINT32*)(TPM_BASE_ADDRESS +R_TPM_INTERFACE_ID);
    TpmLocState = *(UINT8*)(TPM_BASE_ADDRESS +R_PTT_LOCALITY_STATE);
    
    if( !AuxChkIsTpmPresent() )
        return FALSE;
    
    
    if( (TpmInterfaceID & 0x0f) == 1 && (TpmLocState & 0x80) == 0x80 )
        return TRUE;
    
    return FALSE;
}

/**
    Check if TPM device is Tis.

    @retval BOOLEAN Return true if TPMdevice is Tis.
**/

BOOLEAN AuxChkIsTpmTis()
{
    TPM_1_2_REGISTERS_PTR TpmReg = (TPM_1_2_REGISTERS_PTR)TPM_BASE_ADDRESS;
    
    if( !AuxChkIsTpmPresent() )
        return FALSE;
    
    if( (TpmReg->Access & 0x80) == 0 ) // Check the TpmReg Valids
        return FALSE;
    
    return !AuxChkIsTpmPTPCRB();
}

/**
    Check if TPM device type.

    @retval BOOLEAN Return true if TPMdevice is Tis.
**/

TPM_DETECT_DEVICE_TYPE AuxChkTpmTisDeviceType()
{
    TPM_1_2_REGISTERS_PTR TpmReg = (TPM_1_2_REGISTERS_PTR)TPM_BASE_ADDRESS;
    UINT32 u32TpmSts;
    
    if( !AuxChkIsTpmTis() )
        return UnKnowTpm;
    
    TisRequestLocality( TpmReg );
    u32TpmSts = *(UINT32*)(TPM_BASE_ADDRESS+0x18);
    u32TpmSts >>= 26;
    u32TpmSts &= 0x03;
    TisReleaseLocality( TpmReg );
    
    if( u32TpmSts == 0 )
    {
        DEBUG((-1, "[%d]: Check TpmIs_12 \n", __LINE__));
        return TpmIs_12;
    }
    else if( u32TpmSts == 1 )
    {
    DEBUG((-1, "[%d]: Check TpmIs_20 \n", __LINE__));
        return TpmIs_20;
    }
    
    DEBUG((-1, "[%d]: Check UnKnowTpm \n", __LINE__));
    return UnKnowTpm;
    
}


//#if defined(PEI_INTERNAL_PROTOCOL)

/**
    Sent TPM submit command.

    @retval EFI_STATUS Return EFI status.
**/

EFI_STATUS
EFIAPI
TpmOemSubmitCommand (
  IN UINT32            InputParameterBlockSize,
  IN UINT8             *InputParameterBlock,
  IN OUT UINT32        *OutputParameterBlockSize,
  IN UINT8             *OutputParameterBlock
  )
{
    if( FALSE == AuxChkIsTpmPresent() )
        return EFI_DEVICE_ERROR;
    
    if( AuxChkIsTpmTis() )
    {
        return TpmLibPassThroughEX(
                    NULL,
                    InputParameterBlockSize,
                    InputParameterBlock,
                    *OutputParameterBlockSize,
                    OutputParameterBlock
                );
    }
    
    if( AuxChkIsTpmPTPCRB() )
    {
        return PttSubmitCommand(
                    NULL,
                    InputParameterBlockSize,
                    InputParameterBlock,
                    *OutputParameterBlockSize,
                    OutputParameterBlock
                );
    }
    
    return EFI_DEVICE_ERROR;
}
//#endif

/**
  Copies a source buffer to a destination buffer, and returns the destination buffer.

  This function copies Length bytes from SourceBuffer to DestinationBuffer, and returns
  DestinationBuffer.

  @param  DestinationBuffer   The pointer to the destination buffer of the memory copy.
  @param  SourceBuffer        The pointer to the source buffer of the memory copy.
  @param  Length              The number of bytes to copy from SourceBuffer to DestinationBuffer.

  @return EFI_STATUS Return EFI status.

**/

EFI_STATUS AuxCopyMem( VOID* Dest, VOID* Src, UINTN _size )
{
    UINT8   *_dest = Dest;
    UINT8   *_src = Src;

    while( _size-- )
    {
       *(_dest++) = *(_src++);
    }

    return EFI_SUCCESS;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
