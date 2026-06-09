# Examples

Examples are grouped by topic under `examples/` (C++). Each one builds to
`build/<dir>_<name>` (for example `basics/connect.cpp` → `build/basics_connect`).
Omit `robot_ip` to run against the local simulator.

Each example's source file starts with a header comment that is the full
reference (usage, behaviour, safety). The tables below summarise, per example,
what it shows, the main APIs, its prerequisites, and when to use it.

> Looking for Python? See the [Python Examples](../python/examples/index.md).

## Recommended Path

If you are new to the SDK, read and run the examples in this order:

1. `basics/connect.cpp` — verify the SDK can connect.
2. `basics/read_state.cpp` — inspect robot state and motor status.
3. `motion/movej.cpp` — run the simplest joint-space planned motion.
4. `motion/movep.cpp` or `motion/movel.cpp` — try a small Cartesian move.
5. `motion/servoj.cpp` or `motion/servop.cpp` — inspect high-frequency servo
   streaming.
6. `config/config_limits.cpp`, `config/waypoints.cpp`, and `config/payload.cpp`
   — learn the common runtime configuration APIs.
7. Continue with the remaining motion and compliance examples when the basic
   workflow is clear.

## Basics

See the full source walkthrough: [Basics Examples](basics.md)

| Source | Shows | Main APIs | Prerequisites | When to use |
|---|---|---|---|---|
| `basics/connect.cpp` | Minimal lifecycle: connect, check, disconnect | `Initialize` / `IsConnected` / `Shutdown` | none | First smoke test after installing the SDK |
| `basics/read_state.cpp` | Read one full state snapshot + motor status | `GetState` / `GetMotorStatus` | none | Check robot state, pose, errors, and motor flags |
| `basics/error_recovery.cpp` | Recovery chain `EStop → Recover → ClearError → Enable` | `Enable` / `EStop` / `Recover` / `ClearError` | none (it triggers an e-stop) | Understand the full recovery sequence |

## Motion

All motion examples enable the motors themselves; keep the workspace clear and
the e-stop reachable. Cartesian examples move a few cm from the current TCP.

See the full source walkthrough: [Motion Examples](motion.md)

| Source | Shows | Main APIs | Prerequisites | When to use |
|---|---|---|---|---|
| `motion/movej.cpp` | Joint-space move to a fixed conservative target | `MoveJ(JointPositions)` | — | First planned motion example |
| `motion/movep.cpp` | Cartesian point move: current pose +5 cm (Z) | `MoveP(Pose)` / `Pose::FromEuler` | — | Move to a Cartesian target pose |
| `motion/movel.cpp` | Cartesian line move +5 cm (Y) | `MoveL(Pose)` | — | Execute a straight Cartesian segment |
| `motion/movec.cpp` | Cartesian arc through a via pose to a goal | `MoveC(via, goal)` | — | Execute a small circular arc |
| `motion/motion_api.cpp` | Motion domain handle + FK/IK | `Motion()` / `ForwardKinematics` / `InverseKinematics` / `MoveP` | — | Use the domain API directly and inspect FK/IK |
| `motion/move_path.cpp` | Blended Cartesian path (stop/blend waypoints) | `MovePath(vector<CartesianWaypoint>)` | — | Run multiple Cartesian waypoints as one path |
| `motion/async_motion.cpp` | Async move + pause/continue + status polling | `MoveJ(async)` / `GetMotionTaskStatus` / `Pause`/`ContinueMotion` | — | Monitor and control a running motion task |
| `motion/servoj.cpp` | 1 kHz joint-space servo streaming | `ServoJ(JointPositions)` | — | Stream smooth joint targets at high frequency |
| `motion/servop.cpp` | 1 kHz Cartesian-space servo streaming | `ServoP(Pose)` | `cartesian_valid == true` | Stream smooth Cartesian targets at high frequency |

## Config

See the full source walkthrough: [Configuration Examples](config.md)

| Source | Shows | Main APIs | Prerequisites | When to use |
|---|---|---|---|---|
| `config/config_limits.cpp` | Read/modify/verify/restore motion limits (restores originals) | `Get`/`SetVelocityPercentage` / `GetMaxVelocity` / `GetCartesianLimits` | none | Inspect and adjust runtime motion limits |
| `config/waypoints.cpp` | Add/list/remove named waypoints (namespaced name, refuses to overwrite) | `GetWaypoints` / `AddWaypoint` / `RemoveWaypoint` | none | Manage named joint poses without moving |
| `config/payload.cpp` | Set/read/clear end-effector payload (restores original on exit) | `GetPayload` / `SetPayload` / `ClearPayload` | none | Set payload parameters for dynamics-aware features |

## Compliance

Compliance examples use torque/force control. Start with the conservative
parameters provided and keep the e-stop reachable.

See the full source walkthrough: [Compliance Examples](compliance.md)

| Source | Shows | Main APIs | Prerequisites | When to use |
|---|---|---|---|---|
| `compliance/cartesian_impedance.cpp` | Cartesian impedance (CST); equilibrium streamed servo-style +5 cm in Z and back | `EnableCartesianImpedance` / `SetCartesianImpedanceTarget` (streamed) / `DisableCartesianImpedance` | — | Try conservative Cartesian impedance behaviour |
| `compliance/fd_cartesian_admittance.cpp` | Force-led Cartesian admittance (driven by measured wrench) | `EnsureFtSensor` / `Enable`·`SetPoseTarget`·`Disable` `FdCartesianAdmittance` | six-axis F/T sensor installed + saved FT calibration | Try force-led Cartesian admittance with an F/T sensor |

## Building and Running

```bash
./scripts/download.sh
./scripts/build.sh

./build/basics_connect [robot_ip]
./build/basics_read_state [robot_ip]
```

Omit `robot_ip` for local simulation.

## Motion and Force-Control Safety

!!! warning "Safety"
    Before running any motion or compliance example, review the target poses and
    parameters in the source file and confirm they are safe for your robot, tool,
    payload, and workspace. Compliance examples use torque control; start with the
    conservative parameters provided and keep the e-stop reachable.
