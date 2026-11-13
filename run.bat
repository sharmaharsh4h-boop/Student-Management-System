@echo off
title Student Management System - Web Server
color 0F
cls

echo ========================================================
echo       STUDENT MANAGEMENT SYSTEM (LOCALHOST WEB APP)
echo ========================================================
echo.

:: Add MinGW to PATH
set "PATH=C:\tools\mingw64\bin;%PATH%"

:: Ensure database folder exists
if not exist "database" mkdir database

:: Compile sqlite3.o if missing
if not exist "backend\sqlite3.o" (
    echo [1/3] Compiling SQLite3 amalgamation - please wait...
    gcc -O2 -c backend\sqlite3.c -o backend\sqlite3.o
)

:: Compile server.exe if missing
if not exist "server.exe" (
    echo [2/3] Compiling C++ HTTP Backend server...
    g++ -std=c++17 -static -Ibackend backend\main.cpp backend\database.cpp backend\sqlite3.o -lws2_32 -o server.exe
)

echo [SUCCESS] Server is ready!
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
