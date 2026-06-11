#!/usr/bin/env python3
"""motion/kinematics - kinematics queries (no motion).

Usage:
    python examples_py/motion/kinematics.py [robot_ip]

ForwardKinematics maps joint positions to a TCP pose; InverseKinematics maps a
TCP pose back to joint positions. Both return a (Result, value) tuple.
GetJacobian returns the 6x6 TCP geometric Jacobian and IsReachable pre-checks a
Cartesian target; both return a dict carrying the Result fields plus the value.

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

        # TCP geometric Jacobian (6x6, Base frame): rows 0-2 linear velocity,
        # rows 3-5 angular velocity, columns are joints 1..6.
        jac = robot.GetJacobian()
        if jac["success"]:
            row0 = jac["jacobian"][0]
            print(f"Jacobian row 0: {' '.join(f'{v:.4f}' for v in row0)}")

        # Reachability pre-check: cheap test before commanding a motion.
        reach = robot.IsReachable(target)
        if reach["success"]:
            print(f"Target reachable: {reach['reachable']}")
    finally:
        robot.Shutdown()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
