# 架构文档

[English](ARCHITECTURE.md) | [简体中文](ARCHITECTURE_CN.md)

## 完整文件树

```
manosaba-plus/
├── App/
│   ├── CMakeLists.txt
│   ├── Application.h                          # 应用框架接口
│   ├── Application.cpp                        # 应用初始化与主循环
│   ├── main.cpp                               # 应用程序入口点
│   └── Scene/
│       ├── DemoScene.h                        # 演示场景接口
│       └── DemoScene.cpp                      # 演示场景实现
│
├── Assets/                                    # 游戏资源目录
│   ├── Mesh/                                  # 3D 网格文件
│   └── Textures/                              # 纹理图像文件
│
├── Build/                                     # CMake 构建输出（自动生成）
│
├── Core/
│   ├── CMakeLists.txt
│   ├── Time/
│   │   ├── FrameClock.h                       # 帧计时接口
│   │   └── FrameClock.cpp                     # 增量时间与 FPS 计算
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
│       ├── DX11Texture2D.cpp                  # 纹理创建与资源视图
│       ├── DX11DepthBuffer.h                  # 深度缓冲封装接口
│       └── DX11DepthBuffer.cpp                # 深度模板缓冲管理
│
├── Render/
│   ├── CMakeLists.txt
│   ├── DX11Renderer.h                         # 主渲染器接口
│   ├── DX11Renderer.cpp                       # 帧管理与渲染协调
│   ├── DX11CommonState.h                      # 通用渲染状态接口
│   ├── DX11CommonState.cpp                    # 混合/光栅化/采样器状态创建
│   ├── DX11RenderUtils.h                      # 渲染工具函数
│   ├── RenderPlan.h                           # 渲染计划与通道调度
│   │
│   ├── Demo/                                  # (空目录 - 旧的演示已移除)
│   │
│   ├── Draw/
│   │   ├── CardDrawItem.h                     # 卡片绘制项结构
│   │   ├── MeshDrawItem.h                     # 网格绘制项结构
│   │   ├── SpriteDrawItem.h                   # 精灵绘制项结构
│   │   └── DrawList.h                         # 绘制队列与排序逻辑
│   │
│   ├── Drawers/
│   │   ├── CardDrawer.h                       # 卡片批量渲染器接口
│   │   ├── CardDrawer.cpp                     # 卡片渲染实现
│   │   ├── MeshDrawer.h                       # 网格渲染器接口
│   │   ├── MeshDrawer.cpp                     # 网格渲染实现
│   │   ├── SpriteBatcher.h                    # 精灵批量渲染器接口
│   │   ├── SpriteBatcher.cpp                  # 顶点缓冲区管理与精灵批处理
│   │   ├── DrawServices.h                     # 集中式绘制服务接口
│   │   └── DrawServices.cpp                   # 绘制器工厂与管理
│   │
│   ├── Passes/
│   │   ├── IRenderPass.h                      # 渲染通道接口
│   │   ├── RenderPassBase.h                   # 渲染通道基类实现
│   │   ├── ComposePass.h                      # 合成通道接口
│   │   ├── ComposePass.cpp                    # 全屏合成通道
│   │   ├── SceneSpritePass.h                  # 场景精灵渲染通道接口
│   │   ├── SceneSpritePass.cpp                # 2D 精灵场景渲染
│   │   ├── CardPass.h                         # 卡片渲染通道接口
│   │   ├── CardPass.cpp                       # 卡片渲染通道实现
│   │   ├── MeshPass.h                         # 网格渲染通道接口
│   │   └── MeshPass.cpp                       # 3D 网格渲染通道
│   │
│   ├── Pipelines/
│   │   ├── SpritePipeline.h                   # 精灵管线接口
│   │   ├── SpritePipeline.cpp                 # 2D 精灵管线状态与着色器绑定
│   │   ├── ComposePipeline.h                  # 场景合成管线接口
│   │   ├── ComposePipeline.cpp                # 合成管线配置
│   │   ├── CardPipeline.h                     # 卡片渲染管线接口
│   │   ├── CardPipeline.cpp                   # 卡片管线配置
│   │   ├── MeshPipeline.h                     # 3D 网格管线接口
│   │   ├── MeshPipeline.cpp                   # 3D 网格渲染管线
│   │   ├── PipelineLibrary.h                  # 管线管理接口
│   │   └── PipelineLibrary.cpp                # 管线缓存与管理
│   │
│   ├── Scene3D/
│   │   ├── Camera3D.h                         # 3D 相机接口
│   │   ├── Camera3D.cpp                       # 相机视图与投影矩阵
│   │   ├── Mesh.h                             # 网格数据结构
│   │   ├── MeshFactory.h                      # 程序化网格生成接口
│   │   └── MeshFactory.cpp                    # 网格工厂实现
│   │
│   └── Shader/
│       ├── ShaderManager.h                    # 着色器加载接口
│       └── ShaderManager.cpp                  # 着色器缓存与磁盘加载
│
├── Resources/
│   ├── CMakeLists.txt
│   ├── Audio/                                 # 音频资源文件
│   └── Image/
│       ├── WICImageLoader.h                   # WIC 图像加载器接口
│       └── WICImageLoader.cpp                 # 通过 Windows Imaging Component 加载图像
│
├── Shaders/
│   ├── compose.hlsl                           # 合成着色器（VS + PS）
│   ├── sprite.hlsl                            # 精灵着色器（VS + PS）
│   ├── card.hlsl                              # 卡片渲染着色器（VS + PS）
│   ├── triangle.hlsl                          # 调试三角形着色器（VS + PS）
│   └── mesh.hlsl                              # 3D 网格着色器（VS + PS）
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

**应用框架**
- `Application.h` - 应用框架接口
- `Application.cpp` - 应用初始化、更新循环与场景管理
- `main.cpp` - Windows 应用程序入口点，创建应用实例

**场景系统**
- `Scene/DemoScene.h` - 演示场景接口
- `Scene/DemoScene.cpp` - 演示场景实现，包含精灵和 3D 渲染示例

---

### Core/ - 核心系统

**时间管理**
- `Time/FrameClock.h` - 帧计时接口
- `Time/FrameClock.cpp` - 增量时间计算、FPS 跟踪、帧计时工具

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

**深度缓冲**
- `DX11DepthBuffer.h` - 深度缓冲封装接口
- `DX11DepthBuffer.cpp` - 3D 渲染的深度模板缓冲创建与管理

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
- `DX11CommonState.h` - 通用渲染状态接口
- `DX11CommonState.cpp` - 可复用的混合、光栅化、深度模板与采样器状态创建
- `DX11RenderUtils.h` - 渲染工具函数与辅助功能
- `RenderPlan.h` - 渲染计划结构、通道调度与执行逻辑

**绘制系统**
- `Draw/CardDrawItem.h` - 卡片绘制项结构与定义
- `Draw/MeshDrawItem.h` - 网格绘制项结构与定义
- `Draw/SpriteDrawItem.h` - 精灵绘制项结构（Layer 枚举、几何结构体）
- `Draw/DrawList.h` - 带有层级排序和 z 排序的绘制队列

**绘制服务**
- `Drawers/CardDrawer.h` - 卡片批量渲染器接口
- `Drawers/CardDrawer.cpp` - 带批处理的卡片渲染实现
- `Drawers/MeshDrawer.h` - 网格渲染器接口
- `Drawers/MeshDrawer.cpp` - 3D 网格渲染实现
- `Drawers/SpriteBatcher.h` - 精灵批量渲染器接口
- `Drawers/SpriteBatcher.cpp` - 动态顶点缓冲区管理、精灵批处理、绘制调用提交
- `Drawers/DrawServices.h` - 集中式绘制服务接口
- `Drawers/DrawServices.cpp` - 绘制器工厂与管理系统

**渲染通道**
- `Passes/IRenderPass.h` - 渲染通道接口定义
- `Passes/RenderPassBase.h` - 带有通用功能的渲染通道基类
- `Passes/ComposePass.h` - 合成通道接口
- `Passes/ComposePass.cpp` - 从离屏 RT 到后备缓冲区的全屏合成
- `Passes/SceneSpritePass.h` - 场景精灵渲染通道接口
- `Passes/SceneSpritePass.cpp` - 2D 精灵渲染到场景渲染目标
- `Passes/CardPass.h` - 卡片渲染通道接口
- `Passes/CardPass.cpp` - 卡片渲染通道实现
- `Passes/MeshPass.h` - 网格渲染通道接口
- `Passes/MeshPass.cpp` - 带深度测试的 3D 网格渲染通道

**管线配置**
- `Pipelines/SpritePipeline.h` - 精灵渲染管线接口
- `Pipelines/SpritePipeline.cpp` - 2D 精灵着色器绑定与渲染状态设置
- `Pipelines/ComposePipeline.h` - 场景合成管线接口
- `Pipelines/ComposePipeline.cpp` - 合成管线配置
- `Pipelines/CardPipeline.h` - 卡片渲染管线接口
- `Pipelines/CardPipeline.cpp` - 卡片管线配置
- `Pipelines/MeshPipeline.h` - 3D 网格管线接口
- `Pipelines/MeshPipeline.cpp` - 支持深度与光照的 3D 网格渲染管线
- `Pipelines/PipelineLibrary.h` - 管线管理接口
- `Pipelines/PipelineLibrary.cpp` - 管线缓存与集中式管理

**3D 场景系统**
- `Scene3D/Camera3D.h` - 3D 相机接口
- `Scene3D/Camera3D.cpp` - 相机视图与投影矩阵管理
- `Scene3D/Mesh.h` - 网格数据结构定义
- `Scene3D/MeshFactory.h` - 程序化网格生成接口
- `Scene3D/MeshFactory.cpp` - 用于创建常见网格类型（立方体、球体等）的工厂

**着色器管理**
- `Shader/ShaderManager.h` - 着色器加载与缓存接口
- `Shader/ShaderManager.cpp` - 从磁盘加载编译后的着色器、着色器缓存管理

---

### Shaders/ - HLSL 着色器源码

**合成着色器**
- `compose.hlsl` - 全屏合成通道（VS + PS 入口点）

**精灵着色器**
- `sprite.hlsl` - 精灵渲染（VS + PS 入口点）：像素坐标转 NDC、纹理采样

**卡片着色器**
- `card.hlsl` - 卡片渲染（VS + PS 入口点）：专门用于卡牌游戏元素的渲染

**3D 网格着色器**
- `mesh.hlsl` - 3D 网格渲染（VS + PS 入口点）：顶点变换、光照、纹理映射

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
