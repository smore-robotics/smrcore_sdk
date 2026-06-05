#!/usr/bin/env python3
"""01_connect - connection lifecycle: Initialize / IsConnected / Shutdown.

Usage:
    python examples_py/01_connect.py [robot_ip]

Pass a robot IP, such as 192.168.1.100, to connect to a remote robot. Omit
robot_ip for local simulation.
"""

from __future__ import annotations

import sys

from rcore_sdk import Robot


def main() -> int:
    robot_ip = sys.argv[1] if len(sys.argv) > 1 else ""

    robot = Robot()
    if not robot.Initialize(robot_ip):
        print(f'Initialize failed (ip="{robot_ip}")', file=sys.stderr)
        return 1

    print(f"Initialize succeeded, IsConnected = {robot.IsConnected()}")
    robot.Shutdown()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
