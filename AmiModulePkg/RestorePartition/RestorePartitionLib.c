#include <Efi.h>

extern EFI_HANDLE   gBlkIoHandle;

/**
    This function is additional check for restore partition. 
    OEM can implement their restore partition policy here.
    
    @retval TRUE means this device is valid
    @retval FALSE means this device is not valid

**/
BOOLEAN
OemCheckRestorePartition(){
        
    return  TRUE;
}
