param(
  [Parameter(Mandatory=$false)]
  [ValidateSet('cmd','powershell')]
  [string]$Shell = 'cmd'
)

<#
Usage:
  powershell -ExecutionPolicy Bypass -File scripts\with_msys2.ps1 -- <command> <args>
  powershell -ExecutionPolicy Bypass -File scripts\with_msys2.ps1 -Shell powershell

Description:
  Temporarily prepend MSYS2 mingw64 bin to PATH and run a child shell or a command.
  The script does NOT modify system or user environment variables; changes are local to this process and its children.
#>

$msys2 = $env:MSYS2_BIN_PATH
if (-not $msys2) { $msys2 = 'C:\msys64\mingw64\bin' }
$msys2_usr = 'C:\msys64\usr\bin'

# Warn if MSYS2 already exists in system/user PATH (informational only)
$pathEntries = ([Environment]::GetEnvironmentVariable('Path','Machine') -split ';') + ([Environment]::GetEnvironmentVariable('Path','User') -split ';')
if ($pathEntries -join '|' -match [regex]::Escape($msys2)) {
  Write-Host "Warning: MSYS2 path detected in system or user PATH: $msys2" -ForegroundColor Yellow
  Write-Host "It's recommended to remove MSYS2 from system/user PATH and use this script to enable it per-project." -ForegroundColor Yellow
}

$oldPath = $env:PATH
try {
  $env:PATH = "$msys2;$msys2_usr;$env:PATH"
  if ($Shell -eq 'powershell') {
    Write-Host "Opening new PowerShell with MSYS2 in PATH..."
    # Start an interactive PowerShell child so user can use mingw tools interactively
    & powershell -NoExit -ExecutionPolicy Bypass -Command "" 
  } else {
    if ($args.Count -eq 0) {
      Write-Host "Opening new CMD with MSYS2 in PATH..."
      & cmd.exe
    } else {
      & cmd.exe /c ($args -join ' ')
    }
  }
} finally {
  # restore PATH in this process
  $env:PATH = $oldPath
}
