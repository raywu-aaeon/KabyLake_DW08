@rem PORTING NOTE
@rem ===========================================
@rem * Select prefered build server in .gitlab-ci.yml. Ex: [Tag: BIOS] use all; [Tag: BIOS1] use BIOS1 only
@rem * Set variables %1 %2 from the script section in .gitlab-ci.yml:
@rem *   %1 BUILD_ALL=1: Build all; 0: Build; 2: Skip Build process[Test Only]
@rem *   %2 SHA_FOLDER=1: Create SHA and Output folder; 0: Create Output folder only
@rem * Set VEB file name (w/o extension). Ex: [VEB=CoffeeLake_BOXER-6842M]
@rem * Set VEB tool version. Ex: [AptioV_DIR=Aptio_5.x_TOOLS_32_1]
@rem * (Optional) Set output path for copying output file. Ex: [OUTPUT_DIR=\\AA-LK11-1682\Share]
@rem * (Optional) Set user name for accessing share folder. Ex: [USERNAME=username@aaeon.com.tw]
@rem * (Optional) Set password for accessing share folder. Ex: [PASSWORD=########]
@rem * (Optional) Set output folder for copying output file. Ex: [OUTPUT_FOLDER=CoffeeLake_BOXER-6842M]

echo.
echo *******************************************
echo AaeonRemoteBuild.bat Start
echo *******************************************
echo.
echo Set Build Environment Parameters
echo -------------------------------------------
@set BUILD_ALL=%1
@set DEDIPROG_DIR=C:\Program Files (x86)\DediProg\SF Programmer

:SetVebParm
@set VEB=Kabylake
@rem BUILD_WITH_BATCH : For projects use build.bat instead of make
@set BUILD_WITH_BATCH=0
@set AptioV_DIR=Aptio_5.x_TOOLS_33
@rem Veb Support List:
@rem 4.5.5_TOOLS_41
@rem Aptio_5.x_TOOLS_24_6
@rem Aptio_5.x_TOOLS_27
@rem Aptio_5.x_TOOLS_28
@rem Aptio_5.x_TOOLS_30_7
@rem Aptio_5.x_TOOLS_31_1
@rem Aptio_5.x_TOOLS_32_1
@rem Aptio_5.x_TOOLS_34_1
@rem Aptio_5.x_TOOLS_35
@rem Aptio_5.x_TOOLS_35_1
@rem Aptio_5.x_TOOLS_35_2
@rem Aptio_5.x_TOOLS_37_1
@rem Aptio_5.x_TOOLS_37_1_x64
@rem Aptio_5.x_TOOLS_JRE_34_1
@set VEB_DIR=C:\gitlab-runner
@set OUTPUT_FOLDER=runner-build\%CD:~14%

:SetVebToolsDirectories
@set TOOLS_DIR=%VEB_DIR%\%AptioV_DIR%\BuildTools
@set WINDDK_DIR1=%VEB_DIR%\WINDDK\7600.16385.1\bin\x86\x86
@set WINDDK_DIR2=%VEB_DIR%\WINDDK\7600.16385.1\bin\x86
@set EWDK_DIR=%VEB_DIR%\EWDK_1703
@rem EWDK_DIR=%VEB_DIR%\EWDK_19h1
@set path=%EWDK_DIR%;%WINDDK_DIR1%;%WINDDK_DIR2%;%TOOLS_DIR%;%path%

:SetVebEnvVariables
@set CCX86DIR=%VEB_DIR%\WINDDK\7600.16385.1\bin\x86\x86
@set CCX64DIR=%VEB_DIR%\WINDDK\7600.16385.1\bin\x86\amd64

echo.
echo Set deploy Environment Parameters
echo -------------------------------------------

:SetOutputPath
@set NET_IP=172.16.1.26
@set OUTPUT_DIR=\\%NET_IP%\public
@set OUTPUT_FOLDER=%CD:~14%
@set USERNAME=runner
@set PASSWORD=12345678
@ping %NET_IP% -n 1 -w 100

:ConnectOuputDir
if ERRORLEVEL 1 (
    @set OUTPUT_DIR=%USERPROFILE%\AAEON Technology\nas - gitlab_runner
) else (
    rem connect output directory
    @net use %OUTPUT_DIR% /user:%USERNAME% %PASSWORD%
    @set NET_USE=1
    @set OUTPUT_FOLDER=runner-build\%OUTPUT_FOLDER%
)
@set OUTPUT_PATH=%OUTPUT_DIR%\%OUTPUT_FOLDER%

:SetShaFolder
@set SHA_FOLDER=%2
if "%SHA_FOLDER%" == "1" (
    @for /f %%i in ('git rev-parse HEAD') do set SHA=%%i
) else (
    goto StartBuild
)
@set OUTPUT_PATH=%OUTPUT_DIR%\%OUTPUT_FOLDER%\%SHA%

:StartBuild
echo.
echo Starting Build Process
echo -------------------------------------------
@set OLDTIME=%time%

@if "%BUILD_ALL%" == "2" goto BuildSucess
@if "%BUILD_ALL%" == "0" goto Build

:ReBuildAll
if "%BUILD_WITH_BATCH%" == "1" (
call Build.bat rebuild
@echo off
) else (
make clean
call make rebuild
)
if ERRORLEVEL 1 goto BuildFailed
goto BuildSucess

:Build
if "%BUILD_WITH_BATCH%" == "1" (
call Build.bat
@echo off
) else (
call make
)
if ERRORLEVEL 1 goto BuildFailed
goto BuildSucess

:BuildFailed
@set ReportError=1

:BuildSucess

FOR /F "usebackq tokens=1-3" %%I IN (Build\Token.mak) DO (
	if "%%I" EQU "AMI_FSP_BIN_BUILD_TEMP_DIR" (
		Set AmiFspBinBuildTempDir=%%K
	)
)

echo.
echo Copying Output Files
echo -------------------------------------------

:CreateOuputFolder
if exist "%OUTPUT_PATH%\" (
    @del "%OUTPUT_PATH%\*" /Q
    @rmdir "%OUTPUT_PATH%\Build" /S /Q
    @rmdir "%OUTPUT_PATH%\%AmiFspBinBuildTempDir%" /S /Q
)
@mkdir "%OUTPUT_PATH%\Build"
@mkdir "%OUTPUT_PATH%\%AmiFspBinBuildTempDir%"
@mkdir "%OUTPUT_PATH%\Build\AmiCrbMeRoms"

:CopyBuildFiles
@copy *.bin "%OUTPUT_PATH%" /y /b
@copy _Aaeon_Release\*.bin "%OUTPUT_PATH%" /y /b
@copy Build\Token.* "%OUTPUT_PATH%\Build" /y /b
@copy Build\Platform.* "%OUTPUT_PATH%\Build" /y /b
@copy Build\GSIOSETUPVAR.* "%OUTPUT_PATH%\Build" /y /b
@copy Build\*.inf "%OUTPUT_PATH%\Build" /y /b
@copy Build\PCIBRDSDL.* "%OUTPUT_PATH%\Build" /y /b
@copy Build\SIOBRDSDL.* %OUTPUT_PATH%\Build /y /b
@copy Build\*.hfr "%OUTPUT_PATH%\Build" /y /b
@copy Build\*.vfr %OUTPUT_PATH%\Build /y /b
@copy Build\*.asl "%OUTPUT_PATH%\Build" /y /b
@copy Build\*.uni "%OUTPUT_PATH%\Build" /y /b
@copy Build\*.sd "%OUTPUT_PATH%\Build" /y /b
@copy Build\*.mak "%OUTPUT_PATH%\Build" /y /b
@copy Build\*.h %OUTPUT_PATH%\Build /y /b
@copy Build\AmiCrbMeRoms\*.* %OUTPUT_PATH%\Build\AmiCrbMeRoms /y /b
@copy %AmiFspBinBuildTempDir%\Token.* %OUTPUT_PATH%\%AmiFspBinBuildTempDir% /y /b
@copy %AmiFspBinBuildTempDir%\Platform.* %OUTPUT_PATH%\%AmiFspBinBuildTempDir% /y /b
@copy %AmiFspBinBuildTempDir%\*.mak %OUTPUT_PATH%\%AmiFspBinBuildTempDir% /y /b
rem @copy build.log %OUTPUT_PATH% /y /b

echo.
echo Create DediProg batch file
echo -------------------------------------------
FOR /F "tokens=2,3" %%i IN (Build\Token.h) DO (
    if "%%i" == "AAEON_BIN" Set FLASH_BIN=%%j
)
echo BIOS BIN file = %FLASH_BIN%
@echo "%DEDIPROG_DIR%\Dpcmd" --spi-clk 0 -z %OUTPUT_PATH%\%FLASH_BIN% > Batch.bat
@echo pause >> Batch.bat
@copy Batch.bat %OUTPUT_PATH%\Batch.bat /y /b

:DeleteConnection
if "%NET_USE%" == "1" (
    @net use %OUTPUT_DIR% /delete
)

echo.
echo Output files have been put to :
echo %OUTPUT_PATH%

@if "%ReportError%" == "1" goto FinishedWithFailure

:Finished
echo.
echo *******************************************
echo AaeonRemoteBuild.bat Finished
echo *******************************************
@echo Start Time %OLDTIME%
@echo End   Time %time%
EXIT 0

:FinishedWithFailure
echo. 
echo *******************************************
echo Build Failed
echo *******************************************
@echo Start Time %OLDTIME%
@echo End   Time %time%
EXIT 1