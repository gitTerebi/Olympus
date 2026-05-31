@echo off
setlocal

set "ROOT=%~dp0"
set "ROOT=%ROOT:~0,-1%"
set "MINGW=%ROOT%\build-deps\llvm-mingw\bin"
set "NINJA=%ROOT%\build-deps\ninja\ninja.exe"
set "BUILD_DIR=%ROOT%\build-ninja"
set "DEPLOY_DIR=G:\games\eZeus\eZeus-0.8.2-beta\Bin"
set "JOBS=%NUMBER_OF_PROCESSORS%"
if "%JOBS%"=="" set "JOBS=8"

if not exist "%MINGW%\mingw32-make.exe" (
    echo Missing "%MINGW%\mingw32-make.exe"
    exit /b 1
)

if not exist "%ROOT%\build-deps\SDL2" (
    echo Missing "%ROOT%\build-deps\SDL2"
    exit /b 1
)

if not exist "%NINJA%" (
    echo Missing "%NINJA%"
    exit /b 1
)

set "PATH=%MINGW%;%PATH%"

if not exist "%BUILD_DIR%\CMakeCache.txt" (
    cmake -S "%ROOT%" -B "%BUILD_DIR%" -G "Ninja" ^
        -DCMAKE_MAKE_PROGRAM="%NINJA%" ^
        -DCMAKE_C_COMPILER="%MINGW%\gcc.exe" ^
        -DCMAKE_CXX_COMPILER="%MINGW%\g++.exe" ^
        -DCMAKE_BUILD_TYPE=Release

    if errorlevel 1 exit /b %errorlevel%
)

cmake --build "%BUILD_DIR%" --config Release -j "%JOBS%"

if errorlevel 1 exit /b %errorlevel%

if exist "%DEPLOY_DIR%" (
    copy /Y "%BUILD_DIR%\Olympus.exe" "%DEPLOY_DIR%\Olympus.exe" >nul
    copy /Y "%BUILD_DIR%\SDL2.dll" "%DEPLOY_DIR%\" >nul
    copy /Y "%BUILD_DIR%\SDL2_image.dll" "%DEPLOY_DIR%\" >nul
    copy /Y "%BUILD_DIR%\SDL2_mixer.dll" "%DEPLOY_DIR%\" >nul
    copy /Y "%BUILD_DIR%\SDL2_ttf.dll" "%DEPLOY_DIR%\" >nul
    copy /Y "%BUILD_DIR%\libc++.dll" "%DEPLOY_DIR%\" >nul
    copy /Y "%BUILD_DIR%\libunwind.dll" "%DEPLOY_DIR%\" >nul
    echo Copied build files to "%DEPLOY_DIR%"
) else (
    echo Deploy dir not found "%DEPLOY_DIR%"
)

echo Built "%BUILD_DIR%\Olympus.exe"
