@echo off
title Student Management System - Web Server
color 0F
cls

echo ========================================================
echo       STUDENT MANAGEMENT SYSTEM (LOCALHOST WEB APP)
echo ========================================================
echo.

:: 1. Add MinGW to PATH if not already present
where g++ >nul 2>nul
if %errorlevel% neq 0 (
    if exist "C:\tools\mingw64\bin\g++.exe" (
        echo [INFO] Detected MinGW at C:\tools\mingw64\bin. Adding to session PATH...
        set "PATH=C:\tools\mingw64\bin;%PATH%"
    ) else (
        echo [ERROR] 'g++' compiler not found!
        echo Please ensure MinGW-w64 is installed at C:\tools\mingw64 or in PATH.
        echo.
        pause
        exit /b 1
    )
)

:: 2. Create database directory if missing
if not exist "database" mkdir database

:: 3. Check and compile sqlite3.o if not present
if not exist "backend\sqlite3.o" (
    echo [1/3] Compiling SQLite3 amalgamation (this takes ~10 seconds on first run)...
    gcc -O2 -c backend\sqlite3.c -o backend\sqlite3.o
    if %errorlevel% neq 0 (
        echo [ERROR] Failed to compile sqlite3.c
        pause
        exit /b 1
    )
)

:: 4. Compile server.exe if not present
if not exist "server.exe" (
    echo [2/3] Compiling C++ HTTP Backend server...
    g++ -std=c++17 -Ibackend backend\main.cpp backend\database.cpp backend\sqlite3.o -lws2_32 -o server.exe
    if %errorlevel% neq 0 (
        echo [ERROR] Compilation failed!
        pause
        exit /b 1
    )
    echo [SUCCESS] Server compiled successfully!
)

:: 5. Launch browser and start server
echo.
echo [3/3] Starting Student Management System on http://localhost:8080 ...
echo.
echo ========================================================
echo   Open in your browser:  http://localhost:8080
echo   Database file:         database\students.db
echo   Press Ctrl + C in this terminal window to stop server.
echo ========================================================
echo.

start "" "http://localhost:8080"
server.exe

echo.
echo Server stopped.
pause
