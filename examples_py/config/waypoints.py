#!/usr/bin/env python3
"""config/waypoints - add, list, and remove named waypoints.

Usage:
    python examples_py/config/waypoints.py [robot_ip]

Named waypoints store joint poses by name for later replay. This example saves
the current pose under a namespaced demo name, lists all waypoints, then removes
only the entry it created. If a waypoint with the demo name already exists it
refuses to run so it never overwrites one of your own waypoints.

Each waypoint is a dict: {"name": str, "joint_positions": [6 floats]}.
"""

import sys

from rcore_sdk import Robot


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

    demo_name = "example_demo_home"

    try:
        # 1. Refuse to run if the demo name already exists.
        existing = robot.GetWaypoints()
        print(f"Existing waypoints ({len(existing)}):")
        for w in existing:
            print(f"  - {w.get('name')}")
            if w.get("name") == demo_name:
                print(f'Waypoint "{demo_name}" already exists; refusing to '
                      f"overwrite it. Remove it first or rename the demo.",
                      file=sys.stderr)
                return 1

        # 2. Save the current joint pose under the demo name.
        state = robot.GetState()
        check(
            robot.AddWaypoint({
                "name": demo_name,
                "joint_positions": list(state.positions),
            }),
            "AddWaypoint",
        )
        print(f'Added waypoint "{demo_name}"')

        # 3. List all waypoints (now including the demo entry).
        all_wp = robot.GetWaypoints()
        print(f"Waypoints ({len(all_wp)}):")
        for w in all_wp:
            print(f"  - {w.get('name')}")

        # 4. Remove only the demo waypoint we created.
        removed = robot.RemoveWaypoint(demo_name)
        print(f'RemoveWaypoint("{demo_name}"): '
              f"{'ok' if removed else 'failed'}")
    finally:
        robot.Shutdown()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
