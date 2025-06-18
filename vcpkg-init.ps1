git submodule add https://github.com/microsoft/vcpkg.git
git submodule update --init

# Bootstrap Vcpkg (Windows .bat file requires call via cmd.exe)
cmd.exe /c ".\vcpkg\bootstrap-vcpkg.bat"

# Set environment variables
$solutionDir = Get-Location
$env:VCPKG_ROOT = "$solutionDir\vcpkg"
$env:PATH = "$env:VCPKG_ROOT;$env:PATH"

# Run vcpkg
.\vcpkg\vcpkg.exe new --application
