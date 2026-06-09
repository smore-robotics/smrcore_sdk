# Motion Examples (Python)

## Before Running

Motion examples enable the motors and move the robot. Keep the workspace clear
and the e-stop reachable. Cartesian examples move a few cm from the current TCP
pose. Each example's docstring is the full reference.

`servoj.py` / `servop.py` stream targets in a Python loop. Python is not a hard
real-time language; for strict 1 kHz timing on real hardware prefer the C++ SDK.

## movej

### What It Does

Joint-space planned motion to a fixed conservative joint target.

### When to Use

- Return to a known pose (such as home).
- Joint-space point-to-point motion.

### Full Source

```python
--8<-- "examples_py/motion/movej.py"
```

## movep

### What It Does

Cartesian point motion: reads the current TCP pose and moves +5 cm along Z.

### When to Use

- Move the TCP to a Cartesian target pose.

### Full Source

```python
--8<-- "examples_py/motion/movep.py"
```

## movel

### What It Does

Straight-line Cartesian motion +5 cm along Y from the current TCP.

### When to Use

- Linear trajectories (such as approach or insertion).

### Full Source

```python
--8<-- "examples_py/motion/movel.py"
```

## movec

### What It Does

Cartesian arc from the current TCP through a via pose to a goal pose.

### When to Use

- Circular or arcing trajectories.

### Full Source

```python
--8<-- "examples_py/motion/movec.py"
```

## move_path

### What It Does

Blended Cartesian path built from multiple waypoint dicts, each with a stop/blend
mode and blend radius.

### When to Use

- Multi-segment trajectories with smooth corner blending.

### Full Source

```python
--8<-- "examples_py/motion/move_path.py"
```

## async_motion

### What It Does

Starts a non-blocking motion, polls the task status, and demonstrates
pause / continue.

### When to Use

- Do other work or monitor progress while moving.
- Pause / continue / stop on demand.

### Full Source

```python
--8<-- "examples_py/motion/async_motion.py"
```

## kinematics

### What It Does

Computes forward and inverse kinematics directly. Does not move the robot.

### When to Use

- Solve FK/IK without commanding motion.

### Full Source

```python
--8<-- "examples_py/motion/kinematics.py"
```

## servoj

### What It Does

Joint-space servo streaming: no planner, you stream targets. The example
oscillates joint 1 by a few degrees.

### When to Use

- An external trajectory or controller drives the joints at high frequency.

### Full Source

```python
--8<-- "examples_py/motion/servoj.py"
```

## servop

### What It Does

Cartesian-space servo streaming (the counterpart of ServoJ). The example
oscillates the TCP ~1 cm along Z around the current pose.

### When to Use

- An external trajectory or controller drives the TCP pose at high frequency.

### Full Source

```python
--8<-- "examples_py/motion/servop.py"
```
