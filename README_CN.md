# まのさば＋

[English](README.md) | [简体中文](README_CN.md)

-----

为了弥补《魔法少女的魔女审判》本体贫瘠的演出，于是有了这个项目

-----

### 编译 & 运行

```powershell

# 生成构建文件
cmake -B Build

# 编译项目 (默认Debug配置)
cmake --build Build

# 或编译Release配置 (优化版本，体积更小)
cmake --build Build --config Release

# 运行Debug版本
.\Build\Bin\Debug\manosaba_plus.exe

# 运行Release版本
.\Build\Bin\Release\manosaba_plus.exe

```

## 第三方库

本项目使用了以下第三方库：

- **[nlohmann/json](https://github.com/nlohmann/json)** (v3.12.0) - JSON for Modern C++ by Niels Lohmann，采用 MIT 许可证

详见 [ThirdParty/nlohmann/README.md](ThirdParty/nlohmann/README.md)。

## 许可证

本项目采用 MIT 许可证 - 详见 [LICENSE](LICENSE) 文件。