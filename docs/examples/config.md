# Configuration Examples

## Before Running

Configuration examples **read or temporarily modify** robot configuration and
try to restore the original values on exit. Review the original values printed to
the terminal before continuing. `config_limits` and `payload` exit with a
non-zero code if restore fails, so you can check manually.

## config_limits

### What It Does

Read joint limits and other config, tighten them temporarily, then restore.

### When to Use

- Inspect current limits/velocity config.
- Learn how to safely modify and restore config.

### Full Source

```cpp
--8<-- "examples/config/config_limits.cpp"
```

## waypoints

### What It Does

Add a joint waypoint under a dedicated name (`smrcore_example_demo_home`), list
it, then remove it. Refuses to run if that name already exists, so your data is
never overwritten.

### When to Use

- Manage named waypoints (add/list/remove).

### Full Source

```cpp
--8<-- "examples/config/waypoints.cpp"
```

## payload

### What It Does

Read and print the current end-effector payload, set an example payload, then
restore the original payload on exit (clears it if there was none).

### When to Use

- Set/read the end-effector tool payload.
- Learn how to change payload temporarily without breaking user config.

### Full Source

```cpp
--8<-- "examples/config/payload.cpp"
```
