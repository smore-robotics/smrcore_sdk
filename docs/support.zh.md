# 支持

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

机器人是危险设备。运行任何运动代码前：

- 确认机器人已可靠安装。
- 确认工具和负载配置正确。
- 确认急停可触达。
- 确认工作空间已清空。
- 检查所有目标关节位置和笛卡尔位姿。
- 从保守速度和小距离开始。
- 如有本机仿真环境，优先使用仿真验证。
