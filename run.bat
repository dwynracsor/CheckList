@echo off
set PATH=C:\Qt\6.6.3\mingw_64\bin;C:\Users\User\AppData\Local\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.MSVCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin;%PATH%
set QT_QUICK_BACKEND=software
cd /d "%~dp0build"
start "" ChecklistApp.exe
