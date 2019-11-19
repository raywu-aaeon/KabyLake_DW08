pushd ..\..\
:: Consumer SKU Production
REM call RomImage\BatchFiles\Production\Consumer_RVP11_D_ME116.bat
call RomImage\BatchFiles\Production\Consumer_RVP11_D.bat

:: Corporate SKU Production
REM call RomImage\BatchFiles\Production\Corporate_RVP11_D_ME116.bat
call RomImage\BatchFiles\Production\Corporate_RVP11_D.bat

:: Consumer SKU Pre-Production
REM call RomImage\BatchFiles\Pre-Production\Consumer_RVP11_D_ME116.bat
call RomImage\BatchFiles\Pre-Production\Consumer_RVP11_D.bat

:: Corporate SKU Production
REM call RomImage\BatchFiles\Pre-Production\Corporate_RVP11_D_ME116.bat
call RomImage\BatchFiles\Pre-Production\Corporate_RVP11_D.bat

popd

