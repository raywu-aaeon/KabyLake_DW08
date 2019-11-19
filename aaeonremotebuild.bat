@set VEB=Kabylake
@set VEB_BUILD_MODULE=
rem @set /p BUILD_ALL=Type 1 for BUILD_ALL, 0 for BUILD, 2 for Rebuild Fsp
@set BUILD_ALL=%1

@set AAEON_VEB_DIR=C:\gitlab-runner
@set AptioV_DIR=Aptio_5.x_TOOLS_33

if "%BUILD_ALL%" == "2" (
        @set VEB=CoffeeLake
        ren Build Build_Ami
)

REM Tools Directiories
@set WINDDK_DIR1=%AAEON_VEB_DIR%\WINDDK\7600.16385.1\bin\x86\x86
@set WINDDK_DIR2=%AAEON_VEB_DIR%\WINDDK\7600.16385.1\bin\x86
@set TOOLS_DIR=%AAEON_VEB_DIR%\%AptioV_DIR%\BuildTools

@set path=%WINDDK_DIR1%;%WINDDK_DIR2%;%TOOLS_DIR%;%path%


REM Env Variables
@set CCX86DIR=%AAEON_VEB_DIR%\WINDDK\7600.16385.1\bin\x86\x86
@set CCX64DIR=%AAEON_VEB_DIR%\WINDDK\7600.16385.1\bin\x86\amd64

if "%BUILD_ALL%" == "0" goto build

:buildall
make clean
call make rebuild
goto end

:build
call make

:end

if "%BUILD_ALL%" == "2" (
        @set VEB=CoffeeLake
        if exist Build (
                make clean
        )
        ren Build_Ami Build
)
