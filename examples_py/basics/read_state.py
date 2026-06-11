#!/usr/bin/env python3
"""basics/read_state - read robot info, state, and motor status.

Usage:
    python examples_py/basics/read_state.py [robot_ip]

Reads the most commonly used fields: robot identity, joint state, TCP pose and
velocity, control mode, and motor hardware status.
"""

import sys

from rcore_sdk import Robot


def fmt(values):
    return " ".join(f"{v:.4f}" for v in values)


def main():
    robot_ip = sys.argv[1] if len(sys.argv) > 1 else ""

    robot = Robot()
    if not robot.Initialize(robot_ip):
        print("Initialize failed", file=sys.stderr)
        return 1

    # Basic robot identity: model, serial number, and the SDK version.
    info = robot.GetRobotInfo()
    print(
        f"Robot model: {info['robot_model']}  "
        f"serial: {info['robot_serial_number']}  sdk: {info['sdk_version']}"
    )

    state = robot.GetState()

    # Joint-space state.
    print(f"Joint positions  [rad]  : {fmt(state.positions)}")
    print(f"Joint velocities [rad/s]: {fmt(state.velocities)}")
    print(f"Joint torques    [N*m]  : {fmt(state.torques)}")

    # Cartesian-space state (tvec / rvec are read-only properties).
    print(f"TCP position    [m]:   {fmt(state.tcp_pose.tvec)}")
    print(f"TCP orientation [rad]: {fmt(state.tcp_pose.rvec)}")

    # TCP velocity v=J*qd [vx,vy,vz,wx,wy,wz], expressed in the Base frame.
    print(f"TCP velocity [m/s,rad/s]: {fmt(state.cartesian_velocity)}")

    print(f"Control mode: {robot.GetControlMode()} (0 = Kinematics)")
    print(f"Timestamp [s]: {state.timestamp:.3f}")

    # Motor hardware status.
    motor = robot.GetMotorStatus()
    print(
        f"motor.enabled={motor.enabled} estop={motor.estop} "
        f"error={motor.error} operational={motor.operational}"
    )

    robot.Shutdown()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
