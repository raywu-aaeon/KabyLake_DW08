//***********************************************************************
//*                                                                     *
//*   Copyright (c) 1985-2019, American Megatrends International LLC.   *
//*                                                                     *
//*      All rights reserved. Subject to AMI licensing agreement.       *
//*                                                                     *
//***********************************************************************

#include <Token.h>
#include <PiDxe.h>
#include <AmiDxeLib.h>
#include <AmiHobs.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/EsrtManagement.h>
#include <Protocol/AmiReflashProtocol.h>
#include <Protocol/MeFwUpdLclProtocol.h>
#include <Capsule.h>
#include <Setup.h>
