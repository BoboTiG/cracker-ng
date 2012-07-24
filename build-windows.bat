@echo off
echo Cracker-ng, a multiple file password finder.

echo Building cpt module ...
@echo on
del bin\* /Q /F
g++ -march=native -fomit-frame-pointer -DUSE_STATS -s -lpthread -IC:\MinGW\include -LC:\MinGW\bin -O3 modules\cpt\cptcracker-ng.cpp -o bin\cptcracker-ng.exe

@echo off
echo.
echo.
echo Building zip module ...
@echo on
g++ -march=native -fomit-frame-pointer -DUSE_STATS -s -lpthread -IC:\MinGW\include -LC:\MinGW\bin -O2 -lboost_iostreams modules\zip\zipcracker-ng.cpp -o bin\zipcracker-ng.exe

@echo off
echo.
echo.
echo Done.
echo.

pause
