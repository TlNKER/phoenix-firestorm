@echo off
setlocal
set "PS=powershell -NoProfile -Command"

set "BUILD=C:\FS_Build\phoenix-firestorm\build-vc170-64\newview\Release"
set "INSTALLER=C:\FS_Build\Tinkerstorm installer"
set "DEST=C:\Tinkerstorm-test"

echo.
%PS% "Write-Host ([char]0xf07b) ' UPDATE TINKERSTORM (test install)' -ForegroundColor Cyan"
echo.
echo Confirm that a working compile is done. This installation nukes:
echo   %DEST%
echo and rebuilds it from scratch.
echo.
pause
echo.

:: Bail out early if the build doesn't actually exist, so we don't wipe
:: the test folder and then leave it empty.
if not exist "%BUILD%\FirestormOS-Tinkerstorm.exe" (
    %PS% "Write-Host ([char]0xf071) ' ERROR: Build exe not found at:' -ForegroundColor Red"
    echo   %BUILD%\FirestormOS-Tinkerstorm.exe
    echo Aborting - nothing was deleted.
    pause
    exit /b 1
)

%PS% "Write-Host ([char]0xf07b) ' Removing old test install...'"
rmdir /s /q "%DEST%" 2>nul
mkdir "%DEST%"

echo.
%PS% "Write-Host ([char]0xf07b) ' Copying build output...'"
robocopy "%BUILD%" "%DEST%" /E /XF firestorm-bin.pdb firestorm-bin.map firestorm-bin.exe firestorm_setup_tmp.nsi copy_touched.bat licenses.txt /XD skins /NFL /NDL /NJH /NJS
if %errorlevel% GEQ 8 (
    %PS% "Write-Host ([char]0xf071) ' ERROR: robocopy failed copying main build. Aborting.' -ForegroundColor Red"
    pause
    exit /b 1
)

%PS% "Write-Host ([char]0xf07b) ' Copying default skin...'"
robocopy "%BUILD%\skins\default" "%DEST%\skins\default" /E /NFL /NDL /NJH /NJS
if %errorlevel% GEQ 8 (
    %PS% "Write-Host ([char]0xf071) ' ERROR: robocopy failed copying default skin. Aborting.' -ForegroundColor Red"
    pause
    exit /b 1
)

%PS% "Write-Host ([char]0xf07b) ' Copying tinkered skin...'"
robocopy "%INSTALLER%\tinkered" "%DEST%\skins\tinkered" /E /NFL /NDL /NJH /NJS
if %errorlevel% GEQ 8 (
    %PS% "Write-Host ([char]0xf071) ' ERROR: robocopy failed copying tinkered skin. Aborting.' -ForegroundColor Red"
    pause
    exit /b 1
)

echo.
%PS% "Write-Host ([char]0xf07b) ' Applying Tinkerstorm overlay files...'"

:: -- skins\default\textures\legacy --
copy /y "%INSTALLER%\active_voice_tab.tga" "%DEST%\skins\default\textures\legacy\" >nul

:: -- skins\default\textures\windows --
copy /y "%INSTALLER%\login_fs_logo.png" "%DEST%\skins\default\textures\windows\" >nul

:: -- skins\default\textures\icons --
copy /y "%INSTALLER%\profile_badge_team.png" "%DEST%\skins\default\textures\icons\" >nul
copy /y "%INSTALLER%\fstrusted.png" "%DEST%\skins\default\textures\icons\" >nul

:: -- skins\default\xui\en --
copy /y "%INSTALLER%\panel_chiclet_bar.xml" "%DEST%\skins\default\xui\en\" >nul
copy /y "%INSTALLER%\panel_fs_nui_login.xml" "%DEST%\skins\default\xui\en\" >nul
copy /y "%INSTALLER%\panel_status_bar.xml" "%DEST%\skins\default\xui\en\" >nul
copy /y "%INSTALLER%\menu_viewer.xml" "%DEST%\skins\default\xui\en\" >nul
copy /y "%INSTALLER%\floater_profile.xml" "%DEST%\skins\default\xui\en\" >nul
copy /y "%INSTALLER%\floater_fs_im_session.xml" "%DEST%\skins\default\xui\en\" >nul
copy /y "%INSTALLER%\floater_fs_nearby_chat.xml" "%DEST%\skins\default\xui\en\" >nul

:: -- skins\tinkered\xui\en --
copy /y "%INSTALLER%\panel_main_inventory.xml" "%DEST%\skins\tinkered\xui\en\" >nul

:: -- skins root --
copy /y "%INSTALLER%\skins.xml" "%DEST%\skins\" >nul

:: -- app_settings --
copy /y "%INSTALLER%\cloud.xml" "%DEST%\app_settings\" >nul
copy /y "%INSTALLER%\grids.xml" "%DEST%\app_settings\" >nul

echo.
%PS% "Write-Host ([char]0xf07b) ' Trimming unneeded files (English-only, WebRTC voice, no CEF non-English locales)...'"

:: -- CEF locale .pak files: keep only en-US --
for %%F in ("%DEST%\llplugin\locales\*.pak") do (
    if /i not "%%~nxF"=="en-US.pak" del /q "%%F"
)

:: -- Non-English XUI/HTML folders --
for %%D in ("%DEST%\skins\default\html\*") do (
    if /i not "%%~nxD"=="en-us" rmdir /s /q "%%D" 2>nul
)
for %%D in ("%DEST%\skins\default\xui\*") do (
    if /i not "%%~nxD"=="en" rmdir /s /q "%%D" 2>nul
)
for %%D in ("%DEST%\skins\tinkered\xui\*") do (
    if /i not "%%~nxD"=="en" rmdir /s /q "%%D" 2>nul
)

:: -- Vivox (unused - WebRTC voice only) --
del /q "%DEST%\SLVoice.exe" 2>nul
del /q "%DEST%\VivoxAUP.txt" 2>nul
del /q "%DEST%\vivoxsdk_x64.dll" 2>nul
del /q "%DEST%\ortp_x64.dll" 2>nul
del /q "%DEST%\skins\default\textures\3p_icons\vivox_logo.png" 2>nul

:: -- Fonts: keep only the DejaVu pack plus the files every pack needs
:: regardless of font choice (Inter default fallback, emoji, script editor
:: fonts). Everything else here belongs to other selectable font packs
:: (Droid, Liberation, Mobi, Noto, Roboto, Ubuntu, Celestia, Inter Tight,
:: DejaVu All Caps, the dyslexia pack) or is a license/readme file that's
:: never read at runtime.
for %%F in ("%DEST%\fonts\*") do (
    set "KEEPFONT="
    for %%K in (
        DejaVuSans.ttf
        DejaVuSans-Bold.ttf
        DejaVuSans-Oblique.ttf
        DejaVuSans-BoldOblique.ttf
        DejaVuSansMono.ttf
        TwemojiSVG.ttf
        NotoEmoji-Regular.ttf
        SourceCodePro-Regular.ttf
        ocra.ttf
        CascadiaCode-Light.ttf
        InterVariableFont.ttf
        InterItalicVariableFont.ttf
        fonts.xml
        fonts_deja_vu.xml
    ) do (
        if /i "%%~nxF"=="%%K" set "KEEPFONT=1"
    )
    if not defined KEEPFONT del /q "%%F" 2>nul
)

echo.

:: Sanity check that the exe actually made it across.
if not exist "%DEST%\FirestormOS-Tinkerstorm.exe" (
    %PS% "Write-Host ([char]0xf071) ' WARNING: FirestormOS-Tinkerstorm.exe was not found in %DEST%' -ForegroundColor Red"
    echo Something went wrong - check the robocopy output above.
    pause
    exit /b 1
)

%PS% "Write-Host ([char]0xf00c) ' Tinkerstorm test install is up to date.' -ForegroundColor Green"
echo.
set /p choice="Do you want to run it now? (y/n) "
if /i "%choice%"=="y" start "" "%DEST%\FirestormOS-Tinkerstorm.exe"

endlocal
