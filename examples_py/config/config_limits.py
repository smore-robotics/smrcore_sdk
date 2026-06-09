#!/usr/bin/env python3
"""config/config_limits - read, modify, verify and restore motion limits.

Usage:
    python examples_py/config/config_limits.py [robot_ip]

Reads the current joint velocity limits, applies a small reduction, reads back
to verify, then restores the originals before exiting.
"""

import sys

from rcore_sdk import Robot


def check(result, label):
    if not result:
        raise RuntimeError(
            f"{label} failed: code={result.error_code} msg={result.error_msg}"
        )


def fmt(values):
    return " ".join(f"{v:.4f}" for v in values)


def main():
    robot_ip = sys.argv[1] if len(sys.argv) > 1 else ""

    robot = Robot()
    if not robot.Initialize(robot_ip):
        print("Initialize failed", file=sys.stderr)
        return 1

    try:
        # 1. Read current limits.
        max_vel = robot.GetMaxVelocity()
        vel_pct = robot.GetVelocityPercentage()
        print(f"Current max velocity [rad/s]      : {fmt(max_vel)}")
        print(f"Current velocity percentage [0-1] : {fmt(vel_pct)}")

        # 2. Apply a small reduction (90% of the current velocity percentage).
        check(robot.SetVelocityPercentage([v * 0.9 for v in vel_pct]),
              "SetVelocityPercentage")
        try:
            # 3. Read back to verify.
            print(f"After set, velocity percentage    : "
                  f"{fmt(robot.GetVelocityPercentage())}")

            # Cartesian limits are returned as a dict.
            cart = robot.GetCartesianLimits()
            print(f"Cartesian max linear velocity     : "
                  f"{fmt(cart.get('max_velocity', []))}")
        finally:
            # 4. Always restore the original velocity percentage, even if the
            #    read-back above failed.
            check(robot.SetVelocityPercentage(vel_pct),
                  "Restore velocity percentage")
            print("Restore velocity percentage: ok")
    finally:
        robot.Shutdown()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
