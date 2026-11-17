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
contains the DI bitmap, the output-state echo bitmap, and a timestamp. The
example also prints the cached 485 connection and feedback state. It does not
change an output or command a gripper.

Bits 0 and 1 correspond to channels 0 and 1. For example, `di_bits & 0x01`
reads DI0 and `(do_echo_bits >> 1) & 0x01` reads the DO1 echo.

### Full Source

```cpp
--8<-- "examples/end_board/io_state.cpp"
```

## Public API Groups

| Group | Methods | Notes |
|---|---|---|
| RAW485 and board settings | `RAW485_SendFrame`, `WithResponses`, `RS485_SetBaudRate`, `RS485_SetFrameSilenceUs`, `RS485_SetFirstByteTimeoutUs`, `Key_SetTiming` | Synchronous frame exchange and board timing controls |
| 485 feedback | `GetEndBoard485ModeState` | Read cached connection and feedback without polling the device |
| Generic Modbus RTU | `RTU_WriteSingleRegister`, `RTU_WriteMultipleRegisters`, `RTU_ReadRegisters_03`, `RTU_ReadRegisters_04` | Write registers or read decoded holding and input registers |
| ZX gripper | `RTU_ZX_Enable`, `RTU_ZX_SetPosition`, `RTU_ZX_SetSpeed`, `RTU_ZX_SetForce`, `RTU_ZX_Trigger`, `RTU_ZX_ReadPosition`, `RTU_ZX_ReadSpeed`, `RTU_ZX_ReadForce` | Pass the device's Modbus slave ID |
| DH gripper | `RTU_DH_Init`, `RTU_DH_SetForce`, `RTU_DH_SetPosition`, `RTU_DH_SetSpeed`, `RTU_DH_ReadPosition`, `RTU_DH_ReadSpeed`, `RTU_DH_ReadForce` | Pass the device's Modbus slave ID |
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

`RAW485_SendFrame(payload, append_crc)` waits for a matching reply. Pass
`false` for a complete frame with its own CRC, or `true` for a payload that
needs a CRC appended. To capture the transmit and receive bytes, use
`end_board.WithResponses(exchange, timeout_ms)` with an
`EndBoardRaw485Exchange` that outlives the returned handle. Keep one owner
for each exchange record. Check the returned `Result` before
using the response.

## Limits and Device Semantics

| Item | Valid range or limit |
|---|---|
| RAW485 complete frame | 1 to 20 bytes |
| RAW485 payload before appending CRC | 1 to 18 bytes |
| RAW485 transaction timeout | 1 to 60000 ms |
| RS485 baud rate | 1200 to 2000000; default 115200 |
| RS485 frame silence / first-byte wait | 1 to 500000 µs each, default 5000 µs |
| Key debounce | 1 to 255 ms |
| Key long press | 100 to 25500 ms and a multiple of 100 |
| Modbus slave ID | 1 to 247 |
| Multiple-register write | 1 to 5 registers |
| Holding- or input-register read | 1 to 7 registers |
| ZX speed and force | 0 to 100 |
| DH force / position / speed | 20 to 100 / 0 to 1000 / 1 to 100 |
| Digital IO index / output bitmap | channel 0 or 1 / `0x00` to `0x03` |

`RTU_ReadRegisters_03` and `RTU_ReadRegisters_04` validate and decode the
reply. Check the returned result before reading the output vector. ZX/DH
write acknowledgements confirm receipt of the command. Query device state
separately for motion or initialization completion. Cached position is
device feedback, while cached speed and force are the configured values
read back from the device. Unavailable feedback is represented by NaN.

!!! warning "Device safety"
    Register addresses and value meanings are device-specific. Confirm the
    connected device manual before sending a write. Use one owner for the RS485
    bus, avoid changing baud rate while another transaction is active, and keep
    gripper motion clear of people and tooling.
