# smrcore_sdk

`smrcore_sdk` 是 Smartmore Robot SDK 的公开文档与集成仓库，提供
Release 下载说明、C++ 与 Python 示例、兼容性说明，以及由 C++ 头文件生成
的 API 参考文档。

该仓库不包含 SDK 源码。用户从 GitHub Releases 下载带版本号的 SDK 制品，
并基于这些制品构建自己的应用。

## 开始阅读

- [安装](install.md)
- [使用](usage.md)
- [示例](examples.md)
- [支持](support.md)
- [API 参考](api/index.md)

## 快速开始

```bash
git clone https://github.com/smore-robotics/smrcore_sdk.git
cd smrcore_sdk

./scripts/download.sh
./scripts/build.sh

./build/basics_connect [robot_ip]
./build/basics_read_state [robot_ip]
```

本机仿真可以省略 `robot_ip`。运动示例也已提供，但运行前请先检查目标点：

```bash
./build/motion_movej [robot_ip]
./build/motion_movel [robot_ip]
```

## 仓库内容

- 支持平台的 C++ SDK Release 压缩包。
- 匹配 Python ABI 和平台的 Python wheel。
- `examples/` 中的 C++ 示例。
- `examples_py/` 中的 Python 示例。
- 下载最新 C++ SDK 和构建示例的脚本。
- 从 C++ SDK 头文件生成的 API 参考。

## 安全提示

机器人是危险设备。运行任何运动示例或用户程序前，请确认目标点对当前机器
人、工具、负载和工作空间都是安全的。请确认急停可触达，并确保工作空间
清空。
