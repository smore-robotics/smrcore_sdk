# Python SDK

Python SDK 是 Smartmore Robot SDK 的 Python 绑定。它的 API 与 C++ SDK 对齐，
因此相同的机器人流程既可以用 C++ 实现，也可以用 Python 实现。

当你希望快速脚本化、交互式测试、自动化流程，或接入 Python 生态工具时，推荐使用
Python。需要原生应用集成或更严格的构建系统控制时，可以使用 C++。

## 调用风格

Python 绑定有自己的约定，详见 [Python API 摘要](api.md)：

- 扁平方法布局：直接调用 `robot.SetVelocityPercentage(...)`，没有
  `robot.Config()` / `robot.Motion()` 之类的领域句柄。
- 同步调用返回 `Result`，成功时为真值；失败时读取 `result.error_code` 与
  `result.error_msg`。
- 异步调用返回 `AsyncResult`，提供 `IsSuccess()`、`GetErrorCode()`、
  `GetErrorMsg()`、`Wait()`。
- `Pose` 不可变：`pose.tvec` / `pose.rvec` 是只读属性。

## 安装

直接从 GitHub Release 安装与 Python ABI 和平台匹配的 wheel：

```bash
VERSION=0.0.3  # 替换为你所使用的 SDK 版本
PY_TAG=cp310-cp310-linux_x86_64   # Windows: cp310-cp310-win_amd64
python3 -m pip install \
  "https://github.com/smore-robotics/smrcore_sdk/releases/download/v${VERSION}/rcore_sdk_py-${VERSION}-${PY_TAG}.whl"
```

完整的 C++/Python 安装流程见 [快速开始](../getting-started.md)。

## 验证安装

```python
import rcore_sdk
from rcore_sdk import _native

print(rcore_sdk.__version__)
print(_native.linked_sdk())
```

`linked_sdk()` 返回一个描述内置原生 SDK 的字典（版本、链接状态）。参见
`examples_py/basics/linked_sdk.py`。

## 后续阅读

- [Python 示例](examples/index.md) —— 分类、可运行的示例。
- [Python API 摘要](api.md) —— Python 调用如何映射到 SDK 概念。
- [C++ API 参考](../api/index.md) —— 权威的详细类型参考。
