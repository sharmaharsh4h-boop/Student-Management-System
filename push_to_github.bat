@echo off
title Push Student Management System to GitHub
color 0F
cls

echo ========================================================
echo        PUSH STUDENT MANAGEMENT SYSTEM TO GITHUB
echo ========================================================
echo.
echo Target Repository: https://github.com/sharmaharsh4h-boop/sms
echo.

set "PATH=C:\tools\git\cmd;%PATH%"

cd /d C:\SMS

echo [1/3] Checking Git Status...
git status -s

echo.
echo [2/3] Staging and Committing any pending changes...
git add .
git commit -m "Update Student Management System" 2>nul

echo.
echo [3/3] Pushing to GitHub (main branch)...
echo.
echo Note: If prompted, sign in via your browser or paste your GitHub Personal Access Token.
echo.
git push -u origin main

if %errorlevel% equ 0 (
    echo.
    echo ========================================================
    echo [SUCCESS] Project uploaded successfully to:
    echo           https://github.com/sharmaharsh4h-boop/sms
    echo ========================================================
) else (
    echo.
    echo [NOTICE] Push requires GitHub authentication.
    echo If browser sign-in did not pop up, you can push using a Personal Access Token:
    echo     git push https://<YOUR_GITHUB_TOKEN>@github.com/sharmaharsh4h-boop/sms.git main
)

echo.
pause
