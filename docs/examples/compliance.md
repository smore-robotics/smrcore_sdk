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

Force-driven Cartesian admittance control: read the current TCP, enter
admittance mode with the example parameters, send a small offset target, then
exit.

### When to Use

- Hand-guiding / assembly tasks that should yield to external force.

> Prerequisite: a mounted and calibrated 6-axis force/torque sensor.

### Full Source

```cpp
--8<-- "examples/compliance/fd_cartesian_admittance.cpp"
```
