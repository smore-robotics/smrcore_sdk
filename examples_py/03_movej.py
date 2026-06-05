#!/usr/bin/env python3
"""03_movej - joint-space motion with MoveJ.

Usage:
    python examples_py/03_movej.py [robot_ip]

Safety note: this example moves to a fixed conservative joint target. Verify
this target is safe for your robot before running.
"""

from __future__ import annotations

import sys
import time

from rcore_sdk import JointPositions, Robot


def _require_success(result, operation: str) -> None:
    if not result:
        message = result.error_msg or f"{operation} failed"
        raise RuntimeError(f"{operation} failed: code={result.error_code} msg={message}")


def main() -> int:
    robot_ip = sys.argv[1] if len(sys.argv) > 1 else ""

    robot = Robot()
    initialized = False
    try:
        if not robot.Initialize(robot_ip):
            print("Initialize failed", file=sys.stderr)
            return 1
        initialized = True

        _require_success(robot.Enable(), "Enable")
        time.sleep(0.5)

        # Fixed conservative target [rad]. Check and modify this target before
        # running on a real robot.
        target = JointPositions((0.0, -1.5708, 1.5708, -1.5708, -1.5708, 0.0))

        print(
            "MoveJ target [rad]: "
            + " ".join(f"{value:.4f}" for value in target)
        )
        _require_success(robot.MoveJ(target), "MoveJ")
        print("MoveJ completed")
        return 0
    except Exception as exc:
        print(f"Exception: {exc}", file=sys.stderr)
        return 1
    finally:
        if initialized:
            robot.Shutdown()


if __name__ == "__main__":
    raise SystemExit(main())
