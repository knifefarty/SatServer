<#
.SYNOPSIS
    Automated Setup & Staging Script for LocalServerConnect Satisfactory Mod
.DESCRIPTION
    1. Clones the official SatisfactoryModStarterProject into the development workspace (if not present).
    2. Links or copies the LocalServerConnect mod plugin into SatisfactoryModStarterProject/Mods/LocalServerConnect.
    3. Deploys configuration to the live Satisfactory installation.
#>

param(
    [string]$TargetDevDir = "j:\productivity-server\Satisfactory Mod\StarterProject",
    [string]$GameModsDir = "K:\SteamLibrary\steamapps\common\Satisfactory\FactoryGame\Mods"
)

Write-Host "==========================================================" -ForegroundColor Cyan
Write-Host "   LocalServerConnect - Satisfactory Mod Setup Script     " -ForegroundColor Cyan
Write-Host "==========================================================" -ForegroundColor Cyan

# 1. Check Git
if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
    Write-Error "Git is required but was not found in PATH."
    exit 1
}

# 2. Clone Starter Project if needed
if (-not (Test-Path $TargetDevDir)) {
    Write-Host "[1/3] Cloning SatisfactoryModStarterProject into '$TargetDevDir'..." -ForegroundColor Yellow
    New-Item -ItemType Directory -Path $TargetDevDir -Force | Out-Null
    git clone --depth 1 https://github.com/satisfactorymodding/SatisfactoryModStarterProject.git $TargetDevDir
} else {
    Write-Host "[1/3] Starter Project directory already exists at '$TargetDevDir'." -ForegroundColor Green
}

# 3. Stage Mod into Starter Project
$PluginDest = Join-Path $TargetDevDir "Mods\LocalServerConnect"
Write-Host "[2/3] Staging LocalServerConnect plugin into '$PluginDest'..." -ForegroundColor Yellow

if (-not (Test-Path "$TargetDevDir\Mods")) {
    New-Item -ItemType Directory -Path "$TargetDevDir\Mods" -Force | Out-Null
}

$SourceDir = $PSScriptRoot
Copy-Item -Path "$SourceDir\LocalServerConnect.uplugin" -Destination $PluginDest -Force
Copy-Item -Path "$SourceDir\Source" -Destination $PluginDest -Recurse -Force
Copy-Item -Path "$SourceDir\README.md" -Destination $PluginDest -Force

Write-Host "[2/3] Plugin files copied successfully." -ForegroundColor Green

# 4. Verify Live Game Staging
Write-Host "[3/3] Verifying Satisfactory installation..." -ForegroundColor Yellow
if (Test-Path $GameModsDir) {
    Write-Host "Found active Satisfactory Mods folder at: $GameModsDir" -ForegroundColor Green
    $LiveModDir = Join-Path $GameModsDir "LocalServerConnect"
    if (-not (Test-Path $LiveModDir)) {
        New-Item -ItemType Directory -Path $LiveModDir -Force | Out-Null
    }
    Copy-Item -Path "$SourceDir\LocalServerConnect.uplugin" -Destination "$LiveModDir\LocalServerConnect.uplugin" -Force
    Write-Host "Created target mod directory at '$LiveModDir'." -ForegroundColor Green
} else {
    Write-Warning "Could not locate live Satisfactory Mods folder at '$GameModsDir'."
}

Write-Host "`nSetup complete! Open '$TargetDevDir\FactoryGame.uproject' in Unreal Engine 5.3 to build/package." -ForegroundColor Cyan
