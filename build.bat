@echo off
setlocal

set "ROOT=%~dp0"
set "ROOT=%ROOT:~0,-1%"
set "MINGW=%ROOT%\deps\llvm-mingw\bin"
set "BUILD_DIR=%ROOT%\build-cmake"

if not exist "%MINGW%\mingw32-make.exe" (
    echo Missing "%MINGW%\mingw32-make.exe"
    exit /b 1
)

if not exist "%ROOT%\deps\SDL2" (
    echo Missing "%ROOT%\deps\SDL2"
    exit /b 1
)

set "PATH=%MINGW%;%PATH%"

cmake -S "%ROOT%" -B "%BUILD_DIR%" -G "MinGW Makefiles" ^
    -DCMAKE_MAKE_PROGRAM="%MINGW%\mingw32-make.exe" ^
    -DCMAKE_C_COMPILER="%MINGW%\gcc.exe" ^
    -DCMAKE_CXX_COMPILER="%MINGW%\g++.exe" ^
    -DCMAKE_BUILD_TYPE=Release

if errorlevel 1 exit /b %errorlevel%

cmake --build "%BUILD_DIR%" --config Release -j 4

if errorlevel 1 exit /b %errorlevel%

echo Built "%BUILD_DIR%\eZeus.exe"
