#!/usr/bin/env python3
"""basics/connect - connection lifecycle: Initialize / IsConnected / Shutdown.

Usage:
    python examples_py/basics/connect.py [--robot-ip <ip>]

    - Pass --robot-ip <ip>, such as 192.168.1.100, to connect to a remote robot.
    - Omit --robot-ip for local simulation.
"""

import argparse
import sys

from rcore_sdk import Robot


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--robot-ip",
        default="",
        help="Robot IP (omit for local simulation)",
    )
    args = parser.parse_args()
    robot_ip = args.robot_ip

    robot = Robot()
    if not robot.Initialize(robot_ip):
        print(f'Initialize failed (ip="{robot_ip}")', file=sys.stderr)
        return 1

    print(f"Initialize succeeded, IsConnected = {robot.IsConnected()}")

    robot.Shutdown()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
