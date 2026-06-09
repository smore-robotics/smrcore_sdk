# 运动示例（Python）

## 运行前

运动示例会自行使能电机并真实移动机器人，请清空工作区、保证急停可触达。笛卡尔类
示例都从当前 TCP 位姿移动几厘米。每个示例的 docstring 即完整说明。

`servoj.py` / `servop.py` 在 Python 循环中流式发送目标。Python 不是硬实时语言；
真实硬件上需要严格 1kHz 时序时建议使用 C++ SDK。

## movej

### 作用

关节空间运动到一个固定的保守关节目标。

### 适用场景

- 回到已知姿态（如归位）。
- 关节空间点到点运动。

### 完整源码

```python
--8<-- "examples_py/motion/movej.py"
```

## movep

### 作用

笛卡尔点动：读取当前 TCP 位姿、沿 Z 偏移 +5cm 移动过去。

### 适用场景

- 让 TCP 移动到某个笛卡尔目标位姿。

### 完整源码

```python
--8<-- "examples_py/motion/movep.py"
```

## movel

### 作用

从当前 TCP 沿 Y 走一小段笛卡尔直线（+5cm）。

### 适用场景

- 需要直线轨迹（如贴合、插入）。

### 完整源码

```python
--8<-- "examples_py/motion/movel.py"
```

## movec

### 作用

从当前 TCP 经过一个 via 点到 goal 点走一小段圆弧。

### 适用场景

- 需要圆弧/绕行轨迹。

### 完整源码

```python
--8<-- "examples_py/motion/movec.py"
```

## move_path

### 作用

由多个路点 dict 构成的笛卡尔融合路径，可逐点设置 stop/blend 模式与融合半径。

### 适用场景

- 连续多段轨迹，且希望拐角平滑过渡。

### 完整源码

```python
--8<-- "examples_py/motion/move_path.py"
```

## async_motion

### 作用

启动非阻塞运动，轮询任务状态，并演示暂停/继续。

### 适用场景

- 运动过程中需要并行做别的事或监控进度。
- 需要随时暂停/继续/停止。

### 完整源码

```python
--8<-- "examples_py/motion/async_motion.py"
```

## kinematics

### 作用

直接计算正/逆运动学，不移动机器人。

### 适用场景

- 不发送运动指令的情况下求解 FK/IK。

### 完整源码

```python
--8<-- "examples_py/motion/kinematics.py"
```

## servoj

### 作用

关节空间高频伺服：无规划器，需自行流式发送目标，示例让关节 1 小幅摆动。

### 适用场景

- 外部轨迹/控制器以高频驱动关节。

### 完整源码

```python
--8<-- "examples_py/motion/servoj.py"
```

## servop

### 作用

笛卡尔空间高频伺服（ServoJ 的对偶），示例让 TCP 绕当前位姿沿 Z 摆动约 1cm。

### 适用场景

- 外部轨迹/控制器以高频驱动 TCP 位姿。

### 完整源码

```python
--8<-- "examples_py/motion/servop.py"
```
