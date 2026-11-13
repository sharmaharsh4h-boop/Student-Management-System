# Push to GitHub PowerShell Script
Write-Host "========================================================" -ForegroundColor Cyan
Write-Host "       PUSH STUDENT MANAGEMENT SYSTEM TO GITHUB" -ForegroundColor Cyan
Write-Host "========================================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Target: https://github.com/sharmaharsh4h-boop/Student-Management-System.git" -ForegroundColor Yellow
Write-Host ""

$env:PATH = "C:\tools\git\cmd;" + $env:PATH
Set-Location "C:\SMS"

# Configure remote
git remote set-url origin https://github.com/sharmaharsh4h-boop/Student-Management-System.git 2>$null
if ($LASTEXITCODE -ne 0) {
    git remote add origin https://github.com/sharmaharsh4h-boop/Student-Management-System.git
}

Write-Host "[1/2] Staging and Committing all files..." -ForegroundColor Cyan
git add .
git commit -m "Upload Student Management System" 2>$null

Write-Host ""
Write-Host "[2/2] Pushing to GitHub (main branch)..." -ForegroundColor Cyan
Write-Host "If a sign-in dialog or browser pops up, please sign in to authorize." -ForegroundColor Yellow
Write-Host ""

git push -u origin main

if ($LASTEXITCODE -eq 0) {
    Write-Host ""
    Write-Host "========================================================" -ForegroundColor Green
    Write-Host "[SUCCESS] Upload completed successfully!" -ForegroundColor Green
    Write-Host "Check your repository at:" -ForegroundColor White
    Write-Host "https://github.com/sharmaharsh4h-boop/Student-Management-System" -ForegroundColor Cyan
    Write-Host "========================================================" -ForegroundColor Green
} else {
    Write-Host ""
    Write-Host "[NOTICE] If push failed due to authentication, run:" -ForegroundColor Red
    Write-Host "git push https://<TOKEN>@github.com/sharmaharsh4h-boop/Student-Management-System.git main" -ForegroundColor Yellow
}
