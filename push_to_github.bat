@echo off
title Push Student Management System to GitHub
color 0F
cls

echo ========================================================
echo        PUSH STUDENT MANAGEMENT SYSTEM TO GITHUB
echo ========================================================
echo.
echo Target: https://github.com/sharmaharsh4h-boop/Student-Management-System.git
echo.

set "PATH=C:\tools\git\cmd;%PATH%"
cd /d C:\SMS

git remote set-url origin https://github.com/sharmaharsh4h-boop/Student-Management-System.git 2>nul || git remote add origin https://github.com/sharmaharsh4h-boop/Student-Management-System.git

echo [1/2] Staging and Committing changes...
git add .
git commit -m "Upload Student Management System" 2>nul

echo.
echo [2/2] Pushing to GitHub (main branch)...
echo If a GitHub sign-in window appears, please authorize it.
echo.
git push -u origin main

if %errorlevel% equ 0 (
    echo.
    echo ========================================================
    echo [SUCCESS] Upload completed successfully!
    echo Check your repository at:
    echo https://github.com/sharmaharsh4h-boop/Student-Management-System
    echo ========================================================
) else (
    echo.
    echo [NOTICE] If authentication failed, use a Personal Access Token:
    echo git push https://<TOKEN>@github.com/sharmaharsh4h-boop/Student-Management-System.git main
)

echo.
pause
