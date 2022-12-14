//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**        (C)Copyright 1985-2010, American Megatrends, Inc.    **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**             5555 Oakbrook Pkwy   , Norcross, GA 30071       **//
//**                                                             **//
//**                       Phone: (770)-246-8600                 **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/TseLite/StyleHook/styleHook20.c $
//
// $Author: Madhans $
//
// $Revision: 1 $
//
// $Date: 1/06/11 7:36p $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
/** @file styleHook20.c
    Contains generic or default style functions.

**/
//**********************************************************************
#include "minisetup.h"





/**
    Function to get scrollbar downarrow color

    @param VOID

    @retval FGCOLOR|BGCOLOR

**/
UINT8 StyleGetScrollBarDownArrowColor(VOID)
{
	return (EFI_BACKGROUND_BLUE|EFI_WHITE);
//	return  (SCROLLBAR_DOWNARROW_FGCOLOR | SCROLLBAR_DOWNARROW_BGCOLOR) ;
}
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**        (C)Copyright 1985-2010, American Megatrends, Inc.    **//
//**                                                             **//
//**                       All Rights Reserved.                  **//
//**                                                             **//
//**             5555 Oakbrook Pkwy   , Norcross, GA 30071       **//
//**                                                             **//
//**                       Phone: (770)-246-8600                 **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
