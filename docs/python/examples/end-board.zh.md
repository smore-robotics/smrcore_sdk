# 末端功能板示例（Python）

`robot.EndBoard()` 返回 Python 末端功能板门面，提供与 C++ 一致的公开能力分组：
RAW485、ZX/DH 夹爪 Modbus RTU 便捷接口和两路数字 IO。

## 运行前准备

- 连接兼容的末端功能板，确认供电、接线、设备地址、波特率和设备专用寄存器含义。
- 远程连接时设置 `ROBOT_IP`；服务在本机时将其留空。
- 获取末端功能板门面前，按普通流程初始化 `Robot`，结束时调用 `Shutdown()`。
- 只执行操作的方法返回 `Result`；读取数据的方法返回 `(Result, value)`。使用值前应
  先检查结果。

## io_state

### 功能

读取一次数字 IO 快照，打印 DI 位图、输出状态回显位图和时间戳，同时打印缓存的
485 连接与反馈状态。示例不会改变输出或驱动夹爪。

```bash
ROBOT_IP=192.168.1.100 python3 examples_py/end_board/io_state.py
```

### 完整源码

```python
--8<-- "examples_py/end_board/io_state.py"
```

## 调用方式

只执行操作的方法返回 `Result`：

```python
result = robot.EndBoard().RS485_SetBaudRate(115200)
if not result:
    print(result.error_code, result.error_msg)
```

读取方法返回 `(Result, value)`：

```python
result, di0 = robot.EndBoard().IOGetDigitalInput(0)
if result:
    print(di0)

result, response = robot.EndBoard().RAW485_Transceive(
    bytes([0x01, 0x03, 0x00, 0x00, 0x00, 0x01]),
    append_crc=True,
    timeout_ms=1000,
)
if result:
    print(response.frame, response.rx_status)
```

`EndBoardRaw485Response` 包含 `frame`、`rx_status`、`rx_seq`、`echo_seq`、
本次发送请求的字节 `tx_frame`，以及 `attempted` 和 `response_received` 标志。
通过 `Result` 判断事务是否成功。`EndBoardDigitalIoState` 包含 `di_bits`、
`do_echo_bits` 和 `timestamp`。两者都是不可变数据类。

## 公开 API 分组

| 分组 | 方法 |
|---|---|
| RAW485 与功能板配置 | `RAW485_SendFrame`、`RAW485_Transceive`、`RS485_SetBaudRate`、`RS485_SetFrameSilenceUs`、`RS485_SetFirstByteTimeoutUs`、`Key_SetTiming` |
| 485 反馈 | `GetEndBoard485ModeState` |
| 通用 Modbus RTU | `RTU_WriteSingleRegister`、`RTU_WriteMultipleRegisters`、`RTU_ReadRegisters_03`、`RTU_ReadRegisters_04` |
| ZX 夹爪 | `RTU_ZX_Enable`、`RTU_ZX_SetPosition`、`RTU_ZX_SetSpeed`、`RTU_ZX_SetForce`、`RTU_ZX_Trigger`、`RTU_ZX_ReadPosition`、`RTU_ZX_ReadSpeed`、`RTU_ZX_ReadForce` |
| DH 夹爪 | `RTU_DH_Init`、`RTU_DH_SetForce`、`RTU_DH_SetPosition`、`RTU_DH_SetSpeed`、`RTU_DH_ReadPosition`、`RTU_DH_ReadSpeed`、`RTU_DH_ReadForce` |
| 数字 IO | `IOSetDigitalOutput`、`IOSetDigitalOutputs`、`IOGetDigitalInput`、`IOGetDigitalInputs`、`IOGetDigitalOutputEcho`、`IOGetDigitalIoState` |

参数范围和设备安全注意事项与 [C++ 末端功能板指南](../../examples/end-board.md)一致。

调用 ZX 和 DH 接口时须显式传入设备的 Modbus `slave_id`。
`RTU_ReadRegisters_03` 和 `RTU_ReadRegisters_04` 返回 `(Result, list[int])`。
发送完整 RAW485 帧可调用 `RAW485_SendFrame(frame)`，其 `append_crc` 默认为
`False`。`GetEndBoard485ModeState()` 直接返回缓存的 `EndBoard485ModeState`，
未取得的反馈显示为 NaN。

!!! warning "设备安全"
    发送寄存器写命令前请确认所连接设备的手册。RS485 总线应只有一个控制方；其它
    事务进行时不要更改波特率；夹爪运动范围内应远离人员和工装。
