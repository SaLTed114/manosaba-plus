# 架构文档

[English](ARCHITECTURE.md) | [简体中文](ARCHITECTURE_CN.md)

## 完整文件树

```
manosaba-plus/
├── App/
│   ├── CMakeLists.txt
│   └── main.cpp                              # 应用程序入口点
│
├── Assets/                                    # 游戏资源目录
│
├── Build/                                     # CMake 构建输出（自动生成）
│
├── Core/
│   ├── CMakeLists.txt
│   └── Window/
│       ├── Win32Window.h                      # Win32 窗口封装接口
│       └── Win32Window.cpp                    # 窗口创建与消息处理
│
├── Docs/
│   ├── ARCHITECTURE.md                        # 英文架构文档
│   └── ARCHITECTURE_CN.md                     # 本文件
│
├── RHI/
│   ├── CMakeLists.txt
│   └── DX11/
│       ├── CMakeLists.txt
│       ├── DX11Common.h                       # 通用工具和宏定义
│       ├── DX11Device.h                       # D3D11 设备封装接口
│       ├── DX11Device.cpp                     # 设备与上下文管理
│       ├── DX11SwapChain.h                    # 交换链封装接口
│       ├── DX11SwapChain.cpp                  # 交换链创建与呈现逻辑
│       ├── DX11Texture2D.h                    # 2D 纹理封装接口
│       └── DX11Texture2D.cpp                  # 纹理创建与资源视图
│
├── Render/
│   ├── CMakeLists.txt
│   ├── DX11Renderer.h                         # 主渲染器接口
│   ├── DX11Renderer.cpp                       # 帧管理与渲染协调
│   │
│   ├── Draw/
│   │   ├── DrawItem.h                         # 绘制项结构与层级定义
│   │   └── DrawList.h                         # 绘制队列与排序逻辑
│   │
│   ├── Pipeline/
│   │   ├── SpritePipeline.h                   # 精灵渲染管线接口
│   │   ├── SpritePipeline.cpp                 # 管线状态与着色器绑定
│   │   ├── ComposePipeline.h                  # 场景合成管线接口
│   │   └── ComposePipeline.cpp                # 全屏合成通道
│   │
│   ├── Primitives/
│   │   ├── TriangleDemo.h                     # 三角形测试渲染器接口
│   │   └── TriangleDemo.cpp                   # 基础三角形渲染
│   │
│   ├── Shader/
│   │   ├── ShaderManager.h                    # 着色器加载接口
│   │   └── ShaderManager.cpp                  # 着色器缓存与磁盘加载
│   │
│   └── Sprite/
│       ├── SpriteRenderer.h                   # 精灵批量渲染器接口
│       └── SpriteRenderer.cpp                 # 顶点缓冲区管理与精灵批处理
│
├── Resources/
│   ├── CMakeLists.txt
│   └── Image/
│       ├── WICImageLoader.h                   # WIC 图像加载器接口
│       └── WICImageLoader.cpp                 # 通过 Windows Imaging Component 加载图像
│
├── Shaders/
│   ├── compose.hlsl                           # 合成着色器（VS + PS）
│   ├── sprite.hlsl                            # 精灵着色器（VS + PS）
│   └── triangle.hlsl                          # 调试三角形着色器（VS + PS）
│
├── Utils/
│   ├── CMakeLists.txt
│   ├── ConsoleUtils.h                         # 控制台附加工具
│   ├── FileUtils.h                            # 文件路径解析工具
│   └── FileUtils.cpp                          # 文件系统辅助函数
│
├── CMakeLists.txt                             # 根 CMake 配置
├── LICENSE                                    # MIT 许可证
├── README.md                                  # 英文 README
└── README_CN.md                               # 中文 README
```

---

## 模块详解

### App/ - 应用层

**程序入口**
- `main.cpp` - Windows 应用程序入口点，初始化窗口和渲染器，主循环

---

### Core/ - 核心系统

**窗口管理**
- `Window/Win32Window.h` - Win32 窗口封装接口
- `Window/Win32Window.cpp` - Win32 窗口实现，处理窗口创建和消息处理

---

### RHI/DX11/ - DirectX 11 抽象层

提供 RAII 管理的底层 DirectX 11 资源封装。

**设备与上下文**
- `DX11Device.h` - D3D11 设备和上下文封装接口
- `DX11Device.cpp` - 设备创建、特性级别检测、上下文管理

**交换链**
- `DX11SwapChain.h` - 交换链封装接口
- `DX11SwapChain.cpp` - 交换链创建、尺寸调整、呈现逻辑、后台缓冲区管理

**纹理资源**
- `DX11Texture2D.h` - 2D 纹理封装接口
- `DX11Texture2D.cpp` - RGBA8 纹理创建、渲染目标创建、SRV/RTV 生成

**通用工具**
- `DX11Common.h` - 辅助宏（ThrowIfFailed）、通用头文件包含

---

### Resources/ - 资源加载

**图像加载**
- `Image/WICImageLoader.h` - 基于 WIC 的图像加载器接口
- `Image/WICImageLoader.cpp` - 使用 Windows Imaging Component 加载 PNG/JPEG/BMP 图像为 RGBA8 格式

---

### Render/ - 高层渲染系统

**渲染器核心**
- `DX11Renderer.h` - 主渲染器接口
- `DX11Renderer.cpp` - 离屏渲染、帧管理、视口设置、多通道渲染协调

**绘制系统**
- `Draw/DrawItem.h` - 绘制项定义（SpriteDrawItem、Layer 枚举、几何结构体）
- `Draw/DrawList.h` - 带有层级排序和 z 排序的绘制队列

**精灵渲染**
- `Sprite/SpriteRenderer.h` - 精灵批量渲染器接口
- `Sprite/SpriteRenderer.cpp` - 动态顶点缓冲区管理、精灵批处理、绘制调用提交

**管线配置**
- `Pipeline/SpritePipeline.h` - 精灵渲染管线接口
- `Pipeline/SpritePipeline.cpp` - 着色器绑定、渲染状态设置（混合、光栅化、采样器）、常量缓冲区管理
- `Pipeline/ComposePipeline.h` - 场景合成管线接口
- `Pipeline/ComposePipeline.cpp` - 从离屏 RT 到后备缓冲区的全屏合成

**着色器管理**
- `Shader/ShaderManager.h` - 着色器加载与缓存接口
- `Shader/ShaderManager.cpp` - 从磁盘加载编译后的着色器、着色器缓存管理

**基元（调试/测试）**
- `Primitives/TriangleDemo.h` - 简单三角形渲染测试
- `Primitives/TriangleDemo.cpp` - 用于测试管线的基础三角形绘制

---

### Shaders/ - HLSL 着色器源码

**合成着色器**
- `compose.hlsl` - 全屏合成通道（VS + PS 入口点）

**精灵着色器**
- `sprite.hlsl` - 精灵渲染（VS + PS 入口点）：像素坐标转 NDC、纹理采样

**调试着色器**
- `triangle.hlsl` - 调试三角形渲染（VS + PS 入口点）

注意：每个 .hlsl 文件包含顶点着色器（VS）和像素着色器（PS）两个入口点，编译为分离的 .cso 字节码文件。

---

### Utils/ - 工具函数

**控制台工具**
- `ConsoleUtils.h` - 用于 Windows GUI 应用调试的控制台附加功能

**文件系统工具**
- `FileUtils.h` - 文件路径解析接口
- `FileUtils.cpp` - 用于定位资源和着色器的路径解析辅助函数

---

## 数据流程

```
应用程序循环 (main.cpp)
    ↓
DX11Renderer::RenderFrame()
    ↓
DrawList（填充精灵，按层级/z排序）
    ↓
┌────────────────────────────────────────────────┐
│ 通道 1：场景渲染（离屏 RT）                      │
│ BeginScenePass() → SpriteRenderer::Draw()      │
│ （背景、舞台、文本层）                           │
└────────────────────────────────────────────────┘
    ↓
┌────────────────────────────────────────────────┐
│ 通道 2：合成（后备缓冲区）                       │
│ ComposeToBackBuffer() → ComposePipeline        │
│ （从场景 RT 全屏混合）                           │
└────────────────────────────────────────────────┘
    ↓
┌────────────────────────────────────────────────┐
│ 通道 3：HUD 渲染（后备缓冲区）                   │
│ SpriteRenderer::Draw() (HUD 层)                │
└────────────────────────────────────────────────┘
    ↓
DX11SwapChain::Present()
```

---

## 关键设计模式

### RAII 资源管理
所有 DirectX 资源都封装在类中，使用 ComPtr 实现自动生命周期管理。

### 分层架构
- **RHI 层**：直接硬件抽象（无游戏逻辑）
- **渲染层**：高层渲染原语
- **应用层**：游戏逻辑与协调

### 命令模式（绘制系统）
绘制项排队、排序，然后批量处理以实现高效渲染。

### 缓存机制
ShaderManager 缓存着色器以避免冗余的磁盘 I/O。

---

## 渲染管线

1. **初始化阶段**
   - 创建 D3D11 设备和交换链
   - 加载并编译着色器（FXC 字节码）
   - 初始化渲染管线（Sprite、Compose）
   - 创建离屏渲染目标（Scene RT）
   - 通过 WIC 加载纹理

2. **帧循环（多通道）**
   - **通道 1：场景渲染**
     - 清空绘制列表
     - 填充绘制项（精灵）
     - 按层级和 z 顺序排序
     - 将背景→文本层渲染到场景 RT，使用可配置的内部分辨率
   - **通道 2：合成**
     - 使用全屏合成将场景 RT 混合到后备缓冲区
     - 将内部分辨率缩放到窗口分辨率
   - **通道 3：HUD 渲染**
     - 直接在后备缓冲区上渲染 HUD 层，使用原始窗口分辨率
   - 呈现到屏幕

3. **精灵渲染**
   - 映射动态顶点缓冲区
   - 为每个精灵生成四边形顶点
   - 解除映射缓冲区
   - 绑定管线状态
   - 逐精灵纹理绑定和绘制调用

4. **分辨率架构**
   - **内部分辨率** (internalW × internalH)：场景 RT 分辨率，可配置用于性能缩放
   - **窗口分辨率** (width × height)：后备缓冲区分辨率，由窗口尺寸固定
   - 场景层以内部分辨率渲染，通过合成放大
   - HUD 层以原始窗口分辨率渲染，保持清晰的 UI
