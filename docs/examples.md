# Examples

Examples are grouped by topic under `examples/` (C++). Each C++ example is built
to `build/<dir>_<name>` (for example `examples/basics/connect.cpp` builds to
`build/basics_connect`).

> Python examples (`examples_py/`) are being realigned to the same categorized
> layout. A few topics depend on capabilities the current Python wheel does not
> expose yet and are therefore **C++ only** for now (marked below).

## Basics

| Example | C++ | Description |
|---|---|---|
| connect | `examples/basics/connect.cpp` | Initialize, check connection, shutdown |
| read_state | `examples/basics/read_state.cpp` | Read robot state and motor status |
| error_recovery | `examples/basics/error_recovery.cpp` | Recover from e-stop or safety stop: EStop -> Recover -> ClearError -> Enable |

## Motion

| Example | C++ | Description |
|---|---|---|
| movej | `examples/motion/movej.cpp` | Joint-space motion (fixed conservative target) |
| movel | `examples/motion/movel.cpp` | Short Cartesian line motion from current TCP |
| motion_api | `examples/motion/motion_api.cpp` | Use the Motion domain handle + FK/IK |
| move_path | `examples/motion/move_path.cpp` | Blended Cartesian path (stop/blend waypoints) |
| async_motion | `examples/motion/async_motion.cpp` | Async motion + pause/continue + task status |
| servoj | `examples/motion/servoj.cpp` | 1 kHz joint-space servo streaming (ServoJ) |
| servop | `examples/motion/servop.cpp` | 1 kHz Cartesian-space servo streaming (ServoP) |

## Config

| Example | C++ | Description |
|---|---|---|
| config_limits | `examples/config/config_limits.cpp` | Read/modify/restore motion limits |
| waypoints | `examples/config/waypoints.cpp` | Add/list/move-to/remove named waypoints |
| payload | `examples/config/payload.cpp` | Set/read/clear the end-effector payload (C++ only) |

## Compliance

| Example | C++ | Description |
|---|---|---|
| cartesian_impedance | `examples/compliance/cartesian_impedance.cpp` | Cartesian impedance (CST), spring-back (C++ only) |
| fd_cartesian_admittance | `examples/compliance/fd_cartesian_admittance.cpp` | Force-led Cartesian admittance (C++ only) |

## Building and Running (C++)

```bash
./scripts/download.sh
./scripts/build.sh

./build/basics_connect [robot_ip]
./build/basics_read_state [robot_ip]
```

Omit `robot_ip` for local simulation.

## Motion and Force-Control Safety

Before running any motion or compliance example, review the target
poses and parameters in the source file and confirm they are safe for your
robot, tool, payload, and workspace. Compliance examples use torque control;
start with the conservative parameters provided and keep the e-stop reachable.

```bash
./build/motion_movej [robot_ip]
./build/motion_movel [robot_ip]
```
