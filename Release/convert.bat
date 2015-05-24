@echo off

::Convert with to single channel .wav with SOX
.\exe\sox\sox -D .\song\%~nx1 .\exe\%~n1temp.wav channels 1
if errorlevel 1 goto End

::Resample and lower sample size with ssrc
cd .\exe
ssrc --rate 22050 --twopass --dither 0 --bits 8 %~n1temp.wav %~n1out.wav
if errorlevel 1 goto End

::Clip header of wav file
trimheader %~n1out.wav %~n1.bin

::Join with player code
copy PlayWave.bin /b + %~n1.bin /b %~n1.out

::Generate Intel Hex file
To8xk.exe %~n1.out %~n1

::Sign App
rabbitsign -g %~n1.hex

::Move App to 8xk folder
move %~n1.8xk ..\8xk

::Clean temporary files
del %~n1temp.wav
::del %~n1out.wav
del %~n1.bin
del %~n1.out
del %~n1.hex

cd..
:End