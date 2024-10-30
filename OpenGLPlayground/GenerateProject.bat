@echo off

mkdir build
"C:\Program Files\CMake\bin\cmake.exe" -S. -Bbuild

If Errorlevel 1 Goto Err
exit
:Err
pause
