pushd ..\..\
:: Consumer SKU Production
REM call RomImage\BatchFiles\Production\Consumer_RVP10_D_ME116.bat
REM call RomImage\BatchFiles\Production\Consumer_RVP10_D_OC_ME116.bat
call RomImage\BatchFiles\Production\Consumer_RVP10_D.bat
call RomImage\BatchFiles\Production\Consumer_RVP10_D_OC.bat

:: Corporate SKU Production
REM call RomImage\BatchFiles\Production\Corporate_RVP10_D_ME116.bat
REM call RomImage\BatchFiles\Production\Corporate_RVP10_D_OC_ME116.bat
call RomImage\BatchFiles\Production\Corporate_RVP10_D.bat
call RomImage\BatchFiles\Production\Corporate_RVP10_D_OC.bat

:: Consumer SKU Pre-Production
REM call RomImage\BatchFiles\Pre-Production\Consumer_RVP10_D_ME116.bat
REM call RomImage\BatchFiles\Pre-Production\Consumer_RVP10_D_OC_ME116.bat
call RomImage\BatchFiles\Pre-Production\Consumer_RVP10_D.bat
call RomImage\BatchFiles\Pre-Production\Consumer_RVP10_D_OC.bat

:: Corporate SKU Pre-Production
REM call RomImage\BatchFiles\Pre-Production\Corporate_RVP10_D_ME116.bat
REM call RomImage\BatchFiles\Pre-Production\Corporate_RVP10_D_OC_ME116.bat
call RomImage\BatchFiles\Pre-Production\Corporate_RVP10_D.bat
call RomImage\BatchFiles\Pre-Production\Corporate_RVP10_D_OC.bat

popd

