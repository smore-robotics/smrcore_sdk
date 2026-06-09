# Motion Examples

## Before Running

Motion examples enable the motors and physically move the robot. Keep the
workspace clear and the e-stop within reach. Cartesian examples move a few
centimeters from the current TCP pose. The comment header in each source file
is the full guide.

## movej

### What It Does

Joint-space move to a fixed, conservative joint target.

### When to Use

- Return to a known posture (e.g. home).
- Point-to-point motion in joint space.

### Full Source

```cpp
--8<-- "examples/motion/movej.cpp"
```

## movep

### What It Does

Cartesian point-to-point: copy the current TCP pose, offset +5 cm along Z, and
move there with the planner.

### When to Use

- Move the TCP to a Cartesian target pose.
- Apply a small offset from the current pose.

### Full Source

```cpp
--8<-- "examples/motion/movep.cpp"
```

## movel

### What It Does

A short Cartesian straight-line move (+5 cm along Y) from the current TCP.

### When to Use

- Tasks that need a linear path (mating, insertion).

### Full Source

```cpp
--8<-- "examples/motion/movel.cpp"
```

## movec

### What It Does

A short circular arc from the current TCP through a via point to a goal point.

### When to Use

- Tasks that need an arc / go-around path.

### Full Source

```cpp
--8<-- "examples/motion/movec.cpp"
```

## move_path

### What It Does

A blended Cartesian path made of multiple waypoints, with per-point stop/blend
mode and blend radius.

### When to Use

- Continuous multi-segment trajectories with smooth corners.

### Full Source

```cpp
--8<-- "examples/motion/move_path.cpp"
```

## async_motion

### What It Does

Start a non-blocking motion, poll task status, and demonstrate pause/resume.

### When to Use

- Do other work or monitor progress while moving.
- Pause/resume/stop on demand.

### Full Source

```cpp
--8<-- "examples/motion/async_motion.cpp"
```

## motion_api

### What It Does

Use the Motion domain handle (`robot.Motion()`) with forward/inverse kinematics,
then move through the handle.

### When to Use

- Need FK/IK solving.
- Prefer domain-handle calls over shortcuts.

### Full Source

```cpp
--8<-- "examples/motion/motion_api.cpp"
```

## servoj

### What It Does

1 kHz joint-space servoing: no planner, you stream targets yourself; the example
gently rocks joint 1.

### When to Use

- An external trajectory/controller drives joints at high rate.

### Full Source

```cpp
--8<-- "examples/motion/servoj.cpp"
```

## servop

### What It Does

1 kHz Cartesian-space servoing (the dual of ServoJ); the example oscillates the
TCP ~1 cm along Z around the current pose.

### When to Use

- An external trajectory/controller drives the TCP pose at high rate.

### Full Source

```cpp
--8<-- "examples/motion/servop.cpp"
```
