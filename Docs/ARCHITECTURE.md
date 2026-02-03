# Architecture Documentation

[English](ARCHITECTURE.md) | [简体中文](ARCHITECTURE_CN.md)

## Complete File Tree

```
manosaba-plus/
├── App/
│   ├── CMakeLists.txt
│   ├── Application.h                          # Application framework interface
│   ├── Application.cpp                        # Application initialization and main loop
│   ├── main.cpp                               # Application entry point
│   └── Scene/
│       ├── DemoScene.h                        # Demo scene interface
│       └── DemoScene.cpp                      # Demo scene implementation
│
├── Assets/                                    # Game assets directory
│   ├── Font/                                  # Font files
│   ├── Image/                                 # Image texture files
│   ├── Mesh/                                  # 3D mesh files
│   └── Story/
│       └── Demo/
│           ├── demo_story.graph.json          # Demo story graph definition
│           ├── VN/                            # Visual novel script files (.txt)
│           ├── Present/                       # Evidence presentation definitions (.json)
│           └── Debate/                        # Debate definition files (.json)
│
├── Build/                                     # CMake build output (generated)
│
├── Core/
│   ├── CMakeLists.txt
│   ├── Input/
│   │   ├── InputState.h                       # Input state structure (keyboard, mouse)
│   │   └── InputSystem.h                      # Input system interface
│   ├── Time/
│   │   ├── FrameClock.h                       # Frame timing interface
│   │   └── FrameClock.cpp                     # Delta time and FPS calculation
│   └── Window/
│       ├── Win32Window.h                      # Win32 window wrapper interface
│       └── Win32Window.cpp                    # Window creation and message processing
│
├── Docs/
│   ├── ARCHITECTURE.md                        # This file
│   └── ARCHITECTURE_CN.md                     # Chinese architecture documentation
│
├── Game/
│   ├── CMakeLists.txt
│   ├── Common/
│   │   ├── Logger.h                           # Logging interface
│   │   └── Logger.cpp                         # Console logger implementation
│   └── Story/
│       ├── CMakeLists.txt
│       ├── StoryTypes.h                       # Core story type definitions (NodeType, Trigger, Effect)
│       ├── StoryGraph.h                       # Story graph structure (nodes and edges)
│       ├── StoryGraphLoader.h                 # Story graph JSON loader interface
│       ├── StoryGraphLoader.cpp               # JSON parsing and graph construction
│       ├── StoryRuntime.h                     # Story runtime state machine interface
│       ├── StoryRuntime.cpp                   # Graph traversal and event handling
│       ├── StoryPlayer.h                      # High-level story player interface
│       ├── StoryPlayer.cpp                    # Story orchestration and view management
│       ├── StoryView.h                        # Unified view data structures (VN, Present, Debate)
│       ├── Resources/
│       │   ├── VnScript.h                     # Visual novel script structure
│       │   ├── VnScript.cpp                   # VN script parser
│       │   ├── PresentDef.h                   # Evidence presentation definition
│       │   ├── PresentDefLoader.h             # Present definition loader interface
│       │   ├── PresentDefLoader.cpp           # JSON parsing for present nodes
│       │   ├── DebateDef.h                    # Debate definition structure
│       │   ├── DebateDefLoader.h              # Debate definition loader interface
│       │   └── DebateDefLoader.cpp            # JSON parsing for debate nodes
│       └── Runners/
│           ├── VnRunner.h                     # Visual novel runner interface
│           ├── VnRunner.cpp                   # VN text reveal and line management
│           ├── PresentRunner.h                # Evidence presentation runner interface
│           ├── PresentRunner.cpp              # Present node logic and validation
│           ├── DebateRunner.h                 # Debate runner interface
│           └── DebateRunner.cpp               # Statement progression and menu handling
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
│       ├── DX11Texture2D.cpp                  # Texture creation and resource views
│       ├── DX11DepthBuffer.h                  # Depth buffer wrapper interface
│       └── DX11DepthBuffer.cpp                # Depth stencil buffer management
│
├── Render/
│   ├── CMakeLists.txt
│   ├── DX11Renderer.h                         # Main renderer interface
│   ├── DX11Renderer.cpp                       # Frame management and rendering coordination
│   ├── DX11CommonState.h                      # Common render states interface
│   ├── DX11CommonState.cpp                    # Blend/rasterizer/sampler state creation
│   ├── DX11RenderUtils.h                      # Rendering utility functions
│   ├── RenderPlan.h                           # Render plan and pass scheduling
│   │
│   ├── Demo/                                  # (Empty - legacy demos removed)
│   │
│   ├── Draw/
│   │   ├── CardDrawItem.h                     # Card draw item structures
│   │   ├── MeshDrawItem.h                     # Mesh draw item structures
│   │   ├── SpriteDrawItem.h                   # Sprite draw item structures
│   │   └── DrawList.h                         # Draw queue with sorting logic
│   │
│   ├── Drawers/
│   │   ├── CardDrawer.h                       # Card batch renderer interface
│   │   ├── CardDrawer.cpp                     # Card rendering implementation
│   │   ├── MeshDrawer.h                       # Mesh renderer interface
│   │   ├── MeshDrawer.cpp                     # Mesh rendering implementation
│   │   ├── SpriteBatcher.h                    # Sprite batch renderer interface
│   │   ├── SpriteBatcher.cpp                  # Vertex buffer management and sprite batching
│   │   ├── DrawServices.h                     # Centralized drawer services interface
│   │   └── DrawServices.cpp                   # Drawer factory and management
│   │
│   ├── Passes/
│   │   ├── IRenderPass.h                      # Render pass interface
│   │   ├── RenderPassBase.h                   # Base render pass implementation
│   │   ├── ComposePass.h                      # Composition pass interface
│   │   ├── ComposePass.cpp                    # Fullscreen composition pass
│   │   ├── SceneSpritePass.h                  # Scene sprite rendering pass interface
│   │   ├── SceneSpritePass.cpp                # 2D sprite scene rendering
│   │   ├── CardPass.h                         # Card rendering pass interface
│   │   ├── CardPass.cpp                       # Card rendering pass implementation
│   │   ├── MeshPass.h                         # Mesh rendering pass interface
│   │   └── MeshPass.cpp                       # 3D mesh rendering pass
│   │
│   ├── Pipelines/
│   │   ├── SpritePipeline.h                   # Sprite pipeline interface
│   │   ├── SpritePipeline.cpp                 # 2D sprite pipeline state and shader binding
│   │   ├── ComposePipeline.h                  # Scene composition pipeline interface
│   │   ├── ComposePipeline.cpp                # Composition pipeline configuration
│   │   ├── CardPipeline.h                     # Card rendering pipeline interface
│   │   ├── CardPipeline.cpp                   # Card pipeline configuration
│   │   ├── MeshPipeline.h                     # 3D mesh pipeline interface
│   │   ├── MeshPipeline.cpp                   # 3D mesh rendering pipeline
│   │   ├── PipelineLibrary.h                  # Pipeline management interface
│   │   └── PipelineLibrary.cpp                # Pipeline caching and management
│   │
│   ├── Scene3D/
│   │   ├── Camera3D.h                         # 3D camera interface
│   │   ├── Camera3D.cpp                       # Camera view and projection matrices
│   │   ├── Mesh.h                             # Mesh data structure
│   │   ├── MeshFactory.h                      # Procedural mesh generation interface
│   │   └── MeshFactory.cpp                    # Mesh factory implementation
│   │
│   └── Shader/
│       ├── ShaderManager.h                    # Shader loading interface
│       └── ShaderManager.cpp                  # Shader caching and loading from disk
│
├── Resources/
│   ├── CMakeLists.txt
│   ├── Image/
│   │   ├── WICImageLoader.h                   # WIC image loader interface
│   │   └── WICImageLoader.cpp                 # Image loading via Windows Imaging Component
│   └── Mesh/
│       ├── MeshData.h                         # Mesh data structure definition
│       ├── MeshLoader.h                       # Mesh file loader interface
│       └── MeshLoader.cpp                     # Mesh loading from disk
│
├── Shaders/
│   ├── compose.hlsl                           # Composition shaders (VS + PS)
│   ├── sprite.hlsl                            # Sprite shaders (VS + PS)
│   ├── card.hlsl                              # Card rendering shaders (VS + PS)
│   ├── triangle.hlsl                          # Debug triangle shaders (VS + PS)
│   └── mesh.hlsl                              # 3D mesh shaders (VS + PS)
│
├── Utils/
│   ├── CMakeLists.txt
│   ├── ConsoleUtils.h                         # Console attachment utilities
│   ├── FileUtils.h                            # File path resolution utilities
│   └── FileUtils.cpp                          # File system helpers
│
├── Tests/
│   ├── CMakeLists.txt
│   ├── README.md                              # Testing documentation
│   └── Game/
│       └── Story/
│           ├── StoryGraphLoaderTest.cpp       # Story graph loading test
│           ├── StoryRuntimeTest.cpp           # Runtime state machine test
│           └── StoryPlayerTest.cpp            # Interactive story player test
│
├── CMakeLists.txt                             # Root CMake configuration
├── LICENSE                                    # MIT License
├── README.md                                  # English README
└── README_CN.md                               # Chinese README
```

---

## Module Details

### App/ - Application Layer

**Application Framework**
- `Application.h` - Application framework interface
- `Application.cpp` - Application initialization, update loop, and scene management
- `main.cpp` - Windows application entry point, creates application instance

**Scene System**
- `Scene/DemoScene.h` - Demo scene interface
- `Scene/DemoScene.cpp` - Demo scene implementation with sprite and 3D rendering examples

---

### Core/ - Core Systems

**Input System**
- `Input/InputState.h` - Input state structure (keyboard and mouse state)
- `Input/InputSystem.h` - Input system interface for polling input state

**Time Management**
- `Time/FrameClock.h` - Frame timing interface
- `Time/FrameClock.cpp` - Delta time calculation, FPS tracking, frame timing utilities

**Window Management**
- `Window/Win32Window.h` - Win32 window wrapper interface
- `Window/Win32Window.cpp` - Win32 window implementation, handles window creation and message processing

---

### Game/ - Game Logic Layer

**Common Utilities**
- `Common/Logger.h` - Logging interface with multiple log levels (Debug, Info, Warning, Error)
- `Common/Logger.cpp` - Console logger implementation with color-coded output

**Story System**

The story system implements a node-based narrative engine supporting visual novel dialogues, evidence presentation, and debate mechanics.

**Core Story Framework**
- `Story/StoryTypes.h` - Core type definitions (NodeType, Trigger, Effect, GraphEvent)
- `Story/StoryGraph.h` - Story graph data structure (nodes, edges, effects)
- `Story/StoryGraphLoader.h` - Story graph JSON loader interface
- `Story/StoryGraphLoader.cpp` - Parses JSON graph files, constructs node/edge relationships
- `Story/StoryRuntime.h` - Story runtime state machine interface
- `Story/StoryRuntime.cpp` - Graph traversal, event handling, effect triggering
- `Story/StoryPlayer.h` - High-level story player interface
- `Story/StoryPlayer.cpp` - Orchestrates runners, updates views, handles user actions
- `Story/StoryView.h` - Unified view structures for all node types (VN, Present, Debate)

**Resource Definitions**
- `Story/Resources/VnScript.h` - Visual novel script structure (commands, speaker, text)
- `Story/Resources/VnScript.cpp` - VN script parser for text-based dialogue files
- `Story/Resources/PresentDef.h` - Evidence presentation definition (prompt, items)
- `Story/Resources/PresentDefLoader.h` - Present definition loader interface
- `Story/Resources/PresentDefLoader.cpp` - JSON parser for evidence selection nodes
- `Story/Resources/DebateDef.h` - Debate definition (statements, menus, options)
- `Story/Resources/DebateDefLoader.h` - Debate definition loader interface
- `Story/Resources/DebateDefLoader.cpp` - JSON parser for debate mechanics

**Node Runners**
- `Story/Runners/VnRunner.h` - Visual novel runner interface
- `Story/Runners/VnRunner.cpp` - Text reveal animation, line progression, script completion
- `Story/Runners/PresentRunner.h` - Evidence presentation runner interface
- `Story/Runners/PresentRunner.cpp` - Evidence selection validation and event generation
- `Story/Runners/DebateRunner.h` - Debate runner interface
- `Story/Runners/DebateRunner.cpp` - Statement advancement, suspicion menu handling, option validation

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

**Depth Buffer**
- `DX11DepthBuffer.h` - Depth buffer wrapper interface
- `DX11DepthBuffer.cpp` - Depth stencil buffer creation and management for 3D rendering

**Common Utilities**
- `DX11Common.h` - Helper macros (ThrowIfFailed), common includes

---

### Resources/ - Resource Loading

**Image Loading**
- `Image/WICImageLoader.h` - WIC-based image loader interface
- `Image/WICImageLoader.cpp` - Load PNG/JPEG/BMP images to RGBA8 format using Windows Imaging Component

**Mesh Loading**
- `Mesh/MeshData.h` - Mesh data structure definition (vertices, indices, materials)
- `Mesh/MeshLoader.h` - Mesh file loader interface
- `Mesh/MeshLoader.cpp` - Load mesh files from disk (custom format support)

---

### Render/ - High-Level Rendering System

**Renderer Core**
- `DX11Renderer.h` - Main renderer interface
- `DX11Renderer.cpp` - Offscreen rendering, frame management, viewport setup, multi-pass rendering coordination
- `DX11CommonState.h` - Common render states interface
- `DX11CommonState.cpp` - Reusable blend, rasterizer, depth-stencil, and sampler state creation
- `DX11RenderUtils.h` - Rendering utility functions and helpers
- `RenderPlan.h` - Render plan structure, pass scheduling and execution logic

**Draw System**
- `Draw/CardDrawItem.h` - Card draw item structures and definitions
- `Draw/MeshDrawItem.h` - Mesh draw item structures and definitions
- `Draw/SpriteDrawItem.h` - Sprite draw item structures (Layer enum, geometric structs)
- `Draw/DrawList.h` - Draw queue with layer sorting and z-ordering

**Drawer Services**
- `Drawers/CardDrawer.h` - Card batch renderer interface
- `Drawers/CardDrawer.cpp` - Card rendering implementation with batching
- `Drawers/MeshDrawer.h` - Mesh renderer interface
- `Drawers/MeshDrawer.cpp` - 3D mesh rendering implementation
- `Drawers/SpriteBatcher.h` - Sprite batch renderer interface
- `Drawers/SpriteBatcher.cpp` - Dynamic vertex buffer management, sprite batching, draw call submission
- `Drawers/DrawServices.h` - Centralized drawer services interface
- `Drawers/DrawServices.cpp` - Drawer factory and management system

**Render Passes**
- `Passes/IRenderPass.h` - Render pass interface definition
- `Passes/RenderPassBase.h` - Base render pass with common functionality
- `Passes/ComposePass.h` - Composition pass interface
- `Passes/ComposePass.cpp` - Fullscreen composition from offscreen RT to backbuffer
- `Passes/SceneSpritePass.h` - Scene sprite rendering pass interface
- `Passes/SceneSpritePass.cpp` - 2D sprite rendering to scene render target
- `Passes/CardPass.h` - Card rendering pass interface
- `Passes/CardPass.cpp` - Card rendering pass implementation
- `Passes/MeshPass.h` - Mesh rendering pass interface
- `Passes/MeshPass.cpp` - 3D mesh rendering pass with depth testing

**Pipeline Configuration**
- `Pipelines/SpritePipeline.h` - Sprite render pipeline interface
- `Pipelines/SpritePipeline.cpp` - 2D sprite shader binding and render state setup
- `Pipelines/ComposePipeline.h` - Scene composition pipeline interface
- `Pipelines/ComposePipeline.cpp` - Composition pipeline configuration
- `Pipelines/CardPipeline.h` - Card rendering pipeline interface
- `Pipelines/CardPipeline.cpp` - Card pipeline configuration
- `Pipelines/MeshPipeline.h` - 3D mesh pipeline interface
- `Pipelines/MeshPipeline.cpp` - 3D mesh rendering pipeline with depth and lighting support
- `Pipelines/PipelineLibrary.h` - Pipeline management interface
- `Pipelines/PipelineLibrary.cpp` - Pipeline caching and centralized management

**3D Scene System**
- `Scene3D/Camera3D.h` - 3D camera interface
- `Scene3D/Camera3D.cpp` - Camera view and projection matrix management
- `Scene3D/Mesh.h` - Mesh data structure definitions
- `Scene3D/MeshFactory.h` - Procedural mesh generation interface
- `Scene3D/MeshFactory.cpp` - Factory for creating common mesh types (cube, sphere, etc.)

**Shader Management**
- `Shader/ShaderManager.h` - Shader loading and caching interface
- `Shader/ShaderManager.cpp` - Compiled shader loading from disk, shader cache management

---

### Shaders/ - HLSL Shader Sources

**Composition Shaders**
- `compose.hlsl` - Fullscreen composition pass (VS + PS entry points)

**Sprite Shaders**
- `sprite.hlsl` - Sprite rendering (VS + PS entry points): pixel coordinates to NDC, texture sampling

**Card Shaders**
- `card.hlsl` - Card rendering (VS + PS entry points): specialized rendering for card game elements

**3D Mesh Shaders**
- `mesh.hlsl` - 3D mesh rendering (VS + PS entry points): vertex transformation, lighting, texture mapping

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
