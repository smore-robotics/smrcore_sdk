#!/usr/bin/env python3
"""motion/servop - 1 kHz Cartesian servo streaming with ServoP.

Usage:
    python examples_py/motion/servop.py [robot_ip]

ServoP is the Cartesian-space counterpart of ServoJ: it streams a new TCP pose
target every control cycle (1 kHz) with no trajectory planner. Compute all
loop-invariant data (the base pose, amplitudes) BEFORE the loop; inside the loop
only update the target and sleep.

Note:
    Python is not a hard real-time language. This example shows the streaming
    pattern; for strict 1 kHz timing on real hardware prefer the C++ SDK.

Safety note:
    This example oscillates the TCP by ~1 cm along the base-frame Z axis around
    the current pose. Verify the workspace is clear and start with small
    amplitudes.
"""

import math
import sys
import time

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
        check(robot.Enable(), "Enable")
        time.sleep(0.5)
        check(robot.SetCartesianVelocityPercentage(0.1),
              "SetCartesianVelocityPercentage")
        print("Cartesian velocity percentage set to 10%")

        # ServoP starts from where the robot already is; use the current TCP pose
        # as the servo base. Cache its components as loop-invariant data.
        base = robot.GetState().tcp_pose
        base_tvec = list(base.tvec)
        base_rvec = list(base.rvec)

        amplitude_m = 0.01  # 1 cm
        frequency_hz = 0.5  # one cycle every 2 s
        period_s = 0.001  # 1 kHz
        cycles = 4000  # ~4 s

        print(f"Streaming ServoP at 1 kHz for ~{cycles * period_s:.1f} s...")
        next_t = time.perf_counter()
        for i in range(cycles):
            t = i * period_s
            offset = amplitude_m * math.sin(2.0 * math.pi * frequency_hz * t)

            tvec = list(base_tvec)
            tvec[2] += offset
            robot.ServoP(Pose.from_euler(tvec, base_rvec))

            next_t += period_s
            delay = next_t - time.perf_counter()
            if delay > 0:
                time.sleep(delay)

        print("ServoP streaming finished")
        robot.Disable()
    finally:
        robot.Shutdown()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
