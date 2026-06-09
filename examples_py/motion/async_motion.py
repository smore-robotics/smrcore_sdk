#!/usr/bin/env python3
"""motion/async_motion - asynchronous motion with pause / continue / status.

Usage:
    python examples_py/motion/async_motion.py [robot_ip]

An asynchronous Move returns an AsyncResult immediately. While it runs you can
poll the task status and pause / continue / stop the motion.

Safety note:
    This example moves to a fixed conservative joint target. Verify the target
    is safe for your robot before running.
"""

import sys
import time

from rcore_sdk import JointPositions, Robot


def check(result, label):
    if not result:
        raise RuntimeError(
            f"{label} failed: code={result.error_code} msg={result.error_msg}"
        )


STATUS_NAMES = {0: "Pending", 1: "Running", 2: "Paused", 3: "Succeeded",
                4: "Failed"}


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

        target = JointPositions([0.0, -1.5708, 1.5708, -1.5708, -1.5708, 0.0])

        # Start asynchronously: returns an AsyncResult immediately.
        print("Starting async MoveJ...")
        ar = robot.MoveJ(target, asynchronous=True)

        # Poll the task status, and pause briefly part-way through.
        paused_once = False
        for i in range(200):
            status = robot.GetMotionTaskStatus()
            if not status.has_active_task:
                break
            print(f"task {status.task_id}: "
                  f"{STATUS_NAMES.get(status.status, 'Unknown')}")

            if not paused_once and status.status == 1 and i >= 5:
                print("Pausing for 1 s...")
                robot.PauseMotion()
                time.sleep(1.0)
                print("Continuing...")
                robot.ContinueMotion()
                paused_once = True

            if status.status in (3, 4):
                break
            time.sleep(0.1)

        # Wait() blocks until the async motion completes and returns a Result.
        # (IsSuccess() / GetErrorCode() / GetErrorMsg() also block via Wait(),
        # so prefer the Result that Wait() hands back.)
        check(ar.Wait(), "MoveJ")
        print("Async MoveJ completed")
    finally:
        robot.Shutdown()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
