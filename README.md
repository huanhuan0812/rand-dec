# 构建指南

## 项目概述

**rand-dec** 是一个基于 Qt6 的 Windows 桌面应用程序，支持 **x64** 和 **ARM64** 双架构编译，使用 **MSVC** 编译器并采用 **静态链接** 方式（包括 VC 运行时库和 Qt 库）。

---

## 系统要求

### 必需软件
| 软件 | 版本 | 说明 |
|------|------|------|
| **Visual Studio** | 2022 或 2019 | 包含 MSVC 编译器 |
| **CMake** | ≥ 3.16 | 构建系统 |
| **Qt6** | ≥ 6.2.0 | **必须使用静态编译版本** |
| **Windows SDK** | 10.0.17134.0 (Win10 1803) | 限制的最低 SDK 版本 |

### 可选软件
- **Git**：用于版本控制
- **Inno Setup** / **NSIS**：用于打包安装程序

---

## 准备工作

### 1. 安装 Visual Studio 2022

下载并安装 [Visual Studio 2022 Community](https://visualstudio.microsoft.com/zh-hans/downloads/)，确保勾选以下工作负载：

- **使用 C++ 的桌面开发**
- 可选：**通用 Windows 平台开发**（ARM64 支持需要）

在"单个组件"中确认已安装：
- Windows 10 SDK (10.0.17134.0) 
- MSVC v143 - VS 2022 C++ x64/x86/ARM64 生成工具

> **注意**：如果缺少 Windows 10 SDK 10.0.17134.0，可以在 VS Installer 中单独添加。

### 2. 安装 Qt6 静态库

#### 方式一：使用官方预编译静态库（推荐）

从 [Qt 官方下载页面](https://download.qt.io/official_releases/qt/) 下载 **静态编译版本**，例如：
- `qt-everywhere-src-6.x.x.zip`（需要自行编译）
- 或使用第三方提供的静态编译包

#### 方式二：自行编译 Qt6 静态库

```bash
# 配置静态编译
configure -static -static-runtime -release -opensource -confirm-license \
    -platform win32-msvc \
    -prefix C:/Qt/6.x.x/static_msvc2022_64 \
    -qt-zlib -qt-pcre -qt-libpng -qt-libjpeg -qt-freetype \
    -skip qtwebengine -skip qtwebview -skip qtpdf

# 编译
cmake --build . --parallel
cmake --install .
```

> **ARM64 静态 Qt 库**：需要额外指定 `-platform win32-arm64-msvc`

### 3. 安装 CMake

下载 [CMake](https://cmake.org/download/) ≥ 3.16 版本，确保 `cmake.exe` 已加入系统 PATH。

---

## 快速构建

### 构建 x64 版本（Release）

```bash
# 创建构建目录
mkdir build-x64
cd build-x64

# 配置 CMake
cmake .. -G "Visual Studio 17 2022" -A x64 ^
    -DCMAKE_SYSTEM_VERSION=10.0.17134.0 ^
    -DQT_STATIC_PREFIX="C:/Qt/6.7.2/static_msvc2022_64"

# 编译
cmake --build . --config Release --parallel
```

### 构建 ARM64 版本（Release）

```bash
# 创建构建目录
mkdir build-arm64
cd build-arm64

# 配置 CMake
cmake .. -G "Visual Studio 17 2022" -A ARM64 ^
    -DCMAKE_SYSTEM_VERSION=10.0.17134.0 ^
    -DQT_STATIC_PREFIX="C:/Qt/6.7.2/static_msvc2022_arm64"

# 编译
cmake --build . --config Release --parallel
```

---

## 使用 Visual Studio IDE 构建

### 方式一：使用 CMakePresets（推荐）

1. 在项目根目录创建 `CMakePresets.json`（可选）：
```json
{
  "version": 3,
  "configurePresets": [
    {
      "name": "windows-x64",
      "generator": "Visual Studio 17 2022",
      "architecture": "x64",
      "cacheVariables": {
        "CMAKE_SYSTEM_VERSION": "10.0.17134.0",
        "QT_STATIC_PREFIX": "C:/Qt/6.7.2/static_msvc2022_64"
      }
    },
    {
      "name": "windows-arm64",
      "generator": "Visual Studio 17 2022",
      "architecture": "ARM64",
      "cacheVariables": {
        "CMAKE_SYSTEM_VERSION": "10.0.17134.0",
        "QT_STATIC_PREFIX": "C:/Qt/6.7.2/static_msvc2022_arm64"
      }
    }
  ]
}
```

2. 在 VS 中打开项目文件夹：
   - 文件 → 打开 → 文件夹...
   - 选择 `CMakeLists.txt` 所在目录
   - 从顶部工具栏选择预设并构建

### 方式二：使用 VS 开发者命令提示符

打开 **x64 Native Tools Command Prompt for VS 2022** 或 **ARM64 Native Tools Command Prompt**：

```bash
# x64 构建
mkdir build-x64 && cd build-x64
cmake .. -DCMAKE_SYSTEM_VERSION=10.0.17134.0 -DQT_STATIC_PREFIX="C:/Qt/static/6.x.x/msvc2022_64"
msbuild rand-dec.sln /p:Configuration=Release /m

# ARM64 构建
mkdir build-arm64 && cd build-arm64
cmake .. -DCMAKE_SYSTEM_VERSION=10.0.17134.0 -DQT_STATIC_PREFIX="C:/Qt/static/6.x.x/msvc2022_arm64"
msbuild rand-dec.sln /p:Configuration=Release /m
```

---

## 构建配置选项

### CMake 参数说明

| 参数 | 说明 | 示例值 |
|------|------|--------|
| `-G` | 生成器类型 | `"Visual Studio 17 2022"` |
| `-A` | 目标架构 | `x64` 或 `ARM64` |
| `-DCMAKE_SYSTEM_VERSION` | Windows SDK 版本 | `10.0.17134.0` |
| `-DQT_STATIC_PREFIX` | Qt 静态库安装路径 | `C:/Qt/6.7.2/static_msvc2022_64` |
| `-DCMAKE_BUILD_TYPE` | 构建类型（单配置生成器） | `Release` 或 `Debug` |

### 构建配置（多配置生成器）

使用 Visual Studio 生成器时，通过 `--config` 指定：

```bash
# Debug 版本
cmake --build . --config Debug

# Release 版本（默认）
cmake --build . --config Release

# Release with Debug Info
cmake --build . --config RelWithDebInfo

# 最小体积 Release
cmake --build . --config MinSizeRel
```

---

## 验证构建结果

### 输出文件位置

| 架构 | 输出路径 |
|------|----------|
| x64 | `build-x64/Release/rand-dec.exe` |
| ARM64 | `build-arm64/Release/rand-dec.exe` |

### 检查静态链接

使用 `dumpbin` 或 `Dependency Walker` 验证：

```bash
# 检查依赖的 DLL（应该只显示系统 DLL）
dumpbin /dependents rand-dec.exe
```

正常输出应该只包含：
- `KERNEL32.dll`
- `USER32.dll`
- `GDI32.dll`
- `VCRUNTIME140.dll`（如果未正确静态链接，则会出现）

> 如果看到 `Qt6Core.dll` 等 Qt 动态库，说明 Qt 未正确静态链接。

### 检查架构

```bash
dumpbin /headers rand-dec.exe | findstr "machine"
```

- x64: `8664 machine (x64)`
- ARM64: `AA64 machine (ARM64)`

---

## 故障排除

### 问题 1: 找不到 Windows SDK 10.0.17134.0

**错误信息**：`Could not locate Windows SDK version 10.0.17134.0`

**解决方案**：
1. 打开 Visual Studio Installer
2. 修改已安装的 VS 2022
3. 在"单个组件"中搜索并安装 `Windows 10 SDK (10.0.17134.0)`

### 问题 2: 找不到 Qt6 组件

**错误信息**：`find_package(Qt6 REQUIRED COMPONENTS Widgets) ...`

**解决方案**：
1. 确认 `QT_STATIC_PREFIX` 路径正确
2. 确认该路径下存在 `lib/cmake/Qt6/Qt6Config.cmake`
3. 尝试在路径末尾添加 `lib/cmake`：
   ```bash
   -DQt6_DIR="C:/Qt/6.7.2/static_msvc2022_64/lib/cmake"
   ```

### 问题 3: 静态链接 VC 运行时失败

**错误信息**：`LNK2005: _memmove already defined`

**解决方案**：
确保所有依赖库都使用相同的运行时库设置（全部 `/MT` 或全部 `/MTd`）。
检查是否有第三方库使用了不同的编译设置。

### 问题 4: ARM64 编译失败

**解决方案**：
1. 确认 Visual Studio 已安装 ARM64 开发工具
2. 使用专用的 `arm64` 开发者命令提示符
3. 确认 Qt 静态库也是 ARM64 版本

### 问题 5: 静态 Qt 链接时出现大量未解析符号

**解决方案**：
在 CMakeLists.txt 中已包含必要的 Windows 系统库，如果仍有未解析符号，可能需要添加：

```cmake
target_link_libraries(rand-dec PRIVATE
    # 根据错误信息添加缺失的库
    # 例如：opengl32, winmm, imm32, version 等
)
```

---

## 持续集成配置（CI）

### GitHub Actions 示例

```yaml
name: Build

on: [push]

jobs:
  build-x64:
    runs-on: windows-2022
    steps:
      - uses: actions/checkout@v3
      
      - name: Install Qt
        uses: jurplel/install-qt-action@v3
        with:
          version: '6.7.2'
          arch: 'win64_msvc2022_64'
          
      - name: Build
        run: |
          mkdir build
          cd build
          cmake .. -G "Visual Studio 17 2022" -A x64
          cmake --build . --config Release
          
      - name: Upload Artifact
        uses: actions/upload-artifact@v3
        with:
          name: rand-dec-x64
          path: build/Release/rand-dec.exe
```

---

## 联系与支持

如有问题，请提交 Issue 或联系项目维护者。

---

**最后更新**：2026-07-04