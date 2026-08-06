# Compliance Examples (Python)

## Before Running

Compliance examples use torque/force control. Start with the conservative
parameters provided, keep the workspace clear, and keep the e-stop reachable.
The default `fd_cartesian_admittance.py` path uses joint-torque estimated wrench
and does **not** require peripherals. External F/T and SpaceMouse paths require
[smrcore_peripherals](https://github.com/smore-robotics/smrcore_peripherals).

## cartesian_impedance

### What It Does

Cartesian impedance (torque) control. The TCP behaves like a spring-damper around
an equilibrium pose; the equilibrium is streamed +5 cm along Z and back, then the
mode is disabled. Parameters are a dict with `stiffness` and `damping`.

### When to Use

- Try conservative Cartesian impedance behaviour.

### Full Source

```python
--8<-- "examples_py/compliance/cartesian_impedance.py"
```

## fd_cartesian_admittance

### What It Does

Force-led Cartesian admittance (FDCC). Defaults:

- `--wrench-source joint_torque_estimated` (no peripherals)
- `--mode pose` (+5 cm Z target demo)

`EnableFdCartesianAdmittance({...})` takes the params dict (stiffness / kp /
…). Select the wrench source with `SetFdCartesianAdmittanceWrenchSource`
**before** enable.

Advanced:

- `--wrench-source ft-sensor`: **must** calibrate once in
  [smrcore_peripherals](https://github.com/smore-robotics/smrcore_peripherals)
  (`app_peripherals_ft_sensor_calib --save`), then keep
  `app_peripherals_bridge --ft-sensor` streaming samples. Uncalibrated external
  wrench is dangerous.
- `--mode spacemouse`: requires bridge `--spacemouse` (or default dual
  peripherals) to inject SpaceMouse samples.

Edit stiffness / kp in the source to tune; there is no gain CLI.

### When to Use

- Try FDCC without peripherals first; add external F/T or SpaceMouse when needed.

### Full Source

```python
--8<-- "examples_py/compliance/fd_cartesian_admittance.py"
```
