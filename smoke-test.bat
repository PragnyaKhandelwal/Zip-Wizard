@echo off
setlocal

call build.bat
if errorlevel 1 exit /b 1

zipwizard.exe --create smoke_test.txt "hello smoke" >nul
if errorlevel 1 (
  echo [ERROR] create command failed.
  exit /b 1
)

zipwizard.exe --rename smoke_test.txt smoke_test_renamed.txt >nul
if errorlevel 1 (
  echo [ERROR] rename command failed.
  exit /b 1
)

zipwizard.exe --delete smoke_test_renamed.txt >nul
if errorlevel 1 (
  echo [ERROR] delete command failed.
  exit /b 1
)

if exist smoke_test.txt del /q smoke_test.txt
if exist smoke_test_renamed.txt del /q smoke_test_renamed.txt
if exist smoke_test.txt.zwz del /q smoke_test.txt.zwz
if exist smoke_test.txt_output.txt del /q smoke_test.txt_output.txt

echo [OK] Smoke test passed.
endlocal
