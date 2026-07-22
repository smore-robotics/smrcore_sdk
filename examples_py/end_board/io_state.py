"""Read one end-board digital IO snapshot.

Run::

  ROBOT_IP=192.168.1.100 python3 examples_py/end_board/io_state.py

This example is read-only: it does not change digital outputs or command a
gripper.
"""

from __future__ import annotations

import os

from rcore_sdk import Robot

ROBOT_IP = os.environ.get("ROBOT_IP", "")


def main() -> int:
    robot = Robot()
    try:
        if not robot.Initialize(ROBOT_IP):
            print("[FAIL] Initialize")
            return 1

        result, state = robot.EndBoard().IOGetDigitalIoState()
        if not result.success:
            print("[FAIL] IOGetDigitalIoState", result)
            return 1

        print("[OK] IOGetDigitalIoState")
        print(
            "  "
            f"DI bits=0x{state.di_bits:02X}, "
            f"DO echo bits=0x{state.do_echo_bits:02X}, "
            f"timestamp={state.timestamp:.6f}"
        )
        return 0
    finally:
        robot.Shutdown()


if __name__ == "__main__":
    raise SystemExit(main())
