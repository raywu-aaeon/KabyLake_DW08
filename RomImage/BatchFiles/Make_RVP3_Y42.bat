pushd ..\..\
:: Consumer SKU Production
call RomImage\BatchFiles\Production\Consumer_RVP3_Y42_C.bat

:: Corporate SKU Production
call RomImage\BatchFiles\Production\Corporate_RVP3_Y42_C.bat

:: Consumer SKU Pre-Production
call RomImage\BatchFiles\Pre-Production\Consumer_RVP3_Y42_C.bat

:: Corporate SKU Pre-Production
call RomImage\BatchFiles\Pre-Production\Corporate_RVP3_Y42_C.bat

popd

