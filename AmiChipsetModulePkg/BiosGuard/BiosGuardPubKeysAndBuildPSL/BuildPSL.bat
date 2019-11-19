echo off
Set FITC_OUTIMAGE_MAP=%1
Set BuildBIOSMap=%2
Set WORKING_PATH=%3
Set BIOS_SVN=%4
Set /a FlashSectionSize=%5
Set BiosGuardName=%6
Set AMI_BIOS_GUARD_FLASH_CONFIGURATIONS=%7
Set BIOS_GUARD_SL2BIN=%WORKING_PATH%\%8
Set BIOS_GUARD_PB=%WORKING_PATH%\%9
echo AMI_BIOS_GUARD_FLASH_CONFIGURATIONS>%AMI_BIOS_GUARD_FLASH_CONFIGURATIONS%
Set BuildBiosMeRom=0
Set BIOSGUARDME_SIZE=0
Set BIOSGUARDME_EC_SIZE=0
Set BEFORE_END_IMAGE_SIZE=0
Set TOTAL_MEUD_ALIGN_SIZE=0
Set /a FWCAPSULE_IMAGE_ALIGN=0
Set /a SIGNATURE_SIZE=256
Set /a ME_ALIGNMENT_SIZE=0
Set /a EC_FW_BIN_ALIGNMENT_SIZE=0

if not exist %FITC_OUTIMAGE_MAP% (
    echo Error:%FITC_OUTIMAGE_MAP% isn't found, please check the eModule of RomImgae
    goto END
)
if not exist %BuildBIOSMap% (
    echo Error:%BuildBIOSMap% isn't found, please check the BiosGuardPubKeysAndBuildPSL.sdl
    goto END
)
if not exist %BiosGuardName% (
    echo %BiosGuardName% isn't found, please check the eModule of RomImgae
    goto END
)
FOR /F "usebackq tokens=1-2" %%G IN ("%BiosGuardName%") DO (
    Set INPUT_BIOS_IMAGE=%%G
    Set OUTPUT_BIOS_IMAGE=%%H
)
if not exist %INPUT_BIOS_IMAGE% (
    echo Error:INPUT_BIOS_IMAGE isn't found, please check the %BiosGuardName%
    goto END
)

Set /a NullValue=0xFFFFFFFF
Set /a DescriptorRegionStartingAddress=%NullValue%
Set /a DescriptorRegionLength=%NullValue%
Set /a PDRRegionStartingAddress=%NullValue%
Set /a PDRRegionLength=%NullValue%
Set /a ECRegionStartingAddress=%NullValue%
Set /a ECRegionLength=%NullValue%
Set /a GbERegionStartingAddress=%NullValue%
Set /a GbERegionLength=%NullValue%
Set /a CSERegionStartingAddress=%NullValue%
Set /a CSERegionLength=%NullValue%
Set /a PaddingRegionStartingAddress=%NullValue%
Set /a PaddingRegionLength=%NullValue%
Set /a BIOSRegionStartingAddress=%NullValue%
Set /a BIOSRegionLength=%NullValue%

FOR /F "skip=2 usebackq tokens=1,2,3,4-7" %%G IN ("%FITC_OUTIMAGE_MAP%") DO (
    if "%%J %%K %%L" EQU "Flash Image " (
        Set /a TOTAL_ROM_SIZE=0x%%I
    )else if "%%J %%K %%L" EQU "Descriptor Region " (
        Set /a DescriptorRegionStartingAddress=0x%%G
        Set /a DescriptorRegionLength=0x1000
    )else if "%%J %%K %%L" EQU "PDR Region " (
        Set /a PDRRegionStartingAddress=0x%%G
        Set /a PDRRegionLength=0x%%I
    )else if "%%J %%K %%L" EQU "EC Region " (
        Set /a ECRegionStartingAddress=0x%%G
        Set /a ECRegionLength=0x%%I
    )else if "%%J %%K %%L" EQU "GBE Region " (
        Set /a GbERegionStartingAddress=0x%%G
        Set /a GbERegionLength=0x%%I
    )else if "%%J %%K %%L" EQU "ME Region " (
        Set /a CSERegionStartingAddress=0x%%G
        Set /a CSERegionLength=0x%%I
    )else if "%%J %%K %%L" EQU "Required Padding (Region" (
        Set /a PaddingRegionStartingAddress=0x%%G
        Set /a PaddingRegionLength=0x%%I
    )else if "%%J %%K %%L" EQU "BIOS Region " (
        Set /a BIOSRegionStartingAddress=0x%%G
        Set /a BIOSRegionLength=0x%%I
    )
)

FOR /F "usebackq tokens=1,2" %%G IN ("Build\BiosGuardVariable.txt") DO (
    if "%%G" EQU "PlatformID" (
        Set BIOS_GUARD_PLAT_ID=%%H
    )else if "%%G" EQU "RecoveryImageSize" (
        Set /a RECOVERY_IMAGE_SIZE= %%H
    )else if "%%G" EQU "MeudBiosSize" (
        Set /a MEUD_BIOS_SIZE= %%H
    )else if "%%G" EQU "MeAlignmentSize" (
        Set /a ME_ALIGNMENT_SIZE= %%H
    )else if "%%G" EQU "EcFwBinAlignmentSize" (
	    Set /a EC_FW_BIN_ALIGNMENT_SIZE= %%H
    )else if "%%G" EQU "EcFwBinPath" (
	    Set EC_FW_BIN_PATH= %%H
    ) else if "%%G" EQU "BiosGuardPriKey" (
        Set PRIVATE_KEY_FILENAME=%%H
    ) else if "%%G" EQU "FwCapsuleImageAlign" (
        Set /a FWCAPSULE_IMAGE_ALIGN= %%H
    ) else if "%%G" EQU "CertficateHdrPath" (
	    Set CERTIFICATE_HEADER_PATH= %%H
    ) else if "%%G" EQU "CertificateAlignSize" (
	    Set CERTIFICATE_ALIGN_SIZE= %%H
    ) else if "%%G" EQU "OriginalRomPath" (
	    Set ORIGINAL_ROM_PATH= %%H
    )
)
goto Define_Flash_Type

:Define_Flash_Type
Set MeType=0
Set BiosType=1
Set EcType=2
Set OthersType=3
goto Prepare_Definitions

:Prepare_Definitions
Set DESCRIPTOR_REGION_ROM=DescriptorRegion.bin
Set PDR_REGION_ROM=PDRRegion.bin
Set EC_REGION_ROM=ECRegion.bin
Set GBE_REGION_ROM=GBERegion.bin 
Set CSE_REGION_ROM=CSERegion.bin
Set PADDING_REGION_ROM=PaddingRegion.bin
Set MAIN_REGION_ROM=BIOS_FV_MAIN.bin
REM Set PRIVATE_KEY_FILENAME=%WORKING_PATH%\BiosGuardPrivate.key
Set BIOS_GUARD_CRYPTCON=%WORKING_PATH%\BiosGuardCryptoCon.exe
Set CUTROM=%WORKING_PATH%\CutRom.exe
Set BIOS_GUARD_SL_FILE=AmiBiosGuardSL
Set BIOS_GUARD_SCRIPT_HDR=%WORKING_PATH%\BiosGuardCertHeader.bin
REM Set BIOS_GUARD_PLAT_ID=SKYLAKE
Set BIOS_GUARD_UPDATEPKG_FILENAME=update_package.BiosGuard
Set BIOS_GUARD_SIGNATURE_FILENAME=.out_PfatCer
Set BIOS_GUARD_INTREGRATION_ROM=AMIBiosGuard.bin
Set AMI_BIOS_GUARD_HEADER=%WORKING_PATH%\AmiBiosGuardHeader.bin
%WORKING_PATH%\BuildAmiBiosGuardHeader.exe /Empty %AMI_BIOS_GUARD_HEADER%
copy %AMI_BIOS_GUARD_HEADER% %BIOS_GUARD_INTREGRATION_ROM%
goto BUILD_DESCRIPTOR_REGION_ROM

:BUILD_DESCRIPTOR_REGION_ROM
    Set ROM_PRIORITY=%DESCRIPTOR_REGION_ROM%
    Set /a FV_StartingAddress=%DescriptorRegionStartingAddress%
    Set /a FV_Length=%DescriptorRegionLength%
    if %FV_StartingAddress% EQU %NullValue% (
	goto BUILD_PDR_REGION_ROM
    )
    if %FV_Length% EQU %NullValue% (
	goto BUILD_PDR_REGION_ROM
    )
    goto SETUP_PARAMETERS

:BUILD_PDR_REGION_ROM
    Set ROM_PRIORITY=%PDR_REGION_ROM%
    Set /a FV_StartingAddress=%PDRRegionStartingAddress%
    Set /a FV_Length=%PDRRegionLength%
    if %FV_StartingAddress% EQU %NullValue% (
	goto BUILD_EC_REGION_ROM
    )
    if %FV_Length% EQU %NullValue% (
	goto BUILD_EC_REGION_ROM
    )
    goto SETUP_PARAMETERS

:BUILD_EC_REGION_ROM
    Set ROM_PRIORITY=%EC_REGION_ROM%
    Set /a FV_StartingAddress=%ECRegionStartingAddress%
    Set /a FV_Length=%ECRegionLength%
    if %FV_StartingAddress% EQU %NullValue% (
	goto BUILD_GBE_REGION_ROM
    )
    if %FV_Length% EQU %NullValue% (
	goto BUILD_GBE_REGION_ROM
    )
    goto SETUP_PARAMETERS

:BUILD_GBE_REGION_ROM
    Set ROM_PRIORITY=%GBE_REGION_ROM%
    Set /a FV_StartingAddress=%GbERegionStartingAddress%
    Set /a FV_Length=%GbERegionLength%
    if %FV_StartingAddress% EQU %NullValue% (
	goto BUILD_CSE_REGION_ROM
    )
    if %FV_Length% EQU %NullValue% (
	goto BUILD_CSE_REGION_ROM
    )
    goto SETUP_PARAMETERS

:BUILD_CSE_REGION_ROM
    Set ROM_PRIORITY=%CSE_REGION_ROM%
    Set /a FV_StartingAddress=%CSERegionStartingAddress%
    Set /a FV_Length=%CSERegionLength%
    if %FV_StartingAddress% EQU %NullValue% (
	goto BUILD_PADDING_REGION_ROM
    )
    if %FV_Length% EQU %NullValue% (
	goto BUILD_PADDING_REGION_ROM
    )
    goto SETUP_PARAMETERS
    
:BUILD_PADDING_REGION_ROM
    Set ROM_PRIORITY=%PADDING_REGION_ROM%
    Set /a FV_StartingAddress=%PaddingRegionStartingAddress%
    Set /a FV_Length=%PaddingRegionLength%
    if %FV_StartingAddress% EQU %NullValue% (
    goto BUILD_BIOS_REGION_ROM_FIRST_FV
    )
    if %FV_Length% EQU %NullValue% (
    goto BUILD_BIOS_REGION_ROM_FIRST_FV
    )
    goto SETUP_PARAMETERS    

:BUILD_BIOS_REGION_ROM_FIRST_FV
    Set /a BIOS_FV_Number=0
    FOR /F "usebackq tokens=1-4" %%G IN ("%BuildBIOSMap%") DO (
        Set ROM_PRIORITY=%%G
        Set FlashCMD=%%H
        Set /a FV_StartingAddress=%BIOSRegionStartingAddress%+%%I
        Set /a FV_Length=%%J
        goto SETUP_PARAMETERS
    )
    goto BuildAmiBiosGuardHeader

:BUILD_BIOS_REGION_ROM_REMAINDER
    Set /a BIOS_FV_Number+=1
    FOR /F "skip=%BIOS_FV_Number% usebackq tokens=1-4" %%G IN ("%BuildBIOSMap%") DO (
        Set ROM_PRIORITY=%%G
        Set FlashCMD=%%H
        Set /a FV_StartingAddress=%BIOSRegionStartingAddress%+%%I
        Set /a FV_Length=%%J
        goto SETUP_PARAMETERS
    )
    goto BuildAmiBiosGuardHeader

:SETUP_PARAMETERS
    %CUTROM% %INPUT_BIOS_IMAGE% %ROM_PRIORITY% %FV_StartingAddress% %FV_Length%
    Set /a HeaderBackupAddress=%FV_StartingAddress%+%FV_Length%-0x1000
    Set /a FlashRemainderSize=%FV_Length%%%FlashSectionSize%
    Set /a SUBTRACT_BLOCK_SIZE_FROM_DATA_SIZE=%FlashSectionSize%-0x1000
    Set /a START_COUNT=0
    Set /a NoRemainderCount=%FV_Length%/%FlashSectionSize%
    if %NoRemainderCount% GTR 0 (
        Set /a NoRemainderCount-=1
        Set /a SKIP_CUT_ROM=1
    ) else (
        Set /a SKIP_CUT_ROM=0
    )
    if %FlashRemainderSize% EQU 0 (
        Set /a END_COUNT=%FV_Length%/%FlashSectionSize%-1
    ) else (
        Set /a END_COUNT=%FV_Length%/%FlashSectionSize%
    )
    Set /a END_OF_SIZE=%FV_StartingAddress%+%FV_Length%
    goto AutoBuildBiosGuardSL

:AutoBuildBiosGuardSL
    Set /a START_FLASH_SECTION=%START_COUNT%*%FlashSectionSize%
    Set /a START_FLASH_SECTION_IN_MB=%START_FLASH_SECTION%/%FlashSectionSize%
    Set FLASH_ROM=AMI_%START_FLASH_SECTION_IN_MB%M.rom
    Set PSL_OUT_FILENAME=AmiBiosGuardPSLOutput_%START_FLASH_SECTION_IN_MB%M.bin
    Set FLASH_ROM_OUTPUT_FILENAME=ABiosGuard%START_FLASH_SECTION_IN_MB%M.bin
    Set /a SUBTRACT_BLOCK_SIZE_FROM_TOTAL_ROM_SIZE=%FV_StartingAddress%
    Set /a CURRENT_SIZE=%START_COUNT%+1
    Set /a CURRENT_SIZE*=%FlashSectionSize%
    Set /a CURRENT_SIZE-=0x1000
    if %SKIP_CUT_ROM% EQU 1 (
         if %NoRemainderCount% GEQ %START_COUNT% (
             %CUTROM% %ROM_PRIORITY% %FLASH_ROM% %START_FLASH_SECTION% %FlashSectionSize%
             Set /a SUBTRACT_BLOCK_SIZE_FROM_TOTAL_ROM_SIZE+=%CURRENT_SIZE%
         ) else (
             %CUTROM% %ROM_PRIORITY% %FLASH_ROM% %START_FLASH_SECTION% %FlashRemainderSize%
             Set /a SUBTRACT_BLOCK_SIZE_FROM_TOTAL_ROM_SIZE=%END_OF_SIZE%-0x1000
             Set /a SUBTRACT_BLOCK_SIZE_FROM_DATA_SIZE=%FlashRemainderSize%-0x1000
        )
    ) else (
         Set FLASH_ROM=%ROM_PRIORITY%
         Set /a SUBTRACT_BLOCK_SIZE_FROM_TOTAL_ROM_SIZE=%END_OF_SIZE%-0x1000
         Set /a SUBTRACT_BLOCK_SIZE_FROM_DATA_SIZE=%FlashRemainderSize%-0x1000
    )
    echo begin > %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo // Set up initial values for update process >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo set F0 %SUBTRACT_BLOCK_SIZE_FROM_TOTAL_ROM_SIZE%; Total ROM size >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo set B0 %SUBTRACT_BLOCK_SIZE_FROM_DATA_SIZE%; Data size >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo set I0 0; the offset count >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo set I1 0x1000; the FLASH BLOCK SIZE >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo set I2 0; Erase error count >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo set I3 0; Write error count >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo set I4 3; Max errors >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo set I5 0; Error Code Buffer >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo set I6 0; Read error count >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    
    if "%ROM_PRIORITY%" EQU "BIOS_FV_MAIN.bin" (        
        if %START_COUNT% EQU 0 (      
            echo.>> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
            echo set F0 %FV_StartingAddress%;  >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
            echo eraseblk F0; >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
            echo set F0 %SUBTRACT_BLOCK_SIZE_FROM_TOTAL_ROM_SIZE%; Total ROM size >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
        )
        if %START_COUNT% EQU %END_COUNT% (                  
            echo.>> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
            echo read B2 F0 I1; >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl            
        )
    )
    echo jmp _read; >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl    

    echo.>> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo _read_retry_label:  >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo add I6 1; increment the read retries count >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo compare I6 I4; compare read retries count to max retries >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo jge _read_error_label >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo jmp _read; if error START_COUNT is smaller than Max errors jump to read>> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl

    echo.>> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo _erase_retry_label:  >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo add I2 1; increment the erase retries count >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo compare I2 I4; compare erase retries count to max retries >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo jge _erase_error_label >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo jmp _erase; if error START_COUNT is smaller than Max errors jump to erase >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl

    echo.>> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo _write_retry_label: >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo add I3 1; increment the write retries count >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo compare I3 I4; compare write retries count to max retries >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo jge _write_error_label >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo jmp _write; if error START_COUNT is smaller than Max errors jump to weite>> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl

    echo.>> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo _loop: >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl

    echo.>> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo _read: >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo read B1 F0 I1; read block 4KB >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo rdsts I5; read the HW status into I5 >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo compare I5 0; >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo jne _read_retry_label; if HW status != 0 jump to retry >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo compare B0 B1 I1; compare B0 vs B1 >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl    
    echo je _skip_erase_and_write; if B0 == B1 skip erase and write >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl  

    echo.>> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo _erase: >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo eraseblk F0; erase block 4KB >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo rdsts I5; read the HW status into I5 >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo compare I5 0; >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo jne _erase_retry_label; if HW status != 0 jump to retry >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl

    echo.>> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo _write: >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo write F0 B0 I1; write block >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo rdsts I5; read the HW status into I5 >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo compare I5 0; >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo jne _write_retry_label; if HW status != 0 jump to retry  >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl

    echo.>> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo _skip_erase_and_write: >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo add I0 I1; increment offset >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl

    echo.>> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo // check if we reached the end of the buffer. We can't increment F0.offset and B0.offset to be more than BIOS size. >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    if %SKIP_CUT_ROM% EQU 1 (
        if %NoRemainderCount% GEQ %START_COUNT% (
            echo compare I0 %FlashSectionSize%; compare count with BIOS image size >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
            echo jge _end; if the count is greater or equal to %FlashSectionSize% then complete  >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl        
        ) else (
            echo compare I0 %FlashRemainderSize%; compare count with BIOS image size >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
            echo jge _end; if the count is greater or equal to %FlashRemainderSize% then complete  >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
        )
    ) else (
        echo compare I0 %FlashRemainderSize%; compare count with BIOS image size >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
        echo jge _end; if the count is greater or equal to %FlashRemainderSize% then complete  >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    )
    echo.>> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo //Sub 4KB to offsets >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo sub F0 I1; Decrement flash offset >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo sub B0 I1; Decrement buffer offset >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo jmp _loop; work on the next block >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl

    echo.>> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo _read_error_label: >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo set If 1; set error in If >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo jmp _end; >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl

    echo.>> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo _erase_error_label: >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo set If 2; set error in If >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo jmp _end; >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl

    echo.>> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo _write_error_label: >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo set If 3; set error in If >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl

    echo.>> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo _end: >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo set If 0; set error = 0 if success >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    
    if "%ROM_PRIORITY%" EQU "%MAIN_REGION_ROM%" (        
        if %START_COUNT% EQU 0 (
            echo.>> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
            echo set F0 %FV_StartingAddress%;  >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
            echo eraseblk F0; >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
            
            echo.>> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
            echo set F0 %HeaderBackupAddress%;  >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
            echo eraseblk F0; >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
            echo write F0 B0 I1; >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
        )
        if %START_COUNT% EQU %END_COUNT% (
            echo.>> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
            echo set F0 %FV_StartingAddress%;  >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
            echo write F0 B2 I1; >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
        )
    )

    echo.>> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl
    echo end >> %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl

    %BIOS_GUARD_SL2BIN%  %BIOS_GUARD_SL_FILE%_%START_FLASH_SECTION_IN_MB%M.psl  %PSL_OUT_FILENAME%
    %BIOS_GUARD_PB% -script %PSL_OUT_FILENAME% -data %FLASH_ROM% -package %BIOS_GUARD_UPDATEPKG_FILENAME% -use_sfam true -bios_svn %BIOS_SVN% -platform_id %BIOS_GUARD_PLAT_ID% -gfx_mitigation_dis true -use_ftu true
    %BIOS_GUARD_CRYPTCON% -z -k %PRIVATE_KEY_FILENAME% -f %BIOS_GUARD_UPDATEPKG_FILENAME% -s
    copy /b %BIOS_GUARD_UPDATEPKG_FILENAME% + %BIOS_GUARD_SCRIPT_HDR% + %BIOS_GUARD_SIGNATURE_FILENAME% %FLASH_ROM_OUTPUT_FILENAME%
    if exist .out_key (
        del .out_key
    )
    del .out_PfatCer
    del %BIOS_GUARD_UPDATEPKG_FILENAME%
    if %START_COUNT% EQU %END_COUNT% goto FinishedAutoBuildBiosGuardSL
    Set /a START_COUNT+=1
    goto AutoBuildBiosGuardSL

:FinishedAutoBuildBiosGuardSL
    Set /a START_COUNT=0
:IntegrateAllROMs
    Set /a START_FLASH_SECTION=%START_COUNT%*%FlashSectionSize%
    Set /a START_FLASH_SECTION_IN_MB=%START_FLASH_SECTION%/%FlashSectionSize%
    copy /b %BIOS_GUARD_INTREGRATION_ROM%+ABiosGuard%START_FLASH_SECTION_IN_MB%M.bin %BIOS_GUARD_INTREGRATION_ROM%
    del ABiosGuard%START_FLASH_SECTION_IN_MB%M.bin
    del AmiBiosGuardPSLOutput_%START_FLASH_SECTION_IN_MB%M.bin
    if exist AMI_%START_FLASH_SECTION_IN_MB%M.rom (
        del AMI_%START_FLASH_SECTION_IN_MB%M.rom
    )
    if %START_COUNT% EQU %END_COUNT% goto FinishedIntegrateAllROMs
    Set /a START_COUNT+=1
    goto IntegrateAllROMs

:FinishedIntegrateAllROMs
    del *.psl
    Set /a END_COUNT+=1
    del %ROM_PRIORITY%
    if "%ROM_PRIORITY%" EQU "%DESCRIPTOR_REGION_ROM%" (
        echo %MeType% /DESC %END_COUNT% ;%ROM_PRIORITY%>>%AMI_BIOS_GUARD_FLASH_CONFIGURATIONS%
        goto BUILD_PDR_REGION_ROM
    )
    if "%ROM_PRIORITY%" EQU "%PDR_REGION_ROM%" (
        echo %MeType% /PDR  %END_COUNT% ;%ROM_PRIORITY%>>%AMI_BIOS_GUARD_FLASH_CONFIGURATIONS%
        goto BUILD_EC_REGION_ROM
    )
    if "%ROM_PRIORITY%" EQU "%EC_REGION_ROM%" (
        echo %EcType% /EC  %END_COUNT% ;%ROM_PRIORITY%>>%AMI_BIOS_GUARD_FLASH_CONFIGURATIONS%
        goto BUILD_GBE_REGION_ROM
    )
    if "%ROM_PRIORITY%" EQU "%GBE_REGION_ROM%" (
        echo %MeType% /GBE  %END_COUNT% ;%ROM_PRIORITY%>>%AMI_BIOS_GUARD_FLASH_CONFIGURATIONS%
        goto BUILD_CSE_REGION_ROM
    )
    if "%ROM_PRIORITY%" EQU "%CSE_REGION_ROM%" (
        echo %MeType% /ME   %END_COUNT% ;%ROM_PRIORITY%>>%AMI_BIOS_GUARD_FLASH_CONFIGURATIONS%
        goto BUILD_PADDING_REGION_ROM
    )
    if "%ROM_PRIORITY%" EQU "%PADDING_REGION_ROM%" (
        echo %MeType% /PAD  %END_COUNT% ;%ROM_PRIORITY%>>%AMI_BIOS_GUARD_FLASH_CONFIGURATIONS%
        goto BUILD_BIOS_REGION_ROM_FIRST_FV
    )
    echo %BiosType% %FlashCMD% %END_COUNT% ;%ROM_PRIORITY%>>%AMI_BIOS_GUARD_FLASH_CONFIGURATIONS%
    goto BUILD_BIOS_REGION_ROM_REMAINDER

:BuildAmiBiosGuardHeader
    if %BuildBiosMeRom% EQU 1 (
        goto BuildBiosGuardMeRom
    )

    %WORKING_PATH%\BuildAmiBiosGuardHeader.exe %AMI_BIOS_GUARD_FLASH_CONFIGURATIONS% %AMI_BIOS_GUARD_HEADER%
    if not exist Build\AmiCrbMeRoms\BiosGuard (
        MKDIR Build\AmiCrbMeRoms\BiosGuard
    )
    copy /b %AMI_BIOS_GUARD_HEADER%+%BIOS_GUARD_INTREGRATION_ROM% Temp.bin
    %WORKING_PATH%\RomAlignment.exe Temp.bin Build\AmiCrbMeRoms\BiosGuard\AMI_BIOS_GUARD_%OUTPUT_BIOS_IMAGE% %RECOVERY_IMAGE_SIZE%
    del Temp.bin

    Set /a BEFORE_END_IMAGE_SIZE=%RECOVERY_IMAGE_SIZE%-%FWCAPSULE_IMAGE_ALIGN%
    
    if %BEFORE_END_IMAGE_SIZE% EQU %RECOVERY_IMAGE_SIZE% (
        goto SignBiosGuardImage
    )
    Set /a BEFORE_END_IMAGE_SIZE-=%SIGNATURE_SIZE%
    %CUTROM% Build\AmiCrbMeRoms\BiosGuard\AMI_BIOS_GUARD_%OUTPUT_BIOS_IMAGE% BeforeEndRom.bin 0 %BEFORE_END_IMAGE_SIZE%
    %CUTROM% %ORIGINAL_ROM_PATH% TempCert.bin 0 %FWCAPSULE_IMAGE_ALIGN%        
    copy /b BeforeEndRom.bin+TempCert.bin Temp.bin
    %WORKING_PATH%\RomAlignment.exe Temp.bin Build\AmiCrbMeRoms\BiosGuard\AMI_BIOS_GUARD_%OUTPUT_BIOS_IMAGE% %RECOVERY_IMAGE_SIZE%
    del BeforeEndRom.bin
    del TempCert.bin
    del Temp.bin

:SignBiosGuardImage
    Set /a SIGN_LENGTH=%RECOVERY_IMAGE_SIZE%-%SIGNATURE_SIZE%
    %CUTROM% Build\AmiCrbMeRoms\BiosGuard\AMI_BIOS_GUARD_%OUTPUT_BIOS_IMAGE% Temp.bin 0 %SIGN_LENGTH%
    %BIOS_GUARD_CRYPTCON% -s -k %PRIVATE_KEY_FILENAME% -f Temp.bin -o signature.bin
    copy /b Temp.bin+signature.bin Build\AmiCrbMeRoms\BiosGuard\AMI_BIOS_GUARD_%OUTPUT_BIOS_IMAGE%
    
    del Temp.bin
    del %AMI_BIOS_GUARD_HEADER%
    del %BIOS_GUARD_INTREGRATION_ROM%
    del signature.bin
    
    if exist Build\AmiCrbMeRoms\BiosGuard\BIOSGUARDME.CAP (
        goto END
    )
    
    if exist Build\AmiCrbMeRoms\BiosGuard\BIOSGUARDME_EC.CAP (
        goto END
    )
    
    if exist Build\AmiCrbMeRoms\BiosGuard\BIOSGUARDEC.CAP (
        goto END
    )
    
    if exist MEFWSIGN.CAP (
        goto BuildMeEC        
    )   
    
    if "%EC_FW_BIN_ALIGNMENT_SIZE%" NEQ "0" (
        goto BuildMeEC        
    )    
REM    goto END 
    
:BuildMeEC    
    Set /a BuildBiosMeRom=1    
    %WORKING_PATH%\BuildAmiBiosGuardHeader.exe /Empty %AMI_BIOS_GUARD_HEADER%
    copy %AMI_BIOS_GUARD_HEADER% %BIOS_GUARD_INTREGRATION_ROM%
    echo AMI_BIOS_GUARD_FLASH_CONFIGURATIONS>%AMI_BIOS_GUARD_FLASH_CONFIGURATIONS%
    goto BUILD_BIOS_REGION_ROM_FIRST_FV
    
:BuildBiosGuardMeRom
    %WORKING_PATH%\BuildAmiBiosGuardHeader.exe %AMI_BIOS_GUARD_FLASH_CONFIGURATIONS% %AMI_BIOS_GUARD_HEADER%
    copy /b %AMI_BIOS_GUARD_HEADER%+%BIOS_GUARD_INTREGRATION_ROM% Temp.bin
    %WORKING_PATH%\RomAlignment.exe Temp.bin Build\AmiCrbMeRoms\BiosGuard\TEMP.CAP %MEUD_BIOS_SIZE%
    
    copy Build\AmiCrbMeRoms\BiosGuard\TEMP.CAP Build\AmiCrbMeRoms\BiosGuard\BIOS.CAP

    REM ================Create the BIOS.cap ===========
    Set /a SIGN_LENGTHA=%MEUD_BIOS_SIZE%-%SIGNATURE_SIZE%
    Set BIN_SELECTION=Build\AmiCrbMeRoms\BiosGuard\BIOS.CAP
    Set BIN2_SELECTION=Build\AmiCrbMeRoms\BiosGuard\BIOS2.CAP
    if "%FWCAPSULE_IMAGE_ALIGN%" EQU "0" (
        echo MEUD_BIOS_SIZE = %MEUD_BIOS_SIZE%
        echo SIGNATURE_SIZE = %SIGNATURE_SIZE%
        echo SIGN_LENTTHA = %SIGN_LENGTHA%
        %CUTROM% %BIN_SELECTION% Temp2.bin 0 %SIGN_LENGTHA%
        %BIOS_GUARD_CRYPTCON% -s -k %PRIVATE_KEY_FILENAME% -f Temp2.bin -o signature.bin
        copy /b Temp2.bin+signature.bin %BIN2_SELECTION%
        del Temp2.bin
        del signature.bin
        
        goto BuildBiosGuardMeRom2
    )
    echo MEUD_BIOS_SIZE2 = %MEUD_BIOS_SIZE%
    echo SIGNATURE_SIZE2 = %SIGNATURE_SIZE%    
    Set /a BEFORE_END_IMAGE_SIZE=%MEUD_BIOS_SIZE%-%FWCAPSULE_IMAGE_ALIGN%-%SIGNATURE_SIZE%
    %CUTROM% %BIN_SELECTION% DIVIDE_END_OF_4K.CAP 0 %BEFORE_END_IMAGE_SIZE%
    %CUTROM% %ORIGINAL_ROM_PATH% TempCert.bin 0 %FWCAPSULE_IMAGE_ALIGN%
    copy /b DIVIDE_END_OF_4K.CAP+TempCert.bin Temp2.bin
    %BIOS_GUARD_CRYPTCON% -s -k %PRIVATE_KEY_FILENAME% -f Temp2.bin -o signature.bin
    copy /b Temp2.bin+signature.bin %BIN2_SELECTION%
    del Temp2.bin
    del DIVIDE_END_OF_4K.CAP
    del TempCert.bin
    del signature.bin

    REM ================
:BuildBiosGuardMeRom2
    if "%ME_ALIGNMENT_SIZE%" EQU "0" (
        del Temp.bin
        del %AMI_BIOS_GUARD_HEADER%
        del %BIOS_GUARD_INTREGRATION_ROM%
        del Build\AmiCrbMeRoms\BiosGuard\TEMP.CAP
        goto Append_EC        
    ) 

    copy /b Build\AmiCrbMeRoms\BiosGuard\TEMP.CAP + MEFWSIGN.CAP Build\AmiCrbMeRoms\BiosGuard\TEMP.CAP
    SET /a BIOSGUARDME_SIZE=%MEUD_BIOS_SIZE%+%ME_ALIGNMENT_SIZE%
    %WORKING_PATH%\RomAlignment.exe Build\AmiCrbMeRoms\BiosGuard\TEMP.CAP Build\AmiCrbMeRoms\BiosGuard\BIOSGUARDME.CAP %BIOSGUARDME_SIZE%
    Set BIN_SELECTION=Build\AmiCrbMeRoms\BiosGuard\BIOSGUARDME.CAP

    del Temp.bin
    del %AMI_BIOS_GUARD_HEADER%
    del %BIOS_GUARD_INTREGRATION_ROM%
    del Build\AmiCrbMeRoms\BiosGuard\TEMP.CAP

    Set /a SIGN_LENGTH=%BIOSGUARDME_SIZE%-%SIGNATURE_SIZE%
    if "%FWCAPSULE_IMAGE_ALIGN%" EQU "0" (
        %CUTROM% %BIN_SELECTION% Temp.bin 0 %SIGN_LENGTH%
        %BIOS_GUARD_CRYPTCON% -s -k %PRIVATE_KEY_FILENAME% -f Temp.bin -o signature.bin
        copy /b Temp.bin+signature.bin %BIN_SELECTION%
        del Temp.bin
        del signature.bin
        
        goto Append_EC
    )
    Set /a BEFORE_END_IMAGE_SIZE=%BIOSGUARDME_SIZE%-%FWCAPSULE_IMAGE_ALIGN%-%SIGNATURE_SIZE%
    %CUTROM% %BIN_SELECTION% DIVIDE_END_OF_4K.CAP 0 %BEFORE_END_IMAGE_SIZE%
    %CUTROM% %ORIGINAL_ROM_PATH% TempCert.bin 0 %FWCAPSULE_IMAGE_ALIGN%
    copy /b DIVIDE_END_OF_4K.CAP+TempCert.bin Temp.bin

    %BIOS_GUARD_CRYPTCON% -s -k %PRIVATE_KEY_FILENAME% -f Temp.bin -o signature.bin
    copy /b Temp.bin+signature.bin %BIN_SELECTION%
    del Temp.bin
    del DIVIDE_END_OF_4K.CAP
    del TempCert.bin
    del signature.bin
  
:Append_EC

    if "%EC_FW_BIN_ALIGNMENT_SIZE%" EQU "0" (
        del Build\AmiCrbMeRoms\BiosGuard\BIOS.CAP
        goto END
    )
    
    if not exist Build\AmiCrbMeRoms\BiosGuard\BIOSGUARDME.CAP (
        goto BuildBiosEc
    )

    SET /a BIOSGUARDME_EC_SIZE=%BIOSGUARDME_SIZE%+%EC_FW_BIN_ALIGNMENT_SIZE%
    copy /b Build\AmiCrbMeRoms\BiosGuard\BIOSGUARDME.CAP + %EC_FW_BIN_PATH% Build\AmiCrbMeRoms\BiosGuard\BIOSGUARDMEEC_TEMP.CAP
    %WORKING_PATH%\RomAlignment.exe Build\AmiCrbMeRoms\BiosGuard\BIOSGUARDMEEC_TEMP.CAP Build\AmiCrbMeRoms\BiosGuard\BIOSGUARDME_EC.CAP %BIOSGUARDME_EC_SIZE%

    del Build\AmiCrbMeRoms\BiosGuard\BIOSGUARDMEEC_TEMP.CAP
    
    Set BIN_SELECTION=Build\AmiCrbMeRoms\BiosGuard\BIOSGUARDME_EC.CAP
    Set /a SIGN_LENGTH=%BIOSGUARDME_EC_SIZE%-%SIGNATURE_SIZE%
    if "%FWCAPSULE_IMAGE_ALIGN%" EQU "0" (
        %CUTROM% %BIN_SELECTION% Temp.bin 0 %SIGN_LENGTH%
        %BIOS_GUARD_CRYPTCON% -s -k %PRIVATE_KEY_FILENAME% -f Temp.bin -o signature.bin
        copy /b Temp.bin+signature.bin %BIN_SELECTION%
        del Temp.bin
        del signature.bin
        
        goto BuildBiosEc
    )
    
    Set /a BEFORE_END_IMAGE_SIZE=%BIOSGUARDME_EC_SIZE%-%FWCAPSULE_IMAGE_ALIGN%-%SIGNATURE_SIZE%
    %CUTROM% %BIN_SELECTION% DIVIDE_END_OF_4K.CAP 0 %BEFORE_END_IMAGE_SIZE%
    %CUTROM% %ORIGINAL_ROM_PATH% TempCert.bin 0 %FWCAPSULE_IMAGE_ALIGN%
    copy /b DIVIDE_END_OF_4K.CAP+TempCert.bin Temp.bin
    %BIOS_GUARD_CRYPTCON% -s -k %PRIVATE_KEY_FILENAME% -f Temp.bin -o signature.bin
    copy /b Temp.bin+signature.bin %BIN_SELECTION%
    del Temp.bin
    del DIVIDE_END_OF_4K.CAP
    del TempCert.bin
    del signature.bin
    
:BuildBiosEc
    SET /a BIOSGUARD_EC_SIZE=%MEUD_BIOS_SIZE%+%EC_FW_BIN_ALIGNMENT_SIZE%    
    copy /b Build\AmiCrbMeRoms\BiosGuard\BIOS.CAP + %EC_FW_BIN_PATH% Build\AmiCrbMeRoms\BiosGuard\BIOSGUARDEC_TEMP.CAP
    %WORKING_PATH%\RomAlignment.exe Build\AmiCrbMeRoms\BiosGuard\BIOSGUARDEC_TEMP.CAP Build\AmiCrbMeRoms\BiosGuard\BIOSGUARDEC.CAP %BIOSGUARD_EC_SIZE%
    del Build\AmiCrbMeRoms\BiosGuard\BIOSGUARDEC_TEMP.CAP
    
    Set BIN_SELECTION=Build\AmiCrbMeRoms\BiosGuard\BIOSGUARDEC.CAP
    Set /a SIGN_LENGTH=%BIOSGUARD_EC_SIZE%-%SIGNATURE_SIZE%
    if "%FWCAPSULE_IMAGE_ALIGN%" EQU "0" (
        %CUTROM% %BIN_SELECTION% Temp.bin 0 %SIGN_LENGTH%
        %BIOS_GUARD_CRYPTCON% -s -k %PRIVATE_KEY_FILENAME% -f Temp.bin -o signature.bin
        copy /b Temp.bin+signature.bin %BIN_SELECTION%
        del Temp.bin
        del signature.bin
        
        del Build\AmiCrbMeRoms\BiosGuard\BIOS.CAP
        goto END
    )
    
    Set /a BEFORE_END_IMAGE_SIZE=%BIOSGUARD_EC_SIZE%-%FWCAPSULE_IMAGE_ALIGN%-%SIGNATURE_SIZE%
    %CUTROM% %BIN_SELECTION% DIVIDE_END_OF_4K.CAP 0 %BEFORE_END_IMAGE_SIZE%
    %CUTROM% %ORIGINAL_ROM_PATH% TempCert.bin 0 %FWCAPSULE_IMAGE_ALIGN%
    copy /b DIVIDE_END_OF_4K.CAP+TempCert.bin Temp.bin
    %BIOS_GUARD_CRYPTCON% -s -k %PRIVATE_KEY_FILENAME% -f Temp.bin -o signature.bin
    copy /b Temp.bin+signature.bin %BIN_SELECTION%
    del Temp.bin
    del DIVIDE_END_OF_4K.CAP
    del TempCert.bin
    del signature.bin
    del Build\AmiCrbMeRoms\BiosGuard\BIOS.CAP
    
:END
    echo !!!! This END !!!
    if exist Build\AmiCrbMeRoms\BiosGuard\BIOS2.CAP (
        copy Build\AmiCrbMeRoms\BiosGuard\BIOS2.CAP Build\AmiCrbMeRoms\BiosGuard\BIOS.CAP
        del Build\AmiCrbMeRoms\BiosGuard\BIOS2.CAP
    )