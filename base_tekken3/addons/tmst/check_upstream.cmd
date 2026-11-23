@echo off
rem Local compatibility check of the TMST add-on against Expanded releases, on
rem this machine (it needs the player's generated game, which CI cannot have):
rem the surfaces we rely on, then a build with the add-on for each ref.
rem   check_upstream.cmd              v1.4.1 and main
rem   check_upstream.cmd v1.5.0 main  the refs given
rem Results go to docs\COMPAT.md by hand. P2: no in-game smoke test yet.
setlocal
set "ROOT=%~dp0..\.."
set "PY=%ROOT%\.setup\venv\Scripts\python.exe"
set "REFS=%*"
if "%REFS%"=="" set "REFS=v1.4.1 main"
set FAILED=
"%PY%" "%~dp0tools\check_repo.py" || set FAILED=1
for %%R in (%REFS%) do (
    echo === %%R
    call "%~dp0dev\build.cmd" %%R || set FAILED=1
    "%PY%" "%~dp0tools\check_expanded.py" "%~dp0dev\build\%%R\src" || set FAILED=1
)
if defined FAILED (echo check_upstream: FAILED & exit /b 1)
echo check_upstream: OK
