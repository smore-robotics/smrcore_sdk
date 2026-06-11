#!/usr/bin/env python3
"""motion/movej - joint-space motion with MoveJ.

Usage:
    python examples_py/motion/movej.py [robot_ip]

Safety note:
    This example moves to a fixed conservative joint target. Verify the target
    is safe for your robot before running.
"""

import sys
import time

from rcore_sdk import JointPositions, Robot


def check(result, label):
    if not result:
        raise RuntimeError(
            f"{label} failed: code={result.error_code} msg={result.error_msg}"
        )


def main():
    robot_ip = sys.argv[1] if len(sys.argv) > 1 else ""

    robot = Robot()
    if not robot.Initialize(robot_ip):
        print("Initialize failed", file=sys.stderr)
        return 1

    try:
        check(robot.Enable(), "Enable")
        time.sleep(0.5)

        # Joint velocity percentage is a 6-element list (10% here).
        check(robot.SetVelocityPercentage([0.1] * 6), "SetVelocityPercentage")
        print("Velocity percentage set to 10%")

        # Fixed conservative target [rad]. Check before running on real hardware.
        target = JointPositions([0.0, -1.5708, 1.5708, -1.5708, -1.5708, 0.0])
        print(f"MoveJ target [rad]: {' '.join(f'{v:.4f}' for v in target)}")

        # Synchronous mode: block until the motion finishes. The optional
        # velocity_scale (0,1] multiplies the controller's global velocity for
        # this single motion only; 0.5 here halves it. Omit it to use the
        # global velocity unchanged.
        check(robot.MoveJ(target, velocity_scale=0.5), "MoveJ")
        print("MoveJ completed")
    finally:
        robot.Shutdown()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
