@echo off
setlocal enabledelayedexpansion

where gcc >nul 2>nul
if errorlevel 1 (
  echo [ERROR] gcc not found in PATH.
  exit /b 1
)

set SRCS=
for %%f in (src\*.c) do (
  echo %%~nxf | findstr /i /c:"lz77_test.c" >nul
  if errorlevel 1 (
    set SRCS=!SRCS! %%f
  )
)

gcc !SRCS! -I include -Wall -Wextra -O2 -o zipwizard.exe
if errorlevel 1 (
  echo [ERROR] Build failed.
  exit /b 1
)

echo [OK] Build successful: zipwizard.exe
endlocal
