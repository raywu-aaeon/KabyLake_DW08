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

#include <Token.h>
#include <AmiDxeLib.h>
#include <Protocol/Timer.h>
#if CSM_SUPPORT
#include <Protocol/LegacyBiosExt.h>
#endif

#if CSM_SUPPORT
UINT64                      mTimerPeriod = 0;
EFI_TIMER_ARCH_PROTOCOL     *mTimer = NULL;

EFI_STATUS SbCsm16CallCompanionFunctionsHook (
  IN OUT AMI_CSM_THUNK_DATA   *ThunkData,
  IN BOOLEAN                  Priority
  )
{
  EFI_STATUS Status;
  
  if (mTimer == NULL) {
    Status = pBS->LocateProtocol (&gEfiTimerArchProtocolGuid, NULL, (VOID **)&mTimer);
    if (EFI_ERROR(Status))
    {
        ASSERT_EFI_ERROR(Status);
    }
  }
  
  if (Priority) {
    mTimer->GetTimerPeriod (mTimer, &mTimerPeriod); 
    if (mTimerPeriod != 0) {
      mTimer->SetTimerPeriod (mTimer, 0);
    }
  }
  else {
    if (mTimerPeriod != 0) {        
      mTimer->SetTimerPeriod (mTimer, mTimerPeriod);
    }
  }
  
  return EFI_SUCCESS;
}
#endif

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
