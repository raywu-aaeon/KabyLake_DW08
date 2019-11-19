pushd ..\..\
:: Consumer SKU Production
REM call RomImage\BatchFiles\Production\Consumer_RVP9_D_ME116.bat
call RomImage\BatchFiles\Production\Consumer_RVP9_D.bat

:: Corporate SKU Production
REM call RomImage\BatchFiles\Production\Corporate_RVP9_ME116.bat
call RomImage\BatchFiles\Production\Corporate_RVP9_D.bat

:: Consumer SKU Pre-Production
REM call RomImage\BatchFiles\Pre-Production\Consumer_RVP9_D_ME116.bat
call RomImage\BatchFiles\Pre-Production\Consumer_RVP9_D.bat

:: Corporate SKU Production
REM call RomImage\BatchFiles\Pre-Production\Corporate_RVP9_ME116.bat
call RomImage\BatchFiles\Pre-Production\Corporate_RVP9_D.bat

popd

