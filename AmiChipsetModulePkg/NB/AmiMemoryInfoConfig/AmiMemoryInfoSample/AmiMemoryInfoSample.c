//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

#include <Uefi.h>

#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

#include <Protocol/AmiMemInfo.h>

// Followed MEMORY_TYPE_INFO
STATIC CONST CHAR16* MemoryTypeStr[] = {
    L"UnknownType",
    L"DDR1",
    L"DDR2",
    L"DDR3",
    L"DDR3L",
    L"DDR3U",
    L"LPDDR3",
    L"DDR4",
    L"OtherType"
};

STATIC CONST CHAR16* BooleanStr[] = {
    L"FALSE",
    L"TRUE"
};

/**
    This function is used to print AmiMemInfo protocol

    @param AmiMemoryInfo is AmiMemInfo protocol

    @retval void

**/
VOID
PrintAmiMemInfoEfi (
  AMI_MEMORY_INFO        *AmiMemoryInfo
  )
{
  UINT8               Dimm;
  UINTN               MemoryArrayIndex;
  MEMORY_TYPE_INFO    RamType;
  UINT8               DoubleSide;

  for (MemoryArrayIndex = 0; MemoryArrayIndex < MEMORY_ARRAY_NUM; MemoryArrayIndex++ ) {

    RamType = AmiMemoryInfo->MemoryArray[MemoryArrayIndex].RamType;
    Print(L"RamType: %d (%s)\n",    AmiMemoryInfo->MemoryArray[MemoryArrayIndex].RamType, MemoryTypeStr[RamType]);
    Print(L"MaxCapacity: %d MiB\n", AmiMemoryInfo->MemoryArray[MemoryArrayIndex].MaxCapacity);
    Print(L"Speed: %d Mhz\n",       AmiMemoryInfo->MemoryArray[MemoryArrayIndex].Speed);
    Print(L"TotalMemory: %d MiB\n", AmiMemoryInfo->MemoryArray[MemoryArrayIndex].TotalMemory);

    for (Dimm = 0; Dimm < DIMM_SLOT_NUM; Dimm++) {
      DoubleSide = (UINT8) AmiMemoryInfo->MemoryArray[MemoryArrayIndex].Slot[Dimm].DoubleSide;
      Print(L"--- DIMM Slot %0d --- \n", Dimm);
      Print(L"Size: %d MiB\n",          AmiMemoryInfo->MemoryArray[MemoryArrayIndex].Slot[Dimm].Size);
      Print(L"Ecc: %d\n",               AmiMemoryInfo->MemoryArray[MemoryArrayIndex].Slot[Dimm].Ecc);
      Print(L"SpdAddr: 0x%X\n",         AmiMemoryInfo->MemoryArray[MemoryArrayIndex].Slot[Dimm].SpdAddr);
      Print(L"DoubleSide: 0x%X (%s)\n", AmiMemoryInfo->MemoryArray[MemoryArrayIndex].Slot[Dimm].DoubleSide, BooleanStr[DoubleSide]);
    }

  }

}

/**
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
AmiMemoryInfoSampleMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS         Status = EFI_SUCCESS;
  AMI_MEMORY_INFO    *AmiMemoryInfo;

  ///
  /// Get the AmiMemInfo protocol.
  ///
  Status = gBS->LocateProtocol (
                  &gAmiMemoryInfoGuid,
                  NULL,
                  (VOID **) &AmiMemoryInfo
                  );
  if (EFI_ERROR (Status)) {
    Print(L"LocateProtocol failed: %r\n", Status);
    return EFI_SUCCESS;
  }
  
  ///
  /// Print AmiMemInfo
  ///
  Print(L"AmiMemInfo Protocol\n");

  PrintAmiMemInfoEfi(AmiMemoryInfo);
  
  return EFI_SUCCESS;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
