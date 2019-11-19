#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include <Library/DebugLib.h>

/**
  This procedure will get PCIE address

  @param[in] Bus                  Pci Bus Number
  @param[in] Device               Pci Device Number
  @param[in] Function             Pci Function Number

  @retval PCIE address
**/
UINTN
MmPciBase (
  IN UINT32                       Bus,
  IN UINT32                       Device,
  IN UINT32                       Function
  )
{
  ASSERT ((Bus <= 0xFF) && (Device <= 0x1F) && (Function <= 0x7));

  return ((UINTN) (PciRead32 (PCI_LIB_ADDRESS (0, 0, 0, 0x60)) & 0xff000000) + (UINTN) (Bus << 20) + (UINTN) (Device << 15) + (UINTN) (Function << 12));
}
