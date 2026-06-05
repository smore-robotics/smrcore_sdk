#!/usr/bin/env python3
"""04_movel - Cartesian line motion with MoveL.

Usage:
    python examples_py/04_movel.py [robot_ip]

Safety note: this example moves a short distance from the current TCP pose.
Verify the workspace is clear and emergency stop is reachable before running.
"""

from __future__ import annotations

import sys
import time

from rcore_sdk import Pose, Robot


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

        # Read the current TCP pose, keep the current orientation, and move
        # +5 cm along the base-frame Y axis.
        state = robot.GetState()
        current = state.tcp_pose
        target = Pose(
            (
                current[0],
                current[1] + 0.05,
                current[2],
                current[3],
                current[4],
                current[5],
            )
        )

        print(f"Current TCP [m]: {current.to_list()[:3]}")
        print(f"MoveL target TCP [m]: {target.to_list()[:3]}")
        _require_success(robot.MoveL(target), "MoveL")
        print("MoveL completed")
        return 0
    except Exception as exc:
        print(f"Exception: {exc}", file=sys.stderr)
        return 1
    finally:
        if initialized:
            robot.Shutdown()


if __name__ == "__main__":
    raise SystemExit(main())
