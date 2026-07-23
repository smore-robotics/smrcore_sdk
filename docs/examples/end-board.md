# End Board Examples

The End Board API provides RAW485 communication, Modbus RTU helpers for ZX and
DH grippers, and two-channel digital IO. Obtain the lightweight domain handle
with `robot.EndBoard()` and use it only while its `Robot` is alive.

## Before Running

- Connect a compatible end board and confirm its power, cabling, device address,
  baud rate, and device-specific register meanings.
- C++ applications that only need the end board can call
  `InitializeEndBoardOnly()`. This mode does not wait for robot motion state, so
  `IsConnected()` is not its readiness check.
- Applications that also use motion or robot state should use the normal
  `Initialize()` lifecycle.
- Check every returned `Result` before using output values or issuing the next
  command.

## io_state

### What It Does

Connects in end-board-only mode and reads one digital IO snapshot. The snapshot
contains the DI bitmap, the output-state echo bitmap, and a timestamp. This
example is read-only and does not change an output or command a gripper.

Bits 0 and 1 correspond to channels 0 and 1. For example, `di_bits & 0x01`
reads DI0 and `(do_echo_bits >> 1) & 0x01` reads the DO1 echo.

### Full Source

```cpp
--8<-- "examples/end_board/io_state.cpp"
```

## Public API Groups

| Group | Methods | Notes |
|---|---|---|
| RAW485 and board settings | `RAW485_SendFrame`, `RAW485_Transceive`, `RS485_SetBaudRate`, `Key_SetTiming` | Send complete frames or request/response transactions; configure communication and key timing |
| Generic Modbus RTU | `RTU_WriteSingleRegister`, `RTU_WriteMultipleRegisters`, `RTU_RequestReadHoldingRegisters` | Common register writes and read requests |
| ZX gripper | `RTU_ZX_Enable`, `RTU_ZX_SetPosition`, `RTU_ZX_SetSpeed`, `RTU_ZX_SetForce`, `RTU_ZX_SetAccel`, `RTU_ZX_SetDecel`, `RTU_ZX_Trigger` | ZX-specific convenience commands |
| DH gripper | `RTU_DH_Init`, `RTU_DH_SetForce`, `RTU_DH_SetPosition`, `RTU_DH_SetSpeed`, `RTU_DH_RequestEnableState`, `RTU_DH_RequestGripState`, `RTU_DH_RequestPosition` | DH-specific commands and status requests |
| Digital IO | `IOSetDigitalOutput`, `IOSetDigitalOutputs`, `IOGetDigitalInput`, `IOGetDigitalInputs`, `IOGetDigitalOutputEcho`, `IOGetDigitalIoState` | Two inputs, two outputs, and a combined snapshot |

Read operations use an output parameter in C++:

```cpp
auto end_board = robot.EndBoard();

bool di0 = false;
auto result = end_board.IOGetDigitalInput(0, di0);
if (!result.IsSuccess()) {
    // Handle result.GetErrorCode() and result.GetErrorMsg().
}
```

`RAW485_Transceive` returns the received bytes and board status in
`EndBoardRaw485Response`. The transaction succeeds only when a matching response
is received and `rx_status` indicates success.

## Limits and Device Semantics

| Item | Valid range or limit |
|---|---|
| RAW485 complete frame | 1 to 20 bytes |
| RAW485 transaction timeout | 1 to 60000 ms |
| RS485 baud rate | 1200 to 2000000; default 115200 |
| Key debounce | 1 to 255 ms |
| Key long press | 100 to 25500 ms and a multiple of 100 |
| Modbus slave ID | 1 to 247 |
| Multiple-register write | 1 to 5 registers |
| Holding-register read request | 1 to 7 registers |
| ZX speed and force | 0 to 100 |
| DH force / position / speed | 20 to 100 / 0 to 1000 / 1 to 100 |
| Digital IO index / output bitmap | channel 0 or 1 / `0x00` to `0x03` |

`RTU_RequestReadHoldingRegisters` and the DH status-request helpers send a read
request; they do not return decoded device values. Use the RAW485 response path
when your application needs to receive and decode a device-specific reply.

!!! warning "Device safety"
    Register addresses and value meanings are device-specific. Confirm the
    connected device manual before sending a write. Use one owner for the RS485
    bus, avoid changing baud rate while another transaction is active, and keep
    gripper motion clear of people and tooling.
