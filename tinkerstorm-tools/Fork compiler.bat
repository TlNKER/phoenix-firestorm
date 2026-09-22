@echo off
echo ============================
echo   BUILD TINKERSTORM (master)
echo ============================
echo.
cd /d C:\FS_Build\phoenix-firestorm
if errorlevel 1 (
    echo ERROR: Could not find C:\FS_Build\phoenix-firestorm - aborting.
    pause
    exit /b 1
)
echo Discarding local uncommitted changes...
git checkout -- .
if errorlevel 1 (
    echo ERROR: git checkout failed - aborting before pull.
    pause
    exit /b 1
)
echo.
echo Forking latest master...
git config pull.rebase false
git pull myfork master
if errorlevel 1 (
    echo ERROR: git pull failed - aborting before wasting time on a build.
    pause
    exit /b 1
)
echo.
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
echo Registering FMOD package...
autobuild installables edit fmodstudio platform=windows64 hash=0c2ebac91d44914517137a5d1dda64e9 url=file:///C:/FS_Build/3p-fmodstudio/fmodstudio-2.03.13-windows64-262401839.tar.bz2
echo.
echo Configuring...
autobuild configure -A 64 -c ReleaseFS_open -- --chan Tinkerstorm --fmodstudio --avx2 --lto --package -DLL_TESTS:BOOL=FALSE
if errorlevel 1 (
    echo ERROR: Configure failed - aborting before the build step.
    pause
    exit /b 1
)
echo.
echo Building - this will take a while. Log is being written to build_log.txt
autobuild build -A 64 -c ReleaseFS_open --no-configure > build_log.txt 2>&1
echo.
echo ============================
echo   BUILD FINISHED - CHECKING RESULT
echo ============================
findstr /I "error" build_log.txt > nul
if errorlevel 1 (
    echo No errors found in build_log.txt.
) else (
    echo Errors were found - check build_log.txt for details:
    findstr /I "error" build_log.txt
)
echo.
if exist "build-vc170-64\newview\Release\firestorm-bin.exe" (
    echo firestorm-bin.exe exists - build succeeded.
) else (
    echo firestorm-bin.exe NOT found - build likely failed.
)
echo.
pause