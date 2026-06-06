@echo off
REM Usage: with_msys2.bat [command]
REM Temporarily prepend MSYS2 mingw64 bin to PATH and run a command.
REM MSYS2 installed at C:\msys64
setlocal
set "MSYS2_BIN=C:\msys64\mingw64\bin"
set "MSYS2_USR_BIN=C:\msys64\usr\bin"
set "OLDPATH=%PATH%"
set "PATH=%MSYS2_BIN%;%MSYS2_USR_BIN%;%PATH%"
if "%~1"=="" (
  cmd.exe
) else (
  %*
)
endlocal
