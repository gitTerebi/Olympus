@echo off
setlocal

cmake -S . -B build-ninja -DEZEUS_BUILD_TESTS=ON
if errorlevel 1 exit /b %errorlevel%

cmake --build build-ninja --target save-archive-array-test
if errorlevel 1 exit /b %errorlevel%

cmake --build build-ninja --target resource-transport-size-test
if errorlevel 1 exit /b %errorlevel%

ctest --test-dir build-ninja --output-on-failure
exit /b %errorlevel%
