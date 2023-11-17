@echo off

REM Close the existing executable if it's running
taskkill /F /IM logica.exe 2>nul

REM Compile the C++ code
g++ -IC:\SFML-2.5.1\include logica.cpp -o logica -LC:\SFML-2.5.1\lib -lsfml-audio -lsfml-graphics -lsfml-window -lsfml-system

REM Check the compilation status
if %errorlevel% equ 0 (
    REM Open the new executable
    start logica.exe
) else (
    echo Compilation failed
)

