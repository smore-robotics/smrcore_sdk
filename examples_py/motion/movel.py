#!/usr/bin/env python3
"""motion/movel - Cartesian line motion with MoveL.

Usage:
    python examples_py/motion/movel.py [robot_ip]

MoveL moves the TCP in a straight line to the target pose. This example reads
the current TCP pose and shifts it +5 cm along the base-frame Y axis.

Safety note:
    Verify the workspace is clear and the e-stop is reachable before running.
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

        check(robot.SetCartesianVelocityPercentage(0.1),
              "SetCartesianVelocityPercentage")
        print("Cartesian velocity percentage set to 10%")

        initial = robot.GetState().tcp_pose
        target = offset_pose(initial, dy=0.05)
        print(f"Current TCP [m]: {' '.join(f'{v:.4f}' for v in initial.tvec)}")
        print(f"MoveL target TCP [m]: {' '.join(f'{v:.4f}' for v in target.tvec)}")

        check(robot.MoveL(target), "MoveL")
        print("MoveL completed")
    finally:
        robot.Shutdown()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
