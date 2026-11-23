@echo off
rem The add-on's unit tests and repository check with the Expanded's own Python
rem (its .setup\venv has numpy), whatever python is on PATH.
setlocal
set "ROOT=%~dp0..\..\.."
set "PY=%ROOT%\.setup\venv\Scripts\python.exe"
if not exist "%PY%" (
    echo TMST tests: run Setup Tekken 3 first ^(no .setup\venv^).
    exit /b 1
)
cd /d "%~dp0.."
"%PY%" -m unittest discover -s tests %*
if errorlevel 1 exit /b %errorlevel%
"%PY%" tools\check_repo.py
exit /b %errorlevel%
