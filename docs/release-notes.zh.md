# 版本说明

## 0.3.0

请配套使用 0.3.0 的 C++ SDK、Python wheel 与模拟器。本版提供 Linux 和
Windows 的 C++ 包，以及两个平台的 CPython 3.10–3.12 wheel。

### 工程与规划

- 工程 API 支持浏览、上传、下载、加载、启动及停止已保存工程，也可复核结果
  不确定的启动请求、等待完成、读取执行状态和变量。执行工程时选择
  `ProjectExecution` 连接用途。关闭该连接不会停止已受理的工程。启动回包
  丢失时，先复核或查询原请求，再决定是否重新启动。
- 碰撞配置支持工具 OBJ 网格及受管 `tool://` 引用。自碰撞精度仍默认
  `Capsule`。`MeshRefined` 适用于受支持的 i3/B3 Move 和规划中的臂内自碰撞
  检查。Servo 与遥操作继续使用胶囊体检测。
- 关节路径规划新增净空和 RRT 步长范围控制。`ExecuteMoveJSegment` 执行
  仍有效的规划结果中的指定边。规划调用不会启动运动，缓存结果失效后应重新
  规划。`collision_check_step_rad` 默认值为零，且已不再控制碰撞检查密度。

### 运动结果与飞拍

- 异步结果通过 `WaitFor(timeout_ms)` 限时等待。超时后可继续轮询同一任务。
  读取终态后检查最终结果。已受理的多点 MoveJ 会保留本次任务号，C++ 通过
  `GetTaskId()` 读取，Python 使用 `task_id`。
- 飞拍路径预检返回所请求路径的评估。即使请求本身成功，也应读取评估结果。
  Python 的 `MoveJWaypoints` 现在会转发 `trigger_capture`。
- 未指定六轴 FreeDrive 摩擦等级时，默认值从 70 改为 90。需要沿用旧设置时
  可显式传入原值。

### 末端功能板迁移

- ZX/DH 夹爪调用现在显式传入 Modbus `slave_id`。使用
  `RTU_ReadRegisters_03` 读取保持寄存器，使用 `RTU_ReadRegisters_04` 读取
  输入寄存器。两类夹爪均可读回位置和设定速度、设定力。旧的仅请求读取接口及
  ZX 加速和减速设置接口已移除。
- C++ `RAW485_SendFrame` 须显式传入 `append_crc`，可用 `WithResponses`
  记录本次收发。Python 保留 `RAW485_Transceive`，发送完整帧时
  `RAW485_SendFrame` 的 `append_crc` 默认为 `False`。末端功能板还提供
  缓存的 485 模式状态，以及独立的 RS485 收帧静默时间和首字节等待设置。

## 0.2.0

以下为相对 0.1.2 的累计变化，包含 0.2.0-rc.1 已引入的接口。
请配套使用 0.2.0 的 C++ SDK、Python wheel 与模拟器。

### 运动与规划

- C++ `robot.MoveJ(std::vector<JointWaypoint>)` 和 Python
  `robot.MoveJWaypoints(waypoints)` 支持 1–64 个精确关节位置、速度、加速度
  边界，末点必须停止。位置单位 rad，速度 rad/s，加速度 rad/s²，点前/点后
  最小恒速窗口单位 s。C++ waypoint 的 `trigger_capture` 默认 false，
  与恒速窗口独立；0.2.0 Python 绑定未转发该字段。
- C++ `robot.Planning().PlanJointPath(goal, options)` 和 Python
  `robot.PlanJointPath(goal, options)` 从最新机器人状态规划路径，不执行运动。
  应检查返回状态，执行前重新核实 `scene_generation`。可选逐轴范围单位为 rad，
  与当前软限位取交集；未指定时采用当前软限位。需要规划服务及新鲜的控制器状态。

### 配置与末端板

- `GetMotorParameters()` 读取已加载的 J1–J6 减速比及额定/峰值关节输出力矩
  （N·m）。使用 `axes` 前必须检查 `valid`；这是配置快照，不是实时驱动器反馈。
  两种语言的 `read_state` 示例均演示该检查。
- 配置接口新增关节命令、关节状态、末端力矩传感器与 Rokae 母线电压诊断流开关。
  仅运行时生效，服务启动默认关闭，不持久化。
- `EndBoard()` 新增 `FlyshotSetMode`、能力/时序/状态查询、
  `FlyshotArmAt`/`FlyshotArmAfter` 及取消、清空、复位接口。
  预约脉冲前检查能力与时序。ACK 表示接受预约，执行状态才用于确认完成；
  不能仅根据超时认定远端未执行。

### 负载辨识与兼容注意

- C++ 关节力矩负载辨识新增单轮样本记录便利重载及空载/带载差分重载。
  Python 对应 `IdentifyPayloadFromJointTorqueSampleRecords` 和
  `IdentifyPayloadFromDifferentialJointTorqueSamples`。配对姿态每轴差异不超过
  0.01 rad；两轮保持相同负载配置（推荐每轮前 `ClearPayload()`），两轮之间不要
  重新清零关节力矩。单轮辨识会补偿当前配置负载。
- C++ `Config().GetGravityVector(out)` 能报告失败，失败时不覆盖 `out`。
  Python `GetGravityVectorResult()` 返回 `(Result, value)`。标定前须检查结果；
  旧的仅返回值接口仍保留。

### Python 调用形式

0.2.0 Python 已公开上述新增能力。`MoveJWaypoints` 接收字典列表，字段为
`positions`、`velocities`、`accelerations`、`constant_velocity_before`、
`constant_velocity_after`。0.2.0 Python 绑定未转发 `trigger_capture`，
如需 waypoint 拍摄请使用 C++。`PlanJointPath` 返回字典，包含
`status`、`scene_generation`、`effective_goal_joints`、`waypoints`。
返回状态的 Flyshot 查询/命令使用 `(Result, typed_value)`，应先检查结果再使用值。

稳定版相对 0.2.0-rc.1 未改变 C++ 公开头与 Python 绑定接口。
最终 Python 包扩展为 Linux x86_64、Windows amd64 上的 CPython 3.10、3.11、3.12。
