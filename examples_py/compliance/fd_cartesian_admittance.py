#!/usr/bin/env python3
"""compliance/fd_cartesian_admittance - force-led Cartesian admittance.

Usage:
    python examples_py/compliance/fd_cartesian_admittance.py [robot_ip]

Force-led Cartesian admittance drives the TCP from a measured six-axis force and
tracks a commanded pose target. It requires a six-axis force/torque sensor and a
saved, active FT-sensor calibration; run the FT-sensor calibration first.

The flow is:
    EnsureFtSensor -> GetFtCalibration (must be valid)
    -> UpdateFdCartesianAdmittanceParams({...}) -> EnableFdCartesianAdmittance()
    -> SetFdCartesianAdmittancePoseTarget(pose) ...

EnableFdCartesianAdmittance() takes no arguments; parameters are set separately
via UpdateFdCartesianAdmittanceParams (a dict with "stiffness", "kp", ...).

Safety note:
    This mode reacts to external force. Start with conservative stiffness, keep
    the workspace clear, and keep the e-stop reachable.
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

    ft_on = False
    fd_on = False
    try:
        check(robot.Enable(), "Enable")
        time.sleep(0.2)

        # Start the FT-sensor data source and confirm a calibration exists.
        check(robot.EnsureFtSensor(), "EnsureFtSensor")
        ft_on = True
        calib = robot.GetFtCalibration()
        status = calib.get("status", {})
        if not status.get("success"):
            print("FT calibration unavailable; run the FT-sensor calibration "
                  "first.", file=sys.stderr)
            return 1

        # Conservative parameters for a first run.
        check(
            robot.UpdateFdCartesianAdmittanceParams({
                "stiffness": [1000.0, 1000.0, 1000.0, 80.0, 80.0, 80.0],
                "kp": [0.0005, 0.0005, 0.0005, 0.005, 0.005, 0.005],
            }),
            "UpdateFdCartesianAdmittanceParams",
        )
        check(robot.EnableFdCartesianAdmittance(),
              "EnableFdCartesianAdmittance")
        fd_on = True

        # Build the initial and offset targets from the current TCP pose.
        initial = robot.GetState().tcp_pose
        t = list(initial.tvec)
        t[2] += 0.05
        offset = Pose.from_euler(t, list(initial.rvec))

        # Start with the current pose as the target so enabling does not move.
        check(robot.SetFdCartesianAdmittancePoseTarget(initial),
              "SetFdCartesianAdmittancePoseTarget")
        print("FdCartesianAdmittance active.")
        time.sleep(0.5)

        def hold_at(label, target):
            check(robot.SetFdCartesianAdmittancePoseTarget(target), label)
            print(label)
            for _ in range(40):
                tcp = robot.GetState().tcp_pose.tvec
                print(f"\rTCP [m]: {tcp[0]:.4f} {tcp[1]:.4f} {tcp[2]:.4f}   ",
                      end="", flush=True)
                time.sleep(0.1)
            print()

        hold_at("Target -> +5 cm along Z (TCP should rise)...", offset)
        hold_at("Target -> back to initial pose...", initial)
    finally:
        # Always disable the mode and release the FT sensor, even on failure.
        if fd_on:
            disabled = robot.DisableFdCartesianAdmittance()
            print(f"DisableFdCartesianAdmittance: "
                  f"{'ok' if disabled else 'failed'}")
        if ft_on:
            robot.ReleaseFtSensor()
        robot.Disable()
        robot.Shutdown()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
