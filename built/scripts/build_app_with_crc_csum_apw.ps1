##############################################################################
# ALESTA RCOS2 - Automatic CRC, Checksum & OTA Image Builder
##############################################################################
# Description: 
#   - Adds CRC32 and Checksum to application hex file
#   - Creates dynamic filename from version info in source code
#   - Generates Factory image (Bootloader + Application) with 'B' prefix
#   - Generates OTA-ready hex & binary files with 'O' prefix
#   - Relocates metadata from 0x3FF00 to 0x04B00 for OTA
#
# Output Files:
#   1. RF_<StockNo>_V<Ver>R<Rev>_V<CoreVer>R<CoreRev>_crc_csum.hex
#      - Base application with CRC32 + Checksum
#   2. RF_<StockNo>_V<Ver>R<Rev>_BV<CoreVer>R<CoreRev>_crc_csum.hex
#      - Factory image (Bootloader included)
#   3. RF_<StockNo>_V<Ver>R<Rev>_OV<CoreVer>R<CoreRev>_crc_csum.hex
#      - OTA-ready hex (metadata prepended at 0x04B00)
#   4. RF_<StockNo>_V<Ver>R<Rev>_OV<CoreVer>R<CoreRev>_crc_csum.bin
#      - OTA-ready binary (for actual OTA transfer)
#
# Version Sources:
#   - config/rf_configs.c: _StockNo, _StockVersion, _StockRevision
#   - sources/system.h: UXPURE_CORE_VER, UXPURE_CORE_REV
#
# Tools Required:
#   - srec_cat (SRecord utilities package)
#
# Usage: 
#   IAR Post-Build: 
#     powershell.exe -ExecutionPolicy Bypass -File "$PROJ_DIR$\scripts\build_app_with_crc_csum.ps1"
#
# Author: Automated Build System
# Date: 2026-01-13
##############################################################################

param(
    [string]$InputHex = "",
    [string]$OutputBaseName = "",
    [string]$WorkPath = ""
)

$ErrorActionPreference = "Stop"

# Functions
function Write-Success { param([string]$msg) Write-Host "[OK] $msg" -ForegroundColor Green }
function Write-Info { param([string]$msg) Write-Host "[INFO] $msg" -ForegroundColor Cyan }
function Write-Error { param([string]$msg) Write-Host "[ERROR] $msg" -ForegroundColor Red }
function Write-Step { param([string]$msg) Write-Host "`n>>> $msg" -ForegroundColor Yellow }

# Configuration
# $SREC_CAT = "C:\Program Files\srecord\bin\srec_cat.exe"
#---------------- Auto search ----------------
$SREC_CAT = $null

foreach ($drive in @("C:\","D:\")) {

    Write-Host "Searching in $drive ..."

    $result = Get-ChildItem -Path $drive `
                            -Filter "srec_cat.exe" `
                            -Recurse `
                            -ErrorAction SilentlyContinue |
              Select-Object -First 1 -ExpandProperty FullName

    if ($result) {
        $SREC_CAT = $result
        break
    }
}

if (-not $SREC_CAT) {
    Write-Host "srec_cat.exe not found!" -ForegroundColor Red
    exit 1
}

Write-Host "Using: $SREC_CAT"
#---------------- Auto search ----------------

# Detect if running from IAR (check for IAR environment variables)
$IAR_EXE_PATH = $env:EW_DIR
$IAR_CONFIG = $env:CONFIG_NAME

if ($IAR_EXE_PATH -or $IAR_CONFIG) {
    Write-Info "Running from IAR Build System"
    Write-Info "Configuration: $IAR_CONFIG"
}

# Determine work path
if ($WorkPath) {
    $WORK_PATH = $WorkPath
} elseif ($env:TARGET_DIR) {
    # IAR sets TARGET_DIR to output directory
    $WORK_PATH = $env:TARGET_DIR
} else {
    # Fallback: assume script is in project/scripts/
    $SCRIPT_DIR = Split-Path -Parent $PSCommandPath
    $PROJECT_ROOT = Split-Path -Parent $SCRIPT_DIR
    $WORK_PATH = Join-Path $PROJECT_ROOT "Debug\Exe"
}

Write-Host "`n============================================================" -ForegroundColor Cyan
Write-Host "  ALESTA RCOS2 - CRC & Checksum Builder (srec_cat)" -ForegroundColor Cyan
Write-Host "============================================================" -ForegroundColor Cyan

# Parse version information from source files
Write-Info "Reading version information..."

$SCRIPT_DIR = Split-Path -Parent $PSCommandPath
$PROJECT_ROOT = Split-Path -Parent $SCRIPT_DIR

# Read rf_configs.c for Stock parameters
$RF_CONFIG_FILE = Join-Path $PROJECT_ROOT "..\sources\config\rf_configs.c"
if (Test-Path $RF_CONFIG_FILE) {
    $rfContent = Get-Content $RF_CONFIG_FILE -Raw
    
    # Extract StockNo, Version, Revision
    if ($rfContent -match '#define\s+_StockNo\s+(\d+)') {
        $StockNo = $matches[1]
    }
    if ($rfContent -match '#define\s+_StockVersion\s+(\d+)') {
        $StockVersion = [int]$matches[1]
    }
    if ($rfContent -match '#define\s+_StockRevision\s+(\d+)') {
        $StockRevision = [int]$matches[1]
    }
    
    Write-Info "Stock Info: StockNo=$StockNo, Version=$StockVersion, Revision=$StockRevision"
} else {
    Write-Error "rf_configs.c not found at: $RF_CONFIG_FILE"
    exit 1
}

# Read system.h for UXPURE_CORE parameters
$SYSTEM_H_FILE = Join-Path $PROJECT_ROOT "..\sources\system.h"
if (Test-Path $SYSTEM_H_FILE) {
    $sysContent = Get-Content $SYSTEM_H_FILE -Raw
    
    # Extract UXPURE_CORE_VER and UXPURE_CORE_REV
    if ($sysContent -match '#define\s+UXPURE_CORE_VER\s+(\d+)') {
        $CoreVer = [int]$matches[1]
    }
    if ($sysContent -match '#define\s+UXPURE_CORE_REV\s+(\d+)') {
        $CoreRev = [int]$matches[1]
    }
    
    Write-Info "Core Info: Version=$CoreVer, Revision=$CoreRev"
} else {
    Write-Error "system.h not found at: $SYSTEM_H_FILE"
    exit 1
}

# Format version strings with leading zeros
$StockVersionStr = $StockVersion.ToString("D2")   # 02, 09, 10, etc.
$StockRevisionStr = $StockRevision.ToString("D2")
$CoreVerStr = $CoreVer.ToString("D2")
$CoreRevStr = $CoreRev.ToString("D2")

# Generate filename: RF_<StockNo>_V<Version>R<Revision>_V<CoreVer>R<CoreRev>
$VERSION_STRING = "RF_${StockNo}_V${StockVersionStr}R${StockRevisionStr}_V${CoreVerStr}R${CoreRevStr}"
Write-Success "Generated version string: $VERSION_STRING"

# Verify srec_cat exists
try {
    $null = Get-Command $SREC_CAT -ErrorAction Stop
    Write-Success "srec_cat found in PATH"
} catch {
    Write-Error "srec_cat not found in PATH"
    exit 1
}

# Change to work directory
if (-not (Test-Path $WORK_PATH)) {
    Write-Error "Work directory not found: $WORK_PATH"
    exit 1
}
Set-Location $WORK_PATH
Write-Info "Working directory: $WORK_PATH"

# Auto-detect input hex file if not specified
if (-not $InputHex) {
    # Try to find hex file (look for ALESTA_RCOS2.hex or project name)
    $hexFiles = Get-ChildItem -Filter "*.hex" | Where-Object { $_.Name -notlike "*_crc*" -and $_.Name -notlike "*_v0*" }
    if ($hexFiles) {
        $InputHex = $hexFiles[0].Name
        Write-Info "Auto-detected input file: $InputHex"
    } else {
        Write-Error "No input hex file found"
        exit 1
    }
}

# Auto-detect output base name if not specified
if (-not $OutputBaseName) {
    # Use version string as output base name
    $OutputBaseName = $VERSION_STRING
}

# Verify input file
if (-not (Test-Path $InputHex)) {
    Write-Error "Input hex file not found: $InputHex"
    exit 1
}
$inputFile = Get-Item $InputHex
$inputSizeKB = [math]::Round($inputFile.Length/1KB, 2)
Write-Success "Input file: $InputHex (${inputSizeKB} KB)"

$OutputCRC = "${OutputBaseName}_crc.hex"
$OutputFinal = "${OutputBaseName}_crc_csum.hex"
$OutputFactory = ""  # Will be set if bootloader merge happens
$OutputOTA = ""      # Will be set if OTA image created
$OutputOTABin = ""   # Will be set if OTA binary created
$factorySizeKB = 0   # Will be set if factory image created
$otaSizeKB = 0       # Will be set if OTA image created
$otaBinSizeKB = 0    # Will be set if OTA binary created

##############################################################################
# STEP 1: Add CRC32 (based on app_crc32.txt)
##############################################################################
Write-Step "STEP 1/3: Adding CRC32 checksum..."

Write-Info "Command from app_crc32.txt:"
Write-Host "  - Fill 0xFF from 0x04C00 to 0x03FF00" -ForegroundColor Gray
Write-Host "  - Exclude CRC location 0x03FF44-0x03FF48" -ForegroundColor Gray
Write-Host "  - Calculate STM32 CRC and place at 0x03FF44" -ForegroundColor Gray

try {
    # CRC32 command (from app_crc32.txt)
    & $SREC_CAT `
        $InputHex -Intel `
        -fill 0xFF 0x04C00 0x03FF00 `
        -exclude 0x03FF44 0x03FF48 `
        -crop 0x04C00 0x03FF00 `
        -STM32 0x03FF44 `
        -crop 0x03FF44 0x03FF48 `
        $InputHex -Intel `
        -fill 0xFF 0x04C00 0x03FF00 `
        -exclude 0x03FF44 0x03FF48 `
        -o $OutputCRC -Intel
    
    if ($LASTEXITCODE -ne 0) {
        throw "srec_cat CRC32 step failed with exit code: $LASTEXITCODE"
    }
    
    if (Test-Path $OutputCRC) {
        $crcFile = Get-Item $OutputCRC
        $crcSizeKB = [math]::Round($crcFile.Length/1KB, 2)
        Write-Success "CRC32 added successfully: $OutputCRC (${crcSizeKB} KB)"
    } else {
        throw "Output file not created: $OutputCRC"
    }
} catch {
    Write-Error "Failed to add CRC32: $_"
    exit 1
}

##############################################################################
# STEP 2: Add Checksum (based on app_csum.txt)
##############################################################################
Write-Step "STEP 2/3: Adding Checksum..."

Write-Info "Command from app_csum.txt:"
Write-Host "  - Exclude checksum location 0x3FF54-0x3FF55" -ForegroundColor Gray
Write-Host "  - Crop metadata area 0x03FF00-0x3FF54" -ForegroundColor Gray
Write-Host "  - Calculate negative byte checksum at 0x3FF54" -ForegroundColor Gray

try {
    # Checksum command (from app_csum.txt)
    & $SREC_CAT `
        $OutputCRC -Intel `
        -exclude 0x3FF54 0x3FF55 `
        $OutputCRC -Intel `
        -crop 0x03FF00 0x3FF54 `
        -checksum-neg-b-e 0x3FF54 1 1 `
        -crop 0x3FF54 0x3FF55 `
        -o $OutputFinal -Intel
    
    if ($LASTEXITCODE -ne 0) {
        throw "srec_cat Checksum step failed with exit code: $LASTEXITCODE"
    }
    
    if (Test-Path $OutputFinal) {
        $finalFile = Get-Item $OutputFinal
        $finalSizeKB = [math]::Round($finalFile.Length/1KB, 2)
        Write-Success "Checksum added successfully: $OutputFinal (${finalSizeKB} KB)"
        
        # Clean up intermediate CRC file (not needed)
        if (Test-Path $OutputCRC) {
            Remove-Item $OutputCRC -Force
            Write-Info "Removed intermediate file: $OutputCRC"
        }
    } else {
        throw "Output file not created: $OutputFinal"
    }
} catch {
    Write-Error "Failed to add Checksum: $_"
    # Clean up intermediate file on error
    if (Test-Path $OutputCRC) {
        Remove-Item $OutputCRC -Force
        Write-Info "Cleaned up intermediate file: $OutputCRC"
    }
    exit 1
}

##############################################################################
# Summary
##############################################################################
Write-Host "`n============================================================" -ForegroundColor Green
Write-Host "                   BUILD SUCCESSFUL" -ForegroundColor Green
Write-Host "============================================================" -ForegroundColor Green

Write-Host "`nVersion Info:" -ForegroundColor Cyan
Write-Host "  Stock No:       $StockNo" -ForegroundColor White
Write-Host "  Stock Version:  V${StockVersionStr}R${StockRevisionStr}" -ForegroundColor White
Write-Host "  Core Version:   V${CoreVerStr}R${CoreRevStr}" -ForegroundColor White

Write-Host "`nGenerated File:" -ForegroundColor Cyan
Write-Host "  - $OutputFinal" -ForegroundColor White

Write-Host "`nFile Sizes:" -ForegroundColor Cyan
Write-Host "  Original:   ${inputSizeKB} KB" -ForegroundColor Gray
Write-Host "  Final:      ${finalSizeKB} KB (CRC32 + Checksum)" -ForegroundColor Green

Write-Host "`nMemory Layout:" -ForegroundColor Cyan
Write-Host "  Application:  0x04C00 - 0x03FF00" -ForegroundColor Gray
Write-Host "  CRC32:        0x03FF44 - 0x03FF47 (4 bytes)" -ForegroundColor Yellow
Write-Host "  Metadata:     0x03FF00 - 0x3FFFF (256 bytes)" -ForegroundColor Gray
Write-Host "  Checksum:     0x3FF54 (1 byte)" -ForegroundColor Yellow

##############################################################################
# STEP 3: Merge with Bootloader (Factory Image)
##############################################################################
Write-Step "STEP 3/4: Merging with Bootloader for Factory Image..."

# Bootloader path 
$BOOTLOADER_PATH = "D:\REPO_D\BootloaderONE\BootloaderONE\Debug\Exe"
$BOOTLOADER_HEX_ORIGINAL = Join-Path $BOOTLOADER_PATH "BootloaderONE_DBG.hex"
$BOOTLOADER_HEX_FINAL = Join-Path $BOOTLOADER_PATH "BootloaderONE_DBG_crc_csum.hex"

# Check if bootloader exists
if (-not (Test-Path $BOOTLOADER_HEX_ORIGINAL)) {
    Write-Info "Bootloader not found at: $BOOTLOADER_HEX_ORIGINAL"
    Write-Info "Skipping factory image creation"
} else {
    Write-Info "Bootloader found: BootloaderONE_DBG.hex"
    
    # Step 3.1: Process Bootloader CRC32 and Checksum
    Write-Host "  [3.1] Processing bootloader CRC32 and Checksum..." -ForegroundColor Gray
    
    try {
        $BOOTLOADER_CRC = Join-Path $BOOTLOADER_PATH "BootloaderONE_DBG_crc.hex"
        
        # Apply CRC32 to bootloader
        Write-Host "    - Applying bootloader CRC32 (0x00000-0x03F00, CRC@0x03F44)..." -ForegroundColor Gray
        
        & $SREC_CAT `
            $BOOTLOADER_HEX_ORIGINAL -Intel `
            -fill 0xFF 0x00000 0x03F00 `
            -exclude 0x03F44 0x03F48 `
            -crop 0x00000 0x03F00 `
            -STM32 0x03F44 `
            -crop 0x03F44 0x03F48 `
            $BOOTLOADER_HEX_ORIGINAL -Intel `
            -fill 0xFF 0x00000 0x03F00 `
            -exclude 0x03F44 0x03F48 `
            -o $BOOTLOADER_CRC -Intel
        
        if ($LASTEXITCODE -ne 0) {
            throw "Bootloader CRC32 failed with exit code: $LASTEXITCODE"
        }
        Write-Host "    - Bootloader CRC32 added" -ForegroundColor Green
        
        # Apply Checksum to bootloader
        Write-Host "    - Applying bootloader Checksum (0x03F00-0x3F54, Checksum@0x3F54)..." -ForegroundColor Gray
        
        & $SREC_CAT `
            $BOOTLOADER_CRC -Intel `
            -exclude 0x3F54 0x3F55 `
            $BOOTLOADER_CRC -Intel `
            -crop 0x03F00 0x3F54 `
            -checksum-neg-b-e 0x3F54 1 1 `
            -crop 0x3F54 0x3F55 `
            -o $BOOTLOADER_HEX_FINAL -Intel
        
        if ($LASTEXITCODE -ne 0) {
            throw "Bootloader Checksum failed with exit code: $LASTEXITCODE"
        }
        Write-Host "    - Bootloader Checksum added" -ForegroundColor Green
        
        # Clean up intermediate bootloader CRC file
        if (Test-Path $BOOTLOADER_CRC) {
            Remove-Item $BOOTLOADER_CRC -Force
        }
        
        Write-Success "Bootloader processed: BootloaderONE_DBG_crc_csum.hex"
        
        # Step 3.2: Merge Bootloader + Application
        Write-Host "  [3.2] Merging bootloader + application..." -ForegroundColor Gray
        
        # Factory image filename: Insert 'B' before CoreVer
        # RF_6175242600_V01R20_V85R02 -> RF_6175242600_V01R20_BV85R02
        $FactoryBaseName = "RF_${StockNo}_V${StockVersionStr}R${StockRevisionStr}_BV${CoreVerStr}R${CoreRevStr}"
        $OutputFactory = "${FactoryBaseName}_crc_csum.hex"
        
        Write-Info "Factory image will be: $OutputFactory"
        Write-Info "Merging bootloader (0x00000-0x04BFF) + application (0x04C00+)..."
        
        # Merge bootloader and application using srec_cat
        & $SREC_CAT `
            $BOOTLOADER_HEX_FINAL -Intel `
            $OutputFinal -Intel `
            -o $OutputFactory -Intel
        
        if ($LASTEXITCODE -ne 0) {
            throw "srec_cat merge failed with exit code: $LASTEXITCODE"
        }
        
        if (Test-Path $OutputFactory) {
            $factoryFile = Get-Item $OutputFactory
            $factorySizeKB = [math]::Round($factoryFile.Length/1KB, 2)
            Write-Success "Factory image created: $OutputFactory (${factorySizeKB} KB)"
        } else {
            throw "Factory output file not created: $OutputFactory"
        }
    } catch {
        Write-Error "Failed to create factory image: $_"
        Write-Info "Application hex still available: $OutputFinal"
    }
}

Write-Host "`n============================================================" -ForegroundColor Green
Write-Host "                   BUILD COMPLETE" -ForegroundColor Green
Write-Host "============================================================" -ForegroundColor Green

##############################################################################
# STEP 4: Create OTA Image (Metadata Prepended)
##############################################################################
Write-Step "STEP 4/4: Creating OTA Image with Prepended Metadata..."

# OTA image filename: Insert 'O' before CoreVer
# RF_6175242600_V01R20_V85R02 -> RF_6175242600_V01R20_OV85R02
$OTABaseName = "RF_${StockNo}_V${StockVersionStr}R${StockRevisionStr}_OV${CoreVerStr}R${CoreRevStr}"
$OutputOTA = "${OTABaseName}_crc_csum.hex"
$MetaTemp = "${OTABaseName}_meta_temp.hex"

Write-Info "OTA image will be: $OutputOTA"
Write-Info "Extracting metadata from 0x3FF00 and moving to 0x04B00..."

try {
    # Step 4.1: Extract metadata from 0x3FF00-0x3FFFF and relocate to 0x04B00
    Write-Host "  [4.1] Extracting and relocating metadata..." -ForegroundColor Gray
    & $SREC_CAT `
        $OutputFinal -Intel `
        -crop 0x3FF00 0x40000 `
        -offset -0x3B400 `
        -o $MetaTemp -Intel
    
    if ($LASTEXITCODE -ne 0) {
        throw "srec_cat metadata extraction failed with exit code: $LASTEXITCODE"
    }
    
    Write-Success "Metadata extracted and relocated to 0x04B00"
    
    # Step 4.2: Create application without metadata region (0x04C00 to 0x3FEFF only)
    Write-Host "  [4.2] Creating application without metadata at end..." -ForegroundColor Gray
    & $SREC_CAT `
        $OutputFinal -Intel `
        -crop 0x04C00 0x3FF00 `
        -o "${OTABaseName}_app_only.hex" -Intel
    
    if ($LASTEXITCODE -ne 0) {
        throw "srec_cat application crop failed with exit code: $LASTEXITCODE"
    }
    
    # Step 4.3: Merge relocated metadata with application
    Write-Host "  [4.3] Merging metadata at 0x04B00 with application..." -ForegroundColor Gray
    & $SREC_CAT `
        $MetaTemp -Intel `
        "${OTABaseName}_app_only.hex" -Intel `
        -o $OutputOTA -Intel
    
    if ($LASTEXITCODE -ne 0) {
        throw "srec_cat OTA merge failed with exit code: $LASTEXITCODE"
    }
    
    # Step 4.4: Convert hex to binary format
    Write-Host "  [4.4] Converting to binary format..." -ForegroundColor Gray
    $OutputOTABin = $OutputOTA -replace '\.hex$', '.bin'
    & $SREC_CAT `
        $OutputOTA -Intel `
        -crop 0x04B00 0x40000 `
        -offset -0x04B00 `
        -o $OutputOTABin -Binary
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host "[INFO] Binary conversion failed: srec_cat binary conversion failed with exit code: $LASTEXITCODE" -ForegroundColor Yellow
        Write-Host "[INFO] Hex file is still available: $OutputOTA" -ForegroundColor Yellow
        $OutputOTABin = ""  # Clear variable on failure
    } else {
        if (Test-Path $OutputOTABin) {
            $binFile = Get-Item $OutputOTABin
            $otaBinSizeKB = [math]::Round($binFile.Length/1KB, 2)
            Write-Success "OTA binary created: $OutputOTABin (${otaBinSizeKB} KB)"
        }
    }
    
    # Clean up temporary files
    if (Test-Path $MetaTemp) {
        Remove-Item $MetaTemp -Force
    }
    if (Test-Path "${OTABaseName}_app_only.hex") {
        Remove-Item "${OTABaseName}_app_only.hex" -Force
        Write-Info "Removed temporary files"
    }
    
    if (Test-Path $OutputOTA) {
        $otaFile = Get-Item $OutputOTA
        $otaSizeKB = [math]::Round($otaFile.Length/1KB, 2)
        Write-Success "OTA hex created: $OutputOTA (${otaSizeKB} KB)"
        Write-Info "Memory layout: Metadata (0x04B00) + Application (0x04C00+)"
        Write-Info "Original metadata at 0x3FF00 removed for OTA"
    } else {
        throw "OTA output file not created: $OutputOTA"
    }
} catch {
    Write-Error "Failed to create OTA image: $_"
    Write-Info "Application hex still available: $OutputFinal"
    # Clean up temp file on error
    if (Test-Path $MetaTemp) {
        Remove-Item $MetaTemp -Force
    }
}

##############################################################################
# STEP 5: Create Release Package (Folder + Copy + Zip)
##############################################################################
Write-Step "STEP 5/5: Creating Release Package..."

try {
    # Create package folder name (use version string)
    $PackageFolderName = "${VERSION_STRING}_OTA_PACKAGE_SOFT"
    $PackageFolderPath = Join-Path $WORK_PATH $PackageFolderName

    # Remove old package folder if exists
    if (Test-Path $PackageFolderPath) {
        Remove-Item $PackageFolderPath -Recurse -Force
    }

    # Create new folder
    New-Item -ItemType Directory -Path $PackageFolderPath | Out-Null
    Write-Success "Created package folder: $PackageFolderName"

    # Copy generated files
    Copy-Item $OutputFinal -Destination $PackageFolderPath -Force

    if (Test-Path $OutputFactory) {
        Copy-Item $OutputFactory -Destination $PackageFolderPath -Force
    }

    if (Test-Path $OutputOTA) {
        Copy-Item $OutputOTA -Destination $PackageFolderPath -Force
    }

    if (Test-Path $OutputOTABin) {
        Copy-Item $OutputOTABin -Destination $PackageFolderPath -Force
    }

    Write-Success "Copied generated HEX/BIN files"

    # Copy config folder from sources
    $ConfigSourcePath = Join-Path $PROJECT_ROOT "..\sources\config"
    $ConfigDestPath = Join-Path $PackageFolderPath "config"

    if (Test-Path $ConfigSourcePath) {
        Copy-Item $ConfigSourcePath -Destination $ConfigDestPath -Recurse -Force
        Write-Success "Copied config folder"
    } else {
        Write-Host "[INFO] Config folder not found at $ConfigSourcePath" -ForegroundColor Yellow
    }

    # Create BUILD_INFO.txt
    $NoteFilePath = Join-Path $PackageFolderPath "README_INFO.txt"
    $BuildDate = Get-Date -Format "yyyy-MM-dd HH:mm:ss"

    $NoteContent = @"
============================================================
                   BUILD COMPLETE
============================================================

Build Date:
  $BuildDate

Version Info:
  Stock No:       $StockNo
  Stock Version:  V${StockVersionStr}R${StockRevisionStr}
  Core Version:   V${CoreVerStr}R${CoreRevStr}

Generated Files:
  1. Application; Original file:        $OutputFinal
  2. Factory; OTA function (Full):      $OutputFactory
  3. OTA Ready; OTA Upload (HEX):       $OutputOTA
  4. OTA Ready; OTA Upload (BIN):       $OutputOTABin

File Sizes:
  Original:      ${inputSizeKB} KB
  Application:   ${finalSizeKB} KB (CRC32 + Checksum)
  Factory:       ${factorySizeKB} KB (Bootloader + Application)
  OTA HEX:       ${otaSizeKB} KB (Metadata prepended)
  OTA BIN:       ${otaBinSizeKB} KB (Binary format)

============================================================
"@

    $NoteContent | Out-File -FilePath $NoteFilePath -Encoding UTF8
    Write-Success "Created BUILD_INFO.txt"

    # Create ZIP file
    $ZipPath = "${PackageFolderPath}.zip"

    if (Test-Path $ZipPath) {
        Remove-Item $ZipPath -Force
    }

    Compress-Archive -Path "$PackageFolderPath\*" -DestinationPath $ZipPath

    Write-Success "ZIP package created: $ZipPath"

    # Delete package folder after zip completed
    if (Test-Path $ZipPath) {
        Remove-Item $PackageFolderPath -Recurse -Force
        Write-Success "Temporary package folder deleted"
    }

} catch {
    Write-Error "Failed to create release package: $_"
}

Write-Host "`n============================================================" -ForegroundColor Green
Write-Host "                   BUILD COMPLETE" -ForegroundColor Green
Write-Host "============================================================" -ForegroundColor Green

Write-Host "`nVersion Info:" -ForegroundColor Cyan
Write-Host "  Stock No:       $StockNo" -ForegroundColor White
Write-Host "  Stock Version:  V${StockVersionStr}R${StockRevisionStr}" -ForegroundColor White
Write-Host "  Core Version:   V${CoreVerStr}R${CoreRevStr}" -ForegroundColor White

Write-Host "`nGenerated Files:" -ForegroundColor Cyan
Write-Host "  1. Application:        $OutputFinal" -ForegroundColor White
if (Test-Path $OutputFactory) {
    Write-Host "  2. Factory (Full):     $OutputFactory" -ForegroundColor Green
}
if (Test-Path $OutputOTA) {
    Write-Host "  3. OTA Ready (HEX):    $OutputOTA" -ForegroundColor Yellow
}
if (Test-Path $OutputOTABin) {
    Write-Host "  4. OTA Ready (BIN):    $OutputOTABin" -ForegroundColor Cyan
}

Write-Host "`nFile Sizes:" -ForegroundColor Cyan
Write-Host "  Original:      ${inputSizeKB} KB" -ForegroundColor Gray
Write-Host "  Application:   ${finalSizeKB} KB (CRC32 + Checksum)" -ForegroundColor White
if (Test-Path $OutputFactory) {
    Write-Host "  Factory:       ${factorySizeKB} KB (Bootloader + Application)" -ForegroundColor Green
}
if (Test-Path $OutputOTA) {
    Write-Host "  OTA HEX:       ${otaSizeKB} KB (Metadata prepended)" -ForegroundColor Yellow
}
if (Test-Path $OutputOTABin) {
    Write-Host "  OTA BIN:       ${otaBinSizeKB} KB (Binary format)" -ForegroundColor Cyan
}

Write-Host "`nMemory Layout:" -ForegroundColor Cyan
if (Test-Path $OutputFactory) {
    Write-Host "  Factory Image:" -ForegroundColor Yellow
    Write-Host "    0x00000 - 0x04BFF : Bootloader (19 KB)" -ForegroundColor Gray
    Write-Host "    0x04C00 - 0x3FFFF : Application (~250 KB)" -ForegroundColor Gray
}
if (Test-Path $OutputOTA) {
    Write-Host "  OTA Image:" -ForegroundColor Yellow
    Write-Host "    0x04B00 - 0x04BFF : Metadata Header (256 bytes)" -ForegroundColor Cyan
    Write-Host "    0x04C00 - 0x3FEFF : Application Code" -ForegroundColor Gray
    Write-Host "    0x3FF00 - 0x3FFFF : [Removed - was metadata]" -ForegroundColor DarkGray
}
Write-Host "  CRC32:        0x03FF44 - 0x03FF47 (4 bytes)" -ForegroundColor Yellow
Write-Host "  Checksum:     0x3FF54 (1 byte)" -ForegroundColor Yellow

Write-Host "`nUsage:" -ForegroundColor Cyan
Write-Host "  Application hex: Base image with CRC/Checksum" -ForegroundColor White
if (Test-Path $OutputFactory) {
    Write-Host "  Factory hex:     For initial card programming (includes bootloader)" -ForegroundColor White
}
if (Test-Path $OutputOTA) {
    Write-Host "  OTA hex:         For OTA server upload (metadata prepended)" -ForegroundColor White
}
if (Test-Path $OutputOTABin) {
    Write-Host "  OTA bin:         For OTA server upload (binary format, smaller size)" -ForegroundColor White
}

Write-Host "`n============================================================`n" -ForegroundColor Cyan
