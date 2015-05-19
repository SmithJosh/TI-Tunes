@echo off
echo ==== Now assembling %~nx1 for the TI-83/84 Plus ====
..\Compiler\spasm -I ..\Compiler .\%~nx1 .\%~n1.8xk
if errorlevel 1 goto ERRORS
echo Compilation Successful!
goto DONE
:ERRORS
echo Compiled With Errors!!!
:DONE
echo ==== Done ====
pause