@rem PORTING NOTE
@rem ===========================================
@rem * Set variables %1 %2 from the script section in .gitlab-ci.yml:
@rem * %1 BUILD_ALL=1: Build all; 0: Build; 2: Skip Build process[Test Only]
@rem * %2 SHA_FOLDER=1: Create SHA and Output folder; 0: Create Output folder only
@rem * Set VEB directory. Ex: [VEB=CoffeeLake_BOXER-6842M]
@rem * Set VEB version. Ex: [AptioV_DIR=Aptio_5.x_TOOLS_32_1]
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
@set SHA_FOLDER=%2
@set OUTPUT_DIR=\\AA-LA11-1140B\_RemoteBuildFiles
@set USERNAME=raywu@aaeon.com.tw
@set PASSWORD=Aaeon1629x

:SetVebParm
@set VEB=Kabylake
@set VEB_BUILD_MODULE=
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
@rem Aptio_5.x_TOOLS_JRE_34_1
@set VEB_DIR=C:\gitlab-runner
@set OUTPUT_FOLDER=%VEB%

@if "%SHA_FOLDER%" == "1" goto SetShaFolder

:SetOutputPath
@set OUTPUT_PATH=%OUTPUT_DIR%\%OUTPUT_FOLDER%
@goto SetVebToolsDirectories

:SetShaFolder
@for /f %%i in ('git rev-parse HEAD') do set SHA=%%i
@set OUTPUT_PATH=%OUTPUT_DIR%\%OUTPUT_FOLDER%\%SHA%

:SetVebToolsDirectories
@set TOOLS_DIR=%VEB_DIR%\%AptioV_DIR%\BuildTools
@set WINDDK_DIR1=%VEB_DIR%\WINDDK\7600.16385.1\bin\x86\x86
@set WINDDK_DIR2=%VEB_DIR%\WINDDK\7600.16385.1\bin\x86
@set path=%WINDDK_DIR1%;%WINDDK_DIR2%;%TOOLS_DIR%;%path%

:SetVebEnvVariables
@set CCX86DIR=%VEB_DIR%\WINDDK\7600.16385.1\bin\x86\x86
@set CCX64DIR=%VEB_DIR%\WINDDK\7600.16385.1\bin\x86\amd64

echo.
echo Starting Build Process
echo -------------------------------------------
@set OLDTIME=%time%

@if "%BUILD_ALL%" == "2" goto BuildSucess
@if "%BUILD_ALL%" == "0" goto Build

:ReBuildAll
make clean
call make rebuild
if ERRORLEVEL 1 goto BuildFailed
goto BuildSucess

:Build
call make
if ERRORLEVEL 1 goto BuildFailed
goto BuildSucess

:BuildFailed
@set ReportError=1

:BuildSucess
@set NEWTIME=%time%
@echo Start Time %OLDTIME%
@echo End   Time %NEWTIME%


echo.
echo Copying Output Files
echo -------------------------------------------
:ConnectOuputDir
@net use %OUTPUT_DIR% /user:%USERNAME% %PASSWORD%

:CreateOuputFolder
@del %OUTPUT_PATH%\*.* /Q
if not exist "%OUTPUT_PATH%\" mkdir %OUTPUT_PATH%\
if not exist "%OUTPUT_PATH%\Build" mkdir %OUTPUT_PATH%\Build\

:CopyBuildFiles
@copy Build\AmiCrbMeRoms\*.bin %OUTPUT_PATH% /y /b
@copy Build\Token.* %OUTPUT_PATH%\Build /y /b
@copy Build\Platform.* %OUTPUT_PATH%\Build /y /b
@copy Build\GSIOSETUPVAR.* %OUTPUT_PATH%\Build /y /b
@copy Build\*.inf %OUTPUT_PATH%\Build /y /b
@copy Build\PCIBRDSDL.* %OUTPUT_PATH%\Build /y /b
@copy Build\SIOBRDSDL.* %OUTPUT_PATH%\Build /y /b
@copy Build\*.hfr %OUTPUT_PATH%\Build /y /b
@copy Build\*.vfr %OUTPUT_PATH%\Build /y /b
@copy Build\*.asl %OUTPUT_PATH%\Build /y /b
@copy Build\*.mak %OUTPUT_PATH%\Build /y /b
@copy Build\*.h %OUTPUT_PATH%\Build /y /b
@copy Build\SignOnMessage.uni %OUTPUT_PATH%\Build /y /b

:DeleteConnection
@net use %OUTPUT_DIR% /delete

@if "%ReportError%" == "1" goto PrintErrorMessage

:Finished
echo.
echo *******************************************
echo AaeonRemoteBuild.bat Finished
echo *******************************************
EXIT 0

:PrintErrorMessage
echo. 
echo *******************************************
echo Build Failed
echo *******************************************
EXIT 1