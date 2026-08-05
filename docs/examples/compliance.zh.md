# 柔顺控制示例

## 运行前

柔顺控制示例会进入力/力矩控制模式并真实移动机器人。请从保守参数起步、清空工作区、
保证急停可触达。建议先在仿真中熟悉行为再上真机。

## cartesian_impedance

### 作用

进入笛卡尔阻抗模式，将平衡点沿 Z 移动 +5cm 再移回，最后退出阻抗模式。机器人在
平衡点附近表现得像弹簧。

### 适用场景

- 接触类任务中需要可调刚度/阻尼的柔顺行为。

### 完整源码

```cpp
--8<-- "examples/compliance/cartesian_impedance.cpp"
```

## fd_cartesian_admittance

### 作用

力主导笛卡尔导纳（FDCC）。默认使用 **关节力矩估算** 力源，并运行 +5 cm Z
位姿演示——**不需要**外设仓库。

可选进阶路径：

| 路径 | 用法 | 是否需要 [smrcore_peripherals](https://github.com/smore-robotics/smrcore_peripherals) |
|---|---|---|
| 基础（默认） | `--wrench-source joint_torque_estimated`（默认）+ `--mode pose` | 否 |
| 外置六维力 | 先一次性标定 `--save`，再 bridge `--ft-sensor`，再 `--wrench-source ft_sensor` | 是 |
| SpaceMouse 遥操 | bridge `--spacemouse`（或默认双外设），再 `--mode spacemouse` | 是 |

**外置力传感器安全要求：** 使用 `--wrench-source ft_sensor` 前 **必须** 在
`smrcore_peripherals` 完成一次静态标定并 `--save`。未标定的外力十分危险，可导致
大幅非预期运动。标定保存后，日常只需保持 bridge 推送采样，再运行本示例。

```bash
# 一次性标定
app_peripherals_bridge --robot <ip> --ft-sensor
app_peripherals_ft_sensor_calib --robot-ip <ip> --save

# 日常：推送采样后跑 FDCC
app_peripherals_bridge --robot <ip> --ft-sensor
./build/compliance_fd_cartesian_admittance <ip> --wrench-source ft_sensor
```

刚度 / kp 为源码中的保守常量，请直接改源码调参（无增益 CLI）。

### 适用场景

- 需要根据外力顺应运动的拖动/装配类任务。
- 先从默认关节力矩路径上手；需要更高保真外力或遥操时再接外置 F/T / SpaceMouse。

### 完整源码

```cpp
--8<-- "examples/compliance/fd_cartesian_admittance.cpp"
```
