//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************

#ifndef _PEI_SG_TPV_LIB_H_
#define _PEI_SG_TPV_LIB_H_
#include <Token.h>
#include <Efi.h>
#include <Pei.h> 
#include <AmiPeiLib.h>
#include <Library/AmiChipsetIoLib.h>

#include <SaAccess.h>
#include <Ppi/SiPolicy.h>
#include <Library/PchInfoLib.h>
#include <Library/TimerLib.h>
#include <Library/DebugLib.h>
#include <ConfigBlock/SwitchableGraphicsConfig.h>
#include <SaPolicyCommon.h>

VOID
PcieGpioWrite(
  IN       UINT8                        GpioSupport,
  IN       UINT8                        Expander,
  IN       UINT32                       Gpio,
  IN       BOOLEAN                      Active,
  IN       BOOLEAN                      Level
);

BOOLEAN 
SgTpvGpuCheck (
  UINT8          BusNum
);

VOID
SgTpvN17PWorkaround (
  VOID        
);

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
