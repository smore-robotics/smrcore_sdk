#!/usr/bin/env python3
"""basics/error_recovery - recover from an emergency / safety stop.

Usage:
    python examples_py/basics/error_recovery.py [robot_ip]

The full recovery chain has three distinct steps; none is optional for a real
e-stop, safety stop, or collision-detection trip:
    - Recover():    hardware/motor-side recovery. Releases the e-stop intent and
                    clears drive/motor faults. It does NOT re-enable the motors.
    - ClearError(): controller/system-side recovery. Clears the latched errors.
    - Enable():     re-enables the motors once the two steps above succeeded.

Safety note:
    This example deliberately triggers an emergency stop and then re-enables the
    motors. Keep the workspace clear and the physical e-stop reachable.
"""

import sys
import time

from rcore_sdk import Robot


def check(result, label):
    if not result:
        raise RuntimeError(
            f"{label} failed: code={result.error_code} msg={result.error_msg}"
        )


def print_motor_status(robot, label):
    motor = robot.GetMotorStatus()
    print(
        f"[{label}] enabled={motor.enabled} estop={motor.estop} "
        f"error={motor.error} operational={motor.operational}"
    )


def main():
    robot_ip = sys.argv[1] if len(sys.argv) > 1 else ""

    robot = Robot()
    if not robot.Initialize(robot_ip):
        print("Initialize failed", file=sys.stderr)
        return 1

    try:
        # Start from a known-good state.
        check(robot.Enable(), "Initial Enable")
        time.sleep(0.3)
        print_motor_status(robot, "after Enable")

        # Step 1: trigger an emergency stop.
        print("\nTriggering EStop...")
        check(robot.EStop(), "EStop")
        time.sleep(0.3)
        print_motor_status(robot, "after EStop")

        # Step 2: hardware/motor-side recovery (does NOT re-enable).
        print("\nRecovering hardware (Recover)...")
        check(robot.Recover(), "Recover")
        time.sleep(0.3)
        print_motor_status(robot, "after Recover")

        # Step 3: controller/system-side recovery (does NOT re-enable).
        print("\nClearing error...")
        check(robot.ClearError(), "ClearError")
        time.sleep(0.3)
        print_motor_status(robot, "after ClearError")

        # Step 4: explicitly re-enable the motors.
        print("\nRe-enabling motors...")
        check(robot.Enable(), "Enable")
        time.sleep(0.3)
        print_motor_status(robot, "after Enable")

        print(f"\nControl mode: {robot.GetControlMode()} (0 = Kinematics)")
        print("Recovery sequence completed")

        robot.Disable()
    finally:
        robot.Shutdown()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
