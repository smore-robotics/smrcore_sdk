#!/usr/bin/env python3
"""config/payload - set, read, and restore the end-effector payload.

Usage:
    python examples_py/config/payload.py [robot_ip]

The payload (mass + center of mass on the flange frame) feeds the dynamics
model. It is process-state only: it affects the running controller and resets to
mass=0 after a restart.

This example saves the payload active at start, applies a demo payload, then
restores the original value before exiting. It never leaves the robot with a
different payload than it found.

The payload dict schema is {"mass": float, "com": [x, y, z]}.

Safety note:
    An incorrect payload degrades dynamics compensation and force control. Use
    the real tool mass; clear it when no tool is mounted.
"""

import sys

from rcore_sdk import Robot


def check(result, label):
    if not result:
        raise RuntimeError(
            f"{label} failed: code={result.error_code} msg={result.error_msg}"
        )


def describe(payload):
    com = payload.get("com", [0.0, 0.0, 0.0])
    return f"mass={payload.get('mass', 0.0):.3f} kg com={[round(c, 3) for c in com]} m"


def main():
    robot_ip = sys.argv[1] if len(sys.argv) > 1 else ""

    robot = Robot()
    if not robot.Initialize(robot_ip):
        print("Initialize failed", file=sys.stderr)
        return 1

    try:
        # 1. Read the currently active payload so we can restore it later.
        before = robot.GetPayload()
        print(f"Current payload (will be restored on exit): {describe(before)}")

        # 2. Set a small example payload (0.5 kg, 4 cm along flange +Z).
        check(robot.SetPayload({"mass": 0.5, "com": [0.0, 0.0, 0.04]}),
              "SetPayload")
        try:
            print(f"After set: {describe(robot.GetPayload())}")
        finally:
            # 3. Always restore the payload active at start, even on failure.
            #    Only clear when the robot had no payload to begin with.
            if before.get("mass", 0.0) > 0.0:
                check(robot.SetPayload(before), "Restore payload")
            else:
                check(robot.ClearPayload(), "Restore payload")
            print(f"Restored original payload: {describe(before)}")
    finally:
        robot.Shutdown()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
