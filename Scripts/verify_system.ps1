# verify_system.ps1
# 系统验证脚本：检查当前机器是否能运行 まのさば＋

param(
    [switch]$Detailed                       # 显示详细信息
)

# 设置控制台编码为 UTF-8
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8
$OutputEncoding = [System.Text.Encoding]::UTF8

$ErrorActionPreference = "Continue"

Write-Host "=== まのさば＋ 系统验证 ===" -ForegroundColor Cyan
Write-Host ""

$allPassed = $true

# ===========================
# 1. 检查 Windows 版本
# ===========================

Write-Host "[1/4] 检查 Windows 版本..." -ForegroundColor Yellow

$osInfo = Get-CimInstance Win32_OperatingSystem
$osVersion = [System.Environment]::OSVersion.Version

Write-Host "  系统: $($osInfo.Caption)" -ForegroundColor Gray
Write-Host "  版本: $($osVersion.Major).$($osVersion.Minor) (Build $($osInfo.BuildNumber))" -ForegroundColor Gray

if ($osVersion.Major -ge 10) {
    Write-Host "  ✓ Windows 版本满足要求 (≥ Windows 10)" -ForegroundColor Green
} elseif ($osVersion.Major -eq 6 -and $osVersion.Minor -ge 1) {
    Write-Host "  ⚠ Windows 7/8 可能支持，但建议使用 Windows 10+" -ForegroundColor Yellow
} else {
    Write-Host "  ✗ Windows 版本过低，需要 Windows 7 或更高版本" -ForegroundColor Red
    $allPassed = $false
}

Write-Host ""

# ===========================
# 2. 检查 DirectX 支持
# ===========================

Write-Host "[2/4] 检查 DirectX 支持..." -ForegroundColor Yellow

# 检查 D3D11 DLL
$d3d11Path = Join-Path $env:SystemRoot "System32\d3d11.dll"
$dxgiPath = Join-Path $env:SystemRoot "System32\dxgi.dll"

if ((Test-Path $d3d11Path) -and (Test-Path $dxgiPath)) {
    Write-Host "  ✓ d3d11.dll 已找到" -ForegroundColor Green
    Write-Host "  ✓ dxgi.dll 已找到" -ForegroundColor Green
    
    if ($Detailed) {
        $d3d11Version = (Get-Item $d3d11Path).VersionInfo.ProductVersion
        Write-Host "    版本: $d3d11Version" -ForegroundColor DarkGray
    }
} else {
    Write-Host "  ✗ DirectX 11 DLL 未找到" -ForegroundColor Red
    $allPassed = $false
}

# 检查显卡信息
$gpus = Get-CimInstance Win32_VideoController

Write-Host "  检测到的显卡:" -ForegroundColor Gray
foreach ($gpu in $gpus) {
    Write-Host "    - $($gpu.Name)" -ForegroundColor Gray
    if ($Detailed) {
        $driverVersion = $gpu.DriverVersion
        $driverDate = $gpu.DriverDate
        Write-Host "      驱动版本: $driverVersion" -ForegroundColor DarkGray
        if ($null -ne $driverDate) {
            Write-Host "      驱动日期: $driverDate" -ForegroundColor DarkGray
        }
    }
}

Write-Host ""

# ===========================
# 3. 检查 Visual C++ Runtime
# ===========================

Write-Host "[3/4] 检查 Visual C++ Runtime..." -ForegroundColor Yellow

# 检查本地目录是否有 DLL
$localDlls = @("vcruntime140.dll", "msvcp140.dll")
$localDllFound = $true

foreach ($dll in $localDlls) {
    if (-not (Test-Path $dll)) {
        $localDllFound = $false
        break
    }
}

if ($localDllFound) {
    Write-Host "  ✓ VC++ Runtime DLL 已包含在程序目录中" -ForegroundColor Green
} else {
    # 检查系统是否安装了 VC++ Runtime
    $vcRuntimeFound = $false
    $registryPaths = @(
        "HKLM:\SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\X64",
        "HKLM:\SOFTWARE\WOW6432Node\Microsoft\VisualStudio\14.0\VC\Runtimes\X64"
    )

    foreach ($regPath in $registryPaths) {
        if (Test-Path $regPath) {
            $vcRuntimeFound = $true
            if ($Detailed) {
                $version = (Get-ItemProperty -Path $regPath -Name "Version" -ErrorAction SilentlyContinue).Version
                Write-Host "  找到 VC++ Runtime: $version" -ForegroundColor DarkGray
            }
            break
        }
    }

    # 检查关键 DLL
    $vcDlls = @("vcruntime140.dll", "msvcp140.dll")
    $dllsFound = 0

    foreach ($dll in $vcDlls) {
        $dllPath = Join-Path $env:SystemRoot "System32\$dll"
        if (Test-Path $dllPath) {
            $dllsFound++
        }
    }

    if ($vcRuntimeFound -or $dllsFound -eq $vcDlls.Count) {
        Write-Host "  ✓ Visual C++ Runtime 已安装" -ForegroundColor Green
    } else {
        Write-Host "  ✗ Visual C++ Runtime 未安装" -ForegroundColor Red
        Write-Host "    请安装: https://aka.ms/vs/17/release/vc_redist.x64.exe" -ForegroundColor Yellow
        $allPassed = $false
    }
}

Write-Host ""

# ===========================
# 4. 检查程序文件
# ===========================

Write-Host "[4/4] 检查程序文件..." -ForegroundColor Yellow

$exePath = "manosaba_plus.exe"

if (Test-Path $exePath) {
    $exeInfo = Get-Item $exePath
    $exeSize = [math]::Round($exeInfo.Length / 1KB, 2)
    Write-Host "  ✓ 找到程序文件: $exePath ($exeSize KB)" -ForegroundColor Green
    
    if ($Detailed) {
        Write-Host "    最后修改: $($exeInfo.LastWriteTime)" -ForegroundColor DarkGray
    }
} else {
    Write-Host "  ✗ 找不到程序文件: $exePath" -ForegroundColor Red
    Write-Host "    请确保在程序目录下运行此脚本" -ForegroundColor Yellow
    $allPassed = $false
}

# 检查资源目录
$assetsPath = "Assets"
$shadersPath = "Shaders"

if (Test-Path $assetsPath) {
    $assetCount = (Get-ChildItem -Path $assetsPath -File -Recurse -ErrorAction SilentlyContinue).Count
    Write-Host "  ✓ 资源目录存在 ($assetCount 个文件)" -ForegroundColor Green
} else {
    Write-Host "  ⚠ 资源目录不存在，程序可能无法正常运行" -ForegroundColor Yellow
}

if (Test-Path $shadersPath) {
    $shaderCount = (Get-ChildItem -Path $shadersPath -File -ErrorAction SilentlyContinue).Count
    Write-Host "  ✓ 着色器目录存在 ($shaderCount 个文件)" -ForegroundColor Green
} else {
    Write-Host "  ⚠ 着色器目录不存在，程序可能无法正常运行" -ForegroundColor Yellow
}

Write-Host ""

# ===========================
# 生成报告
# ===========================

Write-Host "========================================" -ForegroundColor Cyan

if ($allPassed) {
    Write-Host "验证通过！✓" -ForegroundColor Green
    Write-Host ""
    Write-Host "你的系统可以运行 まのさば＋！" -ForegroundColor White
    Write-Host ""
    Write-Host "运行方法:" -ForegroundColor Yellow
    Write-Host "  双击 manosaba_plus.exe" -ForegroundColor Gray
    Write-Host "  或在命令行运行: .\manosaba_plus.exe" -ForegroundColor Gray
} else {
    Write-Host "验证未完全通过 ⚠" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "请检查上述错误项并修复后重试。" -ForegroundColor White
}

Write-Host ""

# ===========================
# 输出详细帮助（如果有问题）
# ===========================

if (-not $allPassed) {
    Write-Host "常见问题解决方案:" -ForegroundColor Yellow
    Write-Host ""
    
    Write-Host "1. DirectX 问题:" -ForegroundColor White
    Write-Host "   - 更新显卡驱动程序" -ForegroundColor Gray
    Write-Host "   - 运行 Windows Update" -ForegroundColor Gray
    Write-Host "   - 运行 dxdiag 检查 DirectX 版本" -ForegroundColor Gray
    Write-Host ""
    
    Write-Host "2. Visual C++ Runtime 问题:" -ForegroundColor White
    Write-Host "   - 下载安装: https://aka.ms/vs/17/release/vc_redist.x64.exe" -ForegroundColor Gray
    Write-Host ""
    
    Write-Host "3. 文件缺失问题:" -ForegroundColor White
    Write-Host "   - 确保解压了完整的程序文件" -ForegroundColor Gray
    Write-Host "   - 重新下载压缩包" -ForegroundColor Gray
    Write-Host ""
}

Write-Host "如需更多帮助，请查看 README.txt 文件。" -ForegroundColor DarkGray
Write-Host ""

# 返回状态码
if ($allPassed) {
    exit 0
} else {
    exit 1
}
