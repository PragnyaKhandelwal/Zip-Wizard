@echo off
setlocal

where gcc >nul 2>nul
if errorlevel 1 (
  echo [ERROR] gcc not found in PATH.
  exit /b 1
)

gcc src\lz77_test.c src\zipFile.c src\unzipFile.c src\Utils.c src\fileIndex.c -I include -Wall -Wextra -O2 -o lz77_test.exe
if errorlevel 1 (
  echo [ERROR] Failed to build lz77_test.exe
  exit /b 1
)

lz77_test.exe
set TEST_EXIT=%ERRORLEVEL%

del /q lz77_test.exe >nul 2>nul
del /q test_*.txt >nul 2>nul
del /q test_*.zip >nul 2>nul
del /q test_*_out.txt >nul 2>nul

if not "%TEST_EXIT%"=="0" (
  echo [ERROR] LZ77 test harness failed.
  exit /b %TEST_EXIT%
)

echo [OK] LZ77 test harness passed.
endlocal
