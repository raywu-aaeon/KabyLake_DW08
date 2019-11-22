#include <token.h>
#include <AmiDxeLib.h> // Optional. should use Mde Library instead.
#include <AmiCspLib.h> // Optional. should use Mde Library instead.
#include <Setup.h> // Optional. should use Mde Library instead.

extern EFI_BOOT_SERVICES		*pBS;
extern EFI_STATUS
LocateAcpiTableBySignature (
  IN      UINT32                        Signature,
  IN OUT  EFI_ACPI_DESCRIPTION_HEADER   **Table,
  IN OUT  UINTN                         *Handle
  );

int _outp (unsigned short port, int databyte );

#pragma intrinsic(_outp)

VOID Debug_80(
UINT8	debugcode,
UINT8	Delay
){
	UINT32	Count;

	Count = Delay * 0x1FFFF;

	while(Count)
	{
		_outp(0x80, debugcode);
		Count--;
	}
}

/**
    This function allocates memory resource for BFPI
    and put memory address to BFPI ASL opregion.

    @param Event   - Event of callback
    @param Context - Context of callback.

    @retval VOID
**/

VOID UpdateBfpiAcpiArea (
    IN EFI_EVENT  Event,
    IN VOID       *Context )
{
  VOID    *Registration = NULL;
  VOID    *GlobalNvsArea;
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT32  *AaeonBfpiDataBuffer = NULL;
  EFI_ACPI_DESCRIPTION_HEADER  *FacpTable;
  UINTN    AcpiHandle = 0;
  UINT8    *CurrPtr;
  UINT8    *DsdtPointer;
  UINT32  *Signature;
  UINT8    *Operation;
  UINT32  *Address;

	Status = pBS->LocateProtocol (
              &gEfiGlobalNvsAreaProtocolGuid,
              NULL,
              (VOID **)&GlobalNvsArea
  );
	if (EFI_ERROR (Status)) {
		// No GPNV Protocol was found. Register UpdateBfpiAcpiArea as callback instead.
    Status = RegisterProtocolCallback(
                &gEfiGlobalNvsAreaProtocolGuid,
                UpdateBfpiAcpiArea,
                NULL, &Event, &Registration
    );
    if (EFI_ERROR (Status)) {
      TRACE((-1,"ERROR: AaeonBfpi: Register UpdateBfpiAcpiArea as callback failed. This will cause BSOD.\n"));
      Debug_80(0xBF,1);
      Debug_80(0xEE,1);
      Debug_80(0xBF,1);
      Debug_80(0xEE,1);
      Debug_80(0xBF,1);
      Debug_80(0xEE,1);
    }
		return;
	}

  CurrPtr = NULL;
  Status = pBS->AllocatePool(EfiACPIMemoryNVS, 0x1004, (VOID **)&AaeonBfpiDataBuffer);
  if (!EFI_ERROR(Status))
  {
    Status = LocateAcpiTableBySignature (
                EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE,
                (EFI_ACPI_DESCRIPTION_HEADER **)  &FacpTable,
                &AcpiHandle
                );
          
    if (!EFI_ERROR(Status))
    {
      //
      // Loop through the ASL looking for WMDB.
      //
      CurrPtr = (VOID*)(UINTN)((EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE *)FacpTable)->Dsdt;
      for (DsdtPointer = CurrPtr;
           DsdtPointer <= (CurrPtr + ((EFI_ACPI_COMMON_HEADER *) CurrPtr)->Length);
           DsdtPointer++
          ) {
        Signature = (UINT32 *) DsdtPointer;
        if (*Signature == SIGNATURE_32 ('W', 'M', 'D', 'B'))
        {
          //
          // Conditional match.  For Region Objects, the Operator will always be the
          // byte immediately before the specific name.  Therefore, subtract 1 to check
          // the Operator.
          //
          Operation = DsdtPointer - 1;
          if (*Operation == AML_EXT_REGION_OP)
          {
            Address   = (UINT32 *) (DsdtPointer + 6);
            *Address  = (UINT32)AaeonBfpiDataBuffer;
            *AaeonBfpiDataBuffer  = (UINT32)AaeonBfpiDataBuffer;
          }
        }
      }
    }
  }
  if (EFI_ERROR (Status)) {
    TRACE((-1,"ERROR: AaeonBfpi: fail to UpdateBfpiAcpiArea. This will cause BSOD.\n"));
    Debug_80(0xBF,1);
    Debug_80(0xEE,1);
    Debug_80(0xBF,1);
    Debug_80(0xEE,1);
    Debug_80(0xBF,1);
    Debug_80(0xEE,1);
  }
  pBS->CloseEvent(Event);
}

/**
  AAEON BFPI ACPI initialization function.

  @param[in] ImageHandle     Handle for this drivers loaded image protocol.
  @param[in] SystemTable     EFI system table.

  @retval EFI_SUCCESS        The driver installed without error.
  @retval EFI_ABORTED        The driver encountered an error and could not complete installation of
                             the ACPI tables.

**/
EFI_STATUS
EFIAPI
AaeonBfpiDxeEntry (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;

  InitAmiLib(ImageHandle, SystemTable);

  UpdateBfpiAcpiArea(NULL, NULL);

  return EFI_SUCCESS;
}