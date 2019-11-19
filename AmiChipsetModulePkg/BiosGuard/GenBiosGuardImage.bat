echo off
cls

REM Don't change below items
Set BiosGuardBatchConfig=BiosGuardBatchConfig.txt

REM Variable initial
Set BiosImageName=1ATQO.BIN
Set CRB_BOARD=2
Set BIOS_SVN=356352
Set BiosGuardMaxSectionSize=0x100000
Set BIOS_GUARD_SL2BIN=BIOSGuardSL2BIN_PV.2.0.3683.exe
Set BIOS_GUARD_PB=BIOSGuardPB_PV.2.0.3683.exe
Set BiosGuardPriKey=AmiChipsetModulePkg\BiosGuard\BiosGuardKeys\BiosGuardPrivate.key
Set PlatformID=KABYLAKE
Set RecoveryImageSize=16908288
Set MeudBiosSize=6422528
Set MeAlignmentSize=4259840
Set EcFwBinAlignmentSize=327680
Set EcFwBinPath=AmiChipsetModulePkg\BiosGuard\BiosGuardEcFwUpdate\DummyEcFw.bin

echo.
echo ##### AMI build BiosGuard image #####
echo.

REM Change directory to project root
cd ..\..\

REM Check necessary files
if not exist %BiosImageName% (
	echo Error:%BiosImageName% doesn't exist.
	goto END
)

if not exist %BiosGuardPriKey% (
	echo Error:%BiosGuardPriKey% doesn't exist.
	goto END
)

if exist %BiosGuardBatchConfig% (
	echo %BiosGuardBatchConfig% exist. Override default setting.

	FOR /F "usebackq tokens=1,2" %%G IN (%BiosGuardBatchConfig%) DO (
		if "%%G" EQU "BiosImageName" (
			Set BiosImageName=%%H
		) else if "%%G" EQU "CRB_BOARD" (
			Set /a CRB_BOARD=%%H
		) else if "%%G" EQU "BIOS_SVN" (
			Set /a BIOS_SVN=%%H
		) else if "%%G" EQU "BiosGuardMaxSectionSize" (
			Set /a BiosGuardMaxSectionSize=%%H
		) else if "%%G" EQU "BIOS_GUARD_SL2BIN" (
			Set BIOS_GUARD_SL2BIN=%%H
		) else if "%%G" EQU "BIOS_GUARD_PB" (
			Set BIOS_GUARD_PB=%%H
		) else if "%%G" EQU "BiosGuardPriKey" (
			Set BiosGuardPriKey=%%H
		) else if "%%G" EQU "PlatformID" (
			Set PlatformID=%%H
		) else if "%%G" EQU "RecoveryImageSize" (
			Set /a RecoveryImageSize=%%H
		) else if "%%G" EQU "MeudBiosSize" (
			Set /a MeudBiosSize=%%H
		) else if "%%G" EQU "MeAlignmentSize" (
			Set /a MeAlignmentSize=%%H
		) else if "%%G" EQU "EcFwBinAlignmentSize" (
			Set /a EcFwBinAlignmentSize=%%H
		) else if "%%G" EQU "EcFwBinPath" (
			Set EcFwBinPath=%%H
		)
	)
) else if not exist %BiosGuardBatchConfig% (
	echo %BiosGuardBatchConfig% doesn't exist. Use default settings.
	echo If you want to override default settings, please copy AmiChipsetModulePkg\BiosGuard\%BiosGuardBatchConfig% to project root and modify it for your project.
)

REM Print variable value
echo.
echo ##### Variable settings begin #####
echo.
echo BiosImageName           = %BiosImageName%
echo CRB_BOARD               = %CRB_BOARD%
echo BIOS_SVN                = %BIOS_SVN%
echo BiosGuardMaxSectionSize = %BiosGuardMaxSectionSize%
echo BIOS_GUARD_SL2BIN       = %BIOS_GUARD_SL2BIN%
echo BIOS_GUARD_PB           = %BIOS_GUARD_PB%
echo BiosGuardPriKey         = %BiosGuardPriKey%
echo PlatformID              = %PlatformID%
echo RecoveryImageSize       = %RecoveryImageSize%
echo MeudBiosSize            = %MeudBiosSize%
echo MeAlignmentSize         = %MeAlignmentSize%
echo EcFwBinAlignmentSize    = %EcFwBinAlignmentSize%
echo EcFwBinPath             = %EcFwBinPath%
echo.
echo ##### Variable settings end #####
echo.

if exist BiosMap.map (
	echo BiosMap.map exist. Override Build\BiosMap.map.
	echo If you don't want to override BiosMap.map, please remove BiosMap.map from project root folder.
	if not exist Build (
		mkdir Build
	)
	copy /y BiosMap.map Build\BiosMap.map
)

if not exist Build\BiosMap.map (
	echo Error:Build\BiosMap.map doesn't exist.
	echo If you don't know what is BiosMap.map, please re-build your project.
	echo And then copy it to project root folder. 
	goto END
)

echo.
pause

REM 
copy /y %BiosImageName% Build\FITc_BIOS.rom

REM Prepare Build\BiosGuardVariable.txt
echo PlatformID %PlatformID% > Build\BiosGuardVariable.txt
echo RecoveryImageSize %RecoveryImageSize% >> Build\BiosGuardVariable.txt
echo BiosGuardPriKey %BiosGuardPriKey% >> Build\BiosGuardVariable.txt
echo MeudBiosSize %MeudBiosSize% >> Build\BiosGuardVariable.txt
echo MeAlignmentSize %MeAlignmentSize% >> Build\BiosGuardVariable.txt
echo EcFwBinAlignmentSize %EcFwBinAlignmentSize% >> Build\BiosGuardVariable.txt
echo EcFwBinPath %EcFwBinPath% >> Build\BiosGuardVariable.txt

REM Generate full bios image
if %CRB_BOARD% equ 0 ( 	
	
	call RomImage\BatchFiles\Production\Consumer_RVP3_C.bat
	copy /y Build\AmiCrbMeRoms\RVP3_C_Cons_Prod.map Build\AmiCrbMeRoms\BiosGuardMe.map
	echo Build\AmiCrbMeRoms\RVP3_C_Cons_Prod.bin RVP3_C_Cons_Prod.bin > Build\BiosGuardName.txt
    call AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL\BuildPSL.bat Build\AmiCrbMeRoms\BiosGuardMe.map Build\BiosMap.map AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL %BIOS_SVN% %BiosGuardMaxSectionSize% Build\BiosGuardName.txt Build\BiosGuardHeader.ini %BIOS_GUARD_SL2BIN% %BIOS_GUARD_PB%
	del Build\BiosGuardName.txt
	del Build\AmiCrbMeRoms\BiosGuardMe.map
	
	call RomImage\BatchFiles\Production\Corporate_RVP3_C.bat
	copy /y Build\AmiCrbMeRoms\RVP3_C_Corp_Prod.map Build\AmiCrbMeRoms\BiosGuardMe.map
	echo Build\AmiCrbMeRoms\RVP3_C_Corp_Prod.bin RVP3_C_Corp_Prod.bin > Build\BiosGuardName.txt
    call AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL\BuildPSL.bat Build\AmiCrbMeRoms\BiosGuardMe.map Build\BiosMap.map AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL %BIOS_SVN% %BiosGuardMaxSectionSize% Build\BiosGuardName.txt Build\BiosGuardHeader.ini %BIOS_GUARD_SL2BIN% %BIOS_GUARD_PB%
	del Build\BiosGuardName.txt
	del Build\AmiCrbMeRoms\BiosGuardMe.map
	
	goto AfterGenFullBiosImage
)
 
if %CRB_BOARD% equ 1 ( 

	call RomImage\BatchFiles\Production\Consumer_RVP5_C.bat
	copy /y Build\AmiCrbMeRoms\RVP5_C_Cons_Prod.map Build\AmiCrbMeRoms\BiosGuardMe.map
	echo Build\AmiCrbMeRoms\RVP5_C_Cons_Prod.bin RVP5_C_Cons_Prod.bin > Build\BiosGuardName.txt
    call AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL\BuildPSL.bat Build\AmiCrbMeRoms\BiosGuardMe.map Build\BiosMap.map AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL %BIOS_SVN% %BiosGuardMaxSectionSize% Build\BiosGuardName.txt Build\BiosGuardHeader.ini %BIOS_GUARD_SL2BIN% %BIOS_GUARD_PB%
	del Build\BiosGuardName.txt
	del Build\AmiCrbMeRoms\BiosGuardMe.map
	
	call RomImage\BatchFiles\Production\Corporate_RVP5_C.bat
	copy /y Build\AmiCrbMeRoms\RVP5_C_Corp_Prod.map Build\AmiCrbMeRoms\BiosGuardMe.map
	echo Build\AmiCrbMeRoms\RVP5_C_Corp_Prod.bin RVP5_C_Corp_Prod.bin > Build\BiosGuardName.txt
    call AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL\BuildPSL.bat Build\AmiCrbMeRoms\BiosGuardMe.map Build\BiosMap.map AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL %BIOS_SVN% %BiosGuardMaxSectionSize% Build\BiosGuardName.txt Build\BiosGuardHeader.ini %BIOS_GUARD_SL2BIN% %BIOS_GUARD_PB%
	del Build\BiosGuardName.txt
	del Build\AmiCrbMeRoms\BiosGuardMe.map
	
	goto AfterGenFullBiosImage
)

if %CRB_BOARD% equ 2 ( 

	call RomImage\BatchFiles\Production\Consumer_RVP7_C.bat
	copy /y Build\AmiCrbMeRoms\RVP7_C_Cons_Prod.map Build\AmiCrbMeRoms\BiosGuardMe.map
	echo Build\AmiCrbMeRoms\RVP7_C_Cons_Prod.bin RVP7_C_Cons_Prod.bin > Build\BiosGuardName.txt
    call AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL\BuildPSL.bat Build\AmiCrbMeRoms\BiosGuardMe.map Build\BiosMap.map AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL %BIOS_SVN% %BiosGuardMaxSectionSize% Build\BiosGuardName.txt Build\BiosGuardHeader.ini %BIOS_GUARD_SL2BIN% %BIOS_GUARD_PB%
	del Build\BiosGuardName.txt
	del Build\AmiCrbMeRoms\BiosGuardMe.map
	
	call RomImage\BatchFiles\Production\Corporate_RVP7_C.bat
	copy /y Build\AmiCrbMeRoms\RVP7_C_Corp_Prod.map Build\AmiCrbMeRoms\BiosGuardMe.map
	echo Build\AmiCrbMeRoms\RVP7_C_Corp_Prod.bin RVP7_C_Corp_Prod.bin > Build\BiosGuardName.txt
    call AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL\BuildPSL.bat Build\AmiCrbMeRoms\BiosGuardMe.map Build\BiosMap.map AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL %BIOS_SVN% %BiosGuardMaxSectionSize% Build\BiosGuardName.txt Build\BiosGuardHeader.ini %BIOS_GUARD_SL2BIN% %BIOS_GUARD_PB%
	del Build\BiosGuardName.txt
	del Build\AmiCrbMeRoms\BiosGuardMe.map
	
    goto AfterGenFullBiosImage
)
if %CRB_BOARD% equ 3 ( 
	
	call RomImage\BatchFiles\Production\Consumer_RVP8_D.bat
	copy /y Build\AmiCrbMeRoms\RVP8_D_Cons_Prod.map Build\AmiCrbMeRoms\BiosGuardMe.map
	echo Build\AmiCrbMeRoms\RVP8_D_Cons_Prod.bin RVP8_D_Cons_Prod.bin > Build\BiosGuardName.txt
    call AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL\BuildPSL.bat Build\AmiCrbMeRoms\BiosGuardMe.map Build\BiosMap.map AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL %BIOS_SVN% %BiosGuardMaxSectionSize% Build\BiosGuardName.txt Build\BiosGuardHeader.ini %BIOS_GUARD_SL2BIN% %BIOS_GUARD_PB%
	del Build\BiosGuardName.txt
	del Build\AmiCrbMeRoms\BiosGuardMe.map
	
	call RomImage\BatchFiles\Production\Corporate_RVP8_D.bat
	copy /y Build\AmiCrbMeRoms\RVP8_D_Corp_Prod.map Build\AmiCrbMeRoms\BiosGuardMe.map
	echo Build\AmiCrbMeRoms\RVP8_D_Corp_Prod.bin RVP8_D_Corp_Prod.bin> Build\BiosGuardName.txt
    call AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL\BuildPSL.bat Build\AmiCrbMeRoms\BiosGuardMe.map Build\BiosMap.map AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL %BIOS_SVN% %BiosGuardMaxSectionSize% Build\BiosGuardName.txt Build\BiosGuardHeader.ini %BIOS_GUARD_SL2BIN% %BIOS_GUARD_PB%
	del Build\BiosGuardName.txt
	del Build\AmiCrbMeRoms\BiosGuardMe.map
	
	goto AfterGenFullBiosImage
)
if %CRB_BOARD% equ 4 ( 

	call RomImage\BatchFiles\Production\Consumer_RVP9_D.bat
	copy /y Build\AmiCrbMeRoms\RVP9_D_Cons_Prod.map Build\AmiCrbMeRoms\BiosGuardMe.map
	echo Build\AmiCrbMeRoms\RVP9_D_Cons_Prod.bin RVP9_D_Cons_Prod.bin > Build\BiosGuardName.txt
    call AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL\BuildPSL.bat Build\AmiCrbMeRoms\BiosGuardMe.map Build\BiosMap.map AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL %BIOS_SVN% %BiosGuardMaxSectionSize% Build\BiosGuardName.txt Build\BiosGuardHeader.ini %BIOS_GUARD_SL2BIN% %BIOS_GUARD_PB%
	del Build\BiosGuardName.txt
	del Build\AmiCrbMeRoms\BiosGuardMe.map
	
	call RomImage\BatchFiles\Production\Corporate_RVP9_D.bat
	copy /y Build\AmiCrbMeRoms\RVP9_D_Corp_Prod.map Build\AmiCrbMeRoms\BiosGuardMe.map
	echo Build\AmiCrbMeRoms\RVP9_D_Corp_Prod.bin RVP9_D_Corp_Prod.bin> Build\BiosGuardName.txt
    call AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL\BuildPSL.bat Build\AmiCrbMeRoms\BiosGuardMe.map Build\BiosMap.map AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL %BIOS_SVN% %BiosGuardMaxSectionSize% Build\BiosGuardName.txt Build\BiosGuardHeader.ini %BIOS_GUARD_SL2BIN% %BIOS_GUARD_PB%
	del Build\BiosGuardName.txt
	del Build\AmiCrbMeRoms\BiosGuardMe.map
	
	goto AfterGenFullBiosImage
)
if %CRB_BOARD% equ 5 ( 

	call RomImage\BatchFiles\Production\Consumer_RVP10_D.bat
	copy /y Build\AmiCrbMeRoms\RVP10_D_Cons_Prod.map Build\AmiCrbMeRoms\BiosGuardMe.map
	echo Build\AmiCrbMeRoms\RVP10_D_Cons_Prod.bin RVP10_D_Cons_Prod.bin > Build\BiosGuardName.txt
    call AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL\BuildPSL.bat Build\AmiCrbMeRoms\BiosGuardMe.map Build\BiosMap.map AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL %BIOS_SVN% %BiosGuardMaxSectionSize% Build\BiosGuardName.txt Build\BiosGuardHeader.ini %BIOS_GUARD_SL2BIN% %BIOS_GUARD_PB%
	del Build\BiosGuardName.txt
	del Build\AmiCrbMeRoms\BiosGuardMe.map
	
	call RomImage\BatchFiles\Production\Corporate_RVP10_D.bat
	copy /y Build\AmiCrbMeRoms\RVP10_D_Corp_Prod.map Build\AmiCrbMeRoms\BiosGuardMe.map
	echo Build\AmiCrbMeRoms\RVP10_D_Corp_Prod.bin RVP10_D_Corp_Prod.bin> Build\BiosGuardName.txt
    call AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL\BuildPSL.bat Build\AmiCrbMeRoms\BiosGuardMe.map Build\BiosMap.map AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL %BIOS_SVN% %BiosGuardMaxSectionSize% Build\BiosGuardName.txt Build\BiosGuardHeader.ini %BIOS_GUARD_SL2BIN% %BIOS_GUARD_PB%
	del Build\BiosGuardName.txt
	del Build\AmiCrbMeRoms\BiosGuardMe.map
	
	goto AfterGenFullBiosImage
)
if %CRB_BOARD% equ 7 ( 

	call RomImage\BatchFiles\Production\Consumer_RVP11_D.bat
	copy /y Build\AmiCrbMeRoms\RVP11_D_Cons_Prod.map Build\AmiCrbMeRoms\BiosGuardMe.map
	echo Build\AmiCrbMeRoms\RVP11_D_Cons_Prod.bin RVP11_D_Cons_Prod.bin > Build\BiosGuardName.txt
    call AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL\BuildPSL.bat Build\AmiCrbMeRoms\BiosGuardMe.map Build\BiosMap.map AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL %BIOS_SVN% %BiosGuardMaxSectionSize% Build\BiosGuardName.txt Build\BiosGuardHeader.ini %BIOS_GUARD_SL2BIN% %BIOS_GUARD_PB%
	del Build\BiosGuardName.txt
	del Build\AmiCrbMeRoms\BiosGuardMe.map
	
	call RomImage\BatchFiles\Production\Corporate_RVP11_D.bat
	copy /y Build\AmiCrbMeRoms\RVP11_D_Corp_Prod.map Build\AmiCrbMeRoms\BiosGuardMe.map
	echo Build\AmiCrbMeRoms\RVP11_D_Corp_Prod.bin RVP11_D_Corp_Prod.bin > Build\BiosGuardName.txt
    call AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL\BuildPSL.bat Build\AmiCrbMeRoms\BiosGuardMe.map Build\BiosMap.map AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL %BIOS_SVN% %BiosGuardMaxSectionSize% Build\BiosGuardName.txt Build\BiosGuardHeader.ini %BIOS_GUARD_SL2BIN% %BIOS_GUARD_PB%
	del Build\BiosGuardName.txt
	del Build\AmiCrbMeRoms\BiosGuardMe.map
	
	goto AfterGenFullBiosImage
)
if %CRB_BOARD% equ 9 ( 

	call RomImage\BatchFiles\Production\Consumer_KBL_uDIMM_D.bat
	copy /y Build\AmiCrbMeRoms\KBL_uDIMM_D_Cons_Prod.map Build\AmiCrbMeRoms\BiosGuardMe.map
	echo Build\AmiCrbMeRoms\KBL_uDIMM_D_Cons_Prod.bin KBL_uDIMM_D_Cons_Prod.bin  > Build\BiosGuardName.txt
    call AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL\BuildPSL.bat Build\AmiCrbMeRoms\BiosGuardMe.map Build\BiosMap.map AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL %BIOS_SVN% %BiosGuardMaxSectionSize% Build\BiosGuardName.txt Build\BiosGuardHeader.ini %BIOS_GUARD_SL2BIN% %BIOS_GUARD_PB%
	del Build\BiosGuardName.txt
	del Build\AmiCrbMeRoms\BiosGuardMe.map
	
	call RomImage\BatchFiles\Production\Corporate_KBL_uDIMM_D.bat
	copy /y Build\AmiCrbMeRoms\KBL_uDIMM_D_Corp_Prod.map Build\AmiCrbMeRoms\BiosGuardMe.map
	echo Build\AmiCrbMeRoms\KBL_uDIMM_D_Corp_Prod.bin KBL_uDIMM_D_Corp_Prod.bin > Build\BiosGuardName.txt
    call AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL\BuildPSL.bat Build\AmiCrbMeRoms\BiosGuardMe.map Build\BiosMap.map AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL %BIOS_SVN% %BiosGuardMaxSectionSize% Build\BiosGuardName.txt Build\BiosGuardHeader.ini %BIOS_GUARD_SL2BIN% %BIOS_GUARD_PB%
	del Build\BiosGuardName.txt
	del Build\AmiCrbMeRoms\BiosGuardMe.map
	
	call RomImage\BatchFiles\Production\Consumer_KBL_Z370_D.bat
	copy /y Build\AmiCrbMeRoms\KBL_Z370_D_Cons_Prod.map Build\AmiCrbMeRoms\BiosGuardMe.map
	echo Build\AmiCrbMeRoms\KBL_Z370_D_Cons_Prod.bin KBL_Z370_D_Cons_Prod.bin  > Build\BiosGuardName.txt
    call AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL\BuildPSL.bat Build\AmiCrbMeRoms\BiosGuardMe.map Build\BiosMap.map AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL %BIOS_SVN% %BiosGuardMaxSectionSize% Build\BiosGuardName.txt Build\BiosGuardHeader.ini %BIOS_GUARD_SL2BIN% %BIOS_GUARD_PB%
	del Build\BiosGuardName.txt
	del Build\AmiCrbMeRoms\BiosGuardMe.map
	
	call RomImage\BatchFiles\Production\Corporate_KBL_Z370_D.bat
	copy /y Build\AmiCrbMeRoms\KBL_Z370_D_Corp_Prod.map Build\AmiCrbMeRoms\BiosGuardMe.map
	echo Build\AmiCrbMeRoms\KBL_Z370_D_Corp_Prod.bin KBL_Z370_D_Corp_Prod.bin > Build\BiosGuardName.txt
    call AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL\BuildPSL.bat Build\AmiCrbMeRoms\BiosGuardMe.map Build\BiosMap.map AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL %BIOS_SVN% %BiosGuardMaxSectionSize% Build\BiosGuardName.txt Build\BiosGuardHeader.ini %BIOS_GUARD_SL2BIN% %BIOS_GUARD_PB%
	del Build\BiosGuardName.txt
	del Build\AmiCrbMeRoms\BiosGuardMe.map
	
	goto AfterGenFullBiosImage
)
if %CRB_BOARD% equ 11 ( 

	call RomImage\BatchFiles\Production\Consumer_KBL_Refresh_C.bat
	copy /y Build\AmiCrbMeRoms\KBL_Refresh_C_Cons_Prod.map Build\AmiCrbMeRoms\BiosGuardMe.map
	echo Build\AmiCrbMeRoms\KBL_Refresh_C_Cons_Prod.bin KBL_Refresh_C_Cons_Prod.bin  > Build\BiosGuardName.txt
    call AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL\BuildPSL.bat Build\AmiCrbMeRoms\BiosGuardMe.map Build\BiosMap.map AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL %BIOS_SVN% %BiosGuardMaxSectionSize% Build\BiosGuardName.txt Build\BiosGuardHeader.ini %BIOS_GUARD_SL2BIN% %BIOS_GUARD_PB%
	del Build\BiosGuardName.txt
	del Build\AmiCrbMeRoms\BiosGuardMe.map
	
	call RomImage\BatchFiles\Production\Corporate_KBL_Refresh_C.bat
	copy /y Build\AmiCrbMeRoms\KBL_Refresh_C_Corp_Prod.map Build\AmiCrbMeRoms\BiosGuardMe.map
	echo Build\AmiCrbMeRoms\KBL_Refresh_C_Corp_Prod.bin KBL_Refresh_C_Corp_Prod.bin > Build\BiosGuardName.txt
    call AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL\BuildPSL.bat Build\AmiCrbMeRoms\BiosGuardMe.map Build\BiosMap.map AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL %BIOS_SVN% %BiosGuardMaxSectionSize% Build\BiosGuardName.txt Build\BiosGuardHeader.ini %BIOS_GUARD_SL2BIN% %BIOS_GUARD_PB%
	del Build\BiosGuardName.txt
	del Build\AmiCrbMeRoms\BiosGuardMe.map
	
	goto AfterGenFullBiosImage
)
if %CRB_BOARD% equ 12 ( 

    call RomImage\BatchFiles\Production\Consumer_RVP17_D.bat
    copy /y Build\AmiCrbMeRoms\RVP17_D_Cons_Prod.map Build\AmiCrbMeRoms\BiosGuardMe.map
    echo Build\AmiCrbMeRoms\RVP17_D_Cons_Prod.bin RVP17_D_Cons_Prod.bin  > Build\BiosGuardName.txt
    call AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL\BuildPSL.bat Build\AmiCrbMeRoms\BiosGuardMe.map Build\BiosMap.map AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL %BIOS_SVN% %BiosGuardMaxSectionSize% Build\BiosGuardName.txt Build\BiosGuardHeader.ini %BIOS_GUARD_SL2BIN% %BIOS_GUARD_PB%
    del Build\BiosGuardName.txt
    del Build\AmiCrbMeRoms\BiosGuardMe.map
    
    call RomImage\BatchFiles\Production\Corporate_RVP17_D.bat
    copy /y Build\AmiCrbMeRoms\RVP17_D_Corp_Prod.bin.map Build\AmiCrbMeRoms\BiosGuardMe.map
    echo Build\AmiCrbMeRoms\RVP17_D_Corp_Prod.bin RVP17_D_Corp_Prod.bin > Build\BiosGuardName.txt
    call AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL\BuildPSL.bat Build\AmiCrbMeRoms\BiosGuardMe.map Build\BiosMap.map AmiChipsetModulePkg\BiosGuard\BiosGuardPubKeysAndBuildPSL %BIOS_SVN% %BiosGuardMaxSectionSize% Build\BiosGuardName.txt Build\BiosGuardHeader.ini %BIOS_GUARD_SL2BIN% %BIOS_GUARD_PB%
    del Build\BiosGuardName.txt
    del Build\AmiCrbMeRoms\BiosGuardMe.map
    
    goto AfterGenFullBiosImage
)
cd ..\..\
echo Error:Bad CRB_BOARD number(%CRB_BOARD%)
goto END
:AfterGenFullBiosImage


:END
cd AmiChipsetModulePkg\BiosGuard