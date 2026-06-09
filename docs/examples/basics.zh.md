# 基础示例

## 运行前

基础示例不会移动机器人，可直接连接本机仿真器运行（省略 `robot_ip`）。每个示例源
文件顶部的注释即完整说明。

## connect

### 作用

最小生命周期：连接机器人、检查连接状态、断开。

### 适用场景

- 验证 SDK 安装与连接是否正常。
- 作为任何程序的起手式模板。

### 完整源码

```cpp
--8<-- "examples/basics/connect.cpp"
```

## read_state

### 作用

读取一次完整的机器人状态快照以及电机状态。

### 适用场景

- 调试时查看关节/笛卡尔状态、估计 TCP 六维力。
- 确认当前控制模式与锁存错误数。

### 完整源码

```cpp
--8<-- "examples/basics/read_state.cpp"
```

## error_recovery

### 作用

演示急停/安全停后的完整恢复链：`EStop → Recover → ClearError → Enable`
（`Recover` 硬件侧、`ClearError` 控制器侧、`Enable` 重新上使能，三步缺一不可）。

### 适用场景

- 处理急停、安全停、碰撞触发后的恢复。
- 理解三步各自的职责。

> 注意：示例会主动触发急停，请保持工作区清空、急停可触达。

### 完整源码

```cpp
--8<-- "examples/basics/error_recovery.cpp"
```
