@echo off
rem =================================================================================
rem FILE: runcode.bat
rem 
rem WHAT IS INSIDE:
rem - Commands to compile the code into an .exe program.
rem 
rem THE FLOW:
rem 1. It calls g++ (the compiler).
rem 2. It links all your .cpp files and the MySQL library.
rem 3. If successful, it launches the app immediately.
rem 4. If failed, it pauses so you can see the error.
rem =================================================================================

echo Building...
rem Compile all .cpp files. Output name: SouvenirSystem.exe. Link libmysql.
g++ *.cpp -o SouvenirSystem.exe -I include -L . -lmysql

rem Check error level. 0 means Success.
if %errorlevel% neq 0 (
    echo [ERROR] Compilation Failed!
    rem Pause so window doesn't close immediately.
    pause
) else (
    echo [SUCCESS] Launching System...
    rem Run the new program.
    SouvenirSystem.exe
)