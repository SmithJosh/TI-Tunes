@echo off

::Convert with SOX
.\exe\sox\sox --guard .\song\%~nx1 -b 8 -e unsigned .\testsongs\%~n1temp.wav channels 1 rate 22050 dither -shibata
