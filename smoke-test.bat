@echo off
setlocal

call build.bat
if errorlevel 1 exit /b 1

(
  echo 1
  echo smoke_test.txt
  echo hello smoke
  echo.
  echo 3
  echo smoke_test.txt
  echo smoke_test_renamed.txt
  echo.
  echo 4
  echo smoke_test_renamed.txt
  echo.
  echo 9
) | zipwizard.exe >nul

if exist smoke_test.txt del /q smoke_test.txt
if exist smoke_test_renamed.txt del /q smoke_test_renamed.txt
if exist smoke_test.txt.zip del /q smoke_test.txt.zip
if exist smoke_test.txt_output.txt del /q smoke_test.txt_output.txt

echo [OK] Smoke test passed.
endlocal
