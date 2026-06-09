# 柔顺控制示例（Python）

## 运行前

力控示例使用力矩/力控制。请先用示例给出的保守参数起步，清空工作区，并确保急停
可触达。`fd_cartesian_admittance.py` 还需要一个六维力/力矩传感器以及已保存且生效
的 FT 标定。

## cartesian_impedance

### 作用

笛卡尔阻抗（力矩）控制。TCP 在平衡位姿附近表现为弹簧-阻尼；平衡点被流式发送沿 Z
+5cm 往返，随后退出该模式。参数为带 `stiffness` 和 `damping` 的字典。

### 适用场景

- 体验保守参数下的笛卡尔阻抗行为。

### 完整源码

```python
--8<-- "examples_py/compliance/cartesian_impedance.py"
```

## fd_cartesian_admittance

### 作用

力主导笛卡尔导纳：TCP 由实测六维力驱动，同时跟踪指令位姿。示例确保 FT 传感器、
校验标定、设置保守参数、使能该模式，然后指令一个 +5cm 的 Z 目标并返回。
`EnableFdCartesianAdmittance()` 不接受参数；参数通过
`UpdateFdCartesianAdmittanceParams` 设置。

### 适用场景

- 使用六维力传感器体验力主导笛卡尔导纳。

### 完整源码

```python
--8<-- "examples_py/compliance/fd_cartesian_admittance.py"
```
