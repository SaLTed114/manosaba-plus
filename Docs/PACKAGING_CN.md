# 打包与分发指南

本文档介绍如何打包 まのさば＋ 并分发给能支持 DX11 的电脑。

## 目录

- [快速开始](#快速开始)
- [详细步骤](#详细步骤)
  - [1. 构建 Release 版本](#1-构建-release-版本)
  - [2. 打包程序](#2-打包程序)
  - [3. 本地验证](#3-本地验证)
  - [4. 目标机器验证](#4-目标机器验证)
  - [5. 分发](#5-分发)
- [系统要求](#系统要求)
- [故障排除](#故障排除)
- [高级选项](#高级选项)

---

## 快速开始

```powershell
# 1. 构建 Release 版本
cmake --build Build --config Release

# 2. 打包程序（自动创建 ZIP）
.\Scripts\package.ps1

# 或打包包含 VC++ Runtime DLL 的版本
.\package_full.ps1

# 3. 验证（本地）
.\Scripts\verify.ps1

# 4. 将 Dist\Latest\ 用于本地测试
# 5. 将 Dist\まのさば＋_YYYYMMDD.zip 发送给用户
```

---

## 详细步骤

### 1. 构建 Release 版本

#### 方法 A: Visual Studio

1. 打开 `Build/manosaba_plus.sln`
2. 在工具栏中将配置从 `Debug` 切换到 `Release`
3. 按 `Ctrl+Shift+B` 生成解决方案
4. 等待编译完成

#### 方法 B: 命令行

```powershell
# 在项目根目录运行
cmake --build Build --config Release
```

#### 验证构建

确认文件存在：
```powershell
Test-Path Build\Bin\Release\manosaba_plus.exe
# 应返回 True
```

---

### 2. 打包程序

#### 基本打包

```powershell
.\Scripts\package.ps1
```

这将创建 `Dist/` 目录结构：
```
Dist/
├── Latest/                   # 最新版本（未压缩，用于测试）
│   ├── manosaba_plus.exe     # 主程序
│   ├── verify_system.bat     # 系统验证脚本
│   ├── verify_system.ps1     # 系统验证脚本
│   ├── Assets/               # 游戏资源
│   ├── Shaders/              # 编译后的着色器
│   └── README.txt            # 用户说明
└── まのさば＋_20260224.zip    # 带时间戳的压缩包（用于分发）
```

#### 打包选项

```powershell
# 清理所有历史文件并重新打包
.\Scripts\package.ps1 -Clean

# 打包包含 VC++ Runtime DLL 的版本（用户无需安装）
.\Scripts\package.ps1 -IncludeVCRedist

# 或使用快捷方式
.\package_full.ps1

# 跳过构建检查（如果确定已经构建）
.\Scripts\package.ps1 -SkipBuild

# 组合使用
.\Scripts\package.ps1 -Clean -IncludeVCRedist
```

---

### 3. 本地验证

在打包完成后，先在本地测试：

#### 完整验证

```powershell
# 基本检查（默认验证 Dist\Latest）
.\Scripts\verify.ps1

# 包含运行测试
.\Scripts\verify.ps1 -RunTest

# 详细信息
.\Scripts\verify.ps1 -Detailed
```

#### 手动测试

```powershell
cd Dist\Latest
.\manosaba_plus.exe
```

确认程序能够：
- ✓ 正常启动
- ✓ 显示窗口
- ✓ 加载资源
- ✓ 响应输入
- ✓ 正常退出

---

### 4. 目标机器验证

将 ZIP 文件发送给用户后，用户应该：

#### 步骤 A: 解压并运行验证

**方法 1（最简单）**：双击 `verify_system.bat` 文件

**方法 2**：在 PowerShell 中运行：
```powershell
.\verify_system.ps1
```

**注意**：不要双击 `.ps1` 文件（会打开记事本）。`.bat` 文件可以直接双击运行。

验证脚本会检查：
- [x] Windows 版本
- [x] DirectX 11 支持
- [x] Visual C++ Runtime（或本地 DLL）
- [x] 程序文件完整性

#### 步骤 B: 如果验证通过

直接运行程序：
```powershell
.\manosaba_plus.exe
```

或双击 `manosaba_plus.exe`

#### 步骤 C: 处理问题

如果验证失败，脚本会给出详细的解决方案。参考 [故障排除](#故障排除) 章节。

---

### 5. 分发

#### 分发 ZIP 压缩包

每次打包都会自动在 `Dist/` 目录下生成带时间戳的 ZIP 文件：

```
Dist/
├── Latest/                              # 用于本地测试
├── まのさば＋_20260223.zip              # 旧版本
└── まのさば＋_20260224.zip              # 最新版本（发送给用户）
```

发送最新的 ZIP 文件给用户，并附上：
- 系统要求说明
- 解压和运行说明
- 故障排除链接

#### 分发方式

- 通过网盘（推荐）
- 通过邮件（文件较小时）
- 通过 U 盘
- 通过网络共享

#### 用户说明模板

```
まのさば＋

【安装方法】
1. 解压所有文件到任意目录
2. 运行 verify_system.ps1 检查系统要求（推荐）
3. 双击 manosaba_plus.exe 运行

【系统要求】
- Windows 10 或更高版本
- 支持 DirectX 11 的显卡
- VC++ Runtime（已包含，无需安装）

【如果无法运行】
1. 运行 verify_system.ps1 检查系统
2. 更新显卡驱动
3. 确认 DirectX 11 支持（运行 dxdiag 检查）

【联系方式】
如有问题请联系...
```

---

## 系统要求

### 最低要求

| 项目 | 要求 |
|------|------|
| 操作系统 | Windows 7 SP1 或更高 |
| 显卡 | 支持 DirectX 11 |
| 内存 | 2 GB RAM |
| 存储空间 | 100 MB 可用空间 |
| 运行库 | Visual C++ Redistributable 2015-2022 |

### 推荐配置

| 项目 | 推荐 |
|------|------|
| 操作系统 | Windows 10/11 |
| 显卡 | DirectX 11.1+ |
| 内存 | 4 GB RAM |
| 存储空间 | 500 MB 可用空间 |

### DirectX 11 支持检查

大多数现代 Windows 系统都支持 DX11。支持的显卡包括：
- **NVIDIA**: GeForce 400 系列及更高 (2010+)
- **AMD**: Radeon HD 5000 系列及更高 (2009+)
- **Intel**: HD Graphics 2000/3000 及更高 (2011+)

---

## 故障排除

### 问题 1: 程序无法启动（缺少 DLL）

**错误信息**: "无法启动此程序，因为计算机中丢失 VCRUNTIME140.dll"

**解决方案**:
```powershell
# 下载并安装 Visual C++ Redistributable
Start-Process "https://aka.ms/vs/17/release/vc_redist.x64.exe"
```

或手动下载安装：
https://aka.ms/vs/17/release/vc_redist.x64.exe

---

### 问题 2: DirectX 初始化失败

**错误信息**: "Failed to create D3D11 device" 或类似的 DirectX 错误

**诊断**:
```powershell
# 检查 DirectX 版本
dxdiag
```

**解决方案**:
1. **更新显卡驱动**
   - NVIDIA: https://www.nvidia.com/Download/index.aspx
   - AMD: https://www.amd.com/support
   - Intel: https://www.intel.com/content/www/us/en/support/detect.html

2. **运行 Windows Update**
   ```powershell
   Start-Process ms-settings:windowsupdate
   ```

3. **检查显卡是否支持 DX11**
   - 过旧的显卡（2009 年以前）可能不支持

---

### 问题 3: 找不到资源文件

**错误信息**: 程序启动但无法加载图片/模型

**原因**: Assets 目录缺失或路径不正确

**解决方案**:
```powershell
# 让用户运行验证脚本
.\verify_system.ps1

# 开发者重新打包
.\Scripts\package.ps1 -Clean
```

确保目录结构：
```
程序目录/
├── manosaba_plus.exe
├── verify_system.ps1
├── Assets/          ← 必须存在
│   ├── Image/
│   ├── Mesh/
│   └── Story/
└── Shaders/         ← 必须存在
```

---

### 问题 4: 程序在目标机器上显示乱码

**原因**: 控制台编码问题（如果有控制台输出）

**解决方案**:
- 这不影响程序的正常运行
- 如果需要查看日志，使用文本编辑器打开 `Logs/` 目录下的日志文件
- 确保文本编辑器使用 UTF-8 编码

---

### 问题 5: 防病毒软件误报

**现象**: 防病毒软件阻止程序运行

**解决方案**:
1. 将程序添加到防病毒软件的白名单
2. 临时禁用防病毒软件进行测试
3. 如果是企业环境，联系 IT 管理员

---

### 问题 6: 无法运行验证脚本（PowerShell 执行策略限制）

**错误信息**: 
```
无法加载文件 verify_system.ps1，因为在此系统上禁止运行脚本。
```
或
```
File cannot be loaded because running scripts is disabled on this system.
```

**原因**: Windows 默认的 PowerShell 执行策略不允许运行未签名的脚本（安全保护）

**最简单的解决方案**: 直接双击 `verify_system.bat` 文件，它会自动绕过执行策略限制！

**其他解决方案（如果需要运行 .ps1 文件）**:

#### 方法 1: 使用绕过命令（推荐）
```powershell
# 临时绕过执行策略运行单个脚本
powershell -ExecutionPolicy Bypass -File .\verify_system.ps1
```

#### 方法 2: 修改当前用户的执行策略（永久生效）
```powershell
# 打开 PowerShell（无需管理员权限）
Set-ExecutionPolicy RemoteSigned -Scope CurrentUser

# 然后正常运行脚本
.\verify_system.ps1
```

#### 方法 3: 解除阻止（如果下载的脚本）
如果脚本是从网上下载的，Windows会标记为"阻止"：
```powershell
# 右键脚本文件 → 属性 → 勾选"解除阻止" → 确定
# 或使用命令：
Unblock-File .\verify_system.ps1
```

---

## 高级选项

### 本地部署 VC++ Runtime DLL

使用 `-IncludeVCRedist` 参数可以将 VC++ Runtime DLL 打包到程序目录，用户无需安装：

```powershell
# 打包包含 DLL 的版本
.\Scripts\package.ps1 -IncludeVCRedist

# 或使用快捷方式
.\package_full.ps1
```

打包后的文件：
```
Dist/Latest/
├── manosaba_plus.exe
├── vcruntime140.dll      ← VC++ Runtime
├── vcruntime140_1.dll    ← VC++ Runtime
├── msvcp140.dll          ← VC++ Runtime
├── verify_system.ps1
├── Assets/
└── Shaders/
```

优势：
- 用户无需安装任何额外软件
- 无版本冲突
- DLL 总大小约 715 KB

### 自定义打包脚本

编辑 `Scripts/package.ps1` 以自定义：
- 输出目录结构
- 包含/排除的文件
- README 内容
- 额外的 DLL 依赖

### 添加额外的 DLL

如果项目依赖其他 DLL，可以修改打包脚本：

```powershell
# 在 package.ps1 的"复制文件"部分添加
$extraDll = "path\to\your.dll"
if (Test-Path $extraDll) {
    Copy-Item $extraDll -Destination (Join-Path $distRoot "your.dll") -Force
    Write-Host "  ✓ your.dll" -ForegroundColor Green
}
```

### 打包 Debug 版本

虽然不推荐，但如果需要：

1. 修改 `package.ps1` 中的路径：
```powershell
$buildBinDir = "Build\Bin\Debug"  # 改为 Debug
```

2. 运行打包：
```powershell
.\Scripts\package.ps1 -SkipBuild
```

### 管理历史版本

```powershell
# 查看所有历史 ZIP
Get-ChildItem Dist -Filter "*.zip"

# 清理旧版本（保留最新 3 个）
Get-ChildItem Dist -Filter "*.zip" | 
    Sort-Object LastWriteTime -Descending | 
    Select-Object -Skip 3 | 
    Remove-Item

# 完全清理并重新打包
.\Scripts\package.ps1 -Clean
```

### 创建安装程序

对于更专业的分发，可以使用：
- **Inno Setup**: https://jrsoftware.org/isinfo.php
- **NSIS**: https://nsis.sourceforge.io/
- **WiX Toolset**: https://wixtoolset.org/

示例 Inno Setup 配置片段：
```ini
[Setup]
AppName=まのさば＋
AppVersion=1.0
DefaultDirName={autopf}\まのさば＋
DefaultGroupName=まのさば＋

[Files]
Source: "Dist\Bin\*"; DestDir: "{app}\Bin"
Source: "Dist\Assets\*"; DestDir: "{app}\Assets"; Flags: recursesubdirs

[Icons]
Name: "{group}\まのさば＋"; Filename: "{app}\Bin\manosaba_plus.exe"
```

---

## 检查清单

### 打包前

- [ ] Release 版本已成功编译
- [ ] 本地测试程序可以运行
- [ ] Assets 资源完整
- [ ] 删除了不必要的调试文件

### 打包后

- [ ] 运行 `verify.ps1` 检查 Dist\Latest
- [ ] 手动测试 Latest 版本
- [ ] 检查 ZIP 文件大小合理
- [ ] README.txt 内容正确
- [ ] verify_system.ps1 已包含在打包中

### 分发前

- [ ] 在干净的虚拟机/测试机上测试
- [ ] 运行 verify_system.ps1 确认验证脚本工作正常
- [ ] 确认系统要求文档准确
- [ ] 准备好技术支持联系方式
- [ ] 确认 ZIP 文件名包含时间戳

### 分发后

- [ ] 收集用户反馈
- [ ] 记录常见问题
- [ ] 准备更新/补丁计划
- [ ] 保留历史版本的 ZIP 文件

---

## 相关文档

- [项目架构](ARCHITECTURE_CN.md)
- [构建说明](../README_CN.md#编译)
- [开发指南](../README_CN.md)

## 许可证

请确保分发时遵守项目的 LICENSE 协议。

---

**最后更新**: 2026-02-24
