#!/usr/bin/env python3
"""motion/movec - Cartesian circular motion with MoveC.

Usage:
    python examples_py/motion/movec.py [robot_ip]

MoveC moves from the current TCP pose through a via pose to a goal pose. This
example builds a small arc near the current TCP by shifting only the position.

Safety note:
    Verify the via and goal poses are safe before running.
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

        start = robot.GetState().tcp_pose
        via = offset_pose(start, dy=0.03, dz=0.03)
        goal = offset_pose(start, dy=0.06)

        print(f"MoveC via TCP  [m]: {' '.join(f'{v:.4f}' for v in via.tvec)}")
        print(f"MoveC goal TCP [m]: {' '.join(f'{v:.4f}' for v in goal.tvec)}")

        check(robot.MoveC(via, goal), "MoveC")
        print("MoveC completed")
    finally:
        robot.Shutdown()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
