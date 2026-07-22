# 末端功能板示例

末端功能板 API 提供 RAW485 通信、ZX/DH 夹爪的 Modbus RTU 便捷接口，以及两路
数字 IO。通过 `robot.EndBoard()` 获取轻量领域句柄，并且只在对应 `Robot` 的生命周期
内使用。

## 运行前准备

- 连接兼容的末端功能板，确认供电、接线、设备地址、波特率和设备专用寄存器含义。
- 只使用末端功能板的 C++ 程序可以调用 `InitializeEndBoardOnly()`。该模式不会等待
  机器人运动状态，因此不能用 `IsConnected()` 判断它是否就绪。
- 同时使用运动或机器人状态的程序应采用普通 `Initialize()` 生命周期。
- 使用输出值或发送下一条命令前，应检查每次调用返回的 `Result`。

## io_state

### 功能

以末端功能板独立模式连接，并读取一次数字 IO 快照。快照包含 DI 位图、输出状态回显
位图和时间戳。该示例只读，不会改变输出，也不会驱动夹爪。

位 0 和位 1 分别对应通道 0 和通道 1。例如，`di_bits & 0x01` 读取 DI0，
`(do_echo_bits >> 1) & 0x01` 读取 DO1 回显。

### 完整源码

```cpp
--8<-- "examples/end_board/io_state.cpp"
```

## 公开 API 分组

| 分组 | 方法 | 说明 |
|---|---|---|
| RAW485 与功能板配置 | `RAW485_SendFrame`、`RAW485_Transceive`、`RS485_SetBaudRate`、`Key_SetTiming` | 发送完整帧或执行请求/响应事务；配置通信和按键时序 |
| 通用 Modbus RTU | `RTU_WriteSingleRegister`、`RTU_WriteMultipleRegisters`、`RTU_RequestReadHoldingRegisters` | 常用寄存器写入和读取请求 |
| ZX 夹爪 | `RTU_ZX_Enable`、`RTU_ZX_SetPosition`、`RTU_ZX_SetSpeed`、`RTU_ZX_SetForce`、`RTU_ZX_SetAccel`、`RTU_ZX_SetDecel`、`RTU_ZX_Trigger` | ZX 专用便捷命令 |
| DH 夹爪 | `RTU_DH_Init`、`RTU_DH_SetForce`、`RTU_DH_SetPosition`、`RTU_DH_SetSpeed`、`RTU_DH_RequestEnableState`、`RTU_DH_RequestGripState`、`RTU_DH_RequestPosition` | DH 专用命令和状态请求 |
| 数字 IO | `IOSetDigitalOutput`、`IOSetDigitalOutputs`、`IOGetDigitalInput`、`IOGetDigitalInputs`、`IOGetDigitalOutputEcho`、`IOGetDigitalIoState` | 两路输入、两路输出和组合快照 |

C++ 读取接口通过输出参数返回数据：

```cpp
auto end_board = robot.EndBoard();

bool di0 = false;
auto result = end_board.IOGetDigitalInput(0, di0);
if (!result.IsSuccess()) {
    // 处理 result.GetErrorCode() 和 result.GetErrorMsg()。
}
```

`RAW485_Transceive` 通过 `EndBoardRaw485Response` 返回接收字节和功能板状态。只有
收到与本次发送相匹配的回包，并且 `rx_status` 表示成功时，事务才成功。

## 限制与设备语义

| 项目 | 合法范围或限制 |
|---|---|
| RAW485 完整帧 | 1 至 20 字节 |
| RAW485 事务超时 | 1 至 60000 ms |
| RS485 波特率 | 1200 至 2000000；默认 115200 |
| 按键消抖时间 | 1 至 255 ms |
| 按键长按时间 | 100 至 25500 ms，并且是 100 的倍数 |
| Modbus 从站 ID | 1 至 247 |
| 连续写寄存器 | 1 至 5 个寄存器 |
| 读保持寄存器请求 | 1 至 7 个寄存器 |
| ZX 速度和力 | 0 至 100 |
| DH 力 / 位置 / 速度 | 20 至 100 / 0 至 1000 / 1 至 100 |
| 数字 IO 索引 / 输出位图 | 通道 0 或 1 / `0x00` 至 `0x03` |

`RTU_RequestReadHoldingRegisters` 和 DH 状态请求接口只发送读取请求，不直接返回
已解析的设备值。应用需要接收并解析设备专用回包时，请使用 RAW485 请求/响应接口。

!!! warning "设备安全"
    寄存器地址和值的含义由具体设备决定。发送写命令前请确认所连接设备的手册。
    RS485 总线应只有一个控制方；其它事务进行时不要更改波特率；夹爪运动范围内应
    远离人员和工装。
