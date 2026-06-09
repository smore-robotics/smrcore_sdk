# Basics Examples (Python)

## Before Running

Install the wheel first (see [Python SDK](../index.md)). Omit `robot_ip` to run
against the local simulator. `error_recovery.py` deliberately triggers an
e-stop, so keep the workspace clear and the e-stop reachable.

## connect

### What It Does

Minimal connection lifecycle: initialize, check the connection, and shut down.

### When to Use

- First smoke test after installing the wheel.

### Full Source

```python
--8<-- "examples_py/basics/connect.py"
```

## read_state

### What It Does

Reads a state snapshot (joints, TCP pose, control mode) and the motor status.

### When to Use

- Inspect robot state, pose, and motor flags.

### Full Source

```python
--8<-- "examples_py/basics/read_state.py"
```

## linked_sdk

### What It Does

Prints the Python package version and the bundled native SDK info. Does not
connect to a robot.

### When to Use

- Confirm the install and the linked native SDK before reporting a bug.

### Full Source

```python
--8<-- "examples_py/basics/linked_sdk.py"
```

## error_recovery

### What It Does

Runs the full recovery chain: `EStop → Recover → ClearError → Enable`, printing
the motor status after each step.

### When to Use

- Understand the complete recovery sequence after an e-stop or safety stop.

### Full Source

```python
--8<-- "examples_py/basics/error_recovery.py"
```
