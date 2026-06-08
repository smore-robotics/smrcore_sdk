# Support

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

Robots are hazardous machines. Before running any motion code:

- Confirm the robot is securely installed.
- Confirm the tool and payload are configured correctly.
- Confirm emergency stop is reachable.
- Confirm the workspace is clear.
- Review all target joint positions and Cartesian poses.
- Start with conservative speed and distance.
- Use local simulation first when available.
