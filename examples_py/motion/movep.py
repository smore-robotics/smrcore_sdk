#!/usr/bin/env python3
"""motion/movep - Cartesian point motion with MoveP.

Usage:
    python examples_py/motion/movep.py [robot_ip]

MoveP moves the TCP to a target pose using the robot planner. This example reads
the current TCP pose and shifts it +5 cm along the base-frame Z axis.

Safety note:
    Verify the target is safe for your robot and workspace before running.
"""

import sys
import time

from rcore_sdk import Pose, Robot


def check(result, label):
    if not result:
        raise RuntimeError(
            f"{label} failed: code={result.error_code} msg={result.error_msg}"
        )


def offset_pose(pose, dx=0.0, dy=0.0, dz=0.0):
    # Pose is immutable; build a new one with the translation offset applied.
    t = list(pose.tvec)
    t[0] += dx
    t[1] += dy
    t[2] += dz
    return Pose.from_euler(t, pose.rvec)


def main():
    robot_ip = sys.argv[1] if len(sys.argv) > 1 else ""

    robot = Robot()
    if not robot.Initialize(robot_ip):
        print("Initialize failed", file=sys.stderr)
        return 1

    try:
        check(robot.Enable(), "Enable")
        time.sleep(0.5)

        # Cartesian velocity percentage is a scalar (10% here).
        check(robot.SetCartesianVelocityPercentage(0.1),
              "SetCartesianVelocityPercentage")
        print("Cartesian velocity percentage set to 10%")

        target = offset_pose(robot.GetState().tcp_pose, dz=0.05)
        print(f"MoveP target TCP [m]: {' '.join(f'{v:.4f}' for v in target.tvec)}")

        check(robot.MoveP(target), "MoveP")
        print("MoveP completed")
    finally:
        robot.Shutdown()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
