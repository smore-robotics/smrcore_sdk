# Configuration Examples (Python)

## Before Running

These examples read or temporarily modify robot configuration. They restore the
original values (or refuse to overwrite your data) before exiting, but you should
still review the printed output before continuing.

## config_limits

### What It Does

Reads the current joint velocity limits, applies a small reduction, reads back to
verify, then restores the originals.

### When to Use

- Inspect and adjust runtime motion limits.

### Full Source

```python
--8<-- "examples_py/config/config_limits.py"
```

## waypoints

### What It Does

Adds a namespaced demo waypoint built from the current joint pose, lists all
waypoints, then removes only the entry it created. It refuses to run if the demo
name already exists.

### When to Use

- Manage named joint poses without moving the robot.

### Full Source

```python
--8<-- "examples_py/config/waypoints.py"
```

## payload

### What It Does

Saves the active payload, applies a small demo payload, then restores the
original value before exiting. The payload dict is
`{"mass": float, "com": [x, y, z]}`.

### When to Use

- Set payload parameters for dynamics-aware features.

### Full Source

```python
--8<-- "examples_py/config/payload.py"
```
