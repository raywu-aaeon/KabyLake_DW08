//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
#ifndef _SG_TPV_ACPITABLES_H_
#define _SG_TPV_ACPITABLES_H_

#include <Token.h>
#include <AmiCspLib.h>
#include <AmiDxeLib.h>
#include <Setup.h>
#include <Protocol/SaPolicy.h>
#include <IndustryStandard/Acpi.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Protocol/BootScriptSave.h>
#include <Protocol/CpuIo.h>
#include <Protocol/FirmwareVolume.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/AcpiSupport.h>
#include <Protocol/PciRootBridgeIo.h>
#include <CpuAccess.h>
#include <Protocol/PciIo.h>
#include <Library/PciLib.h>
#include <IndustryStandard/Acpi30.h>
#include <Protocol/SaGlobalNvsArea.h>
#include <SaDataHob.h>
#include <Protocol/Mxm30.h>
#include <Library/MmPciLib.h>
#include <NbSetupData.h>
#include <Library/PchInfoLib.h>

#define MemoryRead16(Address)   * (UINT16 *) (Address)
#define MemoryRead8(Address)    * (UINT8 *) (Address)

//
// Function Prototype declaration
//
EFI_STATUS InstallNvidiaOpRegion (VOID);
EFI_STATUS InstallAmdOpRegion (VOID);
EFI_STATUS SgUpdateDsdt (VOID);

#if defined(SGOEMSSDT_SUPPORT) && SGOEMSSDT_SUPPORT == 1
EFI_STATUS InstallOemTpvAcpiTables (  
  IN EFI_GUID   *OemRefAcpiTableGuid
  );

VOID
LoadOemTpvAcpiTables (
  IN EFI_EVENT Event,
  IN VOID      *Context
  );
#endif

VOID
LoadSgTpvAcpiTables (
  IN EFI_EVENT Event,
  IN VOID      *Context
  );

//----------------------------------------------------------------------------
//
// Switchable Graphics defines.
//
//----------------------------------------------------------------------------

//
// SSDT Guid file
//
#define NVIDIA_VID              0x10DE
#define NVOPT_SSID_OFFSET       0x40

#define AMD_SVID_OFFSET         0x4C
#define AMD_SDID_OFFSET         0x4E
#define AMD_VID                 0x1002

//
// OpRegion Header #defines.
//
#define NVIG_HEADER_SIGNATURE    "NVSG-IGD-DSM-VAR"
#define NVIG_OPREGION_SIZE       1
#define NVIG_OPREGION_VER        0x00000201
#define NVHM_HEADER_SIGNATURE    "NvSwitchable_Gfx"
#define NVHM_OPREGION_SIZE       193
#define NVHM_OPREGION_VER        0x00000201
#define APXM_HEADER_SIGNATURE    "AMD--PowerXpress"
#define APXM_OPREGION_SIZE       129
#define APXM_OPREGION_VER        0x00000201

//
// OpRegion structures:
//
// Note: These structures are packed to 1 byte offsets because the exact
// data location is required by the supporting design specification due to
// the fact that the data is used by ASL and Graphics driver code compiled
// Separately.
//

//
// NVIG OpRegion
//
#pragma pack (1)
typedef struct {
  // OpRegion Header             // Byte offset(decimal)

  CHAR8   NISG[0x10];            // 0            NVIG OpRegion Signature
  UINT32  NISZ;                  // 16           NVIG OpRegion Size in KB
  UINT32  NIVR;                  // 20           NVIG OpRegion Version
  // OpRegion Data
  UINT32  GPSS;                  // 24           Policy Selection Switch Status (Current GPU)
  UINT16  GACD;                  // 32           Active Displays
  UINT16  GATD;                  // 34           Attached Displays
  CHAR8   LDES;                  // 36           Lid Event State
  CHAR8   DKST;                  // 37           Dock State
  CHAR8   DACE;                  // 38           Display ACPI Event
  CHAR8   DHPE;                  // 39           Display Hot-Plug Event
  CHAR8   DHPS;                  // 40           Display Hot-Plug Status
  CHAR8   SGNC;                  // 41           Notify Code (Cause of Notify(..,0xD0))
  CHAR8   GPPO;                  // 42           Policy Override
  CHAR8   USPM;                  // 43           Update Scaling Preference Mask
  CHAR8   GPSP;                  // 44           Panel Scaling Preference
  CHAR8   TLSN;                  // 45           Toggle List Sequence Number
  CHAR8   DOSF;                  // 46           Flag for _DOS
  UINT16  ELCL;                  // 47           Endpoint Link Control Register Value
} NVIG_OPREGION;                 // Total 49  Bytes
#pragma pack ()

//
// NVHM OpRegion
//
#pragma pack (1)
typedef struct {
  // OpRegion Header             // Byte offset(decimal)
  CHAR8   NVSG[0x10];            // 0            NVHM OpRegion Signature
  UINT32  NVSZ;                  // 16           NVHM OpRegion Size in KB
  UINT32  NVVR;                  // 20           NVHM OpRegion Version
  // OpRegion Data
  UINT32  NVHO;                  // 24           NVHM Opregion Address
  UINT32  RVBS;                  // 28           Nvidia VBIOS Image Size
  CHAR8   RBUF[0x30000];         // 32           192KB VBIOS
  UINT32  MXML;                  // 192k+32       Nvidia Mxm3 Buffer length
#if MXM30_SUPPORT == 1
  CHAR8   MXM3[MXM_ROM_MAX_SIZE];// 192k+36       Nvidia Mxm3 Buffer
#else
  CHAR8   MXM3[200];
#endif
} NVHM_OPREGION;                
#pragma pack ()

//
// Entire Nvidia OpRegion
//
#pragma pack(1)
typedef struct {
  NVIG_OPREGION          *NvIgOpRegion;                 // 47  Bytes   
  NVHM_OPREGION          *NvHmOpRegion;                 // 65568 Bytes
} NVIDIA_OPREGION;                                      // Total 65615 Bytes
#pragma pack()

//
// APXM OpRegion
//
#pragma pack (1)
typedef struct {
  // OpRegion Header        // Byte offset(decimal)                
  CHAR8   APSG[0x10];       // 0            APXM OpRegion Signature
  UINT32  APSZ;             // 16           APXM OpRegion Size in KB
  UINT32  APVR;             // 20           APXM OpRegion Version
  // OpRegion Data
  UINT32  APXA;             // 24           PX OpRegion Address
  UINT32  RVBS;             // 28           PX Runtime VBIOS Image Size
  UINT16  NTLE;             // 32           Total number of toggle list entries
  UINT16  TLEX[15];         // 34           The display combinations in the list...
  UINT16  TGXA;             // 64           Target GFX adapter as notified by ATPX function 5
  UINT16  AGXA;             // 66           Active GFX adapter as notified by ATPX function 6
  CHAR8   GSTP;             // 68           GPU switch transition in progress
  CHAR8   DSWR;             // 69           Display Switch Request
  CHAR8   EMDR;             // 70           Expansion Mode Change Request
  CHAR8   PXGS;             // 71           PowerXpress graphics switch toggle request
  UINT16  CACD;             // 72           Currently Active Displays
  UINT16  CCND;             // 74           Currently Connected Displays
  UINT16  NACD;             // 76           Next Active Index
  CHAR8   EXPM;             // 78           Expansion Mode
  UINT16  TLSN;             // 79           Toggle list sequence index
  UINT16  ELCL;             // 81           Endpoint Link Contol Register Value
  CHAR8   RBUF[0x10000];    // 83           VBIOS 64KB
} APXM_OPREGION;            // Total 65626   Bytes
#pragma pack ()

//
// Entire AMD OpRegion
//
#pragma pack (1)
typedef struct {
 APXM_OPREGION                          *ApXmOpRegion;                // Total 65617   Bytes
} AMD_OPREGION;
#pragma pack ()

#endif

//**********************************************************************
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
