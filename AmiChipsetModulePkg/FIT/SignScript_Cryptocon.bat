@ECHO OFF

REM ===============================================================================
REM Script input parameters
REM ===============================================================================
SET INPUT_UNSIGN_FILE=%~1 
SET OUTPUT_SIGN_FILE=%~2
SET CRYPTCON=Cryptocon.exe
IF "%1" == "BpmWithoutSigElement.bin" SET KEY=BpmPEM.key
IF "%1" == "KmWithoutSigElement.bin" SET KEY=KmPEM.key
IF "%1" == "FvWithoutSigElement.bin" SET KEY=BpmPEM.key

AmiChipsetModulePkg\FIT\%CRYPTCON% -s -k AmiChipsetModulePkg\FIT\BootGuardKeys\%KEY% -f %INPUT_UNSIGN_FILE% -o %OUTPUT_SIGN_FILE%