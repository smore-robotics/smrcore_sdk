<div align="center">

<img src="docs/assets/logo.png" alt="Smartmore Robotics" width="96" />

# smrcore_sdk

**基于 Smartmore Robot SDK，快速构建、集成并部署机器人应用。**

[![License](https://img.shields.io/badge/License-Apache%202.0-1f6feb.svg)](LICENSE)
[![Docs](https://img.shields.io/badge/Docs-online-2ea043.svg)](https://smore-robotics.github.io/smrcore_sdk/zh/)
[![CI](https://github.com/smore-robotics/smrcore_sdk/actions/workflows/ci.yml/badge.svg)](https://github.com/smore-robotics/smrcore_sdk/actions/workflows/ci.yml)
[![Release](https://img.shields.io/github/v/release/smore-robotics/smrcore_sdk?label=Release&color=1f6feb)](https://github.com/smore-robotics/smrcore_sdk/releases)

[English](README.md) · **简体中文**

</div>

---

**Smartmore Robot SDK**的公开集成仓库,提供构建脚本、可直接
运行的 C++ 与 Python 示例,以及预编译 C++ SDK 和 Python wheel 的下载助手。本仓库
**不包含 SDK 源码**。

## 文档

安装、使用、完整示例讲解与 C++ API 参考均托管在文档站点。

- **[文档站点](https://smore-robotics.github.io/smrcore_sdk/zh/)**
- **[English docs](https://smore-robotics.github.io/smrcore_sdk/)**
- **[Python SDK](https://smore-robotics.github.io/smrcore_sdk/zh/python/)**
- **[C++ API 参考](https://smore-robotics.github.io/smrcore_sdk/api/cpp/)**

## 快速开始

### C++

```bash
git clone https://github.com/smore-robotics/smrcore_sdk.git
cd smrcore_sdk

./scripts/download.sh            # 下载最新预编译 C++ SDK
./scripts/build.sh               # 构建 C++ 示例

./build/basics_connect [robot_ip] # 省略 robot_ip 即为本机仿真
```

### Python

```bash
VERSION=0.0.1  # 按需替换为目标发布版本
PY_TAG=cp310-cp310-linux_x86_64   # Windows: cp310-cp310-win_amd64
curl -L -O "https://github.com/smore-robotics/smrcore_sdk/releases/download/v${VERSION}/rcore_sdk_py-${VERSION}-${PY_TAG}.whl"
python3 -m pip install "./rcore_sdk_py-${VERSION}-${PY_TAG}.whl"

python3 examples_py/basics/connect.py [robot_ip] # 省略 robot_ip 即为本机仿真
```

更多配置与示例详见
[文档站点](https://smore-robotics.github.io/smrcore_sdk/zh/)。

## Release 制品

Release 制品发布在
**[Releases 页面](https://github.com/smore-robotics/smrcore_sdk/releases)**:

| 制品 | 说明 |
|---|---|
| `smrcore_sdk-cpp-linux-x86_64-v<version>.tar.gz` | Linux x86_64 的 C++ SDK |
| `smrcore_sdk-cpp-windows-x86_64-v<version>.tar.gz` | Windows x86_64 的 C++ SDK |
| `rcore_sdk_py-<version>-<python-tags>.whl` | Python wheel（按 Python ABI / 平台） |
| `smrcore_sdk-docs-zh-v<version>.pdf` | 中文文档 PDF |

## 安全提示

> 机器人是危险设备。运行任何运动示例前,请确认示例源码中的目标点对当前机器人、
> 工具、负载和工作空间都是安全的,并确认急停可触达、工作空间已清空。

## 许可证

本仓库(示例、脚本与文档)以 [Apache License 2.0](LICENSE) 发布。预编译 SDK
制品中打包的第三方组件,其许可证与归属声明随每个 release 压缩包一同提供。

<div align="center">

Copyright © Smartmore Corporation

</div>
