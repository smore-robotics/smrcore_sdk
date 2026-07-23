# End Board Examples (Python)

The Python End Board facade is returned by `robot.EndBoard()`. It exposes the
same public groups as C++: RAW485, Modbus RTU helpers for ZX and DH grippers, and
two-channel digital IO.

## Before Running

- Connect a compatible end board and confirm its power, cabling, device address,
  baud rate, and device-specific register meanings.
- Set `ROBOT_IP` for a remote robot, or leave it empty when the services are
  available locally.
- Initialize the `Robot` normally before obtaining the End Board facade, and
  call `Shutdown()` when finished.
- Calls that only perform an operation return `Result`. Calls that read data
  return `(Result, value)`; check the result before using the value.

## io_state

### What It Does

Reads one digital IO snapshot and prints the DI bitmap, output-state echo bitmap,
and timestamp. This example is read-only and does not change an output or command
a gripper.

```bash
ROBOT_IP=192.168.1.100 python3 examples_py/end_board/io_state.py
```

### Full Source

```python
--8<-- "examples_py/end_board/io_state.py"
```

## Calling Patterns

Operation-only methods return a `Result`:

```python
result = robot.EndBoard().RS485_SetBaudRate(115200)
if not result:
    print(result.error_code, result.error_msg)
```

Read methods return `(Result, value)`:

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

`EndBoardRaw485Response` contains `frame`, `rx_status`, `rx_seq`, and
`echo_seq`. `EndBoardDigitalIoState` contains `di_bits`, `do_echo_bits`, and
`timestamp`. Both are immutable data classes.

## Public API Groups

| Group | Methods |
|---|---|
| RAW485 and board settings | `RAW485_SendFrame`, `RAW485_Transceive`, `RS485_SetBaudRate`, `Key_SetTiming` |
| Generic Modbus RTU | `RTU_WriteSingleRegister`, `RTU_WriteMultipleRegisters`, `RTU_RequestReadHoldingRegisters` |
| ZX gripper | `RTU_ZX_Enable`, `RTU_ZX_SetPosition`, `RTU_ZX_SetSpeed`, `RTU_ZX_SetForce`, `RTU_ZX_SetAccel`, `RTU_ZX_SetDecel`, `RTU_ZX_Trigger` |
| DH gripper | `RTU_DH_Init`, `RTU_DH_SetForce`, `RTU_DH_SetPosition`, `RTU_DH_SetSpeed`, `RTU_DH_RequestEnableState`, `RTU_DH_RequestGripState`, `RTU_DH_RequestPosition` |
| Digital IO | `IOSetDigitalOutput`, `IOSetDigitalOutputs`, `IOGetDigitalInput`, `IOGetDigitalInputs`, `IOGetDigitalOutputEcho`, `IOGetDigitalIoState` |

The parameter ranges and device-safety notes are the same as in the
[C++ End Board guide](../../examples/end-board.md).

`RTU_RequestReadHoldingRegisters` and the DH status-request helpers send a read
request; they do not return decoded device values. Use `RAW485_Transceive` when
your application needs to receive and decode a device-specific reply.

!!! warning "Device safety"
    Confirm the connected device manual before sending register writes. Use one
    owner for the RS485 bus, avoid changing baud rate while another transaction
    is active, and keep gripper motion clear of people and tooling.
