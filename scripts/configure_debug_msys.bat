@echo off
REM CMake 配置 - 使用 MSYS Makefiles 生成器
setlocal
set "MSYS2_BIN=C:\msys64\mingw64\bin"
set "MSYS2_USR_BIN=C:\msys64\usr\bin"
set "CMAKE_BIN=C:\Program Files\CMake\bin"
set "PATH=%MSYS2_BIN%;%MSYS2_USR_BIN%;%CMAKE_BIN%;C:\Windows\System32;C:\Windows;%PATH%"

cd /d D:\0_PRJ\Git\data_sampling_PC_host

echo Configuring with MSYS Makefiles...
"%CMAKE_BIN%\cmake.exe" -S . -B build -G "MSYS Makefiles" -DCMAKE_MAKE_PROGRAM="C:/msys64/usr/bin/make.exe" -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DSCOPEFUN_BUILD_TYPE=Debug

if %ERRORLEVEL% equ 0 (
    echo [OK] Configuration successful
) else (
    echo [ERROR] Configuration failed
    pause
)
endlocal
