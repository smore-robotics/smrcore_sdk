# Python 示例

Python 示例按主题分组放在 `examples_py/` 下，与 C++ 的 `examples/` 结构一致。
运行任意示例：

```bash
python examples_py/basics/connect.py [robot_ip]
```

省略 `robot_ip` 即可连接本机仿真器。

每个示例源文件顶部的 docstring 才是完整参考（用法、行为、安全提示）。下表按示例
概括它展示什么、主要 API、前置条件与适用场景。

这些示例使用真实的 Python 绑定风格（扁平方法、真值 `Result`、不可变 `Pose`），
约定详见 [Python API 摘要](../api.md)。

## 推荐阅读路径

如果你刚开始使用 SDK，建议按下面顺序阅读和运行：

1. `basics/connect.py` —— 确认 SDK 可以连接。
2. `basics/read_state.py` —— 查看机器人状态和电机状态。
3. `motion/movej.py` —— 运行最简单的关节空间规划运动。
4. `motion/movep.py` 或 `motion/movel.py` —— 尝试小幅笛卡尔运动。
5. `motion/servoj.py` 或 `motion/servop.py` —— 了解高频伺服目标流。
6. `config/config_limits.py`、`config/waypoints.py`、`config/payload.py`
   —— 熟悉常用运行时配置接口。
7. 基础流程清楚后，再继续看其它运动示例和力控示例。

## 基础（basics）

查看完整源码讲解：[基础示例](basics.md)

| 源文件 | 作用 | 主要 API | 前置条件 | 适用场景 |
|---|---|---|---|---|
| `basics/connect.py` | 最小生命周期：连接、检查、断开 | `Initialize` / `IsConnected` / `Shutdown` | 无 | 安装 wheel 后的第一个连通性测试 |
| `basics/read_state.py` | 读取机器人信息 + 状态快照+ 电机状态 | `GetRobotInfo` / `GetState` / `GetMotorStatus` / `GetControlMode` | 无 | 检查机器人标识、状态、位姿、电机标志 |
| `basics/linked_sdk.py` | 报告包版本与链接的原生 SDK 版本 | `rcore_sdk.__version__` / `_native.linked_sdk` | 无 | 报告问题前确认安装情况 |
| `basics/error_recovery.py` | 恢复链 `EStop → Recover → ClearError → Enable` | `Enable` / `EStop` / `Recover` / `ClearError` | 无（会触发急停） | 理解完整恢复流程 |

## 运动（motion）

所有运动示例都会自行使能电机；请清空工作区、保证急停可触达。笛卡尔类示例都从当前
TCP 移动几厘米。

查看完整源码讲解：[运动示例](motion.md)

| 源文件 | 作用 | 主要 API | 前置条件 | 适用场景 |
|---|---|---|---|---|
| `motion/movej.py` | 关节空间到固定保守目标（演示单次 `velocity_scale`） | `MoveJ(JointPositions, velocity_scale=...)` | — | 第一个规划运动示例 |
| `motion/movep.py` | 笛卡尔点动：当前位姿 +5cm(Z) | `MoveP(Pose)` / `Pose.from_euler` | — | 运动到笛卡尔目标位姿 |
| `motion/movel.py` | 笛卡尔直线 +5cm(Y) | `MoveL(Pose)` | — | 执行一段直线笛卡尔运动 |
| `motion/movec.py` | 经 via 到 goal 的圆弧 | `MoveC(via, goal)` | — | 执行一段小圆弧 |
| `motion/move_path.py` | 多路点笛卡尔融合路径 | `MovePath(list[dict])` | — | 将多个笛卡尔路点作为一条路径执行 |
| `motion/async_motion.py` | 异步运动 + 暂停/继续 + 状态轮询 | `MoveJ(asynchronous=True)` / `GetMotionTaskStatus` | — | 监控并控制正在执行的运动任务 |
| `motion/kinematics.py` | 正/逆运动学 + 雅可比 + 可达性检查（不运动） | `ForwardKinematics` / `InverseKinematics` / `GetJacobian` / `IsReachable` | — | 不移动机器人查询运动学信息 |
| `motion/servoj.py` | 关节空间高频伺服 | `ServoJ(JointPositions)` | — | 高频发送平滑关节目标 |
| `motion/servop.py` | 笛卡尔空间高频伺服 | `ServoP(Pose)` | — | 高频发送平滑笛卡尔目标 |

## 配置（config）

查看完整源码讲解：[配置示例](config.md)

| 源文件 | 作用 | 主要 API | 前置条件 | 适用场景 |
|---|---|---|---|---|
| `config/config_limits.py` | 读取/修改/校验/恢复运动限制 | `Get`/`SetVelocityPercentage` / `GetMaxVelocity` / `GetCartesianLimits` | 无 | 查看和调整运行时运动限制 |
| `config/waypoints.py` | 命名点位增/列/删（拒绝同名覆盖） | `GetWaypoints` / `AddWaypoint` / `RemoveWaypoint` | 无 | 管理命名关节点位，不触发运动 |
| `config/payload.py` | 设/读/恢复末端负载 | `GetPayload` / `SetPayload` / `ClearPayload` | 无 | 为动力学相关功能设置负载参数 |

## 力控（compliance）

力控示例使用力矩/力控制。请先用示例给出的保守参数起步，并确保急停可触达。

查看完整源码讲解：[柔顺控制示例](compliance.md)

| 源文件 | 作用 | 主要 API | 前置条件 | 适用场景 |
|---|---|---|---|---|
| `compliance/cartesian_impedance.py` | 笛卡尔阻抗；平衡点流式沿 Z +5cm 往返 | `EnableCartesianImpedance` / `SetCartesianImpedanceTarget` / `DisableCartesianImpedance` | — | 体验保守参数下的笛卡尔阻抗行为 |
| `compliance/fd_cartesian_admittance.py` | 力主导笛卡尔导纳（由实测六维力驱动） | `EnsureFtSensor` / `UpdateFdCartesianAdmittanceParams` / `EnableFdCartesianAdmittance` | 需装六维力传感器 + 已保存 FT 标定 | 使用六维力传感器体验力主导笛卡尔导纳 |

## 安全

!!! warning "安全提示"
    运行任何运动或力控示例前，请先检查源文件中的目标点与参数，确认其对当前机器人、
    工具、负载和工作空间安全。力控示例使用力矩控制，请先用示例给出的保守参数起步，
    并确保急停可触达。
