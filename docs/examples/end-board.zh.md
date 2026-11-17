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
位图和时间戳。示例还打印缓存的 485 连接与反馈状态，不会改变输出或驱动夹爪。

位 0 和位 1 分别对应通道 0 和通道 1。例如，`di_bits & 0x01` 读取 DI0，
`(do_echo_bits >> 1) & 0x01` 读取 DO1 回显。

### 完整源码

```cpp
--8<-- "examples/end_board/io_state.cpp"
```

## 公开 API 分组

| 分组 | 方法 | 说明 |
|---|---|---|
| RAW485 与功能板配置 | `RAW485_SendFrame`、`WithResponses`、`RS485_SetBaudRate`、`RS485_SetFrameSilenceUs`、`RS485_SetFirstByteTimeoutUs`、`Key_SetTiming` | 同步收发及板端时序配置 |
| 485 反馈 | `GetEndBoard485ModeState` | 读取缓存的连接和反馈，不主动查询设备 |
| 通用 Modbus RTU | `RTU_WriteSingleRegister`、`RTU_WriteMultipleRegisters`、`RTU_ReadRegisters_03`、`RTU_ReadRegisters_04` | 写寄存器或读取解析后的保持寄存器和输入寄存器 |
| ZX 夹爪 | `RTU_ZX_Enable`、`RTU_ZX_SetPosition`、`RTU_ZX_SetSpeed`、`RTU_ZX_SetForce`、`RTU_ZX_Trigger`、`RTU_ZX_ReadPosition`、`RTU_ZX_ReadSpeed`、`RTU_ZX_ReadForce` | 显式传入设备的 Modbus 从站 ID |
| DH 夹爪 | `RTU_DH_Init`、`RTU_DH_SetForce`、`RTU_DH_SetPosition`、`RTU_DH_SetSpeed`、`RTU_DH_ReadPosition`、`RTU_DH_ReadSpeed`、`RTU_DH_ReadForce` | 显式传入设备的 Modbus 从站 ID |
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

`RAW485_SendFrame(payload, append_crc)` 会等待匹配的回包。已包含 CRC 的完整帧
传 `false`，需要自动追加 CRC 的载荷传 `true`。如需记录收发字节，使用
`end_board.WithResponses(exchange, timeout_ms)`，并让 `EndBoardRaw485Exchange`
的生命周期覆盖返回的句柄。每条收发记录应由单一调用方使用。读取响应前检查返回的
`Result`。

## 限制与设备语义

| 项目 | 合法范围或限制 |
|---|---|
| RAW485 完整帧 | 1 至 20 字节 |
| RAW485 追加 CRC 前的载荷 | 1 至 18 字节 |
| RAW485 事务超时 | 1 至 60000 ms |
| RS485 波特率 | 1200 至 2000000；默认 115200 |
| RS485 收帧静默时间 / 首字节等待 | 各 1 至 500000 µs，默认 5000 µs |
| 按键消抖时间 | 1 至 255 ms |
| 按键长按时间 | 100 至 25500 ms，并且是 100 的倍数 |
| Modbus 从站 ID | 1 至 247 |
| 连续写寄存器 | 1 至 5 个寄存器 |
| 读保持寄存器或输入寄存器 | 1 至 7 个寄存器 |
| ZX 速度和力 | 0 至 100 |
| DH 力 / 位置 / 速度 | 20 至 100 / 0 至 1000 / 1 至 100 |
| 数字 IO 索引 / 输出位图 | 通道 0 或 1 / `0x00` 至 `0x03` |

`RTU_ReadRegisters_03` 和 `RTU_ReadRegisters_04` 校验并解析回包，使用输出值前
检查返回结果。ZX/DH 写命令的应答表示设备已确认命令。运动或初始化是否完成，
应另行查询设备状态。缓存位置为设备反馈，速度和力为读回的设定值，未取得的反馈
显示为 NaN。

!!! warning "设备安全"
    寄存器地址和值的含义由具体设备决定。发送写命令前请确认所连接设备的手册。
    RS485 总线应只有一个控制方；其它事务进行时不要更改波特率；夹爪运动范围内应
    远离人员和工装。
