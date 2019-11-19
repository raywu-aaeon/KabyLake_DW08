@ECHO OFF

REM ===============================================================================
REM Script input parameters
REM ===============================================================================
SET INPUT_UNSIGN_FILE=%~1
SET OUTPUT_SIGN_FILE=%~2
SET SIGNTOOL=signtool.exe
SET P7Parser_Tool=P7Parser.exe
IF "%1" == "BpmWithoutSigElement.bin" SET KEY=mycert.pfx
IF "%1" == "KmWithoutSigElement.bin" SET KEY=mycert.pfx
IF "%1" == "FvWithoutSigElement.bin" SET KEY=mycert.pfx

REM Generate the signature.
AmiChipsetModulePkg\FIT\%SIGNTOOL% sign /fd sha256 /p7 . /p7co 1.2.840.113549.1.7.1 /p7ce DetachedSignedData /a /f AmiChipsetModulePkg\FIT\%KEY% %INPUT_UNSIGN_FILE%

AmiChipsetModulePkg\FIT\%P7Parser_Tool% /i %INPUT_UNSIGN_FILE%.p7 /s %OUTPUT_SIGN_FILE%