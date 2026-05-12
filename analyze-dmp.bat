@echo off
setlocal

set "ROOT=%~dp0"
set "LLDB=%ROOT%build-deps\llvm-mingw\bin\lldb.exe"
set "EXE=%ROOT%build-ninja\eZeus.exe"

if "%~1"=="" (
  for /f "delims=" %%F in ('dir /b /o-d "%USERPROFILE%\Desktop\ezeus_crash_*.dmp" 2^>nul') do (
    set "DMP=%USERPROFILE%\Desktop\%%F"
    goto :found_dump
  )
  echo No dump found on Desktop.
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
"%LLDB%" -b ^
  -o "target create \"%EXE%\" --core \"%DMP%\"" ^
  -o "thread list" ^
  -o "thread backtrace all" ^
  -o "frame variable" ^
  -o "register read"
