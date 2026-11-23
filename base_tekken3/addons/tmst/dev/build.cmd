@echo off
rem TEMPORARY developer build with the TMST add-on (dev\build.py --help).
setlocal
set "ROOT=%~dp0..\..\.."
set "PY=%ROOT%\.setup\venv\Scripts\python.exe"
if not exist "%PY%" (
    echo TMST build: run Setup Tekken 3 first ^(no .setup\venv^).
    exit /b 1
)
"%PY%" "%~dp0build.py" %*
exit /b %errorlevel%
