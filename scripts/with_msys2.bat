@echo off
REM Usage: with_msys2.bat [command]
REM Temporarily prepend MSYS2 mingw64 bin to PATH and run a command.
REM Detected MSYS2 mingw64 bin from your system: D:\0_ENV\MSYS2\mingw64\bin
setlocal
set "MSYS2_BIN=D:\0_ENV\MSYS2\mingw64\bin"
set "OLDPATH=%PATH%"
set "PATH=%MSYS2_BIN%;%PATH%"
if "%~1"=="" (
  cmd.exe
) else (
  %*
)
endlocal
