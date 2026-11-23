@echo off
rem Tekken 3 Expanded for Android with the TMST add-on, on Windows: the
rem Expanded's APK build (Build Android APK) with the add-on built in
rem (setup\tmst_android.py). Run it after the Expanded's setup and Setup TMST.
rem Double-click it, or with options:
rem   --no-music  without the Jukebox's Tekken 1, 2, TTT and Tekken 5 DR music (about 800 MB)
rem   --install   then install it on the phone connected by USB (adb)
rem   --without   the APK without the add-on
setlocal
cd /d "%~dp0"
for %%i in ("%~dp0..\..") do set "ROOT=%%~fi"
if not exist "%ROOT%\game.toml" (
    echo This folder is not inside Tekken 3 Expanded.
    echo Put it exactly at ^<Tekken 3 Expanded^>\addons\tmst and run this again.
    pause & exit /b 1
)
if not exist "%ROOT%\.setup\venv\Scripts\python.exe" (
    echo Tekken 3 Expanded has not been set up yet.
    echo Run "Setup Tekken 3.cmd" ^(or "Build Android APK.cmd"^) in %ROOT% first, then this again.
    pause & exit /b 1
)
"%ROOT%\.setup\venv\Scripts\python.exe" setup\tmst_android.py %*
set status=%errorlevel%
pause
exit /b %status%
