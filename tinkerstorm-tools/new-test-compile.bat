@echo off
setlocal
set "PS=powershell -NoProfile -Command"

%PS% "Write-Host ([char]0xf013) ' BUILD TINKERSTORM (test - no git interaction)' -ForegroundColor Cyan"
echo.
cd /d C:\FS_Build\phoenix-firestorm
if errorlevel 1 (
    %PS% "Write-Host ([char]0xf071) ' ERROR: Could not find C:\FS_Build\phoenix-firestorm - aborting.' -ForegroundColor Red"
    pause
    exit /b 1
)

set "source=C:\FS_Build\Tinkerstorm installer\firestorm_icon.ico"
copy /y "%source%" "C:\FS_Build\phoenix-firestorm\indra\newview\icons\beta\firestorm_icon.ico"
copy /y "%source%" "C:\FS_Build\phoenix-firestorm\indra\newview\icons\beta-os\firestorm_icon.ico"
copy /y "%source%" "C:\FS_Build\phoenix-firestorm\indra\newview\icons\development\firestorm_icon.ico"
copy /y "%source%" "C:\FS_Build\phoenix-firestorm\indra\newview\icons\development-os\firestorm_icon.ico"
copy /y "%source%" "C:\FS_Build\phoenix-firestorm\indra\newview\icons\nightly\firestorm_icon.ico"
copy /y "%source%" "C:\FS_Build\phoenix-firestorm\indra\newview\icons\nightly-os\firestorm_icon.ico"
copy /y "%source%" "C:\FS_Build\phoenix-firestorm\indra\newview\icons\private\firestorm_icon.ico"
copy /y "%source%" "C:\FS_Build\phoenix-firestorm\indra\newview\icons\private-os\firestorm_icon.ico"
copy /y "%source%" "C:\FS_Build\phoenix-firestorm\indra\newview\icons\project\firestorm_icon.ico"
copy /y "%source%" "C:\FS_Build\phoenix-firestorm\indra\newview\icons\project-os\firestorm_icon.ico"
copy /y "%source%" "C:\FS_Build\phoenix-firestorm\indra\newview\icons\release\firestorm_icon.ico"
copy /y "%source%" "C:\FS_Build\phoenix-firestorm\indra\newview\icons\release-os\firestorm_icon.ico"
copy /y "%source%" "C:\FS_Build\phoenix-firestorm\indra\newview\icons\test\firestorm_icon.ico"
copy /y "%source%" "C:\FS_Build\phoenix-firestorm\indra\newview\icons\test-os\firestorm_icon.ico"
copy /y "%source%" "C:\FS_Build\phoenix-firestorm\indra\newview\installers\windows\firestorm_icon.ico"
copy /y "%source%" "C:\FS_Build\phoenix-firestorm\indra\newview\installers\windows\firestorm_icon_os.ico"
copy /y "%source%" "C:\FS_Build\phoenix-firestorm\indra\newview\res\firestorm_icon.ico"
copy /y "%source%" "C:\FS_Build\phoenix-firestorm\indra\win_crash_logger\firestorm_icon.ico"

echo.
%PS% "Write-Host ([char]0xf013) ' Registering FMOD package...'"
autobuild installables edit fmodstudio platform=windows64 hash=0c2ebac91d44914517137a5d1dda64e9 url=file:///C:/FS_Build/3p-fmodstudio/fmodstudio-2.03.13-windows64-262401839.tar.bz2

echo.
%PS% "Write-Host ([char]0xf013) ' Configuring...'"
autobuild configure -A 64 -c ReleaseFS_open -- --chan Tinkerstorm --fmodstudio --avx2 --lto --package -DLL_TESTS:BOOL=FALSE
if errorlevel 1 (
    %PS% "Write-Host ([char]0xf071) ' ERROR: Configure failed - aborting before the build step.' -ForegroundColor Red"
    pause
    exit /b 1
)

echo.
%PS% "Write-Host ([char]0xf013) ' Building - this will take a while. Log is being written to build_log.txt'"
autobuild build -A 64 -c ReleaseFS_open --no-configure > build_log.txt 2>&1

echo.
%PS% "Write-Host ([char]0xf013) ' BUILD FINISHED - CHECKING RESULT'"
findstr /I "error" build_log.txt > nul
if errorlevel 1 (
    %PS% "Write-Host ([char]0xf00c) ' No errors found in build_log.txt.' -ForegroundColor Green"
) else (
    %PS% "Write-Host ([char]0xf071) ' Errors were found - check build_log.txt for details:' -ForegroundColor Red"
    findstr /I "error" build_log.txt
)

echo.
if exist "build-vc170-64\newview\Release\firestorm-bin.exe" (
    %PS% "Write-Host ([char]0xf00c) ' firestorm-bin.exe exists - build succeeded.' -ForegroundColor Green"
) else (
    %PS% "Write-Host ([char]0xf071) ' firestorm-bin.exe NOT found - build likely failed.' -ForegroundColor Red"
)

echo.
pause
