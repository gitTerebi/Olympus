@echo off
setlocal

rem Assemble a clean, redistributable Olympus release folder.
rem Ships NO copyrighted game data (no DATA/, no saves). Player supplies their
rem own Zeus: Master of Olympus + Poseidon install. Run build.bat first.

set "ROOT=%~dp0"
set "ROOT=%ROOT:~0,-1%"
set "BUILD_DIR=%ROOT%\build-ninja"
set "VER=0.1.0-rc.1"
set "DIST=%ROOT%\dist\Olympus-%VER%"
set "LIVE=G:\games\eZeus\olympus"

if not exist "%BUILD_DIR%\Olympus.exe" (
    echo Olympus.exe not found in %BUILD_DIR%. Run build.bat first.
    exit /b 1
)

echo Cleaning %DIST% ...
if exist "%DIST%" rmdir /S /Q "%DIST%"
mkdir "%DIST%\Bin"

echo Copying binaries ...
copy /Y "%BUILD_DIR%\Olympus.exe"      "%DIST%\Bin\" >nul
copy /Y "%BUILD_DIR%\SDL2.dll"         "%DIST%\Bin\" >nul
copy /Y "%BUILD_DIR%\SDL2_image.dll"   "%DIST%\Bin\" >nul
copy /Y "%BUILD_DIR%\SDL2_mixer.dll"   "%DIST%\Bin\" >nul
copy /Y "%BUILD_DIR%\SDL2_ttf.dll"     "%DIST%\Bin\" >nul
copy /Y "%BUILD_DIR%\libc++.dll"       "%DIST%\Bin\" >nul
copy /Y "%BUILD_DIR%\libunwind.dll"    "%DIST%\Bin\" >nul
if exist "%LIVE%\Bin\upscaled_icon.ico" copy /Y "%LIVE%\Bin\upscaled_icon.ico" "%DIST%\Bin\" >nul

echo Copying support assets ...
xcopy /E /I /Y /Q "%LIVE%\cursors"      "%DIST%\cursors"      >nul
xcopy /E /I /Y /Q "%LIVE%\Sanctuaries"  "%DIST%\Sanctuaries"  >nul
xcopy /E /I /Y /Q "%LIVE%\stamps"       "%DIST%\stamps"       >nul
mkdir "%DIST%\Text" >nul 2>&1
copy /Y "%LIVE%\Text\language.txt"      "%DIST%\Text\"        >nul
mkdir "%DIST%\Adventures" >nul 2>&1
mkdir "%DIST%\Save"       >nul 2>&1

echo Copying font ...
mkdir "%DIST%\Fonts" >nul 2>&1
copy /Y "%ROOT%\fonts\Zeus.ttf"         "%DIST%\Fonts\"       >nul

echo Copying docs / redist ...
if exist "%ROOT%\dist-README.txt"     copy /Y "%ROOT%\dist-README.txt"     "%DIST%\README.txt" >nul
if exist "%LIVE%\VC_redist.x64.exe"   copy /Y "%LIVE%\VC_redist.x64.exe"   "%DIST%\" >nul

echo.
echo Dist ready: %DIST%
echo Excluded: DATA\, Save\Tim, settings.txt (player-specific / copyrighted).
endlocal
