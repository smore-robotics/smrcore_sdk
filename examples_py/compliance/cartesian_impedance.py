#!/usr/bin/env python3
"""compliance/cartesian_impedance - Cartesian impedance control.

Usage:
    python examples_py/compliance/cartesian_impedance.py [robot_ip]

Cartesian impedance is torque-controlled: the TCP behaves like a spring-damper
around an equilibrium pose. SetCartesianImpedanceTarget is a servo-like (~1 kHz)
interface, so the equilibrium must be STREAMED continuously (even to hold it).
This example streams the equilibrium +5 cm along Z and back, then disables.

Parameters are a dict: {"stiffness": [6], "damping": [6]}.

Note:
    Python is not a hard real-time language; the streaming loop here is best
    effort. For strict timing prefer the C++ SDK.

Safety note:
    This is torque control. Keep the e-stop reachable, and lower the stiffness
    for a brand-new hardware bring-up.
"""

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

    impedance_on = False
    try:
        check(robot.Enable(), "Enable")
        time.sleep(0.5)

        # The current TCP pose becomes the equilibrium.
        initial = robot.GetState().tcp_pose
        initial_tvec = list(initial.tvec)
        rvec = list(initial.rvec)

        params = {
            "stiffness": [1000.0, 1000.0, 1000.0, 150.0, 150.0, 150.0],
            "damping": [20.0, 20.0, 20.0, 10.0, 10.0, 10.0],
        }
        check(robot.EnableCartesianImpedance(params),
              "EnableCartesianImpedance")
        impedance_on = True
        print("CartesianImpedance active.")

        def glide(dz_from, dz_to, steps):
            # Stream the equilibrium translation, refreshing the target each step.
            # Move slowly so the joints stay under their velocity limits.
            for i in range(steps + 1):
                s = i / steps
                tvec = list(initial_tvec)
                tvec[2] += dz_from + s * (dz_to - dz_from)
                robot.SetCartesianImpedanceTarget(Pose.from_euler(tvec, rvec))
                time.sleep(0.002)

        print("Moving equilibrium +5 cm along Z and back...")
        glide(0.0, 0.05, 2000)
        glide(0.05, 0.0, 2000)

        # Let the joints settle before disabling (the disable does a static check).
        time.sleep(1.0)
    finally:
        # Always leave torque control before exiting, even on failure.
        if impedance_on:
            disabled = robot.DisableCartesianImpedance()
            print(f"DisableCartesianImpedance: {'ok' if disabled else 'failed'}")
            robot.Disable()
        robot.Shutdown()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
