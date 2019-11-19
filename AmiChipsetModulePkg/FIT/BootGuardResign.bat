@ECHO OFF

cls

cd ..\..\

echo.
echo ##### Boot Guard Resign Start #####
echo.

Set ResignTool=AmiChipsetModulePkg\FIT\BpmKmGen.exe
Set ResignBios=Build\FITc_BIOS.rom
Set FITPath=AmiChipsetModulePkg\FIT
Set BpmKey=AmiChipsetModulePkg\FIT\BootGuardKeys\BpmPEM.key
Set KmKey=AmiChipsetModulePkg\FIT\BootGuardKeys\KmPEM.key
Set FvAmKey=AmiChipsetModulePkg\FIT\BootGuardKeys\BpmPEM.key

%ResignTool% -RESIGN %ResignBios% %FITPath% %BpmKey% %KmKey% %FvAmKey%

echo.
echo ##### Boot Guard Resign End #####
echo.

cd AmiChipsetModulePkg\FIT