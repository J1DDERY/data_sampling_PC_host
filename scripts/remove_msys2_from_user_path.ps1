<#
Backup and remove MSYS2/mingw entries from the current user's PATH.
Usage (Preview):
  powershell -ExecutionPolicy Bypass -File scripts\remove_msys2_from_user_path.ps1 -WhatIf
To actually run (will backup then set):
  powershell -ExecutionPolicy Bypass -File scripts\remove_msys2_from_user_path.ps1
#>
param(
  [switch]$WhatIf
)

$pattern = 'MSYS2|mingw64|msys64'
$userPath = [Environment]::GetEnvironmentVariable('Path','User')
$timestamp = (Get-Date).ToString('yyyyMMddHHmmss')
$backupFile = "$env:USERPROFILE\path_backup_$timestamp.txt"

if ($WhatIf) {
  Write-Host "[WhatIf] Would backup user PATH to: $backupFile"
  Write-Host "[WhatIf] User PATH entries matching pattern '$pattern' that would be removed:"
  $userPath -split ';' | Where-Object { $_ -match $pattern } | ForEach-Object { Write-Host "  $_" }
  exit 0
}

# Backup
$userPath | Out-File $backupFile -Encoding utf8
Write-Host "Backed up current user PATH to: $backupFile"

# Remove matching entries
$new = ($userPath -split ';' | Where-Object { $_ -and ($_ -notmatch $pattern) }) -join ';'
[Environment]::SetEnvironmentVariable('Path',$new,'User')
Write-Host "Removed MSYS2/mingw entries from user PATH. If something breaks, restore from: $backupFile"
