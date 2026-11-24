##############################################################################
# Create Release Package (Folder + Copy + Zip)
# Author: Apiwat Kamsiri | Date: 2026-06-09
##############################################################################

function Write-Success { param([string]$msg) Write-Host "[OK] $msg" -ForegroundColor Green }
function Write-Info { param([string]$msg) Write-Host "[INFO] $msg" -ForegroundColor Cyan }
function Write-Error { param([string]$msg) Write-Host "[ERROR] $msg" -ForegroundColor Red }
function Write-Step { param([string]$msg) Write-Host "`n>>> $msg" -ForegroundColor Yellow }

# --- 1. Parse version information from source files ---
Write-Info "Reading version information..."
$SCRIPT_DIR = Split-Path -Parent $PSCommandPath
$PROJECT_ROOT = Split-Path -Parent $SCRIPT_DIR

$RF_CONFIG_FILE = Join-Path $PROJECT_ROOT "..\sources\config\rf_configs.c"
$SYSTEM_H_FILE = Join-Path $PROJECT_ROOT "..\sources\system.h"
$WORK_PATH = Join-Path $PROJECT_ROOT "..\built\Debug\Exe"

function Get-ParsedValue {
    param($File, $Pattern)
    if (Test-Path $File) {
        $content = Get-Content $File -Raw
        # Use [\s]+ to support spaces and tabs in source files
        if ($content -match $Pattern) { return $matches[1].Trim() }
    }
    return $null
}

# Extract values using regex
$StockNo = Get-ParsedValue $RF_CONFIG_FILE '#define\s+_StockNo[\s]+(\d+)'
$StockVersion = Get-ParsedValue $RF_CONFIG_FILE '#define\s+_StockVersion[\s]+(\d+)'
$StockRevision = Get-ParsedValue $RF_CONFIG_FILE '#define\s+_StockRevision[\s]+(\d+)'
$CoreVer = Get-ParsedValue $SYSTEM_H_FILE '#define\s+UXPURE_CORE_VER[\s]+(\d+)'
$CoreRev = Get-ParsedValue $SYSTEM_H_FILE '#define\s+UXPURE_CORE_REV[\s]+(\d+)'

# Validate if values were found
if ($null -eq $StockNo -or $null -eq $StockVersion -or $null -eq $CoreVer) {
    Write-Error "Could not parse version info from source files!"
    exit 1
}

# Format labels and version string
$VersionLabel = "V$($StockVersion.PadLeft(2,'0'))R$($StockRevision.PadLeft(2,'0'))"
$CoreLabel = "V$($CoreVer.PadLeft(2,'0'))R$($CoreRev.PadLeft(2,'0'))"
$VERSION_STRING = "RF_$($StockNo)-$($VersionLabel)_$($CoreLabel)"
$NewHexName = "$($VERSION_STRING).hex"

Write-Success "Generated version string: $VERSION_STRING"

# --- 2. Create Release Package ---
Write-Step "Creating Release Package..."
try {
    $PackageFolderName = "$($VERSION_STRING)_PACKSOFT"
    $PackageFolderPath = Join-Path $WORK_PATH $PackageFolderName
    $ZipPath = "$($PackageFolderPath).zip"

    # Remove existing package files
    if (Test-Path $PackageFolderPath) { Remove-Item $PackageFolderPath -Recurse -Force }
    if (Test-Path $ZipPath) { Remove-Item $ZipPath -Force }

    # Create temporary directory
    New-Item -ItemType Directory -Path $PackageFolderPath | Out-Null
    
    # Copy HEX file into the package
    $SourceHex = Join-Path $WORK_PATH "RCoS2_100.hex"
    if (Test-Path $SourceHex) {
        Copy-Item -Path $SourceHex -Destination (Join-Path $PackageFolderPath $NewHexName) -Force
        Write-Success "Copied: $NewHexName"
    } else {
        Write-Error "Source HEX not found at $SourceHex"
    }

    # Copy config directory
    $SourceConfig = Join-Path $PROJECT_ROOT "..\sources\config"
    if (Test-Path $SourceConfig) {
        Copy-Item -Path $SourceConfig -Destination (Join-Path $PackageFolderPath "config") -Recurse -Force
        Write-Success "Copied config folder"
    }

    # Generate README_INFO.txt
    $NoteContent = "Build Date: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')`nStock No: $StockNo`nVersion: $VersionLabel`nCore: $CoreLabel"
    $NoteContent | Out-File -FilePath (Join-Path $PackageFolderPath "README_INFO.txt") -Encoding UTF8

    # Compress into ZIP
    Compress-Archive -Path "$($PackageFolderPath)\*" -DestinationPath $ZipPath -Force
    
    # Copy HEX file outside the ZIP for easy access
    $HexOutsideZip = Join-Path $WORK_PATH $NewHexName
    Copy-Item -Path (Join-Path $PackageFolderPath $NewHexName) -Destination $HexOutsideZip -Force
    Write-Success "Placed HEX file outside ZIP: $HexOutsideZip"

    # Clean up temporary directory
    Remove-Item $PackageFolderPath -Recurse -Force
    
    Write-Success "Package created successfully: $ZipPath"
} catch {
    Write-Error "Failed to create package: $_"
}