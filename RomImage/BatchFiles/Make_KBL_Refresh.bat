pushd ..\..\
REM :: Consumer SKU Production
REM call RomImage\BatchFiles\Production\Consumer_KBL_Refresh_C_ME116.bat

REM :: Corporate SKU Production
REM call RomImage\BatchFiles\Production\Corporate_KBL_Refresh_C_ME116.bat

REM :: Consumer SKU Pre-Production
REM call RomImage\BatchFiles\Pre-Production\Consumer_KBL_Refresh_C_ME116.bat

REM :: Corporate SKU Pre-Production
REM call RomImage\BatchFiles\Pre-Production\Corporate_KBL_Refresh_C_ME116.bat

:: Consumer SKU Production
call RomImage\BatchFiles\Production\Consumer_KBL_Refresh_C.bat

:: Corporate SKU Production
call RomImage\BatchFiles\Production\Corporate_KBL_Refresh_C.bat

:: Consumer SKU Pre-Production
call RomImage\BatchFiles\Pre-Production\Consumer_KBL_Refresh_C.bat

:: Corporate SKU Pre-Production
call RomImage\BatchFiles\Pre-Production\Corporate_KBL_Refresh_C.bat

popd

