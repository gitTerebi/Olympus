@echo off
setlocal

set "ROOT=%~dp0"
set "LLDB=%ROOT%build-deps\llvm-mingw\bin\lldb.exe"
set "EXE=%ROOT%build-ninja\eZeus.exe"

if "%~1"=="" (
  for /f "delims=" %%F in ('dir /b /o-d "%ROOT%debug\ezeus-*.dmp" 2^>nul') do (
    set "DMP=%ROOT%debug\%%F"
    goto :found_dump
  )
  for /f "delims=" %%F in ('dir /b /o-d "%USERPROFILE%\Desktop\ezeus_crash_*.dmp" 2^>nul') do (
    set "DMP=%USERPROFILE%\Desktop\%%F"
    goto :found_dump
  )
  echo No dump found in "%ROOT%debug" or on Desktop.
  exit /b 1
) else (
  set "DMP=%~1"
)

:found_dump
if not exist "%LLDB%" (
  echo Missing LLDB: "%LLDB%"
  exit /b 1
)
if not exist "%EXE%" (
  echo Missing exe: "%EXE%"
  exit /b 1
)
if not exist "%DMP%" (
  echo Missing dump: "%DMP%"
  exit /b 1
)

echo Dump: "%DMP%"
set "SUMMARY=%DMP%.crash-thread.txt"
set "FULL=%DMP%.all-threads.txt"

echo.
echo ===== Crash thread summary =====
"%LLDB%" -b ^
  -o "target create \"%EXE%\" --core \"%DMP%\"" ^
  -o "process status" ^
  -o "thread info" ^
  -o "thread backtrace" ^
  -o "frame info" ^
  -o "frame variable" ^
  -o "register read" ^
  -o "thread list" ^
  > "%SUMMARY%" 2>&1
set "LLDB_EXIT=%ERRORLEVEL%"
type "%SUMMARY%"

echo.
echo ===== Full all-thread backtrace saved to =====
echo "%FULL%"
"%LLDB%" -b ^
  -o "target create \"%EXE%\" --core \"%DMP%\"" ^
  -o "thread list" ^
  -o "thread backtrace all" ^
  > "%FULL%" 2>&1

echo Deleting DMP files...
if /i "%DMP:~-4%"==".dmp" if exist "%DMP%" (
  echo Deleting "%DMP%"
  call :delete_file "%DMP%"
)
for %%F in ("%ROOT%debug\ezeus-*.dmp") do if exist "%%~fF" (
  echo Deleting "%%~fF"
  call :delete_file "%%~fF"
)
goto :eof

:delete_file
setlocal
set "file=%~1"
:retry_del
del /q "%file%" >nul 2>nul
if exist "%file%" (
  echo Retrying delete for "%file%"
  timeout /t 1 /nobreak >nul
  goto retry_del
)
echo Successfully deleted "%file%"
goto :eof

exit /b %LLDB_EXIT%
