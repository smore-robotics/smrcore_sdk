#!/usr/bin/env python3
"""compliance/fd_cartesian_admittance - force-led Cartesian admittance (FDCC).

Usage:
    python examples_py/compliance/fd_cartesian_admittance.py [robot_ip]
        [--wrench-source joint_torque_estimated|ft_sensor]
        [--mode pose|spacemouse]

FdCartesianAdmittance drives the TCP from an external wrench while tracking a
pose target. Two wrench sources are supported:

  joint_torque_estimated (default)
    Uses joint-torque estimates (no peripherals repository required).
    Recommended: CalibrateEndTorqueSensorZero once with no external load.

  ft_sensor
    Uses an external six-axis F/T sensor. You MUST complete a one-time static
    calibration in smrcore_peripherals before enabling this source. Running
    FDCC on an uncalibrated external wrench is dangerous: wrong force/torque
    can cause large unintended motion.

    One-time calibration (smrcore_peripherals, build with --with-sdk ON):
      app_peripherals_bridge --robot <ip> --ft-sensor
      app_peripherals_ft_sensor_calib --robot-ip <ip> --save
    Daily use after calibration is saved:
      app_peripherals_bridge --robot <ip> --ft-sensor
      python .../fd_cartesian_admittance.py <ip> --wrench-source ft_sensor

Demo modes:
  pose (default)  Hold current TCP, command +5 cm along Z, then return.
  spacemouse      Teleop until Ctrl+C. SpaceMouse samples must be injected by
                  smrcore_peripherals (this script does not open the device):
      app_peripherals_bridge --robot <ip> --spacemouse

Repositories:
  https://github.com/smore-robotics/smrcore_sdk
  https://github.com/smore-robotics/smrcore_peripherals

Safety:
  Start with the conservative stiffness/kp below (edit in source to tune).
  Keep the workspace clear and the e-stop reachable.
"""

from __future__ import annotations

import argparse
import signal
import sys
import time

from rcore_sdk import (
    CartesianJogFrameReference,
    FdCartesianAdmittanceWrenchSourceFtSensor,
    FdCartesianAdmittanceWrenchSourceJointTorqueEstimated,
    Pose,
    Robot,
    TeleopInputSourceManual,
    TeleopInputSourceSpaceMouse,
)

g_running = True


def _on_signal(_signum, _frame):
    global g_running
    g_running = False


def check(result, label):
    if not result:
        raise RuntimeError(
            f"{label} failed: code={result.error_code} msg={result.error_msg}"
        )


def parse_args(argv):
    parser = argparse.ArgumentParser(
        description="Force-led Cartesian admittance (FDCC) example"
    )
    parser.add_argument("robot_ip", nargs="?", default="", help="Robot IP")
    parser.add_argument(
        "--wrench-source",
        choices=("joint_torque_estimated", "ft_sensor"),
        default="joint_torque_estimated",
        help="Wrench input source (default: joint_torque_estimated)",
    )
    parser.add_argument(
        "--mode",
        choices=("pose", "spacemouse"),
        default="pose",
        help="pose=+5 cm Z demo; spacemouse=teleop until Ctrl+C",
    )
    return parser.parse_args(argv)


def main(argv=None):
    args = parse_args(argv if argv is not None else sys.argv[1:])
    signal.signal(signal.SIGINT, _on_signal)
    signal.signal(signal.SIGTERM, _on_signal)

    use_ft = args.wrench_source == "ft_sensor"
    wrench_source = (
        FdCartesianAdmittanceWrenchSourceFtSensor
        if use_ft
        else FdCartesianAdmittanceWrenchSourceJointTorqueEstimated
    )

    robot = Robot()
    if not robot.Initialize(args.robot_ip):
        print("Initialize failed", file=sys.stderr)
        return 1

    ft_on = False
    fd_on = False
    spacemouse_on = False
    try:
        check(robot.Enable(), "Enable")
        time.sleep(0.2)

        print(f"FDCC wrench source: {args.wrench_source}")

        if use_ft:
            # External F/T: require a saved calibration from
            # smrcore_peripherals before enabling. Do not skip this.
            check(robot.EnsureFtSensor(), "EnsureFtSensor")
            ft_on = True
            calib = robot.GetFtCalibration()
            status = calib.get("status", {})
            if not status.get("success"):
                print(
                    "FT calibration unavailable.\n"
                    "DANGER: do not run FDCC with an uncalibrated external "
                    "F/T sensor — incorrect wrench can cause large unintended "
                    "motion.\n"
                    "Calibrate once in smrcore_peripherals, then stream "
                    "samples:\n"
                    "  app_peripherals_bridge --robot <ip> --ft-sensor\n"
                    "  app_peripherals_ft_sensor_calib --robot-ip <ip> --save\n"
                    "Repo: https://github.com/smore-robotics/smrcore_peripherals",
                    file=sys.stderr,
                )
                return 1
            # Tare (zero) the external F/T reading before enable. Optional in
            # your own application — omit this call if you need to preserve the
            # current tare state.
            check(robot.TareFtCalibration(), "TareFtCalibration")
            print("TareFtCalibration done.")
        else:
            print(
                "Tip: for joint_torque_estimated, run "
                "CalibrateEndTorqueSensorZero once with no external load "
                "(optional; enable is not blocked)."
            )

        check(
            robot.SetFdCartesianAdmittanceWrenchSource(wrench_source),
            "SetFdCartesianAdmittanceWrenchSource",
        )

        # Conservative first-run gains. Edit these lists to tune behaviour.
        # EnableFdCartesianAdmittance accepts the full params dict (same as C++).
        check(
            robot.EnableFdCartesianAdmittance({
                "stiffness": [1000.0, 1000.0, 1000.0, 80.0, 80.0, 80.0],
                "kp": [0.0005, 0.0005, 0.0005, 0.005, 0.005, 0.005],
            }),
            "EnableFdCartesianAdmittance",
        )
        fd_on = True

        initial = robot.GetState().tcp_pose

        if args.mode == "spacemouse":
            # SpaceMouse samples must already be flowing from
            # app_peripherals_bridge (--spacemouse or default dual peripherals).
            check(
                robot.SetSpaceMouseParams({
                    "linear_delta_max": [0.0001, 0.0001, 0.0001],
                    "angular_delta_max": [0.0, 0.0, 0.0],
                    "frame": CartesianJogFrameReference,
                }),
                "SetSpaceMouseParams",
            )
            src = robot.SetFdCartesianAdmittanceTeleopSource(
                TeleopInputSourceSpaceMouse
            )
            if not src:
                raise RuntimeError(
                    "SetFdCartesianAdmittanceTeleopSource(SpaceMouse) failed: "
                    f"code={src.error_code} msg={src.error_msg}. "
                    "Start smrcore_peripherals bridge first, e.g. "
                    "app_peripherals_bridge --robot <ip> --spacemouse"
                )
            spacemouse_on = True
            print(
                "FdCartesianAdmittance + SpaceMouse active. Move the "
                "SpaceMouse; Ctrl+C to exit."
            )
            while g_running:
                tcp = robot.GetState().tcp_pose.tvec
                print(
                    f"\rTCP [m]: {tcp[0]:.4f} {tcp[1]:.4f} {tcp[2]:.4f}   ",
                    end="",
                    flush=True,
                )
                time.sleep(0.1)
            print()
        else:
            t = list(initial.tvec)
            t[2] += 0.05
            offset = Pose.from_euler(t, list(initial.rvec))

            check(robot.SetFdCartesianAdmittancePoseTarget(initial),
                  "SetFdCartesianAdmittancePoseTarget")
            print("FdCartesianAdmittance active.")
            time.sleep(0.5)

            def hold_at(label, target):
                check(robot.SetFdCartesianAdmittancePoseTarget(target), label)
                print(label)
                for _ in range(40):
                    if not g_running:
                        break
                    tcp = robot.GetState().tcp_pose.tvec
                    print(
                        f"\rTCP [m]: {tcp[0]:.4f} {tcp[1]:.4f} {tcp[2]:.4f}   ",
                        end="",
                        flush=True,
                    )
                    time.sleep(0.1)
                print()

            hold_at("Target -> +5 cm along Z (TCP should rise)...", offset)
            hold_at("Target -> back to initial pose...", initial)
    finally:
        if spacemouse_on:
            robot.SetFdCartesianAdmittanceTeleopSource(TeleopInputSourceManual)
        if fd_on:
            disabled = robot.DisableFdCartesianAdmittance()
            print(
                "DisableFdCartesianAdmittance: "
                f"{'ok' if disabled else 'failed'}"
            )
        if ft_on:
            robot.ReleaseFtSensor()
        robot.Disable()
        robot.Shutdown()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
