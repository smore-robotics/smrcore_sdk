# 安装

SDK 制品发布在 GitHub Releases：

```text
https://github.com/smore-robotics/smrcore_sdk/releases
```

每个版本包含 C++ SDK 压缩包和 Python wheel：

```text
smrcore_sdk-cpp-linux-x86_64-v<version>.tar.gz
smrcore_sdk-cpp-windows-x86_64-v<version>.tar.gz
rcore_sdk_py-<version>-<python-tags>.whl
```

## C++ SDK

Linux 用户可以直接使用脚本：

```bash
./scripts/download.sh
```

默认会下载最新 C++ SDK Release，并解压到：

```text
3rdparty/smrcore_sdk/
```

如需固定版本：

```bash
VERSION=0.0.1 ./scripts/download.sh
```

也可以手动下载对应平台的压缩包并解压到任意目录。构建应用时，将该目录作为
`CMAKE_PREFIX_PATH` 传给 CMake。

## Python Wheel

下载与 Python ABI 和平台匹配的 wheel：

```bash
VERSION=0.0.1
PY_TAG=cp310-cp310-linux_x86_64
curl -L --fail \
  "https://github.com/smore-robotics/smrcore_sdk/releases/download/v${VERSION}/rcore_sdk_py-${VERSION}-${PY_TAG}.whl" \
  -o rcore_sdk_py-${VERSION}-${PY_TAG}.whl
pip install rcore_sdk_py-${VERSION}-${PY_TAG}.whl
```

验证链接的 C++ SDK 版本：

```bash
python -c "import rcore_sdk; from rcore_sdk import _native; print(_native.linked_sdk())"
```
