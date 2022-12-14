#ifndef __AMI_SB_CHIPSET_GUID_H__
#define __AMI_SB_CHIPSET_GUID_H__

#define AMI_SB_CHIPSET_VARIABLE_GUID \
  { 0x75FD68A7, 0xE4B5, 0x475B, { 0x97, 0x08, 0xA3, 0x20, 0x7A, 0x49, 0xB5, 0x1D } }

#define AMI_SB_DXE_INIT_DONE_GUID \
  { 0x7551C584, 0xD387, 0x44D8, { 0xB8, 0x9F, 0x34, 0xA1, 0x8C, 0x70, 0xF9, 0xC8 } }

extern EFI_GUID gAmiSbChipsetVariableGuid;
extern EFI_GUID gAmiSbDxeInitDoneGuid;

#endif
