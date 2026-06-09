# Compliance Examples (Python)

## Before Running

Compliance examples use torque/force control. Start with the conservative
parameters provided, keep the workspace clear, and keep the e-stop reachable.
`fd_cartesian_admittance.py` additionally requires a six-axis force/torque sensor
and a saved, active FT-sensor calibration.

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

Force-led Cartesian admittance: the TCP is driven by a measured six-axis force
while tracking a commanded pose. It ensures the FT sensor, validates the
calibration, sets conservative parameters, enables the mode, then commands a
+5 cm Z target and back. `EnableFdCartesianAdmittance()` takes no arguments;
parameters are set via `UpdateFdCartesianAdmittanceParams`.

### When to Use

- Try force-led Cartesian admittance with an F/T sensor.

### Full Source

```python
--8<-- "examples_py/compliance/fd_cartesian_admittance.py"
```
