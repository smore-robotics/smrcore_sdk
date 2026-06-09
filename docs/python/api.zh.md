# Python API 摘要

本页概述 Python 的调用风格，以及它如何映射到 [C++ API 参考](../api/index.md)
中记录的 SDK 概念。这不是自动生成的完整参考；完整类型细节请查阅 C++ 参考。

## 版本

```python
import rcore_sdk
print(rcore_sdk.__version__)
```

## linked_sdk()

```python
from rcore_sdk import _native

info = _native.linked_sdk()
print(info["version"])     # 内置原生 SDK 版本
print(info["linked"])      # 原生 SDK 已链接时为 True
```

## Robot 生命周期

```python
from rcore_sdk import Robot

robot = Robot()
robot.Initialize(robot_ip)   # "" 表示本机仿真；返回 bool
robot.IsConnected()          # bool
robot.Enable()               # Result
robot.Disable()              # Result
robot.Shutdown()
```

恢复：`robot.EStop()`、`robot.Recover()`、`robot.ClearError()` 均返回 `Result`。

## 结果（Result）

同步调用返回 `Result`，成功时为真值：

```python
result = robot.MoveJ(target)
if not result:
    print(result.error_code, result.error_msg)
```

用一个小助手让示例更简洁：

```python
def check(result, label):
    if not result:
        raise RuntimeError(
            f"{label} failed: code={result.error_code} msg={result.error_msg}"
        )
```

异步调用（`asynchronous=True`）返回 `AsyncResult`。`Wait()` 会阻塞直到运动完成并
返回一个 `Result`，因此最简洁的写法是复用同一个 `check()` 助手：

```python
ar = robot.MoveJ(target, asynchronous=True)
# ... 可选：轮询 GetMotionTaskStatus()、暂停/继续、并行做其它事 ...
result = ar.Wait()        # 阻塞直到完成；返回 Result
check(result, "MoveJ")
```

注意：`IsSuccess()`、`GetErrorCode()`、`GetErrorMsg()` 内部都会调用 `Wait()`，因此
它们同样是阻塞的。建议直接使用 `Wait()` 返回的 `Result`，而不是依次调用它们。

## 状态（State）

```python
state = robot.GetState()        # RobotState
state.positions                 # 关节位置（6 个）
state.velocities                # 关节速度
state.torques                   # 关节力矩
state.tcp_pose                  # Pose
state.timestamp                 # 浮点秒

robot.GetControlMode()          # int；0 表示 Kinematics

motor = robot.GetMotorStatus()  # MotorStatus
motor.enabled, motor.estop, motor.error, motor.operational
```

## 位姿与关节数据

`JointPositions` 与 `Pose` 由浮点序列构造。`Pose` 不可变；`tvec` / `rvec` 是返回
3 元组的只读属性。

```python
from rcore_sdk import JointPositions, Pose

jp = JointPositions([0.0, -1.5708, 1.5708, -1.5708, -1.5708, 0.0])

pose = Pose.from_euler([0.4, 0.0, 0.3], [3.14, 0.0, 0.0])
pose.tvec          # (0.4, 0.0, 0.3)
pose.rvec          # (3.14, 0.0, 0.0)
pose.to_list()     # [x, y, z, rx, ry, rz]
```

要偏移一个位姿，需重建一个新位姿（不能原地修改 `tvec`）：

```python
t = list(pose.tvec)
t[2] += 0.05
target = Pose.from_euler(t, pose.rvec)
```

## 运动（Motion）

```python
robot.MoveJ(jp)                       # 关节空间规划运动
robot.MoveP(pose)                     # 笛卡尔点动
robot.MoveL(pose)                     # 笛卡尔直线
robot.MoveC(via, goal)                # 笛卡尔圆弧
robot.MovePath(waypoints)             # 笛卡尔融合路径
robot.ServoJ(jp)                      # 1kHz 关节流式（返回 None）
robot.ServoP(pose)                    # 1kHz 笛卡尔流式（返回 None）
```

`MovePath` 接收 dict 列表：

```python
from rcore_sdk import PathWaypointStop, PathWaypointBlend

waypoints = [
    {"pose": [x, y, z, rx, ry, rz], "mode": PathWaypointBlend, "blend_radius": 0.01},
    {"pose": [x, y, z, rx, ry, rz], "mode": PathWaypointStop,  "blend_radius": 0.0},
]
```

异步运动控制：`robot.GetMotionTaskStatus()`、`robot.PauseMotion()`、
`robot.ContinueMotion()`、`robot.StopMotion()`。

## 配置（Configuration）

```python
robot.GetMaxVelocity()                # list[float]
robot.GetVelocityPercentage()         # list[float]，0-1
robot.SetVelocityPercentage([0.1]*6)  # 关节速度：6 元素列表
robot.SetCartesianVelocityPercentage(0.1)  # 笛卡尔速度：标量
robot.GetCartesianLimits()            # dict

robot.GetPayload()                    # {"mass": float, "com": [x, y, z]}
robot.SetPayload({"mass": 0.5, "com": [0.0, 0.0, 0.04]})
robot.ClearPayload()

robot.GetWaypoints()                  # list[dict]
robot.AddWaypoint({"name": "p1", "joint_positions": [.. 6 ..]})
robot.RemoveWaypoint("p1")
```

## 控制与柔顺（Compliance）

```python
# 笛卡尔阻抗（力矩控制）：流式发送平衡点目标。
robot.EnableCartesianImpedance({"stiffness": [.. 6 ..], "damping": [.. 6 ..]})
robot.SetCartesianImpedanceTarget(pose)   # 类 servo，需持续流式发送
robot.DisableCartesianImpedance()

# 力主导笛卡尔导纳：Enable 不接受参数；需先设置参数。
robot.UpdateFdCartesianAdmittanceParams({"stiffness": [.. 6 ..], "kp": [.. 6 ..]})
robot.EnableFdCartesianAdmittance()
robot.SetFdCartesianAdmittancePoseTarget(pose)
robot.DisableFdCartesianAdmittance()
```

力/力矩传感器（力主导导纳所需）：`robot.EnsureFtSensor()`、
`robot.GetFtCalibration()`、`robot.ReleaseFtSensor()`。

## 运动学（Kinematics）

FK 与 IK 返回 `(Result, value)` 元组，且不会移动机器人：

```python
result, pose = robot.ForwardKinematics(joints)
check(result, "ForwardKinematics")

result, joints = robot.InverseKinematics(pose)
check(result, "InverseKinematics")
```
