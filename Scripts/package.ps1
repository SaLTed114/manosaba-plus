# Scripts/package.ps1
# 打包脚本：创建可分发的 Release 包

param(
    [switch]$Clean,                         # 清理所有旧的打包文件（包括历史 ZIP）
    [switch]$SkipBuild,                     # 跳过构建检查
    [switch]$IncludeVCRedist                # 打包 VC++ Runtime DLL（避免用户需要安装）
)

$ErrorActionPreference = "Stop"

# ===========================
# 初始化
# ===========================

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$projectRoot = Split-Path -Parent $scriptDir
Set-Location $projectRoot

Write-Host "=== まのさば＋ 打包脚本 ===" -ForegroundColor Cyan
Write-Host ""

# ===========================
# 检查 Release 构建
# ===========================

$releaseExe = "Build\Bin\Release\manosaba_plus.exe"

if (-not $SkipBuild) {
    Write-Host "[1/5] 检查 Release 构建..." -ForegroundColor Yellow
    
    if (-not (Test-Path $releaseExe)) {
        Write-Host ""
        Write-Host "错误: 找不到 Release 版本的可执行文件" -ForegroundColor Red
        Write-Host "路径: $releaseExe" -ForegroundColor Red
        Write-Host ""
        Write-Host "请先编译 Release 版本：" -ForegroundColor Yellow
        Write-Host "  1. 打开 Visual Studio" -ForegroundColor Gray
        Write-Host "  2. 将配置切换到 Release" -ForegroundColor Gray
        Write-Host "  3. 生成解决方案 (Ctrl+Shift+B)" -ForegroundColor Gray
        Write-Host ""
        Write-Host "或者使用命令行：" -ForegroundColor Yellow
        Write-Host "  cmake --build Build --config Release" -ForegroundColor Gray
        Write-Host ""
        Write-Host "如果想跳过此检查，请使用 -SkipBuild 参数" -ForegroundColor Gray
        exit 1
    }
    
    # 检查文件是否是 Release 版本（通过文件大小或时间戳）
    $exeInfo = Get-Item $releaseExe
    Write-Host "  找到可执行文件: $($exeInfo.Name) ($([math]::Round($exeInfo.Length / 1KB, 2)) KB)" -ForegroundColor Green
    Write-Host "  最后修改时间: $($exeInfo.LastWriteTime)" -ForegroundColor Gray
    Write-Host ""
}

# ===========================
# 清理旧文件
# ===========================

$mainDistDir = Join-Path $projectRoot "Dist"
$latestDir = Join-Path $mainDistDir "Latest"

if ($Clean) {
    Write-Host "[2/5] 清理所有打包文件..." -ForegroundColor Yellow
    if (Test-Path $mainDistDir) {
        Remove-Item $mainDistDir -Recurse -Force
        Write-Host "  已清理: Dist\" -ForegroundColor Green
    } else {
        Write-Host "  目录不存在，跳过清理" -ForegroundColor Gray
    }
    Write-Host ""
} else {
    Write-Host "[2/5] 清理最新版本..." -ForegroundColor Yellow
    if (Test-Path $latestDir) {
        Remove-Item $latestDir -Recurse -Force
        Write-Host "  已清理: Dist\Latest\" -ForegroundColor Green
    } else {
        Write-Host "  Latest 目录不存在，跳过清理" -ForegroundColor Gray
    }
    Write-Host ""
}

# ===========================
# 创建打包目录
# ===========================

Write-Host "[3/5] 创建打包目录..." -ForegroundColor Yellow

# 创建目录结构
New-Item -ItemType Directory -Path $mainDistDir -Force | Out-Null
New-Item -ItemType Directory -Path $latestDir -Force | Out-Null

$distRoot = $latestDir

Write-Host "  输出目录: Dist\Latest\" -ForegroundColor Green
Write-Host ""

# ===========================
# 复制文件
# ===========================

Write-Host "[4/5] 复制文件..." -ForegroundColor Yellow

# 源目录：Build/Bin/Release
$buildBinDir = "Build\Bin\Release"

# 复制整个 Release 目录内容，但排除不需要的文件
Write-Host "  从 $buildBinDir 复制文件..." -ForegroundColor Gray

$items = Get-ChildItem -Path $buildBinDir -ErrorAction SilentlyContinue

if ($null -eq $items) {
    Write-Host "    ⚠ 警告: $buildBinDir 目录为空或不存在" -ForegroundColor Yellow
} else {
    $copiedCount = 0
    
    foreach ($item in $items) {
        $shouldCopy = $true
        $itemName = $item.Name
        
        # 排除规则
        if ($itemName -like "*.pdb") {
            $shouldCopy = $false  # 排除调试符号
        } elseif ($itemName -eq "Logs") {
            $shouldCopy = $false  # 排除日志目录
        } elseif ($itemName -like "*Test.exe" -or $itemName -like "*Test.pdb") {
            $shouldCopy = $false  # 排除测试程序
        }
        
        if ($shouldCopy) {
            $destPath = Join-Path $distRoot $itemName
            
            if ($item.PSIsContainer) {
                # 复制目录
                Copy-Item -Path $item.FullName -Destination $destPath -Recurse -Force
                $fileCount = (Get-ChildItem -Path $destPath -File -Recurse).Count
                Write-Host "    ✓ $itemName/ ($fileCount 个文件)" -ForegroundColor Green
            } else {
                # 复制文件
                Copy-Item -Path $item.FullName -Destination $destPath -Force
                Write-Host "    ✓ $itemName" -ForegroundColor Green
            }
            
            $copiedCount++
        } else {
            Write-Host "    ⊗ 跳过: $itemName" -ForegroundColor DarkGray
        }
    }
    
    if ($copiedCount -eq 0) {
        Write-Host "    ⚠ 警告: 没有复制任何文件" -ForegroundColor Yellow
    }
}

# 复制系统验证脚本
Write-Host "  复制系统验证脚本..." -ForegroundColor Gray

# 复制 PowerShell 脚本
$verifyScriptSource = Join-Path $projectRoot "Scripts\verify_system.ps1"
$verifyScriptDest = Join-Path $distRoot "verify_system.ps1"

if (Test-Path $verifyScriptSource) {
    Copy-Item -Path $verifyScriptSource -Destination $verifyScriptDest -Force
    Write-Host "    ✓ verify_system.ps1" -ForegroundColor Green
} else {
    Write-Host "    ⚠ 警告: verify_system.ps1 未找到" -ForegroundColor Yellow
}

# 复制 BAT 包装脚本（用户可以双击运行）
$verifyBatSource = Join-Path $projectRoot "Scripts\verify_system.bat"
$verifyBatDest = Join-Path $distRoot "verify_system.bat"

if (Test-Path $verifyBatSource) {
    Copy-Item -Path $verifyBatSource -Destination $verifyBatDest -Force
    Write-Host "    ✓ verify_system.bat" -ForegroundColor Green
} else {
    Write-Host "    ⚠ 警告: verify_system.bat 未找到" -ForegroundColor Yellow
}

Write-Host ""

# ===========================
# 复制 VC++ Runtime DLL（可选）
# ===========================

if ($IncludeVCRedist) {
    Write-Host "[附加] 复制 VC++ Runtime DLL..." -ForegroundColor Yellow
    
    # 查找必需的 DLL
    $requiredDlls = @(
        "vcruntime140.dll",
        "vcruntime140_1.dll",
        "msvcp140.dll"
    )
    
    $searchPaths = @(
        "$env:SystemRoot\System32"
    )
    
    $copiedDllCount = 0
    $missingDlls = @()
    
    foreach ($dll in $requiredDlls) {
        $found = $false
        
        foreach ($searchPath in $searchPaths) {
            $dllPath = Join-Path $searchPath $dll
            
            if (Test-Path $dllPath) {
                $destPath = Join-Path $distRoot $dll
                Copy-Item -Path $dllPath -Destination $destPath -Force
                
                $fileSize = [math]::Round((Get-Item $destPath).Length / 1KB, 2)
                Write-Host "  ✓ $dll ($fileSize KB)" -ForegroundColor Green
                
                $copiedDllCount++
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
                $missingDlls += $dll
            }
        }
    }
    
    if ($missingDlls.Count -gt 0 -and $missingDlls -notcontains "vcruntime140_1.dll") {
        Write-Host ""
        Write-Host "  ⚠ 警告: 缺少关键 DLL，程序可能无法在目标机器上运行" -ForegroundColor Yellow
        Write-Host "  解决方案: 要求用户安装 VC++ Redistributable" -ForegroundColor Gray
    } elseif ($copiedDllCount -gt 0) {
        Write-Host ""
        Write-Host "  已复制 $copiedDllCount 个 DLL 文件，用户无需安装 VC++ Runtime" -ForegroundColor Green
    }
    
    Write-Host ""
}

# ===========================
# 创建 README
# ===========================

# 根据是否包含 DLL 生成不同的说明
$vcRuntimeNote = if ($IncludeVCRedist) {
    "- VC++ Runtime DLL 已包含（无需额外安装）"
} else {
    "- Visual C++ Redistributable 2015-2022 (通常系统已安装)"
}

$troubleshootVCRuntime = if ($IncludeVCRedist) {
    @"
1. **DirectX 错误**:
   - 确认显卡支持 DirectX 11
   - 更新显卡驱动程序
"@
} else {
    @"
1. **缺少 DLL 文件**: 
   - 安装 Visual C++ Redistributable: https://aka.ms/vs/17/release/vc_redist.x64.exe

2. **DirectX 错误**:
   - 确认显卡支持 DirectX 11
   - 更新显卡驱动程序
"@
}

$readmeContent = @"
# まのさば＋

## 系统要求

- Windows 10 或更高版本
- 支持 DirectX 11 的显卡
$vcRuntimeNote

## 运行前检查（推荐）

双击 ``verify_system.bat`` 文件即可

如需运行 .ps1 文件，右键选择“使用 PowerShell 运行”或使用命令：
``````powershell
powershell -ExecutionPolicy Bypass -File .\verify_system.ps1
``````

## 运行方法

1. 确保你的显卡支持 DirectX 11（可运行 verify_system.bat 检查）
2. 双击运行 ``manosaba_plus.exe``
3. 如果遇到问题，请查看下方的故障排除

## 目录结构

``````
manosaba_plus/
├── manosaba_plus.exe         # 主程序
├── verify_system.ps1         # 系统验证脚本
├── verify_system.bat         # 系统验证脚本
├── Assets/                   # 游戏资源
│   ├── Image/                # 图片资源
│   ├── Mesh/                 # 3D 模型
│   ├── Story/                # 剧情数据
│   └── Font/                 # 字体（可选）
└── Shaders/                  # 着色器（编译后的 .cso 文件）
``````

## 故障排除

### 程序无法启动

$troubleshootVCRuntime

3. **文件权限错误**:
   - 右键 manosaba_plus.exe → 属性 → 取消勾选"只读"

### 如何验证 DirectX 支持

运行验证脚本（如果提供）：
``````powershell
.\verify.ps1
``````

或手动检查：
1. Win+R 运行 ``dxdiag``
2. 查看"显示"选项卡
3. 确认 DirectX 版本 ≥ 11

## 技术信息

- 渲染: DirectX 11
- 构建日期: $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")
- 版本: Release

## 项目信息

GitHub: https://github.com/SaLTed114/manosaba-plus
许可证: 请查看仓库中的 LICENSE 文件
"@

Set-Content -Path (Join-Path $distRoot "README.txt") -Value $readmeContent -Encoding UTF8
Write-Host "[5/5] 创建说明文件..." -ForegroundColor Yellow
Write-Host "  ✓ README.txt" -ForegroundColor Green
Write-Host ""

# ===========================
# 创建 ZIP 压缩包
# ===========================

Write-Host "[6/6] 创建 ZIP 压缩包..." -ForegroundColor Yellow

$timestamp = Get-Date -Format 'yyyyMMdd'
$zipName = "まのさば＋_$timestamp.zip"
$zipPath = Join-Path $mainDistDir $zipName

Compress-Archive -Path $distRoot\* -DestinationPath $zipPath -Force

$zipSize = [math]::Round((Get-Item $zipPath).Length / 1MB, 2)
Write-Host "  ✓ $zipName ($zipSize MB)" -ForegroundColor Green
Write-Host ""

# ===========================
# 完成
# ===========================

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "打包完成！" -ForegroundColor Green
Write-Host ""
Write-Host "输出结构:" -ForegroundColor White
Write-Host "  Dist/" -ForegroundColor Gray
Write-Host "  ├── Latest/              ← 最新版本（未压缩，用于测试）" -ForegroundColor Gray
Write-Host "  └── $zipName  ← 带时间戳的版本（用于分发）" -ForegroundColor Gray
Write-Host ""
Write-Host "下一步操作：" -ForegroundColor Yellow
Write-Host "  1. 在本地测试: cd Dist\Latest && .\manosaba_plus.exe" -ForegroundColor Gray
Write-Host "  2. 运行验证: .\Scripts\verify.ps1 -PackageDir Dist\Latest" -ForegroundColor Gray
Write-Host "  3. 分发 ZIP: 发送 Dist\$zipName 给用户" -ForegroundColor Gray
Write-Host ""

if (-not $IncludeVCRedist) {
    Write-Host "提示: 使用 -IncludeVCRedist 参数可打包 VC++ Runtime DLL（用户无需安装）" -ForegroundColor DarkGray
    Write-Host ""
}
