@echo off
setlocal

set "SEVENZIP=C:\Program Files\7-Zip\7z.exe"
set "SRC=C:\Tinkerstorm-test"
set "OUTDIR=C:\FS_Build"
set "ARCHIVE=%OUTDIR%\tinkerstorm-install.7z"
set "EXE=%OUTDIR%\tinkerstorm-install.exe"
set "ICON=C:\FS_Build\Tinkerstorm installer\firestorm_icon.ico"

echo.
echo ==================================
echo   BUILD TINKERSTORM INSTALLER EXE
echo ==================================
echo.
echo This packages:
echo   %SRC%
echo into:
echo   %EXE%
echo.
echo Close Tinkerstorm.exe in %SRC% first if it's running. 
:: added the %src% above. if something breaks now, thats it.
echo.
pause
echo.

if not exist "%SRC%" (
    echo ERROR: %SRC% not found - nothing to package. Aborting.
    pause
    exit /b 1
)

if not exist "%SEVENZIP%" (
    echo ERROR: 7z.exe not found at:
    echo   %SEVENZIP%
    echo Is 7-Zip installed? Aborting.
    pause
    exit /b 1
)

if not exist "%ICON%" (
    echo ERROR: Icon not found at:
    echo   %ICON%
    echo Aborting before wasting time on the archive.
    pause
    exit /b 1
)

:: Pick whichever GUI SFX module is actually present. 7z.sfx prompts the
:: person running the exe for a destination folder (like the GUI checkbox
:: does); 7zCon.sfx is a console-only fallback with no destination prompt.
set "SFXMODULE=%ProgramFiles%\7-Zip\7z.sfx"
if not exist "%SFXMODULE%" set "SFXMODULE=%ProgramFiles%\7-Zip\7zCon.sfx"
if not exist "%SFXMODULE%" (
    echo ERROR: No .sfx module found in "%ProgramFiles%\7-Zip\". Aborting.
    pause
    exit /b 1
)

set "SFXWORK=%OUTDIR%\_sfx_module_work.sfx"

echo Cleaning up any previous build...
del /q "%ARCHIVE%" 2>nul
del /q "%EXE%" 2>nul
del /q "%SFXWORK%" 2>nul

:: Apply the icon to a WORKING COPY of the bare SFX module first, before
:: it's glued to the archive. rcedit rewrites the PE resource section when
:: it saves, and that rewrite doesn't reliably preserve raw bytes appended
:: after the PE image - which is exactly how an SFX exe carries its archive
:: data. Editing the finished (module + archive) exe was truncating it back
:: down to just the stub, which is why it came out at ~347 KB and wouldn't
:: unpack. Icon-editing the small standalone module first avoids that
:: entirely, since copy /b afterwards just appends raw bytes and never
:: touches the module's structure again.
copy /y "%SFXMODULE%" "%SFXWORK%" >nul
echo Applying icon to SFX module...
rcedit "%SFXWORK%" --set-icon "%ICON%"
if errorlevel 1 (
    echo WARNING: rcedit failed to set the icon. Continuing with the
    echo default 7-Zip SFX icon instead.
    copy /y "%SFXMODULE%" "%SFXWORK%" >nul
)

echo.
echo Compressing %SRC% ...
"%SEVENZIP%" a -t7z -mx=9 "%ARCHIVE%" "%SRC%"
if errorlevel 1 (
    echo ERROR: 7z.exe failed to build the archive. Aborting.
    pause
    exit /b 1
)

:: Sanity check the archive isn't suspiciously small (e.g. if SRC somehow
:: pointed at a shortcut instead of the real folder). The real install is
:: comfortably in the hundreds of MB, so anything under ~50 MB is almost
:: certainly wrong.
for %%A in ("%ARCHIVE%") do set ARCHSIZE=%%~zA
set /a ARCHSIZE_MB=%ARCHSIZE% / 1048576
echo Archive size: %ARCHSIZE_MB% MB
if %ARCHSIZE% LSS 50000000 (
    echo.
    echo WARNING: That's much smaller than expected for a full Tinkerstorm
    echo install. Something may be wrong with the source folder.
    echo Check %ARCHIVE% before continuing.
    pause
)

echo.
echo Wrapping archive into self-extracting exe...
copy /b "%SFXWORK%" + "%ARCHIVE%" "%EXE%" >nul
if errorlevel 1 (
    echo ERROR: Failed to build the SFX exe. Aborting.
    pause
    exit /b 1
)

del /q "%ARCHIVE%" 2>nul
del /q "%SFXWORK%" 2>nul

echo.
if not exist "%EXE%" (
    echo ERROR: %EXE% was not created. Something went wrong.
    pause
    exit /b 1
)
for %%A in ("%EXE%") do set FINALSIZE=%%~zA
set /a FINALSIZE_MB=%FINALSIZE% / 1048576
if %FINALSIZE% LSS 50000000 (
    echo WARNING: %EXE% is only %FINALSIZE_MB% MB - that's too small to
    echo contain the real install. Something went wrong; do not distribute
    echo this file. Test-extract it before trusting it either way.
) else (
    echo Done. %EXE% is ready ^(%FINALSIZE_MB% MB^).
)
echo.
pause

endlocal