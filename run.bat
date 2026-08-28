@echo off
set PATH=C:\Qt\6.6.3\mingw_64\bin;C:\msys64\mingw64\bin;%PATH%
set QT_QUICK_BACKEND=software
cd /d "%~dp0build"
start "" ChecklistApp.exe
