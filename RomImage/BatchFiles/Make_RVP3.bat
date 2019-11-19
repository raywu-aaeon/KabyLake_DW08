pushd ..\..\
:: Consumer SKU Production
REM call RomImage\BatchFiles\Production\Consumer_RVP3_C_ME115.bat
REM call RomImage\BatchFiles\Production\Consumer_RVP3_C_ME116.bat
call RomImage\BatchFiles\Production\Consumer_RVP3_C.bat

:: Corporate SKU Production
REM call RomImage\BatchFiles\Production\Corporate_RVP3_C_ME115.bat
REM call RomImage\BatchFiles\Production\Corporate_RVP3_C_ME116.bat
call RomImage\BatchFiles\Production\Corporate_RVP3_C.bat

:: Consumer SKU Pre-Production
REM call RomImage\BatchFiles\Pre-Production\Consumer_RVP3_C_ME115.bat
REM call RomImage\BatchFiles\Pre-Production\Consumer_RVP3_C_ME116.bat
call RomImage\BatchFiles\Pre-Production\Consumer_RVP3_C.bat

:: Corporate SKU Pre-Production
REM call RomImage\BatchFiles\Pre-Production\Corporate_RVP3_C_ME115.bat
REM call RomImage\BatchFiles\Pre-Production\Corporate_RVP3_C_ME116.bat
call RomImage\BatchFiles\Pre-Production\Corporate_RVP3_C.bat

popd

