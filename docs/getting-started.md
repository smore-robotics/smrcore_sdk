# Getting Started

This page covers downloading the SDK, building against it, and running a minimal
C++ or Python program. Pick your path:

<div class="grid cards" markdown>

-   :material-language-cpp:{ .lg .middle } **C++ SDK**

    ---

    Download the tarball, build the examples, and integrate with CMake.

    [Go to C++ SDK →](#c-sdk)

-   :material-language-python:{ .lg .middle } **Python SDK**

    ---

    Install the wheel, verify the linked SDK, and run the examples.

    [Go to Python SDK →](#python-sdk)

</div>

## Release Assets

SDK assets are published on GitHub Releases:

```text
https://github.com/smore-robotics/smrcore_sdk/releases
```

Each release contains C++ SDK archives, a Python wheel, and the documentation
PDF:

| Asset | Description |
|---|---|
| `smrcore_sdk-cpp-linux-x86_64-v<version>.tar.gz` | C++ SDK for Linux x86_64 |
| `smrcore_sdk-cpp-windows-x86_64-v<version>.tar.gz` | C++ SDK for Windows x86_64 |
| `rcore_sdk_py-<version>-<python-tags>.whl` | Python wheel (per Python ABI / platform) |
| `smrcore_sdk-docs-zh-v<version>.pdf` | Chinese documentation (this manual) |

## C++ SDK {#c-sdk}

### Download

Linux users can use the provided script:

```bash
./scripts/download.sh
```

By default this downloads the latest C++ SDK release and extracts it to
`3rdparty/smrcore_sdk/`. To pin a version:

```bash
VERSION=0.0.1 ./scripts/download.sh
```

For manual installation, download the asset for your platform and extract it to
any directory. Use that directory as `CMAKE_PREFIX_PATH` when building.

### Build Examples

```bash
./scripts/build.sh
./build/basics_connect [robot_ip]   # omit robot_ip for local simulation
```

### Use in Your CMake Project

The C++ SDK archive provides headers, libraries, and a CMake package:

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

### Minimal Program

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

### Install Wheel

Download the wheel matching your Python ABI and platform:

```bash
VERSION=0.0.1  # replace with the release version you want
PY_TAG=cp310-cp310-linux_x86_64   # Windows: cp310-cp310-win_amd64
curl -L --fail \
  "https://github.com/smore-robotics/smrcore_sdk/releases/download/v${VERSION}/rcore_sdk_py-${VERSION}-${PY_TAG}.whl" \
  -o rcore_sdk_py-${VERSION}-${PY_TAG}.whl
pip install rcore_sdk_py-${VERSION}-${PY_TAG}.whl
```

### Verify Version

```python
import rcore_sdk
from rcore_sdk import _native

print(rcore_sdk.__version__)
print(_native.linked_sdk())
```

### Minimal Program

```python
from rcore_sdk import Robot

robot = Robot()
if not robot.Initialize(""):
    raise RuntimeError("Initialize failed")

print(f"connected={robot.IsConnected()}")
robot.Shutdown()
```

## Next Steps

- [C++ Examples](examples/index.md)
- [Python Examples](python/examples/index.md)
- [API Reference](api/index.md)

## Compatibility

| Component | Platform |
|---|---|
| C++ SDK | Linux x86_64, Windows x86_64 |
| Python wheel | Platform and Python ABI specific |
| C++ standard | C++17 |
| CMake | 3.16 or newer |

The SDK client and robot controller/runtime must use the same protocol version.
If the controller was rebuilt from a different source state, rebuild and
republish the SDK assets from that same state.

For Python, check the linked C++ SDK version:

```bash
python -c "from rcore_sdk import _native; print(_native.linked_sdk())"
```

## Troubleshooting

### CMake Cannot Find `smrcore_sdk`

Pass the extracted SDK directory through `CMAKE_PREFIX_PATH`:

```bash
cmake -S examples -B build -DCMAKE_PREFIX_PATH=/path/to/smrcore_sdk
```

The directory should contain:

```text
lib/cmake/smrcore_sdk/smrcore_sdkConfig.cmake
```

### Python Wheel Does Not Install

Use a wheel matching your Python ABI and platform. For example,
`cp310-cp310-linux_x86_64` requires CPython 3.10 on Linux x86_64.

### Version Mismatch

If connection or motion RPC calls fail unexpectedly, confirm that the SDK client
and robot controller/runtime were built from compatible versions.

### Local Simulation

For local simulation, omit `robot_ip` in examples:

```bash
./build/basics_connect
python examples_py/basics/connect.py
```

## Safety

!!! warning "Safety"
    Robots are hazardous machines. Before running any motion code:

    - Confirm the robot is securely installed.
    - Confirm the tool and payload are configured correctly.
    - Confirm emergency stop is reachable.
    - Confirm the workspace is clear.
    - Review all target joint positions and Cartesian poses.
    - Start with conservative speed and distance.
    - Use local simulation first when available.
