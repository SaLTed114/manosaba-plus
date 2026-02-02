# Tests

这个目录包含了项目的所有测试代码。

## 目录结构

```
Tests/
├── CMakeLists.txt          # 测试的主配置文件
├── Game/                   # Game模块的测试
│   └── Story/
│       └── StoryGraphLoaderTest.cpp
├── Core/                   # Core模块的测试（将来）
├── Render/                 # Render模块的测试（将来）
└── Utils/                  # Utils模块的测试（将来）
```

## 运行所有测试

```powershell
# 编译所有测试
cmake --build Build --target Tests

# 或者编译特定的测试
cmake --build Build --target StoryGraphLoaderTest
```

## 运行测试程序

测试可执行文件位于 `Build/Bin/Debug/` 或 `Build/Bin/Release/` 目录下。

```powershell
# 运行 StoryGraphLoaderTest
.\Build\Bin\Debug\StoryGraphLoaderTest.exe

# 运行 StoryRuntimeTest
.\Build\Bin\Debug\StoryRuntimeTest.exe

# 运行 StoryPlayerTest
.\Build\Bin\Debug\StoryPlayerTest.exe
```

## 添加新测试

1. 在对应模块目录下创建测试文件（例如 `Tests/Game/Story/YourTest.cpp`）
2. 在 `Tests/CMakeLists.txt` 中添加新的测试可执行文件配置
3. 重新运行 `cmake -B Build` 生成构建文件
4. 编译并运行测试

## 测试组织原则

- 测试文件命名：`<被测试类名>Test.cpp`
- 测试目录结构镜像源代码结构
- 每个测试应该是独立的可执行文件
- 测试数据放在 `Assets/` 目录下
