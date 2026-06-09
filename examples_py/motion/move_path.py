#!/usr/bin/env python3
"""motion/move_path - blended Cartesian path with MovePath.

Usage:
    python examples_py/motion/move_path.py [robot_ip]

MovePath runs a sequence of Cartesian waypoints in one motion. Each waypoint is
a dict with:
    - "pose":         6 floats [x, y, z, rx, ry, rz]
    - "mode":         PathWaypointStop (0) or PathWaypointBlend (1)
    - "blend_radius": blend radius in metres (ignored for Stop points)

Safety note:
    This example builds a small square relative to the current TCP pose. Verify
    the workspace is clear before running.
"""

import sys
import time

from rcore_sdk import PathWaypointBlend, PathWaypointStop, Pose, Robot


def check(result, label):
    if not result:
        raise RuntimeError(
            f"{label} failed: code={result.error_code} msg={result.error_msg}"
        )


def offset_pose(pose, dx=0.0, dy=0.0):
    t = list(pose.tvec)
    t[0] += dx
    t[1] += dy
    return Pose.from_euler(t, pose.rvec)


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

        base = robot.GetState().tcp_pose

        # A 5 cm square in the base XY plane: three corners blend, the last stops.
        s = 0.05
        corners = [
            (s, 0.0, PathWaypointBlend),
            (s, s, PathWaypointBlend),
            (0.0, s, PathWaypointBlend),
            (0.0, 0.0, PathWaypointStop),
        ]
        waypoints = [
            {
                "pose": offset_pose(base, dx, dy).to_list(),
                "mode": mode,
                "blend_radius": 0.01,
            }
            for dx, dy, mode in corners
        ]

        print(f"Running MovePath with {len(waypoints)} waypoints...")
        check(robot.MovePath(waypoints), "MovePath")
        print("MovePath completed")
    finally:
        robot.Shutdown()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
