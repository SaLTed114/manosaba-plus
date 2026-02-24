# package_full.ps1 - 快捷方式
# 完整打包：先编译 Release 版本，然后打包（包含 VC++ Runtime DLL）

Write-Host "=== 完整打包流程 ===" -ForegroundColor Cyan
Write-Host ""

# 1. 编译 Release 版本
Write-Host "[1/2] 编译 Release 版本..." -ForegroundColor Yellow
cmake --build Build --config Release

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "编译失败！" -ForegroundColor Red
    exit $LASTEXITCODE
}

Write-Host ""

# 2. 打包（包含 VC++ Runtime DLL）
Write-Host "[2/2] 开始打包..." -ForegroundColor Yellow
& "$PSScriptRoot\Scripts\package.ps1" -IncludeVCRedist @args
