# ScopeFun Debug 构建脚本
$msys2Bin = "C:\msys64\mingw64\bin"
$msys2Usr = "C:\msys64\usr\bin"
$cmake = "C:\Program Files\CMake\bin\cmake.exe"
$srcDir = "D:\0_PRJ\Git\data_sampling_PC_host"
$buildDir = "$srcDir\build"

# 保存原 PATH 并添加 MSYS2
$oldPath = $env:Path
$env:Path = "$msys2Bin;$msys2Usr;$env:Path"

try {
    Write-Host "[1/2] Configuring CMake (Debug)..."
    & $cmake -S $srcDir -B $buildDir -G "MinGW Makefiles" `
        "-DCMAKE_MAKE_PROGRAM=C:/msys64/usr/bin/make.exe" `
        "-DCMAKE_POLICY_VERSION_MINIMUM=3.5" `
        "-DSCOPEFUN_BUILD_TYPE=Debug"
    if ($LASTEXITCODE -ne 0) { throw "CMake configuration failed" }
    
    Write-Host "[2/2] Building ScopeFun..."
    & $cmake --build $buildDir --target ScopeFun
    if ($LASTEXITCODE -eq 0) {
        Write-Host "[DONE] Build successful! Binary at: build\bin\sfDebug.exe" -ForegroundColor Green
    } else {
        throw "Build failed (exit code: $LASTEXITCODE)"
    }
}
catch {
    Write-Host "[ERROR] $_" -ForegroundColor Red
    Read-Host "Press Enter to exit"
}
finally {
    $env:Path = $oldPath
}

# 不自动退出，让用户看到结果
pause
