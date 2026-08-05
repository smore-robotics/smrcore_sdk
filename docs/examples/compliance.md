# Compliance Examples

## Before Running

Compliance examples enter force/torque control and physically move the robot.
Start with conservative parameters, keep the workspace clear, and keep the
e-stop within reach. Get familiar with the behavior in simulation first.

## cartesian_impedance

### What It Does

Enter Cartesian impedance mode, move the equilibrium +5 cm along Z and back,
then exit. The robot behaves like a spring around the equilibrium pose.

### When to Use

- Contact tasks that need compliant behavior with tunable stiffness/damping.

### Full Source

```cpp
--8<-- "examples/compliance/cartesian_impedance.cpp"
```

## fd_cartesian_admittance

### What It Does

Force-driven Cartesian admittance (FDCC). By default it uses the
**joint-torque estimated** wrench source and runs a small +5 cm Z pose demo —
no peripherals repository is required.

Optional advanced paths:

| Path | How | Requires [smrcore_peripherals](https://github.com/smore-robotics/smrcore_peripherals)? |
|---|---|---|
| Basic (default) | `--wrench-source joint_torque_estimated` (default) + `--mode pose` | No |
| External F/T | One-time calib `--save`, then bridge `--ft-sensor`, then `--wrench-source ft_sensor` | Yes |
| SpaceMouse teleop | Bridge `--spacemouse` (or default dual peripherals), then `--mode spacemouse` | Yes |

**External F/T safety:** you **must** complete a one-time static calibration in
`smrcore_peripherals` before using `--wrench-source ft_sensor`. An uncalibrated
external wrench is dangerous and can cause large unintended motion. After
`--save`, keep the bridge running to stream samples into the controller, then
start this example.

```bash
# One-time calibration
app_peripherals_bridge --robot <ip> --ft-sensor
app_peripherals_ft_sensor_calib --robot-ip <ip> --save

# Daily: stream samples, then run FDCC
app_peripherals_bridge --robot <ip> --ft-sensor
./build/compliance_fd_cartesian_admittance <ip> --wrench-source ft_sensor
```

Stiffness / kp are conservative constants in the source file — edit them there
to tune (no CLI for gains).

### When to Use

- Hand-guiding / assembly tasks that should yield to external force.
- Start with the default joint-torque path; add external F/T or SpaceMouse when
  you need higher-fidelity wrench or teleop.

### Full Source

```cpp
--8<-- "examples/compliance/fd_cartesian_admittance.cpp"
```
