@echo off
setlocal

if not exist zipwizard.exe (
  call build.bat
  if errorlevel 1 exit /b 1
)

zipwizard.exe
endlocal
