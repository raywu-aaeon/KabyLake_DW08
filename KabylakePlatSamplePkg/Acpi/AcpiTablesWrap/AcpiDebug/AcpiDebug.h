#ifndef _ACPI_DEBUG_H_
#define _ACPI_DEBUG_H_

#include <Base.h>
#include <Uefi.h>
#include <Setup.h>
#include <AmiLib.h>
#include <IndustryStandard/Acpi.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/AcpiTable.h>
#include <AcpiGetSetupData/AcpiSetupData.h>
///
/// AcpiDebug ACPI table data storage file
///
#define ACPI_DEBUG_TABLE_STORAGE_GUID \
{0xa54c760e, 0x415d, 0x4157, {0xa7, 0x4a, 0xf7, 0x49, 0x41, 0x87, 0x3f, 0x65}}


///
/// Variable and External Declaration(s)
///
#define  AcpiDebugBufferSize    0x10000 // 64k buffer data
#define  ACPI_DEBUG_STR "INTEL ACPI DEBUG"
#define  AML_NAME_OP         0x08

///
/// ASL NAME structure
///
#pragma pack(1)
typedef struct {
  UINT8   NameOp;       //  Byte [0]=0x08:NameOp.
  UINT32  NameString;   //  Byte [4:1]=Name of object.
  UINT8   DWordPrefix;  //  Byte [5]=0x0C:DWord Prefix.
  UINT32  Value;        //  Byte [9:6] ; Value of named object.
} NAME_LAYOUT;
#pragma pack()

/**
  Routine to search the FV for compiled AcpiDebug SSDT AML binary and install it.

  @retval EFI_SUCCESS    IRMT ACPI tables are initialized successfully
  @retval EFI_NOT_FOUND  IRMT ACPI tables not found
**/
STATIC
EFI_STATUS
InitializeAcpiDebugAcpiTables (
  VOID
  );

/**
  Callback to install AcpiDebug ACPI table at ready to boot. 

  @param[in] Event    The event that triggered this notification function
  @param[in] Context  Pointer to the notification functions context
**/
VOID
AcpiDebugOnReadyToBoot (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  );

/**
  Initialize the Acpi Debug Table support.

  @param[in] ImageHandle         - Handle for the image of this driver
  @param[in] SystemTable         - Pointer to the EFI System Table

  @retval EFI_SUCCESS          - IrmtAcpi initialization complete
  @retval EFI_OUT_OF_RESOURCES - Unable to allocated memory
**/
EFI_STATUS
EFIAPI
InstallADebTabl (
  IN EFI_HANDLE                      ImageHandle,
  IN EFI_SYSTEM_TABLE                *SystemTable
  );

#endif
