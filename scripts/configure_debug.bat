@echo off
REM 临时启用 MSYS2 路径并配置 CMake Debug 构建
set "MSYS2_BIN=C:\msys64\mingw64\bin"
set "MSYS2_USR_BIN=C:\msys64\usr\bin"
set "OLDPATH=%PATH%"
set "PATH=%MSYS2_BIN%;%MSYS2_USR_BIN%;%PATH%"

"C:\Program Files\CMake\bin\cmake.exe" -S . -B build -G "MinGW Makefiles" -DCMAKE_MAKE_PROGRAM="C:/msys64/usr/bin/make.exe" -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DSCOPEFUN_BUILD_TYPE=Debug

set "PATH=%OLDPATH%"
pause
