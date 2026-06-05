#!/usr/bin/env python3
"""02_read_state - read robot state and motor status.

Usage:
    python examples_py/02_read_state.py [robot_ip]
"""

from __future__ import annotations

import sys

from rcore_sdk import Robot


def main() -> int:
    robot_ip = sys.argv[1] if len(sys.argv) > 1 else ""

    robot = Robot()
    if not robot.Initialize(robot_ip):
        print("Initialize failed", file=sys.stderr)
        return 1

    state = robot.GetState()
    motor = robot.GetMotorStatus()

    print(
        "Joint positions [rad]: "
        + " ".join(f"{value:.4f}" for value in state.positions)
    )

    x, y, z = state.tcp_pose.tvec
    print(f"TCP position [m]: {x:.4f} {y:.4f} {z:.4f}")
    print(f"Timestamp: {state.timestamp:.6f}")
    print(
        "motor.enabled={} estop={} error={} operational={}".format(
            motor.enabled,
            motor.estop,
            motor.error,
            motor.operational,
        )
    )

    robot.Shutdown()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
