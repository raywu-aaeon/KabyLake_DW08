pushd ..\..\
:: Consumer SKU Production
REM call RomImage\BatchFiles\Production\Consumer_RVP8_D_ME116.bat
REM call RomImage\BatchFiles\Production\Consumer_RVP8_D_OC_ME116.bat
call RomImage\BatchFiles\Production\Consumer_RVP8_D.bat
call RomImage\BatchFiles\Production\Consumer_RVP8_D_OC.bat

:: Corporate SKU Production
REM call RomImage\BatchFiles\Production\Corporate_RVP8_C236_D_ME116.bat
REM call RomImage\BatchFiles\Production\Corporate_RVP8_D_ME116.bat
REM call RomImage\BatchFiles\Production\Corporate_RVP8_D_OC_ME116.bat
call RomImage\BatchFiles\Production\Corporate_RVP8_C236_D.bat
call RomImage\BatchFiles\Production\Corporate_RVP8_D.bat
call RomImage\BatchFiles\Production\Corporate_RVP8_D_OC.bat

:: Consumer SKU Pre-Production
REM call RomImage\BatchFiles\Pre-Production\Consumer_RVP8_D_ME116.bat
REM call RomImage\BatchFiles\Pre-Production\Consumer_RVP8_D_OC_ME116.bat
call RomImage\BatchFiles\Pre-Production\Consumer_RVP8_D.bat
call RomImage\BatchFiles\Pre-Production\Consumer_RVP8_D_OC.bat


:: Corporate SKU Production
REM call RomImage\BatchFiles\Pre-Production\Corporate_RVP8_C236_D_ME116.bat
REM call RomImage\BatchFiles\Pre-Production\Corporate_RVP8_D_ME116.bat
REM call RomImage\BatchFiles\Pre-Production\Corporate_RVP8_D_OC_ME116.bat
call RomImage\BatchFiles\Pre-Production\Corporate_RVP8_C236_D.bat
call RomImage\BatchFiles\Pre-Production\Corporate_RVP8_D.bat
call RomImage\BatchFiles\Pre-Production\Corporate_RVP8_D_OC.bat

popd

