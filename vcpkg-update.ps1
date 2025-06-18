# setup.ps1 - Voor automatisch instellen van vcpkg na git clone

Write-Host "🔧 Setup start..."

$solutionDir = Get-Location
cd $solutionDir
# Zet pad naar lokale vcpkg directory
$VcpkgPath = Join-Path $solutionDir "vcpkg"
$VcpkgExe = Join-Path $VcpkgPath "vcpkg.exe"

# Controleer of vcpkg.exe bestaat
if (!(Test-Path $VcpkgExe)) {
    Write-Host "📦 vcpkg.exe niet gevonden. Bootstrapping..."
    Push-Location $VcpkgPath
    .\bootstrap-vcpkg.bat
    Pop-Location

    if (!(Test-Path $VcpkgExe)) {
        Write-Error "❌ vcpkg.exe kon niet worden aangemaakt. Check bootstrap output."
        exit 1
    } else {
        Write-Host "✅ vcpkg.exe succesvol gebouwd."
    }
} else {
    Write-Host "✅ vcpkg.exe bestaat al."
}

# Installeer dependencies via vcpkg manifest
Write-Host "📥 Dependencies installeren via manifest..."
& $VcpkgExe install --triplet x64-windows --vcpkg-root $VcpkgPath --x-manifest-root=$solutionDir

if ($LASTEXITCODE -ne 0) {
    Write-Error "❌ vcpkg install is mislukt met code $LASTEXITCODE"
    exit $LASTEXITCODE
} else {
    Write-Host "✅ Alle dependencies geïnstalleerd."
}

Write-Host "🎉 Setup voltooid!"