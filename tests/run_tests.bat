@echo off
setlocal
set "ROOT=%~dp0.."
set "CXX=%ROOT%\build-deps\llvm-mingw\bin\g++.exe"
set "OUT=%ROOT%\tests\sim_sheep.exe"

if not exist "%CXX%" (
    echo ERROR: compiler not found: %CXX%
    exit /b 1
)

echo [build] sim_sheep.cpp
"%CXX%" -std=c++17 -O2 -o "%OUT%" "%ROOT%\tests\sim_sheep.cpp"
if errorlevel 1 (
    echo [FAIL] build error
    exit /b 1
)

echo [run]
"%OUT%" %*
exit /b %errorlevel%
