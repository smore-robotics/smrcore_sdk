#!/usr/bin/env python3
"""motion/servoj - 1 kHz joint servo streaming with ServoJ.

Usage:
    python examples_py/motion/servoj.py [robot_ip]

ServoJ streams a new joint target every control cycle (1 kHz). Unlike MoveJ
there is no trajectory planner: you must send a smooth, continuous stream of
targets. Compute all loop-invariant parameters BEFORE the loop; inside the loop
only update the target and sleep.

Note:
    Python is not a hard real-time language. This example shows the streaming
    pattern; for strict 1 kHz timing on real hardware prefer the C++ SDK.

Safety note:
    This example oscillates joint 1 by a few degrees around the home pose. Verify
    the workspace is clear and start with small amplitudes.
"""

import math
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
        check(robot.SetVelocityPercentage([0.1] * 6), "SetVelocityPercentage")
        print("Velocity percentage set to 10%")

        # Move to a known home pose with the planner first.
        home = JointPositions([0.0, -1.5708, 1.5708, -1.5708, -1.5708, 0.0])
        check(robot.MoveJ(home), "MoveJ home")

        # Loop-invariant parameters: set them here, never inside the loop.
        amplitude_rad = 0.05  # ~2.9 deg
        frequency_hz = 0.5  # one cycle every 2 s
        period_s = 0.001  # 1 kHz
        cycles = 4000  # ~4 s
        home_list = home.to_list()

        print(f"Streaming ServoJ at 1 kHz for ~{cycles * period_s:.1f} s...")
        next_t = time.perf_counter()
        for i in range(cycles):
            t = i * period_s
            offset = amplitude_rad * math.sin(2.0 * math.pi * frequency_hz * t)

            target = list(home_list)
            target[0] = home_list[0] + offset
            robot.ServoJ(JointPositions(target))

            next_t += period_s
            delay = next_t - time.perf_counter()
            if delay > 0:
                time.sleep(delay)

        print("ServoJ streaming finished")
        robot.Disable()
    finally:
        robot.Shutdown()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
