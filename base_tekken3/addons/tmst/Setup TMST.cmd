@echo off
rem The TMST add-on for Tekken 3 Expanded: opens its setup window
rem (setup\tmst_gui.py), which runs the add-on's setup and builds the game again
rem with it. Run after the Expanded's own setup (Setup Tekken 3.cmd). Drop a
rem Tekken 2 disc (.cue, .bin or .chd) on this file to give it. It uses the
rem Expanded's .setup\venv and toolchain; none of the Expanded's own files change.
setlocal
cd /d "%~dp0"
for %%i in ("%~dp0..\..") do set "ROOT=%%~fi"
if not exist "%ROOT%\game.toml" (
    echo This folder is not inside Tekken 3 Expanded.
    echo Put it exactly at ^<Tekken 3 Expanded^>\addons\tmst and run this again.
    pause & exit /b 1
)
if not exist "%ROOT%\.setup\venv\Scripts\pythonw.exe" (
    echo Tekken 3 Expanded has not been set up yet.
    echo Run "Setup Tekken 3.cmd" in %ROOT% first and let it finish
    echo ^(the game starts^). Close the game, then run this again.
    pause & exit /b 1
)
start "" "%ROOT%\.setup\venv\Scripts\pythonw.exe" setup\tmst_gui.py %*
