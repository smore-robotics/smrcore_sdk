# 柔顺控制示例（Python）

## 运行前

柔顺控制示例会使用力/力矩控制。请从保守参数起步、清空工作区、保证急停可触达。
默认的 `fd_cartesian_admittance.py` 使用关节力矩估算力源，**不需要**外设仓库。
外置 F/T 与 SpaceMouse 路径需要
[smrcore_peripherals](https://github.com/smore-robotics/smrcore_peripherals)。

## cartesian_impedance

### 作用

笛卡尔阻抗（力矩）控制。TCP 在平衡位姿附近表现为弹簧阻尼；示例将平衡点沿 Z
流式移动 +5 cm 再移回，然后退出。参数为含 `stiffness` / `damping` 的 dict。

### 适用场景

- 体验保守的笛卡尔阻抗行为。

### 完整源码

```python
--8<-- "examples_py/compliance/cartesian_impedance.py"
```

## fd_cartesian_admittance

### 作用

力主导笛卡尔导纳（FDCC）。默认：

- `--wrench-source joint_torque_estimated`（无需外设）
- `--mode pose`（+5 cm Z 目标演示）

`EnableFdCartesianAdmittance({...})` 接受参数 dict（stiffness / kp / …）。
启用前用 `SetFdCartesianAdmittanceWrenchSource` 选择力源。

进阶：

- `--wrench-source ft_sensor`：**必须**先在
  [smrcore_peripherals](https://github.com/smore-robotics/smrcore_peripherals)
  完成一次标定（`app_peripherals_ft_sensor_calib --save`），再保持
  `app_peripherals_bridge --ft-sensor` 推送采样。未标定外力十分危险。
- `--mode spacemouse`：需要 bridge `--spacemouse`（或默认双外设）注入
  SpaceMouse 采样。

刚度 / kp 请在源码中修改；无增益 CLI。

### 适用场景

- 先用默认关节力矩路径体验 FDCC；需要外置力或遥操时再接 F/T / SpaceMouse。

### 完整源码

```python
--8<-- "examples_py/compliance/fd_cartesian_admittance.py"
```
