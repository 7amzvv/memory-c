param([string]$ArchivePath = "")
$ErrorActionPreference = "Stop"
$compiler = Get-Command gcc -ErrorAction SilentlyContinue
if (-not $compiler) {
    throw "GCC est introuvable. Installer MinGW-w64 et ajouter son dossier bin au PATH."
}
$architecture = & $compiler.Source -dumpmachine
if ($LASTEXITCODE -ne 0 -or $architecture -ne "x86_64-w64-mingw32") {
    throw "Ce projet Windows utilise GCC MinGW-w64 en 64 bits."
}
$libDir = Join-Path $PSScriptRoot "lib"
$destination = Join-Path $libDir "raylib"
$packageDir = Join-Path $libDir "raylib-package"
New-Item -ItemType Directory -Force -Path $libDir, $destination, (Join-Path $PSScriptRoot "build") | Out-Null
if (-not $ArchivePath) {
    $ArchivePath = Join-Path $libDir "raylib-6.2.zip"
    Invoke-WebRequest "https://github.com/raysan5/raylib/releases/download/6.2/raylib-6.2_win64_mingw-w64.zip" -OutFile $ArchivePath
}
$expected = "48BA378D100FE7E13FC2C35D708764B855B815596F3B0A31746E37259283961F"
if ((Get-FileHash -LiteralPath $ArchivePath -Algorithm SHA256).Hash -ne $expected) {
    throw "L'archive raylib ne correspond pas a la version attendue."
}
Expand-Archive -LiteralPath $ArchivePath -DestinationPath $packageDir -Force
$package = Join-Path $packageDir "raylib-6.2_win64_mingw-w64"
Copy-Item -LiteralPath (Join-Path $package "include"), (Join-Path $package "lib") -Destination $destination -Recurse -Force
Write-Host "raylib est pret. Dans VS Code : Ctrl + Maj + B pour compiler."
