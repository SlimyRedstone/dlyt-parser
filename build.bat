@echo off
cls
echo Compiling Resource File...
windres main.rc -O coff -o output/main.res
echo Compiling main.exe...
g++ main.cpp -o output/main.exe output/main.res
echo Done !
@REM pause