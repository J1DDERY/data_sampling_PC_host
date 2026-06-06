@echo off
REM ============================================
REM ScopeFun Debug 构建脚本
REM 自动启用 MSYS2 环境并构建项目
REM ============================================
setlocal

REM MSYS2 路径
set "MSYS2_BIN=C:\msys64\mingw64\bin"
set "MSYS2_USR_BIN=C:\msys64\usr\bin"
set "CMAKE_BIN=C:\Program Files\CMake\bin"

REM 临时添加 MSYS2 + Windows 系统目录到 PATH
set "OLDPATH=%PATH%"
set "PATH=%MSYS2_BIN%;%MSYS2_USR_BIN%;%CMAKE_BIN%;C:\Windows\System32;C:\Windows;%PATH%"

REM 切换到项目目录
cd /d "D:\0_PRJ\Git\data_sampling_PC_host"

REM 配置 CMake
echo [1/2] Configuring CMake...
"%CMAKE_BIN%\cmake.exe" -S . -B build -G "MinGW Makefiles" ^
    "-DCMAKE_MAKE_PROGRAM=C:/msys64/usr/bin/make.exe" ^
    "-DCMAKE_POLICY_VERSION_MINIMUM=3.5" ^
    "-DSCOPEFUN_BUILD_TYPE=Debug"
if %ERRORLEVEL% neq 0 (
    echo [ERROR] CMake configuration failed!
    goto :error
)

REM 构建
echo [2/2] Building ScopeFun...
"%CMAKE_BIN%\cmake.exe" --build build --target ScopeFun
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Build failed!
    goto :error
)

echo.
echo [DONE] Build successful! Binary at: build\bin\sfDebug.exe
goto :end

:error
echo.
echo [FAILED] Build failed with error %ERRORLEVEL%
pause

:end
set "PATH=%OLDPATH%"
endlocal
