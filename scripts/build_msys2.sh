#!/bin/sh
# ScopeFun 构建脚本 (MSYS2 shell)
export PATH="/mingw64/bin:/usr/bin:/bin:/c/Windows/System32:/c/Windows:/c/Windows/System32/Wbem:/c/Windows/System32/WindowsPowerShell/v1.0"
cd /d/0_PRJ/Git/data_sampling_PC_host/build
echo "[BUILD] Starting ScopeFun build..."
make -j4 ScopeFun 2>&1
echo "[BUILD] Exit code: $?"
