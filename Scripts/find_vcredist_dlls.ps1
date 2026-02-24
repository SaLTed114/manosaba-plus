# Scripts/find_vcredist_dlls.ps1
# 查找并列出程序需要的 VC++ Runtime DLL

param(
    [string]$ExePath = "Build\Bin\Release\manosaba_plus.exe"
)

# 设置控制台编码为 UTF-8
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8
$OutputEncoding = [System.Text.Encoding]::UTF8

$ErrorActionPreference = "Continue"

Write-Host "=== VC++ Runtime DLL 查找工具 ===" -ForegroundColor Cyan
Write-Host ""

# ===========================
# 查找 Visual Studio 安装
# ===========================

Write-Host "[1/3] 查找 Visual Studio 安装..." -ForegroundColor Yellow

$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"

if (Test-Path $vswhere) {
    $vsPath = & $vswhere -latest -property installationPath
    if ($vsPath) {
        Write-Host "  ✓ 找到 Visual Studio: $vsPath" -ForegroundColor Green
        
        # 查找 Redist 目录
        $redistDirs = Get-ChildItem -Path "$vsPath\VC\Redist\MSVC" -Directory -ErrorAction SilentlyContinue
        
        if ($redistDirs) {
            $latestRedist = $redistDirs | Sort-Object Name -Descending | Select-Object -First 1
            $redistPath = Join-Path $latestRedist.FullName "x64\Microsoft.VC143.CRT"
            
            if (Test-Path $redistPath) {
                Write-Host "  ✓ 找到 Redist 目录: $redistPath" -ForegroundColor Green
            } else {
                # 尝试其他可能的路径
                $redistPath = Join-Path $latestRedist.FullName "x64\Microsoft.VC142.CRT"
                if (Test-Path $redistPath) {
                    Write-Host "  ✓ 找到 Redist 目录: $redistPath" -ForegroundColor Green
                } else {
                    $redistPath = $null
                }
            }
        }
    }
} else {
    Write-Host "  ⚠ vswhere.exe 未找到" -ForegroundColor Yellow
}

Write-Host ""

# ===========================
# 查找必需的 DLL
# ===========================

Write-Host "[2/3] 查找必需的 VC++ Runtime DLL..." -ForegroundColor Yellow

$requiredDlls = @(
    "vcruntime140.dll",
    "vcruntime140_1.dll",
    "msvcp140.dll"
)

$foundDlls = @()
$searchPaths = @()

# 添加搜索路径
if ($redistPath) {
    $searchPaths += $redistPath
}
$searchPaths += "$env:SystemRoot\System32"
$searchPaths += "$env:SystemRoot\SysWOW64"

Write-Host "  搜索路径:" -ForegroundColor Gray
foreach ($path in $searchPaths) {
    Write-Host "    - $path" -ForegroundColor DarkGray
}
Write-Host ""

foreach ($dll in $requiredDlls) {
    $found = $false
    
    foreach ($searchPath in $searchPaths) {
        $dllPath = Join-Path $searchPath $dll
        
        if (Test-Path $dllPath) {
            $fileInfo = Get-Item $dllPath
            $version = $fileInfo.VersionInfo.ProductVersion
            
            $foundDlls += @{
                Name = $dll
                Path = $dllPath
                Version = $version
                Size = [math]::Round($fileInfo.Length / 1KB, 2)
            }
            
            Write-Host "  ✓ $dll" -ForegroundColor Green
            Write-Host "    路径: $dllPath" -ForegroundColor Gray
            Write-Host "    版本: $version" -ForegroundColor Gray
            Write-Host "    大小: $($foundDlls[-1].Size) KB" -ForegroundColor Gray
            
            $found = $true
            break
        }
    }
    
    if (-not $found) {
        # vcruntime140_1.dll 不是必需的（取决于编译器版本）
        if ($dll -eq "vcruntime140_1.dll") {
            Write-Host "  ⊗ $dll (可选，未找到)" -ForegroundColor DarkGray
        } else {
            Write-Host "  ✗ $dll (未找到)" -ForegroundColor Red
        }
    }
}

Write-Host ""

# ===========================
# 生成复制命令
# ===========================

Write-Host "[3/3] 生成打包建议..." -ForegroundColor Yellow
Write-Host ""

if ($foundDlls.Count -gt 0) {
    Write-Host "找到 $($foundDlls.Count) 个 DLL 文件" -ForegroundColor Green
    Write-Host ""
    
    Write-Host "使用以下命令打包（带 DLL）：" -ForegroundColor Yellow
    Write-Host "  .\Scripts\package.ps1 -IncludeVCRedist" -ForegroundColor Cyan
    Write-Host ""
    
    Write-Host "或者手动复制 DLL 到分发目录：" -ForegroundColor Yellow
    foreach ($dll in $foundDlls) {
        Write-Host "  Copy-Item `"$($dll.Path)`" -Destination `"Dist\`"" -ForegroundColor Gray
    }
    Write-Host ""
    
    # 保存 DLL 路径到临时文件供打包脚本使用
    $dllListFile = "Temp\vcredist_dlls.txt"
    New-Item -ItemType Directory -Path "Temp" -Force | Out-Null
    
    $foundDlls | ForEach-Object { $_.Path } | Set-Content -Path $dllListFile -Encoding UTF8
    Write-Host "DLL 路径已保存到: $dllListFile" -ForegroundColor DarkGray
    
} else {
    Write-Host "未找到任何 VC++ Runtime DLL" -ForegroundColor Red
    Write-Host ""
    Write-Host "可能的原因：" -ForegroundColor Yellow
    Write-Host "  1. Visual Studio 未正确安装" -ForegroundColor Gray
    Write-Host "  2. 程序使用静态链接（/MT）" -ForegroundColor Gray
    Write-Host "  3. DLL 在非标准位置" -ForegroundColor Gray
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan

# 返回找到的 DLL 数量
return $foundDlls
