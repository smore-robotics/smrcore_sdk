# Install

SDK assets are published on GitHub Releases:

```text
https://github.com/smore-robotics/smrcore_sdk/releases
```

Each release contains C++ SDK archives and Python wheels:

```text
smrcore_sdk-cpp-linux-x86_64-v<version>.tar.gz
smrcore_sdk-cpp-windows-x86_64-v<version>.tar.gz
rcore_sdk_py-<version>-<python-tags>.whl
```

## C++ SDK

Linux users can use the provided script:

```bash
./scripts/download.sh
```

By default this downloads the latest C++ SDK release and extracts it to:

```text
3rdparty/smrcore_sdk/
```

To use a fixed version:

```bash
VERSION=0.0.1 ./scripts/download.sh
```

For manual installation, download the asset for your platform and extract it to
any directory. Use that directory as `CMAKE_PREFIX_PATH` when building your
application.

## Python Wheel

Download the wheel matching your Python ABI and platform:

```bash
VERSION=0.0.1
PY_TAG=cp310-cp310-linux_x86_64
curl -L --fail \
  "https://github.com/smore-robotics/smrcore_sdk/releases/download/v${VERSION}/rcore_sdk_py-${VERSION}-${PY_TAG}.whl" \
  -o rcore_sdk_py-${VERSION}-${PY_TAG}.whl
pip install rcore_sdk_py-${VERSION}-${PY_TAG}.whl
```

Verify the linked C++ SDK version:

```bash
python -c "import rcore_sdk; from rcore_sdk import _native; print(_native.linked_sdk())"
```
