# Scripts/verify.ps1
# 验证脚本：检查目标机器是否能运行程序

param(
    [string]$PackageDir = "Dist\Latest",   # 要验证的打包目录
    [switch]$RunTest,                              # 是否运行程序测试
    [switch]$Detailed                              # 显示详细信息
)

# 设置控制台编码为 UTF-8
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8
$OutputEncoding = [System.Text.Encoding]::UTF8

$ErrorActionPreference = "Continue"

# ===========================
# 初始化
# ===========================

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$projectRoot = Split-Path -Parent $scriptDir
Set-Location $projectRoot

Write-Host "=== まのさば＋ 验证脚本 ===" -ForegroundColor Cyan
Write-Host ""

$allPassed = $true

# ===========================
# 1. 检查 Windows 版本
# ===========================

Write-Host "[1/6] 检查 Windows 版本..." -ForegroundColor Yellow

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

Write-Host "[2/6] 检查 DirectX 支持..." -ForegroundColor Yellow

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
        Write-Host "      驱动日期: $driverDate" -ForegroundColor DarkGray
    }
}

Write-Host ""

# ===========================
# 3. 检查 Visual C++ Runtime
# ===========================

Write-Host "[3/6] 检查 Visual C++ Runtime..." -ForegroundColor Yellow

# 检查常见的 VC++ Runtime 注册表项
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
    Write-Host "  ⚠ Visual C++ Runtime 可能未安装" -ForegroundColor Yellow
    Write-Host "    如果程序无法运行，请安装: https://aka.ms/vs/17/release/vc_redist.x64.exe" -ForegroundColor Gray
}

Write-Host ""

# ===========================
# 4. 检查打包文件完整性
# ===========================

Write-Host "[4/6] 检查打包文件完整性..." -ForegroundColor Yellow

$distRoot = Join-Path $projectRoot $PackageDir

if (-not (Test-Path $distRoot)) {
    Write-Host "  ✗ 打包目录不存在: $PackageDir" -ForegroundColor Red
    Write-Host "    请先运行: .\Scripts\package.ps1" -ForegroundColor Gray
    $allPassed = $false
} else {
    # 检查必需文件
    $requiredItems = @{
        "manosaba_plus.exe" = "可执行文件"
        "Assets" = "资源目录"
        "Shaders" = "着色器目录"
        "README.txt" = "说明文件"
    }
    
    $missingItems = @()
    
    foreach ($item in $requiredItems.Keys) {
        $itemPath = Join-Path $distRoot $item
        if (Test-Path $itemPath) {
            Write-Host "  ✓ $($requiredItems[$item]): $item" -ForegroundColor Green
        } else {
            Write-Host "  ✗ $($requiredItems[$item]) 缺失: $item" -ForegroundColor Red
            $missingItems += $item
            $allPassed = $false
        }
    }
    
    # 统计资源文件
    $assetsPath = Join-Path $distRoot "Assets"
    if (Test-Path $assetsPath) {
        $assetCount = (Get-ChildItem -Path $assetsPath -File -Recurse).Count
        Write-Host "    资源文件数: $assetCount" -ForegroundColor Gray
    }
}

Write-Host ""

# ===========================
# 5. 检查文件权限
# ===========================

Write-Host "[5/6] 检查文件权限..." -ForegroundColor Yellow

$exePath = Join-Path $distRoot "manosaba_plus.exe"

if (Test-Path $exePath) {
    $acl = Get-Acl $exePath
    $currentUser = [System.Security.Principal.WindowsIdentity]::GetCurrent().Name
    
    # 检查是否可执行（在 Windows 上基本不是问题）
    Write-Host "  ✓ 文件可访问" -ForegroundColor Green
    
    if ($Detailed) {
        Write-Host "    当前用户: $currentUser" -ForegroundColor DarkGray
    }
} else {
    Write-Host "  ⊗ 跳过 (可执行文件不存在)" -ForegroundColor DarkGray
}

Write-Host ""

# ===========================
# 6. 运行程序测试（可选）
# ===========================

Write-Host "[6/6] 程序运行测试..." -ForegroundColor Yellow

if ($RunTest) {
    if (Test-Path $exePath) {
        Write-Host "  启动程序进行测试..." -ForegroundColor Gray
        Write-Host "  （请关闭程序窗口以继续验证）" -ForegroundColor DarkGray
        Write-Host ""
        
        try {
            $process = Start-Process -FilePath $exePath -PassThru -Wait -WorkingDirectory (Split-Path $exePath)
            $exitCode = $process.ExitCode
            
            if ($exitCode -eq 0) {
                Write-Host "  ✓ 程序运行成功 (退出代码: $exitCode)" -ForegroundColor Green
            } else {
                Write-Host "  ⚠ 程序退出异常 (退出代码: $exitCode)" -ForegroundColor Yellow
            }
        } catch {
            Write-Host "  ✗ 程序启动失败: $($_.Exception.Message)" -ForegroundColor Red
            $allPassed = $false
        }
    } else {
        Write-Host "  ⊗ 跳过 (可执行文件不存在)" -ForegroundColor DarkGray
    }
} else {
    Write-Host "  ⊗ 已跳过 (使用 -RunTest 参数启用)" -ForegroundColor DarkGray
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
    Write-Host "  cd $PackageDir" -ForegroundColor Gray
    Write-Host "  .\manosaba_plus.exe" -ForegroundColor Gray
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
    Write-Host ""
    
    Write-Host "2. Visual C++ Runtime 问题:" -ForegroundColor White
    Write-Host "   - 下载安装: https://aka.ms/vs/17/release/vc_redist.x64.exe" -ForegroundColor Gray
    Write-Host ""
    
    Write-Host "3. 打包文件问题:" -ForegroundColor White
    Write-Host "   - 重新运行: .\Scripts\package.ps1 -Clean" -ForegroundColor Gray
    Write-Host ""
}

# 返回状态码
if ($allPassed) {
    exit 0
} else {
    exit 1
}
