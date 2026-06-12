# Python SDK

The Python SDK provides Python bindings for the Smartmore Robot SDK. Its API is
aligned with the C++ SDK, so the same robot workflows can be implemented in
either C++ or Python.

Use Python for scripting, interactive testing, automation, and integration with
Python-based tools. Use C++ when native application integration or build system
control is the priority.

## Calling Style

The Python binding has its own conventions, summarised on the
[Python API Summary](api.md) page:

- A flat method layout: call `robot.SetVelocityPercentage(...)` directly, there
  are no `robot.Config()` / `robot.Motion()` domain handles.
- Synchronous calls return a `Result` that is truthy on success; read
  `result.error_code` and `result.error_msg` on failure.
- Asynchronous calls return an `AsyncResult` with `IsSuccess()`,
  `GetErrorCode()`, `GetErrorMsg()`, and `Wait()`.
- `Pose` is immutable: `pose.tvec` / `pose.rvec` are read-only properties.

## Install

Install the wheel matching your Python ABI and platform directly from a GitHub
Release:

```bash
VERSION=0.0.3  # replace with the release version you use
PY_TAG=cp310-cp310-linux_x86_64   # Windows: cp310-cp310-win_amd64
python3 -m pip install \
  "https://github.com/smore-robotics/smrcore_sdk/releases/download/v${VERSION}/rcore_sdk_py-${VERSION}-${PY_TAG}.whl"
```

For the full C++/Python setup flow, see [Getting Started](../getting-started.md).

## Verify the Installation

```python
import rcore_sdk
from rcore_sdk import _native

print(rcore_sdk.__version__)
print(_native.linked_sdk())
```

`linked_sdk()` returns a dict describing the bundled native SDK (version, link
status). See `examples_py/basics/linked_sdk.py`.

## Next Steps

- [Python Examples](examples/index.md) — categorized, runnable examples.
- [Python API Summary](api.md) — how Python calls map to SDK concepts.
- [C++ API Reference](../api/index.md) — the canonical detailed type reference.
