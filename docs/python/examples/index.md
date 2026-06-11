# Python Examples

Python examples are grouped by topic under `examples_py/`, mirroring the C++
`examples/` layout. Run any example with:

```bash
python examples_py/basics/connect.py [robot_ip]
```

Omit `robot_ip` to run against the local simulator.

Each example's source file starts with a docstring that is the full reference
(usage, behaviour, safety). The tables below summarise, per example, what it
shows, the main APIs, its prerequisites, and when to use it.

These examples use the real Python binding style (flat methods, truthy `Result`,
immutable `Pose`). See the [Python API Summary](../api.md) for the conventions.

## Recommended Path

If you are new to the SDK, read and run the examples in this order:

1. `basics/connect.py` — verify the SDK can connect.
2. `basics/read_state.py` — inspect robot state and motor status.
3. `motion/movej.py` — run the simplest joint-space planned motion.
4. `motion/movep.py` or `motion/movel.py` — try a small Cartesian move.
5. `motion/servoj.py` or `motion/servop.py` — inspect high-frequency servo
   streaming.
6. `config/config_limits.py`, `config/waypoints.py`, and `config/payload.py`
   — learn the common runtime configuration APIs.
7. Continue with the remaining motion and compliance examples when the basic
   workflow is clear.

## Basics

See the full source walkthrough: [Basics Examples](basics.md)

| Source | Shows | Main APIs | Prerequisites | When to use |
|---|---|---|---|---|
| `basics/connect.py` | Minimal lifecycle: connect, check, disconnect | `Initialize` / `IsConnected` / `Shutdown` | none | First smoke test after installing the wheel |
| `basics/read_state.py` | Read robot info + state snapshot + motor status | `GetRobotInfo` / `GetState` / `GetMotorStatus` / `GetControlMode` | none | Check robot identity, state, pose, and motor flags |
| `basics/linked_sdk.py` | Report package and linked native SDK versions | `rcore_sdk.__version__` / `_native.linked_sdk` | none | Confirm the install before reporting a bug |
| `basics/error_recovery.py` | Recovery chain `EStop → Recover → ClearError → Enable` | `Enable` / `EStop` / `Recover` / `ClearError` | none (it triggers an e-stop) | Understand the full recovery sequence |

## Motion

All motion examples enable the motors themselves; keep the workspace clear and
the e-stop reachable. Cartesian examples move a few cm from the current TCP.

See the full source walkthrough: [Motion Examples](motion.md)

| Source | Shows | Main APIs | Prerequisites | When to use |
|---|---|---|---|---|
| `motion/movej.py` | Joint-space move to a fixed conservative target (with per-move `velocity_scale`) | `MoveJ(JointPositions, velocity_scale=...)` | — | First planned motion example |
| `motion/movep.py` | Cartesian point move: current pose +5 cm (Z) | `MoveP(Pose)` / `Pose.from_euler` | — | Move to a Cartesian target pose |
| `motion/movel.py` | Cartesian line move +5 cm (Y) | `MoveL(Pose)` | — | Execute a straight Cartesian segment |
| `motion/movec.py` | Cartesian arc through a via pose to a goal | `MoveC(via, goal)` | — | Execute a small circular arc |
| `motion/move_path.py` | Blended Cartesian path (stop/blend waypoints) | `MovePath(list[dict])` | — | Run multiple Cartesian waypoints as one path |
| `motion/async_motion.py` | Async move + pause/continue + status polling | `MoveJ(asynchronous=True)` / `GetMotionTaskStatus` | — | Monitor and control a running motion task |
| `motion/kinematics.py` | FK / IK + Jacobian + reachability check (no motion) | `ForwardKinematics` / `InverseKinematics` / `GetJacobian` / `IsReachable` | — | Kinematics queries without moving |
| `motion/servoj.py` | Joint-space servo streaming | `ServoJ(JointPositions)` | — | Stream smooth joint targets |
| `motion/servop.py` | Cartesian-space servo streaming | `ServoP(Pose)` | — | Stream smooth Cartesian targets |

## Configuration

See the full source walkthrough: [Configuration Examples](config.md)

| Source | Shows | Main APIs | Prerequisites | When to use |
|---|---|---|---|---|
| `config/config_limits.py` | Read/modify/verify/restore motion limits | `Get`/`SetVelocityPercentage` / `GetMaxVelocity` / `GetCartesianLimits` | none | Inspect and adjust runtime motion limits |
| `config/waypoints.py` | Add/list/remove named waypoints (refuses to overwrite) | `GetWaypoints` / `AddWaypoint` / `RemoveWaypoint` | none | Manage named joint poses without moving |
| `config/payload.py` | Set/read/restore end-effector payload | `GetPayload` / `SetPayload` / `ClearPayload` | none | Set payload parameters for dynamics-aware features |

## Compliance

Compliance examples use torque/force control. Start with the conservative
parameters provided and keep the e-stop reachable.

See the full source walkthrough: [Compliance Examples](compliance.md)

| Source | Shows | Main APIs | Prerequisites | When to use |
|---|---|---|---|---|
| `compliance/cartesian_impedance.py` | Cartesian impedance; equilibrium streamed +5 cm in Z and back | `EnableCartesianImpedance` / `SetCartesianImpedanceTarget` / `DisableCartesianImpedance` | — | Try conservative Cartesian impedance behaviour |
| `compliance/fd_cartesian_admittance.py` | Force-led Cartesian admittance (driven by measured wrench) | `EnsureFtSensor` / `UpdateFdCartesianAdmittanceParams` / `EnableFdCartesianAdmittance` | six-axis F/T sensor + saved FT calibration | Try force-led Cartesian admittance with an F/T sensor |

## Safety

!!! warning "Safety"
    Before running any motion or compliance example, review the target poses and
    parameters in the source file and confirm they are safe for your robot, tool,
    payload, and workspace. Compliance examples use torque control; start with the
    conservative parameters provided and keep the e-stop reachable.
