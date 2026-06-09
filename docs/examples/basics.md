# Basics Examples

## Before Running

Basics examples do not move the robot and can run against the local simulator
(omit `robot_ip`). The comment header in each source file is the full guide.

## connect

### What It Does

Minimal lifecycle: connect to the robot, check the connection, disconnect.

### When to Use

- Verify the SDK install and connectivity.
- Use as the starting template for any program.

### Full Source

```cpp
--8<-- "examples/basics/connect.cpp"
```

## read_state

### What It Does

Read one full robot state snapshot plus motor status.

### When to Use

- Inspect joint/Cartesian state and estimated TCP wrench while debugging.
- Confirm the active control mode and latched error count.

### Full Source

```cpp
--8<-- "examples/basics/read_state.cpp"
```

## error_recovery

### What It Does

Demonstrates the full recovery chain after an e-stop / safety stop:
`EStop → Recover → ClearError → Enable` (hardware side, controller side, then
re-enable — all three are required).

### When to Use

- Recover after an e-stop, safety stop, or collision trigger.
- Understand the role of each step.

> Note: this example triggers an e-stop. Keep the workspace clear and the
> e-stop within reach.

### Full Source

```cpp
--8<-- "examples/basics/error_recovery.cpp"
```
