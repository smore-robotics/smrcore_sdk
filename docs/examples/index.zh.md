# 示例

示例按主题分组放在 `examples/`（C++）下，每个构建为 `build/<目录>_<名称>`
（例如 `basics/connect.cpp` → `build/basics_connect`）。省略 `robot_ip` 即可连接
本机仿真器。

每个示例源文件顶部的注释才是完整参考（用法、行为、安全提示）。下表按示例概括它
展示什么、主要 API、前置条件与适用场景。

> 需要 Python？请见 [Python 示例](../python/examples/index.md)。

## 推荐阅读路径

如果你刚开始使用 SDK，建议按下面顺序阅读和运行：

1. `basics/connect.cpp` —— 确认 SDK 可以连接。
2. `basics/read_state.cpp` —— 查看机器人状态和电机状态。
3. `motion/movej.cpp` —— 运行最简单的关节空间规划运动。
4. `motion/movep.cpp` 或 `motion/movel.cpp` —— 尝试小幅笛卡尔运动。
5. `motion/servoj.cpp` 或 `motion/servop.cpp` —— 了解高频伺服目标流。
6. `config/config_limits.cpp`、`config/waypoints.cpp`、`config/payload.cpp`
   —— 熟悉常用运行时配置接口。
7. 基础流程清楚后，再继续看其它运动示例和力控示例。

## 基础（basics）

查看完整源码讲解：[基础示例](basics.md)

| 源文件 | 作用 | 主要 API | 前置条件 | 适用场景 |
|---|---|---|---|---|
| `basics/connect.cpp` | 最小生命周期：连接、检查、断开 | `Initialize` / `IsConnected` / `Shutdown` | 无 | 安装 SDK 后的第一个连通性测试 |
| `basics/read_state.cpp` | 读取机器人信息 + 完整状态快照+ 电机状态 | `GetRobotInfo` / `GetState` / `GetMotorStatus` | 无 | 检查机器人标识、位姿、错误、电机标志等状态 |
| `basics/error_recovery.cpp` | 恢复链 `EStop → Recover → ClearError → Enable` | `Enable` / `EStop` / `Recover` / `ClearError` | 无（会触发急停） | 理解完整恢复流程 |

## 运动（motion）

所有运动示例都会自行使能电机；请清空工作区、保证急停可触达。笛卡尔类示例都从当前
TCP 移动几厘米。

查看完整源码讲解：[运动示例](motion.md)

| 源文件 | 作用 | 主要 API | 前置条件 | 适用场景 |
|---|---|---|---|---|
| `motion/movej.cpp` | 关节空间到固定保守目标（演示单次 `velocity_scale`） | `MoveJ(JointPositions, async, velocity_scale)` | — | 第一个规划运动示例 |
| `motion/movep.cpp` | 笛卡尔点动：当前位姿 +5cm(Z) | `MoveP(Pose)` / `Pose::FromEuler` | — | 运动到笛卡尔目标位姿 |
| `motion/movel.cpp` | 笛卡尔直线 +5cm(Y) | `MoveL(Pose)` | — | 执行一段直线笛卡尔运动 |
| `motion/movec.cpp` | 经 via 到 goal 的圆弧 | `MoveC(via, goal)` | — | 执行一段小圆弧 |
| `motion/motion_api.cpp` | Motion 句柄 + 正/逆运动学 + 雅可比 + 可达性检查 | `Motion()` / `ForwardKinematics` / `InverseKinematics` / `GetJacobian` / `IsReachable` / `MoveP` | — | 直接使用领域 API 并查看运动学信息 |
| `motion/move_path.cpp` | 多路点笛卡尔融合路径 | `MovePath(vector<CartesianWaypoint>)` | — | 将多个笛卡尔路点作为一条路径执行 |
| `motion/async_motion.cpp` | 异步运动 + 暂停/继续 + 状态轮询 | `MoveJ(async)` / `GetMotionTaskStatus` / `Pause`·`ContinueMotion` | — | 监控并控制正在执行的运动任务 |
| `motion/servoj.cpp` | 1kHz 关节空间高频伺服 | `ServoJ(JointPositions)` | — | 高频发送平滑关节目标 |
| `motion/servop.cpp` | 1kHz 笛卡尔空间高频伺服 | `ServoP(Pose)` | `cartesian_valid == true` | 高频发送平滑笛卡尔目标 |

## 配置（config）

查看完整源码讲解：[配置示例](config.md)

| 源文件 | 作用 | 主要 API | 前置条件 | 适用场景 |
|---|---|---|---|---|
| `config/config_limits.cpp` | 读取/修改/校验/恢复运动限制（恢复原值） | `Get`/`SetVelocityPercentage` / `GetMaxVelocity` / `GetCartesianLimits` | 无 | 查看和调整运行时运动限制 |
| `config/waypoints.cpp` | 命名点位增/列/删（命名空间化，拒绝同名覆盖） | `GetWaypoints` / `AddWaypoint` / `RemoveWaypoint` | 无 | 管理命名关节点位，不触发运动 |
| `config/payload.cpp` | 设/读/清末端负载（退出前恢复原值） | `GetPayload` / `SetPayload` / `ClearPayload` | 无 | 为动力学相关功能设置负载参数 |

## 力控（compliance）

力控示例使用力矩/力控制。请先用示例给出的保守参数起步，并确保急停可触达。

查看完整源码讲解：[柔顺控制示例](compliance.md)

| 源文件 | 作用 | 主要 API | 前置条件 | 适用场景 |
|---|---|---|---|---|
| `compliance/cartesian_impedance.cpp` | 笛卡尔阻抗(CST)；平衡点 servo 流式沿 Z +5cm 往返 | `EnableCartesianImpedance` / `SetCartesianImpedanceTarget`（流式） / `DisableCartesianImpedance` | — | 体验保守参数下的笛卡尔阻抗行为 |
| `compliance/fd_cartesian_admittance.cpp` | 力主导笛卡尔导纳（由实测六维力驱动） | `EnsureFtSensor` / `FdCartesianAdmittance` 的 `Enable`·`SetPoseTarget`·`Disable` | 需装六维力传感器 + 已保存 FT 标定 | 使用六维力传感器体验力主导笛卡尔导纳 |

## 构建与运行

```bash
./scripts/download.sh
./scripts/build.sh

./build/basics_connect [robot_ip]
./build/basics_read_state [robot_ip]
```

本机仿真可省略 `robot_ip`。

## 运动与力控安全

!!! warning "安全提示"
    运行任何运动或力控示例前，请先检查源文件中的目标点与参数，确认其对当前机器人、
    工具、负载和工作空间安全。力控示例使用力矩控制，请先用示例给出的保守参数起步，
    并确保急停可触达。
