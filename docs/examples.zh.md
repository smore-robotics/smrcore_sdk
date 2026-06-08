# 示例

示例按主题分组放在 `examples/`（C++）下。每个 C++ 示例会构建为
`build/<目录>_<名称>`（例如 `examples/basics/connect.cpp` 构建为
`build/basics_connect`）。

> Python 示例（`examples_py/`）正在对齐到相同的分类结构。其中少数主题依赖当前
> Python wheel 尚未导出的能力，暂时**仅 C++**（下表已标注）。

## 基础（basics）

| 示例 | C++ | 说明 |
|---|---|---|
| connect | `examples/basics/connect.cpp` | 初始化、检查连接、关闭 |
| read_state | `examples/basics/read_state.cpp` | 读取机器人状态和电机状态 |
| error_recovery | `examples/basics/error_recovery.cpp` | 急停/安全停恢复：EStop -> Recover -> ClearError -> Enable |

## 运动（motion）

| 示例 | C++ | 说明 |
|---|---|---|
| movej | `examples/motion/movej.cpp` | 关节空间运动（固定保守目标） |
| movel | `examples/motion/movel.cpp` | 从当前 TCP 出发的短距离笛卡尔直线运动 |
| motion_api | `examples/motion/motion_api.cpp` | 使用 Motion 领域句柄 + 正/逆运动学 |
| move_path | `examples/motion/move_path.cpp` | 笛卡尔复合路径（stop/blend 路点） |
| async_motion | `examples/motion/async_motion.cpp` | 异步运动 + 暂停/继续 + 任务状态轮询 |
| servoj | `examples/motion/servoj.cpp` | 1 kHz 关节空间高频伺服（ServoJ） |
| servop | `examples/motion/servop.cpp` | 1 kHz 笛卡尔空间高频伺服（ServoP） |

## 配置（config）

| 示例 | C++ | 说明 |
|---|---|---|
| config_limits | `examples/config/config_limits.cpp` | 读取/修改/恢复运动限制 |
| waypoints | `examples/config/waypoints.cpp` | 命名点位增删查 + 按名运动 |
| payload | `examples/config/payload.cpp` | 设置/读取/清除末端负载（仅 C++） |

## 柔顺力控（compliance）

| 示例 | C++ | 说明 |
|---|---|---|
| cartesian_impedance | `examples/compliance/cartesian_impedance.cpp` | 笛卡尔阻抗（CST），手推回弹（仅 C++） |
| fd_cartesian_admittance | `examples/compliance/fd_cartesian_admittance.cpp` | 力主导笛卡尔导纳（仅 C++） |

## 构建与运行（C++）

```bash
./scripts/download.sh
./scripts/build.sh

./build/basics_connect [robot_ip]
./build/basics_read_state [robot_ip]
```

本机仿真可省略 `robot_ip`。

## 运动与力控安全

运行任何运动或力控示例前，请先检查源文件中的目标点与参数，确认其对当前
机器人、工具、负载和工作空间安全。力控示例使用力矩控制，请先用示例给出的保守
参数起步，并确保急停可触达。

```bash
./build/motion_movej [robot_ip]
./build/motion_movel [robot_ip]
```
