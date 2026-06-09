# 运动示例

## 运行前

运动示例会自行使能电机并真实移动机器人，请清空工作区、保证急停可触达。笛卡尔类
示例都从当前 TCP 位姿移动几厘米。每个示例源文件顶部的注释即完整说明。

## movej

### 作用

关节空间运动到一个固定的保守关节目标。

### 适用场景

- 回到已知姿态（如归位）。
- 关节空间点到点运动。

### 完整源码

```cpp
--8<-- "examples/motion/movej.cpp"
```

## movep

### 作用

笛卡尔点到点运动：复制当前 TCP 位姿、沿 Z 偏移 +5cm，用规划器移动过去。

### 适用场景

- 让 TCP 移动到某个笛卡尔目标位姿。
- 在当前位姿基础上做小位移。

### 完整源码

```cpp
--8<-- "examples/motion/movep.cpp"
```

## movel

### 作用

从当前 TCP 沿 Y 走一小段笛卡尔直线（+5cm）。

### 适用场景

- 需要直线轨迹（如贴合、插入）。

### 完整源码

```cpp
--8<-- "examples/motion/movel.cpp"
```

## movec

### 作用

从当前 TCP 经过一个 via 点到 goal 点走一小段圆弧。

### 适用场景

- 需要圆弧/绕行轨迹。

### 完整源码

```cpp
--8<-- "examples/motion/movec.cpp"
```

## move_path

### 作用

由多个路点构成的笛卡尔融合路径，可逐点设置 stop/blend 模式与融合半径。

### 适用场景

- 连续多段轨迹，且希望拐角平滑过渡。

### 完整源码

```cpp
--8<-- "examples/motion/move_path.cpp"
```

## async_motion

### 作用

启动非阻塞运动，轮询任务状态，并演示暂停/继续。

### 适用场景

- 运动过程中需要并行做别的事或监控进度。
- 需要随时暂停/继续/停止。

### 完整源码

```cpp
--8<-- "examples/motion/async_motion.cpp"
```

## motion_api

### 作用

使用 Motion 领域句柄（`robot.Motion()`）配合正/逆运动学，再经句柄移动。

### 适用场景

- 需要 FK/IK 求解。
- 偏好通过领域句柄而非快捷方式调用。

### 完整源码

```cpp
--8<-- "examples/motion/motion_api.cpp"
```

## servoj

### 作用

1kHz 关节空间高频伺服：无规划器，需自行流式发送目标，示例让关节 1 小幅摆动。

### 适用场景

- 外部轨迹/控制器以高频驱动关节。

### 完整源码

```cpp
--8<-- "examples/motion/servoj.cpp"
```

## servop

### 作用

1kHz 笛卡尔空间高频伺服（ServoJ 的对偶），示例让 TCP 绕当前位姿沿 Z 摆动约 1cm。

### 适用场景

- 外部轨迹/控制器以高频驱动 TCP 位姿。

### 完整源码

```cpp
--8<-- "examples/motion/servop.cpp"
```
