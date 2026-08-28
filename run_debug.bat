@echo off
set PATH=C:\Qt\6.6.3\mingw_64\bin;C:\msys64\mingw64\bin;%PATH%
cd /d "%~dp0build"
ChecklistApp.exe > "%TEMP%\checklist_stdout.log" 2> "%TEMP%\checklist_stderr.log"
echo Exit code: %ERRORLEVEL%
echo.
echo === STDOUT ===
type "%TEMP%\checklist_stdout.log"
echo.
echo === STDERR ===
type "%TEMP%\checklist_stderr.log"
pause
