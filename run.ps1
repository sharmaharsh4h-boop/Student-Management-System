# Student Management System - PowerShell Runner
Write-Host "========================================================" -ForegroundColor Cyan
Write-Host "      STUDENT MANAGEMENT SYSTEM (LOCALHOST WEB APP)" -ForegroundColor Cyan
Write-Host "========================================================" -ForegroundColor Cyan
Write-Host ""

$env:PATH = "C:\tools\mingw64\bin;" + $env:PATH

if (!(Test-Path "database")) {
    New-Item -ItemType Directory -Path "database" | Out-Null
}

if (!(Test-Path "backend\sqlite3.o")) {
    Write-Host "[1/3] Compiling SQLite3 amalgamation - please wait..." -ForegroundColor Yellow
    gcc -O2 -c backend\sqlite3.c -o backend\sqlite3.o
}

if (!(Test-Path "server.exe")) {
    Write-Host "[2/3] Compiling C++ HTTP Backend server..." -ForegroundColor Yellow
    g++ -std=c++17 -static -Ibackend backend\main.cpp backend\database.cpp backend\sqlite3.o -lws2_32 -o server.exe
}

Write-Host "[SUCCESS] Server is ready!" -ForegroundColor Green
Write-Host ""
Write-Host "[3/3] Starting Student Management System on http://localhost:8080 ..." -ForegroundColor Cyan
Write-Host ""
Write-Host "========================================================" -ForegroundColor Green
Write-Host "  Open in your browser:  http://localhost:8080" -ForegroundColor Yellow
Write-Host "  Database file:         database\students.db" -ForegroundColor White
Write-Host "  Press Ctrl + C in this terminal window to stop server." -ForegroundColor White
Write-Host "========================================================" -ForegroundColor Green
Write-Host ""

Start-Process "http://localhost:8080"
.\server.exe
