# Architecture Documentation

[English](ARCHITECTURE.md) | [简体中文](ARCHITECTURE_CN.md)

## Complete File Tree

```
manosaba-plus/
├── App/
│   ├── CMakeLists.txt
│   └── main.cpp                              # Application entry point
│
├── Assets/                                    # Game assets directory
│
├── Build/                                     # CMake build output (generated)
│
├── Core/
│   ├── CMakeLists.txt
│   └── Window/
│       ├── Win32Window.h                      # Win32 window wrapper interface
│       └── Win32Window.cpp                    # Window creation and message processing
│
├── Docs/
│   └── ARCHITECTURE.md                        # This file
│
├── RHI/
│   ├── CMakeLists.txt
│   └── DX11/
│       ├── CMakeLists.txt
│       ├── DX11Common.h                       # Common utilities and macros
│       ├── DX11Device.h                       # D3D11 device wrapper interface
│       ├── DX11Device.cpp                     # Device and context management
│       ├── DX11SwapChain.h                    # Swap chain wrapper interface
│       ├── DX11SwapChain.cpp                  # Swap chain creation and present logic
│       ├── DX11Texture2D.h                    # 2D texture wrapper interface
│       └── DX11Texture2D.cpp                  # Texture creation and resource views
│
├── Render/
│   ├── CMakeLists.txt
│   ├── DX11Renderer.h                         # Main renderer interface
│   ├── DX11Renderer.cpp                       # Frame management and rendering coordination
│   │
│   ├── Draw/
│   │   ├── DrawItem.h                         # Draw item structures and layer definitions
│   │   └── DrawList.h                         # Draw queue with sorting logic
│   │
│   ├── Pipeline/
│   │   ├── SpritePipeline.h                   # Sprite pipeline interface
│   │   ├── SpritePipeline.cpp                 # Pipeline state and shader binding
│   │   ├── ComposePipeline.h                  # Scene composition pipeline interface
│   │   └── ComposePipeline.cpp                # Fullscreen composition pass
│   │
│   ├── Primitives/
│   │   ├── TriangleDemo.h                     # Triangle test renderer interface
│   │   └── TriangleDemo.cpp                   # Basic triangle rendering
│   │
│   ├── Shader/
│   │   ├── ShaderManager.h                    # Shader loading interface
│   │   └── ShaderManager.cpp                  # Shader caching and loading from disk
│   │
│   └── Sprite/
│       ├── SpriteRenderer.h                   # Sprite batch renderer interface
│       └── SpriteRenderer.cpp                 # Vertex buffer management and sprite batching
│
├── Resources/
│   ├── CMakeLists.txt
│   └── Image/
│       ├── WICImageLoader.h                   # WIC image loader interface
│       └── WICImageLoader.cpp                 # Image loading via Windows Imaging Component
│
├── Shaders/
│   ├── compose.hlsl                           # Composition shaders (VS + PS)
│   ├── sprite.hlsl                            # Sprite shaders (VS + PS)
│   └── triangle.hlsl                          # Debug triangle shaders (VS + PS)
│
├── Utils/
│   ├── CMakeLists.txt
│   ├── ConsoleUtils.h                         # Console attachment utilities
│   ├── FileUtils.h                            # File path resolution utilities
│   └── FileUtils.cpp                          # File system helpers
│
├── CMakeLists.txt                             # Root CMake configuration
├── LICENSE                                    # MIT License
├── README.md                                  # English README
└── README_CN.md                               # Chinese README
```

---

## Module Details

### App/ - Application Layer

**Entry Point**
- `main.cpp` - Windows application entry point, initializes window and renderer, main loop

---

### Core/ - Core Systems

**Window Management**
- `Window/Win32Window.h` - Win32 window wrapper interface
- `Window/Win32Window.cpp` - Win32 window implementation, handles window creation and message processing

---

### RHI/DX11/ - DirectX 11 Abstraction Layer

Low-level DirectX 11 resource wrappers providing RAII management.

**Device & Context**
- `DX11Device.h` - D3D11 device and context wrapper interface
- `DX11Device.cpp` - Device creation, feature level detection, context management

**Swap Chain**
- `DX11SwapChain.h` - Swap chain wrapper interface
- `DX11SwapChain.cpp` - Swap chain creation, resizing, present logic, back buffer management

**Texture Resources**
- `DX11Texture2D.h` - 2D texture wrapper interface
- `DX11Texture2D.cpp` - RGBA8 texture creation, render target creation, SRV/RTV generation

**Common Utilities**
- `DX11Common.h` - Helper macros (ThrowIfFailed), common includes

---

### Resources/ - Resource Loading

**Image Loading**
- `Image/WICImageLoader.h` - WIC-based image loader interface
- `Image/WICImageLoader.cpp` - Load PNG/JPEG/BMP images to RGBA8 format using Windows Imaging Component

---

### Render/ - High-Level Rendering System

**Renderer Core**
- `DX11Renderer.h` - Main renderer interface
- `DX11Renderer.cpp` - Offscreen rendering, frame management, viewport setup, multi-pass rendering coordination

**Draw System**
- `Draw/DrawItem.h` - Draw item definitions (SpriteDrawItem, Layer enum, geometric structs)
- `Draw/DrawList.h` - Draw queue with layer sorting and z-ordering

**Sprite Rendering**
- `Sprite/SpriteRenderer.h` - Sprite batch renderer interface
- `Sprite/SpriteRenderer.cpp` - Dynamic vertex buffer management, sprite batching, draw call submission

**Pipeline Configuration**
- `Pipeline/SpritePipeline.h` - Sprite render pipeline interface
- `Pipeline/SpritePipeline.cpp` - Shader binding, render state setup (blend, rasterizer, sampler), constant buffer management
- `Pipeline/ComposePipeline.h` - Scene composition pipeline interface
- `Pipeline/ComposePipeline.cpp` - Fullscreen composition from offscreen RT to backbuffer

**Shader Management**
- `Shader/ShaderManager.h` - Shader loading and caching interface
- `Shader/ShaderManager.cpp` - Compiled shader loading from disk, shader cache management

**Primitives (Debug/Testing)**
- `Primitives/TriangleDemo.h` - Simple triangle rendering test
- `Primitives/TriangleDemo.cpp` - Basic triangle drawing for testing pipeline

---

### Shaders/ - HLSL Shader Sources

**Composition Shaders**
- `compose.hlsl` - Fullscreen composition pass (VS + PS entry points)

**Sprite Shaders**
- `sprite.hlsl` - Sprite rendering (VS + PS entry points): pixel coordinates to NDC, texture sampling

**Debug Shaders**
- `triangle.hlsl` - Debug triangle rendering (VS + PS entry points)

Note: Each .hlsl file contains both vertex shader (VS) and pixel shader (PS) entry points, compiled to separate .cso bytecode files.

---

### Utils/ - Utility Functions

**Console Utilities**
- `ConsoleUtils.h` - Console attachment for debugging in Windows GUI applications

**File System Utilities**
- `FileUtils.h` - File path resolution interface
- `FileUtils.cpp` - Path resolution helpers for locating assets and shaders

---

## Data Flow

```
Application Loop (main.cpp)
    ↓
DX11Renderer::RenderFrame()
    ↓
DrawList (populate sprites, sort by layer/z)
    ↓
┌─────────────────────────────────────────────┐
│ Pass 1: Scene Rendering (Offscreen RT)      │
│ BeginScenePass() → SpriteRenderer::Draw()   │
│ (Background, Stage, Text layers)            │
└─────────────────────────────────────────────┘
    ↓
┌─────────────────────────────────────────────┐
│ Pass 2: Composition (Backbuffer)            │
│ ComposeToBackBuffer() → ComposePipeline     │
│ (Fullscreen blit from Scene RT)             │
└─────────────────────────────────────────────┘
    ↓
┌─────────────────────────────────────────────┐
│ Pass 3: HUD Rendering (Backbuffer)          │
│ SpriteRenderer::Draw() (HUD layer)          │
└─────────────────────────────────────────────┘
    ↓
DX11SwapChain::Present()
```

---

## Key Design Patterns

### RAII Resource Management
All DirectX resources wrapped in classes with ComPtr for automatic lifetime management.

### Layered Architecture
- **RHI Layer**: Direct hardware abstraction (no game logic)
- **Render Layer**: High-level rendering primitives
- **Application Layer**: Game logic and coordination

### Command Pattern (Draw System)
Draw items queued, sorted, then batched for efficient rendering.

### Caching
Shaders cached by ShaderManager to avoid redundant disk I/O.

---

## Rendering Pipeline

1. **Setup Phase**
   - Create D3D11 device and swap chain
   - Load and compile shaders (FXC bytecode)
   - Initialize render pipelines (Sprite, Compose)
   - Create offscreen render target (Scene RT)
   - Load textures via WIC

2. **Frame Loop (Multi-Pass)**
   - **Pass 1: Scene Rendering**
     - Clear draw list
     - Populate draw items (sprites)
     - Sort by layer and z-order
     - Render Background→Text layers to Scene RT at configurable internal resolution
   - **Pass 2: Composition**
     - Blit Scene RT to backbuffer using fullscreen composition
     - Scales internal resolution to window resolution
   - **Pass 3: HUD Rendering**
     - Render HUD layer directly to backbuffer at native window resolution
   - Present to screen

3. **Sprite Rendering**
   - Map dynamic vertex buffer
   - Generate quad vertices for each sprite
   - Unmap buffer
   - Bind pipeline state
   - Per-sprite texture binding and draw calls

4. **Resolution Architecture**
   - **Internal Resolution** (internalW × internalH): Scene RT resolution, configurable for performance scaling
   - **Window Resolution** (width × height): Backbuffer resolution, fixed by window size
   - Scene layers rendered at internal resolution, upscaled via composition
   - HUD layer rendered at native window resolution for crisp UI
