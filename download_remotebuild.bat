@set SUB_FOLDER=Kabylake
@set WORK_FOLDER=C:\AAEON\BIOS\_work\Kabylake_DT_1ATQP080

cd %WORK_FOLDER%
del *.bin /Q
del *.rom /Q
del Build\*.* /Q
rem del _Aaeon_Release\*.bin /Q
net use \\172.16.1.26\public\runner-build\%SUB_FOLDER% /user:nas5\raywu ray
copy \\172.16.1.26\public\runner-build\%SUB_FOLDER%\*.bin Build /y /b
copy \\172.16.1.26\public\runner-build\%SUB_FOLDER%\Token.* Build /y /b
copy \\172.16.1.26\public\runner-build\%SUB_FOLDER%\Platform.* Build /y /b
copy \\172.16.1.26\public\runner-build\%SUB_FOLDER%\SignOnMessage.* Build /y /b
copy \\172.16.1.26\public\runner-build\%SUB_FOLDER%\GSIOSETUPVAR.* Build /y /b
copy \\172.16.1.26\public\runner-build\%SUB_FOLDER%\*.inf Build /y /b
copy \\172.16.1.26\public\runner-build\%SUB_FOLDER%\PCIBRDSDL.* Build /y /b
copy \\172.16.1.26\public\runner-build\%SUB_FOLDER%\*.hfr Build /y /b
net use \\172.16.1.26\public\runner-build\%SUB_FOLDER% /delete

pause.