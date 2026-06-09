#!/usr/bin/env python3
"""motion/kinematics - forward and inverse kinematics (no motion).

Usage:
    python examples_py/motion/kinematics.py [robot_ip]

ForwardKinematics maps joint positions to a TCP pose; InverseKinematics maps a
TCP pose back to joint positions. Both return a (Result, value) tuple and do not
move the robot.

Safety note:
    This example only computes kinematics; it does not command any motion.
"""

import sys

from rcore_sdk import Pose, Robot


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
        state = robot.GetState()
        current_jp = state.positions

        # Forward kinematics: current joints -> TCP pose.
        result, fk_pose = robot.ForwardKinematics(current_jp)
        check(result, "ForwardKinematics")
        print(f"FK TCP [m]: {' '.join(f'{v:.4f}' for v in fk_pose.tvec)}")

        # Build a target pose 5 cm above the FK pose, same orientation.
        t = list(fk_pose.tvec)
        t[2] += 0.05
        target = Pose.from_euler(t, fk_pose.rvec)

        # Inverse kinematics: target pose -> joints (solution only, no motion).
        result, ik_jp = robot.InverseKinematics(target)
        check(result, "InverseKinematics")
        print(f"IK joints [rad]: {' '.join(f'{v:.4f}' for v in ik_jp)}")
    finally:
        robot.Shutdown()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
