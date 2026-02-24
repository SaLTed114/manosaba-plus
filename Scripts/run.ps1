# Scripts/run.ps1
# 启动脚本：清理旧日志并运行程序

param(
    [int]$KeepDays = 7,           # 保留最近几天的日志
    [int]$KeepCount = 10,         # 或者保留最近几个日志文件
    [switch]$NoCleanup            # 跳过清理步骤
)

$ErrorActionPreference = "Stop"

# 设置控制台编码为 UTF-8
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8
$OutputEncoding = [System.Text.Encoding]::UTF8

# 切换到项目根目录
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$projectRoot = Split-Path -Parent $scriptDir
Set-Location $projectRoot

Write-Host "=== Manosaba+ 启动脚本 ===" -ForegroundColor Cyan
Write-Host ""

# 清理旧日志
if (-not $NoCleanup) {
    Write-Host "[1/2] 清理旧日志..." -ForegroundColor Yellow
    
    $logsDir = Join-Path $projectRoot "Logs"
    
    # 提前处理特殊情况
    if (-not (Test-Path $logsDir)) {
        Write-Host "  日志目录不存在，跳过清理" -ForegroundColor Gray
    }
    else {
        # 获取所有日志文件
        $logFiles = Get-ChildItem -Path $logsDir -Filter "*.log" -File | Sort-Object LastWriteTime -Descending
        
        if ($logFiles.Count -eq 0) {
            Write-Host "  日志目录为空" -ForegroundColor Gray
        }
        else {
            Write-Host "  找到 $($logFiles.Count) 个日志文件" -ForegroundColor Gray
            
            # 按日期清理
            $cutoffDate = (Get-Date).AddDays(-$KeepDays)
            $filesToDelete = $logFiles | Where-Object { $_.LastWriteTime -lt $cutoffDate }
            
            # 按数量清理（如果文件数超过限制）
            if ($logFiles.Count -gt $KeepCount) {
                $filesToDeleteByCount = $logFiles | Select-Object -Skip $KeepCount
                $filesToDelete = @($filesToDelete) + @($filesToDeleteByCount) | Sort-Object -Unique -Property FullName
            }
            
            if ($filesToDelete.Count -gt 0) {
                Write-Host "  删除 $($filesToDelete.Count) 个旧日志文件:" -ForegroundColor Gray
                foreach ($file in $filesToDelete) {
                    Write-Host "    - $($file.Name)" -ForegroundColor DarkGray
                    Remove-Item $file.FullName -Force
                }
            }
            else {
                Write-Host "  没有需要清理的旧日志" -ForegroundColor Green
            }
        }
    }
    
    Write-Host ""
}

# 启动程序
Write-Host "[2/2] 启动程序..." -ForegroundColor Yellow

$exePath = Join-Path $projectRoot "Build\Bin\Debug\manosaba_plus.exe"

if (-not (Test-Path $exePath)) {
    Write-Host "错误: 找不到可执行文件" -ForegroundColor Red
    Write-Host "路径: $exePath" -ForegroundColor Red
    Write-Host ""
    Write-Host "请先编译项目！" -ForegroundColor Yellow
    exit 1
}

Write-Host "  执行: $exePath" -ForegroundColor Gray
Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# 启动程序并等待退出
$process = Start-Process -FilePath $exePath -PassThru -Wait
$exitCode = $process.ExitCode

$exitColor = if ($exitCode -eq 0) { "Green" } else { "Red" }

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "程序已退出 (代码: $exitCode)" -ForegroundColor $exitColor

exit $exitCode