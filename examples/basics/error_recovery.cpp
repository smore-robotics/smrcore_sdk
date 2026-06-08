// basics/error_recovery - recover from an emergency / safety stop:
//   EStop -> Recover -> ClearError -> Enable, checking MotorStatus / ControlMode
//
// Usage: ./basics_error_recovery [robot_ip]
//
// The full recovery chain has three distinct steps; none of them is optional
// for a real e-stop, safety stop, or collision-detection trip:
//   - Recover():    hardware/motor-side recovery. Releases the e-stop intent,
//                   clears drive/motor faults, and recovers the software safety
//                   stop. It does NOT re-enable the motors.
//   - ClearError(): controller/system-side recovery. Clears the latched errors,
//                   active alarms, and rejected-command events in RobotState.
//   - Enable():     re-enables the motors once the two steps above succeeded.
//
// Safety note: this example deliberately triggers an emergency stop and then
// re-enables the motors. Keep the workspace clear and the physical e-stop
// reachable before running on a real robot.

#include "sdk/robot.hpp"

#include <chrono>
#include <cstdio>
#include <exception>
#include <string>
#include <thread>

namespace
{

void PrintMotorStatus(rcore::sdk::Robot &robot, const char *label)
{
    const auto motor = robot.GetMotorStatus();
    std::printf("[%s] enabled=%s estop=%s error=%s operational=%s\n", label,
                motor.enabled ? "true" : "false",
                motor.estop ? "true" : "false",
                motor.error ? "true" : "false",
                motor.operational ? "true" : "false");
}

} // namespace

int main(int argc, char **argv)
{
    const std::string robot_ip = (argc > 1) ? argv[1] : "";

    try
    {
        rcore::sdk::Robot robot;
        if (!robot.Initialize(robot_ip))
        {
            std::fprintf(stderr, "Initialize failed\n");
            return 1;
        }

        // Start from a known-good state.
        auto initial_enable = robot.Enable();
        if (!initial_enable.IsSuccess())
        {
            std::fprintf(stderr, "Initial Enable failed: code=%u msg=%s\n",
                         initial_enable.GetErrorCode(),
                         initial_enable.GetErrorMsg().c_str());
            robot.Shutdown();
            return 1;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        PrintMotorStatus(robot, "after Enable");

        // Step 1: trigger an emergency stop. Motors drop out of the
        // operational state and latch an error.
        std::printf("\nTriggering EStop...\n");
        auto estopped = robot.EStop();
        if (!estopped.IsSuccess())
        {
            std::fprintf(stderr, "EStop failed: code=%u msg=%s\n",
                         estopped.GetErrorCode(),
                         estopped.GetErrorMsg().c_str());
            robot.Shutdown();
            return 1;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        PrintMotorStatus(robot, "after EStop");

        // Step 2: hardware/motor-side recovery. Recover releases the e-stop
        // intent and clears drive/motor faults, but does NOT re-enable.
        std::printf("\nRecovering hardware (Recover)...\n");
        auto recovered = robot.Recover();
        if (!recovered.IsSuccess())
        {
            std::fprintf(stderr, "Recover failed: code=%u msg=%s\n",
                         recovered.GetErrorCode(),
                         recovered.GetErrorMsg().c_str());
            robot.Shutdown();
            return 1;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        PrintMotorStatus(robot, "after Recover");

        // Step 3: controller/system-side recovery. ClearError clears the
        // latched errors in RobotState; it does NOT re-enable the motors.
        std::printf("\nClearing error...\n");
        auto cleared = robot.ClearError();
        if (!cleared.IsSuccess())
        {
            std::fprintf(stderr, "ClearError failed: code=%u msg=%s\n",
                         cleared.GetErrorCode(),
                         cleared.GetErrorMsg().c_str());
            robot.Shutdown();
            return 1;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        PrintMotorStatus(robot, "after ClearError");

        // Step 4: explicitly re-enable the motors to return to operation.
        std::printf("\nRe-enabling motors...\n");
        auto enabled = robot.Enable();
        if (!enabled.IsSuccess())
        {
            std::fprintf(stderr, "Enable failed: code=%u msg=%s\n",
                         enabled.GetErrorCode(), enabled.GetErrorMsg().c_str());
            robot.Shutdown();
            return 1;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        PrintMotorStatus(robot, "after Enable");

        std::printf("\nControl mode: %d (0 = Kinematics)\n",
                    static_cast<int>(robot.Control().GetControlMode()));
        std::printf("Recovery sequence completed\n");

        robot.Disable();
        robot.Shutdown();
        return 0;
    }
    catch (const std::exception &e)
    {
        std::fprintf(stderr, "Exception: %s\n", e.what());
        return 1;
    }
}
