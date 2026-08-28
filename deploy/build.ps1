# ChecklistApp Deployment Script
# Builds and packages the application for all platforms

param(
    [Parameter(Mandatory=$false)]
    [ValidateSet("windows", "linux", "macos", "all")]
    [string]$Platform = "all",
    
    [Parameter(Mandatory=$false)]
    [ValidateSet("Debug", "Release")]
    [string]$Config = "Release",
    
    [Parameter(Mandatory=$false)]
    [switch]$Clean
)

$ErrorActionPreference = "Stop"
$ProjectRoot = Split-Path -Parent $PSScriptRoot
$BuildDir = Join-Path $ProjectRoot "build"

Write-Host "=== ChecklistApp Deployment ===" -ForegroundColor Cyan
Write-Host "Platform: $Platform" -ForegroundColor Yellow
Write-Host "Config: $Config" -ForegroundColor Yellow

# Clean build directory if requested
if ($Clean) {
    Write-Host "Cleaning build directory..." -ForegroundColor Gray
    if (Test-Path $BuildDir) {
        Remove-Item -Recurse -Force $BuildDir
    }
}

# Create build directory
if (!(Test-Path $BuildDir)) {
    New-Item -ItemType Directory -Path $BuildDir | Out-Null
}

# Build function
function Build-Project {
    param(
        [string]$BuildType
    )
    
    Write-Host "`n=== Building Project ===" -ForegroundColor Cyan
    
    # Configure CMake
    Write-Host "Configuring CMake..." -ForegroundColor Gray
    $cmakeArgs = @(
        "-B", $BuildDir,
        "-DCMAKE_BUILD_TYPE=$BuildType",
        "-DCMAKE_INSTALL_PREFIX=$BuildDir/install"
    )
    
    # Add vcpkg toolchain if available
    $vcpkgRoot = $env:VCPKG_ROOT
    if ($vcpkgRoot) {
        $toolchainFile = Join-Path $vcpkgRoot "scripts/buildsystems/vcpkg.cmake"
        if (Test-Path $toolchainFile) {
            $cmakeArgs += "-DCMAKE_TOOLCHAIN_FILE=$toolchainFile"
        }
    }
    
    & cmake @cmakeArgs
    
    if ($LASTEXITCODE -ne 0) {
        throw "CMake configuration failed"
    }
    
    # Build
    Write-Host "Building..." -ForegroundColor Gray
    & cmake --build $BuildDir --config $BuildType
    
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed"
    }
    
    # Install
    Write-Host "Installing..." -ForegroundColor Gray
    & cmake --install $BuildDir --config $BuildType
    
    if ($LASTEXITCODE -ne 0) {
        throw "Install failed"
    }
    
    Write-Host "Build completed successfully!" -ForegroundColor Green
}

# Windows deployment
function Deploy-Windows {
    Write-Host "`n=== Windows Deployment ===" -ForegroundColor Cyan
    
    $deployDir = Join-Path $ProjectRoot "deploy\windows"
    $releaseDir = Join-Path $BuildDir "install\bin"
    
    # Run windeployqt
    Write-Host "Running windeployqt..." -ForegroundColor Gray
    $windeployqt = Get-Command windeployqt6 -ErrorAction SilentlyContinue
    if (!$windeployqt) {
        $windeployqt = Get-Command windeployqt -ErrorAction SilentlyContinue
    }
    
    if ($windeployqt) {
        & $windeployqt.Source "$releaseDir\ChecklistApp.exe"
    } else {
        Write-Host "windeployqt not found. Please install Qt or add to PATH." -ForegroundColor Yellow
    }
    
    # Create NSIS installer
    $nsis = Get-Command makensis -ErrorAction SilentlyContinue
    if ($nsis) {
        Write-Host "Creating NSIS installer..." -ForegroundColor Gray
        & makensis (Join-Path $deployDir "installer.nsi")
    } else {
        Write-Host "NSIS not found. Please install NSIS or add to PATH." -ForegroundColor Yellow
    }
    
    Write-Host "Windows deployment completed!" -ForegroundColor Green
}

# Linux deployment
function Deploy-Linux {
    Write-Host "`n=== Linux Deployment ===" -ForegroundColor Cyan
    
    $deployDir = Join-Path $ProjectRoot "deploy\linux"
    $buildScript = Join-Path $deployDir "build-appimage.sh"
    
    if (Test-Path $buildScript) {
        Write-Host "Building AppImage..." -ForegroundColor Gray
        & bash $buildScript
    } else {
        Write-Host "AppImage build script not found." -ForegroundColor Yellow
    }
    
    Write-Host "Linux deployment completed!" -ForegroundColor Green
}

# macOS deployment
function Deploy-MacOS {
    Write-Host "`n=== macOS Deployment ===" -ForegroundColor Cyan
    
    $deployDir = Join-Path $ProjectRoot "deploy\macos"
    $buildScript = Join-Path $deployDir "build-dmg.sh"
    
    if (Test-Path $buildScript) {
        Write-Host "Building DMG..." -ForegroundColor Gray
        & bash $buildScript
    } else {
        Write-Host "DMG build script not found." -ForegroundColor Yellow
    }
    
    Write-Host "macOS deployment completed!" -ForegroundColor Green
}

# Main execution
try {
    # Build project
    Build-Project -BuildType $Config
    
    # Deploy based on platform
    switch ($Platform) {
        "windows" { Deploy-Windows }
        "linux" { Deploy-Linux }
        "macos" { Deploy-MacOS }
        "all" {
            if ($IsWindows -or $env:OS -eq "Windows_NT") {
                Deploy-Windows
            } elseif ($IsLinux) {
                Deploy-Linux
            } elseif ($IsMacOS) {
                Deploy-MacOS
            } else {
                Write-Host "Unknown platform. Skipping deployment." -ForegroundColor Yellow
            }
        }
    }
    
    Write-Host "`n=== Deployment Complete! ===" -ForegroundColor Green
    Write-Host "Build artifacts are in: $BuildDir" -ForegroundColor Gray
}
catch {
    Write-Host "`nError: $_" -ForegroundColor Red
    exit 1
}
