目的
- 在本项目中需要使用 MSYS2/minGW 工具链时，只在本项目范围临时启用 MSYS2 的二进制路径，避免影响系统级别或其他 conda 项目。

提供的脚本
- with_msys2.bat
  - 用法：`scripts\with_msys2.bat <command>` 或直接双击进入带 MSYS2 PATH 的 `cmd` 会话。
- with_msys2.ps1
  - 用法：`powershell -ExecutionPolicy Bypass -File scripts\with_msys2.ps1 -- <command>`，没有参数则打开一个 `cmd` 会话。

工作原理
- 这些脚本会在当前进程临时将 MSYS2 的 `mingw64\bin` 目录放到 PATH 前端，执行完命令后还原 PATH，因此不会修改系统或用户级别的环境变量。

建议的操作流程
1. 从系统用户/系统 PATH 中移除 MSYS2 的 `...\MSYS2\mingw64\bin`（可选，但推荐）。
   - 手动：通过“系统属性 → 环境变量”编辑用户/系统 PATH，删掉与 MSYS2 相关的条目。
   - PowerShell（示例，先备份再运行）：
```
$old = [Environment]::GetEnvironmentVariable("Path", "User")
$backupPath = "$env:USERPROFILE\path_backup_$(Get-Date -Format 'yyyyMMddHHmmss').txt"
$old | Out-File $backupPath -Encoding utf8
$new = ($old -split ';' | Where-Object {$_ -notmatch 'MSYS2|mingw64'}) -join ';'
[Environment]::SetEnvironmentVariable("Path", $new, "User")
Write-Host "User PATH updated; backup saved to $backupPath"
```
2. 在本项目需要使用 MSYS2 时，运行：
```
# Windows CMD
scripts\with_msys2.bat <your-build-or-cmake-command>
# PowerShell
powershell -ExecutionPolicy Bypass -File scripts\with_msys2.ps1 -- <your-build-or-cmake-command>
```

额外建议
- 将 VS Code 的构建任务（`tasks.json`）调整为在需要 MSYS2 的任务中调用上述脚本。
- 如果遇到 Python 扩展的 DLL 冲突，考虑启用 `CONDA_DLL_SEARCH_MODIFICATION_ENABLE=1`（系统/用户环境变量），并优先使用 conda 提供的编译工具包（conda-forge）。
- 本脚本使用了检测到的路径 `D:\0_ENV\MSYS2\mingw64\bin` 作为默认值；如果你的 MSYS2 安装路径不同，请设置 `MSYS2_BIN_PATH` 环境变量或编辑脚本。
3. 可选：从用户 PATH 中移除 MSYS2（推荐）

如果 MSYS2 路径被添加到用户或系统 PATH，会增加与其他 conda 项目冲突的风险。推荐把 MSYS2 从用户 PATH 中移除，仅在需要时通过本脚本启用。

项目提供了一个辅助清理脚本：

```
powershell -ExecutionPolicy Bypass -File scripts\remove_msys2_from_user_path.ps1 -WhatIf
```

先用 `-WhatIf` 预览将要删除的条目。确认无误后直接运行：

```
powershell -ExecutionPolicy Bypass -File scripts\remove_msys2_from_user_path.ps1
```

该脚本会先备份用户 PATH 到 `%USERPROFILE%\path_backup_*.txt`，再删除匹配 `MSYS2|mingw64|msys64` 的条目。

4. VS Code 任务建议

在需要 MSYS2 的构建任务中，调用包装脚本。例如在 `.vscode\tasks.json` 中：

```
{
  "label": "build with msys2",
  "type": "shell",
  "command": "${workspaceFolder}\\scripts\\with_msys2.bat",
  "args": ["cmake", "--build", "build", "--target", "ScopeFun"],
  "group": "build"
}
```

这样 VS Code 只会在执行该任务时临时启用 MSYS2，不会影响其它任务或项目。
