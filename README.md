# smrcore_sdk

Smartmore Robot SDK public integration repository.

This repository does not contain the SDK source code. It provides examples,
build scripts, and release download instructions for the prebuilt C++ SDK and
Python wheel.

## Quick Start

```bash
git clone https://github.com/smore-robotics/smrore_sdk.git
cd smrore_sdk

# Download and extract the C++ SDK into ./3rdparty/smrcore_sdk
./scripts/download.sh

# Build C++ examples
./scripts/build.sh

# Run basic examples. Omit robot_ip for local simulation.
./build/01_connect [robot_ip]
./build/02_read_state [robot_ip]
```

`03_movej` and `04_movel` are motion examples. Before running them, open the
source files and verify the target joint positions or Cartesian poses are safe
for your robot, tool, payload, and workspace. Modify the targets if needed.
Also confirm the robot or simulator is ready, emergency stop is reachable, and
the workspace is clear.

```bash
# Review these files first:
#   examples/03_movej.cpp
#   examples/04_movel.cpp

./build/03_movej [robot_ip]
./build/04_movel [robot_ip]
```

## Release Assets

Releases are published on GitHub:

```text
https://github.com/smore-robotics/smrore_sdk/releases
```

Version `0.0.1` assets:

```text
smrcore_sdk-cpp-linux-x86_64-v0.0.1.tar.gz
smrcore_sdk-cpp-windows-x86_64-v0.0.1.tar.gz
rcore_sdk_py-0.0.1-<python-tags>.whl
```

`scripts/download.sh` downloads the Linux C++ SDK directly from the public
GitHub Release URL.

## C++ Integration

The C++ SDK archive extracts to `3rdparty/smrcore_sdk/` and provides a CMake package:

```text
3rdparty/smrcore_sdk/
├── include/rcore/sdk/*.hpp
└── lib/cmake/smrcore_sdk/
```

Use it in your own CMake project:

```cmake
cmake_minimum_required(VERSION 3.16)
project(my_robot_app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(smrcore_sdk REQUIRED)

add_executable(my_robot_app main.cpp)
target_link_libraries(my_robot_app PRIVATE smrcore::sdk)
```

Build with:

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/smrcore_sdk/3rdparty/smrcore_sdk
cmake --build build
```

Include headers as:

```cpp
#include "sdk/robot.hpp"
```

Linux runtime dependencies are handled by the SDK package. No extra environment
variables are required in a normal setup.

## Python Wheel

Download the wheel from the same GitHub Release, then install it:

```bash
VERSION=0.0.1
PY_TAG=cp310-cp310-linux_x86_64
curl -L --fail \
  "https://github.com/smore-robotics/smrore_sdk/releases/download/v${VERSION}/rcore_sdk_py-${VERSION}-${PY_TAG}.whl" \
  -o rcore_sdk_py-${VERSION}-${PY_TAG}.whl
pip install rcore_sdk_py-${VERSION}-${PY_TAG}.whl
python -c "import rcore_sdk; from rcore_sdk import _native; print(_native.linked_sdk())"
```

The `linked_sdk()` output includes the C++ SDK runtime version. It should match
the release version used by the controller/runtime.

## Examples

| Example | Description |
|---------|-------------|
| `01_connect` | Initialize / IsConnected / Shutdown |
| `02_read_state` | Read robot state and motor status |
| `03_movej` | Fixed conservative joint-space motion |
| `04_movel` | Conservative Cartesian line motion |

## Compatibility

- OS: Linux x86_64 and Windows x86_64.
- C++: C++17, CMake 3.16 or newer.
- Python: use the wheel matching your Python ABI and platform.
- The SDK client and robot controller/runtime must use the same protocol
  version. If the controller was rebuilt from a different commit, rebuild and
  republish the SDK assets from the same source state.

## Safety

Robots are hazardous machines. Before running any motion example, verify the
target in the example source code is safe for your setup, the workspace is
clear, and emergency stop is reachable.
