# 基础示例（Python）

## 运行前

请先安装 wheel（见 [Python SDK](../index.md)）。省略 `robot_ip` 即可连接本机
仿真器。`error_recovery.py` 会主动触发急停，请清空工作区、保证急停可触达。

## connect

### 作用

最小连接生命周期：初始化、检查连接、断开。

### 适用场景

- 安装 wheel 后的第一个连通性测试。

### 完整源码

```python
--8<-- "examples_py/basics/connect.py"
```

## read_state

### 作用

读取状态快照（关节、TCP 位姿、控制模式）以及电机状态。

### 适用场景

- 检查机器人状态、位姿和电机标志。

### 完整源码

```python
--8<-- "examples_py/basics/read_state.py"
```

## linked_sdk

### 作用

打印 Python 包版本与内置原生 SDK 信息。不连接机器人。

### 适用场景

- 报告问题前确认安装情况与链接的原生 SDK。

### 完整源码

```python
--8<-- "examples_py/basics/linked_sdk.py"
```

## error_recovery

### 作用

运行完整恢复链 `EStop → Recover → ClearError → Enable`，并在每步后打印电机状态。

### 适用场景

- 理解急停或安全停止后的完整恢复流程。

### 完整源码

```python
--8<-- "examples_py/basics/error_recovery.py"
```
