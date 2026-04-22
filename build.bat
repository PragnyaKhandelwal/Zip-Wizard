@echo off
setlocal

where gcc >nul 2>nul
if errorlevel 1 (
  echo [ERROR] gcc not found in PATH.
  exit /b 1
)

gcc src\*.c -I include -Wall -Wextra -O2 -o zipwizard.exe
if errorlevel 1 (
  echo [ERROR] Build failed.
  exit /b 1
)

echo [OK] Build successful: zipwizard.exe
endlocal
