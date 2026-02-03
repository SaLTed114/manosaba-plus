#!/usr/bin/env pwsh
# Clear log files from manosaba-plus project
# Usage: .\Scripts\clear_logs.ps1 (run from project root)

# Set UTF-8 encoding for console output
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8
$OutputEncoding = [System.Text.Encoding]::UTF8

$projectRoot = Split-Path $PSScriptRoot -Parent
$logsPath = Join-Path $projectRoot "Logs"

Write-Host "Clearing logs..." -ForegroundColor Cyan

if (Test-Path $logsPath) {
    Remove-Item $logsPath -Recurse -Force
    Write-Host "[OK] Logs directory removed" -ForegroundColor Green
} else {
    Write-Host "[INFO] Logs directory not found, skipping removal" -ForegroundColor Yellow
}

New-Item $logsPath -ItemType Directory -Force | Out-Null
Write-Host "[OK] Fresh Logs directory created at: $logsPath" -ForegroundColor Green
Write-Host ""
Write-Host "Done!" -ForegroundColor Green
