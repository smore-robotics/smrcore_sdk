# Python API Summary

This page summarises the Python calling style and how it maps to the SDK
concepts documented in the [C++ API Reference](../api/index.md). It is not a
generated, exhaustive reference; for full type details use the C++ reference.

## Version

```python
import rcore_sdk
print(rcore_sdk.__version__)
```

## linked_sdk()

```python
from rcore_sdk import _native

info = _native.linked_sdk()
print(info["version"])     # bundled native SDK version
print(info["linked"])      # True when the native SDK is linked
```

## Robot Lifecycle

```python
from rcore_sdk import Robot

robot = Robot()
robot.Initialize(robot_ip)   # "" for local simulation; returns bool
robot.IsConnected()          # bool
robot.Enable()               # Result
robot.Disable()              # Result
robot.Shutdown()
```

Recovery: `robot.EStop()`, `robot.Recover()`, `robot.ClearError()` each return a
`Result`.

## Results

Synchronous calls return a `Result` that is truthy on success:

```python
result = robot.MoveJ(target)
if not result:
    print(result.error_code, result.error_msg)
```

A small helper keeps examples short:

```python
def check(result, label):
    if not result:
        raise RuntimeError(
            f"{label} failed: code={result.error_code} msg={result.error_msg}"
        )
```

Asynchronous calls (with `asynchronous=True`) return an `AsyncResult`. `Wait()`
blocks until the motion completes and returns a `Result`, so the cleanest pattern
reuses the same `check()` helper:

```python
ar = robot.MoveJ(target, asynchronous=True)
# ... optionally poll GetMotionTaskStatus(), pause/continue, do other work ...
result = ar.Wait()        # blocks until completion; returns a Result
check(result, "MoveJ")
```

Note: `IsSuccess()`, `GetErrorCode()`, and `GetErrorMsg()` each call `Wait()`
internally, so they are also blocking. Prefer the `Result` returned by `Wait()`
rather than calling them in sequence.

## State

```python
state = robot.GetState()        # RobotState
state.positions                 # joint positions (sequence of 6)
state.velocities                # joint velocities
state.torques                   # joint torques
state.tcp_pose                  # Pose
state.cartesian_velocity        # TCP velocity v=J*qd [vx,vy,vz,wx,wy,wz], Base frame
state.timestamp                 # float seconds

robot.GetControlMode()          # int; 0 == Kinematics

motor = robot.GetMotorStatus()  # MotorStatus
motor.enabled, motor.estop, motor.error, motor.operational
```

Robot identity:

```python
info = robot.GetRobotInfo()
info["robot_model"], info["robot_serial_number"], info["sdk_version"]
```

## Pose and Joint Data

`JointPositions` and `Pose` are constructed from sequences of floats. `Pose` is
immutable; `tvec` / `rvec` are read-only properties returning 3-tuples.

```python
from rcore_sdk import JointPositions, Pose

jp = JointPositions([0.0, -1.5708, 1.5708, -1.5708, -1.5708, 0.0])

pose = Pose.from_euler([0.4, 0.0, 0.3], [3.14, 0.0, 0.0])
pose.tvec          # (0.4, 0.0, 0.3)
pose.rvec          # (3.14, 0.0, 0.0)
pose.to_list()     # [x, y, z, rx, ry, rz]
```

To offset a pose, build a new one (you cannot mutate `tvec` in place):

```python
t = list(pose.tvec)
t[2] += 0.05
target = Pose.from_euler(t, pose.rvec)
```

## Motion

```python
robot.MoveJ(jp)                       # joint-space planned motion
robot.MoveRelativeJ(delta)            # joint-space relative motion (delta [rad])
robot.MoveP(pose)                     # Cartesian point
robot.MoveL(pose)                     # Cartesian line
robot.MoveC(via, goal)                # Cartesian arc
robot.MoveCByAngle(via, ref, angle)   # arc by center angle (3-point circle)
robot.MovePath(waypoints)             # blended Cartesian path
robot.ServoJ(jp)                      # 1 kHz joint streaming (returns None)
robot.ServoP(pose)                    # 1 kHz Cartesian streaming (returns None)
```

`MoveJ` / `MoveP` / `MoveL` / `MoveC` / `MovePath` / `MoveJWaypoint` accept an
optional `velocity_scale` keyword: `-1` (default) uses the controller's global velocity,
a value in `(0, 1]` is multiplied onto the global velocity for that single
motion only:

```python
robot.MoveJ(jp, velocity_scale=0.5)   # run this move at half the global velocity
```

`MovePath` takes a list of dicts:

```python
from rcore_sdk import PathWaypointStop, PathWaypointBlend

waypoints = [
    {"pose": [x, y, z, rx, ry, rz], "mode": PathWaypointBlend, "blend_radius": 0.01},
    {"pose": [x, y, z, rx, ry, rz], "mode": PathWaypointStop,  "blend_radius": 0.0},
]
```

Async motion control: `robot.GetMotionTaskStatus()`, `robot.PauseMotion()`,
`robot.ContinueMotion()`, `robot.StopMotion()`.

## Configuration

```python
robot.GetMaxVelocity()                # list[float]
robot.GetVelocityPercentage()         # list[float], 0-1
robot.SetVelocityPercentage([0.1]*6)  # joint velocity: 6-element list
robot.SetCartesianVelocityPercentage(0.1)  # Cartesian velocity: scalar
robot.GetCartesianLimits()            # dict

robot.GetPayload()                    # {"mass": float, "com": [x, y, z]}
robot.SetPayload({"mass": 0.5, "com": [0.0, 0.0, 0.04]})
robot.ClearPayload()

robot.GetWaypoints()                  # list[dict]
robot.AddWaypoint({"name": "p1", "joint_positions": [.. 6 ..]})
robot.RemoveWaypoint("p1")
```

Soft limits and safety switches. `Set*` only changes the values; protection is
toggled separately with `Enable*` / `Disable*`:

```python
robot.GetSoftPositionLimit()          # dict: per-joint min/max [rad]
robot.SetSoftPositionLimit(limit)     # Result
robot.EnableSoftPositionLimit()       # Result; DisableSoftPositionLimit() to stop

robot.GetSoftJointTorqueLimit()       # dict: per-joint torque base + percentage
robot.SetSoftJointTorqueLimit(limit)  # Result
robot.EnableSoftJointTorqueLimit()    # Result; DisableSoftJointTorqueLimit() to stop

robot.EnableSelfCollision()           # Result; DisableSelfCollision() to stop
robot.EnableVirtualWall()             # Result; DisableVirtualWall() to stop
```

## Frames

Tool and workpiece/reference frame query and switching. These only change the
current selection; frame definitions are maintained by robot configuration or
calibration:

```python
robot.GetToolsInfo()                  # list[dict]; "is_active" marks the current tool
robot.GetWorkpieceFramesInfo()        # list[dict]; candidates for reference frame
robot.GetToolInfo("flange")           # dict | None; single tool frame by name
robot.GetWorkpieceFrameInfo("base")   # dict | None; single workpiece frame by name
robot.GetCurrentTool()                # str
robot.GetCurrentReference()           # str
robot.SetCurrentTool("flange")        # Result
robot.SetCurrentReference("base")     # Result
```

## Control and Compliance

```python
# Cartesian impedance (torque control): stream the equilibrium target.
robot.EnableCartesianImpedance({"stiffness": [.. 6 ..], "damping": [.. 6 ..]})
robot.SetCartesianImpedanceTarget(pose)   # servo-like, stream continuously
robot.DisableCartesianImpedance()

# Force-led Cartesian admittance: Enable takes no arguments; set params first.
robot.UpdateFdCartesianAdmittanceParams({"stiffness": [.. 6 ..], "kp": [.. 6 ..]})
robot.EnableFdCartesianAdmittance()
robot.SetFdCartesianAdmittancePoseTarget(pose)
robot.DisableFdCartesianAdmittance()
```

Force/torque sensor (required by force-led admittance):
`robot.EnsureFtSensor()`, `robot.GetFtCalibration()`, `robot.ReleaseFtSensor()`.
`robot.CalibrateEndTorqueSensorZero()` re-zeroes the end torque sensor; run it
with no external load on the tool.

## Kinematics

FK and IK return a `(Result, value)` tuple and do not move the robot:

```python
result, pose = robot.ForwardKinematics(joints)
check(result, "ForwardKinematics")

result, joints = robot.InverseKinematics(pose)
check(result, "InverseKinematics")
```

Jacobian and reachability return a dict carrying the `Result` fields
(`success`, `error_code`, `error_msg`) plus the value:

```python
jac = robot.GetJacobian()
if jac["success"]:
    jac["jacobian"]                   # 6x6 list[list[float]], Base frame;
                                      # rows 0-2 linear v, rows 3-5 angular w

reach = robot.IsReachable(pose)
if reach["success"]:
    reach["reachable"]                # bool
```
