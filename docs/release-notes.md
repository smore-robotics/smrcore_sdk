# Release Notes

## 0.3.0

Use the matching 0.3.0 C++ SDK, Python wheel, and simulator. The release
includes Linux and Windows C++ packages and CPython 3.10–3.12 wheels for both
platforms.

### Projects and planning

- The Projects API lists, uploads, downloads, loads, starts, and stops saved
  projects. It can also resolve an uncertain start, wait for completion, and
  read execution state and variable values. For project execution, select
  the `ProjectExecution` connection purpose. Closing that connection does
  not stop an accepted project. If a start response is lost, resolve or query
  the original request before attempting another start.
- Collision configuration supports tool OBJ meshes and managed `tool://`
  references. `Capsule` remains the default self-collision precision.
  `MeshRefined` applies to arm self-collision for supported i3/B3 Move and
  planning paths. Servo and teleoperation continue to use capsule checks.
- Joint path planning adds clearance and RRT range controls.
  `ExecuteMoveJSegment` executes a segment from a still-valid planning
  result. A planning call does not start motion. Replan if the cached result
  is no longer valid. `collision_check_step_rad` defaults to zero and no
  longer controls collision-check density.

### Motion results and flyshot

- `WaitFor(timeout_ms)` allows bounded waiting on an asynchronous result.
  A timeout leaves the original task available for another poll. Check the
  terminal result for success. Accepted multi-waypoint MoveJ calls expose
  their own task ID in C++ (`GetTaskId()`) and Python (`task_id`).
- Flyshot path prechecks return an assessment for the requested path.
  Read the assessment even when the request itself succeeds. Python
  `MoveJWaypoints` now forwards `trigger_capture`.
- The default six-axis FreeDrive friction level changes from 70 to 90
  when the caller omits it. Pass an explicit value to retain prior tuning.

### End Board migration

- ZX/DH gripper calls now take an explicit Modbus `slave_id`. Use
  `RTU_ReadRegisters_03` for holding registers and `RTU_ReadRegisters_04`
  for input registers. ZX and DH helpers read back position and the configured
  speed and force. The previous request-only calls and ZX acceleration and
  deceleration setters were removed.
- C++ `RAW485_SendFrame` requires an explicit `append_crc` argument. Use
  `WithResponses` to record the transaction's transmit and receive data.
  Python retains `RAW485_Transceive` and defaults `RAW485_SendFrame` to
  `append_crc=False` for a complete frame. The End Board APIs also expose
  cached 485 mode state and separate RS485 frame-silence and first-byte
  timing settings.

## 0.2.0

These changes are cumulative since 0.1.2 and include the APIs introduced in
0.2.0-rc.1. Use matching 0.2.0 C++ SDK, Python wheel, and simulator packages.

### Motion and planning

- C++ `robot.MoveJ(std::vector<JointWaypoint>)` and Python
  `robot.MoveJWaypoints(waypoints)` accept 1–64 exact joint position, velocity,
  and acceleration boundaries. The last waypoint must stop. Positions use
  radians, velocities rad/s, accelerations rad/s²; the before/after constant
  velocity windows use seconds. The C++ waypoint's `trigger_capture` defaults
  to false and is independent of these windows; the 0.2.0 Python binding does
  not forward this field.
- C++ `robot.Planning().PlanJointPath(goal, options)` and Python
  `robot.PlanJointPath(goal, options)` plan from the latest robot state without
  executing motion. Check the returned status and revalidate `scene_generation`
  before execution. Optional per-axis limits use radians and intersect the
  current soft limits; omitted limits use the current soft limits. Planning
  requires the planning service and a fresh controller state.

### Configuration and end board

- `GetMotorParameters()` returns the loaded J1–J6 gear ratios and rated/peak
  joint output torques in N·m. Check `valid` before using `axes`; this is a
  configuration snapshot, not live drive feedback. Both `read_state` examples
  demonstrate the check.
- Configuration APIs add runtime switches for joint-command, joint-state,
  end torque sensor, and Rokae DC bus voltage diagnostic streams. They default
  to off on service startup and are not persistent configuration.
- `EndBoard()` adds `FlyshotSetMode`, capability/timing/state queries,
  `FlyshotArmAt`/`FlyshotArmAfter`, cancel, clear, and reset. Check capabilities
  and timing before scheduling pulses. An ACK confirms acceptance; use execution
  state to establish completion. A timeout alone does not establish the outcome.

### Payload identification and compatibility

- C++ joint-torque payload identification adds a sample-record convenience
  overload and an empty/loaded differential overload. Python exposes
  `IdentifyPayloadFromJointTorqueSampleRecords` and
  `IdentifyPayloadFromDifferentialJointTorqueSamples`. Pair poses within
  0.01 rad per joint, keep the same payload configuration across both rounds
  (prefer `ClearPayload()` before each), and do not re-zero joint torque between
  rounds. Single-round identification compensates for the configured payload.
- C++ `Config().GetGravityVector(out)` reports failure without replacing `out`.
  Python `GetGravityVectorResult()` returns `(Result, value)`. Check the result
  before calibration; the older value-only calls remain available.

### Python calling style

Python exposes the additions in 0.2.0. `MoveJWaypoints` accepts dictionaries
with `positions`, `velocities`, `accelerations`,
`constant_velocity_before` and `constant_velocity_after`. The 0.2.0 Python
binding does not forward `trigger_capture`; use C++ for waypoint capture.
`PlanJointPath` returns a dictionary containing `status`, `scene_generation`,
`effective_goal_joints`, and `waypoints`. Flyshot read/command calls that return
state use `(Result, typed_value)`; check the result before consuming the value.

The stable release does not change the C++ public headers or Python binding
interfaces relative to 0.2.0-rc.1. The final Python packages expand distribution
to CPython 3.10, 3.11, and 3.12 on Linux x86_64 and Windows amd64.
