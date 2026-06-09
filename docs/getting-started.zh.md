# 快速开始

本页介绍如何下载 SDK、基于它构建，以及运行一个最小的 C++ 或 Python 程序。请选择你的路径：

<div class="grid cards" markdown>

-   :material-language-cpp:{ .lg .middle } **C++ SDK**

    ---

    下载压缩包、构建示例，并接入 CMake。

    [前往 C++ SDK →](#c-sdk)

-   :material-language-python:{ .lg .middle } **Python SDK**

    ---

    安装 wheel、验证链接的 SDK，并运行示例。

    [前往 Python SDK →](#python-sdk)

</div>

## Release 制品

SDK 制品发布在 GitHub Releases：

```text
https://github.com/smore-robotics/smrcore_sdk/releases
```

每个版本包含 C++ SDK 压缩包、Python wheel 和文档 PDF：

| 制品 | 说明 |
|---|---|
| `smrcore_sdk-cpp-linux-x86_64-v<version>.tar.gz` | Linux x86_64 的 C++ SDK |
| `smrcore_sdk-cpp-windows-x86_64-v<version>.tar.gz` | Windows x86_64 的 C++ SDK |
| `rcore_sdk_py-<version>-<python-tags>.whl` | Python wheel（按 Python ABI / 平台） |
| `smrcore_sdk-docs-zh-v<version>.pdf` | 中文文档（即本手册） |

## C++ SDK {#c-sdk}

### 下载

Linux 用户可以直接使用脚本：

```bash
./scripts/download.sh
```

默认会下载最新 C++ SDK Release，并解压到 `3rdparty/smrcore_sdk/`。如需固定版本：

```bash
VERSION=0.0.1 ./scripts/download.sh
```

也可以手动下载对应平台的压缩包并解压到任意目录，构建时将该目录作为
`CMAKE_PREFIX_PATH` 传给 CMake。

### 构建示例

```bash
./scripts/build.sh
./build/basics_connect [robot_ip]   # 省略 robot_ip 即为本机仿真
```

### 接入你的 CMake 工程

C++ SDK 压缩包提供头文件、库文件和 CMake package：

```text
3rdparty/smrcore_sdk/
├── include/rcore/sdk/*.hpp
└── lib/cmake/smrcore_sdk/
```

```cmake
cmake_minimum_required(VERSION 3.16)
project(my_robot_app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(smrcore_sdk REQUIRED)

add_executable(my_robot_app main.cpp)
target_link_libraries(my_robot_app PRIVATE smrcore::sdk)
```

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/smrcore_sdk
cmake --build build
```

### 最小程序

```cpp
#include "sdk/robot.hpp"

#include <cstdio>

int main()
{
    rcore::sdk::Robot robot;
    if (!robot.Initialize(""))
    {
        std::fprintf(stderr, "Initialize failed\n");
        return 1;
    }

    std::printf("connected=%s\n", robot.IsConnected() ? "true" : "false");
    robot.Shutdown();
    return 0;
}
```

## Python SDK {#python-sdk}

### 安装 wheel

下载与 Python ABI 和平台匹配的 wheel：

```bash
VERSION=0.0.1  # 按需替换为目标发布版本
PY_TAG=cp310-cp310-linux_x86_64   # Windows: cp310-cp310-win_amd64
curl -L --fail \
  "https://github.com/smore-robotics/smrcore_sdk/releases/download/v${VERSION}/rcore_sdk_py-${VERSION}-${PY_TAG}.whl" \
  -o rcore_sdk_py-${VERSION}-${PY_TAG}.whl
pip install rcore_sdk_py-${VERSION}-${PY_TAG}.whl
```

### 验证版本

```python
import rcore_sdk
from rcore_sdk import _native

print(rcore_sdk.__version__)
print(_native.linked_sdk())
```

### 最小程序

```python
from rcore_sdk import Robot

robot = Robot()
if not robot.Initialize(""):
    raise RuntimeError("Initialize failed")

print(f"connected={robot.IsConnected()}")
robot.Shutdown()
```

## 后续阅读

- [C++ 示例](examples/index.md)
- [Python 示例](python/examples/index.md)
- [API 参考](api/index.md)

## 兼容性

| 组件 | 平台 |
|---|---|
| C++ SDK | Linux x86_64、Windows x86_64 |
| Python wheel | 与平台和 Python ABI 相关 |
| C++ 标准 | C++17 |
| CMake | 3.16 或更新 |

SDK client 和机器人 controller/runtime 必须使用相同的协议版本。如果
controller 是从不同源码状态重新构建的，请从同一源码状态重新构建并重新发布
SDK 制品。

Python 可通过以下命令检查所链接的 C++ SDK 版本：

```bash
python -c "from rcore_sdk import _native; print(_native.linked_sdk())"
```

## 故障排查

### CMake 找不到 `smrcore_sdk`

通过 `CMAKE_PREFIX_PATH` 传入已解压的 SDK 目录：

```bash
cmake -S examples -B build -DCMAKE_PREFIX_PATH=/path/to/smrcore_sdk
```

该目录应包含：

```text
lib/cmake/smrcore_sdk/smrcore_sdkConfig.cmake
```

### Python Wheel 无法安装

请使用与 Python ABI 和平台匹配的 wheel。例如
`cp310-cp310-linux_x86_64` 需要 Linux x86_64 上的 CPython 3.10。

### 版本不匹配

如果连接或运动 RPC 调用异常失败，请确认 SDK client 和机器人
controller/runtime 是兼容版本。

### 本机仿真

本机仿真时，示例可以省略 `robot_ip`：

```bash
./build/basics_connect
python examples_py/basics/connect.py
```

## 安全说明

!!! warning "安全提示"
    机器人是危险设备。运行任何运动代码前：

    - 确认机器人已可靠安装。
    - 确认工具和负载配置正确。
    - 确认急停可触达。
    - 确认工作空间已清空。
    - 检查所有目标关节位置和笛卡尔位姿。
    - 从保守速度和小距离开始。
    - 如有本机仿真环境，优先使用仿真验证。
