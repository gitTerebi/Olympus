$ErrorActionPreference = "Stop"
$depsDir  = "$PSScriptRoot\build-deps"
$sdlDir   = "$depsDir\SDL2"
$llvmDir  = "$depsDir\llvm-mingw"
$tmp      = $env:TEMP

$downloads = @(
    @{ url = "https://github.com/libsdl-org/SDL/releases/download/release-2.30.8/SDL2-devel-2.30.8-mingw.zip";           out = "SDL2-2.30.8.zip";       dest = $sdlDir }
    @{ url = "https://github.com/libsdl-org/SDL_image/releases/download/release-2.8.2/SDL2_image-devel-2.8.2-mingw.zip"; out = "SDL2_image-2.8.2.zip";  dest = $sdlDir }
    @{ url = "https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.22.0/SDL2_ttf-devel-2.22.0-mingw.zip";   out = "SDL2_ttf-2.22.0.zip";   dest = $sdlDir }
    @{ url = "https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.8.0/SDL2_mixer-devel-2.8.0-mingw.zip"; out = "SDL2_mixer-2.8.0.zip";  dest = $sdlDir }
    @{ url = "https://github.com/mstorsjo/llvm-mingw/releases/download/20241119/llvm-mingw-20241119-ucrt-x86_64.zip";    out = "llvm-mingw.zip";        dest = $depsDir }
)

Add-Type -AssemblyName System.IO.Compression.FileSystem

Write-Host "Removing old deps..."
Remove-Item -Recurse -Force $sdlDir  -ErrorAction SilentlyContinue
Remove-Item -Recurse -Force $llvmDir -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Force $sdlDir | Out-Null

foreach ($d in $downloads) {
    $zip = "$tmp\$($d.out)"
    Write-Host "Downloading $($d.out)..."
    Invoke-WebRequest -Uri $d.url -OutFile $zip -UseBasicParsing
    Write-Host "Extracting $($d.out)..."
    [System.IO.Compression.ZipFile]::ExtractToDirectory($zip, $d.dest)
    Remove-Item $zip
}

# rename llvm-mingw versioned folder
$extracted = Get-ChildItem $depsDir -Directory | Where-Object { $_.Name -like "llvm-mingw-*" } | Select-Object -First 1
if ($extracted) { Rename-Item $extracted.FullName "llvm-mingw" }

# Copy SDL2 base headers into SDL2_image/ttf/mixer include dirs
Write-Host "Copying SDL2 base headers..."
$sdlBase = "$sdlDir\SDL2-2.30.8\x86_64-w64-mingw32\include\SDL2"
foreach ($lib in @("SDL2_image-2.8.2","SDL2_ttf-2.22.0","SDL2_mixer-2.8.0")) {
    $dest = "$sdlDir\$lib\x86_64-w64-mingw32\include\SDL2"
    Copy-Item "$sdlBase\*" $dest -Force
}

Write-Host "All deps restored. Run .\build.bat"
