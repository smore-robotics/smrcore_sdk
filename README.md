<div align="center">

<img src="docs/assets/logo.png" alt="Smartmore Robotics" width="96" />

# smrcore_sdk

**Build, integrate, and deploy robot applications on the Smartmore Robot SDK.**

[![License](https://img.shields.io/badge/License-Apache%202.0-1f6feb.svg)](LICENSE)
[![Docs](https://img.shields.io/badge/Docs-online-2ea043.svg)](https://smore-robotics.github.io/smrcore_sdk/)
[![CI](https://github.com/smore-robotics/smrcore_sdk/actions/workflows/ci.yml/badge.svg)](https://github.com/smore-robotics/smrcore_sdk/actions/workflows/ci.yml)
[![Release](https://img.shields.io/github/v/release/smore-robotics/smrcore_sdk?label=Release&color=1f6feb)](https://github.com/smore-robotics/smrcore_sdk/releases)

**English** · [简体中文](README.zh.md)

</div>

---

The public integration repository for the **Smartmore Robot SDK**. It ships
build scripts, runnable C++ and Python examples, and release download helpers
for the prebuilt C++ SDK and Python wheel. It does **not** contain SDK source
code.

## Documentation

Installation, usage, the full example walkthrough, and the C++ API reference all
live on the documentation site.

- **[Documentation site](https://smore-robotics.github.io/smrcore_sdk/)**
- **[中文文档](https://smore-robotics.github.io/smrcore_sdk/zh/)**
- **[C++ API reference](https://smore-robotics.github.io/smrcore_sdk/api/cpp/)**

## Quick Start

```bash
git clone https://github.com/smore-robotics/smrcore_sdk.git
cd smrcore_sdk

./scripts/download.sh            # fetch the latest prebuilt C++ SDK
./scripts/build.sh               # build the C++ examples

./build/basics_connect [robot_ip] # omit robot_ip for local simulation
```

Python setup and the remaining examples are covered on the
[documentation site](https://smore-robotics.github.io/smrcore_sdk/).

## Release Assets

Prebuilt binaries are published on the
**[Releases page](https://github.com/smore-robotics/smrcore_sdk/releases)**:

```text
smrcore_sdk-cpp-linux-x86_64-v<version>.tar.gz
smrcore_sdk-cpp-windows-x86_64-v<version>.tar.gz
rcore_sdk_py-<version>-<python-tags>.whl
```

## Safety

> Robots are hazardous machines. Before running any motion example, verify the
> target in the example source is safe for your robot, tool, payload, and
> workspace. Confirm the emergency stop is reachable and the workspace is clear.

## License

This repository (examples, scripts, and docs) is released under the
[Apache License 2.0](LICENSE). Prebuilt SDK release artifacts bundle third-party
components whose license and attribution notices ship inside each release
archive.

<div align="center">

Copyright © Smartmore Corporation

</div>
