//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file
    This file contains test related to Secure Boot Bypass and for CSM 
    being disabled when Secure Boot is active

 */
#include <AmiHsti.h>

#include <Setup.h>
#include <ImageAuthentication.h>
#include <Protocol/LegacyBiosExt.h>

extern EFI_GUID gEfiGlobalVariableGuid;
extern CHAR16 ImplementationId[];

VOID SecureBootBypass( VOID
        )
{
    UINT64 ImplementationBitMap;
    BOOLEAN Error = TRUE; // for this case error is initialized to true
    UINT8 TestBit;
    
    UINTN      DataSize;
    UINT8      *pSecureBootEnable= NULL;
    
    TestBit = PcdGet8(AmiPcdHstiSecureBootBypassImplementationBit);
    
    // get the PCD that defines the Bitmap of Implemented Tests for the IBV
    ImplementationBitMap = PcdGet64(AmiPcdHstiImplementedTestBitmap);
    
    DEBUG((DEBUG_INFO, "SecureBootBypass BitMap %0lx, TesBit %d\r\n", ImplementationBitMap, TestBit));
    // check if test should be run for this system
    if ((TestBit == 0xFF) || !(ImplementationBitMap & ((UINT64)1 << TestBit) ))
    {
        DEBUG((DEBUG_INFO, "SecureBootBypass is not implemented\r\n"));
        return;
    }

    // SecureBoot variable to be installed along with NVRAM driver
    GetEfiVariable(EFI_SECURE_BOOT_NAME, &gEfiGlobalVariableGuid, NULL, &DataSize, &pSecureBootEnable);
    // If secure boot is enabled, check the bypass status.  If bypass is enabled there is no error
    // SecureBoot Enabled - Bypass TRUE     = Error
    // SecureBoot Enabled - Bypass FALSE    = No Error
    // SecureBoot Disabled - Bypass XXXX    = Error
    // SecureBoot Disabled - Bypass XXXX    = Error
    if(pSecureBootEnable && (*pSecureBootEnable) == 1)
    {
        // Secure Boot is enabled, check for Bypass bring enabled.
        // Clear error if bypass is disabled
        if (!PcdGetBool(AmiPcdSecureBootBypass))
            Error = FALSE;
    }
    	
    DEBUG((DEBUG_ERROR, "SecureBootBypass Error %x\n",  Error));
    
    if (Error)
    {
        ReportError(TestBit, 1, STRING_TOKEN(AMI_HSTI_ERROR_SECURE_BOOT_BYPASS));
    }
    else
    {
        DEBUG((DEBUG_INFO, "Setting Secure boot bypass verification bits\n\r"));

        HstiLibSetFeaturesVerified(PLATFORM_SECURITY_ROLE_PLATFORM_IBV, ImplementationId, TestBit / 8, 1 << (TestBit % 8));
#if defined (HSTI_REVISION) && (HSTI_REVISION == 1)
        HstiLibSetWindowsSecurityFeaturesVerified(PLATFORM_SECURITY_ROLE_PLATFORM_IBV, ImplementationId, 
                HSTI_WINDOWS_SECURITY_FEATURE_BIOS_NO_SECURE_BOOT_BYPASS / 8,
                1 << (HSTI_WINDOWS_SECURITY_FEATURE_BIOS_NO_SECURE_BOOT_BYPASS % 8));
#endif
    }
  
}



VOID SecureBootCsmDisable( VOID
        )
{
    EFI_STATUS Status;
    UINT64 ImplementationBitMap;
    BOOLEAN Error = FALSE;
    UINT8 TestBit;
    
    UINTN      DataSize;
    UINT8      *pSecureBootEnable= NULL;
    EFI_LEGACY_BIOS_EXT_PROTOCOL *LegacyBoot;

    TestBit = PcdGet8(AmiPcdHstiSecureBootCsmDisableImplementationBit); 
    // get the PCD that defines the Bitmap of Implemented Tests for the IBV
    ImplementationBitMap = PcdGet64(AmiPcdHstiImplementedTestBitmap);
    
    DEBUG((DEBUG_INFO, "SecureBootCsmDisable BitMap %0lx, TesBit %d\r\n", ImplementationBitMap, TestBit));
    // check if test should be run for this system
    if ((TestBit == 0xFF) || !(ImplementationBitMap & ((UINT64)1 << TestBit) ))
    {
        DEBUG((DEBUG_INFO, "SecureBootCsmDisable is not implemented\r\n"));
        return;
    }
    //
    // SecureBoot variable to be installed along with NVRAM driver
    //
    GetEfiVariable(EFI_SECURE_BOOT_NAME, &gEfiGlobalVariableGuid, NULL, &DataSize, &pSecureBootEnable);
    Status = gBS->LocateProtocol (
                    &gAmiLegacyBootProtocolGuid,
                    NULL,
                    &LegacyBoot
                    );

    
    // If secure boot is enabled, check the bypass status.  If bypass is enabled there is no error
    // SecureBoot Enabled - LegacyBootProtocol - Found  = Error
    // SecureBoot Enabled - LegacyBootProtocol - not Found  = No Error
    if(pSecureBootEnable && ((*pSecureBootEnable) == 1) && !EFI_ERROR(Status))
    {
            Error = TRUE;
    }    
        
    DEBUG((DEBUG_ERROR, "Secure Boot Csm Disable Error %x\n",  Error));
    
    if (Error)
    {
        ReportError(TestBit, 1, STRING_TOKEN(AMI_HSTI_ERROR_SECURE_BOOT_CSM_DISABLE));
        Error = TRUE;
    }
    else
    {
        DEBUG((DEBUG_INFO, "Setting Secure Boot CSM Disable verification bits\n\r"));

        HstiLibSetFeaturesVerified(PLATFORM_SECURITY_ROLE_PLATFORM_IBV, ImplementationId, TestBit / 8, 1 << (TestBit % 8));
#if defined (HSTI_REVISION) && (HSTI_REVISION == 1)
        HstiLibSetWindowsSecurityFeaturesVerified(PLATFORM_SECURITY_ROLE_PLATFORM_IBV, ImplementationId, 
                HSTI_WINDOWS_SECURITY_FEATURE_BIOS_NO_SECURE_BOOT_BYPASS / 8,
                1 << (HSTI_WINDOWS_SECURITY_FEATURE_BIOS_NO_SECURE_BOOT_BYPASS % 8));
#endif
    }
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
